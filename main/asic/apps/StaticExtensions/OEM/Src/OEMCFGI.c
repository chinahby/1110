/*===========================================================================
                              OEMCFGI.c

  OEM CONFIG CONTROL ROUTINES

  This file contains a reference implementation of a OEM Config
  control interfaces.

        Copyright ? 1999-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/

/*=============================================================================

$Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Src/OEMCFGI.c#142 $
$DateTime: 2008/10/31 07:05:40 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/31/08   phani   Propagating the changes done for BMP build on 6290.
09/26/08   kps     Add support to replace tcxo_srm functions with tcxomgr.
07/03/08   yb      Added CFGI_TOOLKIT_CS_PS_PARALLEL
03/20/08   clm     Fix memory leak.
03/13/08   yc      Add CFGI_LP_SEC, CFGI_LTM_OFF, CFGI_DAYLT items
02/27/08   yc      Correct the setting for AUTO_ANSWER and TTY config items
02/13/08   sam     Remove include for CustomOEMConfigItems.h
02/06/07   sam     Remove CFGI_TTY and add it to CutomOEMConfigItems.
12/03/07   clm     Replace OEMFS funcs with efs funcs for brew 4.0
10/05/07   mjv     Tweaking implementation of CFGI_UNITTEST_ACTIVE
10/01/07   cvs     Remove calls to STRCPY()
09/28/07   clm     Add CFGI_NITZ_NW_INFO_MCC and CFGI_NITZ_NW_INFO_MNC items.
03/26/07   jas     Tweaking implementation of CFGI_UNITTEST_ACTIVE
03/06/07   jas     Removing rfm_ioctl proto, which is in the include file.
02/20/07   jas     Changing RF includes so that rfm_ioctl is prototyped
02/05/07   ypmw    Fix the compiler errors for the changes of sound dev 
                   attach variables
02/01/07   ypmw    Move the sound device attach variable from IConfig to 
                   uisnd_device_info_table
01/22/07   ayk     Support for CFGI_AUTO_ANSWER.
01/04/06   jfc/jas Applying changes for new RFM IOCTL drivers
12/08/06   jas     Adding support for CFGI_FIRMWARE_VERSION (for HDK)
12/07/06   jas     One more fix of prior checkin
12/07/06   jas     Correcting prior checkin; the FTM item was reimplemented
12/07/06   mv/jas  Removing CFGI_FACTORY_TEST_MODE, which is unused.
11/09/06   jas     Reverting prior changes; they are not yet adequate.
11/09/06   jfc/jas Changes for new, common RF driver
10/30/06   jas     Fixing a compile problem on HDK builds
10/20/06   jas     Fixing compile problem for UIM builds
10/19/06   jas     Mainlining uiOne HDK changes
09/27/06   jas     Fix RVCT compiler warnings.
09/14/06   jas     New item for availability of Manual PLMN selection
08/08/06   jas     Fixing a compiler warning
08/02/06   aps     Fix for CDMA parms being looked up in GSM mode
07/12/06   jas     Fixing RVCT compiler warnings.
06/28/06   jas     Correcting featurization to work on 6275 with BREW 3.1.4
06/21/06   cvs     Fix TTY init for 1x targets
06/12/06   jks     Ported IUsrSysClock to new BREW ISYSCLOCK interface
06/07/06   JD      removed CFGI_SMS_CHANNEL_SELECTION and MAX_MSG_PAYLOAD
06/13/06   JD      Added support for CFGI_VOICEPRIVACY
03/31/06   ac      Added support for QIPCALL debug info
02/21/06   cvs     Add support for persistent game mode
01/16/06   jas     Modifications to support RF driver changes
11/21/05   jas     New item for disabling background images
11/15/05   jas     New item for disabling display during calls
11/08/05   rsr/jas Changes to use rfgsm_get_debug_screen_data API to get gsm 
                   debug screen info.
10/21/05   RI      Added plmn_name config item.
10/10/05   PMD     Added Retry Interval; Changed Retry Period Implementation.
                   Updated OEMCFGLIST Version
09/22/05   cvs     Support for Stereo USB Carkit
09/27/05   cvs     Revert previous changes.  They need better featurization.
09/12/05   rsr     Changes to use rfgsm_get_debug_screen_data API to get gsm 
                   debug screen info.
09/08/05   PMD     Used OEMNV_SMS_RETRY_PERIOD for SMS Retry Period.
08/29/05   RI      Enable auto update of user time using MM info
07/27/05   cvs     Code Review cleanups
07/15/05   AT      Added CFGI items display_duration and back_light_intensity.
06/20/05   bmg     Added CFGI item for IntelliCeiver state
06/10/05   cvs     Support for Smart USB Carkit
04/22/05   RI      Fixed OEMPriv_SetItem_CFGI_TTY() to set the tty value correctly. 
04/19/05   sun     Added new Item for Manual PLMN Selection
04/06/05   RI      Support for TTY.
03/23/05   cvs     RVCT compiler fixes
03/02/05   JLG     Add CFGI_SUBSCRIBERID support.
01/12/05   AT      Set the NAM to NAM1 for the ROAM timer.
01/10/05   src     Added featurisation around references to RF GSM variables.
                   (Requires updated RFGSM header and source.)
12/14/04   AT      Fixed issues with call timers.
12/03/04   PMD     Updated the OEMCONFIGLIST_VERSION to 8
11/17/04   SUN     Added new CFGI Item for Beep Length
11/04/04   PMD     Added SMS GCF Flag CFGI Item.
10/30/04   bhas    Debug Screen support for UMTS targets
10/11/04   PMD     Changed path for oemconfig.dat
10/06/04   PMD     Changed Default SMS Segmentation Setting to 8-bit
10/04/04   sun     Removed CFGI Item for CPHS
09/27/04   sun     Added new CFGI Item to support CPHS
09/22/04   cvs     Add Contrast menu, lint fixes
09/17/04   dyc     Obtain RF hw and NV type for MSM6500
08/24/04   Rex     Changed sms retry default value to 30 seconds
08/16/04   NNG     Support for voice debug.
06/23/04   sun     Fixed Compile Error
06/07/04   sun     Added support for LCD Display
05/17/04   ak      Added HDR Debug Screen Related cache
05/13/04   SUN     Fixed Compile Warning
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
#include "target.h"
#include "customer.h"

#include "OEMFeatures.h"
#include "BREWVersion.h"
#include "AEEComdef.h"
#include "OEMConfig.h"
#include "AEEShell.h"
#include "AEE_OEM.h"
#include "AEEStdLib.h"
#include "OEMSVC.h"
#if defined(FEATURE_BMP)
#include "AEEDownloadInfo.h"
#include "AEEFS.h"
#else
#include "AEEDownload.h"
#endif
#ifdef FEATURE_BREW_3_0
#include "AEE_OEMDispatch.h"
#endif

#include "OEMCFGI.h"
#include "OEMNV.h"
#include "fs_errno.h"
#include "fs_public.h"
#if MIN_BREW_VERSION(4,0)
#include "OEMefs.h"
#else
#include "OEMFS.h"
#endif //MIN_BREW_VERSION(4,0)
#include "snd.h"
#include "hs.h"
#include "mobile.h"
#include "adc.h"

#ifdef FEATURE_ICM
#ifdef FEATURE_ICM
#include "AEECM.h"
#else
#include "AEETelephone.h"
#include "AEETelDef.h"
#endif
#endif //FEATURE_ICM


#include "rfm.h" /* RF Driver interface */ 
#ifndef FEATURE_RFM_IOCTL
#if defined (T_MSM6500) || defined (PLATFORM_LTK)
#include "rfi.h"
#include "rf.h"
#include "rfcom.h"
#endif
#ifdef FEATURE_INTELLICEIVER
#include "rf_intelliceiver.h"
#endif
#endif /* FEATURE_RFM_IOCTL */

#include "therm.h"

#if defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
#include "db.h"
#endif

#include "cm.h"
#include "disp.h"

#ifdef FEATURE_HDR
#error code not present
#endif

#ifdef FEATURE_QIPCALL
#include "qipcalldbg.h"
#endif

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */


#if defined(FEATURE_WCDMA)
#error code not present
#endif // FEATURE_WCDMA


#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif

#ifdef CUST_EDITION
#include "Appscommon.h"
#include "AEECallList.h" 
#include "wms.h"
#include "AEEAlarm.h"
#ifdef FEATURE_KEYGUARD
#include "OEMKeyguard.h"
#endif
#ifdef FEATURE_ICM
#include "AEECM.h"
#else
#include "AEETelephone.h"
#include "AEETelDef.h"
#endif

#ifdef FEATURE_SUPPORT_BT_APP
#ifndef WIN32
#include "bt_ui_int.h"
#endif
#endif
#ifdef FEATURE_LCD_TOUCH_ENABLE
#include "touchpad.h"
#endif
#ifdef FEATURE_RANDOM_MENU_REND
#include "DisplayRendUtils.h"
#endif
#include "tcxomgr.h"
#endif // CUST_EDITION

#ifdef FEATURE_OEMOMH
#include "AEERUIM.h"
#endif

#ifdef FEATURE_USES_MMS
#include "WMSMMS.h"
#endif

#ifndef WIN32
#include "uim.h"
#include "mobile.h"             // for ver_modelname
#include "mccdma.h"  //for use of cdma_bs_type
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
#ifdef FEATURE_PEKTEST
#define OEMCONFIGLIST_VERSION ( (uint16) 0x0010 )
#else
#define OEMCONFIGLIST_VERSION ( (uint16) 0x000F )
#endif

////
// The EFS file that stores the OEM configuration.
#if MIN_BREW_VERSION(3,0)
#define OEMCONFIGLIST_FILE (AEEFS_SYS_DIR"priv/oemconfig.dat")
#else
#define OEMCONFIGLIST_FILE ("brew/oemconfig.dat")
#endif
#define MAX_URL_LEN                    256      // URL最大允许长度

#define FMMIN 0		//Add By zzg 2012_02_18
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
typedef struct
{
   boolean                         present;
   byte                            msb;
   byte                            lsb;
} OEMConfigIndex;

/* Forward CUG info */
typedef struct
{
   boolean                         present;
   OEMConfigIndex                  cug_index;
   boolean                         suppress_pref_cug;
   boolean                         suppress_outgoing_access;
} OEMConfigCugInfo;
/* BREW Download Information */

#define OEMCFG_DL_AKEY_SIZE           8
#define OEMCFG_DL_BKEY_SIZE           16
#define OEMCFG_DL_SERVER_LEN          64

typedef struct {
   uint32   dwCID;         // Carrier ID
   uint32   dwPID;         // Platform ID
   uint8    bBKey[OEMCFG_DL_BKEY_SIZE+1];    // BKey
   uint8    bAKey[OEMCFG_DL_AKEY_SIZE+1];      // AKey
   char     szServer[OEMCFG_DL_SERVER_LEN];  // DL Server
   uint16   wFlags;        // DL Flags
   uint16   wAuth;         // Auth policy
   uint16   wPrivP;        // Privacy policy
} OEMConfigDownloadInfo;
#ifdef CUST_EDITION
#ifdef FEATRUE_KEY_PAD_CTL
#ifdef FEATURE_CARRIER_ISRAEL_PELEPHONE
    #define KEY_PDA_CTL_FROM_TIME 57600000
    #define KEY_PDA_CTL_TO_TIME      28800000
#else
    #define KEY_PDA_CTL_FROM_TIME 59400000
    #define KEY_PDA_CTL_TO_TIME      32400000
#endif
#endif
#define   IP_STRING   "0.0.0.0"

#define IMSI_MIN1_S1_ZERO 0xF9EBE7L	//Add By zzg 2012_02_18


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

#elif defined(FEATURE_VERSION_W515)

// W515 PlatformID
#define DEFAULT_BREW_CARRIER_ID     27
#define DEFAULT_BREW_PLATFORM_ID    805000
#define DEFAULT_BREW_USERNAME       ""
#define DEFAULT_BREW_PASSWORD       ""
#define DEFAULT_BREW_SERVER         "oemdemo.qualcomm.com"
#define DEFAULT_BREW_DOWNLOAD_FLG   (DIF_TEST_ALLOWED | DIF_SID_VALIDATE_ALL | DIF_RUIM_DEL_OVERRIDE | DIF_MIN_FOR_SID)
#define DEFAULT_BREW_APOLICY        APOLICY_SID
#define DEFAULT_BREW_PPOLICY        PPOLICY_BREW_OR_CARRIER

#elif defined(FEATURE_VERSION_W203)

// W203 PlatformID
#define DEFAULT_BREW_CARRIER_ID     27
#define DEFAULT_BREW_PLATFORM_ID    805001
#define DEFAULT_BREW_USERNAME       ""
#define DEFAULT_BREW_PASSWORD       ""
#define DEFAULT_BREW_SERVER         "oemdemo.qualcomm.com"
#define DEFAULT_BREW_DOWNLOAD_FLG   (DIF_TEST_ALLOWED | DIF_SID_VALIDATE_ALL | DIF_RUIM_DEL_OVERRIDE | DIF_MIN_FOR_SID)
#define DEFAULT_BREW_APOLICY        APOLICY_SID
#define DEFAULT_BREW_PPOLICY        PPOLICY_BREW_OR_CARRIER
#elif defined(FEATURE_VERSION_W208S)

#define DEFAULT_BREW_CARRIER_ID     62
#define DEFAULT_BREW_PLATFORM_ID    805002
#define DEFAULT_BREW_USERNAME       ""
#define DEFAULT_BREW_PASSWORD       ""
#define DEFAULT_BREW_SERVER         "brewpostpago.movilnet.com.ve"
#define DEFAULT_BREW_DOWNLOAD_FLG   (DIF_TEST_ALLOWED | DIF_SID_VALIDATE_ALL | DIF_MIN_FOR_SID)
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
#endif //#ifdef CUST_EDITION
////
// Configuration items managed by the OEM layer.
//
// NOTE: Whenever a change is made to an existing memory ofthis structure,
//       OEMCONFIGLIST_VERSION must be incremented.   It does not need to be
//       incremented after adding a new item to the end of the structure.
//
typedef struct
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
   byte		 	 ppp_auth_enabled;			   // CFGI_PPP_AUTH
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
   word          sms_retry_interval;           // CFGI_SMS_RETRY_INTERVAL
   boolean       sms_gcf_flag;                 // CFGI_SMS_GCF_FLAG

   // CFGI_VOICEMAIL_NUMBER
   AECHAR        voicemail_number[OEMNV_VOICEMAIL_MAXLEN];

   // CFGI_VOICEMAIL_NUMBER_CDMA
   AECHAR        voicemail_number_cdma[OEMNV_VOICEMAIL_MAXLEN];

   // CFGI_VOICEMAIL_NUMBER_GW
   AECHAR        voicemail_number_gw[OEMNV_VOICEMAIL_MAXLEN];

   dword         recent_mt_call_time;          // CFGI_RECENT_MT_CALL_TIMER
   dword         recent_mo_call_time;          // CFGI_RECENT_MO_TIMER

   byte          minute_alert;                 // CFGI_MINUTE_ALERT
#ifdef CUST_EDITION
   byte          any_key_answer;               // CFGI_ANYKEY_ANSWER
#else
   boolean       any_key_answer;               // CFGI_ANYKEY_ANSWER
#endif
   byte          headset_autoanswer;           // CFGI_HEADSET_AUTOANSWER
   byte          time_format;                  // CFGI_TIME_FORMAT
   uint32        device_mask;                  // CFGI_HEADSET|CFGI_STEREO_HEADSET|
                                               // CFGI_USB_HFK_ATTACHED|CFGI_EXT_PWR | 
                                               // CFGI_STEREO_USB_HFK_ATTACHED

   OEMConfigCugInfo  cug;                         // CFGI_CUG
    boolean           vr;                         // CFGI_VR
   OEMConfigDownloadInfo  download_info;          // BREW Download information
   char szSubscriberID[DEFAULT_SUBSCRIBERID_LEN]; // CFGI_SUBSCRIBERID
   boolean       disable_in_call_disp;            // CFGI_DISABLE_IN_CALL_DISP
   boolean       disable_bg_image;              // CFGI_DISABLE_BG_IMAGE
   boolean       manual_plmn_sel_allowed;      // CFGI_MANUAL_PLMN_SEL_ALLOWED
#ifdef CUST_EDITION
   uint32        shake_mask;   //CFGI_SHAKE_MUSIC_CHECK|CFGI_SHAKE_FM_RADIO_CHECK|CFGI_SHAKE_WALLPAPER_CHECK|
                              //CFGI_SHAKE_SNOOZE_ALARM_CHECK|CFGI_SHAKE_VIDEO_CHECK|CFGI_PEDOMETER_CHECK
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
#if defined(FEATURE_VERSION_W317A) ||defined(FEATURE_VERSION_C316)   
   boolean		 b_mediagallery_lock;			//CFGI_MEDIAGALLERY_LOCK_CHECK
#endif    
#ifdef FEATURE_VERSION_C316
   boolean       b_mobiletracker_lock;	   //CFGI_MOBILETRACKER_LOCK_CHECK,
   boolean       b_multimedia_lock;	     // CFGI_MULTIMEDIA_LOCK_CHECK,
   uint16        mobiletracker_password;         // CFGI_MOBILETRACKER_PASSWORD,        //type = uint16
   boolean       mobiletracker_password_check;   // CFGI_MOBILETRACKER_PASSWORD_CHECK,  //type = boolean
   AECHAR        mobile_tracker_number_2[OEMNV_LOCKMUM_MAXLEN]; 
   AECHAR        mobile_tracker_number_3[OEMNV_LOCKMUM_MAXLEN]; 
   AECHAR        mobile_tracker_content[OEMNV_LOCKMUM_MAXLEN*6];
   boolean       b_mobile_tracker_back;    //CFGI_MOBILE_TRACKER_BACK
#endif
   byte          b_key_lock;                    //CFGI_KEY_LOCK_CHECK
   boolean       lock_ruim;  
   AEEConfigSIDNIDPairType lock_mccmnc[OEMNV_LOCK_MCCMNC_ARRSIZE];  
   byte          desktop_theme;                //CFGI_DESKTOP_THEME
#ifdef FEATURE_SCREEN_SAVE
   char          s_screensave_type[AEE_MAX_FILE_NAME/*FILESPECLEN*/];//CFGI_SCREENSAVE_TYPE 
#endif
#ifdef FEATURE_USES_MMS
   CFG_MMsInfo   mms_setting;				//CFGI_MMS_SETTING
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
   char     UNCONDITIONAL_ENABLE[FEATURE_CODE_MAX_LENTH]; //CFGI_CALLFORWARD_UNCONDITIONAL_ENABLE
   char     UNCONDITIONAL_DISABLE[FEATURE_CODE_MAX_LENTH];//CFGI_CALLFORWARD_UNCONDITIONAL_DISABLE
   char     UNREACHABLE_ENABLE[FEATURE_CODE_MAX_LENTH];    //CFGI_CALLFORWARD_UNREACHABLE_ENABLE
   char     UNREACHABLE_DISABLE[FEATURE_CODE_MAX_LENTH];   //CFGI_CALLFORWARD_UNREACHABLE_DISABLE
   char     DISABLE_ALL[FEATURE_CODE_MAX_LENTH];      //CFGI_CALLFORWARD_DISABLE_ALL
   char     WAIT_ENABLE[FEATURE_CODE_MAX_LENTH];      //CFGI_CALLFORWARD_WAIT_ENABLE
   char     WAIT_DISABLE[FEATURE_CODE_MAX_LENTH];     //CFGI_CALLFORWARD_WAIT_DISABLE
   char     VOICEMAIL_ENABLE[FEATURE_CODE_MAX_LENTH]; //CFGI_CALLFORWARD_VOICEMAIL_ENABLE
   //输入法设置   
   byte     input_mode;                                //CFGI_INPUTMODE   
   boolean m_fm_background;                            /*CFGI_FM_BACKGROUND*/
#ifdef FEATURE_VERSION_C337   
   boolean	m_defaultcont;							/*CFGI_DEFAULTCONT Add by zzg 2012_11_14*/
#endif
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
#ifdef FEATURE_CAMERA_MULTI_SENSOR
   byte camera_multi;       //CFGI_CAMERA_MULTI
   byte camera_flash;       //CFGT_CAMERA_FLASH
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
   boolean missed_call_icon;                            /* CFGI_MISSED_CALL_ICON    */
#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add for rend
   byte m_nrendstate;                      //CFGI_REND_STATE
#endif
#ifdef FEATURE_LCD_TOUCH_ENABLE
   pen_cal_type m_pencal_data;
#endif
#endif //#ifdef CUST_EDITION
   
   byte   brewsetings_usename[MAS_BREWSETINT_STRING];
   byte   brewsetings_password[MAS_BREWSETINT_STRING];
   boolean brew_laguagefalgs;//uint16
#ifdef FEATURE_ANALOG_TV
   uint32 tv_or_camera;
   CFG_TvSetting tv_set;
#endif

   //Add By zzg 2010_11_22
#ifdef FEATURE_APP_BLUETOOTH
   boolean bt_status;
#endif
   boolean flashlight_status;
   boolean wmswriteend_status;
   boolean FMPlayMode_status;
   byte    Idle_datetime_mode;   //add by yangceai 2011-04-13
   boolean    Camera_Photo_mode;   // add by pyuangui 20110516
   uint8	otksl_times;	//Add By zzg 2012_01_18
   //Add End
   #ifdef FEATURE_LONG_NETLOCK
   boolean netlock_flg;
   #endif
#ifdef FEATURE_USES_MMS   
   char  s_MMSImage[AEE_MAX_FILE_NAME];     
   char  s_MMSSOUND[AEE_MAX_FILE_NAME];   
   char  s_MMSVIDEO[AEE_MAX_FILE_NAME];
   MMSData   MMSDraftDataInfo[MAX_MMS_STORED];   //CFGI_MMSDraftDATA_INFO
   uint8 mmsDraftCount;   
   MMSData   MMSOutDataInfo[MAX_MMS_STORED];   //CFGI_MMSOUTDATA_INFO
   MMSData   MMSInDataInfo[MAX_MMS_STORED];   //CFGI_MMSINDATA_INFO
   uint8 mmsOutCount;//CFGI_MMS_OUTCOUNT
   uint8 mmsInCount;//CFGI_MMS_INCOUNT
   boolean mmsNotify;
   int8 mmsDeliverReport;
   int8 mmsReadReply;
   int8 mmsReportAllowed;
   int8 mmsSenderVisibility;
   char  s_MMSServerAddress[MAX_URL_LEN];     
   char  s_MMSUserName[MAS_BREWSETINT_STRING];   
   char  s_MMSPassword[MAS_BREWSETINT_STRING];   
   char  s_MMSProxy[MAS_BREWSETINT_STRING];  //CFGI_MMS_PROXY 
   char  s_MMSPort[MAS_BREWSETINT_STRING];   //CFGI_MMS_PORT  
#endif
#ifdef FEATURE_VERSION_W208S
   uint8        sms_restrict_receive_total;  
   sms_restrict_recive_info   sms_restrict_recive[MAX_SMS_RESTRICT];
#endif
#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)
   AECHAR    mobile_tracker_number[OEMNV_LOCKMUM_MAXLEN]; 
   AECHAR    mobile_tracker_imsi[OEMNV_LOCKIMSI_MAXLEN];       //CFGI_MOBILE_TRACKER_IMSI
   boolean	 mobile_tracker_setnumb ;     //CFGI_MOBILE_TRACKER_SETNUM
   boolean   sms_tarcker_b;              //CFGI_SMS_TRACKER_SEND_B
   uint16    sms_tarcker_time_uint;      //CFGI_SMS_TRACKER_TIME
   char      sms_tracker_number[OEMNV_LOCKMUM_MAXLEN];       //CFGI_SMS_TRACKER_NUMBER
#endif
#ifdef FEATURE_VERSION_C337
   char    mizone_num[MAS_BREWSETINT_STRING];
   char    mizone_smsinfo[MAS_BREWSETINT_STRING];   
#endif
#if defined(FEATURE_VERSION_C316) || defined(FEATURE_VERSION_W317A)
   boolean  autocallrecord;                                 //CFGI_AUTOCALLRECORD Add by pyuangui 20121231 
#endif
#ifdef FEATURE_VERSION_C316   
   boolean	m_onekey_lock_keypad;							/*CFGI_ONEKEY_LOCK_KEYPAD Add by xuhui 2012/12/24*/
#endif
} OEMConfigListType;


  /* International Mobile Equipment Identity */
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
#else
#define OEMCFG_IMEI_SIZE 10
#endif

boolean b_first = FALSE;


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
  byte  ringer_level;                      /* CFGI_RINGER_VOL */
  byte  handset_ear_level;               /* CFGI_EAR_VOL */
  byte  beep_level;                  /* CFGI_BEEP_VOL */
  byte  ringer_type;                  /* CFGI_RINGER_TYPE */
  byte  key_sound;            /* CFGI_KEYBEEP_SOUND */


  ////////////////////////////////////////////////////////////////
  // Display
  ////////////////////////////////////////////////////////////////
  byte contrast;          /* CFGI_CONTRAST_LVL */
  byte backlight;         /* CFGI_BACK_LIGHT */
  byte language;        /* CFGI_LANGUAGE_SELECTION */
  AECHAR banner[NV_MAX_LTRS+1]; /* CFGI_BANNER */
  byte menu_format;       /* CFGI_MENU_TYPE */

  ////////////////////////////////////////////////////////////////
  // Security
  ////////////////////////////////////////////////////////////////
  byte          lockcode[NV_LOCK_CODE_SIZE];  // CFGI_LOCKCODE

   ////////////////////////////////////////////////////////////////
   // Toolkit
   ////////////////////////////////////////////////////////////////
#ifdef FEATURE_GSTK_CS_PS_PARALLEL
  boolean toolkit_cs_ps_parallel;      /* CFGI_TOOLKIT_CS_PS_PARALLEL */
#endif /* FEATURE_GSTK_CS_PS_PARALLEL */

  ////////////////////////////////////////////////////////////////
  // ETC
  ////////////////////////////////////////////////////////////////
  dword   all_call_time;        /* CFGI_ALL_CALL_TIMER */
  byte    backlight_hfk;        /* CFGI_EXTPWR_BK_LIGHT */
  byte    svc_alert;            /* CFGI_SVC_ALERT */
  boolean amr;                  /*CFGI_AMR*/
  boolean cdma_rx_diversity_ctrl; /*CFGI_CDMA_RX_DIVERSITY_CTL*/
  boolean hdr_rx_diversity_ctrl;  /*HDR_CDMA_RX_DIVERSITY_CTL*/
  OEMTTY tty;
  boolean game_mode;            /*CFGI_GAME_MODE*/
  boolean game_mode_persist;    /*CFGI_GAME_MODE_PERSIST*/

  ////////////////////////////////////////////////////////////////
  // Data Call Counters
  ////////////////////////////////////////////////////////////////
  uint32 last_rx_data_count;    /* CFGI_LAST_RX_DATA_COUNT */
  uint32 last_tx_data_count;    /* CFGI_LAST_TX_DATA_COUNT */
  uint32 total_rx_data_count;   /* CFGI_TOTAL_RX_DATA_COUNT */
  uint32 total_tx_data_count;   /* CFGI_TOTAL_TX_DATA_COUNT */
  uint32 beep_length;           /* CFGI_BEEP_LENGTH*/
  boolean manual_plmn_sel;      /* CFGI_MANUAL_PLMN_SEL */
  OEMCCBS ccbs_sel;
  uint32  display_duration;      /* CFGI_LCD_TIMER */
  uint8   backlight_intensity;   /* CFGI_BACK_LIGHT_INTENSITY */
#if defined(FEATURE_USRSYSCLOCK) || defined(FEATURE_ISYSCLOCK)
  boolean auto_time_enable;      /* CFGI_ENABLE_NETWORK_TIME */
  AECHAR  plmn_name[NV_MAX_NW_NAME_CHARS + 1]; /* CFGI_NITZ_NW_INFO */
  uint32  mcc; /* CFGI_NITZ_NW_INFO_MCC */
  uint32  mnc; /* CFGI_NITZ_NW_INFO_MNC */
#endif 
  uint8 voice_priv;            /* CFGI_VOICEPRIVACY */
  
#ifdef CUST_EDITION
  byte set_time_format;                /* CFGI_TIME_FORMAT*/
#ifdef FEATURE_TIME_DATA_SETTING  
  byte set_date_format;                /* CFGI_DATE_FORMAT*/
#endif
  byte lock;                           /*CFGI_PHONE_PASSWORD_CHECK*/
  nv_auto_redial_type auto_redial;     /*CFGI_AUTO_REDIAL*/

  keyToneLength key_tone_length;       /* CFGI_KEYTONE_LENGTH*/
#endif //#ifdef CUST_EDITION
} NVConfigListType;

////
// Configuration items managed by the NV subsystem.
#ifdef FEATURE_HDR
#error code not present
#endif


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

/*===========================================================================

                     STATIC FUNCTION PROTOTYPES

===========================================================================*/


// Number-classification callback functions
static void OEMPriv_WriteOEMConfigList(void);
static void OEMPriv_ReadOEMConfigList(void);

#ifdef FEATURE_UIM_RUN_TIME_ENABLE
static int OEMPriv_GetItem_CFGI_RTRE_CONFIGURATION(void *pBuff);
static int OEMPriv_SetItem_CFGI_RTRE_CONFIGURATION(void *pBuff);
#endif

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
static int OEMPriv_GetItem_CFGI_GAME_MODE(void *pBuff);
static int OEMPriv_SetItem_CFGI_GAME_MODE(void *pBuff);
static int OEMPriv_GetItem_CFGI_GAME_MODE_PERSIST(void *pBuff);
static int OEMPriv_SetItem_CFGI_GAME_MODE_PERSIST(void *pBuff);
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
static int OEMPriv_GetItem_CFGI_BACK_LIGHT_INTENSITY(void *pBuff);
static int OEMPriv_SetItem_CFGI_BACK_LIGHT_INTENSITY(void *pBuff);
static int OEMPriv_GetItem_CFGI_LANGUAGE_SELECTION(void *pBuff);
static int OEMPriv_SetItem_CFGI_LANGUAGE_SELECTION(void *pBuff);
static int OEMPriv_GetItem_CFGI_MENU_FORMAT(void *pBuff);
static int OEMPriv_SetItem_CFGI_MENU_FORMAT(void *pBuff);
static int OEMPriv_GetItem_CFGI_TIME_FORMAT(void *pBuff);
static int OEMPriv_SetItem_CFGI_TIME_FORMAT(void *pBuff);
static int OEMPriv_GetItem_CFGI_MANUAL_PLMN_SEL(void *pBuff);
static int OEMPriv_SetItem_CFGI_MANUAL_PLMN_SEL(void *pBuff);
#ifdef FEATURE_GSTK_CS_PS_PARALLEL
static int OEMPriv_GetItem_CFGI_TOOLKIT_CS_PS_PARALLEL(void *pBuff);
static int OEMPriv_SetItem_CFGI_TOOLKIT_CS_PS_PARALLEL(void *pBuff);
#endif /* FEATURE_GSTK_CS_PS_PARALLEL */
static int OEMPriv_GetItem_CFGI_IMEI(void *pBuff);
static int OEMPriv_SetItem_CFGI_IMEI(void *pBuff);
static int OEMPriv_GetItem_CFGI_CUG(void *pBuff);
static int OEMPriv_SetItem_CFGI_CUG(void *pBuff);
static int OEMPriv_GetItem_CFGI_VR(void *pBuff);
static int OEMPriv_SetItem_CFGI_VR(void *pBuff);
static int OEMPriv_GetItem_CFGI_AMR(void *pBuff);
static int OEMPriv_SetItem_CFGI_AMR(void *pBuff);
static int OEMPriv_GetItem_CFGI_LCD(void *pBuff);
static int OEMPriv_SetItem_CFGI_LCD(void *pBuff);
static int OEMPriv_GetItem_CFGI_LCD_TIMER(void *pBuff);
static int OEMPriv_SetItem_CFGI_LCD_TIMER(void *pBuff);
static int OEMPriv_GetItem_CFGI_CCBS(void *pBuff);
static int OEMPriv_SetItem_CFGI_CCBS(void *pBuff);
static int OEMPriv_GetItem_CFGI_LAST_RX_DATA_COUNT(void *pBuff);
static int OEMPriv_SetItem_CFGI_LAST_RX_DATA_COUNT(void *pBuff);
static int OEMPriv_GetItem_CFGI_LAST_TX_DATA_COUNT(void *pBuff);
static int OEMPriv_SetItem_CFGI_LAST_TX_DATA_COUNT(void *pBuff);
static int OEMPriv_GetItem_CFGI_TOTAL_RX_DATA_COUNT(void *pBuff);
static int OEMPriv_SetItem_CFGI_TOTAL_RX_DATA_COUNT(void *pBuff);
static int OEMPriv_GetItem_CFGI_TOTAL_TX_DATA_COUNT(void *pBuff);
static int OEMPriv_SetItem_CFGI_TOTAL_TX_DATA_COUNT(void *pBuff);
static int OEMPriv_GetItem_CFGI_BEEP_LENGTH(void *pBuff);
static int OEMPriv_SetItem_CFGI_BEEP_LENGTH(void *pBuff);
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
static int OEMPriv_GetItem_CFGI_PPP_AUTH(void *pBuff);
static int OEMPriv_SetItem_CFGI_PPP_AUTH(void *pBuff);
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
#error code not present
#endif /* FEATURE_ACP */
static int OEMPriv_GetItem_CFGI_DATA_QNC_ENABLED(void *pBuff);
static int OEMPriv_SetItem_CFGI_DATA_QNC_ENABLED(void *pBuff);
#ifdef FEATURE_FDN
static int OEMPriv_GetItem_CFGI_FDN_ENABLED(void *pBuff);
static int OEMPriv_SetItem_CFGI_FDN_ENABLED(void *pBuff);
#endif /* FEATURE_FDN */
static int OEMPriv_GetItem_CFGI_HEADSET(void *pBuff);
static int OEMPriv_SetItem_CFGI_HEADSET(void *pBuff);
static int OEMPriv_GetItem_CFGI_STEREO_HEADSET(void *pBuff);
static int OEMPriv_SetItem_CFGI_STEREO_HEADSET(void *pBuff);
static int OEMPriv_GetItem_CFGI_USB_HFK_ATTACHED(void *pBuff);
static int OEMPriv_SetItem_CFGI_USB_HFK_ATTACHED(void *pBuff);
static int OEMPriv_GetItem_CFGI_STEREO_USB_HFK_ATTACHED(void *pBuff);
static int OEMPriv_SetItem_CFGI_STEREO_USB_HFK_ATTACHED(void *pBuff);
static int OEMPriv_GetItem_CFGI_EXT_PWR(void *pBuff);
static int OEMPriv_SetItem_CFGI_EXT_PWR(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEVICE(void *pBuff);
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
static int OEMPriv_SetItem_CFGI_BREWSET_USENAME(void* pBuff);
static int OEMPriv_GetItem_CFGI_BREWSET_USENAME(void* pBuff);
static int OEMPriv_SetItem_CFGI_BREWSET_PASSWORD(void* pBuff);
static int OEMPriv_GetItem_CFGI_BREWSET_PASSWORD(void* pBuff);
static int OEMPriv_SetItem_CFGI_LANGUAGE_MOD(void *pBuff);
static int OEMPriv_GetItem_CFGI_LANGUAGE_MOD(void *pBuff);
static int OEMPriv_GetItem_CFGI_FM_PLAY_MODE(void *pBuff);
static int OEMPriv_SetItem_CFGI_FM_PLAY_MODE(void *pBuff);
static int OEMPriv_SetItem_CFGI_IDLE_DATETIME_MODE(void *pBuff);
static int OEMPriv_GetItem_CFGI_IDLE_DATETIME_MODE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CAMERA_PHOTO_MODE(void *pBuff);
static int OEMPriv_GetItem_CFGI_CAMERA_PHOTO_MODE(void *pBuff);

#ifdef FEATURE_VERSION_W208S
static int OEMPriv_SetItem_CFGI_OTKSL_TIMES(void *pBuff);
static int OEMPriv_GetItem_CFGI_OTKSL_TIMES(void *pBuff);
#endif



#if defined(FEATURE_WCDMA)
#error code not present
#endif

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
static int OEMPriv_GetItem_CFGI_DEBUG_RF_CARD_NV_ID(void *pBuff);   
static int OEMPriv_GetItem_CFGI_DEBUG_THERM_BATT(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEBUG_THERM_MSM(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEBUG_THERM_PA(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEBUG_THERM_PMIC(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEBUG_THERM_CHRGR(void *pBuff);
#endif

#ifdef FEATURE_HDR
#error code not present
#endif

#ifdef FEATURE_QIPCALL
static int OEMPriv_GetItem_CFGI_QIPCALL_DEBUG_EVDO_IP_ADDR(void *pBuff);
static int OEMPriv_GetItem_CFGI_QIPCALL_DEBUG_EVDO_SIP_REG_STATUS(void *pBuff);
static int OEMPriv_GetItem_CFGI_QIPCALL_DEBUG_WLAN_IP_ADDR(void *pBuff);
static int OEMPriv_GetItem_CFGI_QIPCALL_DEBUG_WLAN_SIP_REG_STATUS(void *pBuff);
#endif

#if defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined(FEATURE_WCDMA) */

#ifdef FEATURE_INTELLICEIVER
static int OEMPriv_GetItem_CFGI_DEBUG_INTELLICEIVER_STATE(void *pBuff);
#endif /* FEATURE_INTELLICEIVER */

#if defined(FEATURE_USRSYSCLOCK) || defined(FEATURE_ISYSCLOCK)
static int OEMPriv_GetItem_CFGI_AUTO_TIME_ENABLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_AUTO_TIME_ENABLE(void *pBuff);
#endif 

static int OEMPriv_GetItem_CFGI_DISABLE_IN_CALL_DISP(void *pBuff);
static int OEMPriv_SetItem_CFGI_DISABLE_IN_CALL_DISP(void *pBuff);

static int OEMPriv_GetItem_CFGI_DISABLE_BG_IMAGE(void *pBuff);
static int OEMPriv_SetItem_CFGI_DISABLE_BG_IMAGE(void *pBuff);
static int OEMPriv_GetItem_CFGI_VOICEPRIVACY(void *pBuff);
static int OEMPriv_SetItem_CFGI_VOICEPRIVACY(void *pBuff);
static int OEMPriv_GetItem_CFGI_MANUAL_PLMN_SEL_ALLOWED(void *pBuff);
static int OEMPriv_SetItem_CFGI_MANUAL_PLMN_SEL_ALLOWED(void *pBuff);

#ifdef CUST_EDITION
static int OEMPriv_GetItem_CFGI_GSENSOR(void *pBuff);
static int OEMPriv_GetItem_CFGI_SHAKE_MUSIC_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_SHAKE_MUSIC_CHECK(void *pBuff);

static int OEMPriv_GetItem_CFGI_SHAKE_FM_RADIO_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_SHAKE_FM_RADIO_CHECK(void *pBuff);

static int OEMPriv_GetItem_CFGI_SHAKE_WALLPAPER_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_SHAKE_WALLPAPER_CHECK(void *pBuff);

static int OEMPriv_GetItem_CFGI_SHAKE_SNOOZE_ALARM_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_SHAKE_SNOOZE_ALARM_CHECK(void *pBuff);
static int OEMPriv_GetItem_CFGI_DEBUG_ECIO(void *pBuff);

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
static int OEMPriv_GetItem_CFGI_CALLFORWARD_UNCONDITIONAL_ENABLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_UNCONDITIONAL_ENABLE(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_UNCONDITIONAL_DISABLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_UNCONDITIONAL_DISABLE(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_UNREACHABLE_ENABLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_UNREACHABLE_ENABLE(void *pBuff);
static int OEMPriv_GetItem_CFGI_CALLFORWARD_UNREACHABLE_DISABLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALLFORWARD_UNREACHABLE_DISABLE(void *pBuff);
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
static int OEMPriv_SetItem_CFGI_INPUTMODE(void *pBuff);
static int OEMPriv_GetItem_CFGI_INPUTMODE(void *pBuff);
static int OEMPriv_GetItem_CFGI_TTY(void *pBuff);
static int OEMPriv_SetItem_CFGI_TTY(void *pBuff);
#ifdef FEATURE_TIME_DATA_SETTING
static int OEMPriv_GetItem_CFGI_DATE_FORMAT(void *pBuff);
static int OEMPriv_SetItem_CFGI_DATE_FORMAT(void *pBuff);
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

static int OEMPriv_GetItem_CFGI_PHONEBOOK_LOCK_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_PHONEBOOK_LOCK_CHECK(void *pBuff);

static int OEMPriv_GetItem_CFGI_RECENTCALL_LOCK_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_RECENTCALL_LOCK_CHECK(void *pBuff);

static int OEMPriv_GetItem_CFGI_SMS_LOCK_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_LOCK_CHECK(void *pBuff);

static int OEMPriv_GetItem_CFGI_CALENDAR_LOCK_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_CALENDAR_LOCK_CHECK(void *pBuff);

#if defined(FEATURE_VERSION_W317A) ||defined(FEATURE_VERSION_C316)
static int OEMPriv_GetItem_CFGI_MEDIAGALLERY_LOCK_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_MEDIAGALLERY_LOCK_CHECK(void *pBuff);
#endif

#ifdef FEATURE_VERSION_C316
static int OEMPriv_GetItem_CFGI_MOBILETRACKER_LOCK_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_MOBILETRACKER_LOCK_CHECK(void *pBuff);
static int OEMPriv_GetItem_CFGI_MULTIMEDIA_LOCK_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_MULTIMEDIA_LOCK_CHECK(void *pBuff);



static int OEMPriv_GetItem_CFGI_MOBILETRACKER_PASSWORD(void *pBuff);
static int OEMPriv_SetItem_CFGI_MOBILETRACKER_PASSWORD(void *pBuff);
static int OEMPriv_GetItem_CFGI_MOBILETRACKER_PASSWORD_CHECK(void *pBuff);
static int OEMPriv_SetItem_CFGI_MOBILETRACKER_PASSWORD_CHECK(void *pBuff);

static int OEMPriv_GetItem_CFGI_MOBILE_TRACKER_PHONENUMBTWO(void *pBuff);
static int OEMPriv_SetItem_CFGI_MOBILE_TRACKER_PHONENUMBTWO(void *pBuff);
static int OEMPriv_GetItem_CFGI_MOBILE_TRACKER_PHONENUMBTHREE(void *pBuff);
static int OEMPriv_SetItem_CFGI_MOBILE_TRACKER_PHONENUMBTHREE(void *pBuff);
static int OEMPriv_GetItem_CFGI_MOBILE_TRACKER_CONTECT(void *pBuff);
static int OEMPriv_SetItem_CFGI_MOBILE_TRACKER_CONTECT(void *pBuff);
static int OEMPriv_GetItem_CFGI_MOBILE_TRACKER_BACK(void *pBuff);
static int OEMPriv_SetItem_CFGI_MOBILE_TRACKER_BACK(void *pBuff);


//uint16		  mobiletracker_password;		  // CFGI_MOBILETRACKER_PASSWORD,		 //type = uint16
//boolean		mobiletracker_password_check;	// CFGI_MOBILETRACKER_PASSWORD_CHECK,  //type = boolean

#endif

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

//Add By zzg 2012_11_14
#ifdef FEATURE_VERSION_C337
static int OEMPriv_GetItem_CFGI_DEFAULTCONT(void *pBuff) ;
static int OEMPriv_SetItem_CFGI_DEFAULTCONT(void *pBuff) ;
#endif
//Add End

//Add By zzg 2012_11_14
#ifdef FEATURE_VERSION_C316
static int OEMPriv_GetItem_CFGI_ONEKEY_LOCK_KEYPAD(void *pBuff) ;
static int OEMPriv_SetItem_CFGI_ONEKEY_LOCK_KEYPAD(void *pBuff) ;
#endif
//Add End
#ifdef FEATURE_RANDOM_MENU_COLOR
static int OEMPriv_GetItem_CFGI_MENU_BGCOLOR(void *pBuff);

static int OEMPriv_SetItem_CFGI_MENU_BGCOLOR(void *pBuff);
static int OEMPriv_GetItem_CFGI_RANDOM_MENU(void *pBuff);

static int OEMPriv_SetItem_CFGI_RANDOM_MENU(void *pBuff);
#endif

#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add 
static int OEMPriv_GetItem_CFGI_REND_STATE(void *pBuff);
static int OEMPriv_SetItem_CFGI_REND_STATE(void *pBuff);
#endif//FEATURE_RANDOM_MENU_REND

#ifdef FEATURE_LCD_TOUCH_ENABLE
static int OEMPriv_GetItem_CFGI_PEN_CAL(void *pBuff);
static int OEMPriv_SetItem_CFGI_PEN_CAL(void *pBuff);
#endif//FEATURE_TOUCHPAD

#ifdef FEATURE_ANALOG_TV
static int OEMPriv_GetItem_CFGI_TV_SETCHANNL(void *pBuff);
static int OEMPriv_SetItem_CFGI_TV_SETCHANNL(void *pBuff);
#endif

#ifdef FEATURE_ANALOG_TV
static int OEMPriv_GetItem_CFGI_TV_OR_CAMERA(void *pBuff);
static int OEMPriv_SetItem_CFGI_TV_OR_CAMERA(void *pBuff);
#endif

#ifdef FEATURE_LONG_NETLOCK
static int OEMPriv_GetItem_CFGI_NET_LOCK_FLAGS(void *pBuff);
static int OEMPriv_SetItem_CFGI_NET_LOCK_FLAGS(void *pBuff);

#endif

#ifdef FEATURE_USES_MMS
static int OEMPriv_GetItem_CFGI_MMS_SETTING(void *pBuff);
static int OEMPriv_SetItem_CFGI_MMS_SETTING(void *pBuff);

static int OEMPriv_GetItem_CFGI_MMS_USER_NAME(void* pBuff);
static int OEMPriv_SetItem_CFGI_MMS_USER_NAME(void* pBuff);

static int OEMPriv_GetItem_CFGI_MMS_PASSWORD(void* pBuff);
static int OEMPriv_SetItem_CFGI_MMS_PASSWORD(void* pBuff);

static int OEMPriv_GetItem_CFGI_MMS_PROXY(void* pBuff);
static int OEMPriv_SetItem_CFGI_MMS_PROXY(void* pBuff);

static int OEMPriv_GetItem_CFGI_MMS_PORT(void* pBuff);
static int OEMPriv_SetItem_CFGI_MMS_PORT(void* pBuff);

#endif

//Add By zzg 2010_11_22
#ifdef FEATURE_APP_BLUETOOTH
static int OEMPriv_GetItem_CFGI_BT_STATUS(void *pBuff);
static int OEMPriv_SetItem_CFGI_BT_STATUS(void *pBuff);

#endif
//Add End

static int OEMPriv_GetItem_CFGI_WMSWRITD_END_STATUS(void *pBuff);
static int OEMPriv_SetItem_CFGI_WMSWRITD_END_STATUS(void *pBuff);

static int OEMPriv_GetItem_CFGI_FLSHLITHG_STATUS(void *pBuff);
static int OEMPriv_SetItem_CFGI_FLSHLITHG_STATUS(void *pBuff);

static int OEMPriv_GetItem_CFGI_ALARM_FLAG(void *pBuff);
static int OEMPriv_SetItem_CFGI_ALARM_FLAG(void *pBuff);

#ifdef FEATURE_SPORTS_APP
static int OEMPriv_GetItem_CFGI_SPORT_FLAG(void *pBuff);
static int OEMPriv_SetItem_CFGI_SPORT_FLAG(void *pBuff);
#endif


#ifdef FEATURE_MENU_STYLE
static int OEMPriv_GetItem_CFGI_MENU_STYLE(void *pBuff);
static int OEMPriv_SetItem_CFGI_MENU_STYLE(void *pBuff);
#endif

#ifdef FEATURE_CAMERA_MULTI_SENSOR
static int OEMPriv_GetItem_CFGI_CAMERA_MULTI(void *pBuff);
static int OEMPriv_SetItem_CFGI_CAMERA_MULTI(void *pBuff);
static int OEMPriv_GetItem_CFGT_CAMERA_FLASH(void *pBuff);
static int OEMPriv_SetItem_CFGT_CAMERA_FLASH(void *pBuff);
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
#ifdef FEATURE_PLANEMODE
static int OEMPriv_GetItem_CFGI_PLANEMODE(void *pBuff);
static int OEMPriv_SetItem_CFGI_PLANEMODE(void *pBuff);
#endif

static int OEMPriv_GetItem_CFGI_MISSED_CALL_ICON(void *pBuff);
static int OEMPriv_SetItem_CFGI_MISSED_CALL_ICON(void *pBuff);
#endif //#ifdef CUST_EDITION

#ifdef FEATURE_USES_MMS   
static int OEMPriv_GetItem_CFGI_MMSOUTDATA_INFO(void *pBuff);
static int OEMPriv_SetItem_CFGI_MMSOUTDATA_INFO(void *pBuff);
static int OEMPriv_GetItem_CFGI_MMSINDATA_INFO(void *pBuff);
static int OEMPriv_SetItem_CFGI_MMSINDATA_INFO(void *pBuff);
static int OEMPriv_GetItem_CFGI_MMS_OUTCOUNT(void *pBuff); 
static int OEMPriv_SetItem_CFGI_MMS_OUTCOUNT(void *pBuff);
static int OEMPriv_GetItem_CFGI_MMS_INCOUNT(void *pBuff); 
static int OEMPriv_SetItem_CFGI_MMS_INCOUNT(void *pBuff);
static int OEMPriv_GetItem_CFGI_WMS_MMSNOTIFY(void *pBuff);
static int OEMPriv_SetItem_CFGI_WMS_MMSNOTIFY(void *pBuff);

static int OEMPriv_GetItem_CFGI_WMS_MMSDELIVERYREPORT(void *pBuff);
static int OEMPriv_SetItem_CFGI_WMS_MMSDELIVERYREPORT(void *pBuff);

static int OEMPriv_GetItem_CFGI_WMS_READREPLY(void *pBuff);
static int OEMPriv_SetItem_CFGI_WMS_READREPLY(void *pBuff);

static int OEMPriv_GetItem_CFGI_WMS_REPORTALLOWED(void *pBuff);
static int OEMPriv_SetItem_CFGI_WMS_REPORTALLOWED(void *pBuff);

static int OEMPriv_GetItem_CFGI_WMS_SENDERVISIBILITY(void *pBuff);
static int OEMPriv_SetItem_CFGI_WMS_SENDERVISIBILITY(void *pBuff);

static int OEMPriv_GetItem_CFGI_MMSDRAFTDATA_INFO(void *pBuff);
static int OEMPriv_SetItem_CFGI_MMSDRAFTDATA_INFO(void *pBuff);
static int OEMPriv_GetItem_CFGI_MMS_DRAFTCOUNT(void *pBuff); 
static int OEMPriv_SetItem_CFGI_MMS_DRAFTCOUNT(void *pBuff);

#endif

#ifdef FEATURE_VERSION_W208S
static int OEMPriv_GetItem_CFGI_SMS_RESTRICT_RECEIVE_INFO(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_RESTRICT_RECEIVE_INFO(void *pBuff);
static int OEMPriv_GetItem_CFGI_SMS_RESTRICT_RECEIVE_TOTAL(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_RESTRICT_RECEIVE_TOTAL(void *pBuff);
#endif
#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)
static int OEMPriv_GetItem_CFGI_MOBILE_TRACKER_PHONENUMB(void *pBuff);
static int OEMPriv_SetItem_CFGI_MOBILE_TRACKER_PHONENUMB(void *pBuff);
static int OEMPriv_GetItem_CFGI_MOBILE_TRACKER_IMSI(void *pBuff);
static int OEMPriv_SetItem_CFGI_MOBILE_TRACKER_IMSI(void *pBuff);
static int OEMPriv_GetItem_CFGI_MOBILE_TRACKER_SETNUM(void *pBuff);
static int OEMPriv_SetItem_CFGI_MOBILE_TRACKER_SETNUM(void *pBuff);
static int OEMPriv_GetItem_CFGI_SMS_TRACKER_SEND_B(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_TRACKER_SEND_B(void *pBuff);
static int OEMPriv_GetItem_CFGI_SMS_TRACKER_TIME(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_TRACKER_TIME(void *pBuff);
static int OEMPriv_GetItem_CFGI_SMS_TRACKER_NUMBER(void *pBuff);
static int OEMPriv_SetItem_CFGI_SMS_TRACKER_NUMBER(void *pBuff);
#endif
#ifdef FEATURE_VERSION_C337
static int OEMPriv_GetItem_CFGI_MIZONE_NUM(void *pBuff);
static int OEMPriv_SetItem_CFGI_MIZONE_NUM(void *pBuff);
static int OEMPriv_GetItem_CFGI_MIZONE_SMSINFO(void *pBuff);
static int OEMPriv_SetItem_CFGI_MIZONE_SMSINFO(void *pBuff);
#endif
//Add by pyuangui 20121231
#if defined(FEATURE_VERSION_C316) || defined(FEATURE_VERSION_W317A)
static int OEMPriv_GetItem_CFGI_AUTOCALLRECORD(void *pBuff);
static int OEMPriv_SetItem_CFGI_AUTOCALLRECORD(void *pBuff);
#endif
//Add End


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
   OEMNV_ALERTTYPE_RINGER,                          // CFGI_ALERT_TYPE
#endif
   OEMNV_SA_NORMAL,                                 // CFGI_SILENCEALL
   OEMNV_HEADSET_RNG_ON,                            // CFGI_HEADSET_RNG
   OEMNV_ALERT_ENABLE,                              // CFGI_MISSED_CALL_ALERT
   OEMNV_ALERT_ENABLE,                              // CFGI_ROAMING_ALERT
   OEMNV_ALERT_ENABLE,                              // CFGI_INTERNET_ALERT
   0,                                               // CFGI_BROWSER_CALL_TIMER
   OEMNV_SMS_SO_SO6,
   OEMNV_SMS_EMAIL_ADDRESSING_IS41,
#ifdef FEATURE_VERSION_W208S   
   OEMNV_PPP_AUTH_DISABLED,							// CFGI_PPP_AUTH
#else
	OEMNV_PPP_AUTH_ENABLED,							// CFGI_PPP_AUTH
#endif   
#if defined(FEATURE_CARRIER_ANGOLA_MOVICEL) || defined(FEATURE_CARRIER_MAROC_WANA) || defined (FEATURE_CARRIER_ISRAEL_PELEPHONE)
   OEMNV_SMS_MO_ENCODING_OCTET,
#else   
   OEMNV_SMS_MO_ENCODING_7BIT,
#endif
   OEMNV_SMS_MODE_CDMA,
   OEMNV_SMS_APP_TEST,
   OEMNV_SMS_CB_DISABLE_AUTO_DELETE,
   OEMNV_SMS_CB_DISABLE_SILENT_ALERT,
   OEMNV_SMS_CB_DISABLE_LANG_FILTER,
   OEMNV_SMS_ALERT_DISPLAY_ENABLE,
   OEMNV_SMS_ALERT_SOUND_ENABLE,
   OEMNV_SMS_ALERT_VOLUME_2,
   0,                                              // CFGI_SMS_CDMA_DEFAULT_TEMPLATE
   0,                                              // CFGI_SMS_GW_DEFAULT_TEMPLATE
   8,                                              // CFGI_SMS_SEGMENT_SETTING
   0,                                              // CFGI_SMS_SEGMENT_REF_NUM
   OEMNV_SMS_RETRY_PERIOD,                         // CFGI_SMS_RETRY_PERIOD
   OEMNV_SMS_RETRY_INTERVAL,                       // CFGI_SMS_RETRY_INTERVAL
   FALSE,                                          // CFGI_SMS_GCF_FLAG
   {OEMNV_VOICEMAIL_NUMBER},                                           // CFGI_VOICEMAIL_NUMBER
   {OEMNV_VOICEMAIL_NUMBER},                                           // CFGI_VOICEMAIL_NUMBER_CDMA
   {OEMNV_VOICEMAIL_NUMBER},                                           // CFGI_VOICEMAIL_NUMBER_GW
   0,                                              // CFGI_RECENT_MT_CALL_TIMER
   0,                                              // CFGI_RECENT_MO_TIMER
   OEMNV_ALERT_DISABLE,                            // CFGI_MINUTE_ALERT
#ifdef CUST_EDITION
   OEM_AUTO_ANSWER_MODE,                           // CFGI_ANYKEY_ANSWER
#else
   FALSE,                                          // CFGI_ANYKEY_ANSWER
#endif
   OEMNV_HEADSET_AUTOANSWER_OFF,                   // CFGI_HEADSET_AUTOANSWER
   OEMNV_TIMEFORM_AMPM,
   0,                                              // CFGI_DEVICE
   {0,},                                           // CFGI_CUG
   FALSE,                                          // CFGI_VR
   {DEFAULT_BREW_CARRIER_ID, DEFAULT_BREW_PLATFORM_ID,   // Download Info
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0,0,0,0},
    DEFAULT_BREW_SERVER, DEFAULT_BREW_DOWNLOAD_FLG,
    DEFAULT_BREW_APOLICY, DEFAULT_BREW_PPOLICY
   },
   "0000000000000000000000000000000",              // CFGI_SUBSCRIBERID
   FALSE,                                          // CFGI_DISABLE_IN_CALL_DISP
   FALSE,                                          // CFGI_DISABLE_BG_IMAGE
   TRUE,                                           // CFGI_MANUAL_PLMN_SEL_ALLOWED
#ifdef CUST_EDITION
   0,                                               //CFGI_GSENSOR
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
#if defined(FEATURE_VERSION_W317A) ||defined(FEATURE_VERSION_C316)   
   FALSE,											//CFGI_MEDIAGALLERY_LOCK_CHECK
#endif 
#ifdef FEATURE_VERSION_C316
   FALSE,
   FALSE,
   OEMNV_PHONE_PASSWORD,
   FALSE,
   {0},
   {0},
   {L"Mobile Tracker Alert!:The sender of this SMS is using your phone."},
   FALSE,
#endif
#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)||defined(FEATURE_VERSION_C01)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)
   1,											//CFGI_KEY_LOCK_CHECK			
#elif defined(FEATURE_VERSION_K202_LM129C)//xxzhen
   2,
#else
   0,
#endif   

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
#ifdef FEATURE_USES_MMS
   {TRUE,MMS_DEFAULT_PROXYHOST,MMS_DEFAULT_PROXYPORT},
#endif

   WMS_PRIORITY_NORMAL,                             //CFGI_WMS_PRIORITY
   SENDOPT_SAVEANDSEND,                             //CFGI_WMS_SENDMODE
   WMS_MEMORY_STORE_NV_CDMA,                        //CFGI_WMS_STORETYPE
   TRUE                                            //CFGI_WMS_DELIVERYREPORTS
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
   

   , MAX_FMRADIO_VOLUME/5 *3                   //CFGI_FMRADIO_VOLUME
#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
   ,{{L"88", 5}, {L"88.5", 10}, {L"89.5", 20}, {L"91", 35}, {L"91.5", 40},
    {L"93", 55}, {L"94",   65}, {L"94.5", 70}, {L"95", 75}, {L"95.5", 80},
    {L"96", 85}, {L"97.5", 100}, {L"98.5", 110}, {L"100",125}, {L"102.5",150},
    {L"103",155},{L"103.5",160},{L"105.5",180},{L"106.5",190}, {L"107.5", 200} 
    }  //CFGI_FMRADIO_CHAN_INFO
    ,20//CFGI_FMRADIO_CHAN_TOTAL
#elif defined(FEATURE_VERSION_C316)
   ,{{L"91.1", 36}, {L"92.7", 52}, {L"93.5", 60}, {L"94.3", 68}, {L"95", 75},
    {L"98.6", 111}, {L"102.4", 149}, {L"104", 165}, {L"106.4", 189}}  //CFGI_FMRADIO_CHAN_INFO
    ,9//CFGI_FMRADIO_CHAN_TOTAL
#else   
   ,{0}                                              //CFGI_FMRADIO_CHAN_INFO
   , 0                                              //CFGI_FMRADIO_CHAN_TOTAL
#endif
   ,OEMNV_EMERGENCYNUM_TABLE_NUM                                            //CFGI_EMERGENCYNUM_TABLE
   ,{OEMNV_CALLFORWARD_BUSY_ENABLE}                                         //CFGI_CALLFORWARD_BUSY_ENABLE
   ,{OEMNV_CALLFORWARD_BUSY_DISABLE}                                        //CFGI_CALLFORWARD_BUSY_DISABLE
   ,{OEMNV_CALLFORWARD_NOANSWER_ENABLE}                                     //CFGI_CALLFORWARD_NOANSWER_ENABLE
   ,{OEMNV_CALLFORWARD_NOANSWER_DISABLE}                                    //CFGI_CALLFORWARD_NOANSWER_DISABLE
   ,{OEMNV_CALLFORWARD_UNCONDITIONAL_ENABLE}                                    //CFGI_CALLFORWARD_UNCONDITIONAL_ENABLE
   ,{OEMNV_CALLFORWARD_UNCONDITIONAL_DISABLE}                                   //CFGI_CALLFORWARD_UNCONDITIONAL_DISABLE
   ,{OEMNV_CALLFORWARD_UNREACHABLE_ENABLE}                                       //CFGI_CALLFORWARD_UNREACHABLE_ENABLE
   ,{OEMNV_CALLFORWARD_UNREACHABLE_DISABLE}                                      //CFGI_CALLFORWARD_UNREACHABLE_DISABLE
   ,{OEMNV_CALLFORWARD_DISABLE_ALL}                                         //CFGI_CALLFORWARD_DISABLE_ALL
   ,{OEMNV_CALLFORWARD_WAIT_ENABLE}                                         //CFGI_CALLFORWARD_WAIT_ENABLE
   ,{OEMNV_CALLFORWARD_WAIT_DISABLE}                                        //CFGI_CALLFORWARD_WAIT_DISABLE
   ,{OEMNV_CALLFORWARD_VOICEMAIL_ENABLE}                                    //CFGI_CALLFORWARD_VOICEMAIL_ENABLE
   ,OEMNV_INPUTMODE_DEFAULT                        //CFGI_INPUTMODE     
   ,FALSE                                          //CFGI_FM_BACKGROUND
#ifdef FEATURE_VERSION_C337
   ,FALSE											//CFGI_DEFAULTCONT
#endif
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
    #ifdef FEATURE_MENU_STYLE
   ,OEMNV_MENU_STYLE_ICON
   #endif
#ifdef FEATURE_CAMERA_MULTI_SENSOR
   ,OEMNV_CAMERA_MULTI_TWO       //CFGI_CAMER_MULTI
   ,OEMNV_CAMERA_FLASH_OFF
#endif
   ,OEMNV_CAMERA_ENVIR_AUTO         /*CFGI_CAMERA_ENVIROMENT*/
   ,OEMNV_CAMERA_QUALITY_HIGH       /*CFGI_CAMERA_QUALITY*/
   ,OEMNV_CAMERA_SIZE_DEFAULT         /*CFGI_CAMERA_SIZE*/
#ifdef FEATURE_CARRIER_CHINA_TELCOM
   ,OEMNV_CAMERA_SHUTTER_TONE_SHUTTER1/*CFGI_CAMERA_TONE*/
#else
   ,OEMNV_CAMERA_SHUTTER_TONE_ENABLE       /*CFGI_CAMERA_TONE*/
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
   ,FALSE
#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add for rend
#if defined (FEATURE_VERSION_C337) || defined (FEATURE_VERSION_C316)
   ,DISPLAYREND_TYPE_MAX
#else
   ,DISPLAYREND_TYPE_ONEBYONE
#endif   
#endif
#ifdef FEATURE_LCD_TOUCH_ENABLE
   ,{-1,-1,-1,-1}
#endif//FEATURE_TOUCHPAD
#endif //CUST_EDITION
   ,{DEFAULT_BREW_USERNAME}  //CFGI_BREWSET_USENAME
   ,{DEFAULT_BREW_PASSWORD}  //CFGI_BREWSET_PASSWORD
   ,FALSE    //CFGI_LANGUAGE_MOD
#ifdef FEATURE_ANALOG_TV
   ,0
   ,OEMNV_TV_TvSetting //收藏夹
#endif
   //Add By zzg 2010_11_22
   #ifdef FEATURE_APP_BLUETOOTH
   ,0 
   #endif
   ,0
   ,1
   ,1
   ,1
   ,1
   ,1
   //Add End
   #ifdef FEATURE_LONG_NETLOCK
   ,0
   #endif
#ifdef FEATURE_USES_MMS   
   ,{OEMNV_MMSIMAGE}
   ,{OEMNV_MMSSOUND}
   ,{OEMNV_MMSVIDEO}
   ,{{"0", "0", 0, FALSE}, {"0", "0", 0, FALSE}, {"0", "0", 0, FALSE}, {"0", "0", 0, FALSE}, {"0", "0", 0, FALSE}
    }    ,      
    0   
   ,{{"0", "0", 0, FALSE}, {"0", "0", 0, FALSE}, {"0", "0", 0, FALSE}, {"0", "0", 0, FALSE}, {"0", "0", 0, FALSE}
    }  //CFGI_MMSDATA_INFO
   ,{{"0", "0", 0, FALSE}, {"0", "0", 0, FALSE}, {"0", "0", 0, FALSE}, {"0", "0", 0, FALSE}, {"0", "0", 0, FALSE}
    },  //CFGI_MMSDATA_INFO
    0,//CFGI_MMS_COUNT 
    0,//CFGI_MMS_COUNT
    TRUE,
    -1,
    -1,
    -1,
    -1,
    {"http://mms.movilnet.com.ve"}
   ,{""}
   ,{""}    
   ,{OEMNV_MMSPROXY}
   ,{OEMNV_MMSPORT}     
#endif   
#ifdef FEATURE_VERSION_W208S
     ,0                                              //CFGI_SMS_RESTRICT_TOTAL
    ,{0}                                              //CFGI_SMS_RESTRICT_RECEIVE_INFO
#endif    
#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)
	,{0}  //CFGI_MOBILE_TRACKER_PHONENUMB
	,{0}
	,FALSE
	,FALSE
#if defined(FEATURE_VERSION_W317A)
	,240   //  CFGI_SMS_TRACKER_TIME
#else
	,10    //  CFGI_SMS_TRACKER_TIME
#endif
	,{OEMNV_DEFAULTNUMBER} //CFGI_SMS_TRACKER_NUMBER
#endif
#ifdef FEATURE_VERSION_C337
    ,{OEMNV_MIZONENUM}
    ,{0}
#endif
//Add by pyuangui 20120104
#if defined(FEATURE_VERSION_C316) || defined(FEATURE_VERSION_W317A)
    ,FALSE
#endif
//Add End
#ifdef FEATURE_VERSION_C316
   ,TRUE											//CFGI_ONEKEY_LOCK_KEYPAD
#endif

};

////
// Cache of NV configuration items (defaults are supplied by OEMNV.c)
static NVConfigListType nvi_cache;

#ifdef FEATURE_HDR
#error code not present
#endif


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
   CFGTABLEITEM_EMPTY(CFGI_AUTOKEYGUARD),
   CFGTABLEITEM(CFGI_RECENT_MT_CALL_TIMER, sizeof(dword)),
   CFGTABLEITEM(CFGI_RECENT_MO_CALL_TIMER, sizeof(dword)),
   CFGTABLEITEM(CFGI_ALL_CALL_TIMER, sizeof(dword)),
   CFGTABLEITEM(CFGI_BROWSER_CALL_TIMER, sizeof(dword)),
   CFGTABLEITEM(CFGI_HEADSET_RNG, sizeof(byte)),
#ifdef CUST_EDITION
   CFGTABLEITEM(CFGI_ANYKEY_ANSWER, sizeof(byte)),
#else
   CFGTABLEITEM(CFGI_ANYKEY_ANSWER, sizeof(boolean)),
#endif
   CFGTABLEITEM(CFGI_HEADSET_AUTOANSWER, sizeof(byte)),
   CFGTABLEITEM(CFGI_MISSED_CALL_ALERT, sizeof(byte)),
   CFGTABLEITEM(CFGI_ROAMING_ALERT, sizeof(byte)),
   CFGTABLEITEM(CFGI_INTERNET_ALERT, sizeof(byte)),
   CFGTABLEITEM(CFGI_MINUTE_ALERT, sizeof(byte)),
   CFGTABLEITEM(CFGI_GAME_MODE, sizeof(boolean)),
   CFGTABLEITEM(CFGI_GAME_MODE_PERSIST, sizeof(boolean)),

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
#ifdef CUST_EDITION
   CFGTABLEITEM(CFGI_KEYTONE_LENGTH, sizeof(keyToneLength)),
#else
   CFGTABLEITEM_EMPTY(CFGI_KEYTONE_LENGTH),
#endif
   CFGTABLEITEM(CFGI_RINGER_TYPE, sizeof(byte)),
   CFGTABLEITEM(CFGI_IMEI, OEMCFG_IMEI_SIZE),
   CFGTABLEITEM(CFGI_CUG, sizeof(OEMConfigCugInfo)),
   CFGTABLEITEM(CFGI_VR, sizeof(boolean)),
   CFGTABLEITEM(CFGI_AMR, sizeof(boolean)),
   CFGTABLEITEM(CFGI_LCD, sizeof(uint8)),
   CFGTABLEITEM(CFGI_LCD_TIMER, sizeof(uint32)),
   CFGTABLEITEM(CFGI_CCBS, sizeof(OEMCCBS)),

   ////////////////////////////////////////////////////////////////
   // Data Call Counters
   ////////////////////////////////////////////////////////////////
   CFGTABLEITEM(CFGI_LAST_RX_DATA_COUNT, sizeof(uint32)),
   CFGTABLEITEM(CFGI_LAST_TX_DATA_COUNT, sizeof(uint32)),
   CFGTABLEITEM(CFGI_TOTAL_RX_DATA_COUNT, sizeof(uint32)),
   CFGTABLEITEM(CFGI_TOTAL_TX_DATA_COUNT, sizeof(uint32)),
   CFGTABLEITEM(CFGI_BEEP_LENGTH, sizeof(uint32)),

   CFGTABLEITEM(CFGI_CDMA_RX_DIVERSITY_CTL, sizeof(boolean)),
   CFGTABLEITEM(CFGI_HDR_RX_DIVERSITY_CTL, sizeof(boolean)),

   ////////////////////////////////////////////////////////////////
   // Phone Number
   ////////////////////////////////////////////////////////////////
   CFGTABLEITEM_EMPTY(CFGI_PHONE_NUMBER),

   ////////////////////////////////////////////////////////////////
   // Display Menu
   ////////////////////////////////////////////////////////////////

   CFGTABLEITEM(CFGI_BACK_LIGHT_INTENSITY, sizeof(uint8)),
#ifdef CUST_EDITION
   CFGTABLEITEM(CFGI_LANGUAGE_SELECTION, sizeof(nv_language_enum_type)),
#else
   CFGTABLEITEM(CFGI_LANGUAGE_SELECTION, sizeof(byte)),
#endif
   CFGTABLEITEM_EMPTY(CFGI_BANNER),
   CFGTABLEITEM(CFGI_MENU_FORMAT, sizeof(byte)),
   CFGTABLEITEM(CFGI_TIME_FORMAT, sizeof(byte)),
   CFGTABLEITEM(CFGI_MANUAL_PLMN_SEL, sizeof(byte)),
#if defined(FEATURE_USRSYSCLOCK) || defined(FEATURE_ISYSCLOCK)
   CFGTABLEITEM(CFGI_AUTO_TIME_ENABLE, sizeof(byte)),
   CFGTABLEITEM_EMPTY(CFGI_NITZ_NW_INFO),
   CFGTABLEITEM_EMPTY(CFGI_NITZ_NW_INFO_MCC),
   CFGTABLEITEM_EMPTY(CFGI_NITZ_NW_INFO_MNC),
#endif 

   ////////////////////////////////////////////////////////////////
   // Toolkit
   ////////////////////////////////////////////////////////////////
#ifdef FEATURE_GSTK_CS_PS_PARALLEL   
   CFGTABLEITEM(CFGI_TOOLKIT_CS_PS_PARALLEL, sizeof(boolean)),
#endif /* FEATURE_GSTK_CS_PS_PARALLEL */

   ////////////////////////////////////////////////////////////////
   // SMS Applet
   ////////////////////////////////////////////////////////////////

   CFGTABLEITEM(CFGI_SMS_SERVICE_OPTION, sizeof(byte)),
   CFGTABLEITEM(CFGI_SMS_TIMESTAMP, sizeof(byte)),   
   CFGTABLEITEM(CFGI_SMS_IS41_WORKAROUND, sizeof(byte)),
   CFGTABLEITEM(CFGI_PPP_AUTH, sizeof(byte)),
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
   CFGTABLEITEM_EMPTY(CFGI_SMS_RETRY_PERIOD),
   CFGTABLEITEM_EMPTY(CFGI_SMS_RETRY_INTERVAL),
   CFGTABLEITEM_EMPTY(CFGI_SMS_GCF_FLAG),
   CFGTABLEITEM_EMPTY(CFGI_VOICEMAIL_NUMBER),
   CFGTABLEITEM_EMPTY(CFGI_VOICEMAIL_NUMBER_CDMA),
   CFGTABLEITEM_EMPTY(CFGI_VOICEMAIL_NUMBER_GW),

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
   CFGTABLEITEM_EMPTY(CFGI_BUILD_TIME),
   CFGTABLEITEM_READONLY(CFGI_RFCAL_VERSION, sizeof(AECHAR) *
                                             OEMNV_RFCALVER_LEN),
   CFGTABLEITEM_READONLY(CFGI_RFCAL_DATE, sizeof(dword)),
   CFGTABLEITEM(CFGI_HOME_SIDNID_LIST,sizeof(AEEConfigSIDNIDPairType) *
                                      OEMNV_HOME_SIDNID_ARRSIZE),
   CFGTABLEITEM(CFGI_LOCK_SIDNID_LIST,sizeof(AEEConfigSIDNIDPairType) *
                                      OEMNV_LOCK_SIDNID_ARRSIZE),
   CFGTABLEITEM(CFGI_IMSI_MCC,sizeof(uint16)),
   CFGTABLEITEM(CFGI_IMSI_11_12,sizeof(uint16)),
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
#error code not present
#endif /* FEATURE_ACP */
   CFGTABLEITEM(CFGI_DATA_QNC_ENABLED,sizeof(boolean)),
   CFGTABLEITEM_EMPTY(CFGI_DATA_DIALSTRING),
   
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
   CFGTABLEITEM(CFGI_RTRE_CONFIGURATION,sizeof(nv_rtre_configuration_type)),
#endif 
   CFGTABLEITEM_EMPTY(CFGI_BREW_CID),
   CFGTABLEITEM_EMPTY(CFGI_BREW_PID),
   CFGTABLEITEM_EMPTY(CFGI_BREW_BKEY),
   CFGTABLEITEM_EMPTY(CFGI_BREW_AKEY),
   CFGTABLEITEM_EMPTY(CFGI_BREW_SERVER),
   CFGTABLEITEM_EMPTY(CFGI_BREW_FLAGS),
   CFGTABLEITEM_EMPTY(CFGI_BREW_AUTH),
   CFGTABLEITEM_EMPTY(CFGI_BREW_PRIVP),

   ////////////////////////////////////////////////////////////////
   // Security Menu
   ////////////////////////////////////////////////////////////////
   CFGTABLEITEM_EMPTY(CFGI_LOCKCODE),

#ifdef FEATURE_FDN
   
   CFGTABLEITEM(CFGI_FDN_ENABLED,sizeof(boolean)),
#endif /* FEATURE_FDN */

   CFGTABLEITEM(CFGI_HEADSET,sizeof(boolean)),
   CFGTABLEITEM(CFGI_STEREO_HEADSET,sizeof(boolean)),
   CFGTABLEITEM(CFGI_USB_HFK_ATTACHED,sizeof(boolean)),
   CFGTABLEITEM(CFGI_STEREO_USB_HFK_ATTACHED,sizeof(boolean)),
   CFGTABLEITEM(CFGI_EXT_PWR,sizeof(boolean)),
#ifdef FEATURE_VOICE_FIXED_PATTERN_DEBUG
#error code not present
#endif /* FEATURE_VOICE_FIXED_PATTERN_DEBUG */

   CFGTABLEITEM(CFGI_DISABLE_IN_CALL_DISP, sizeof(boolean)),
   CFGTABLEITEM(CFGI_DISABLE_BG_IMAGE, sizeof(boolean)),
   CFGTABLEITEM(CFGI_MANUAL_PLMN_SEL_ALLOWED, sizeof(boolean)),
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

#if defined(FEATURE_WCDMA)
#error code not present
#endif

#ifdef T_MSM6500
   CFGTABLEITEM_READONLY(CFGI_DEBUG_RF_CARD_NV_ID,sizeof(uint16)),
   CFGTABLEITEM_READONLY(CFGI_DEBUG_THERM_BATT,sizeof(byte)),
   CFGTABLEITEM_READONLY(CFGI_DEBUG_THERM_MSM,sizeof(byte)),
   CFGTABLEITEM_READONLY(CFGI_DEBUG_THERM_PA,sizeof(byte)),
   CFGTABLEITEM_READONLY(CFGI_DEBUG_THERM_PMIC,sizeof(byte)),
   CFGTABLEITEM_READONLY(CFGI_DEBUG_THERM_CHRGR,sizeof(byte)),
#endif

   // Items required for power calculation in Debug Menu
   CFGTABLEITEM_READONLY(CFGI_CDMA_DYNAMIC_RANGE,sizeof(uint16)),
   CFGTABLEITEM_READONLY(CFGI_CDMA_MIN_RX_RSSI,sizeof(uint16)),

#ifdef FEATURE_QIPCALL
   CFGTABLEITEM_READONLY(CFGI_QIPCALL_DEBUG_EVDO_IP_ADDR, sizeof(dword)),
   CFGTABLEITEM_READONLY(CFGI_QIPCALL_DEBUG_EVDO_SIP_REG_STATUS, sizeof(byte)),
   CFGTABLEITEM_READONLY(CFGI_QIPCALL_DEBUG_WLAN_IP_ADDR, sizeof(dword)),
   CFGTABLEITEM_READONLY(CFGI_QIPCALL_DEBUG_WLAN_SIP_REG_STATUS, sizeof(byte)),
#endif

#ifdef FEATURE_HDR
#error code not present
#endif

   CFGTABLEITEM_READONLY(CFGI_DEBUG_THERM,sizeof(word)),
   CFGTABLEITEM_READONLY(CFGI_DEBUG_BAND,sizeof(byte)),
#ifdef CUST_EDITION
   CFGTABLEITEM(CFGI_KEYGUARD_ENABLED,sizeof(boolean)),
#else
   CFGTABLEITEM_EMPTY(CFGI_KEYGUARD_ENABLED),
#endif
#ifdef FEATURE_INTELLICEIVER
  CFGTABLEITEM_READONLY(CFGI_DEBUG_INTELLICEIVER_STATE,sizeof(byte)),
#endif /* FEATURE_INTELLICEIVER */
};
#ifdef CUST_EDITION
static ConfigItemTableEntry const customOEMItemTable[] =
{
   CFGTABLEITEM(CFGI_BACK_LIGHT, sizeof(byte)),
   CFGTABLEITEM(CFGI_SVC_ALERT, sizeof(byte)),
   CFGTABLEITEM(CFGI_EXTPWR_BK_LIGHT, sizeof(byte)),
   CFGTABLEITEM(CFGI_VOICEPRIVACY,sizeof(byte)),
   CFGTABLEITEM(CFGI_CONTRAST_LVL, sizeof(byte)),
   CFGTABLEITEM_READONLY(CFGI_FACTORY_TEST_MODE,sizeof(boolean)),
   CFGTABLEITEM(CFGI_TTY, sizeof(OEMTTY)),
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
   //短信铃声(0-表示无)
   CFGTABLEITEM(CFGI_SMS_RINGER, sizeof(byte)) ,             // type=byte    
   //闹钟铃声,
   CFGTABLEITEM(CFGI_ALARM_RINGER, sizeof(uint16)),        
   
   //短信铃声
   CFGTABLEITEM(CFGI_SMS_RINGER_ID, sizeof(uint16)),
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
#ifdef FEATURE_USES_MMS
	CFGTABLEITEM(CFGI_MMS_SETTING, sizeof(CFG_MMsInfo)),
#endif

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
   CFGTABLEITEM(CFGI_PHONE_PASSWORD, sizeof(uint16)) ,       //type = uint16
   CFGTABLEITEM(CFGI_PHONE_PASSWORD_CHECK,  sizeof(byte)),//type = boolean
   CFGTABLEITEM(CFGI_RESTRICT_OUTGOING, sizeof(byte)) ,      //type = byte
   CFGTABLEITEM(CFGI_RESTRICT_INCOMING, sizeof(byte)),       //type = byte
   CFGTABLEITEM(CFGI_PHONEBOOK_LOCK_CHECK, sizeof(boolean)), //type = boolena
   CFGTABLEITEM(CFGI_RECENTCALL_LOCK_CHECK, sizeof(boolean)),//type = boolena
   CFGTABLEITEM(CFGI_SMS_LOCK_CHECK, sizeof(boolean)),       //type = boolena
   CFGTABLEITEM(CFGI_CALENDAR_LOCK_CHECK, sizeof(boolean)),  //type = boolean
#if defined(FEATURE_VERSION_W317A) ||defined(FEATURE_VERSION_C316)
   CFGTABLEITEM(CFGI_MEDIAGALLERY_LOCK_CHECK, sizeof(boolean)),  //type = boolean
#endif
#ifdef FEATURE_VERSION_C316
   CFGTABLEITEM(CFGI_MOBILETRACKER_LOCK_CHECK, sizeof(boolean)),  //type = boolean
   CFGTABLEITEM(CFGI_MULTIMEDIA_LOCK_CHECK, sizeof(boolean)),  //type = boolean
   CFGTABLEITEM(CFGI_MOBILETRACKER_PASSWORD, sizeof(uint16)),  //type = boolean
   CFGTABLEITEM(CFGI_MOBILETRACKER_PASSWORD_CHECK, sizeof(boolean)),  //type = boolean
   CFGTABLEITEM(CFGI_MOBILE_TRACKER_PHONENUMBTWO, sizeof(uint16) * OEMNV_LOCKMUM_MAXLEN),
   CFGTABLEITEM(CFGI_MOBILE_TRACKER_PHONENUMBTHREE, sizeof(uint16) * OEMNV_LOCKMUM_MAXLEN),
   CFGTABLEITEM(CFGI_MOBILE_TRACKER_CONTECT, sizeof(uint16) * (OEMNV_LOCKMUM_MAXLEN*6)),
   CFGTABLEITEM(CFGI_MOBILE_TRACKER_BACK, sizeof(boolean)),  //type = boolean
#endif
   CFGTABLEITEM(CFGI_KEY_LOCK_CHECK, sizeof(byte)),       //type = byte
   CFGTABLEITEM(CFGI_LOCK_RUIM,  sizeof(boolean)),            //type = boolean
   CFGTABLEITEM(CFGI_LOCK_MCCMNC_LIST,  sizeof(oemi_cache.lock_mccmnc)), 
   
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
   CFGTABLEITEM(CFGI_CALLFORWARD_UNCONDITIONAL_ENABLE,FEATURE_CODE_MAX_LENTH),
   CFGTABLEITEM(CFGI_CALLFORWARD_UNCONDITIONAL_DISABLE,FEATURE_CODE_MAX_LENTH),
   CFGTABLEITEM(CFGI_CALLFORWARD_UNREACHABLE_ENABLE,FEATURE_CODE_MAX_LENTH),
   CFGTABLEITEM(CFGI_CALLFORWARD_UNREACHABLE_DISABLE,FEATURE_CODE_MAX_LENTH),
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
#ifdef FEATURE_CAMERA_MULTI_SENSOR
   ,CFGTABLEITEM(CFGI_CAMERA_MULTI, sizeof(byte))
   ,CFGTABLEITEM(CFGT_CAMERA_FLASH, sizeof(byte))
#endif
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
   ,CFGTABLEITEM(CFGI_MISSED_CALL_ICON, sizeof(boolean)),
   CFGTABLEITEM_READONLY(CFGI_GSENSOR,sizeof(uint32)),
   CFGTABLEITEM_READONLY(CFGI_HEADSET_PRESENT,sizeof(boolean)),
   CFGTABLEITEM(CFGI_FM_BACKGROUND,sizeof(boolean)),
#ifdef FEATURE_VERSION_C337
   CFGTABLEITEM(CFGI_DEFAULTCONT,sizeof(boolean)),
#endif
#ifdef FEATURE_RANDOM_MENU_COLOR
   CFGTABLEITEM(CFGI_MENU_BGCOLOR,sizeof(uint32)),
   CFGTABLEITEM(CFGI_RANDOM_MENU,sizeof(byte)),
#endif
#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add
   CFGTABLEITEM(CFGI_REND_STATE,sizeof(byte)),
#endif//FEATURE_RANDOM_MENU_REND

#ifdef FEATURE_LCD_TOUCH_ENABLE
   CFGTABLEITEM(CFGI_PEN_CAL,sizeof(pen_cal_type)),
#endif//FEATURE_TOUCHPAD
   CFGTABLEITEM(CFGI_BREWSET_USENAME,sizeof(byte)*MAS_BREWSETINT_STRING),
   CFGTABLEITEM(CFGI_BREWSET_PASSWORD,sizeof(byte)*MAS_BREWSETINT_STRING),
   CFGTABLEITEM(CFGI_LANGUAGE_MOD,sizeof(boolean)),
#ifdef FEATURE_ANALOG_TV
   CFGTABLEITEM(CFGI_TV_OR_CAMERA,sizeof(uint32)),
   CFGTABLEITEM(CFGI_TV_SETCHANNL,sizeof(CFG_TvSetting)),
#endif
   //Add By zzg 2010_11_22
   #ifdef FEATURE_APP_BLUETOOTH
   CFGTABLEITEM(CFGI_BT_STATUS, sizeof(boolean)),
   #endif   
   //Add End
   CFGTABLEITEM(CFGI_FLSHLITHG_STATUS, sizeof(boolean)),
   CFGTABLEITEM(CFGI_WMSWRITD_END_STATUS,sizeof(boolean)),
   CFGTABLEITEM(CFGI_FM_PLAY_MODE,sizeof(boolean)),
   CFGTABLEITEM(CFGI_IDLE_DATETIME_MODE,sizeof(byte)),  //ADD BY YANGDECAI 2011-04-13
   CFGTABLEITEM(CFGI_CAMERA_PHOTO_MODE,sizeof(byte)),
#ifdef FEATURE_VERSION_W208S
   CFGTABLEITEM(CFGI_OTKSL_TIMES,sizeof(uint8)),	//Add By zzg 2012_01_18
#endif
   #ifdef FEATURE_LONG_NETLOCK
   CFGTABLEITEM(CFGI_NET_LOCK_FLAGS,sizeof(boolean)),
   #endif
#ifdef FEATURE_USES_MMS   
   CFGTABLEITEM_EMPTY(CFGI_MMSIMAGE) ,
   CFGTABLEITEM_EMPTY(CFGI_MMSSOUND) ,
   CFGTABLEITEM_EMPTY(CFGI_MMSVIDEO) ,
   CFGTABLEITEM(CFGI_MMSDRAFTDATA_INFO, sizeof(MMSData) * MAX_MMS_STORED),
   CFGTABLEITEM(CFGI_MMS_DRAFTCOUNT, sizeof(uint8)),   
   CFGTABLEITEM(CFGI_MMSOUTDATA_INFO, sizeof(MMSData) * MAX_MMS_STORED),
   CFGTABLEITEM(CFGI_MMSINDATA_INFO, sizeof(MMSData) * MAX_MMS_STORED),
   CFGTABLEITEM(CFGI_MMS_OUTCOUNT, sizeof(uint8)),
   CFGTABLEITEM(CFGI_MMS_INCOUNT, sizeof(uint8)),
   CFGTABLEITEM(CFGI_WMS_MMSNOTIFY, sizeof(boolean)),
   CFGTABLEITEM(CFGI_WMS_MMSDELIVERYREPORT, sizeof(int8)),
   CFGTABLEITEM(CFGI_WMS_READREPLY, sizeof(int8)),
   CFGTABLEITEM(CFGI_WMS_REPORTALLOWED, sizeof(int8)),
   CFGTABLEITEM(CFGI_WMS_SENDERVISIBILITY, sizeof(int8)),   
   CFGTABLEITEM_EMPTY(CFGI_MMS_SERVER_ADDRESS) ,
   CFGTABLEITEM(CFGI_MMS_USER_NAME,sizeof(char)*MAS_BREWSETINT_STRING),
   CFGTABLEITEM(CFGI_MMS_PASSWORD,sizeof(char)*MAS_BREWSETINT_STRING),   
   CFGTABLEITEM(CFGI_MMS_PROXY,sizeof(char)*MAX_MMS_PROXY),
   CFGTABLEITEM(CFGI_MMS_PORT,sizeof(char)*MAX_MMS_PORT),     
#endif   
#ifdef FEATURE_VERSION_W208S
   CFGTABLEITEM(CFGI_SMS_RESTRICT_RECEIVE_TOTAL, sizeof(uint8)),
   CFGTABLEITEM(CFGI_SMS_RESTRICT_RECEIVE_INFO, sizeof(sms_restrict_recive_info) * MAX_SMS_RESTRICT),
#endif    
#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)
   CFGTABLEITEM(CFGI_MOBILE_TRACKER_PHONENUMB, sizeof(uint16) * OEMNV_LOCKMUM_MAXLEN),
   CFGTABLEITEM(CFGI_MOBILE_TRACKER_IMSI,sizeof(uint16)*OEMNV_LOCKIMSI_MAXLEN),//CFGI_MOBILE_TRACKER_IMSI
   CFGTABLEITEM(CFGI_MOBILE_TRACKER_SETNUM,sizeof(boolean)),                   //CFGI_MOBILE_TRACKER_SETNUM
   CFGTABLEITEM(CFGI_SMS_TRACKER_SEND_B,sizeof(boolean)),
   CFGTABLEITEM(CFGI_SMS_TRACKER_TIME,sizeof(uint16)),                         //CFGI_SMS_TRACKER_TIME
   CFGTABLEITEM(CFGI_SMS_TRACKER_NUMBER, sizeof(char) * OEMNV_LOCKMUM_MAXLEN),   //CFGI_SMS_TRACKER_NUMBER
#endif
#ifdef FEATURE_VERSION_C337
   CFGTABLEITEM(CFGI_MIZONE_NUM,sizeof(char)*MAS_BREWSETINT_STRING),
   CFGTABLEITEM(CFGI_MIZONE_SMSINFO,sizeof(char)*MAS_BREWSETINT_STRING), 
#endif
#if defined(FEATURE_VERSION_C316) || defined(FEATURE_VERSION_W317A)
   CFGTABLEITEM(CFGI_AUTOCALLRECORD,sizeof(boolean)),				 //Add by pyuangui 20121231
#endif
#ifdef FEATURE_VERSION_C316
   CFGTABLEITEM(CFGI_ONEKEY_LOCK_KEYPAD,sizeof(boolean)),
#endif
   //CFGTABLEITEM(CFGI_SALES_TRACK_SMS_SEND, sizeof(boolean)),		//Add By zzg 2012_10_29
};
#endif

////
//  The table containing all the config tables.
static ConfigTableInfo const cfgTable[] =
{
   { customItemTable,      ARR_SIZE(customItemTable)      },
   { customStateItemTable, ARR_SIZE(customStateItemTable) }
#ifdef CUST_EDITION
  ,{ customOEMItemTable,   ARR_SIZE(customOEMItemTable)   }
#endif
};

/* Cache initialized ? */
static boolean cache_initialized = FALSE;
static boolean cfgi_lcd;

/*===========================================================================

                     FUNCTION DEFINITIONS

============================================================================*/

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
   boolean bRet = FALSE;
   nv_item_type nvi;
      
   if (NV_DONE_S == OEMNV_Get(NV_FTM_MODE_I, &nvi)) {
      bRet = nvi.ftm_mode ? TRUE : FALSE;
   }
   return bRet;
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
#ifndef CUST_EDITION
   disp_info_type di = disp_get_info();
#endif
   // Reset the OEM Configuration list
   //
   // NOTE: Any changes to these values should be mirrored in the
   //       initialization of the oemi_cache variable.
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

   oemi_cache.sms_service_option  = OEMNV_SMS_SO_SO6;
   oemi_cache.sms_is41_workaround = OEMNV_SMS_EMAIL_ADDRESSING_IS41;
#ifdef FEATURE_VERSION_W208S
	oemi_cache.ppp_auth_enabled	  = OEMNV_PPP_AUTH_DISABLED;
#else
   oemi_cache.ppp_auth_enabled	  = OEMNV_PPP_AUTH_ENABLED;
#endif
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
   oemi_cache.sms_retry_period          = OEMNV_SMS_RETRY_PERIOD;
   oemi_cache.sms_retry_interval        = OEMNV_SMS_RETRY_INTERVAL;
   oemi_cache.sms_gcf_flag              = FALSE;
#ifdef CUST_EDITION
   WSTRCPY(oemi_cache.voicemail_number,OEMNV_VOICEMAIL_NUMBER);
   nvi.sms_vm_number.num_digits = WSTRLEN(oemi_cache.voicemail_number);
   WSTRTOSTR(oemi_cache.voicemail_number, (char *)nvi.sms_vm_number.digits, nvi.sms_vm_number.num_digits+1);
   OEMNV_Put(NV_SMS_VM_NUMBER_I, &nvi);
#else
   MEMSET((void *) oemi_cache.voicemail_number,
          0,
          sizeof(oemi_cache.voicemail_number));
#endif
   MEMSET((void *) oemi_cache.voicemail_number_cdma,
          0,
          sizeof(oemi_cache.voicemail_number_cdma));
   MEMSET((void *) oemi_cache.voicemail_number_gw,
          0,
          sizeof(oemi_cache.voicemail_number_gw));
#ifndef CUST_EDITION
   oemi_cache.time_format         = OEMNV_TIMEFORM_AMPM;
#endif
   MEMSET((void *)&oemi_cache.cug, 0, sizeof(OEMConfigCugInfo));

   oemi_cache.vr = 0;

   MEMSET((void *)&oemi_cache.download_info, 0, sizeof(OEMConfigDownloadInfo));
   oemi_cache.download_info.dwCID  = DEFAULT_BREW_CARRIER_ID;
   oemi_cache.download_info.dwPID  = DEFAULT_BREW_PLATFORM_ID;
   oemi_cache.download_info.wFlags = DEFAULT_BREW_DOWNLOAD_FLG;
   oemi_cache.download_info.wAuth  = DEFAULT_BREW_APOLICY;
   oemi_cache.download_info.wPrivP = DEFAULT_BREW_PPOLICY;
   MEMSET(oemi_cache.download_info.bBKey,0,OEMCFG_DL_BKEY_SIZE);
   MEMSET(oemi_cache.download_info.bAKey,0,OEMCFG_DL_AKEY_SIZE);

   STRLCPY((char *)oemi_cache.download_info.szServer, 
           (const char *)DEFAULT_BREW_SERVER, 
           sizeof(oemi_cache.download_info.szServer));
   oemi_cache.disable_in_call_disp = FALSE;
   oemi_cache.disable_bg_image = FALSE;
   oemi_cache.manual_plmn_sel_allowed = TRUE;
   STRCPY((char *)oemi_cache.brewsetings_usename,DEFAULT_BREW_USERNAME);
   STRCPY((char *)oemi_cache.brewsetings_password,DEFAULT_BREW_PASSWORD);
#ifdef CUST_EDITION
#ifdef FEATURE_KEYGUARD	 

#ifdef FEATURE_PEKTEST
    oemi_cache.b_key_lock       =  0;
#elif defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM) ||defined(FEATURE_VERSION_C01)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)
	oemi_cache.b_key_lock       =  1; 
#elif defined(FEATURE_VERSION_K202_LM129C)//xxzhen
	oemi_cache.b_key_lock       =  2; 
#elif defined(FEATURE_VERSION_W208S)
    oemi_cache.b_key_lock       =  4; 
#else
	oemi_cache.b_key_lock       =  0;
#endif

#endif
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
   MEMSET((void *)&oemi_cache.m_ip_number, 0 ,sizeof(IP_Number_Cfg));
   oemi_cache.m_b_ip_pound = TRUE;
#endif

   oemi_cache.b_phonebook_lock         = FALSE;
   oemi_cache.b_recentcall_lock        = FALSE;
   oemi_cache.b_sms_lock               = FALSE;

   //Add By zzg 2010_08_21
   oemi_cache.b_calendar_lock          = FALSE;
   //Add End
   
#if defined(FEATURE_VERSION_W317A) || defined(FEATURE_VERSION_C316)  
   oemi_cache.b_mediagallery_lock      = FALSE;
#endif 

#if defined(FEATURE_VERSION_C316)  
   oemi_cache.b_multimedia_lock         = FALSE;
#endif 


#ifdef FEATURE_VERSION_C337
	oemi_cache.m_defaultcont		   = FALSE;
#endif
   
#if defined(FEATURE_CARRIER_VENEZUELA_MOVILNET)
    nvi_cache.key_tone_length = OEMNV_KEYTONE_LONG;
    nvi.cont_key_dtmf = TRUE;
    (void) OEMNV_Put( NV_CONT_KEY_DTMF_I, &nvi);
#elif defined (FEATURE_VERSION_W208S)
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

#ifdef FEATURE_SET_AUTOKEYGUARD
   oemi_cache.autokey_guard            = OEMNV_AKG_OFF;
#endif
   oemi_cache.roaming_alert            = OEMNV_ALERT_ENABLE;

   oemi_cache.minute_alert             = OEMNV_ALERT_DISABLE;
#ifdef CUST_EDITION
   oemi_cache.any_key_answer           = OEM_AUTO_ANSWER_MODE;
#else
   oemi_cache.any_key_answer           = FALSE;
#endif
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
   MEMCPY(oemi_cache.s_startupani,OEMNV_STARTUPANI, AEE_MAX_FILE_NAME/*FILESPECLEN*/);
   
   //关机动画
   MEMCPY(oemi_cache.s_poweroffani,OEMNV_POWEROFFANI, AEE_MAX_FILE_NAME/*FILESPECLEN*/); 
#endif
   //桌面墙纸
   MEMCPY(oemi_cache.s_wallpaper,OEMNV_WALLPAPER, AEE_MAX_FILE_NAME/*FILESPECLEN*/); 
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
#ifdef FEATURE_USES_MMS   
   MEMCPY(oemi_cache.s_MMSImage,OEMNV_MMSIMAGE, AEE_MAX_FILE_NAME/*FILESPECLEN*/); 
   MEMCPY(oemi_cache.s_MMSSOUND,OEMNV_MMSSOUND, AEE_MAX_FILE_NAME/*FILESPECLEN*/); 
   MEMCPY(oemi_cache.s_MMSVIDEO,OEMNV_MMSVIDEO, AEE_MAX_FILE_NAME/*FILESPECLEN*/); 
   {
        uint8 index = 0; 
        for(; index < MAX_MMS_STORED; index++)
        {
            MEMSET(oemi_cache.MMSInDataInfo[index].phoneNumber, 0, 32);
            MEMSET(oemi_cache.MMSInDataInfo[index].MMSDataFileName, 0, AEE_MAX_FILE_NAME);
            oemi_cache.MMSInDataInfo[index].MMSDatasize = 0;
            oemi_cache.MMSInDataInfo[index].MMSDataReaded = FALSE;

            MEMSET(oemi_cache.MMSOutDataInfo[index].phoneNumber, 0, 32);
            MEMSET(oemi_cache.MMSOutDataInfo[index].MMSDataFileName, 0, AEE_MAX_FILE_NAME);
            oemi_cache.MMSOutDataInfo[index].MMSDatasize = 0;
            oemi_cache.MMSOutDataInfo[index].MMSDataReaded = FALSE;

            MEMSET(oemi_cache.MMSDraftDataInfo[index].phoneNumber, 0, 32);
            MEMSET(oemi_cache.MMSDraftDataInfo[index].MMSDataFileName, 0, AEE_MAX_FILE_NAME);
            oemi_cache.MMSDraftDataInfo[index].MMSDatasize = 0;
            oemi_cache.MMSDraftDataInfo[index].MMSDataReaded = FALSE;            
        }   
        oemi_cache.mmsNotify = TRUE;
        oemi_cache.mmsDeliverReport = -1;
        oemi_cache.mmsReadReply = -1;
        oemi_cache.mmsReportAllowed = -1;
        oemi_cache.mmsSenderVisibility = -1;
   }
   MEMCPY(oemi_cache.s_MMSServerAddress,"http://mms.movilnet.com.ve", MAX_URL_LEN/*FILESPECLEN*/); 
   MEMCPY(oemi_cache.s_MMSUserName,"", MAS_BREWSETINT_STRING/*FILESPECLEN*/); 
   MEMCPY(oemi_cache.s_MMSPassword,"", MAS_BREWSETINT_STRING/*FILESPECLEN*/);    
   MEMCPY(oemi_cache.s_MMSProxy,OEMNV_MMSPROXY, MAX_MMS_PROXY/*FILESPECLEN*/); 
   MEMCPY(oemi_cache.s_MMSPort,OEMNV_MMSPORT, MAX_MMS_PORT/*FILESPECLEN*/);    
   oemi_cache.mmsInCount = 0;
   oemi_cache.mmsOutCount = 0;
   oemi_cache.mmsDraftCount = 0;
   {
#ifdef FEATURE_VERSION_W208S
        uint8 index = 0;
        for(; index < MAX_SMS_RESTRICT; ++index)
        {
            MEMSET(oemi_cache.sms_restrict_recive[index].szName, 0, 32+1);
        }
        oemi_cache.sms_restrict_receive_total = 0;
#endif        
   }
#endif  
	#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)
	MEMSET(oemi_cache.mobile_tracker_number,0,OEMNV_LOCKMUM_MAXLEN);
	MEMSET(oemi_cache.mobile_tracker_imsi,0,OEMNV_LOCKIMSI_MAXLEN);//CFGI_MOBILE_TRACKER_IMSI
    oemi_cache.mobile_tracker_setnumb =FALSE;                      //CFGI_MOBILE_TRACKER_SETNUM
    oemi_cache.sms_tarcker_b = FALSE;                              //CFGI_SMS_TRACKER_SEND_B 
    #ifdef FEATURE_VERSION_C337
	oemi_cache.sms_tarcker_time_uint = 10;                      //CFGI_SMS_TRACKER_TIME
	#else
    oemi_cache.sms_tarcker_time_uint = 240;                      //CFGI_SMS_TRACKER_TIME
	#endif
    MEMCPY(oemi_cache.sms_tracker_number,OEMNV_DEFAULTNUMBER, OEMNV_LOCKMUM_MAXLEN/*FILESPECLEN*/); //CFGI_SMS_TRACKER_NUMBER
	#endif
	#ifdef FEATURE_VERSION_C316
	oemi_cache.b_mobiletracker_lock = FALSE;
	oemi_cache.mobiletracker_password = OEMNV_PHONE_PASSWORD;
	oemi_cache.mobiletracker_password_check = TRUE;
	MEMSET(oemi_cache.mobile_tracker_number_2,0,OEMNV_LOCKMUM_MAXLEN);
	MEMSET(oemi_cache.mobile_tracker_number_3,0,OEMNV_LOCKMUM_MAXLEN);
	WSTRCPY(oemi_cache.mobile_tracker_content,L"Mobile Tracker Alert!:The sender of this SMS is using your phone.");
	oemi_cache.b_mobile_tracker_back = FALSE;
	#endif
    #ifdef FEATURE_VERSION_C337
    MEMCPY(oemi_cache.mizone_num,OEMNV_MIZONENUM, MAS_BREWSETINT_STRING/*FILESPECLEN*/); 
    MEMCPY(oemi_cache.mizone_smsinfo,0, MAS_BREWSETINT_STRING/*FILESPECLEN*/); 
    //MEMSET(oemi_cache.mizone_num,OEMNV_MIZONENUM,OEMNV_LOCKIMSI_MAXLEN);
    //MEMSET(oemi_cache.mizone_smsinfo,0,OEMNV_LOCKIMSI_MAXLEN);
    #endif
	#if defined(FEATURE_VERSION_C316) || defined(FEATURE_VERSION_W317A)
    oemi_cache.autocallrecord = FALSE;  //CFGI_AUTOCALLRECORD    //Add by pyuangui 2013-01-09 
    #endif
   //屏保时间
   oemi_cache.p_screensaver_time=0; 
   oemi_cache.restrict_incoming = 0;
   oemi_cache.restrict_outgoing = 0;

   oemi_cache.fmRadio_volume = MAX_FMRADIO_VOLUME/5*3;     
#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM) 
   {
       oemi_cache.fmRadio_chan_total = 20;
       WSTRCPY(oemi_cache.fmRadio_chan_info[0].szName ,L"88");
       oemi_cache.fmRadio_chan_info[0].wChannel = 5;
       WSTRCPY(oemi_cache.fmRadio_chan_info[1].szName ,L"88.5");
       oemi_cache.fmRadio_chan_info[1].wChannel = 10;       
       WSTRCPY(oemi_cache.fmRadio_chan_info[2].szName ,L"89.5");
       oemi_cache.fmRadio_chan_info[2].wChannel = 20;  
       WSTRCPY(oemi_cache.fmRadio_chan_info[3].szName ,L"91");
       oemi_cache.fmRadio_chan_info[3].wChannel = 35; 
       WSTRCPY(oemi_cache.fmRadio_chan_info[4].szName ,L"91.5");
       oemi_cache.fmRadio_chan_info[4].wChannel = 40;  
       WSTRCPY(oemi_cache.fmRadio_chan_info[5].szName ,L"93");
       oemi_cache.fmRadio_chan_info[5].wChannel = 55;   
       WSTRCPY(oemi_cache.fmRadio_chan_info[6].szName ,L"94");
       oemi_cache.fmRadio_chan_info[6].wChannel = 65;    
       WSTRCPY(oemi_cache.fmRadio_chan_info[7].szName ,L"94.5");
       oemi_cache.fmRadio_chan_info[7].wChannel = 70;   
       WSTRCPY(oemi_cache.fmRadio_chan_info[8].szName ,L"95");
       oemi_cache.fmRadio_chan_info[8].wChannel = 75;  
       WSTRCPY(oemi_cache.fmRadio_chan_info[9].szName ,L"95.5");
       oemi_cache.fmRadio_chan_info[9].wChannel = 80;
       WSTRCPY(oemi_cache.fmRadio_chan_info[10].szName ,L"96");
       oemi_cache.fmRadio_chan_info[10].wChannel = 85;  
       WSTRCPY(oemi_cache.fmRadio_chan_info[11].szName ,L"97.5");
       oemi_cache.fmRadio_chan_info[11].wChannel = 100;
       WSTRCPY(oemi_cache.fmRadio_chan_info[12].szName ,L"98.5");
       oemi_cache.fmRadio_chan_info[12].wChannel = 110;   
       WSTRCPY(oemi_cache.fmRadio_chan_info[13].szName ,L"100");
       oemi_cache.fmRadio_chan_info[13].wChannel = 125;   
       WSTRCPY(oemi_cache.fmRadio_chan_info[14].szName ,L"102.5");
       oemi_cache.fmRadio_chan_info[14].wChannel = 150;
       WSTRCPY(oemi_cache.fmRadio_chan_info[15].szName ,L"103");
       oemi_cache.fmRadio_chan_info[15].wChannel = 155;      
       WSTRCPY(oemi_cache.fmRadio_chan_info[16].szName ,L"103.5");
       oemi_cache.fmRadio_chan_info[16].wChannel = 160;
       WSTRCPY(oemi_cache.fmRadio_chan_info[17].szName ,L"105.5");
       oemi_cache.fmRadio_chan_info[17].wChannel = 180;
       WSTRCPY(oemi_cache.fmRadio_chan_info[18].szName ,L"106.5");
       oemi_cache.fmRadio_chan_info[18].wChannel = 190;
       WSTRCPY(oemi_cache.fmRadio_chan_info[19].szName ,L"107.5");
       oemi_cache.fmRadio_chan_info[19].wChannel = 200;        
   }
#else
   MEMSET((void *)&oemi_cache.fmRadio_chan_info, 0, sizeof(sChanInfo) * MAX_FMRADIO_STORED_CHANNEL);
   oemi_cache.fmRadio_chan_total = 0;
#endif   

#ifdef FEATURE_VERSION_W208S
    oemi_cache.emerg_table.emert_size = OEMNV_EMERT_SEZE;
    
    oemi_cache.emerg_table.emerg_num[0].num_len = OEMNV_EMERG_NUM_LEN_ONE;
    STRCPY(oemi_cache.emerg_table.emerg_num[0].num_buf,OEMNV_EMERG_NUM_ONE);

    oemi_cache.emerg_table.emerg_num[1].num_len = OEMNV_EMERG_NUM_LEN_TWO;
    STRCPY(oemi_cache.emerg_table.emerg_num[1].num_buf,OEMNV_EMERG_NUM_TWO);
    
    oemi_cache.emerg_table.emerg_num[2].num_len = OEMNV_EMERG_NUM_LEN_TRE;
    STRCPY(oemi_cache.emerg_table.emerg_num[2].num_buf,OEMNV_EMERG_NUM_TRE); 
#else
    oemi_cache.emerg_table.emert_size = OEMNV_EMERT_SEZE;
    oemi_cache.emerg_table.emerg_num[0].num_len = OEMNV_EMERG_NUM_LEN;
    STRCPY(oemi_cache.emerg_table.emerg_num[0].num_buf,OEMNV_EMERG_NUM_ONE);
#if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
#else
    oemi_cache.emerg_table.emerg_num[1].num_len = OEMNV_EMERG_NUM_LEN;
    STRCPY(oemi_cache.emerg_table.emerg_num[1].num_buf,OEMNV_EMERG_NUM_TWO);
    oemi_cache.emerg_table.emerg_num[2].num_len = OEMNV_EMERG_NUM_LEN;
    STRCPY(oemi_cache.emerg_table.emerg_num[2].num_buf,OEMNV_EMERG_NUM_TRE);
#if !(defined(FEATURE_VERSION_CITYCELL) || defined(FEATURE_VERSION_M8P))
    oemi_cache.emerg_table.emerg_num[3].num_len = OEMNV_EMERG_NUM_LEN;
    STRCPY(oemi_cache.emerg_table.emerg_num[3].num_buf,OEMNV_EMERG_NUM_FOR);
#endif
#endif  

#endif

                                   
   STRCPY(oemi_cache.BUSY_ENABLE,OEMNV_CALLFORWARD_BUSY_ENABLE);   //CFGI_CALLFORWARD_BUSY_ENABLE
   STRCPY(oemi_cache.BUSY_DISABLE,OEMNV_CALLFORWARD_BUSY_DISABLE);  //CFGI_CALLFORWARD_BUSY_DISABLE
   STRCPY(oemi_cache.NOANSWER_ENABLE,OEMNV_CALLFORWARD_NOANSWER_ENABLE);  //CFGI_CALLFORWARD_NOANSWER_ENABLE
   STRCPY(oemi_cache.NOANSWER_DISABLE,OEMNV_CALLFORWARD_NOANSWER_DISABLE);  //CFGI_CALLFORWARD_NOANSWER_DISABLE
   STRCPY(oemi_cache.UNCONDITIONAL_ENABLE,OEMNV_CALLFORWARD_UNCONDITIONAL_ENABLE);  //CFGI_CALLFORWARD_UNCONDITIONAL_ENABLE
   STRCPY(oemi_cache.UNCONDITIONAL_DISABLE,OEMNV_CALLFORWARD_UNCONDITIONAL_DISABLE); //CFGI_CALLFORWARD_UNCONDITIONAL_DISABLE
   STRCPY(oemi_cache.UNREACHABLE_ENABLE,OEMNV_CALLFORWARD_UNREACHABLE_ENABLE);  //CFGI_CALLFORWARD_UNREACHABLE_ENABLE
   STRCPY(oemi_cache.UNREACHABLE_DISABLE,OEMNV_CALLFORWARD_UNREACHABLE_DISABLE); //CFGI_CALLFORWARD_UNREACHABLE_DISABLE
   STRCPY(oemi_cache.DISABLE_ALL,OEMNV_CALLFORWARD_DISABLE_ALL);  //CFGI_CALLFORWARD_DISABLE_ALL
   STRCPY(oemi_cache.WAIT_ENABLE,OEMNV_CALLFORWARD_WAIT_ENABLE);  //CFGI_CALLFORWARD_WAIT_ENABLE
   STRCPY(oemi_cache.WAIT_DISABLE,OEMNV_CALLFORWARD_WAIT_DISABLE);  //CFGI_CALLFORWARD_WAIT_DISABLE
   STRCPY(oemi_cache.VOICEMAIL_ENABLE,OEMNV_CALLFORWARD_VOICEMAIL_ENABLE); //CFGI_CALLFORWARD_VOICEMAIL_ENABLE	
   oemi_cache.input_mode=OEMNV_INPUTMODE_DEFAULT;
#ifdef FEATURE_MENU_STYLE
   oemi_cache.menu_style= OEMNV_MENU_STYLE_ICON;
#endif
#ifdef FEATURE_CAMERA_MULTI_SENSOR
   oemi_cache.camera_multi = OEMNV_CAMERA_MULTI_TWO;
   oemi_cache.camera_flash = OEMNV_CAMERA_FLASH_OFF;
#endif
   oemi_cache.camera_enviroment = OEMNV_CAMERA_ENVIR_AUTO;
   oemi_cache.camera_quality = OEMNV_CAMERA_QUALITY_HIGH;
   oemi_cache.camera_size = OEMNV_CAMERA_SIZE_DEFAULT;
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

   
#ifdef FEATURE_RANDOM_MENU_REND//wlh 20090405 add for rend
#if defined (FEATURE_VERSION_C337) || defined (FEATURE_VERSION_C316)
     oemi_cache.m_nrendstate = DISPLAYREND_TYPE_MAX;
#else
     oemi_cache.m_nrendstate = DISPLAYREND_TYPE_ONEBYONE;
#endif 
#endif

   //Add By zzg 2010_10_22
#ifdef FEATURE_APP_BLUETOOTH
    oemi_cache.bt_status = 0; 
#endif
   oemi_cache.flashlight_status = 0;
   oemi_cache.wmswriteend_status = 1;
   oemi_cache.FMPlayMode_status = 1;
   oemi_cache.Idle_datetime_mode = 1;
   oemi_cache.Camera_Photo_mode =1;
   oemi_cache.otksl_times = 1;
   //Add End
#ifdef FEATURE_PLANEMODE
   oemi_cache.planeMode = OEMNV_PLANEMODE_OFF;
#endif
#endif //CUST_EDITION
   OEMPriv_WriteOEMConfigList();

   // Backlight with external pwr
#ifndef WIN32
   nvi.back_light_hfk = OEMNV_EXTPWR_BL_ON;
   (void) OEMNV_Put( NV_BACK_LIGHT_HFK_I, &nvi );
   nvi_cache.backlight_hfk = OEMNV_EXTPWR_BL_ON;
   
   // CFGI_RINGER_VOL:
   nvi.ringer_level = OEMNV_VOLUME_MAX;
   (void) OEMNV_Put( NV_RINGER_LVL_I, &nvi );
   nvi_cache.ringer_level = OEMNV_VOLUME_MAX;

   // CFGI_EAR_VOL:
   nvi.ear_level = OEMNV_VOLUME_HIGH;
   (void) OEMNV_Put( NV_EAR_LVL_I, &nvi );
   nvi_cache.handset_ear_level = OEMNV_VOLUME_HIGH;

   // CFGI_BEEP_VOL:
#ifdef FEATURE_VERSION_W317A
	nvi.beep_level = OEMNV_VOLUME_OFF;
   (void) OEMNV_Put( NV_BEEP_LVL_I, &nvi );
   nvi_cache.beep_level = OEMNV_VOLUME_OFF;
#else
   nvi.beep_level = OEMNV_VOLUME_LOW;
   (void) OEMNV_Put( NV_BEEP_LVL_I, &nvi );
   nvi_cache.beep_level = OEMNV_VOLUME_LOW;
#endif

#ifdef FEATURE_SMART_SOUND
   // CFGI_SMART_SOUND:
   nvi.auto_volume_enabled = OEMNV_SMARTSOUND_DISABLE;
   (void) OEMNV_Put( NV_AUTO_VOLUME_ENABLED_I, &nvi );
   nvi_cache.smart_sound = OEMNV_SMARTSOUND_DISABLE;
   OEMSound_SmartSoundEnable(FALSE);
#endif

   // CFGI_RINGER_TYPE:
   nvi.ringer_type = (byte) SND_ALERT;
   (void) OEMNV_Put( NV_RINGER_TYPE_I, &nvi );
   nvi_cache.ringer_type = (byte) SND_ALERT;

   // CFGI_KEYBEEP_SOUND:
   nvi.key_sound = (nv_key_sound_enum_type) OEMNV_KEYBEEP_TONE;
   (void) OEMNV_Put( NV_KEY_SOUND_I, &nvi );
   nvi_cache.key_sound = OEMNV_KEYBEEP_TONE;

   // LCD contrast
#ifdef CUST_EDITION
   nvi.lcd = OEMNV_CONTRAST_LEVEL_4;
   (void) OEMNV_Put( NV_LCD_I, &nvi );
   nvi_cache.contrast = OEMNV_CONTRAST_LEVEL_4;
#else
   nvi.lcd = di.contrast_default;
   (void) OEMNV_Put( NV_LCD_I, &nvi );
   nvi_cache.contrast = di.contrast_default;
#endif
   // Menu Format
   nvi.menu_format = (nv_menu_format_enum_type) OEMNV_MENUFORM_LARGE;
   (void) OEMNV_Put( NV_MENU_FORMAT_I, &nvi );
   nvi_cache.menu_format = (byte) OEMNV_MENUFORM_LARGE;

#ifdef FEATURE_PEKTEST
   nvi.back_light = OEMNV_BL_30S;
   (void) OEMNV_Put( NV_BACK_LIGHT_I, &nvi );
   nvi_cache.backlight = OEMNV_BL_30S;
#elif defined(FEATURE_CARRIER_THAILAND_CAT)
   nvi.back_light = OEMNV_BL_30S;
   (void) OEMNV_Put( NV_BACK_LIGHT_I, &nvi );
   nvi_cache.backlight = OEMNV_BL_30S;
#else
   // NV_BACK_LIGHT_I
   #ifdef FEATURE_VERSION_W208S //add pandy 2011-11-04
   nvi.back_light = OEMNV_BL_7S;
   (void) OEMNV_Put( NV_BACK_LIGHT_I, &nvi );
   nvi_cache.backlight = OEMNV_BL_7S;
   #elif defined (FEATURE_VERSION_C337)
   nvi.back_light = OEMNV_BL_30S;
   (void) OEMNV_Put( NV_BACK_LIGHT_I, &nvi );
   nvi_cache.backlight = OEMNV_BL_30S;
   #else
   nvi.back_light = OEMNV_BL_10S;
   (void) OEMNV_Put( NV_BACK_LIGHT_I, &nvi );
   nvi_cache.backlight = OEMNV_BL_10S;
   #endif
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

#ifdef FEATURE_WCDMA
#error code not present
#else
   nvi.dtmf = OEMNV_DTMFBURST; // Burst
#endif
   (void) OEMNV_Put( NV_DTMF_I, &nvi);
   nvi_cache.beep_length = nvi.dtmf;

#ifdef FEATURE_TTY
   nvi.tty = (byte)OEMNV_TTY_OFF;
   (void) OEMNV_Put( NV_TTY_I, &nvi);
   nvi_cache.tty = (OEMTTY)nvi.tty;
#endif  /* FEATURE_TTY */

   nvi.game_mode_persist = 0;
   (void) OEMNV_Put( NV_GAME_MODE_PERSIST_I, &nvi);
   nvi_cache.game_mode_persist = nvi.game_mode_persist;

   nvi.game_mode = 0;
   (void) OEMNV_Put( NV_GAME_MODE_I, &nvi);
   nvi_cache.game_mode = nvi.game_mode;

   nvi.manual_plmn_sel = 0;
   (void) OEMNV_Put( NV_MANUAL_PLMN_SEL_I, &nvi);
   nvi_cache.manual_plmn_sel = nvi.manual_plmn_sel;

#ifdef FEATURE_GSTK_CS_PS_PARALLEL
   nvi.toolkit_cs_ps_parallel = FALSE;
   (void) OEMNV_Put( NV_TOOLKIT_CS_PS_PARALLEL_I, &nvi);
   nvi_cache.toolkit_cs_ps_parallel = nvi.toolkit_cs_ps_parallel;
#endif /* FEATURE_GSTK_CS_PS_PARALLEL */

#ifdef FEATURE_CCBS
#error code not present
#endif

   nvi.display_duration = 0;
   (void) OEMNV_Put( NV_DISPLAY_DURATION_I, &nvi);
   nvi_cache.display_duration = nvi.display_duration;

   nvi.back_light_intensity = 0;
   (void) OEMNV_Put( NV_BACK_LIGHT_INTENSITY_I, &nvi);
   nvi_cache.backlight_intensity = nvi.back_light_intensity;

#if defined(FEATURE_USRSYSCLOCK) || defined(FEATURE_ISYSCLOCK)
    nvi.auto_time_enable = 0;
   (void) OEMNV_Put( NV_AUTO_TIME_ENABLE_I, &nvi);
   nvi_cache.auto_time_enable = nvi.auto_time_enable;
   
   memset((void *) &nvi.nitz_nw_info, 0 , sizeof (nvi.nitz_nw_info));
   memset( (void *) &nvi_cache.plmn_name, 0 , sizeof (nvi_cache.plmn_name));
   (void) OEMNV_Put( NV_NITZ_NW_INFO_I, &nvi);

   nvi.nitz_nw_info_mcc = nvi_cache.mcc = 0;
   (void) OEMNV_Put( NV_NITZ_NW_INFO_MCC_I, &nvi);

   nvi.nitz_nw_info_mnc = nvi_cache.mnc = 0;
   (void) OEMNV_Put( NV_NITZ_NW_INFO_MNC_I, &nvi);
#endif 
   
   nvi.voice_priv = CM_PRIVACY_PREF_STANDARD;//VOICEPRIVACY_STD;
   (void) OEMNV_Put( NV_VOICE_PRIV_I, &nvi);
   nvi_cache.voice_priv = nvi.voice_priv;  
#ifdef CUST_EDITION
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
#if defined (FEATURE_VERSION_W208S) || defined (FEATURE_VERSION_C337)
   nvi.set_time_format = NV_SET_TIME_FORMAT_12_HOUR;
   (void) OEMNV_Put( NV_SET_TIME_FORMAT_I, &nvi);
   nvi_cache.set_time_format = (byte)NV_SET_TIME_FORMAT_12_HOUR;
#else
	
   nvi.set_time_format = NV_SET_TIME_FORMAT_24_HOUR;
   (void) OEMNV_Put( NV_SET_TIME_FORMAT_I, &nvi);
   nvi_cache.set_time_format = (byte)NV_SET_TIME_FORMAT_24_HOUR;
#endif   
#ifdef FEATURE_TIME_DATA_SETTING
#ifdef FEATURE_VERSION_K202_LM129C //xxzhen
    nvi.set_date_format = NV_SET_DATE_FORMAT_YYYY_MM_DD_1;
    (void) OEMNV_Put( NV_SET_DATE_FORMAT_I, &nvi);
    nvi_cache.set_date_format = (byte)NV_SET_DATE_FORMAT_YYYY_MM_DD_1;
#else
    nvi.set_date_format = NV_SET_DATE_FORMAT_MM_DD_YYYY;
    (void) OEMNV_Put( NV_SET_DATE_FORMAT_I, &nvi);
    nvi_cache.set_date_format = (byte)NV_SET_DATE_FORMAT_MM_DD_YYYY;
#endif
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
   
   //CFGI_VOICEPRIVACY
   nvi.voice_priv = CM_PRIVACY_PREF_STANDARD;
   (void) OEMNV_Put( NV_VOICE_PRIV_I, &nvi);
   nvi_cache.voice_priv = CM_PRIVACY_PREF_STANDARD;
#endif//#ifndef win32   
#ifdef FEATURE_SUPPORT_BT_APP
#ifndef WIN32
   bt_ui_restore_set(); 
#endif
#endif
   OEMFS_Remove( ALARM_EFS_FILE);
   
#ifdef FEATURE_DUAL_UIMCARD
   nvi.sim_select = 0;
   (void) OEMNV_Put( NV_SIM_SELECT_I, &nvi);
#endif

   {
       extern void ClockApps_ClearAll(void);
       DBGPRINTF( ";restorefactory, remove alarm file success");
#ifndef WIN32//wlh 临时修改
       ClockApps_ClearAll();
#endif//WIN32
   }
#ifndef WIN32//wlh 临时修改
   {
#ifdef FEATURE_APP_STOPWATCH   
	   extern void StopWatch_ClearData(void);	   
	   StopWatch_ClearData();
#endif	   
   }
#endif//WIN32
    {
        IShell *piShell = AEE_GetShell();
        if(piShell)
            ISHELL_PostURL(piShell,"brw_app:ResetSetting");
    }	
	
   //oemi_cache.input_mode=OEMNV_INPUTMODE_DEFAULT;
#endif //CUST_EDITION

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
  OEMPriv_ReadOEMConfigList();
#ifdef CUST_EDITION
#ifndef WIN32  
  wms_msg_setmochannel(oemi_cache.wms_mochannel);
#endif
#endif // CUST_EDITION
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

  (void) OEMNV_Get( NV_BACK_LIGHT_I, &nvi );
  nvi_cache.backlight = nvi.back_light;

  (void) OEMNV_Get( NV_BACK_LIGHT_INTENSITY_I, &nvi );
  nvi_cache.backlight_intensity = nvi.back_light_intensity;

  (void) OEMNV_Get( NV_LCD_I, &nvi );
  nvi_cache.contrast = nvi.lcd;

  (void) OEMNV_Get( NV_DISPLAY_DURATION_I, &nvi );
  nvi_cache.display_duration = nvi.display_duration;

  (void) OEMNV_Get( NV_LANGUAGE_SELECTION_I, &nvi );
#ifdef CUST_EDITION
  nvi_cache.language = nvi.language_selection;
#else
  nvi_cache.language = (byte) nvi.language_selection;
#endif

  (void) OEMNV_Get( NV_BANNER_I, &nvi );

#ifdef FEATURE_VERSION_W208S
  memset(nvi_cache.banner, 0, sizeof(nvi_cache.banner));
#else
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
#endif

  

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
  if( OEMNV_Get( NV_TTY_I, &nvi) != NV_DONE_S )
  {
   nvi.tty = (byte)OEMNV_TTY_OFF;
   (void) OEMNV_Put( NV_TTY_I, &nvi);
  }
  nvi_cache.tty  = (OEMTTY)nvi.tty;

  //game mode
  if( OEMNV_Get( NV_GAME_MODE_PERSIST_I, &nvi) != NV_DONE_S )
  {
   nvi.game_mode_persist = 0;
   (void) OEMNV_Put( NV_GAME_MODE_PERSIST_I, &nvi);
  }
  nvi_cache.game_mode_persist  = nvi.game_mode_persist;

  if( OEMNV_Get( NV_GAME_MODE_I, &nvi) != NV_DONE_S )
  {
   nvi.game_mode = 0;
   (void) OEMNV_Put( NV_GAME_MODE_I, &nvi);
  }
  
  //only set game mode to the NV value if persistent is on
  if (nvi_cache.game_mode_persist) {
    nvi_cache.game_mode  = nvi.game_mode;
  } else {
    nvi_cache.game_mode = 0;
  }

  // Data Call Counters
  (void) OEMNV_Get( NV_LAST_RX_DATA_COUNT_I, &nvi);
  nvi_cache.last_rx_data_count = nvi.last_rx_data_count;

  (void) OEMNV_Get( NV_LAST_TX_DATA_COUNT_I, &nvi);
  nvi_cache.last_tx_data_count = nvi.last_tx_data_count;

  (void) OEMNV_Get( NV_TOTAL_RX_DATA_COUNT_I, &nvi);
  nvi_cache.total_rx_data_count = nvi.last_rx_data_count;

  (void) OEMNV_Get( NV_TOTAL_TX_DATA_COUNT_I, &nvi);
  nvi_cache.total_tx_data_count = nvi.total_tx_data_count;

  if( OEMNV_Get( NV_DTMF_I, &nvi) != NV_DONE_S)
  {
#ifdef FEATURE_WCDMA
#error code not present
#else
   nvi.dtmf = OEMNV_DTMFBURST; // Burst
#endif
   (void) OEMNV_Put( NV_DTMF_I, &nvi);
  }
  nvi_cache.beep_length = nvi.dtmf;

  if( OEMNV_Get( NV_MANUAL_PLMN_SEL_I, &nvi) != NV_DONE_S)
  {
    nvi.manual_plmn_sel = 0; // Read From Card by Default
    (void) OEMNV_Put( NV_MANUAL_PLMN_SEL_I, &nvi);
  }
  nvi_cache.manual_plmn_sel = nvi.manual_plmn_sel;

#ifdef FEATURE_GSTK_CS_PS_PARALLEL
  if( OEMNV_Get( NV_TOOLKIT_CS_PS_PARALLEL_I, &nvi) != NV_DONE_S)
  {
    nvi.toolkit_cs_ps_parallel = 0; // Read From Card by Default
    (void) OEMNV_Put( NV_TOOLKIT_CS_PS_PARALLEL_I, &nvi);
  }
  nvi_cache.toolkit_cs_ps_parallel = nvi.toolkit_cs_ps_parallel;
#endif /* FEATURE_GSTK_CS_PS_PARALLEL */
    
#ifdef FEATURE_CCBS
#error code not present
#endif

  cfgi_lcd = TRUE;

#if defined(FEATURE_USRSYSCLOCK) || defined(FEATURE_ISYSCLOCK)
  if( OEMNV_Get( NV_AUTO_TIME_ENABLE_I, &nvi) != NV_DONE_S){
    nvi.auto_time_enable = FALSE;  
    (void) OEMNV_Put( NV_AUTO_TIME_ENABLE_I, &nvi);
  }
  nvi_cache.auto_time_enable = nvi.auto_time_enable;
  
  OEMNV_Get( NV_NITZ_NW_INFO_I, &nvi);
  {
      /* Convert the nv banner data from UTF-8 to Unicode */
     int Len =0;

     while ( (Len < (int) sizeof(nvi.nitz_nw_info.plmn_name)) &&
             (nvi.nitz_nw_info.plmn_name[Len]) ) {
        ++Len;
     }
     memset(nvi_cache.plmn_name, 0, sizeof(nvi_cache.plmn_name));
     (void) UTF8TOWSTR((byte *) nvi.nitz_nw_info.plmn_name,
                       Len,
                       nvi_cache.plmn_name,
                       sizeof(nvi_cache.plmn_name) - sizeof(AECHAR));
                       
  }
  
  (void)OEMNV_Get( NV_NITZ_NW_INFO_MCC_I, &nvi);
  nvi_cache.mcc = nvi.nitz_nw_info_mcc;

  (void)OEMNV_Get( NV_NITZ_NW_INFO_MNC_I, &nvi);
  nvi_cache.mnc = nvi.nitz_nw_info_mnc;
  
#endif 
  if( OEMNV_Get( NV_VOICE_PRIV_I, &nvi) != NV_DONE_S){
     nvi.voice_priv = CM_PRIVACY_PREF_STANDARD;//VOICEPRIVACY_STD;
    (void) OEMNV_Put( NV_VOICE_PRIV_I, &nvi);
  }
  nvi_cache.voice_priv = nvi.voice_priv;
#ifdef CUST_EDITION
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
   
   //CFGI_PHONE_PASSWORD_CHECK
   (void) OEMNV_Get( NV_LOCK_I, &nvi);
   nvi_cache.lock = nvi.lock;

   //CFGI_KEYTONE_LENGTH
#ifdef FEATURE_VERSION_W208S
	nvi_cache.key_tone_length = OEMNV_KEYTONE_LONG;
#else
   (void) OEMNV_Get( NV_CONT_KEY_DTMF_I, &nvi);
   if(nvi.cont_key_dtmf)
   {
      nvi_cache.key_tone_length = OEMNV_KEYTONE_LONG;
   }
   else
   {
      nvi_cache.key_tone_length = OEMNV_KEYTONE_NORMAL;
   }
#endif   
#endif // CUST_EDITION
#endif//#ifndef WIN32

#ifdef CUST_EDITION
    cfgi_lcd = TRUE;
#endif // CUST_EDITION
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
   nv_item_type nvi;
   
   if (!cache_initialized) {
      OEM_InitPreference();
   }

   if (pBuff == NULL) {
      DBGPRINTF("OEM_GetCachedConfig(): invalid parm size");
      return EBADPARM;
   }

   // First check if the item can be found in any of the config
   // tables before resorting to the ugly switch() statement below...
   for (j = 0; j < ARR_SIZE(cfgTable); j++) {
     
      // Is the item in the range of this table?
      if ( (i >= cfgTable[j].tbl[0].item) &&
           (i <= cfgTable[j].tbl[cfgTable[j].size-1].item) ) {

         int idx = (int)i - (int) cfgTable[j].tbl[0].item;
        
         // Ensure the config item table is valid
         ASSERT(i == (int) cfgTable[j].tbl[idx].item);

         if (NULL == cfgTable[j].tbl[idx].get) {
            // Exit the for() loop immediately if we found the
            // entry and it had a NULL function.
            DBGPRINTF("GetConfig(): invalid parm size");
            break;
         }

         // Perform the size check here so the item functions can
         // assume the size is ok.
         if (nSize != cfgTable[j].tbl[idx].size) {
            DBGPRINTF("GetConfig(): %d %d %d = %d",j,idx,nSize,cfgTable[j].tbl[idx].size);
            return EBADPARM;
         }
      
         return cfgTable[j].tbl[idx].get(pBuff);
      }
   }
   
   // Didn't find the config item in the tables, maybe it's in this switch...
   switch (i) {
   case CFGI_BACK_LIGHT:
      if (nSize != sizeof(byte)) {
         ERR("GetConfig(): invalid parm size", 0, 0, 0);
         return EBADPARM;
      }
      return OEMPriv_GetItem_CFGI_BACK_LIGHT(pBuff);

   case CFGI_SVC_ALERT:
      if (nSize != sizeof(byte)) {
         ERR("GetConfig(): invalid parm size", 0, 0, 0);
         return EBADPARM;
      }
      return OEMPriv_GetItem_CFGI_SVC_ALERT(pBuff);

   case CFGI_EXTPWR_BK_LIGHT:
      if (nSize != sizeof(byte)) {
         ERR("GetConfig(): invalid parm size", 0, 0, 0);
         return EBADPARM;
      }
      return OEMPriv_GetItem_CFGI_EXTPWR_BK_LIGHT(pBuff);

   case CFGI_CONTRAST_LVL:
      if (nSize != sizeof(byte)) {
         ERR("GetConfig(): invalid parm size", 0, 0, 0);
         return EBADPARM;
      }
      return OEMPriv_GetItem_CFGI_CONTRAST_LVL(pBuff);
  
   case CFGI_VOICEPRIVACY:
      if (nSize != sizeof(byte)) {
         ERR("GetConfig(): invalid parm size", 0, 0, 0);
         return EBADPARM;
      }
      return OEMPriv_GetItem_CFGI_VOICEPRIVACY(pBuff);
#ifndef CUST_EDITION
   case CFGI_RTRE_CONFIG:
      if (nSize != sizeof(byte)) {
         ERR("GetConfig(): invalid parm size", 0, 0, 0);
         return EBADPARM;
      }
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
      if (NV_DONE_S != OEMNV_Get(NV_RTRE_CONFIG_I, &nvi)) {
         // Default to R-UIM only.
         #ifdef FEATURE_VERSION_S600S
		 *(byte *) pBuff = RTRE_CONFIG_NVONLY;
		 #else
         *(byte *) pBuff = RTRE_CONFIG_RUIMORDROPBACK;
		 #endif
         return SUCCESS;
      }
      *(byte *) pBuff = (byte)nvi.rtre_config;
      return SUCCESS;
#else
      *(byte *) pBuff = (byte)RTRE_CONFIG_DISABLED;
      return SUCCESS;
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */

   case CFGI_UNITTEST_ACTIVE:
      if (nSize != (int)sizeof(boolean)) {
         ERR("GetConfig(): invalid parm size", 0, 0, 0);
         return EBADPARM;
      }
#ifdef srIMON
#error code not present
#else
      *(boolean *) pBuff = FALSE;
#endif /* srIMON */
      return SUCCESS;

   case CFGI_RADIO_TECH:
      if (nSize != sizeof(byte)) {
         ERR("GetConfig(): invalid parm size", 0, 0, 0);
         return EBADPARM;
      }

      *(byte *) pBuff = 0;
#ifdef FEATURE_CDMA1X
      *(byte *) pBuff |= RADIO_TECH_CDMA;
#endif
#ifdef FEATURE_GSM
#error code not present
#endif
#ifdef FEATURE_WCDMA
#error code not present
#endif
      return SUCCESS;
#endif
#ifdef AEE_SIMULATOR
   case CFGI_LNG:
      if (nSize != (int) sizeof(uint32)) {
         return EBADPARM;
      }

      // Default to Off
      *(uint32 *) pBuff = 0;

      if (NV_DONE_S == OEMNV_Get(NV_LANG_I, &nvi)) {
         *(uint32 *) pBuff = nvi.u;
      }
      return SUCCESS;

#endif
#ifndef CUST_EDITION
   case CFGI_FIRMWARE_VERSION:
      if (nSize != FIRMWAREVERSION_MAX_LEN * (int)sizeof(AECHAR)) {
         ERR("GetConfig(): invalid parm size", 0, 0, 0);
         return EBADPARM;
      }
      (void) STRTOWSTR(mob_sw_rev, 
                       (AECHAR*)pBuff, 
                       FIRMWAREVERSION_MAX_LEN *(int) sizeof(AECHAR));
      return SUCCESS;

   case CFGI_AUTO_ANSWER:
   {
      nv_item_type nvi;
      
      if (nSize != sizeof(boolean)) {
         ERR("GetConfig(): invalid parm size", 0, 0, 0);
         return EBADPARM;
      }
     
      if (OEMNV_Get(NV_AUTO_ANSWER_I, &nvi) != NV_DONE_S)
      {
         ERR("OEM_GetCachedConfig: OEMNV_Get failed",0,0,0);
         return EFAILED;   
      }
            
      *(boolean*)pBuff = nvi.auto_answer.enable;

      return SUCCESS;

   }
#endif

   case CFGI_TTY:
   {
      nv_item_type nvi;

      if (nSize != sizeof(uint32)){
         ERR("GetConfig(): invalid parm size", 0, 0, 0);
         return EBADPARM;
      }

      if (OEMNV_Get(NV_TTY_I, &nvi) != NV_DONE_S)
      {
         ERR("OEM_GetCachedConfig: OEMNV_Get failed",0,0,0);
         return EFAILED;
      }

      *(uint32 *)pBuff  = nvi.tty;

      return SUCCESS;
   }
#ifndef CUST_EDITION
   case CFGI_LP_SEC:
   {
      nv_item_type nvi;

      if (nSize != sizeof(uint8)){
         ERR("GetConfig(): invalid parm size", 0, 0, 0);
         return EBADPARM;
      }

      if (OEMNV_Get(NV_DB_LP_SEC_I, &nvi) != NV_DONE_S)
      {
         ERR("OEM_GetCachedConfig: OEMNV_Get failed",0,0,0);
         return EFAILED;
      }

      *(uint8 *)pBuff  = nvi.db_lp_sec;

      return SUCCESS;
   }
   
   
   
   case CFGI_LTM_OFF:
   {
      nv_item_type nvi;

      if (nSize != sizeof(uint8)){
         ERR("GetConfig(): invalid parm size", 0, 0, 0);
         return EBADPARM;
      }

      if (OEMNV_Get(NV_DB_LTM_OFF_I, &nvi) != NV_DONE_S)
      {
         ERR("OEM_GetCachedConfig: OEMNV_Get failed",0,0,0);
         return EFAILED;
      }

      *(uint8 *)pBuff  = nvi.db_ltm_off;

      return SUCCESS;
   }
   
   case CFGI_DAYLT:
   {
      nv_item_type nvi;

      if (nSize != sizeof(boolean)){
         ERR("GetConfig(): invalid parm size", 0, 0, 0);
         return EBADPARM;
      }

      if (OEMNV_Get(NV_DB_DAYLT_I, &nvi) != NV_DONE_S)
      {
         ERR("OEM_GetCachedConfig: OEMNV_Get failed",0,0,0);
         return EFAILED;
      }

      *(boolean *)pBuff  = nvi.db_daylt;

      return SUCCESS;
   }
#endif
   //////////////////////////////////////////////////////////////////////
   // Security Menu
   //////////////////////////////////////////////////////////////////////

   case CFGI_LOCKCODE:
      //check to see if we have enough buffer size
      if (nSize != (NV_LOCK_CODE_SIZE+1)) {
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
         char         digits[OEMNV_PHONENUMBER_MAXLEN];

         if (nSize < (int) sizeof(AECHAR) * OEMNV_PHONENUMBER_MAXLEN) {
            return EFAILED;
         }
#ifndef WIN32
         nvi.mob_dir_number.nam = (byte) CM_NAM_1;

         if (NV_DONE_S != OEMNV_Get(NV_DIR_NUMBER_PCS_I, &nvi)) {
           return EFAILED;
         }
         if (nSize <= nvi.mob_dir_number.n_digits) {
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

#ifdef FEATURE_SMS_RETRY
   case CFGI_SMS_RETRY_PERIOD:
   {

     if (OEMNV_Get(NV_SMS_MO_RETRY_PERIOD_I, &nvi) != NV_DONE_S) 
     {
       return EFAILED;   
     }
     *(word*)pBuff = nvi.sms_mo_retry_period;
     return SUCCESS;
   }

   case CFGI_SMS_RETRY_INTERVAL:
   {

     if (OEMNV_Get(NV_SMS_MO_RETRY_INTERVAL_I, &nvi) != NV_DONE_S) 
     {
       return EFAILED;   
     }
     *(word*)pBuff = nvi.sms_mo_retry_interval;
     return SUCCESS;
   }
#endif /* FEATURE_SMS_RETRY */

#ifdef FEATURE_GWSMS
#error code not present
#endif /* FEATURE_GWSMS */

   case CFGI_VOICEMAIL_NUMBER:
   {
#ifdef CUST_EDITION
#ifndef WIN32  
//wangliang modify 
    /* WSTRNCOPYN(pBuff,
                nSize / (int) sizeof(AECHAR),
                (void *) oemi_cache.voicemail_number,
                (int) (sizeof(oemi_cache.voicemail_number) / sizeof(AECHAR)));*/
//#if 0   
     if (OEMNV_Get(NV_SMS_VM_NUMBER_I, &nvi) != NV_DONE_S) 
     {
       return EFAILED;
     }
     nvi.sms_vm_number.digits[nvi.sms_vm_number.num_digits] = 0;
     STRTOWSTR((char *)nvi.sms_vm_number.digits, pBuff, (nvi.sms_vm_number.num_digits+1) * sizeof(AECHAR));
//#endif	 
#endif
     if (WSTRLEN(pBuff) == 0)
     {
#ifdef FEATURE_CARRIER_VENEZUELA_MOVILNET
        (void)STRTOWSTR("*9", pBuff, nSize);
#endif     
     }
#else
#ifdef FEATURE_WMS_APP

     if (OEMNV_Get(NV_SMS_VM_NUMBER_I, &nvi) != NV_DONE_S)
     {
       return EFAILED;
     }
     nvi.sms_vm_number.digits[nvi.sms_vm_number.num_digits] = 0;
     STRTOWSTR((char *)nvi.sms_vm_number.digits, pBuff, (nvi.sms_vm_number.num_digits+1) * sizeof(AECHAR));
#else
     WSTRNCOPYN(pBuff,
                nSize / (int) sizeof(AECHAR),
                (void *) oemi_cache.voicemail_number,
                (int) (sizeof(oemi_cache.voicemail_number) / sizeof(AECHAR)));
#endif /* FEATURE_WMS_APP */
#endif // CUST_EDITION
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
#ifndef WIN32      
      nv_item_type nvItem;

      nvItem.air_timer.nam = (byte) CM_NAM_1;
      if(NV_DONE_S != OEMNV_Get(NV_AIR_TIMER_I, &nvItem))
      {
        return EFAILED;
      }

      *(dword*)pBuff = nvItem.air_timer.time;
#endif
      return SUCCESS;
   }

   // returns the total roaming time used.
   case CFGI_ROAM_TIMER:
   {
#ifndef WIN32     
      nv_item_type nvItem;

      nvItem.roam_timer.nam = (byte) CM_NAM_1;
      if(NV_DONE_S != OEMNV_Get(NV_ROAM_TIMER_I, &nvItem))
      {
        return EFAILED;
      }

      *(dword*)pBuff = nvItem.roam_timer.time;
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
#ifdef FEATURE_PEKTEST
      STR_TO_WSTR((char *)mob_sw_rev,
                  (AECHAR *) pBuff,
                  nSize);
#else
#ifdef CUST_EDITION
#ifndef WIN32
      STR_TO_WSTR((char *)ver_modelversion,
                  (AECHAR *) pBuff,
                  nSize);
#endif
#else
      STR_TO_WSTR((char *)ver_dir,
                  (AECHAR *) pBuff,
                  nSize);
#endif // CUST_EDITION
#endif // FEATURE_PEKTEST
      return SUCCESS;
   case CFGI_BUILD_TIME:
   	  {
   	  	  STR_TO_WSTR((char *)ver_modelbuiletime,
                  (AECHAR *) pBuff,
                  nSize);
   	  }
   	  return SUCCESS;
   
#ifdef FEATURE_FACTORY_TESTMODE
   case CFGI_FACTORY_TEST_MODE:
      if (nSize != sizeof(boolean)) {
         ERR("GetConfig(): invalid parm size", 0, 0, 0);
         return EBADPARM;
      }
      return OEMPriv_GetItem_CFGI_FACTORY_TEST_MODE(pBuff);
#endif /*FEATURE_FACTORY_TESTMODE*/

   case CFGI_PRI_CH_A:
   case CFGI_PRI_CH_B:
      {

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
         rc->dx = 0;
         rc->dy = 0;

         return SUCCESS;
      }

   case CFGI_DOWNLOAD:
      {
         AEEDownloadInfo * pdi = (AEEDownloadInfo *)pBuff;

         if(nSize != sizeof(AEEDownloadInfo))
            return(EBADPARM);

         MEMSET((byte *) pdi, 0, nSize);

         pdi->dwCarrierID  = oemi_cache.download_info.dwCID;
         pdi->dwPlatformID = oemi_cache.download_info.dwPID;
         pdi->nAuth        = oemi_cache.download_info.wAuth;
         pdi->wFlags       = oemi_cache.download_info.wFlags;
         pdi->nPolicy      = (AEEPrivacyPolicy)oemi_cache.download_info.wPrivP;
         STRLCPY((char *)pdi->szServer, 
                (const char *)oemi_cache.download_info.szServer, 
                 sizeof(oemi_cache.download_info.szServer));
         MEMCPY((void *)pdi->bBKey, (void *)oemi_cache.download_info.bBKey, DL_BKEY_SIZE);
         MEMCPY((void *)pdi->bAKey, (void *)oemi_cache.download_info.bAKey, DL_AKEY_SIZE);

         return(SUCCESS);
      }

   case CFGI_BREW_CID:
      {
         uint32 *val = (uint32 *)pBuff;
         if (nSize != (int) sizeof(*val)) {
            return EBADPARM;
         }
         *val = oemi_cache.download_info.dwCID;
         return(SUCCESS);
      }

   case CFGI_BREW_PID:
      {
         uint32 *val = (uint32 *)pBuff;
         if (nSize != (int) sizeof(*val)) {
            return EBADPARM;
         }
         *val = oemi_cache.download_info.dwPID;
         return(SUCCESS);
      }

   case CFGI_BREW_BKEY:
      {

         if (nSize < (int) DL_BKEY_SIZE * sizeof(AECHAR)) {
            return EBADPARM;
         }
         STRTOWSTR((char *)oemi_cache.download_info.bBKey, pBuff, nSize);

         return(SUCCESS);
      }

   case CFGI_BREW_AKEY:
      {
         if (nSize < (int) DL_AKEY_SIZE * sizeof(AECHAR)) {
            return EBADPARM;
         }
         STRTOWSTR((char *)oemi_cache.download_info.bAKey, pBuff, nSize);

         return(SUCCESS);
      }

   case CFGI_BREW_SERVER:
      {
         if (nSize > (int) DL_MAX_SERVER * sizeof(AECHAR)) {
            return EBADPARM;
         }
         STRTOWSTR((char *)oemi_cache.download_info.szServer, pBuff, DL_MAX_SERVER * sizeof(AECHAR));
         return(SUCCESS);
      }

   case CFGI_BREW_FLAGS:
      {
         uint16 *val = (uint16 *)pBuff;
         if (nSize != (int) sizeof(*val)) {
            return EBADPARM;
         }
         *val = oemi_cache.download_info.wFlags;
         return(SUCCESS);
      }

   case CFGI_BREW_AUTH:
      {
         byte *val = (byte *)pBuff;
         if (nSize != (int) sizeof(*val)) {
            return EBADPARM;
         }
         *val = oemi_cache.download_info.wAuth;
         return(SUCCESS);
      }

   case CFGI_BREW_PRIVP:
      {
         byte *val = (byte *)pBuff;
         if (nSize != (int) sizeof(*val)) {
            return EBADPARM;
         }
         *val = oemi_cache.download_info.wPrivP;
         return(SUCCESS);
      }
#ifndef CUST_EDITION
   case CFGI_SUBSCRIBERID:
      {
         if(nSize < (STRLEN((const char *)oemi_cache.szSubscriberID) + 1))
            return EBADPARM;

         STRLCPY((char*)pBuff, (const char *)oemi_cache.szSubscriberID, STRLEN((const char *)oemi_cache.szSubscriberID)+1);         

         return AEE_SUCCESS;
      }
#endif
#if defined(FEATURE_USRSYSCLOCK) || defined(FEATURE_ISYSCLOCK)
    case CFGI_NITZ_NW_INFO:
      WSTRNCOPYN(pBuff,
                 nSize / (int) sizeof(AECHAR),
                 nvi_cache.plmn_name,
                 (int) (sizeof(nvi_cache.plmn_name) / sizeof(AECHAR)) );
      return (SUCCESS);
   case CFGI_NITZ_NW_INFO_MCC:
      {
         uint32 *val = (uint32 *)pBuff;
         if (nSize != (int) sizeof(*val)) {
            return EBADPARM;
         }
         *val = nvi_cache.mcc;
         return(SUCCESS);
      }
   case CFGI_NITZ_NW_INFO_MNC:
      {
         uint32 *val = (uint32 *)pBuff;
         if (nSize != (int) sizeof(*val)) {
            return EBADPARM;
         }
         *val = nvi_cache.mnc;
         return(SUCCESS);
      }
#endif 
#ifdef CUST_EDITION
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
         
         GetMobileInfo(&info);
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
         
         GetMobileInfo(&info);
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
   {
      if (nSize != sizeof(AEEMobileInfo))
      {
         return(EBADPARM);
      }
      GetMobileInfo((AEEMobileInfo *) pBuff);
   }
      return SUCCESS;
    
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
#endif // CUST_EDITION
    case CFGI_NET_LOCK_SID:
    {
        nv_item_type nvi;
        int k;
#ifndef WIN32         
        if (OEMNV_Get(NV_NET_LOCK_I, &nvi) != NV_DONE_S)
        {
           DBGPRINTF("OEM_GetCachedConfig: CFGI_NET_LOCK_SID failed");
           return EFAILED;   
        }
        for (k=0; k<NV_MAX_SID_LOCK; k++)
        {
            *((word *)(pBuff)+k) = *(nvi.enabled_net_lock.sid+k);
        }
#endif        
        return SUCCESS;
    }    

    case CFGI_NET_LOCK_MNC:
    {
        nv_item_type nvi;
        int k;
#ifndef WIN32         
        if (OEMNV_Get(NV_NET_LOCK_I, &nvi) != NV_DONE_S)
        {
           DBGPRINTF("OEM_GetCachedConfig: CFGI_NET_LOCK_MNC failed");
           return EFAILED;   
        }
        for (k=0; k<NV_MAX_SID_LOCK; k++)
        {
            *((word *)(pBuff)+k) = *(nvi.enabled_net_lock.mnc+k);
            //DBGPRINTF("lock.mnc[%d]=%d", k, (word *)pBuff[k]);
        }
#endif        
        return SUCCESS;
    }  

    case CFGI_NET_LOCK_MCC:
    {
        nv_item_type nvi;
        int k;
#ifndef WIN32         
        if (OEMNV_Get(NV_NET_LOCK_I, &nvi) != NV_DONE_S)
        {
           DBGPRINTF("OEM_GetCachedConfig: CFGI_NET_LOCK_MNC failed");
           return EFAILED;   
        }
        for (k=0; k<NV_MAX_SID_LOCK; k++)
        {
            ((word *)pBuff)[k] = nvi.enabled_net_lock.mcc[k];
            //*((word *)(pBuff)+k) = *(nvi.enabled_net_lock.mcc+k);
         //   DBGPRINTF("lock.mcc[%d]=%d", k, (word *)pBuff[k]);
        }
#endif        
        return SUCCESS;
    }   

    case CFGI_NET_LOCK_ENABLED:
    {
        nv_item_type nvi;
#ifndef WIN32         
        if (OEMNV_Get(NV_NET_LOCK_I, &nvi) != NV_DONE_S)
        {
             DBGPRINTF("OEM_GetCachedConfig: CFGI_NET_LOCK_ENABLED failed",0,0,0);
             return EFAILED;
        }

        *(word *)pBuff  = nvi.enabled_net_lock.b_lock;
        DBGPRINTF("OEM_GetCachedConfig: CFGI_NET_LOCK_ENABLED %d",nvi.enabled_net_lock.b_lock);
#endif        
        return SUCCESS;
    }    
#ifdef FEATURE_USES_MMS
    case CFGI_MMSIMAGE:
      MEMCPY((void *)pBuff, (void *)oemi_cache.s_MMSImage, AEE_MAX_FILE_NAME);
      return AEE_SUCCESS;

    case CFGI_MMSSOUND:
      MEMCPY((void *)pBuff, (void *)oemi_cache.s_MMSSOUND, AEE_MAX_FILE_NAME);
      return AEE_SUCCESS;      

    case CFGI_MMSVIDEO:
      MEMCPY((void *)pBuff, (void *)oemi_cache.s_MMSVIDEO, AEE_MAX_FILE_NAME);
      return AEE_SUCCESS;        

    case CFGI_MMS_SERVER_ADDRESS:
      MEMCPY((void *)pBuff, (void *)oemi_cache.s_MMSServerAddress, AEE_MAX_FILE_NAME);
      return AEE_SUCCESS;
#endif      
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
#if defined(FEATURE_UIONE_HDK) && ! defined(AEE_SIMULATOR)
   disp_info_type di;
   uint32 contrast;
#endif
   MSG_FATAL("OEM_SetCachedConfig Start i=%d",i,0,0);
   if (!cache_initialized) {
      OEM_InitPreference();
   }
   MSG_FATAL("OEM_SetCachedConfig 1", 0, 0, 0); 
   if (pBuff == NULL) {
      MSG_FATAL("GetConfig(): invalid parm size", 0, 0, 0);
      return EBADPARM;
   }
   MSG_FATAL("OEM_SetCachedConfig 2", 0, 0, 0); 
   // First check if the item can be found in any of the config
   // tables before resorting to the ugly switch() statement below...
   for (j = 0; j < ARR_SIZE(cfgTable); j++) {
     MSG_FATAL("OEM_SetCachedConfig 3", 0, 0, 0); 
      // Is the item in the range of this table?
      if ( (i >= cfgTable[j].tbl[0].item) &&
           (i <= cfgTable[j].tbl[cfgTable[j].size-1].item) ) {

         int idx = (int)i - (int) cfgTable[j].tbl[0].item;

         // Ensure the config item table is valid
         ASSERT(i == (int) cfgTable[j].tbl[idx].item);

         if (NULL == cfgTable[j].tbl[idx].get) {
            // Exit the for() loop immediately if we found the
            // entry and it had a NULL function.
            MSG_FATAL("it had a NULL function", 0, 0, 0);
            break;
         }

         // Perform the size check here so the item functions can
         // assume the size is ok.
         if (nSize != cfgTable[j].tbl[idx].size) {
            MSG_FATAL("SetConfig(): invalid parm size", 0, 0, 0);
            return EBADPARM;
         }

         return cfgTable[j].tbl[idx].set(pBuff);
      }
   }
   MSG_FATAL("OEM_SetCachedConfig i=%d",i,0,0);
   // Didn't find the config item in the tables, maybe it's in this switch...
   switch(i) {
   case CFGI_BACK_LIGHT:
      if (nSize != sizeof(byte)) {
         ERR("GetConfig(): invalid parm size", 0, 0, 0);
         return EBADPARM;
      }
      return OEMPriv_SetItem_CFGI_BACK_LIGHT(pBuff);

   case CFGI_SVC_ALERT:
      if (nSize != sizeof(byte)) {
         ERR("GetConfig(): invalid parm size", 0, 0, 0);
         return EBADPARM;
      }
      return OEMPriv_SetItem_CFGI_SVC_ALERT(pBuff);

   case CFGI_EXTPWR_BK_LIGHT:
      if (nSize != sizeof(byte)) {
         ERR("GetConfig(): invalid parm size", 0, 0, 0);
         return EBADPARM;
      }
      return OEMPriv_SetItem_CFGI_EXTPWR_BK_LIGHT(pBuff);

   case CFGI_CONTRAST_LVL:
      if (nSize != sizeof(byte)) {
         ERR("GetConfig(): invalid parm size", 0, 0, 0);
         return EBADPARM;
      }

#if defined(FEATURE_UIONE_HDK) && ! defined(AEE_SIMULATOR)
      /* For HDK target builds, set the contrast here. */
      di = disp_get_info();

      //linearize the contrast value over the dynamic range.  We'll
      //account for integer arithmetic underflow as well
      contrast = (uint32)((di.contrast_max - di.contrast_min) * 100)
                 / ((uint32)CONTRAST_LVL_MAX - (uint32)CONTRAST_LVL_MIN);

      //interpolate and round up
      contrast = di.contrast_min + ((contrast * (*(byte *)pBuff) + 50)/100);

      disp_set_contrast( (uint16) contrast);
#endif /* FEATURE_UIONE_HDK && ! AEE_SIMULATOR */

      return OEMPriv_SetItem_CFGI_CONTRAST_LVL(pBuff);
   
   case CFGI_VOICEPRIVACY:
      if (nSize != sizeof(byte)) {
         ERR("GetConfig(): invalid parm size", 0, 0, 0);
         return EBADPARM;
      }
      return OEMPriv_SetItem_CFGI_VOICEPRIVACY(pBuff);
#ifndef CUST_EDITION
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
   case CFGI_RTRE_CONFIG:
      if (nSize != sizeof(byte)) {
         ERR("GetConfig(): invalid parm size", 0, 0, 0);
         return EBADPARM;
      }

      nvi.rtre_config = (nv_rtre_configuration_type) *((byte*)pBuff);
      if (NV_DONE_S != OEMNV_Put(NV_RTRE_CONFIG_I, &nvi)) {
         return EFAILED;
      }
      return SUCCESS;
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
#endif
#ifdef srIMON
#error code not present
#endif

#ifdef AEE_SIMULATOR
   case CFGI_LNG:
      if ( (NULL == pBuff) || (nSize != (int) sizeof(uint32)) ) {
         return EBADPARM;
      }
      nvi.u = *(uint32 *) pBuff;

      if (NV_DONE_S == OEMNV_Put(NV_LANG_I, &nvi)) {
         // Notify the world that the language has changed
#if defined(AEE_SIMULATOR) && defined(AEE_STATIC)
         (void) AEE_IssueSystemCallback(AEE_SCB_DEVICE_INFO_CHANGED);
#endif
         result = SUCCESS;
      }
      return result;

#endif


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
#ifdef FEATURE_VERSION_W208S
	  memset((void *)nvi.banner.letters, 0, sizeof(nvi.banner.letters));
#else
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
#endif	  


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
    return SUCCESS;
   }
#endif
#ifdef FEATURE_SMS_RETRY
   case CFGI_SMS_RETRY_PERIOD:
   {
     if ((!pBuff) || (nSize != (int)sizeof(word)))
     {
       return EBADPARM;
     }
     nvi.sms_mo_retry_period = *(word*)pBuff;
     if (OEMNV_Put(NV_SMS_MO_RETRY_PERIOD_I,&nvi) !=  NV_DONE_S)
     {
       return EFAILED;
     }
     return SUCCESS;
   }

   case CFGI_SMS_RETRY_INTERVAL:
   {
     if ((!pBuff) || (nSize != (int)sizeof(word)))
     {
       return EBADPARM;
     }
     nvi.sms_mo_retry_interval = *(word*)pBuff;
     if (OEMNV_Put(NV_SMS_MO_RETRY_INTERVAL_I,&nvi) !=  NV_DONE_S)
     {
       return EFAILED;
     }
     return SUCCESS;
   }
#endif /* FEATURE_SMS_RETRY */
   case CFGI_VOICEMAIL_NUMBER:
   {
     if(!pBuff) 
       return EFAILED;
#ifdef CUST_EDITION
#ifndef WIN32    
	MSG_FATAL("SET.............................................",0,0,0);
    nvi.sms_vm_number.num_digits = WSTRLEN(pBuff);
    WSTRTOSTR(pBuff, (char *)nvi.sms_vm_number.digits, nvi.sms_vm_number.num_digits+1);

    if (OEMNV_Put(NV_SMS_VM_NUMBER_I, &nvi) != NV_DONE_S)
    {
      return EFAILED;
    }
#endif
#else
#ifdef FEATURE_WMS_APP

    nvi.sms_vm_number.num_digits = WSTRLEN(pBuff);
    WSTRTOSTR(pBuff, (char *)nvi.sms_vm_number.digits, nvi.sms_vm_number.num_digits+1);

    if (OEMNV_Put(NV_SMS_VM_NUMBER_I, &nvi) != NV_DONE_S)
    {
      return EFAILED;
    }
#else
    WSTRNCOPYN((void *) oemi_cache.voicemail_number,
                sizeof(oemi_cache.voicemail_number)/sizeof(AECHAR),
                (AECHAR*) pBuff,
                -1);
    OEMPriv_WriteOEMConfigList();
#endif
#endif // CUST_EDITION
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

      if(OEMNV_Put(NV_CALL_TIMER_G_I,&nvi)!= NV_DONE_S)
        return EFAILED;
#endif
      return SUCCESS;
     }

   case CFGI_AIR_TIMER:
     {
      if ((!pBuff) || (nSize != (int)sizeof(dword)))
        return EBADPARM;
#ifndef WIN32      
      nvi.air_timer.nam = CM_NAM_1;
      nvi.air_timer.time = *(dword*) pBuff;

      if(OEMNV_Put(NV_AIR_TIMER_I,&nvi)!= NV_DONE_S)
        return EFAILED;
#endif
      return SUCCESS;
     }
   
   case CFGI_ROAM_TIMER:
     {
      if ((!pBuff) || (nSize != (int)sizeof(dword)))
        return EBADPARM;
#ifndef WIN32      
      nvi.roam_timer.nam = CM_NAM_1;
      nvi.roam_timer.time = *(dword*) pBuff;

      if(OEMNV_Put(NV_ROAM_TIMER_I,&nvi)!= NV_DONE_S)
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

      if(OEMNV_Put(NV_LIFE_TIMER_G_I,&nvi)!= NV_DONE_S)
        return EFAILED;
#endif
      return SUCCESS;
     }
   ////////////////////////////////////////////////////////////////
   // Service Programming
   ////////////////////////////////////////////////////////////////
   case CFGI_BUILD_VERSION:
   case CFGI_BUILD_TIME:
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

      if(!pBuff || nSize != sizeof(AEEDownloadInfo))
         return(EBADPARM);

      MEMSET((void *)&oemi_cache.download_info, 0, sizeof(OEMConfigDownloadInfo));
      oemi_cache.download_info.dwCID  = pdi->dwCarrierID;
      oemi_cache.download_info.dwPID  = pdi->dwPlatformID;
      oemi_cache.download_info.wAuth  = pdi->nAuth;
      oemi_cache.download_info.wFlags = pdi->wFlags;
      oemi_cache.download_info.wPrivP = pdi->nPolicy;
      STRLCPY((char *)oemi_cache.download_info.szServer, 
              (const char *)pdi->szServer, 
              sizeof(oemi_cache.download_info.szServer));
      MEMCPY((void *)oemi_cache.download_info.bBKey, (void *)pdi->bBKey, DL_BKEY_SIZE);
      MEMCPY((void *)oemi_cache.download_info.bAKey, (void *)pdi->bAKey, DL_AKEY_SIZE);
      OEMPriv_WriteOEMConfigList();
      return SUCCESS;
     }
   case CFGI_BREW_CID:
      {
         uint32 *val = (uint32 *)pBuff;
         if (!pBuff || nSize != (int) sizeof(*val)) {
            return EBADPARM;
         }
         oemi_cache.download_info.dwCID  = *val;
         OEMPriv_WriteOEMConfigList();
         return SUCCESS;
      }

   case CFGI_BREW_PID:
      {
         uint32 *val = (uint32 *)pBuff;
         if (!pBuff || nSize != (int) sizeof(*val)) {
            return EBADPARM;
         }
         oemi_cache.download_info.dwPID  = *val;
         OEMPriv_WriteOEMConfigList();
         return SUCCESS;
      }

   case CFGI_BREW_BKEY:
      {
         if (!pBuff || nSize > (int) (DL_BKEY_SIZE+1) * sizeof(AECHAR)) {
            return EBADPARM;
         }
         MEMSET((void*)oemi_cache.download_info.bBKey, 0, sizeof(oemi_cache.download_info.bBKey));
         WSTRTOSTR(pBuff, (char *)oemi_cache.download_info.bBKey, DL_BKEY_SIZE+1);
         OEMPriv_WriteOEMConfigList();
         return SUCCESS;
      }

   case CFGI_BREW_AKEY:
      {
         if (!pBuff || nSize > (int) (DL_AKEY_SIZE+1) * sizeof(AECHAR)) {
            return EBADPARM;
         }
         MEMSET((void*)oemi_cache.download_info.bAKey, 0, sizeof(oemi_cache.download_info.bAKey));
         WSTRTOSTR(pBuff, (char *)oemi_cache.download_info.bAKey, DL_AKEY_SIZE+1);
         OEMPriv_WriteOEMConfigList();
         return SUCCESS;
      }

   case CFGI_BREW_SERVER:
      {
         if (!pBuff || nSize > (int) DL_MAX_SERVER * sizeof(AECHAR)) {
            return EBADPARM;
         }
         MEMSET((void*)oemi_cache.download_info.szServer, 0, sizeof(oemi_cache.download_info.szServer));
         WSTRTOSTR(pBuff, (char *)oemi_cache.download_info.szServer, DL_MAX_SERVER);
         OEMPriv_WriteOEMConfigList();
         return SUCCESS;
      }

   case CFGI_BREW_FLAGS:
      {
         uint16 *val = (uint16 *)pBuff;
         if (!pBuff || nSize != (int) sizeof(*val)) {
            return EBADPARM;
         }
         oemi_cache.download_info.wFlags = *val;
         OEMPriv_WriteOEMConfigList();
         return SUCCESS;
      }

   case CFGI_BREW_AUTH:
      {
         uint16 *val = (uint16 *)pBuff;
         if (!pBuff || nSize != (int) sizeof(*val)) {
            return EBADPARM;
         }
         oemi_cache.download_info.wAuth  = *val;
         OEMPriv_WriteOEMConfigList();
         return SUCCESS;
      }

   case CFGI_BREW_PRIVP:
      {
         uint16 *val = (uint16 *)pBuff;
         if (!pBuff || nSize != (int) sizeof(*val)) {
            return EBADPARM;
         }
         oemi_cache.download_info.wPrivP = *val;
         OEMPriv_WriteOEMConfigList();
         return SUCCESS;
      }

   case CFGI_SUBSCRIBERID:
      {
         if(!pBuff || nSize >= DEFAULT_SUBSCRIBERID_LEN)
            return EBADPARM;

         STRLCPY((char *)oemi_cache.szSubscriberID, (char*)pBuff, DEFAULT_SUBSCRIBERID_LEN);
         OEMPriv_WriteOEMConfigList();
         return AEE_SUCCESS;
      }

#if defined(FEATURE_USRSYSCLOCK) || defined(FEATURE_ISYSCLOCK)
  case CFGI_NITZ_NW_INFO:
      if(!pBuff) return EFAILED;

      if (0 == WSTRNCMP(nvi_cache.plmn_name, (AECHAR*) pBuff, 
                        sizeof(nvi_cache.plmn_name)/sizeof(AECHAR)))
      {
        //Don't need to update if no change has occured.
        return SUCCESS;
      }

      WSTRNCOPYN(nvi_cache.plmn_name,
                 sizeof(nvi_cache.plmn_name)/sizeof(AECHAR),
                 (AECHAR*) pBuff,
                 -1);

      /* Convert the nv banner data from Unicode to UTF-8 */
      memset((void *)nvi.nitz_nw_info.plmn_name, 0, sizeof(nvi.nitz_nw_info.plmn_name));
      (void) WSTRTOUTF8((AECHAR*) pBuff,
                        WSTRLEN((AECHAR*) pBuff),
                        (byte *)nvi.nitz_nw_info.plmn_name,
                        sizeof(nvi.nitz_nw_info.plmn_name));


      (void) OEMNV_Put( NV_NITZ_NW_INFO_I, &nvi );

      return SUCCESS;
   case CFGI_NITZ_NW_INFO_MCC:
      {
         uint32 *val = (uint32 *)pBuff;
         if (!pBuff || nSize != (int) sizeof(*val)) {
            return EBADPARM;
         }

         if (nvi_cache.mcc == *val)
         {
           //Don't need to update if no change has occured.
           return SUCCESS;
         }
      
         nvi.nitz_nw_info_mcc = nvi_cache.mcc = *val;
         (void) OEMNV_Put( NV_NITZ_NW_INFO_MCC_I, &nvi );
         return SUCCESS;
      }
   case CFGI_NITZ_NW_INFO_MNC:
      {
         uint32 *val = (uint32 *)pBuff;
         if (!pBuff || nSize != (int) sizeof(*val)) {
            return EBADPARM;
         }

         if (nvi_cache.mnc == *val)
         {
           //Don't need to update if no change has occured.
           return SUCCESS;
         }

         nvi.nitz_nw_info_mnc = nvi_cache.mnc = *val;
         (void) OEMNV_Put( NV_NITZ_NW_INFO_MNC_I, &nvi );
         return SUCCESS;
      }
#endif
#ifndef CUST_EDITION
   case CFGI_AUTO_ANSWER:
      {
         nv_item_type nvi;

         if ( (NULL == pBuff) || (nSize != (int) sizeof(boolean)) ) {
            return EBADPARM;
         }
         nvi.auto_answer.enable = *(boolean *) pBuff;
         if (NV_DONE_S != OEMNV_Put(NV_AUTO_ANSWER_I, &nvi)) {
            return EFAILED;  
         }
         return SUCCESS;
      }
#endif
   case CFGI_TTY:
      {
         nv_item_type nvi;

         if ( (NULL == pBuff) || (nSize != (int) sizeof(uint32)) ) {
            return EBADPARM;
         }
         nvi.tty = *(uint32 *) pBuff;
         if (NV_DONE_S != OEMNV_Put(NV_TTY_I, &nvi)) {
            return EFAILED;
         }
         return SUCCESS;
      }
#ifndef CUST_EDITION
   case CFGI_LP_SEC:
      {
         nv_item_type nvi;

         if ( (NULL == pBuff) || (nSize != (int) sizeof(uint8)) ) {
            return EBADPARM;
         }
         nvi.db_lp_sec = *(uint8 *) pBuff;
         if (NV_DONE_S != OEMNV_Put(NV_DB_LP_SEC_I, &nvi)) {
            return EFAILED;
         }
         return SUCCESS;
      }
   
   case CFGI_LTM_OFF:
      {
         nv_item_type nvi;

         if ( (NULL == pBuff) || (nSize != (int) sizeof(uint8)) ) {
            return EBADPARM;
         }
         nvi.db_ltm_off = *(uint8 *) pBuff;
         if (NV_DONE_S != OEMNV_Put(NV_DB_LTM_OFF_I, &nvi)) {
            return EFAILED;
         }
         return SUCCESS;
      }
    
   case CFGI_DAYLT:
      {
         nv_item_type nvi;

         if ( (NULL == pBuff) || (nSize != (int) sizeof(boolean)) ) {
            return EBADPARM;
         }
         nvi.db_daylt = *(boolean *) pBuff;
         if (NV_DONE_S != OEMNV_Put(NV_DB_DAYLT_I, &nvi)) {
            return EFAILED;
         }
         return SUCCESS;
      }
#endif
#ifdef CUST_EDITION
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
#else
#endif
      return AEE_SUCCESS;
#endif //#ifdef CUST_EDITION

    case CFGI_NET_LOCK_ENABLED:
    {
        nv_item_type nvi, nvi2;
        int k;
        if ( (NULL == pBuff) || (nSize != sizeof(word)) ) 
        {
            return EBADPARM;
        }
        if (OEMNV_Get(NV_NET_LOCK_I, &nvi2) != NV_DONE_S)
        {
           DBGPRINTF("OEM_GetCachedConfig: CFGI_NET_LOCK_MCC failed");
           return EFAILED;   
        }
        for (k=0; k<NV_MAX_SID_LOCK; k++)
        {
            nvi.enabled_net_lock.mnc[k] = *(nvi2.enabled_net_lock.mnc+k);
            DBGPRINTF("lock.mnc[%d]=%d", k, nvi.enabled_net_lock.mnc[k]);
            nvi.enabled_net_lock.mcc[k] = *(nvi2.enabled_net_lock.mcc+k);
            DBGPRINTF("lock.mcc[%d]=%d", k, nvi.enabled_net_lock.mcc[k]);
            nvi.enabled_net_lock.sid[k] = *(nvi2.enabled_net_lock.sid+k);
            DBGPRINTF("lock.sid[%d]=%d", k, nvi.enabled_net_lock.sid[k]);            
        }
        
        nvi.enabled_net_lock.b_lock = *(word *) pBuff;
        DBGPRINTF(";put CFGI_NET_LOCK_ENABLED b_lock=%d", nvi.enabled_net_lock.b_lock);
        if (NV_DONE_S != OEMNV_Put(NV_NET_LOCK_I, &nvi)) 
        {
            DBGPRINTF(";put CFGI_NET_LOCK_ENABLED failed");
            return EFAILED;  
        }
        return SUCCESS;
    }

    case CFGI_NET_LOCK_MNC:
    {
        nv_item_type nvi;
        int k;
        word *pWStr = (word *)pBuff;    
        nv_item_type nvi2;    
#ifndef WIN32         
        if (OEMNV_Get(NV_NET_LOCK_I, &nvi2) != NV_DONE_S)
        {
           DBGPRINTF("OEM_GetCachedConfig: CFGI_NET_LOCK_MNC failed");
           return EFAILED;   
        }
        for (k=0; k<NV_MAX_SID_LOCK; k++)
        {
            nvi.enabled_net_lock.mcc[k] = *(nvi2.enabled_net_lock.mcc+k);
            DBGPRINTF("lock.mcc[%d]=%d", k, nvi.enabled_net_lock.mcc[k]);
            nvi.enabled_net_lock.sid[k] = *(nvi2.enabled_net_lock.sid+k);
            DBGPRINTF("lock.sid[%d]=%d", k, nvi.enabled_net_lock.sid[k]);
        }
        nvi.enabled_net_lock.b_lock = nvi2.enabled_net_lock.b_lock;
        
        for (k=0; k<NV_MAX_SID_LOCK; k++)
        {
           nvi.enabled_net_lock.mnc[k] = (word)pWStr[k];
           DBGPRINTF("lock.mnc[%d]=%d", k, (word)pWStr[k]);
        }
        if (OEMNV_Put(NV_NET_LOCK_I, &nvi) != NV_DONE_S)
        {
           DBGPRINTF(";put CFGI_NET_LOCK_MNC failed");
           return EFAILED;
        }
#endif
        return SUCCESS;
    }    

    case CFGI_NET_LOCK_MCC:
    {
        nv_item_type nvi ,nvi2;
        int k;
        word *pWStr = (word *)pBuff;        
#ifndef WIN32         
        if (OEMNV_Get(NV_NET_LOCK_I, &nvi2) != NV_DONE_S)
        {
           DBGPRINTF("OEM_GetCachedConfig: CFGI_NET_LOCK_MCC failed");
           return EFAILED;   
        }
        for (k=0; k<NV_MAX_SID_LOCK; k++)
        {
            nvi.enabled_net_lock.mnc[k] = *(nvi2.enabled_net_lock.mnc+k);
            DBGPRINTF("lock.mnc[%d]=%d", k, nvi.enabled_net_lock.mnc[k]);
            nvi.enabled_net_lock.sid[k] = *(nvi2.enabled_net_lock.sid+k);
            DBGPRINTF("lock.sid[%d]=%d", k, nvi.enabled_net_lock.sid[k]);
        }
        nvi.enabled_net_lock.b_lock = nvi2.enabled_net_lock.b_lock;

        for (k=0; k<NV_MAX_SID_LOCK; k++)
        {
           nvi.enabled_net_lock.mcc[k] = (word)pWStr[k];
           DBGPRINTF("lock.mcc[%d]=%d", k, (word)pWStr[k]);
        }
        if (OEMNV_Put(NV_NET_LOCK_I, &nvi) != NV_DONE_S)
        {
           DBGPRINTF(";put CFGI_NET_LOCK_MCC failed");
           return EFAILED;
        }
#endif
        return SUCCESS;
    }       

    case CFGI_NET_LOCK_SID:
    {
        nv_item_type nvi,nvi2;
        int k;
        word *pWStr = (word *)pBuff;        
#ifndef WIN32   
        if (OEMNV_Get(NV_NET_LOCK_I, &nvi2) != NV_DONE_S)
        {
           DBGPRINTF("OEM_GetCachedConfig: CFGI_NET_LOCK_MCC failed");
           return EFAILED;   
        }
        for (k=0; k<NV_MAX_SID_LOCK; k++)
        {
            nvi.enabled_net_lock.mnc[k] = *(nvi2.enabled_net_lock.mnc+k);
            DBGPRINTF("lock.mnc[%d]=%d", k, nvi.enabled_net_lock.mnc[k]);
            nvi.enabled_net_lock.mcc[k] = *(nvi2.enabled_net_lock.mcc+k);
            DBGPRINTF("lock.mcc[%d]=%d", k, nvi.enabled_net_lock.mcc[k]);
        }
        nvi.enabled_net_lock.b_lock = nvi2.enabled_net_lock.b_lock;

        for (k=0; k<NV_MAX_SID_LOCK; k++)
        {
            nvi.enabled_net_lock.sid[k] = (word)pWStr[k];
            DBGPRINTF("lock.sid[%d]=%d", k, (word)pWStr[k]);
        }
        if (OEMNV_Put(NV_NET_LOCK_I, &nvi) != NV_DONE_S)
        {
            DBGPRINTF(";put CFGI_NET_LOCK_SID failed");
            return EFAILED;
        }
#endif
        return SUCCESS;
    }   

#ifdef FEATURE_USES_MMS
   case CFGI_MMSIMAGE:
      if(!pBuff) return EFAILED;
      DBGPRINTF("pBuff=%s", (char*)pBuff);
      MEMSET((void *)oemi_cache.s_MMSImage,'\0', AEE_MAX_FILE_NAME/*FILESPECLEN*/);
      MEMCPY((void *)oemi_cache.s_MMSImage, (void *)pBuff, AEE_MAX_FILE_NAME/*FILESPECLEN*/);
      OEMPriv_WriteOEMConfigList();
      return AEE_SUCCESS;

   case CFGI_MMSSOUND:
      if(!pBuff) return EFAILED;
      DBGPRINTF("pBuff=%s", (char*)pBuff);
      MEMSET((void *)oemi_cache.s_MMSSOUND,'\0', AEE_MAX_FILE_NAME/*FILESPECLEN*/);
      MEMCPY((void *)oemi_cache.s_MMSSOUND, (void *)pBuff, AEE_MAX_FILE_NAME/*FILESPECLEN*/);
      OEMPriv_WriteOEMConfigList();
      return AEE_SUCCESS;      

   case CFGI_MMSVIDEO:
      if(!pBuff) return EFAILED;
      DBGPRINTF("pBuff=%s", (char*)pBuff);
      MEMSET((void *)oemi_cache.s_MMSVIDEO,'\0', AEE_MAX_FILE_NAME/*FILESPECLEN*/);
      MEMCPY((void *)oemi_cache.s_MMSVIDEO, (void *)pBuff, AEE_MAX_FILE_NAME/*FILESPECLEN*/);
      OEMPriv_WriteOEMConfigList();
      return AEE_SUCCESS;             

   case CFGI_MMS_SERVER_ADDRESS:
      if(!pBuff) return EFAILED;
      DBGPRINTF("pBuff=%s", (char*)pBuff);
      MEMSET((void *)oemi_cache.s_MMSServerAddress,'\0', MAX_URL_LEN/*FILESPECLEN*/);
      MEMCPY((void *)oemi_cache.s_MMSServerAddress, (void *)pBuff, MAX_URL_LEN/*FILESPECLEN*/);
      OEMPriv_WriteOEMConfigList();
      return AEE_SUCCESS;
#endif

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

   *pPlatformID = oemi_cache.download_info.dwPID;
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
   uint16  version = OEMCONFIGLIST_VERSION;
   int  nErr = EFAILED;
   int  nFileNameLen = 0;
   char *pszBREWPath = OEMCONFIGLIST_FILE;  /* Path using BREW convention */
   char *pszFSPath = NULL;    /* Path with full EFS convention */
   int  fd = -1;
   ERR("OEMPriv_WriteOEMConfigListCB Start", 0, 0, 0); 
#if MIN_BREW_VERSION(4,0)
   nErr = OEMefs_GetNativePath(pszBREWPath, NULL, NULL, &nFileNameLen);
#else
   nErr = OEMFS_GetNativePath(pszBREWPath, NULL, &nFileNameLen);
#endif //MIN_BREW_VERSION(4,0)

   if ((nErr != SUCCESS) || (! nFileNameLen))
   {
      ERR("Unable to resolve OEM config file path", 0, 0, 0);
      return;
   }
        
   pszFSPath = (char *)MALLOC((uint32)nFileNameLen);
   if (! pszFSPath)
   {
      ERR("Unable to create OEM config file", 0, 0, 0);
      return;
   }
        
#if MIN_BREW_VERSION(4,0)
   nErr = OEMefs_GetNativePath(pszBREWPath, pszFSPath, nFileNameLen, &nFileNameLen);
#else
   nErr = OEMFS_GetNativePath(pszBREWPath, pszFSPath, &nFileNameLen);
#endif //MIN_BREW_VERSION(4,0)
   if ((nErr != SUCCESS))
   {
     ERR("Unable to resolve OEM config file path", 0, 0, 0);
     FREEIF(pszFSPath);
     return;
   }
   
   fd = efs_open(pszFSPath, O_RDWR);
   if (fd < 0) {
      fd = efs_open(pszFSPath, O_CREAT);
      if (fd < 0) {
         ERR("Unable to create OEM config file", 0, 0, 0);
         FREEIF(pszFSPath);
         return;
      }
   }
   FREEIF(pszFSPath);

   (void) efs_lseek(fd, 0, SEEK_SET);
   (void) efs_write(fd, &version, sizeof(version));
   (void) efs_write(fd, (void *) &oemi_cache, sizeof(oemi_cache));
   (void) efs_close(fd);
   ERR("OEMPriv_WriteOEMConfigListCB End", 0, 0, 0); 
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
#ifdef FEATURE_BREW_3_0
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
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void OEMPriv_ReadOEMConfigList(void)
{
   uint16  version;
   int  nErr = EFAILED;
   int  nFileNameLen = 0;
   char *pszBREWPath = OEMCONFIGLIST_FILE;  /* Path using BREW convention */
   char *pszFSPath = NULL;    /* Path with full EFS convention */
   int  fd = -1;

#if MIN_BREW_VERSION(4,0)
   nErr = OEMefs_GetNativePath(pszBREWPath, NULL, NULL, &nFileNameLen);
#else
   nErr = OEMFS_GetNativePath(pszBREWPath, NULL, &nFileNameLen);
#endif //MIN_BREW_VERSION(4,0)

   if ((nErr != SUCCESS) || (! nFileNameLen))
   {
      ERR("Unable to resolve OEM config file path", 0, 0, 0);
      return;
   }
        
   pszFSPath = (char *)MALLOC((uint32)nFileNameLen);
   if (! pszFSPath)
   {
      ERR("Unable to create OEM config file", 0, 0, 0);
      return;
   }
        
#if MIN_BREW_VERSION(4,0)
   nErr = OEMefs_GetNativePath(pszBREWPath, pszFSPath, nFileNameLen, &nFileNameLen);
#else
   nErr = OEMFS_GetNativePath(pszBREWPath, pszFSPath, &nFileNameLen);
#endif //MIN_BREW_VERSION(4,0)
   if ((nErr != SUCCESS))
   {
     ERR("Unable to resolve OEM config file path", 0, 0, 0);
     FREEIF(pszFSPath);
     return;
   }

   fd = efs_open(pszFSPath, O_RDONLY);
   FREEIF(pszFSPath);
   if (fd < 0) {
      // Config file does not exist yet.  No big deal, just
      // use the defaults
      return;
   }

   (void) efs_lseek(fd, 0, SEEK_SET);
   (void) efs_read(fd, &version, sizeof(version));
   if (version != OEMCONFIGLIST_VERSION) {
      // Different version.  Ignore it and use the defaults
      (void) efs_close(fd);
      return;
   }
   (void) efs_read(fd, (void *) &oemi_cache, sizeof(oemi_cache));
   (void) efs_close(fd);

   //
   // Verify the config file data is valid
   //
   if (oemi_cache.alert_type > OEMNV_ALERTTYPE_VIBRINGER) {
      oemi_cache.alert_type = OEMNV_ALERTTYPE_RINGER;
   }

   if (oemi_cache.silence_all > OEMNV_SA_LITONLY) {
      oemi_cache.silence_all = OEMNV_SA_NORMAL;
   }

   if (oemi_cache.sms_service_option > OEMNV_SMS_SO_SO14) {
      oemi_cache.sms_service_option = OEMNV_SMS_SO_SO6;
   }

   if (oemi_cache.sms_is41_workaround > OEMNV_SMS_EMAIL_ADDRESSING_STANDARD) {
      oemi_cache.sms_is41_workaround = OEMNV_SMS_EMAIL_ADDRESSING_IS41;
   }

#ifdef FEATURE_VERSION_W208S
	if (oemi_cache.ppp_auth_enabled> OEMNV_PPP_AUTH_ENABLED) {
      oemi_cache.ppp_auth_enabled = OEMNV_PPP_AUTH_DISABLED;
   }
#else
   if (oemi_cache.ppp_auth_enabled> OEMNV_PPP_AUTH_ENABLED) {
      oemi_cache.ppp_auth_enabled = OEMNV_PPP_AUTH_ENABLED;
   }
#endif   

   if (oemi_cache.sms_mo_encoding > OEMNV_SMS_MO_ENCODING_UNICODE) {
      oemi_cache.sms_mo_encoding = OEMNV_SMS_MO_ENCODING_7BIT;
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

   if (oemi_cache.time_format > OEMNV_TIMEFORM_24HR) {
      oemi_cache.time_format = OEMNV_TIMEFORM_AMPM;
   }

   oemi_cache.device_mask = 0;

   MEMSET((void *)&oemi_cache.cug, 0, sizeof(OEMConfigCugInfo));
   if (STRCMP((const char *)oemi_cache.download_info.szServer, "") == 0)
   {
     oemi_cache.download_info.dwCID  = DEFAULT_BREW_CARRIER_ID;
     oemi_cache.download_info.dwPID  = DEFAULT_BREW_PLATFORM_ID;
     oemi_cache.download_info.wFlags = DEFAULT_BREW_DOWNLOAD_FLG;
     oemi_cache.download_info.wAuth  = DEFAULT_BREW_APOLICY;
     oemi_cache.download_info.wPrivP = DEFAULT_BREW_PPOLICY;
     MEMSET(oemi_cache.download_info.bBKey,0,OEMCFG_DL_BKEY_SIZE);
     MEMSET(oemi_cache.download_info.bAKey,0,OEMCFG_DL_AKEY_SIZE);

     STRLCPY((char *)oemi_cache.download_info.szServer, 
             (const char *)DEFAULT_BREW_SERVER, 
             sizeof(oemi_cache.download_info.szServer));
   }

   // Force the value on the following members
   oemi_cache.disable_in_call_disp = FALSE;
   oemi_cache.disable_bg_image = FALSE;
   oemi_cache.manual_plmn_sel_allowed = TRUE;
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
#ifdef CUST_EDITION
   if (oemi_cache.any_key_answer != *(byte *)pBuff) {
      oemi_cache.any_key_answer = *((byte *)pBuff);
#else
   if (oemi_cache.any_key_answer != *(boolean *)pBuff) {
      oemi_cache.any_key_answer = *((boolean *)pBuff);
#endif
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

static int OEMPriv_GetItem_CFGI_GAME_MODE_PERSIST(void *pBuff)
{
  *(boolean *) pBuff = nvi_cache.game_mode_persist;
  return SUCCESS;
}

//lint -save -esym(818, pBuff)  Suppress "could be declared as const pointer"
static int OEMPriv_SetItem_CFGI_GAME_MODE_PERSIST(void *pBuff)
{
  nv_item_type nvi;

  if (nvi_cache.game_mode_persist != *((boolean *)pBuff)) {
      nvi_cache.game_mode_persist = *((boolean *)pBuff);
      nvi.game_mode_persist = nvi_cache.game_mode_persist;
     (void) OEMNV_Put( NV_GAME_MODE_PERSIST_I, &nvi);
  }
  return SUCCESS;
}
//lint -restore

static int OEMPriv_GetItem_CFGI_GAME_MODE(void *pBuff)
{
  *(boolean *) pBuff = nvi_cache.game_mode;
  return SUCCESS;
}

//lint -save -esym(818, pBuff)  Suppress "could be declared as const pointer"
static int OEMPriv_SetItem_CFGI_GAME_MODE(void *pBuff)
{
  nv_item_type nvi;

  if (nvi_cache.game_mode != *((boolean *)pBuff)) {
      nvi_cache.game_mode = *((boolean *)pBuff);
      nvi.game_mode = nvi_cache.game_mode;
     (void) OEMNV_Put( NV_GAME_MODE_I, &nvi);
  }
  return SUCCESS;
}
//lint -restore

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
#ifdef CUST_EDITION
    // To do - ringer type
    *(byte *) pBuff = nvi_cache.ringer_type;
#endif
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
  nv_item_type nvi;

  if (nvi_cache.display_duration != *((uint32 *)pBuff)) {
      nvi_cache.display_duration = *((uint32 *)pBuff);
      nvi.display_duration = nvi_cache.display_duration;
     (void) OEMNV_Put( NV_DISPLAY_DURATION_I, &nvi);
  }
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

static int OEMPriv_GetItem_CFGI_BEEP_LENGTH(void *pBuff)
{
  *(uint32 *) pBuff = nvi_cache.beep_length;
  return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_BEEP_LENGTH(void *pBuff)
{
  nv_item_type nvi;
  if (nvi_cache.beep_length != *((uint32 *)pBuff)) {
    nvi_cache.beep_length = *((uint32 *)pBuff);
    nvi.dtmf = *((uint32 *)pBuff);
    (void) OEMNV_Put(NV_DTMF_I, &nvi);
  }
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

static int OEMPriv_GetItem_CFGI_BACK_LIGHT_INTENSITY(void *pBuff)
{
   *(uint8 *) pBuff = nvi_cache.backlight_intensity;

   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_BACK_LIGHT_INTENSITY(void *pBuff)
{
   nv_item_type nvi;

   if (nvi_cache.backlight_intensity != *((uint8 *)pBuff)) {
      nvi_cache.backlight_intensity = *((uint8 *)pBuff);
      nvi.back_light_intensity = *((uint8 *)pBuff);
      (void) OEMNV_Put( NV_BACK_LIGHT_INTENSITY_I, &nvi );
   }

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
#ifdef CUST_EDITION
   *(nv_language_enum_type *) pBuff = nvi_cache.language;
#else
   *(byte *) pBuff = nvi_cache.language;
#endif
   return SUCCESS;
}


static int OEMPriv_SetItem_CFGI_LANGUAGE_SELECTION(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;
#ifdef CUST_EDITION
   if( nvi_cache.language != *((nv_language_enum_type *)pBuff )) {
      nvi_cache.language = *((nv_language_enum_type *)pBuff );
      nvi.language_selection = *((nv_language_enum_type *)pBuff);
#else
   if( nvi_cache.language != *((byte *)pBuff )) {
      nvi_cache.language = *((byte *)pBuff );
      nvi.language_selection = (nv_language_enum_type) *((byte *)pBuff);
#endif
      (void) OEMNV_Put( NV_LANGUAGE_SELECTION_I, &nvi );

      // NOTE!
      //
      // After setting the new language, the caller must call
      // ISHELL_CloseApplet(ps, TRUE) to exit all applets and return to
      // the Idle applet as the fonts may be different for the new language.
      // Currently only the Idle applet keeps track of the language
      // and takes appropriate action when it is resumed.
      //
#ifdef CUST_EDITION
     (void) AEE_IssueSystemCallback(AEE_SCB_DEVICE_INFO_CHANGED);
#endif
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
#ifdef CUST_EDITION
   *(byte *) pBuff = nvi_cache.set_time_format;
#else
   *(byte *) pBuff = oemi_cache.time_format;
#endif
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_TIME_FORMAT(void *pBuff)
{
#ifdef CUST_EDITION
#ifndef WIN32
    nv_item_type nvi;

   if (nvi_cache.set_time_format != *((byte *)pBuff)) {
      nvi_cache.set_time_format = *((byte *)pBuff);
      nvi.set_time_format = *((nv_set_time_format_enum_type *)pBuff);
      (void) OEMNV_Put( NV_SET_TIME_FORMAT_I,  &nvi);
   }
#endif
#else
   if (oemi_cache.time_format != *(boolean *)pBuff) {
      oemi_cache.time_format = *(boolean *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
#endif //CUST_EDITION
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_MANUAL_PLMN_SEL(void *pBuff)
{
   *(boolean *) pBuff = nvi_cache.manual_plmn_sel;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_MANUAL_PLMN_SEL(void *pBuff)
{
   nv_item_type nvi;

   if (nvi_cache.manual_plmn_sel != *(boolean *)pBuff) {
      nvi_cache.manual_plmn_sel = *(boolean *)pBuff;
      nvi.manual_plmn_sel = nvi_cache.manual_plmn_sel;
      (void) OEMNV_Put(NV_MANUAL_PLMN_SEL_I, &nvi);
   }
   return SUCCESS;
}

#ifdef FEATURE_GSTK_CS_PS_PARALLEL
static int OEMPriv_GetItem_CFGI_TOOLKIT_CS_PS_PARALLEL(void *pBuff)
{
   *(boolean *) pBuff = nvi_cache.toolkit_cs_ps_parallel;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_TOOLKIT_CS_PS_PARALLEL(void *pBuff)
{
   nv_item_type nvi;

   if (nvi_cache.toolkit_cs_ps_parallel != *(boolean *)pBuff) {
      nvi_cache.toolkit_cs_ps_parallel = *(boolean *)pBuff;
      nvi.toolkit_cs_ps_parallel = nvi_cache.toolkit_cs_ps_parallel;
      (void) OEMNV_Put(NV_TOOLKIT_CS_PS_PARALLEL_I, &nvi);
   }
   return SUCCESS;
}
#endif /* FEATURE_GSTK_CS_PS_PARALLEL */

static int OEMPriv_GetItem_CFGI_CCBS(void *pBuff)
{
   *(OEMCCBS *) pBuff = nvi_cache.ccbs_sel;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_CCBS(void *pBuff)
{
#ifdef FEATURE_CCBS
#error code not present
#endif

   if (nvi_cache.ccbs_sel != *(OEMCCBS *)pBuff) {
      nvi_cache.ccbs_sel = *(OEMCCBS *)pBuff;
#ifdef FEATURE_CCBS
#error code not present
#endif
   }
   return SUCCESS;
}

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

static int OEMPriv_GetItem_CFGI_USB_HFK_ATTACHED(void *pBuff)
{
   if(oemi_cache.device_mask & OEMNV_USB_HFK_ATTACHED_MASK)
     *(boolean *) pBuff = TRUE;
   else{
     *(boolean *) pBuff = FALSE;
   }

   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_USB_HFK_ATTACHED(void *pBuff)
{
   if(*(boolean *) pBuff == TRUE){
     oemi_cache.device_mask |= OEMNV_USB_HFK_ATTACHED_MASK;
   }
   else{
     oemi_cache.device_mask &= ~OEMNV_USB_HFK_ATTACHED_MASK;
   }

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_STEREO_USB_HFK_ATTACHED(void *pBuff)
{
   if(oemi_cache.device_mask & OEMNV_STEREO_USB_HFK_ATTACHED_MASK)
     *(boolean *) pBuff = TRUE;
   else{
     *(boolean *) pBuff = FALSE;
   }

   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_STEREO_USB_HFK_ATTACHED(void *pBuff)
{
   if(*(boolean *) pBuff == TRUE){
     oemi_cache.device_mask |= OEMNV_STEREO_USB_HFK_ATTACHED_MASK;
   }
   else{
     oemi_cache.device_mask &= ~OEMNV_STEREO_USB_HFK_ATTACHED_MASK;
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

static int OEMPriv_GetItem_CFGI_DEVICE(void *pBuff)
{
   *(uint32 *) pBuff = oemi_cache.device_mask;
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

static int OEMPriv_GetItem_CFGI_PPP_AUTH(void *pBuff)
{
   *(byte *)pBuff = oemi_cache.ppp_auth_enabled;
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

static int OEMPriv_SetItem_CFGI_PPP_AUTH(void *pBuff)
{
   if (oemi_cache.ppp_auth_enabled != *(byte *)pBuff) {
      oemi_cache.ppp_auth_enabled = *(byte *)pBuff;
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
#ifdef FEATURE_UIM_RUN_TIME_ENABLE
static int OEMPriv_GetItem_CFGI_RTRE_CONFIGURATION(void *pBuff)
{
#ifndef WIN32
   nv_item_type nvi;

   if (NV_DONE_S != OEMNV_Get(NV_RTRE_CONFIG_I, &nvi)) {
      // Default to R-UIM only.
      #ifdef FEATURE_VERSION_S600S
		 *(nv_rtre_configuration_type *) pBuff = NV_RTRE_CONFIG_NV_ONLY;
	  #else
         *(nv_rtre_configuration_type *) pBuff = NV_RTRE_CONFIG_RUIM_ONLY;
	  #endif
      
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


static int OEMPriv_GetItem_CFGI_DEBUG_ECIO(void *pBuff)
{
   *(byte *)pBuff = (byte) 0;
   return EUNSUPPORTED;
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
   if (NV_DONE_S != OEMNV_Get(NV_MIN2_I, &nvi)) 
   {
      return EFAILED;
   }


   min2 = OEMPriv_MIN2_TO_DEC(nvi.min2.min2[CDMAMIN]);


   imsi_s[0] = '0' + (char) (min2 / 100);
   imsi_s[1] = '0' + (char) ( (min2 / 10) % 10);
   imsi_s[2] = '0' + (char) (min2 % 10);


   nvi.min1.nam = (byte) CM_NAM_1;
   if (NV_DONE_S != OEMNV_Get(NV_MIN1_I, &nvi)) 
   {
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

//Add By zzg 2012_02_02
#ifdef FEATURE_VERSION_W208S
	{
		AECHAR  wstrPrefix1[]={(AECHAR)'1',(AECHAR)'5', (AECHAR)'8', 0};
		AECHAR  wstrPrefix2[]={(AECHAR)'1',(AECHAR)'9', (AECHAR)'9', 0};

	
		imsi = (AECHAR *) pBuff;



		if ((WSTRNCMP(imsi, wstrPrefix1, 3) != 0)
			&& (WSTRNCMP(imsi, wstrPrefix2, 3) != 0))
	    {
	        return EFAILED;
	    }	    
	}	
#endif
//Add End

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
   //

   nvi.min2.nam = (byte) CM_NAM_1;
   nvi.min2.min2[CDMAMIN] = min2;

   
   if (NV_DONE_S != OEMNV_Put(NV_MIN2_I, &nvi)) {
      return EFAILED;
   }

   nvi.min1.nam = (byte) CM_NAM_1;
   nvi.min1.min1[FMMIN] = IMSI_MIN1_S1_ZERO;	//Add By zzg 2012_02_18
   nvi.min1.min1[CDMAMIN] = min1;


   if (NV_DONE_S != OEMNV_Put(NV_MIN1_I, &nvi)) {
      return EFAILED;
   }
#endif
   return SUCCESS;
}


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
#error code not present
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
#if defined(FEATURE_RFM_IOCTL)
  rfcom_ioctl_pointer_type rfm_ioctl_data;
  if (rfm_ioctl( RFCOM_TRANSCEIVER_0, RFCOM_IOCTL_GET_DBG_SCR_CHANNEL, &rfm_ioctl_data, 2) 
      != RFCOM_IOCTL_NO_ERROR )
  {
    *(uint16 *)pBuff = 0;
    return EUNSUPPORTED;
  }
  *(uint16 *)pBuff = rfm_ioctl_data.uint16_field;
  return SUCCESS;

#elif defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
   db_items_value_type dbi;

   db_get(DB_CHANNEL, &dbi);
   *(uint16 *)pBuff = dbi.channel;

   return SUCCESS;

#elif defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#error code not present
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
#ifndef FEATURE_ICM
   MSG_HIGH("OEMPriv_GetItem_CFGI_DEBUG_TX_AGC: FEATURE_ICM not turned on",0,0,0);
   *(uint16 *)pBuff = 0;
   return EFAILED;
#else //FEATURE_ICM
   ICM* pICM;
   boolean nReturnStatus;
   AEECMSysMode sys_mode;
  
   /* obtain shell instance */
   IShell *pIShell = AEE_GetShell();
   if (NULL == pIShell) {
    MSG_ERROR("OEMPriv_GetItem_CFGI_DEBUG_TX_AGC: NULL IShell Pointer",0,0,0);
    *(uint16 *)pBuff = 0;
    return EFAILED;
   }
   
   /* Create the Call Manager object. */
   nReturnStatus = ISHELL_CreateInstance(pIShell,
                                         AEECLSID_CM,
                                         (void **) &pICM);
   if ((pICM == NULL) || (nReturnStatus != SUCCESS))
   {
     MSG_ERROR("OEMPriv_GetItem_CFGI_DEBUG_TX_AGC: ICM object creation failed pICM: %d, nReturnStatus: %d",
               pICM,nReturnStatus,0);
     if (pICM != NULL)
     {
       (void) ICM_Release(pICM);
     }
     *(uint16 *)pBuff = 0;
     return EFAILED;
   }
   sys_mode = ICM_GetSysMode(pICM);
   (void) ICM_Release(pICM);

#if defined(FEATURE_RFM_IOCTL)
  sys_mode = sys_mode; /* not used but easier to do this than to featurize out */
  {
    rfcom_ioctl_pointer_type rfm_ioctl_data;
    if( rfm_ioctl( RFCOM_TRANSCEIVER_0, RFCOM_IOCTL_GET_DBG_SCR_TX_AGC, &rfm_ioctl_data, 2) 
        != RFCOM_IOCTL_NO_ERROR )
    {
      *(uint16 *)pBuff = 0;
      return EUNSUPPORTED;
    }
    *(uint16 *)pBuff = rfm_ioctl_data.uint16_field;
  }
  return SUCCESS;

#elif defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)
   if (sys_mode == AEECM_SYS_MODE_CDMA)
   {
     *(uint16 *)pBuff = (uint16) rf_get_cdma_tx_agc();
     return SUCCESS;
   }
   *(uint16 *)pBuff = 0;
   return EUNSUPPORTED;
   
#elif defined (FEATURE_WCDMA)
#error code not present
#else
   *(uint16 *)pBuff = 0;
   return EUNSUPPORTED;
#endif 

#endif //FEATURE_ICM
}

static int OEMPriv_GetItem_CFGI_DEBUG_TX_AGC_IN_DBM(void *pBuff)
{
#ifndef FEATURE_ICM
   MSG_HIGH("OEMPriv_GetItem_CFGI_DEBUG_TX_AGC_IN_DBM: FEATURE_ICM not turned on",0,0,0);
   *(int16 *)pBuff = 0;
   return EFAILED;
#else //FEATURE_ICM
   ICM* pICM;
   boolean nReturnStatus;
   AEECMSysMode sys_mode;
   
   /* obtain shell instance */
   IShell *pIShell = AEE_GetShell();
   if (NULL == pIShell) {
    MSG_ERROR("OEMPriv_GetItem_CFGI_DEBUG_TX_AGC_IN_DBM: NULL IShell Pointer",0,0,0);
    *(int16 *)pBuff = 0;
    return EFAILED;
   }
   
   /* Create the Call Manager object. */
   nReturnStatus = ISHELL_CreateInstance(pIShell,
                                         AEECLSID_CM,
                                         (void **) &pICM);
   if ((pICM == NULL) || (nReturnStatus != SUCCESS))
   {
     MSG_ERROR("OEMPriv_GetItem_CFGI_DEBUG_TX_AGC_IN_DBM: ICM object creation failed pICM: %d, nReturnStatus: %d",
               pICM,nReturnStatus,0);
     if (pICM != NULL)
     {
       (void) ICM_Release(pICM);
     }
     *(int16 *)pBuff = 0;
     return EFAILED;
   }
   sys_mode = ICM_GetSysMode(pICM);
   (void) ICM_Release(pICM);

#if defined(FEATURE_RFM_IOCTL)
  sys_mode = sys_mode; /* not used but easier to do this than to featurize out */
  {
    rfcom_ioctl_pointer_type rfm_ioctl_data;
    if( rfm_ioctl( RFCOM_TRANSCEIVER_0, RFCOM_IOCTL_GET_DBG_SCR_TX_AGC_IN_DBM, &rfm_ioctl_data, 2) 
        != RFCOM_IOCTL_NO_ERROR )
    {
      *(int16 *)pBuff = 0;
      return EUNSUPPORTED;
    }
    *(int16 *)pBuff = (int16)(rfm_ioctl_data.uint16_field);
  }
  return SUCCESS;

#elif (defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)) && defined(FEATURE_RF_ZIF) && defined (T_MSM6500)
   if (sys_mode == AEECM_SYS_MODE_CDMA)
   {
     *(int16 *)pBuff = rf_get_cdma_tx_agc_in_dBm();
     return SUCCESS;
   }
   *(int16 *)pBuff = 0;
   return EUNSUPPORTED;

#elif defined (FEATURE_WCDMA)
#error code not present
#else
   *(int16 *)pBuff = 0;
   return EUNSUPPORTED;
#endif 

#endif //FEATURE_ICM
}


static int OEMPriv_GetItem_CFGI_DEBUG_RX_AGC(void *pBuff)
{
#ifndef FEATURE_ICM
   MSG_HIGH("OEMPriv_GetItem_CFGI_DEBUG_RX_AGC: FEATURE_ICM not turned on",0,0,0);
   *(signed char *)pBuff = 0;
   return EFAILED;
#else //FEATURE_ICM
#if defined (FEATURE_CDMA1X)
   ICM* pICM;
   AEECMSysMode sys_mode;
   boolean nReturnStatus;
   IShell *pIShell;
   
   /* obtain shell instance */
   pIShell = AEE_GetShell();
   if (NULL == pIShell) {
     MSG_ERROR("OEMPriv_GetItem_CFGI_DEBUG_RX_AGC: NULL IShell Pointer",0,0,0);
     *(signed char *)pBuff = 0;
     return EFAILED;
   }
   
   /* Create the Call Manager object. */
   nReturnStatus = ISHELL_CreateInstance(pIShell,
                                         AEECLSID_CM,
                                         (void **) &pICM);
   if ((pICM == NULL) || (nReturnStatus != SUCCESS))
   {
     MSG_ERROR("OEMPriv_GetItem_CFGI_DEBUG_RX_AGC: ICM object creation failed pICM: %d, nReturnStatus: %d",
               pICM,nReturnStatus,0);
     if (pICM != NULL)
     {
       (void) ICM_Release(pICM);
     }
     *(signed char *)pBuff = 0;
     return EFAILED;
   }
   sys_mode = ICM_GetSysMode(pICM);
   (void) ICM_Release(pICM);

   if (sys_mode == AEECM_SYS_MODE_CDMA)
   {
#if defined(FEATURE_RFM_IOCTL)
     rfcom_ioctl_pointer_type rfm_ioctl_data;
     (void)rfm_ioctl( RFCOM_TRANSCEIVER_0, RFCOM_IOCTL_GET_DBG_SCR_RX_AGC, &rfm_ioctl_data, 1);
     *(byte *)pBuff = (byte)(rfm_ioctl_data.uint8_field);
#else
     *(signed char *)pBuff = rf_get_cdma_rx_agc();
#endif
     return SUCCESS;
   }
   else
   {
     *(signed char *)pBuff = 0;
     return EUNSUPPORTED;
   } 
#else /* FEATURE_CDMA1X */
   *(signed char *)pBuff = 0;
   return EUNSUPPORTED;
#endif //FEATURE_CDMA1X
#endif //FEATURE_ICM
}

static int OEMPriv_GetItem_CFGI_DEBUG_RX_AGC_IN_DBM(void *pBuff)
{
#ifndef FEATURE_ICM
   MSG_HIGH("OEMPriv_GetItem_CFGI_DEBUG_RX_AGC_IN_DBM: FEATURE_ICM not turned on",0,0,0);
   *(int32 *)pBuff = 0;
   return EFAILED;
#else //FEATURE_ICM

#ifndef FEATURE_RFM_IOCTL
#if (defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)) && defined(FEATURE_RF_ZIF)
   int16 ch0_rxagc;
   int16 ch1_rxagc;
#endif
#endif
   ICM* pICM;
   boolean nReturnStatus;
   AEECMSysMode sys_mode;
   
   /* obtain shell instance */
   IShell *pIShell = AEE_GetShell();
   if (NULL == pIShell) {
    MSG_ERROR("OEMPriv_GetItem_CFGI_DEBUG_RX_AGC_IN_DBM: NULL IShell Pointer",0,0,0);
    *(int32 *)pBuff = 0;
    return EFAILED;
   }
   
   /* Create the Call Manager object. */
   nReturnStatus = ISHELL_CreateInstance(pIShell,
                                         AEECLSID_CM,
                                         (void **) &pICM);
   if ((pICM == NULL) || (nReturnStatus != SUCCESS))
   {
     MSG_ERROR("OEMPriv_GetItem_CFGI_DEBUG_RX_AGC_IN_DBM: ICM object creation failed pICM: %d, nReturnStatus: %d",
               pICM,nReturnStatus,0);
     if (pICM != NULL)
     {
       (void) ICM_Release(pICM);
     }
     *(int32 *)pBuff = 0;
     return EFAILED;
   }
   sys_mode = ICM_GetSysMode(pICM);
   (void) ICM_Release(pICM);

#if defined(FEATURE_RFM_IOCTL)
  sys_mode = sys_mode; /* not used but easier to do this than to featurize out */
  {
    rfcom_ioctl_pointer_type rfm_ioctl_data;
    if( rfm_ioctl( RFCOM_TRANSCEIVER_0, RFCOM_IOCTL_GET_DBG_SCR_RX_AGC_IN_DBM, &rfm_ioctl_data, 4) 
        != RFCOM_IOCTL_NO_ERROR )
    {
      *(int32 *)pBuff = 0;
      return EUNSUPPORTED;
    }
    *(int32 *)pBuff = (int32)(rfm_ioctl_data.uint32_field);
  }
  return SUCCESS;

#elif (defined (FEATURE_CDMA_800) || defined (FEATURE_CDMA_1900)) && defined(FEATURE_RF_ZIF)
   if (sys_mode == AEECM_SYS_MODE_CDMA)
   {
     /* obtain Rx AGC readings from both Rx chains */
     ch0_rxagc = rf_get_path_cdma_rx_agc_in_dBm( RFCOM_TRANSCEIVER_0 );
     ch1_rxagc = rf_get_path_cdma_rx_agc_in_dBm( RFCOM_RECEIVER_1 );
  
     /* assemble into a 32 bit packet */
     *(int32 *)pBuff = (ch0_rxagc << 16) | (ch1_rxagc &0xFFFF );
     return SUCCESS;
   }
   *(int32 *) pBuff = 0;
   return EUNSUPPORTED;

#elif defined (FEATURE_WCDMA)
#error code not present
#else
   *(int32 *)pBuff = 0;
   return EUNSUPPORTED;
#endif 

#endif //FEATURE_ICM
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
#ifndef FEATURE_ICM
   MSG_HIGH("OEMPriv_GetItem_CFGI_DEBUG_TX_ADJUST: FEATURE_ICM not turned on",0,0,0);
   *(signed char *)pBuff = 0;
   return EFAILED;
#else //FEATURE_ICM
#ifdef FEATURE_RF_ZIF
   ICM* pICM;
   boolean nReturnStatus;
   AEECMSysMode sys_mode;
   
   /* obtain shell instance */
   IShell *pIShell = AEE_GetShell();
   if (NULL == pIShell) {
    MSG_ERROR("OEMPriv_GetItem_CFGI_DEBUG_TX_ADJUST: NULL IShell Pointer",0,0,0);
    *(signed char *)pBuff = 0;
    return EFAILED;
   }
   
   /* Create the Call Manager object. */
   nReturnStatus = ISHELL_CreateInstance(pIShell,
                                         AEECLSID_CM,
                                         (void **) &pICM);
   if ((pICM == NULL) || (nReturnStatus != SUCCESS))
   {
     MSG_ERROR("OEMPriv_GetItem_CFGI_DEBUG_TX_ADJUST: ICM object creation failed pICM: %d, nReturnStatus: %d",
               pICM,nReturnStatus,0);
     if (pICM != NULL)
     {
       (void) ICM_Release(pICM);
     }
     *(signed char *)pBuff = 0;
     return EFAILED;
   }
   sys_mode = ICM_GetSysMode(pICM);
   (void) ICM_Release(pICM);

   if (sys_mode == AEECM_SYS_MODE_CDMA)
   {
#if defined(FEATURE_RFM_IOCTL)
     rfcom_ioctl_pointer_type rfm_ioctl_data;
     (void)rfm_ioctl( RFCOM_TRANSCEIVER_0, RFCOM_IOCTL_GET_DBG_SCR_TX_ADJ, &rfm_ioctl_data, 1);
     *(signed char *)pBuff = (signed char)(rfm_ioctl_data.uint8_field);
#else
     *(signed char *)pBuff = rf_get_tx_gain_adj();
#endif
     return SUCCESS;
   }
   else
   {
     *(signed char *)pBuff = 0;
     return EUNSUPPORTED;
   }
#else
#error code not present
#endif
#endif //FEATURE_ICM
}

static int OEMPriv_GetItem_CFGI_DEBUG_HDET(void *pBuff)
{
#if defined(FEATURE_RFM_IOCTL)
  rfcom_ioctl_pointer_type rfm_ioctl_data;
  if( rfm_ioctl( RFCOM_TRANSCEIVER_0, RFCOM_IOCTL_GET_DBG_SCR_HDET, &rfm_ioctl_data, 1) 
      != RFCOM_IOCTL_NO_ERROR )
  {
    *(byte *)pBuff = 0;
    return EUNSUPPORTED;
  }
  *(byte *)pBuff = (byte)(rfm_ioctl_data.uint8_field);
  return SUCCESS;

#elif defined(FEATURE_RF_ZIF)
   *(byte *)pBuff = (byte) rf_hdet_data; /* adc_read(ADC_HDET_CELL); */
   return SUCCESS;
#elif defined (FEATURE_WCDMA)
#error code not present
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
#elif defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
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

#ifndef FEATURE_RFM_IOCTL
#if defined(FEATURE_RF_ZIF) && defined(T_MSM6500)
// Since these aren't prototyped in rf.h, we'll do it here.
extern lna_gain_state_type
       rf_get_path_lna_gain_state(rfcom_device_enum_type device);
extern boolean rf_is_sleeping ( rfcom_device_enum_type device);
#endif
#endif

static int OEMPriv_GetItem_CFGI_DEBUG_LNA_STATE(void *pBuff)
{

#if defined(FEATURE_RFM_IOCTL)
  rfcom_ioctl_pointer_type rfm_ioctl_data;
  if( rfm_ioctl( RFCOM_TRANSCEIVER_0, RFCOM_IOCTL_GET_DBG_SCR_LNA_STATE, &rfm_ioctl_data, 1) 
      != RFCOM_IOCTL_NO_ERROR )
  {
    *(byte *)pBuff = 0;
    return EUNSUPPORTED;
  }
  *(byte *)pBuff = (byte)(rfm_ioctl_data.uint8_field);
  return SUCCESS;

#elif defined(FEATURE_RF_ZIF) && defined(T_MSM6500)
   byte ch0_lna;
   byte ch1_lna;

   ch0_lna = rf_get_path_lna_gain_state( RFCOM_TRANSCEIVER_0 );
   ch1_lna = rf_get_path_lna_gain_state( RFCOM_RECEIVER_1 );

   *(byte *)pBuff = (ch0_lna << 4) | (ch1_lna & 0x0F);
   return SUCCESS;
#elif defined(FEATURE_RF_ZIF)
#error code not present
#elif defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#else
#error code not present
#endif
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_DEBUG_PA_STATE(void *pBuff)
{
#ifndef FEATURE_ICM
   MSG_HIGH("OEMPriv_GetItem_CFGI_DEBUG_PA_STATE: FEATURE_ICM not turned on",0,0,0);
   *(byte *)pBuff = 0;
   return EFAILED;
#else //FEATURE_ICM
   ICM* pICM;
   boolean nReturnStatus;
   AEECMSysMode sys_mode;
   
   /* obtain shell instance */
   IShell *pIShell = AEE_GetShell();
   if (NULL == pIShell) {
    MSG_ERROR("OEMPriv_GetItem_CFGI_DEBUG_PA_STATE: NULL IShell Pointer",0,0,0);
    *(byte *)pBuff = 0;
    return EFAILED;
   }
   
   /* Create the Call Manager object. */
   nReturnStatus = ISHELL_CreateInstance(pIShell,
                                         AEECLSID_CM,
                                         (void **) &pICM);
   if ((pICM == NULL) || (nReturnStatus != SUCCESS))
   {
     MSG_ERROR("OEMPriv_GetItem_CFGI_DEBUG_PA_STATE: ICM object creation failed pICM: %d, nReturnStatus: %d",
               pICM,nReturnStatus,0);
     if (pICM != NULL)
     {
       (void) ICM_Release(pICM);
     }
     *(byte *)pBuff = 0;
     return EFAILED;
   }
   sys_mode = ICM_GetSysMode(pICM);
   (void) ICM_Release(pICM);

#if defined(FEATURE_RFM_IOCTL)
  sys_mode = sys_mode; /* not used but easier to do this than to featurize out */
  {
    rfcom_ioctl_pointer_type rfm_ioctl_data;
    if( rfm_ioctl( RFCOM_TRANSCEIVER_0, RFCOM_IOCTL_GET_DBG_SCR_PA_RANGE, &rfm_ioctl_data, 1) 
        != RFCOM_IOCTL_NO_ERROR )
    {
      *(byte *)pBuff = 0;
      return EUNSUPPORTED;
    }
    *(byte *)pBuff = (byte)(rfm_ioctl_data.uint8_field);
    return SUCCESS;
  }

#elif defined(FEATURE_RF_ZIF)
   if (sys_mode == AEECM_SYS_MODE_CDMA)
   {
     *(byte *)pBuff = (byte) rf_get_pa_state();
     return SUCCESS;
   }
   *(byte *)pBuff = 0;
   return EUNSUPPORTED;

#elif defined(FEATURE_WCDMA)
#error code not present
#else
#error code not present
#endif 

#endif //FEATURE_ICM
}

static int OEMPriv_GetItem_CFGI_DEBUG_RATCHET_STATE(void *pBuff)
{
#ifndef FEATURE_ICM
   MSG_HIGH("OEMPriv_GetItem_CFGI_DEBUG_RATCHET_STATE: FEATURE_ICM not turned on",0,0,0);
   *(byte *)pBuff = 0;
   return EFAILED;
#else //FEATURE_ICM
#ifdef FEATURE_RF_ZIF
   ICM* pICM;
   boolean nReturnStatus;
   AEECMSysMode sys_mode;
   
   /* obtain shell instance */
   IShell *pIShell = AEE_GetShell();
   if (NULL == pIShell) {
    MSG_ERROR("OEMPriv_GetItem_CFGI_DEBUG_RATCHET_STATE: NULL IShell Pointer",0,0,0);
    *(byte *)pBuff = 0;
    return EFAILED;
   }
   
   /* Create the Call Manager object. */
   nReturnStatus = ISHELL_CreateInstance(pIShell,
                                         AEECLSID_CM,
                                         (void **) &pICM);
   if ((pICM == NULL) || (nReturnStatus != SUCCESS))
   {
     MSG_ERROR("OEMPriv_GetItem_CFGI_DEBUG_RATCHET_STATE: ICM object creation failed pICM: %d, nReturnStatus: %d",
               pICM,nReturnStatus,0);
     if (pICM != NULL)
     {
       (void) ICM_Release(pICM);
     }
     *(byte *)pBuff = 0;
     return EFAILED;
   }
   sys_mode = ICM_GetSysMode(pICM);
   (void) ICM_Release(pICM);

   if (sys_mode == AEECM_SYS_MODE_CDMA)
   {
#if defined(FEATURE_RFM_IOCTL)
     rfcom_ioctl_pointer_type rfm_ioctl_data;
     (void)rfm_ioctl( RFCOM_TRANSCEIVER_0, RFCOM_IOCTL_GET_DBG_SCR_RATCHET_STATE, &rfm_ioctl_data, 1);
     *(byte *)pBuff = (byte)(rfm_ioctl_data.uint8_field);
#else
     *(byte *)pBuff = (byte) rf_get_ratchet_state();
#endif
     return SUCCESS;
   }
#endif //FEATURE_RF_ZIF
   *(byte *)pBuff = 0;
   return EUNSUPPORTED;
#endif //FEATURE_ICM
}

#if defined(FEATURE_WCDMA)
#error code not present
#endif  // defined(FEATURE_WCDMA)

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
static int OEMPriv_GetItem_CFGI_DEBUG_RF_CARD_NV_ID(void *pBuff)
{
  byte rf_card, nv_type;

#if defined(FEATURE_RFM_IOCTL)
  rfcom_ioctl_pointer_type rfm_ioctl_data_hw, rfm_ioctl_data_nv;
  rfm_ioctl( RFCOM_TRANSCEIVER_0, RFCOM_IOCTL_GET_DBG_SCR_SUPPORTED_HW, &rfm_ioctl_data_hw, 1);
  rfm_ioctl( RFCOM_TRANSCEIVER_0, RFCOM_IOCTL_GET_DBG_SCR_NV_BAND, &rfm_ioctl_data_nv, 1);
  rf_card = (byte)(rfm_ioctl_data_hw.uint8_field);
  nv_type = (byte)(rfm_ioctl_data_nv.uint8_field);
#else
  rf_card = rf_get_supported_hw();
  nv_type = rf_get_nv_band( RFCOM_TRANSCEIVER_0 );
#endif
  *(word *)pBuff = (rf_card << 8)|(nv_type & 0xFF); 
  return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_DEBUG_THERM_BATT(void *pBuff)
{
   *(word *)pBuff = (byte) adc_read(ADC_BATT_THERM_DEGC);
   return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_DEBUG_THERM_MSM(void *pBuff)
{
   *(word *)pBuff = (byte) adc_read(ADC_MSM_THERM_DEGC);
   return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_DEBUG_THERM_PA(void *pBuff)
{
   *(word *)pBuff = (byte) adc_read(ADC_PA_THERM_DEGC);
   return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_DEBUG_THERM_PMIC(void *pBuff)
{
   *(word *)pBuff = (byte) adc_read(ADC_PMIC_THERM_DEGC);
   return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_DEBUG_THERM_CHRGR(void *pBuff)
{
   *(word *)pBuff = (byte) adc_read(ADC_CHG_THERM_DEGC);
   return SUCCESS;
}

#endif


static int OEMPriv_GetItem_CFGI_DEBUG_THERM(void *pBuff)
{
#if defined(FEATURE_RF_ZIF) || defined(FEATURE_WCDMA)
   *(word *)pBuff = (byte) therm_read();
   *(word *)pBuff = (*(word *)pBuff) << 8;
   *(word *)pBuff |= (((byte) adc_read(ADC_THERM)) & 0xFF);
   return SUCCESS;
#else
#error code not present
#endif
}

static int OEMPriv_GetItem_CFGI_DEBUG_BAND(void *pBuff)
{
#if defined(FEATURE_RFM_IOCTL)
  rfcom_ioctl_pointer_type rfm_ioctl_data;
  rfi_band_type band;
  if ( (rfm_ioctl( RFCOM_TRANSCEIVER_0, RFCOM_IOCTL_GET_SLEEP_STATE, &rfm_ioctl_data, 1)
        == RFCOM_IOCTL_NO_ERROR ) && (rfm_ioctl_data.uint8_field == TRUE) )
  {
    *(byte *)pBuff = CFGI_DEBUG_BAND_SLEEP;
  }
  else if( rfm_ioctl( RFCOM_TRANSCEIVER_0, RFCOM_IOCTL_GET_DBG_SCR_BAND_1X, &rfm_ioctl_data, 1) 
           == RFCOM_IOCTL_NO_ERROR )
  {
    band = (rfi_band_type)(rfm_ioctl_data.uint8_field);
    switch (band)
    {
    case RFI_GPS_BAND:
      *(byte *)pBuff = CFGI_DEBUG_BAND_GPS;
      break;
    default:
      *(byte *)pBuff = band;
      break;
    }
  }
  else if( rfm_ioctl( RFCOM_TRANSCEIVER_0, RFCOM_IOCTL_GET_DBG_SCR_BAND_GSM, &rfm_ioctl_data, 1) 
           == RFCOM_IOCTL_NO_ERROR )
  {
    *(byte *)pBuff = CFGI_DEBUG_BAND_GSM;
  }
  else if( rfm_ioctl( RFCOM_TRANSCEIVER_0, RFCOM_IOCTL_GET_DBG_SCR_BAND_WCDMA, &rfm_ioctl_data, 1) 
           == RFCOM_IOCTL_NO_ERROR )
  {
    *(byte *)pBuff = CFGI_DEBUG_BAND_WCDMA;
  }
  else
  {
    *(byte *)pBuff = 0;
    return EUNSUPPORTED;
  }
  return SUCCESS;

#elif defined(FEATURE_RF_ZIF)
#ifdef T_MSM6500
  /* Use RF API to determine RF state */
  rf_card_band_type band;
  word chan;

  if ( rf_is_sleeping ( RFCOM_TRANSCEIVER_0 )) 
  {
     *(byte *)pBuff = CFGI_DEBUG_BAND_SLEEP;
  }
  else 
  {
     rf_get_band_chan( &band, &chan );

     switch ( band )
     {
     case RF_GPS_BAND:
       *(byte *)pBuff = CFGI_DEBUG_BAND_GPS;
       break;
     case RF_FM_BAND:
       *(byte *)pBuff = CFGI_DEBUG_BAND_AMPS;
       break;
     default:
       /* assign the RF band class */
       *(byte *)pBuff = band;
       break;
     }
  } // rf_is_sleeping check
#else  // !T_MSM6500
#error code not present
#endif //T_MSM6500
  return SUCCESS;

#elif defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#else  // !defined(FEATURE_GSM) || defined(FEATURE_WCDMA)
#error code not present
#endif //FEATURE_RF_ZIF
}


#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR */

#ifdef FEATURE_QIPCALL
static int OEMPriv_GetItem_CFGI_QIPCALL_DEBUG_EVDO_IP_ADDR(void *pBuff)
{
   qipcall_debug_screen_type   qipcall_debug_info;

   qipcall_get_debug_display(&qipcall_debug_info);

   *(uint32 *) pBuff = qipcall_debug_info.evdo_ip_addr;

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_QIPCALL_DEBUG_EVDO_SIP_REG_STATUS(void *pBuff)
{
   qipcall_debug_screen_type   qipcall_debug_info;

   qipcall_get_debug_display(&qipcall_debug_info);

   *(byte *) pBuff = qipcall_debug_info.evdo_sip_reg_status;

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_QIPCALL_DEBUG_WLAN_IP_ADDR(void *pBuff)
{
   qipcall_debug_screen_type   qipcall_debug_info;

   qipcall_get_debug_display(&qipcall_debug_info);

   *(uint32 *) pBuff = qipcall_debug_info.wlan_ip_addr;

   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_QIPCALL_DEBUG_WLAN_SIP_REG_STATUS(void *pBuff)
{
   qipcall_debug_screen_type   qipcall_debug_info;

   qipcall_get_debug_display(&qipcall_debug_info);

   *(byte *) pBuff = qipcall_debug_info.wlan_sip_reg_status;

   return SUCCESS;
}
#endif

#if defined(FEATURE_GSM) || defined (FEATURE_WCDMA)
#error code not present
#endif /* defined(FEATURE_GSM) || defined (FEATURE_WCDMA) */

#ifdef FEATURE_INTELLICEIVER
static int OEMPriv_GetItem_CFGI_DEBUG_INTELLICEIVER_STATE(void *pBuff)
{
#if defined(FEATURE_RFM_IOCTL)
  rfcom_ioctl_pointer_type rfm_ioctl_data;
  (void)rfm_ioctl( RFCOM_TRANSCEIVER_0, 
                   RFCOM_IOCTL_GET_DBG_SCR_INTELLICEIVER_POWER_MODE, 
                   &rfm_ioctl_data, 
                   1); 
  *(byte *)pBuff = (byte)(rfm_ioctl_data.uint8_field);

#else
  *(byte *)pBuff = (byte)rf_intelliceiver_get_power_mode( RF_PATH_0 );
#endif
  
  return SUCCESS;
}
#endif /* FEATURE_INTELLICEIVER */

#if defined(FEATURE_USRSYSCLOCK) || defined(FEATURE_ISYSCLOCK)
static int OEMPriv_GetItem_CFGI_AUTO_TIME_ENABLE(void *pBuff)
{
   *(boolean *) pBuff = nvi_cache.auto_time_enable;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_AUTO_TIME_ENABLE(void *pBuff)
{
   nv_item_type nvi;

   if (nvi_cache.auto_time_enable != *(boolean *)pBuff) {
      nvi_cache.auto_time_enable = *(boolean *)pBuff;
      nvi.auto_time_enable = nvi_cache.auto_time_enable;
      (void) OEMNV_Put(NV_AUTO_TIME_ENABLE_I, &nvi);
   }
   return SUCCESS;
}
#endif 

static int OEMPriv_GetItem_CFGI_DISABLE_IN_CALL_DISP(void *pBuff)
{
   *(boolean *)pBuff = oemi_cache.disable_in_call_disp;
   return SUCCESS;
}

//lint -save -esym(818, pBuff)  Suppress "could be declared as const pointer"
static int OEMPriv_SetItem_CFGI_DISABLE_IN_CALL_DISP(void *pBuff)
{

   oemi_cache.disable_in_call_disp = *(boolean *)pBuff;
   return SUCCESS;
}
//lint -restore

static int OEMPriv_GetItem_CFGI_DISABLE_BG_IMAGE(void *pBuff)
{
   *(boolean *)pBuff = oemi_cache.disable_bg_image;
   return SUCCESS;
}

//lint -save -esym(818, pBuff)  Suppress "could be declared as const pointer"
static int OEMPriv_SetItem_CFGI_DISABLE_BG_IMAGE(void *pBuff)
{
   oemi_cache.disable_bg_image = *(boolean *)pBuff;
   return SUCCESS;
}
//lint -restore

static int OEMPriv_GetItem_CFGI_VOICEPRIVACY(void *pBuff)
{
  *(uint8 *) pBuff = nvi_cache.voice_priv;
   return SUCCESS;
}

//lint -save -esym(818, pBuff)  Suppress "could be declared as const pointer"
static int OEMPriv_SetItem_CFGI_VOICEPRIVACY(void *pBuff)
{
  nv_item_type nvi;

   if (nvi_cache.voice_priv != *(uint8 *)pBuff) {
      nvi_cache.voice_priv = *(uint8 *)pBuff;
      nvi.voice_priv = nvi_cache.voice_priv;
      (void) OEMNV_Put(NV_VOICE_PRIV_I, &nvi);
   }
   return SUCCESS;
}
//lint -restore

static int OEMPriv_GetItem_CFGI_MANUAL_PLMN_SEL_ALLOWED(void *pBuff)
{
   *(boolean *)pBuff = oemi_cache.manual_plmn_sel_allowed;
   return SUCCESS;
}

//lint -save -esym(818, pBuff)  Suppress "could be declared as const pointer"
static int OEMPriv_SetItem_CFGI_MANUAL_PLMN_SEL_ALLOWED(void *pBuff)
{
   oemi_cache.manual_plmn_sel_allowed = *(boolean *)pBuff;
   return SUCCESS;
}

#ifdef CUST_EDITION
//lint -restore
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
}
#endif

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

//摇一摇换音??
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

#if defined(FEATURE_VERSION_W317A) ||defined(FEATURE_VERSION_C316)
static int OEMPriv_GetItem_CFGI_MEDIAGALLERY_LOCK_CHECK(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.b_mediagallery_lock;
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_MEDIAGALLERY_LOCK_CHECK(void *pBuff)
{
   if (oemi_cache.b_mediagallery_lock != *(byte *)pBuff) {
      oemi_cache.b_mediagallery_lock = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}	
#endif
#ifdef FEATURE_VERSION_C316
static int OEMPriv_GetItem_CFGI_MOBILETRACKER_LOCK_CHECK(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.b_mobiletracker_lock;
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_MOBILETRACKER_LOCK_CHECK(void *pBuff)
{
  if (oemi_cache.b_mobiletracker_lock != *(byte *)pBuff) {
      oemi_cache.b_mobiletracker_lock = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_MOBILE_TRACKER_BACK(void *pBuff)
{
	*(byte *) pBuff = oemi_cache.b_mobile_tracker_back;
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_MOBILE_TRACKER_BACK(void *pBuff)
{
	if (oemi_cache.b_mobile_tracker_back != *(byte *)pBuff) {
      oemi_cache.b_mobile_tracker_back = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_MULTIMEDIA_LOCK_CHECK(void *pBuff)
{
   *(byte *) pBuff = oemi_cache.b_multimedia_lock;
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_MULTIMEDIA_LOCK_CHECK(void *pBuff)
{
  if (oemi_cache.b_multimedia_lock != *(byte *)pBuff) {
      oemi_cache.b_multimedia_lock = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_MOBILE_TRACKER_PHONENUMBTWO(void *pBuff)
{
	int len = STRLEN((void*)oemi_cache.mobile_tracker_number_2);
	MSG_FATAL("GetItem_CFGI_MOBILE_TRACKER_PHONENUMBTWO,,,,,,,=%d",len,0,0);
	MEMCPY(pBuff, oemi_cache.mobile_tracker_number_2, sizeof(uint16) * OEMNV_LOCKMUM_MAXLEN);
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_MOBILE_TRACKER_PHONENUMBTWO(void *pBuff)
{
	int len = STRLEN((void*)oemi_cache.mobile_tracker_number_2);
	MSG_FATAL("SetItem_CFGI_MOBILE_TRACKER_PHONENUMBTWO,,,,,,,len==%d",len,0,0);
	MEMCPY(oemi_cache.mobile_tracker_number_2, pBuff, sizeof(uint16) * OEMNV_LOCKMUM_MAXLEN);
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_MOBILE_TRACKER_PHONENUMBTHREE(void *pBuff)
{
	int len = STRLEN((void*)oemi_cache.mobile_tracker_number_3);
	MSG_FATAL("GetItem_CFGI_MOBILE_TRACKER_PHONENUMBTHREE,,,,,,,=%d",len,0,0);
	MEMCPY(pBuff, oemi_cache.mobile_tracker_number_3, sizeof(uint16) * OEMNV_LOCKMUM_MAXLEN);
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_MOBILE_TRACKER_PHONENUMBTHREE(void *pBuff)
{
	int len = STRLEN((void*)oemi_cache.mobile_tracker_number_3);
	MSG_FATAL("SetItem_CFGI_MOBILE_TRACKER_PHONENUMBTHREE,,,,,,,len==%d",len,0,0);
	MEMCPY(oemi_cache.mobile_tracker_number_3, pBuff, sizeof(uint16) * OEMNV_LOCKMUM_MAXLEN);
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}


static int OEMPriv_GetItem_CFGI_MOBILE_TRACKER_CONTECT(void *pBuff)
{
	int len = STRLEN((void*)oemi_cache.mobile_tracker_content);
	MSG_FATAL("GetItem_CFGI_MOBILE_TRACKER_PHONENUMBTHREE,,,,,,,=%d",len,0,0);
	MEMCPY(pBuff, oemi_cache.mobile_tracker_content, sizeof(uint16) * OEMNV_LOCKMUM_MAXLEN*6);
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_MOBILE_TRACKER_CONTECT(void *pBuff)
{
	int len = STRLEN((void*)oemi_cache.mobile_tracker_content);
	MSG_FATAL("SetItem_CFGI_MOBILE_TRACKER_PHONENUMBTHREE,,,,,,,len==%d",len,0,0);
	MEMCPY(oemi_cache.mobile_tracker_content, pBuff, sizeof(uint16) * OEMNV_LOCKMUM_MAXLEN*6);
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_MOBILETRACKER_PASSWORD(void *pBuff)
{
	*(uint16 *) pBuff = oemi_cache.mobiletracker_password;
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_MOBILETRACKER_PASSWORD(void *pBuff)
{
  if (oemi_cache.mobiletracker_password != *(uint16 *)pBuff) {
      oemi_cache.mobiletracker_password = *(uint16 *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_MOBILETRACKER_PASSWORD_CHECK(void *pBuff)
{
	*(byte *) pBuff = oemi_cache.mobiletracker_password_check;
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_MOBILETRACKER_PASSWORD_CHECK(void *pBuff)
{
   if (oemi_cache.mobiletracker_password_check != *(byte *)pBuff) {
      oemi_cache.mobiletracker_password_check = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}


#endif

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
        wms_msg_setmochannel(oemi_cache.wms_mochannel);
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
    MEMCPY(pBuff, oemi_cache.BUSY_ENABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_BUSY_ENABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.BUSY_ENABLE) != 0)
    {
        STRLCPY(oemi_cache.BUSY_ENABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_BUSY_DISABLE(void *pBuff)
{
    MEMCPY(pBuff, oemi_cache.BUSY_DISABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_BUSY_DISABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.BUSY_DISABLE) != 0)
    {
        STRLCPY(oemi_cache.BUSY_DISABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_NOANSWER_ENABLE(void *pBuff)
{
    MEMCPY(pBuff, oemi_cache.NOANSWER_ENABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_NOANSWER_ENABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.NOANSWER_ENABLE) != 0)
    {
        STRLCPY(oemi_cache.NOANSWER_ENABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_NOANSWER_DISABLE(void *pBuff)
{
    MEMCPY(pBuff, oemi_cache.NOANSWER_DISABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_NOANSWER_DISABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.NOANSWER_DISABLE) != 0)
    {
        STRLCPY(oemi_cache.NOANSWER_DISABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_UNCONDITIONAL_ENABLE(void *pBuff)
{
    MEMCPY(pBuff, oemi_cache.UNCONDITIONAL_ENABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_UNCONDITIONAL_ENABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.UNCONDITIONAL_ENABLE) != 0)
    {
        STRLCPY(oemi_cache.UNCONDITIONAL_ENABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_UNCONDITIONAL_DISABLE(void *pBuff)
{
    MEMCPY(pBuff, oemi_cache.UNCONDITIONAL_DISABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_UNCONDITIONAL_DISABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.UNCONDITIONAL_DISABLE) != 0)
    {
        STRLCPY(oemi_cache.UNCONDITIONAL_DISABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_UNREACHABLE_ENABLE(void *pBuff)
{
    MEMCPY(pBuff, oemi_cache.UNREACHABLE_ENABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_UNREACHABLE_ENABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.UNREACHABLE_ENABLE) != 0)
    {
        STRLCPY(oemi_cache.UNREACHABLE_ENABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_UNREACHABLE_DISABLE(void *pBuff)
{
    MEMCPY(pBuff, oemi_cache.UNREACHABLE_DISABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_UNREACHABLE_DISABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.UNREACHABLE_DISABLE) != 0)
    {
        STRLCPY(oemi_cache.UNREACHABLE_DISABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_DISABLE_ALL(void *pBuff)
{
    MEMCPY(pBuff, oemi_cache.DISABLE_ALL, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_DISABLE_ALL(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.DISABLE_ALL) != 0)
    {
        STRLCPY(oemi_cache.DISABLE_ALL, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_WAIT_ENABLE(void *pBuff)
{
    MEMCPY(pBuff, oemi_cache.WAIT_ENABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_WAIT_ENABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.WAIT_ENABLE) != 0)
    {
        STRLCPY(oemi_cache.WAIT_ENABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_WAIT_DISABLE(void *pBuff)
{
    MEMCPY(pBuff, oemi_cache.WAIT_DISABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_WAIT_DISABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.WAIT_DISABLE) != 0)
    {
        STRLCPY(oemi_cache.WAIT_DISABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_VOICEMAIL_ENABLE(void *pBuff)
{
    MEMCPY(pBuff, oemi_cache.VOICEMAIL_ENABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_VOICEMAIL_ENABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.VOICEMAIL_ENABLE) != 0)
    {
        STRLCPY(oemi_cache.VOICEMAIL_ENABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
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
    int i;
    
    MSG_FATAL("OEM_IsEmergency_Number Start",0,0,0);
    if(pNumber)
    {
        for (i = 0; i < oemi_cache.emerg_table.emert_size; i++)
        {
            //DBGPRINTF("%s %d OEM_IsEmergency_Number1",oemi_cache.emerg_table.emerg_num[i].num_buf,oemi_cache.emerg_table.emerg_num[i].num_len);
            if(len == oemi_cache.emerg_table.emerg_num[i].num_len)
            {
                if(STRCMP(pNumber,oemi_cache.emerg_table.emerg_num[i].num_buf) == 0)
                {
                    return TRUE;
                }
            }
        }
#ifdef FEATURE_OEMOMH         
        { 
        	IRUIM        *pIRUIM;
            IShell *pIShell = AEE_GetShell();
            if(NULL == pIShell)
            {
                return is_emergency;
            }
        	if (AEE_SUCCESS != ISHELL_CreateInstance(pIShell, AEECLSID_RUIM, (void **)&pIRUIM))
        	{
        		return is_emergency;
        	}     
            
#ifndef WIN32
            MSG_FATAL("IRUIM_Get_Ecc_Code len=%d",len,0,0);
            if (pIRUIM != NULL)
            {
                byte  Assnum[RUIM_ECC_NUMBER][RUIM_ECC_MAXSIZE+1];
                int   nNum;
                MEMSET(Assnum, 0x00, sizeof(Assnum));  
                MSG_FATAL("IRUIM_Get_Ecc_Code",0,0,0);
                
                if(SUCCESS == IRUIM_Get_Ecc_Code(pIRUIM,&Assnum[0][0],&nNum,RUIM_ECC_MAXSIZE+1))
                {
                    int   i;
                    byte *temp;
                    
                    MSG_FATAL("Assnum's nNum = %d",nNum,0,0);
                    for(i=0;i<nNum;i++)
                    {
                        temp = &Assnum[i][0];
                        if(STRCMP(pNumber,(char *)temp) == 0)
                        {
                            IRUIM_Release(pIRUIM);
                        	pIRUIM = NULL;
                            return TRUE;
                        }
                    }
                }
        	    IRUIM_Release(pIRUIM);
        	    pIRUIM = NULL;              
            }
#endif                
        }
#endif
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
   
   MEMCPY(pBuff, oemi_cache.fmRadio_chan_info, sizeof(sChanInfo) * MAX_FMRADIO_STORED_CHANNEL);
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_FMRADIO_CHAN_INFO(void *pBuff)
{
    MEMCPY(oemi_cache.fmRadio_chan_info, pBuff, sizeof(sChanInfo) * MAX_FMRADIO_STORED_CHANNEL);
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}

#ifdef FEATURE_VERSION_W208S
static int OEMPriv_GetItem_CFGI_SMS_RESTRICT_RECEIVE_INFO(void *pBuff)
{
   DBGPRINTF("OEMPriv_GetItem_CFGI_SMS_RESTRICT_RECEIVE_INFO Start");
   if(pBuff == NULL)
   {
        DBGPRINTF("pBuff == NULL"); 
   }
   MEMCPY(pBuff, oemi_cache.sms_restrict_recive, sizeof(sms_restrict_recive_info) * MAX_SMS_RESTRICT);
   DBGPRINTF("OEMPriv_GetItem_CFGI_SMS_RESTRICT_RECEIVE_INFO End");
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SMS_RESTRICT_RECEIVE_INFO(void *pBuff)
{
    MEMCPY(oemi_cache.sms_restrict_recive, pBuff, sizeof(sms_restrict_recive_info) * MAX_SMS_RESTRICT);
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
} 

static int OEMPriv_GetItem_CFGI_SMS_RESTRICT_RECEIVE_TOTAL(void *pBuff) 
{
   DBGPRINTF("CFGI_SMS_RESTRICT_RECEIVE_TOTAL = %d", oemi_cache.sms_restrict_receive_total); 
   *(uint8 *) pBuff = oemi_cache.sms_restrict_receive_total;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_SMS_RESTRICT_RECEIVE_TOTAL(void *pBuff)
{
   DBGPRINTF("SetItem_CFGI_SMS_RESTRICT_RECEIVE_TOTAL = %d", *(uint8 *)pBuff);  
   if (oemi_cache.sms_restrict_receive_total != *(uint8 *)pBuff) {
      oemi_cache.sms_restrict_receive_total = *(uint8 *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   DBGPRINTF("oemi_cache.sms_restrict_receive_total = %d", oemi_cache.sms_restrict_receive_total);  
   return SUCCESS;
}

#endif

#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)
static int OEMPriv_GetItem_CFGI_MOBILE_TRACKER_PHONENUMB(void *pBuff)
{
	int len = STRLEN((void*)oemi_cache.mobile_tracker_number);
	MSG_FATAL("GetItem_CFGI_MOBILE_TRACKER_PHONENUMB,,,,,,,=%d",len,0,0);
	MEMCPY(pBuff, oemi_cache.mobile_tracker_number, sizeof(uint16) * OEMNV_LOCKMUM_MAXLEN);
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_MOBILE_TRACKER_PHONENUMB(void *pBuff)
{

	int len = STRLEN((void*)oemi_cache.mobile_tracker_number);
	MSG_FATAL("SetItem_CFGI_MOBILE_TRACKER_PHONENUMB,,,,,,,len==%d",len,0,0);
	MEMCPY(oemi_cache.mobile_tracker_number, pBuff, sizeof(uint16) * OEMNV_LOCKMUM_MAXLEN);
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_MOBILE_TRACKER_IMSI(void *pBuff)
{	
	MEMCPY(pBuff, oemi_cache.mobile_tracker_imsi, sizeof(uint16) * OEMNV_LOCKIMSI_MAXLEN);
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_MOBILE_TRACKER_IMSI(void *pBuff)
{
	MEMCPY(oemi_cache.mobile_tracker_imsi, pBuff, sizeof(uint16) * OEMNV_LOCKIMSI_MAXLEN);
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_MOBILE_TRACKER_SETNUM(void *pBuff)
{
	*(boolean *) pBuff = oemi_cache.mobile_tracker_setnumb;
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_MOBILE_TRACKER_SETNUM(void *pBuff)
{
   if (oemi_cache.mobile_tracker_setnumb != *(boolean *)pBuff) 
  {
      oemi_cache.mobile_tracker_setnumb = *(boolean *)pBuff;
      OEMPriv_WriteOEMConfigList();
  }
  return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SMS_TRACKER_SEND_B(void *pBuff)
{
	*(boolean *) pBuff = oemi_cache.sms_tarcker_b;
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_SMS_TRACKER_SEND_B(void *pBuff)
{
  if (oemi_cache.sms_tarcker_b != *(boolean *)pBuff) 
  {
      oemi_cache.sms_tarcker_b = *(boolean *)pBuff;
      OEMPriv_WriteOEMConfigList();
  }
  return SUCCESS;
}

//uint32	sms_tarcker_time_uint		//CFGI_SMS_TRACKER_TIME
static int OEMPriv_GetItem_CFGI_SMS_TRACKER_TIME(void *pBuff)
{
   MEMCPY(pBuff, (void*) &oemi_cache.sms_tarcker_time_uint, sizeof(uint16));
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_SMS_TRACKER_TIME(void *pBuff)
{
	MEMCPY((void*) &oemi_cache.sms_tarcker_time_uint, pBuff, sizeof(uint16));
    OEMPriv_WriteOEMConfigList();
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_SMS_TRACKER_NUMBER(void *pBuff)
{

   MEMCPY(pBuff, oemi_cache.sms_tracker_number, sizeof(char) * OEMNV_LOCKMUM_MAXLEN);
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_SMS_TRACKER_NUMBER(void *pBuff)
{
	int len = STRLEN((void*)oemi_cache.sms_tracker_number);
	MSG_FATAL("OEMPriv_SetItem_CFGI_SMS_TRACKER_NUMBER,,,,,,,len==%d",len,0,0);
	MEMCPY(oemi_cache.sms_tracker_number, pBuff, sizeof(char) * OEMNV_LOCKMUM_MAXLEN);
	DBGPRINTF("pMe->m_strPhoneNUM=== %s",oemi_cache.sms_tracker_number);
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}

#endif

#ifdef FEATURE_VERSION_C337
static int OEMPriv_GetItem_CFGI_MIZONE_NUM(void *pBuff)
{
	/*int len = STRLEN((void*)oemi_cache.mizone_num);
	MSG_FATAL("OEMPriv_GetItem_CFGI_MIZONE_NUM,,,,,,,=%d",len,0,0);
	MEMCPY(pBuff, oemi_cache.mizone_num, sizeof(uint16) * OEMNV_LOCKMUM_MAXLEN);
    */
    MEMCPY(pBuff, (void*) &oemi_cache.mizone_num, sizeof(char) * MAS_BREWSETINT_STRING);
    DBGPRINTF("CFGI_MMS_USER_NAME %s",oemi_cache.mizone_num);
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_MIZONE_NUM(void *pBuff)
{

	/* int len = STRLEN((void*)oemi_cache.mizone_num);
	MSG_FATAL("OEMPriv_SetItem_CFGI_MIZONE_NUM,,,,,,,len==%d",len,0,0);
	MEMCPY(oemi_cache.mizone_num, pBuff, sizeof(uint16) * OEMNV_LOCKMUM_MAXLEN);
    OEMPriv_WriteOEMConfigList(); 
    */
    MEMCPY((void*) &oemi_cache.mizone_num, pBuff, sizeof(char) * MAS_BREWSETINT_STRING);
    DBGPRINTF("mizone_num %s",oemi_cache.mizone_num);
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_MIZONE_SMSINFO(void *pBuff)
{	
	//MEMCPY(pBuff, oemi_cache.mizone_smsinfo, sizeof(uint16) * OEMNV_LOCKMUM_MAXLEN);
	  MEMCPY(pBuff, (void*) &oemi_cache.mizone_smsinfo, sizeof(char) * MAS_BREWSETINT_STRING);
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_MIZONE_SMSINFO(void *pBuff)
{
	//MEMCPY(oemi_cache.mizone_smsinfo, pBuff, sizeof(uint16) * OEMNV_LOCKMUM_MAXLEN);
	MEMCPY((void*) &oemi_cache.mizone_smsinfo, pBuff, sizeof(char) * MAS_BREWSETINT_STRING);
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}

#endif

//Add by pyuangui 20121231
#if defined(FEATURE_VERSION_C316) || defined(FEATURE_VERSION_W317A)
static int OEMPriv_GetItem_CFGI_AUTOCALLRECORD(void *pBuff)
{
	MEMCPY(pBuff, (void*) &oemi_cache.autocallrecord, sizeof(boolean));
    DBGPRINTF("AUTOCALLRECORD %s",oemi_cache.autocallrecord);
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_AUTOCALLRECORD(void *pBuff)
{

	MEMCPY((void*) &oemi_cache.autocallrecord, pBuff, sizeof(boolean));
    DBGPRINTF("AUTOCALLRECORD %s",oemi_cache.autocallrecord);
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}
#endif
//Add End

#ifdef FEATURE_ANALOG_TV
static int OEMPriv_SetItem_CFGI_TV_OR_CAMERA(void *pBuff)
{
    MEMCPY((void*) &oemi_cache.tv_or_camera, pBuff, sizeof(uint32));
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_TV_OR_CAMERA(void *pBuff)
{
	 MEMCPY(pBuff, (void*) &oemi_cache.tv_or_camera, sizeof(uint32));
     return SUCCESS;
}
#endif
#ifdef FEATURE_LONG_NETLOCK
static int OEMPriv_GetItem_CFGI_NET_LOCK_FLAGS(void *pBuff)
{
	MEMCPY(pBuff, (void*) &oemi_cache.netlock_flg, sizeof(boolean));
	return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_NET_LOCK_FLAGS(void *pBuff)
{
	MEMCPY((void*) &oemi_cache.netlock_flg, pBuff, sizeof(boolean));
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}

#endif

#ifdef FEATURE_USES_MMS
static int OEMPriv_GetItem_CFGI_MMS_SETTING(void *pBuff)
{
	MEMCPY(pBuff, (void*) &oemi_cache.mms_setting, sizeof(CFG_MMsInfo));
	return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_MMS_SETTING(void *pBuff)
{
	MEMCPY((void*) &oemi_cache.mms_setting, pBuff, sizeof(CFG_MMsInfo));
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}
#endif


//Add By  zzg 2010_11_22
#ifdef FEATURE_APP_BLUETOOTH
static int OEMPriv_GetItem_CFGI_BT_STATUS(void *pBuff) 
{
  MEMCPY(pBuff, (void*) &oemi_cache.bt_status, sizeof(boolean));
	return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_BT_STATUS(void *pBuff) 
{
   MEMCPY((void*) &oemi_cache.bt_status, pBuff, sizeof(boolean));
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}
#endif
//Add End
static int OEMPriv_GetItem_CFGI_FLSHLITHG_STATUS(void *pBuff)
{
	MEMCPY(pBuff, (void*) &oemi_cache.flashlight_status, sizeof(boolean));
	return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_FLSHLITHG_STATUS(void *pBuff)
{
	MEMCPY((void*) &oemi_cache.flashlight_status, pBuff, sizeof(boolean));
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_FM_PLAY_MODE(void *pBuff)
{
	MEMCPY(pBuff, (void*) &oemi_cache.FMPlayMode_status, sizeof(boolean));
	return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_FM_PLAY_MODE(void *pBuff)
{
	MEMCPY((void*) &oemi_cache.FMPlayMode_status, pBuff, sizeof(boolean));
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_WMSWRITD_END_STATUS(void *pBuff)
{
	MEMCPY(pBuff, (void*) &oemi_cache.wmswriteend_status, sizeof(boolean));
	return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_WMSWRITD_END_STATUS(void *pBuff)
{
	MEMCPY((void*) &oemi_cache.wmswriteend_status, pBuff, sizeof(boolean));
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
#ifdef FEATURE_USES_MMS   
static int OEMPriv_GetItem_CFGI_MMSOUTDATA_INFO(void * pBuff)
{
   MSG_FATAL("OEMPriv_GetItem_CFGI_MMSOUTDATA_INFO Start",0,0,0);
   MEMCPY(pBuff, (void*)(oemi_cache.MMSOutDataInfo), sizeof(MMSData) * MAX_MMS_STORED);
   MSG_FATAL("OEMPriv_GetItem_CFGI_MMSOUTDATA_INFO ENd",0,0,0);
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_MMSOUTDATA_INFO(void *pBuff)
{
    MSG_FATAL("OEMPriv_SetItem_CFGI_MMSOUTDATA_INFO Start", 0,0,0);
    MEMCPY((void*)(oemi_cache.MMSOutDataInfo), pBuff, sizeof(MMSData) * MAX_MMS_STORED);
    OEMPriv_WriteOEMConfigList(); 
    MSG_FATAL("OEMPriv_SetItem_CFGI_MMSOUTDATA_INFO End", 0,0,0);
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_MMS_OUTCOUNT(void *pBuff) 
{
    MSG_FATAL("OEMPriv_GetItem_CFGI_MMS_OUTCOUNT Start mmsCount=%d", oemi_cache.mmsOutCount,0,0); 
   *(uint8 *) pBuff = oemi_cache.mmsOutCount;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_MMS_OUTCOUNT(void *pBuff)
{
   MSG_FATAL("OEMPriv_SetItem_CFGI_MMS_OUTCOUNT Start *pBuff=%d", *(uint8*)pBuff,0,0); 
   if (oemi_cache.mmsOutCount != *(uint8 *)pBuff) {
      oemi_cache.mmsOutCount = *(uint8 *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   MSG_FATAL("OEMPriv_SetItem_CFGI_MMS_OUTCOUNT Start mmsCount=%d", oemi_cache.mmsOutCount,0,0); 
   return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_MMSINDATA_INFO(void *pBuff) 
{
   MSG_FATAL("OEMPriv_GetItem_CFGI_MMSINDATA_INFO Start",0,0,0);
   MEMCPY(pBuff, (void*)(oemi_cache.MMSInDataInfo), sizeof(MMSData) * MAX_MMS_STORED);
   MSG_FATAL("OEMPriv_GetItem_CFGI_MMSINDATA_INFO ENd",0,0,0);
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_MMSINDATA_INFO(void *pBuff)
{
    MSG_FATAL("OEMPriv_SetItem_CFGI_MMSINDATA_INFO Start", 0,0,0);
    MEMCPY((void*)(oemi_cache.MMSInDataInfo), pBuff, sizeof(MMSData) * MAX_MMS_STORED);
    OEMPriv_WriteOEMConfigList(); 
    MSG_FATAL("OEMPriv_SetItem_CFGI_MMSINDATA_INFO End", 0,0,0);
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_MMS_INCOUNT(void *pBuff) 
{
    MSG_FATAL("OEMPriv_GetItem_CFGI_MMS_INCOUNT Start mmsCount=%d", oemi_cache.mmsInCount,0,0); 
   *(uint8 *) pBuff = oemi_cache.mmsInCount;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_MMS_INCOUNT(void *pBuff)
{
   MSG_FATAL("OEMPriv_SetItem_CFGI_MMS_INCOUNT Start *pBuff=%d", *(uint8*)pBuff,0,0); 
   if (oemi_cache.mmsInCount != *(uint8 *)pBuff) {
      oemi_cache.mmsInCount = *(uint8 *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   MSG_FATAL("OEMPriv_SetItem_CFGI_MMS_INCOUNT Start mmsCount=%d", oemi_cache.mmsInCount,0,0); 
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_MMSDRAFTDATA_INFO(void *pBuff) 
{
   MSG_FATAL("OEMPriv_GetItem_CFGI_MMSDRAFTDATA_INFO Start",0,0,0);
   MEMCPY(pBuff, (void*)(oemi_cache.MMSDraftDataInfo), sizeof(MMSData) * MAX_MMS_STORED);
   MSG_FATAL("OEMPriv_GetItem_CFGI_MMSDRAFTDATA_INFO ENd",0,0,0);
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_MMSDRAFTDATA_INFO(void *pBuff)
{
    MSG_FATAL("OEMPriv_SetItem_CFGI_MMSDRAFTDATA_INFO Start", 0,0,0);
    MEMCPY((void*)(oemi_cache.MMSDraftDataInfo), pBuff, sizeof(MMSData) * MAX_MMS_STORED);
    OEMPriv_WriteOEMConfigList(); 
    MSG_FATAL("OEMPriv_SetItem_CFGI_MMSDRAFTDATA_INFO End", 0,0,0);
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_MMS_DRAFTCOUNT(void *pBuff) 
{
    MSG_FATAL("OEMPriv_GetItem_CFGI_MMS_DraftCOUNT Start mmsCount=%d", oemi_cache.mmsDraftCount,0,0); 
   *(uint8 *) pBuff = oemi_cache.mmsDraftCount;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_MMS_DRAFTCOUNT(void *pBuff)
{
   MSG_FATAL("OEMPriv_SetItem_CFGI_MMS_DraftCOUNT Start *pBuff=%d", *(uint8*)pBuff,0,0); 
   if (oemi_cache.mmsDraftCount != *(uint8 *)pBuff) {
      oemi_cache.mmsDraftCount = *(uint8 *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   MSG_FATAL("OEMPriv_SetItem_CFGI_MMS_DraftCOUNT Start mmsCount=%d", oemi_cache.mmsDraftCount,0,0); 
   return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_WMS_MMSNOTIFY(void *pBuff) 
{
    MSG_FATAL("OEMPriv_GetItem_CFGI_MMS_INCOUNT Start mmsCount=%d", oemi_cache.mmsNotify,0,0); 
   *(uint8 *) pBuff = oemi_cache.mmsNotify;
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_WMS_MMSNOTIFY(void *pBuff)
{
   MSG_FATAL("OEMPriv_SetItem_CFGI_MMS_INCOUNT Start *pBuff=%d", *(uint8*)pBuff,0,0); 
   if (oemi_cache.mmsNotify != *(boolean *)pBuff) {
      oemi_cache.mmsNotify = *(boolean *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   MSG_FATAL("OEMPriv_SetItem_CFGI_MMS_INCOUNT Start mmsNotify=%d", oemi_cache.mmsNotify,0,0); 
   return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_WMS_MMSDELIVERYREPORT(void *pBuff) 
{
    MSG_FATAL("OEMPriv_GetItem_CFGI_MMS_INCOUNT Start mmsCount=%d", oemi_cache.mmsDeliverReport,0,0); 
    *(int8 *) pBuff = oemi_cache.mmsDeliverReport;
    return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_WMS_MMSDELIVERYREPORT(void *pBuff)
{
    MSG_FATAL("OEMPriv_SetItem_CFGI_MMS_INCOUNT Start *pBuff=%d", *(int8*)pBuff,0,0); 
    oemi_cache.mmsDeliverReport = *(int8 *)pBuff;
    OEMPriv_WriteOEMConfigList();
    MSG_FATAL("OEMPriv_SetItem_CFGI_MMS_INCOUNT Start mmsNotify=%d", oemi_cache.mmsDeliverReport,0,0); 
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_WMS_READREPLY(void *pBuff) 
{
    MSG_FATAL("OEMPriv_GetItem_CFGI_MMS_INCOUNT Start mmsCount=%d", oemi_cache.mmsReadReply,0,0); 
    *(int8 *) pBuff = oemi_cache.mmsReadReply;
    return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_WMS_READREPLY(void *pBuff)
{
    MSG_FATAL("OEMPriv_SetItem_CFGI_MMS_INCOUNT Start *pBuff=%d", *(int8*)pBuff,0,0); 
    oemi_cache.mmsReadReply = *(int8 *)pBuff;
    OEMPriv_WriteOEMConfigList();
    MSG_FATAL("OEMPriv_SetItem_CFGI_MMS_INCOUNT Start mmsNotify=%d", oemi_cache.mmsReadReply,0,0); 
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_WMS_REPORTALLOWED(void *pBuff) 
{
    MSG_FATAL("OEMPriv_GetItem_CFGI_MMS_INCOUNT Start mmsCount=%d", oemi_cache.mmsReportAllowed,0,0); 
    *(int8 *) pBuff = oemi_cache.mmsReportAllowed;
    return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_WMS_REPORTALLOWED(void *pBuff)
{
    MSG_FATAL("OEMPriv_SetItem_CFGI_MMS_INCOUNT Start *pBuff=%d", *(int8*)pBuff,0,0); 
    oemi_cache.mmsReportAllowed = *(int8 *)pBuff;
    OEMPriv_WriteOEMConfigList();
    MSG_FATAL("OEMPriv_SetItem_CFGI_MMS_INCOUNT Start mmsNotify=%d", oemi_cache.mmsReportAllowed,0,0); 
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_WMS_SENDERVISIBILITY(void *pBuff) 
{
    MSG_FATAL("OEMPriv_GetItem_CFGI_MMS_INCOUNT Start mmsCount=%d", oemi_cache.mmsSenderVisibility,0,0); 
    *(int8 *) pBuff = oemi_cache.mmsSenderVisibility;
    return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_WMS_SENDERVISIBILITY(void *pBuff)
{
    MSG_FATAL("OEMPriv_SetItem_CFGI_MMS_INCOUNT Start *pBuff=%d", *(int8*)pBuff,0,0); 
    oemi_cache.mmsSenderVisibility = *(int8 *)pBuff;
    OEMPriv_WriteOEMConfigList();
    MSG_FATAL("OEMPriv_SetItem_CFGI_MMS_INCOUNT Start mmsNotify=%d", oemi_cache.mmsSenderVisibility,0,0); 
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_MMS_USER_NAME(void *pBuff)
{
	 MEMCPY(pBuff, (void*) &oemi_cache.s_MMSUserName, sizeof(char) * MAS_BREWSETINT_STRING);
     DBGPRINTF("CFGI_MMS_USER_NAME %s",oemi_cache.s_MMSUserName);
     return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_MMS_USER_NAME(void *pBuff)
{
	MEMCPY((void*) &oemi_cache.s_MMSUserName, pBuff, sizeof(char) * MAS_BREWSETINT_STRING);
    DBGPRINTF("CFGI_MMS_USER_NAME %s",oemi_cache.s_MMSUserName);
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_MMS_PASSWORD(void *pBuff)
{
	 MEMCPY(pBuff, (void*) &oemi_cache.s_MMSPassword, sizeof(char) * MAS_BREWSETINT_STRING);
     DBGPRINTF("CFGI_MMS_PASSWORD %s",oemi_cache.s_MMSPassword);
     return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_MMS_PASSWORD(void *pBuff)
{
	MEMCPY((void*) &oemi_cache.s_MMSPassword, pBuff, sizeof(char) * MAS_BREWSETINT_STRING);
    DBGPRINTF("CFGI_MMS_PASSWORD %s",oemi_cache.s_MMSPassword);
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_MMS_PROXY(void *pBuff)
{
	 MEMCPY(pBuff, (void*) &oemi_cache.s_MMSProxy, sizeof(char) * MAX_MMS_PROXY);
     DBGPRINTF("CFGI_MMS_PROXY %s",oemi_cache.s_MMSProxy);
     return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_MMS_PROXY(void *pBuff)
{
	MEMCPY((void*) &oemi_cache.s_MMSProxy, pBuff, sizeof(char) * MAX_MMS_PROXY);
    DBGPRINTF("CFGI_MMS_PROXY %s",oemi_cache.s_MMSProxy);
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_MMS_PORT(void *pBuff)
{
	 MEMCPY(pBuff, (void*) &oemi_cache.s_MMSPort, sizeof(char) * MAX_MMS_PORT);
     DBGPRINTF("CFGI_MMS_PORT %s",oemi_cache.s_MMSPort);
     return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_MMS_PORT(void *pBuff)
{
	MEMCPY((void*) &oemi_cache.s_MMSPort, pBuff, sizeof(char) * MAX_MMS_PORT);
    DBGPRINTF("CFGI_MMS_PORT %s",oemi_cache.s_MMSPort);
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}

#endif
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

#ifdef FEATURE_VERSION_C337
static int OEMPriv_GetItem_CFGI_DEFAULTCONT(void *pBuff) 
{
   MEMCPY(pBuff, (void*) &oemi_cache.m_defaultcont, sizeof(boolean));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_DEFAULTCONT(void *pBuff) 
{
    MEMCPY((void*) &oemi_cache.m_defaultcont, pBuff, sizeof(boolean));
    OEMPriv_WriteOEMConfigList();
    return SUCCESS;
}
#endif
#ifdef FEATURE_VERSION_C316
static int OEMPriv_GetItem_CFGI_ONEKEY_LOCK_KEYPAD(void *pBuff) 
{
   MEMCPY(pBuff, (void*) &oemi_cache.m_onekey_lock_keypad, sizeof(boolean));
   return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_ONEKEY_LOCK_KEYPAD(void *pBuff) 
{
    MEMCPY((void*) &oemi_cache.m_onekey_lock_keypad, pBuff, sizeof(boolean));
    OEMPriv_WriteOEMConfigList();
    return SUCCESS;
}
#endif
static int OEMPriv_GetItem_CFGI_BREWSET_USENAME(void *pBuff)
{
	 MEMCPY(pBuff, (void*) &oemi_cache.brewsetings_usename, sizeof(byte) * MAS_BREWSETINT_STRING);
     DBGPRINTF("CFGI_BREWSET_USENAME %s",oemi_cache.brewsetings_usename);
     return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_BREWSET_USENAME(void *pBuff)
{
	MEMCPY((void*) &oemi_cache.brewsetings_usename, pBuff, sizeof(byte) * MAS_BREWSETINT_STRING);
    DBGPRINTF("CFGI_BREWSET_USENAME %s",oemi_cache.brewsetings_usename);
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}

static int OEMPriv_GetItem_CFGI_BREWSET_PASSWORD(void *pBuff)
{
	MEMCPY(pBuff, (void*) &oemi_cache.brewsetings_password, sizeof(byte) * MAS_BREWSETINT_STRING);
    DBGPRINTF("CFGI_BREWSET_PASSWORD %s",oemi_cache.brewsetings_password);
    return SUCCESS;
}

static int OEMPriv_SetItem_CFGI_BREWSET_PASSWORD(void *pBuff)
{
	MEMCPY((void*) &oemi_cache.brewsetings_password, pBuff, sizeof(byte) * MAS_BREWSETINT_STRING);
    DBGPRINTF("CFGI_BREWSET_PASSWORD %s",oemi_cache.brewsetings_password);
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_IDLE_DATETIME_MODE(void *pBuff)
{
	MEMCPY((void*) &oemi_cache.Idle_datetime_mode, pBuff, sizeof(byte));
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_IDLE_DATETIME_MODE(void *pBuff)
{
	MEMCPY(pBuff, (void*) &oemi_cache.Idle_datetime_mode, sizeof(byte));
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CAMERA_PHOTO_MODE(void *pBuff)
{
	MEMCPY((void*) &oemi_cache.Camera_Photo_mode, pBuff, sizeof(byte));
    OEMPriv_WriteOEMConfigList(); 
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CAMERA_PHOTO_MODE(void *pBuff)
{
	MEMCPY(pBuff, (void*) &oemi_cache.Camera_Photo_mode, sizeof(byte));
    return SUCCESS;
}

#ifdef FEATURE_VERSION_W208S
static int OEMPriv_SetItem_CFGI_OTKSL_TIMES(void *pBuff)
{
	MSG_FATAL("OEMPriv_SetItem_CFGI_OTKSL_TIMES Start *otksl_times=%d", *(uint8*)pBuff,0,0); 
	if (oemi_cache.otksl_times != *(uint8 *)pBuff)
	{
		oemi_cache.otksl_times = *(uint8 *)pBuff;
		OEMPriv_WriteOEMConfigList();
	}
	MSG_FATAL("OEMPriv_SetItem_CFGI_OTKSL_TIMES Start otksl_times=%d", oemi_cache.mmsOutCount,0,0); 
	return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_OTKSL_TIMES(void *pBuff)
{
	*(uint8 *) pBuff = oemi_cache.otksl_times;
	return SUCCESS;
}
#endif

static int OEMPriv_SetItem_CFGI_LANGUAGE_MOD(void *pBuff)
{
	memcpy((void *)&oemi_cache.brew_laguagefalgs,pBuff,sizeof(boolean));
    OEMPriv_WriteOEMConfigList();
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_LANGUAGE_MOD(void *pBuff)
{
	memcpy(pBuff,(void *)&oemi_cache.brew_laguagefalgs,sizeof(boolean));
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
#ifdef FEATURE_LCD_TOUCH_ENABLE
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
#ifdef FEATURE_MENU_STYLE
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
#endif
#ifdef FEATURE_CAMERA_MULTI_SENSOR
static int OEMPriv_GetItem_CFGI_CAMERA_MULTI(void *pBuff)
{
	*(byte *) pBuff = oemi_cache.camera_multi;
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CAMERA_MULTI(void *pBuff)
{
	if (oemi_cache.camera_multi != *(byte *)pBuff) {
      oemi_cache.camera_multi = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}
static int OEMPriv_GetItem_CFGT_CAMERA_FLASH(void *pBuff)
{
	*(byte *) pBuff = oemi_cache.camera_flash;
   return SUCCESS;
}
static int OEMPriv_SetItem_CFGT_CAMERA_FLASH(void *pBuff)
{
	if (oemi_cache.camera_flash != *(byte *)pBuff) {
      oemi_cache.camera_flash = *(byte *)pBuff;
      OEMPriv_WriteOEMConfigList();
   }
   return SUCCESS;
}

#endif

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

#ifdef FEATURE_ANALOG_TV
static int OEMPriv_GetItem_CFGI_TV_SETCHANNL(void *pBuff)
{
    MEMCPY(pBuff, (void*) &oemi_cache.tv_set, sizeof(CFG_TvSetting));
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_TV_SETCHANNL(void *pBuff)
{
    MEMCPY((void*) &oemi_cache.tv_set, pBuff, sizeof(CFG_TvSetting));
    OEMPriv_WriteOEMConfigList();
    return SUCCESS;
}
#endif
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
    MEMCPY(pBuff, oemi_cache.BUSY_VOICEMAIL, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_BUSY_VOICEMAIL(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.BUSY_VOICEMAIL) != 0)
    {
        STRLCPY(oemi_cache.BUSY_VOICEMAIL, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_NOANSWER_VOICEMAIL(void *pBuff)
{
    MEMCPY(pBuff, oemi_cache.NOANSWER_VOICEMAIL, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_NOANSWER_VOICEMAIL(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.NOANSWER_VOICEMAIL) != 0)
    {
        STRLCPY(oemi_cache.NOANSWER_VOICEMAIL, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_NOCONNECT_VOICEMAIL(void *pBuff)
{
    MEMCPY(pBuff, oemi_cache.NOCONNECT_VOICEMAIL, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_NOCONNECT_VOICEMAIL(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.NOCONNECT_VOICEMAIL) != 0)
    {
        STRLCPY(oemi_cache.NOCONNECT_VOICEMAIL, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_ANYWAY_VOICEMAIL(void *pBuff)
{
    MEMCPY(pBuff, oemi_cache.ANYWAY_VOICEMAIL, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_ANYWAY_VOICEMAIL(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.ANYWAY_VOICEMAIL) != 0)
    {
        STRLCPY(oemi_cache.ANYWAY_VOICEMAIL, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_WAIT_TEMP_DISABLE(void *pBuff)
{
    MEMCPY(pBuff, oemi_cache.WAIT_TEMP_DISABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_WAIT_TEMP_DISABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.WAIT_TEMP_DISABLE) != 0)
    {
        STRLCPY(oemi_cache.WAIT_TEMP_DISABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_DND_ENABLE(void *pBuff)
{
    MEMCPY(pBuff, oemi_cache.DND_ENABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_DND_ENABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.DND_ENABLE) != 0)
    {
        STRLCPY(oemi_cache.DND_ENABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_DND_DISABLE(void *pBuff)
{
    MEMCPY(pBuff, oemi_cache.DND_DISABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_DND_DISABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.DND_DISABLE) != 0)
    {
        STRLCPY(oemi_cache.DND_DISABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_CNIR_ENABLE(void *pBuff)
{
    MEMCPY(pBuff, oemi_cache.CNIR_ENABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_CNIR_ENABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.CNIR_ENABLE) != 0)
    {
        STRLCPY(oemi_cache.CNIR_ENABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
static int OEMPriv_GetItem_CFGI_CALLFORWARD_CNIR_DISABLE(void *pBuff)
{
    MEMCPY(pBuff, oemi_cache.CNIR_DISABLE, FEATURE_CODE_MAX_LENTH);
    return SUCCESS;
}
static int OEMPriv_SetItem_CFGI_CALLFORWARD_CNIR_DISABLE(void *pBuff)
{
    if(STRCMP((char*)pBuff,(char*)oemi_cache.CNIR_DISABLE) != 0)
    {
        STRLCPY(oemi_cache.CNIR_DISABLE, (char*)pBuff, FEATURE_CODE_MAX_LENTH);
        OEMPriv_WriteOEMConfigList();
    }
    return SUCCESS;
}
#endif
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
#if defined(FEATURE_OEMOMH)
#include "Gsdi.h"
void OEM_SetBAM_ADSAccount(void)
{
#ifndef WIN32
    if(!gsdi_uim_omh_cap.omh_enabled)
    {
        nv_item_type nvi;
        char username[MAS_BREWSETINT_STRING] = {0};
        char password[MAS_BREWSETINT_STRING] = {0};

        OEMPriv_GetItem_CFGI_BREWSET_USENAME((void*)username);
        OEMPriv_GetItem_CFGI_BREWSET_PASSWORD((void*)password);
        DBGPRINTF("OEM_SetBAM_ADSAccount username=%s ",username);
        DBGPRINTF("OEM_SetBAM_ADSAccoun passwordt=%s",password);

        // 账号
        (void)STRCPY((char *)nvi.pap_user_id.user_id, (char *)username);
        nvi.pap_user_id.user_id_len = STRLEN((char *)username);
        (void)OEMNV_Put(NV_PPP_USER_ID_I, &nvi);


        // 账号密码
        (void)STRCPY((char *)nvi.pap_password.password, (char *)password);
        nvi.pap_password.password_len = STRLEN((char *)password);
        (void)OEMNV_Put(NV_PPP_PASSWORD_I, &nvi);
    }
#endif
} /* OEM_SetBAM_ADSAccount */

void OEM_SetUCBROWSER_ADSAccount(void)
{
#ifndef WIN32
    if(!gsdi_uim_omh_cap.omh_enabled)
    {
        nv_item_type nvi;
        char username[MAS_BREWSETINT_STRING] = {0};
        char password[MAS_BREWSETINT_STRING] = {0};

        OEMPriv_GetItem_CFGI_BREWSET_USENAME((void*)username);
        OEMPriv_GetItem_CFGI_BREWSET_PASSWORD((void*)password);
        DBGPRINTF("OEM_SetBROWSER_ADSAccount username=%s ",username);
        DBGPRINTF("OEM_SetBROWSER_ADSAccoun passwordt=%s",password);
        
        // 账号
        (void)STRCPY((char *)nvi.pap_user_id.user_id, (char *)username);
        nvi.pap_user_id.user_id_len = STRLEN((char *)username);
        (void)OEMNV_Put(NV_PPP_USER_ID_I, &nvi);
        // 账号密码
        (void)STRCPY((char *)nvi.pap_password.password, (char *)password);
        nvi.pap_password.password_len = STRLEN((char *)password);
        (void)OEMNV_Put(NV_PPP_PASSWORD_I, &nvi);
    }
#endif
} /* OEM_SetBAM_ADSAccount */


#elif defined(FEATURE_FLEXI_STATIC_BREW_APP)
void OEM_SetBAM_ADSAccount(STATIC_BREW_APP_e eApp)
{
#ifndef WIN32
    nv_item_type nvi;
    char username[MAS_BREWSETINT_STRING] = {0};
    char password[MAS_BREWSETINT_STRING] = {0};
    switch(eApp)
    	{
    		case STATIC_BREW_APP_FLEXI_MUSLIM:
    		case STATIC_BREW_APP_FLEXI_NASRANI:
			MEMCPY(username,"fm",2);	
			MEMCPY(password,"fm",2);	
			break;

               case STATIC_BREW_APP_FLEXI_PORTAL:		
			MEMCPY(username,"fb",2);	
			MEMCPY(password,"fb",2);	
			break;
    		default:
			OEMPriv_GetItem_CFGI_BREWSET_USENAME((void*)username);
			OEMPriv_GetItem_CFGI_BREWSET_PASSWORD((void*)password);
              	break;			
    	}

    // 账号

    //(void)STRCPY((char *)nvi.pap_user_id.user_id, (char *)DEFAULT_BREW_USERNAME);
    //nvi.pap_user_id.user_id_len = STRLEN((char *)DEFAULT_BREW_USERNAME);
    (void)STRCPY((char *)nvi.pap_user_id.user_id, (char *)username);
    nvi.pap_user_id.user_id_len = STRLEN((char *)username);
    (void)OEMNV_Put(NV_PPP_USER_ID_I, &nvi);

    // 账号密码

    //(void)STRCPY((char *)nvi.pap_password.password, (char *)DEFAULT_BREW_PASSWORD);
    //nvi.pap_password.password_len = STRLEN((char *)DEFAULT_BREW_PASSWORD);
    (void)STRCPY((char *)nvi.pap_password.password, (char *)password);
    nvi.pap_password.password_len = STRLEN((char *)password);
    (void)OEMNV_Put(NV_PPP_PASSWORD_I, &nvi);
#endif
} /* OEM_SetBAM_ADSAccount */
#elif defined (FEATURE_VERSION_FLEXI203P)
void OEM_SetBAM_ADSAccount(void)
{
#ifndef WIN32
    nv_item_type nvi;
    char username[MAS_BREWSETINT_STRING] = {0};
    char password[MAS_BREWSETINT_STRING] = {0};
	MEMCPY(username,"fb",2);	
	MEMCPY(password,"fb",2);	
   
    // 账号

    //(void)STRCPY((char *)nvi.pap_user_id.user_id, (char *)DEFAULT_BREW_USERNAME);
    //nvi.pap_user_id.user_id_len = STRLEN((char *)DEFAULT_BREW_USERNAME);
    (void)STRCPY((char *)nvi.pap_user_id.user_id, (char *)username);
    nvi.pap_user_id.user_id_len = STRLEN((char *)username);
    (void)OEMNV_Put(NV_PPP_USER_ID_I, &nvi);

    // 账号密码

    //(void)STRCPY((char *)nvi.pap_password.password, (char *)DEFAULT_BREW_PASSWORD);
    //nvi.pap_password.password_len = STRLEN((char *)DEFAULT_BREW_PASSWORD);
    (void)STRCPY((char *)nvi.pap_password.password, (char *)password);
    nvi.pap_password.password_len = STRLEN((char *)password);
    (void)OEMNV_Put(NV_PPP_PASSWORD_I, &nvi);
#endif
} /* OEM_SetBAM_ADSAccount */

#elif defined(FEATURE_SMARTFREN_STATIC_BREW_APP)
void OEM_SetBAM_ADSAccount(STATIC_BREW_APP_e eApp)
{
#ifndef WIN32
    nv_item_type nvi;
    char username[MAS_BREWSETINT_STRING] = {0};
    char password[MAS_BREWSETINT_STRING] = {0};
    switch(eApp)
	{
		case STATIC_BREW_APP_SMARTFREN_FACEBOOK:
		case STATIC_BREW_APP_SMARTFREN_MNEWS:
		case STATIC_BREW_APP_SMARTFREN_MKARAOKE:
		case STATIC_BREW_APP_SMARTFREN_SATU:
		case STATIC_BREW_APP_SMARTFREN_TWITTER:
		case STATIC_BREW_APP_SMARTFREN_SFM:
		case STATIC_BREW_APP_SMARTFREN_MSHOP:
		case STATIC_BREW_APP_SMARTFREN_MDIRECTORY:
		case STATIC_BREW_APP_SMARTFREN_FGEN:
		case STATIC_BREW_APP_SMARTFREN_MOKA:
#if defined(FEATURE_ADS_M8_ACCOUNT)
			MEMCPY(username,"m8",2);	
			MEMCPY(password,"m8",2);	
#elif defined(FEATURE_ADS_SMART_ACCOUNT)
			MEMCPY(username,"smart",5);	
			MEMCPY(password,"smart",5);
#else
			MEMCPY(username,"m8",2);	
			MEMCPY(password,"m8",2);
#endif
			break;
		default:
			OEMPriv_GetItem_CFGI_BREWSET_USENAME((void*)username);
			OEMPriv_GetItem_CFGI_BREWSET_PASSWORD((void*)password);
          	break;			
	}

    // 账号
    (void)STRCPY((char *)nvi.pap_user_id.user_id, (char *)username);
    nvi.pap_user_id.user_id_len = STRLEN((char *)username);
    (void)OEMNV_Put(NV_PPP_USER_ID_I, &nvi);

    // 账号密码
    (void)STRCPY((char *)nvi.pap_password.password, (char *)password);
    nvi.pap_password.password_len = STRLEN((char *)password);
    (void)OEMNV_Put(NV_PPP_PASSWORD_I, &nvi);
#endif
} /* OEM_SetBAM_ADSAccount */
#else 

void OEM_SetBAM_ADSAccount(void)
{
#ifndef WIN32
    nv_item_type nvi;
    char username[MAS_BREWSETINT_STRING] = {0};
    char password[MAS_BREWSETINT_STRING] = {0};

    OEMPriv_GetItem_CFGI_BREWSET_USENAME((void*)username);
    OEMPriv_GetItem_CFGI_BREWSET_PASSWORD((void*)password);

    DBGPRINTF("OEM_SetBAM_ADSAccount username=%s ",username);
    DBGPRINTF("OEM_SetBAM_ADSAccount passwordt=%s",password);    
    // 账号
    (void)STRCPY((char *)nvi.pap_user_id.user_id, (char *)username);
    nvi.pap_user_id.user_id_len = STRLEN((char *)username);
    (void)OEMNV_Put(NV_PPP_USER_ID_I, &nvi);


    // 账号密码
    (void)STRCPY((char *)nvi.pap_password.password, (char *)password);
    nvi.pap_password.password_len = STRLEN((char *)password);
    (void)OEMNV_Put(NV_PPP_PASSWORD_I, &nvi);
#endif	
} /* OEM_SetBAM_ADSAccount */

#endif

void OEM_SetBROWSER_ADSAccount(void)
{
#ifndef WIN32
    nv_item_type nvi;
    char username[MAS_BREWSETINT_STRING] = {0};
    char password[MAS_BREWSETINT_STRING] = {0};

    OEMPriv_GetItem_CFGI_BREWSET_USENAME((void*)username);
    OEMPriv_GetItem_CFGI_BREWSET_PASSWORD((void*)password);
    DBGPRINTF("OEM_SetBROWSER_ADSAccount username=%s ",username);
    DBGPRINTF("OEM_SetBROWSER_ADSAccoun passwordt=%s",password);

    // 账号
    (void)STRCPY((char *)nvi.pap_user_id.user_id, (char *)username);
    nvi.pap_user_id.user_id_len = STRLEN((char *)username);
    (void)OEMNV_Put(NV_PPP_USER_ID_I, &nvi);


    // 账号密码
    (void)STRCPY((char *)nvi.pap_password.password, (char *)password);
    nvi.pap_password.password_len = STRLEN((char *)password);
    (void)OEMNV_Put(NV_PPP_PASSWORD_I, &nvi);
#endif	
} /* OEM_SetBROWSER_ADSAccount */

#ifdef FEATURE_USES_MMS
void OEM_SetMMS_ADSAccount(void)
{
#ifndef WIN32
    nv_item_type nvi;
    char username[MAS_BREWSETINT_STRING] = {0};
    char password[MAS_BREWSETINT_STRING] = {0};

    OEMPriv_GetItem_CFGI_MMS_USER_NAME((void*)username);
    OEMPriv_GetItem_CFGI_MMS_PASSWORD((void*)password);
    DBGPRINTF("OEM_SetMMS_ADSAccount username=%s ",username);
    DBGPRINTF("OEM_SetMMS_ADSAccount passwordt=%s",password);

    // 账号
    (void)STRCPY((char *)nvi.pap_user_id.user_id, (char *)username);
    nvi.pap_user_id.user_id_len = STRLEN((char *)username);
    (void)OEMNV_Put(NV_PPP_USER_ID_I, &nvi);


    // 账号密码
    (void)STRCPY((char *)nvi.pap_password.password, (char *)password);
    nvi.pap_password.password_len = STRLEN((char *)password);
    (void)OEMNV_Put(NV_PPP_PASSWORD_I, &nvi);
#endif	

} /* OEM_SetMMS_ADSAccount */
#endif
#endif // CUST_EDITION

