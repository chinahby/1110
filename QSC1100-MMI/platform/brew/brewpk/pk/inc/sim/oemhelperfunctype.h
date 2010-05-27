#if !defined(OEMHelperFuncType_H)
#define OEMHelperFuncType_H

#include "AEE.h"
#include "AEEComdef.h"
#include "AEEStddef.h"
#include "g_sensor.h"


#ifdef WIN32

#include "sys.h"
/**typedef******************************/
typedef void * uim_cmd_type;
typedef void * uim_rpt_type;
typedef void * nv_mob_dir_data_type;
#define UI_MSG_SND

void uisnd_snd(int,int);
#define NV_SMS_MO_ON_TRAFFIC_CHANNEL_I   4229
#define  NV_SMS_MO_ON_ACCESS_CHANNEL_I   4228
#define NV_SMS_MAX_PAYLOAD_LENGTH_I      4227
#define NV_DS_SIP_ACTIVE_PROFILE_INDEX_I 5452
#define NV_CURR_NAM_I                    40
#define NV_DIR_NUMBER_PCS_I              215
  typedef enum {
    NV_DONE_S,          /* Request completed okay */
    NV_BUSY_S,          /* Request is queued */
    NV_BADCMD_S,        /* Unrecognizable command field */
    NV_FULL_S,          /* The NVM is full */
    NV_FAIL_S,          /* Command failed, reason other than NVM was full */
    NV_NOTACTIVE_S,     /* Variable was not active */
    NV_BADPARM_S,       /* Bad parameter in command block */
    NV_READONLY_S,      /* Parameter is write-protected and thus read only */
    NV_BADTG_S,         /* Item not valid for Target */
    NV_NOMEM_S,         /* free memory exhausted */
    NV_NOTALLOC_S,      /* address is not a valid allocation */
    NV_STAT_ENUM_PAD = 0x7FFF     /* Pad to 16 bits on ARM */
  } nv_stat_enum_type;

#ifdef CUST_EDITION
#define DISP_WIDTH        128
#define DISP_HEIGHT       128
#else
#define DISP_WIDTH        176
#define DISP_HEIGHT       220
#endif
typedef enum
{
  DISP_8BPP,
  DISP_16BPP,
  DISP_18BPP
} disp_bpp_type;

typedef struct
{
   word                    disp_width;
   word                    disp_height;
   disp_bpp_type           bpp;
   boolean                 palette_support;
   boolean                 contrast_support;
   byte                    contrast_min;
   byte                    contrast_max;
   byte                    contrast_default;
   boolean                 backlight_support;
   byte                    backlight_min;
   byte                    backlight_max;
   byte                    backlight_default;
}  disp_info_type;
#define MSG_LOW( sf, a, b, c )

typedef uint32 MP4BufType;
typedef enum {
  UIM_NONE = 0x00,    /* Disabled */
  UIM_AUTH = 0x01,    /* uim power-down bit for Auth task */
  UIM_MC   = 0x02,    /* uim power-down bit for MC task */
  UIM_PROACTIVE_UIM = 0x4, /* power up bit if the UIM is pro active */
  UIM_UI   = 0x8,     /* uim power down bit for UI task */
  UIM_USIM_SESSION = 0x10, /* bit indicating a USIM session */
  UIM_SESSION = 0x20, /* any module that wants to execute a series of cmds
                         without powering down between commands */
  UIM_TC   = 0x40,    /* uim powerdown bit for traffic channel */
  UIM_CDMA_TC = 0x80, /* uim voting for CDMA traffic channel */
  UIM_GW_TC = 0x100   /* uim voting for GSM/WCDMA traffic channel */
} uim_voter_type;

typedef enum 
{
    DB_CAPTURE_NONE = 0x00,
    DB_CAPTURE_NEED,
    DB_CAPTURE_INIDLE,
    DB_CAPTURE_MAX
} db_capture_type;
/*
typedef enum
{
  SYS_OPRT_MODE_NONE = -1,
  SYS_OPRT_MODE_PWROFF = 0,
  SYS_OPRT_MODE_FTM = 1,
  SYS_OPRT_MODE_OFFLINE = 2,
  SYS_OPRT_MODE_OFFLINE_AMPS = 3,
  SYS_OPRT_MODE_OFFLINE_CDMA = 4,
  SYS_OPRT_MODE_ONLINE = 5,
  SYS_OPRT_MODE_LPM = 6,
  SYS_OPRT_MODE_RESET = 7,
  SYS_OPRT_MODE_NET_TEST_GW = 8,
  SYS_OPRT_MODE_OFFLINE_IF_NOT_FTM = 9,
  SYS_OPRT_MODE_MAX
  #ifdef FEATURE_RPC
#error code not present
  #endif 
} sys_oprt_mode_e_type;
*/
typedef enum {
  DB_IN_USE,               /* Call in progress indication                  */
  DB_NO_SVC,               /* Cannot find cellular service indication      */
  DB_DIGITAL,              /* Indicates Digital service                    */
  DB_CONV,                 /* Indicates Conversation is taking place       */
  DB_ROAM,                 /* Roaming status indication                    */
  DB_CHANNEL,              /* Current channel mobile is tuned to           */
  DB_CODE_CHANNEL,         /* Current code-channel mobile is tuned to      */
  DB_PILOT_PN_OFF,         /* Current pilot PN offset                      */
  DB_SID,                  /* Current System ID                            */
  DB_LOCAID,               /* Current Location ID                          */
  DB_NID,                  /* Current Network ID                           */
  DB_LP_SEC,               /* Leap Seconds since start of System Time      */
  DB_LTM_OFF,              /* Offset local time from System Time           */
  DB_DAYLT,                /* Daylight savings time indicator              */
  DB_DMSS_STATE,           /* DMSS System State                            */
  DB_POWER,                /* Current mobile output power level            */
  DB_PRIVACY_MODE,         /* Current privacy mode                         */
  DB_RSSI,                 /* Current FM RSSI level                        */
  DB_CDMA_RSSI,            /* Current CDMA RSSI level                      */
  DB_CDMA_ECIO,
  DB_BATTERY_LEVEL,        /* Current Battery level                        */
  DB_TEMPERATURE,          /* Current Temperature                          */
  DB_RF_MODE,              /* Current RF mode (analog, CDMA, PCN)          */
  DB_HOLD_ORIG,            /* Hold incoming origination cmds               */
  DB_POWERUP_SCANNING,     /* Hold origination on powerup                  */
  DB_ON_HOOK,              /* Is the subscriber unit on hook?              */
  DB_VOX,                  /* Current VOX mode                             */
  DB_DATA_SRVC_STATE,      /* Current state of data services task          */
  DB_SLEEP_MODE,           /* Is sleep enabled?                            */
  DB_SLEEP_ACTIVE,         /* Is sleep occurring?                          */
  DB_ACQ_MODE,             /* Type of CDMA acquisition desired             */
  DB_13K_VOC_AVAILABLE,    /* Vocoder 13k availability flag                */
  DB_VOICE_PRIVACY,        /* Voice Privacy                                */
  DB_BS_P_REV,             /* Base Station's protocol revision level       */
  DB_VOICE_AS_DATA,        /* Answer next call as data                     */
  DB_LCD,
#ifndef WIN32
#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
  DB_UIMINIT_MASK,         /* 卡上数据初始化掩码，由UI设置, UIM 负责清除   */
  DB_UIMSMSINIT,           /* 卡上短信是否初始化完毕                       */
  DB_UIMADDINIT,           /* 卡上电话本是否初始化完毕                     */
#endif
#endif
  DB_PWKCLEARED,           /* 开机按键是否清除                             */
  DB_POWERONCHK,           /* 开机检查是否通过                             */
  DB_POWERUPTYPE,          /* 开机方式                                     */
  DB_REFRESHING,    /* UTK refresh?                                    */
#ifdef FEATURE_SID_LOCK
  DB_SID_LOCK,
#endif
  DB_CAPTURE_WALLPER,
#ifdef FEATURE_SUPPORT_VC0848
  DB_CAMERA_MODE, // save the current mode for camera
  DB_BACKLIGHT_ON,
  DB_SD_ON,  // if sd_on = off, means efs2 is selected, sd_on=on, sd is selected.
  DB_ENVIRONMENT_MODE,  // save the camera environment mode
#endif
  DB_LAST_ITEM             /* Internal Use Only                            */
} db_items_type;
typedef enum 
{
	  LCM_BYD4227P,
	  LCM_CT020TN06,			  //176 x 220, TFT
	  other_lcd
} db_lcd_type;

typedef union {
  boolean               in_use;
  boolean               no_svc;
  boolean               digital;
  boolean               conv;
  //db_roam_type          roam;
  word                  channel;
  byte                  code_channel;
  word                  pilot_pn_off;
  word                  sid;
  word                  locaid;
  word                  nid;
  byte                  lp_sec;
  byte                  ltm_off;
  boolean               daylt;
  //db_dmss_state_type    dmss_state;
  byte                  power;
  boolean               privacy_mode;
  word                  rssi;
  word                  cdma_rssi;
  byte                  cdma_ecio;
  word                  battery_level;
  word                  temperature;
  //db_rf_mode_type       rf_mode;
  boolean               hold_orig;
  boolean               powerup_scanning;
  boolean               on_hook;
  boolean               vox;
  boolean               data_srvc_enabled;
 // db_sleep_mode_type    sleep_mode;
  //db_sleep_active_type  sleep_active;
 // db_acq_mode_type      acq_mode;
  boolean               voc13k_available;
  boolean               voice_privacy;
  byte                  bs_p_rev;
  
#ifdef FEATURE_GPSONE
#error code not present
#endif /* FEATURE_GPSONE */
#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
  byte                  db_uiminitmask;
  boolean               db_uimsmsinited;
  boolean               db_uimaddinited;
  boolean               db_uimsmsadd_init_done;
#endif
  boolean               db_pwkcleared;
  boolean               db_poweronchk;
  //db_powerup_type       db_poweruptype;
  boolean               bRefreshing; 
#ifdef FEATURE_SID_LOCK
  byte                    b_sid_lock;
#endif
  db_capture_type  b_capture;
#ifdef FEATURE_SUPPORT_VC0848
  byte             nCamMode;
  boolean          bBacklightOn;
  boolean          bSDOn; // save efs type: native efs2 or sd card
  byte             nEnvironMode;
#endif
  db_lcd_type db_lcd;
} db_items_value_type;

#define SNDDEV_DUMMY_DATA_UINT32_MAX 0x7FFFFFFF

/*  All of the audio devices supported on this platform
*/
typedef enum {
  SND_DEVICE_HANDSET,
  SND_DEVICE_HFK,
  SND_DEVICE_HEADSET,         /* Mono headset               */
#if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
  SND_DEVICE_STEREO_HEADSET,  /* Stereo headset             */
#endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */
#ifdef FEATURE_ANALOG_HFK
  SND_DEVICE_AHFK,
#endif  
#if defined(FEATURE_INTERNAL_SDAC) || defined(FEATURE_EXTERNAL_SDAC)
  SND_DEVICE_SDAC,
#endif /* FEATURE_INTERNAL_SDAC || FEATURE_EXTERNAL_SDAC */
#ifdef FEATURE_SPEAKER_PHONE
  SND_DEVICE_SPEAKER_PHONE,
#endif
#ifdef FEATURE_TTY
  SND_DEVICE_TTY_HFK,
  SND_DEVICE_TTY_HEADSET,
  SND_DEVICE_TTY_VCO,
  SND_DEVICE_TTY_HCO,
#endif
#ifdef FEATURE_SUPPORT_BT_BCM
  SND_DEVICE_BT_HEADSET,
#endif /* FEATURE_SUPPORT_BT_BCM */
#ifdef FEATURE_USB_CARKIT
#error code not present
#endif /* FEATURE_USB_CARKIT */
#if defined(FEATURE_INTERNAL_SADC) || defined(FEATURE_EXTERNAL_SADC)
  SND_DEVICE_IN_M_SADC_OUT_HANDSET, /* Input Mono   SADD, Output Handset */
  SND_DEVICE_IN_S_SADC_OUT_HANDSET, /* Input Stereo SADD, Output Headset */ 
  SND_DEVICE_IN_M_SADC_OUT_HEADSET, /* Input Mono   SADD, Output Handset */
  SND_DEVICE_IN_S_SADC_OUT_HEADSET, /* Input Stereo SADD, Output Headset */
#endif /* FEATURE_INTERNAL_SADC || FEATURE_EXTERNAL_SADC */

  SND_DEVICE_MAX,
  SND_DEVICE_CURRENT,

  /* DO NOT USE: Force this enum to be a 32bit type */
  SND_DEVICE_32BIT_DUMMY = SNDDEV_DUMMY_DATA_UINT32_MAX
} snd_device_type;

/* The sound generation methods supported on this platform
*/
typedef enum {
  /* This method is required
  */
  SND_METHOD_VOICE = 0,   /* Use the device's voice generator                */
  
  /* Application-Specific Methods
  */
  SND_METHOD_KEY_BEEP,    /* Use the device's keybeep generator              */
  SND_METHOD_MESSAGE,     /* Use the path's ringer, or voice generator       */
  SND_METHOD_RING,        /* Use the device's ring generator                 */
#ifdef FEATURE_CLICK
#error code not present
#endif
#if defined(FEATURE_AUDIO_FORMAT) || defined(FEATURE_MIDI_OUT)
  SND_METHOD_MIDI,        /* Use the device's Midi generator                 */
#endif
  SND_METHOD_AUX,         /* Use the device's auxiliary generator if avail.  */
  SND_METHOD_MAX,          /* Always last in the list                         */

  /* DO NOT USE: Force this enum to be a 32bit type */
  SND_METHOD_32BIT_DUMMY = SNDDEV_DUMMY_DATA_UINT32_MAX
} snd_method_type;

enum sdb_sounds {          /* Description of sound                 */

  SND_FIRST_SND = -1,   /* Use for range checking last sound    */

  SND_ALERT = 0,                /* 0 -  Ringing for incoming call            */
  SND_WAKEUP,                   /* Wake-up/Power-up sound                    */
  SND_DIAL_TONE,                /* Dial tone                                 */
  SND_DTACO_ROAM_TONE,          /* DTACO roaming dial tone                   */
  SND_RING_BACK,                /* Ring-back sound                           */
  SND_INTERCEPT,                /* Send request intercepted locally          */
  SND_REORDER,                  /* System busy                               */
  SND_BUSY_ALERT,               /* Busy Signal                               */
  SND_CONFIRMATION,             /* Confirmation Tone                         */
  SND_CALL_WAITING,             /* Call Waiting                              */
  SND_ANSWER,                   /* 10 - Answer Tone                          */
  SND_OFF_HOOK,                 /* Off-Hook Warning                          */
  SND_NORMAL_ALERT,             /* "Normal" Alerting                         */
  SND_INTR_GROUP_ALERT,         /* Intergroup Alerting                       */
  SND_SPCL_ALERT,               /* Special/Priority Alerting                 */
  SND_PING_RING,                /* "Ping ring"                               */
  SND_IS54B_LONG_H_ALERT,       /* IS-54B High Long                          */
  SND_IS54B_SS_H_ALERT,         /* IS-54B High Short-short                   */
  SND_IS54B_SSL_H_ALERT,        /* IS-54B High Short-short-long              */
  SND_IS54B_SS2_H_ALERT,        /* IS-54B High Short-short-2                 */
  SND_IS54B_SLS_H_ALERT,        /* 20 - IS-54B High Short-long-short         */
  SND_IS54B_SSSS_H_ALERT,       /* IS-54B High Short-short-short-short       */
  SND_IS54B_PBX_LONG_H_ALERT,   /* IS-54B High PBX Long                      */
  SND_IS54B_PBX_SS_H_ALERT,     /* IS-54B High PBX Short-short               */
  SND_IS54B_PBX_SSL_H_ALERT,    /* IS-54B High PBX Short-short-long          */
  SND_IS54B_PBX_SLS_H_ALERT,    /* IS-54B High PBX Short-long-short          */
  SND_IS54B_PBX_SSSS_H_ALERT,   /* IS-54B High PBX Short-short-short-short   */
  SND_IS53A_PPPP_H_ALERT,       /* IS-53A High Pip-Pip-Pip-Pip Alert         */
  SND_IS54B_LONG_M_ALERT,       /* IS-54B Medium Long                        */
  SND_IS54B_SS_M_ALERT,         /* IS-54B Medium Short-short                 */
  SND_IS54B_SSL_M_ALERT,        /* 30 - IS-54B Medium Short-short-long       */
  SND_IS54B_SS2_M_ALERT,        /* IS-54B Medium Short-short-2               */
  SND_IS54B_SLS_M_ALERT,        /* IS-54B Medium Short-long-short            */
  SND_IS54B_SSSS_M_ALERT,       /* IS-54B Medium Short-short-short-short     */
  SND_IS54B_PBX_LONG_M_ALERT,   /* IS-54B Medium PBX Long                    */
  SND_IS54B_PBX_SS_M_ALERT,     /* IS-54B Medium PBX Short-short             */
  SND_IS54B_PBX_SSL_M_ALERT,    /* IS-54B Medium PBX Short-short-long        */
  SND_IS54B_PBX_SLS_M_ALERT,    /* IS-54B Medium PBX Short-long-short        */
  SND_IS54B_PBX_SSSS_M_ALERT,   /* IS-54B Medium PBX Short-short-short-short */
  SND_IS53A_PPPP_M_ALERT,       /* IS-53A Medium Pip-Pip-Pip-Pip Alert       */
  SND_IS54B_LONG_L_ALERT,       /* 40 - IS-54B Low Long                      */
  SND_IS54B_SS_L_ALERT,         /* IS-54B Low Short-short                    */
  SND_IS54B_SSL_L_ALERT,        /* IS-54B Low Short-short-long               */
  SND_IS54B_SS2_L_ALERT,        /* IS-54B Low Short-short-2                  */
  SND_IS54B_SLS_L_ALERT,        /* IS-54B Low Short-long-short               */
  SND_IS54B_SSSS_L_ALERT,       /* IS-54B Low Short-short-short-short        */
  SND_IS54B_PBX_LONG_L_ALERT,   /* IS-54B Low PBX Long                       */
  SND_IS54B_PBX_SS_L_ALERT,     /* IS-54B Low PBX Short-short                */
  SND_IS54B_PBX_SSL_L_ALERT,    /* IS-54B Low PBX Short-short-long           */
  SND_IS54B_PBX_SLS_L_ALERT,    /* IS-54B Low PBX Short-long-short           */
  SND_IS54B_PBX_SSSS_L_ALERT,   /* 50 - IS-54B Low PBX Short-short-short-shrt*/
  SND_IS53A_PPPP_L_ALERT,       /* IS-53A Low Pip-Pip-Pip-Pip Alert          */
  SND_FADE_TONE,                /* Tone to inform user of a fade             */
  SND_SVC_CHANGE,               /* Inform user of a service area change      */
  SND_HORN_ALERT,               /* Horn alert                                */
  SND_ABRV_REORDER,             /* Abbreviated System busy                   */
  SND_ABRV_INTERCEPT,           /* Abbrev'd Send request intercepted locally */
  SND_ALTERNATE_REORDER,        /* Alternate reorder                         */
  SND_MESSAGE_ALERT,            /* Message Waiting Signal                    */
  SND_ABRV_ALERT,               /* Abbreviated alert                         */
  SND_PIP_TONE,                 /* 60 - Pip Tone (Voice Mail Waiting)        */
  SND_ROAM_RING,                /* Ringing option while roaming              */
  SND_SVC_ACQ,                  /* Service acquired sound                    */
  SND_SVC_LOST,                 /* Service lost sound                        */
  SND_SVC_CHNG_MORE_PREF,       /* Change to a more preferred service sound  */
  SND_SVC_CHNG_LESS_PREF,       /* Change to a less preferred service sound  */
  SND_EXT_PWR_ON,               /* External power on sound                   */
  SND_EXT_PWR_OFF,              /* External power off sound                  */
  SND_RING_1,                   /* User selectable ring 1                    */
  SND_RING_2,                   /* User selectable ring 2                    */
  SND_RING_3,                   /* 70 - User selectable ring 3               */
  SND_RING_4,                   /* User selectable ring 4                    */
  SND_RING_5,                   /* User selectable ring 5                    */
  SND_RING_6,                   /* User selectable ring 6                    */
  SND_RING_7,                   /* User selectable ring 7                    */
  SND_RING_8,                   /* User selectable ring 8                    */
  SND_RING_9,                   /* User selectable ring 9                    */
  SND_VR_HFK_CALL_RECEIVED,     /* Call answer sound when in HFK             */
  SND_HFK_CALL_ORIG,            /* Call origination sound when in HFK        */
  SND_SPECIAL_INFO,             /* Special info sound                        */
                                /* GSM tones, defined in 3GPP 2.40           */
  SND_SUBSCRIBER_BUSY,          /* 80 - Subscriber busy sound                */
  SND_CONGESTION,               /* Congestion sound                          */
  SND_ERROR_INFO,               /* Error information sound                   */
  SND_NUMBER_UNOBTAINABLE,      /* Number unobtainable sound                 */
  SND_AUTH_FAILURE,             /* Authentication failure sound              */
  SND_RADIO_PATH_ACK,           /* Radio path acknowledgement sound          */
  SND_RADIO_PATH_NOT_AVAIL,     /* Radio path not available sound            */
  SND_CEPT_CALL_WAITING,        /* CEPT call waiting sound                   */
  SND_CEPT_RING,                /* CEPT ringing sound                        */
  SND_CEPT_DIAL_TONE,           /* CEPT dial tone                            */
  SND_LAST_SND                  /* Use for range checking last sound         */

};
#define SND_DUMMY_DATA_UINT32_MAX 0x7FFFFFFF

typedef enum {
  SND_PRIO_LOW,                 /* All sounds except DTMF burst    */
  SND_PRIO_MED,                 /* All sounds except DTMF burst    */
  SND_PRIO_HIGH,                /* Used in DTMF burst transmission */
  SND_PRIO_ALL,                 /* Used to stop all sounds         */

  /* DO NOT USE: Force this enum to be a 32bit type */
  SND_PRIO_32BIT_DUMMY = SND_DUMMY_DATA_UINT32_MAX
} snd_priority_type;
typedef enum {
  SND_APATH_LOCAL,         /* DTMF's on local audio        */
  SND_APATH_TX,            /* Transmit DTMFs               */
  SND_APATH_BOTH,          /* Tx and sound DTMF's locally  */
  SND_APATH_MUTE,          /* Mute the DTMFs               */

  /* DO NOT USE: Force this enum to be a 32bit type */
  SND_APATH_32BIT_DUMMY = SND_DUMMY_DATA_UINT32_MAX
} snd_apath_type;

/**Var********************************/
#ifndef CUST_EDITION
extern boolean g_b_no_batt;
#endif

mmi_g_sensor_state_type mmi_g_sensor_state;


/**define****************************/
#define APP_MEDIA_ALLOW         0    
#define APP_MEDIA_IMPACT_BY_FM  1
#define APP_MEDIA_IMPACT_BY_MP3 2

#define ERR(p,p1,p2,p3)
#define CM_CLIENT_ID_ANONYMOUS  (int) (-1)
#define UIM_MAX_CHV_DIGITS 8
//#define ASSERT(p)
#define  NV_MAX_LTRS   12
#define MSG_ERROR(p,p1,p2,p3)
#define ARR_SIZE(a)  (sizeof((a))/sizeof((a[0])))
#define ASSERT(p,p1,p2,p3)

typedef  word  nv_language_enum_type;
  /* Use English language */
  #define  NV_LANGUAGE_ENGLISH                           ((nv_language_enum_type)0)
  /* Use Spanish language */
  #define  NV_LANGUAGE_SPANISH                           ((nv_language_enum_type)1)
  /* Use French language */
  #define  NV_LANGUAGE_FRENCH                            ((nv_language_enum_type)2)
  /* Use Portuguese language */
  #define  NV_LANGUAGE_PORTUGUESE                        ((nv_language_enum_type)3)
   /* Use TChinese language    */
  #define NV_LANGUAGE_TCHINESE                           ((nv_language_enum_type)4)  
   /* Use Chinese language    */
  #define NV_LANGUAGE_CHINESE                            ((nv_language_enum_type)5)
   /* Use Japanese  language    */
  #define NV_LANGUAGE_JAPANESE                           ((nv_language_enum_type)6)
   /* Use Korean language    */
  #define NV_LANGUAGE_KOREAN                             ((nv_language_enum_type)7)  
   /* Use Hebrew language    */
  #define NV_LANGUAGE_HEBREW                             ((nv_language_enum_type)8)
   /* Use Thai language    */
  #define NV_LANGUAGE_THAI                               ((nv_language_enum_type)9)
   /* Use Italian language    */
  #define NV_LANGUAGE_ITALIAN                            ((nv_language_enum_type)10)  
   /* Use Indonesian language    */
  #define NV_LANGUAGE_INDONESIAN                         ((nv_language_enum_type)11)
   /* Use Arabic language    */
  #define NV_LANGUAGE_ARABIC                             ((nv_language_enum_type)12)
   /* Use Hindi language    */
  #define NV_LANGUAGE_HINDI                              ((nv_language_enum_type)13)      
   /* Use Vietnamese language    */
  #define NV_LANGUAGE_VIETNAMESE                         ((nv_language_enum_type)14)
#define  NV_MAX_LTRS                                            12
#define  NV_LOCK_CODE_SIZE                                       4
#define  NV_BREW_SERVER_SIZ                                     64
#define  NV_BREW_BKEY_SIZ                                       16
#define  NV_BREW_SID_SIZ                                        32
#define NV_CONT_KEY_DTMF_I                                      67
#define NV_SMS_VM_NUMBER_I                             558
typedef PACKED struct { 
  /* TRUE if auto redial enabled */
  boolean                                          enable;
  /* Number of 10 seconds system busy before redial */
  byte                                             rings;
} nv_auto_redial_type;

typedef  word  nv_rtre_configuration_type;
  /* RTRE configuration: use R-UIM only */
  #define  NV_RTRE_CONFIG_RUIM_ONLY                      ((nv_rtre_configuration_type)0)
  /* RTRE configuration: use NV only */
  #define  NV_RTRE_CONFIG_NV_ONLY                        ((nv_rtre_configuration_type)1)
  /* RTRE configuration: use R-UIM if available or NV */
  #define  NV_RTRE_CONFIG_RUIM_OR_DROP_BACK              ((nv_rtre_configuration_type)2)
  /* RTRE configuration: GSM on 1x */
  #define  NV_RTRE_CONFIG_SIM_ACCESS                     ((nv_rtre_configuration_type)3)



#define  NV_MAX_NAMS                                             1
#define NV_AIR_TIMER_I                                 53


typedef int NOTICE_TYPE;

#define ERR(p,p1,p2,p3)

//#define FEATURE_WMS_APP
/**Function******************************/

void 		oemui_cm_init(void);
dword 		uim_power_control(int mask,boolean uim_reqd);
boolean 	IsRunAsUIMVersion(void);
boolean 	MakeVoiceCall(IShell *pShell, boolean bCloseAll, AECHAR *number);
void 		OEM_RestoreFactorySetting( void );
void 		db_put(db_items_type  item,db_items_value_type  *value_ptr );
void 		db_get(db_items_type   item, db_items_value_type  *value_ptr );
boolean 	OEMRTC_Process_Auto_Power_On(void);
boolean 	cm_ph_cmd_oprt_mode(void *    cmd_cb_func,void *data_block_ptr,int      client_id,int    oprt_mode);
void mmi_g_sensor_process(g_sensor_cmd_type);
int ReadPedoCounter(void);
void MMD_LCDRotate(int i);
void disp_set_contrast(word contrast);

#endif
#endif


