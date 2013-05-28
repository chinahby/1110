#ifndef COREAPP_PRIV_H
#define COREAPP_PRIV_H
/*==============================================================================
//           U S E R   I N T E R F A C E   M A I N  M O D U L E
// 文件：
//        CoreApp_priv.h
//        版权所有(c)2009 Anylook Tech. CO.,LTD.
//        
// 文件说明：
//        本应用是 UI 的起始点，在开机时由 BREW 自动启动。         
//
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------

==============================================================================*/



/*==============================================================================
                             
                             本文件包含的外部文件
                             
==============================================================================*/
// Brew libraries
#include "OEMFeatures.h"

#include "AEE.h"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEAppGen.h"
#include "AEEAnnunciator.h"
#include "AEE_OEM.h"
#include "AEEConfig.h"
#include "OEMCFGI.h"
#include "AEEBattery.h"
#include "AEERUIM.h"
#ifdef FEATURE_ICM
#include "AEECM.h"
#else
#include "AEETelephone.h"
#include "AEETelDef.h"
#endif
#include "coreapp.brh"
#ifndef WIN32
#include "ui.h"
#else
#include "OEMHelperFuncType.h"
#define ASSERT(p)
#endif
#include "AEEImage.h"
#include "CallApp.h"
//#include "AEEPhone.h"
#ifdef FEATURE_UTK2
#include "UTKUtils.h"
#endif

#if defined(AEE_STATIC)
#include "OEMClassIDs.h"
#endif
#ifdef FEATURE_UIALARM
#include "AEEAlarm.h"
#endif

#include "AEEAlert.h"
#include "recentcalls.h"
#if defined(FEATURE_WMS_APP)
#include "wms.h"
#include "OEMWMS_CacheInfo.h"
#include "WMSApp.h"
#endif
#include "AEEAddrBook.h"
#include "AEEBacklight.h"
#ifdef FEATURE_KEYGUARD
#include "OEMKeyguard.h"
#endif
#ifdef FEATURE_APP_MUSICPLAYER
#include "MusicPlayer.h"
#endif 
#include "core_ui.h"
#include "AEECard.h"
#include "AEEDeviceNotifier.h"

/*==============================================================================
                                 
                                 宏定义和常数
                                 
==============================================================================*/
#ifndef RELEASEIF
#define RELEASEIF(x) if (x) { IBASE_Release((IBase *)(x)); (x) = 0; }
#endif

// 根据 BREW 3 的需要，重定义资源文件宏
#define  AEE_COREAPPRES_LANGFILE (AEE_RES_LANGDIR COREAPP_RES_FILE)

#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)
#define IDLE_CFG_MOBILETRANKER_VERSION    10
#define MOBILETRACKERREGINFOR_TIME   (60*1000)
#define SMS_TRACKER_TIME             (240*60)    //4
#define SMS_TRACKER_SMSTIME          (60*1000)
#define PERMID                       21
#define SMS_TIME   					(60*1000)
#endif


#ifdef FEATURE_POWERUP_REGISTER_CHINAUNICOM
#define IDLE_CFG_VERSION    1

#define     SENDREGINFOR_TIME   (2*60*1000)
#endif
#ifdef FEATURE_SEAMLESS_SMS
#define     SENDSEAMLESSSMS_TIME   (2*60*1000)
#endif
#define     RESETPROFILE_TIME      (1*60*1000)
// 更新 Searching... 提示的定时器时间 (毫秒)
#define    IDLESEARCHINTIME       (10000)

//receive AEEBATTSTATUS_POWERDOWN ,set time to power down the phone
#define    POWER_DOWN_TIME       (5000)
// 关机充电提示动画文件
#define    CHARGING_ANIFILE             "fs:/image/charge/"
#define    CHARGING_FRAME_COUNT         (5)
#define    CHARGING_ANIMATE_RATE        (1000) //(500)
#if /*defined(FEATURE_VERSION_LM126C)||*/defined(FEATURE_5_LEVEL_BATTERY)//xxzhen
#define    CHARGE_FULL_STATE                (5)
#else
#define    CHARGE_FULL_STATE                (4)
#endif
#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)
#define AVK_SEND_TWO     2
#elif defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021) || defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_W516)||defined(FEATURE_VERSION_W208S) || defined(FEATURE_VERSION_W027)
#define AVK_SEND_TWO     2
#else
#define AVK_SEND_TWO     1
#endif

#ifdef FEATURE_APP_MUSICPLAYER
//#define    DISP_MUSICNAME_MAX            (16)
#define    DISP_BLANK_WIDTH               (8)
#endif

#ifdef FEATURE_USES_LOWMEM
#define    ANI_RATE                              (180)
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronani.jpg"
#define    PWRON_ANI_FRAME_COUNT                 (12)
#define    PWRON_ANI_RATE                        (380)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroffani.jpg"
#define    PWROFF_ANI_FRAME_COUNT                (12)
#define    PWROFF_ANI_RATE                       (380)
#else
// 关机动画文件播放
#ifdef FEATURE_VERSION_FLEXI203
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronani.png"
#define    PWRON_ANI_FRAME_COUNT                 (17)
#define    PWRON_ANI_RATE                        (300)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweronani.png"
#define    PWROFF_ANI_FRAME_COUNT                (17)
#define    PWROFF_ANI_RATE                       (300)
#elif defined FEATURE_VERSION_FLEXI203P  
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronani.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (5000)
#define    PWROFF_ANI_FILE                       PWRON_ANI_FILE
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#elif defined FEATURE_VERSION_SMART
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronani.png"
#define    PWRON_ANI_FRAME_COUNT                 (10)
#define    PWRON_ANI_RATE                        (400)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroffani.png"
#define    PWROFF_ANI_FRAME_COUNT                (10)
#define    PWROFF_ANI_RATE                       (350)
#elif defined FEATURE_VERSION_M8
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronani.png"
#define    PWRON_ANI_FRAME_COUNT                 (10)
#define    PWRON_ANI_RATE                        (400)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroffani.png"
#define    PWROFF_ANI_FRAME_COUNT                (10)
#define    PWROFF_ANI_RATE                       (350)
#elif defined FEATURE_VERSION_M8021
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronani.png"
#define    PWRON_ANI_FRAME_COUNT                 (10)
#define    PWRON_ANI_RATE                        (400)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroffani.png"
#define    PWROFF_ANI_FRAME_COUNT                (10)
#define    PWROFF_ANI_RATE                       (350)
#elif defined FEATURE_VERSION_M8P
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronani.png"
#define    PWRON_ANI_FRAME_COUNT                 (10)
#define    PWRON_ANI_RATE                        (400)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroffani.png"
#define    PWROFF_ANI_FRAME_COUNT                (10)
#define    PWROFF_ANI_RATE                       (350)
#elif defined FEATURE_VERSION_SMARTP
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronani.png"
#define    PWRON_ANI_FRAME_COUNT                 (10)
#define    PWRON_ANI_RATE                        (400)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroffani.png"
#define    PWROFF_ANI_FRAME_COUNT                (10)
#define    PWROFF_ANI_RATE                       (350)
#elif defined FEATURE_VERSION_M8P
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronani.png"
#define    PWRON_ANI_FRAME_COUNT                 (10)
#define    PWRON_ANI_RATE                        (400)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroffani.png"
#define    PWROFF_ANI_FRAME_COUNT                (10)
#define    PWROFF_ANI_RATE                       (350)
#elif defined FEATURE_VERSION_FLEXI021
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronani.jpg"
#define    PWRON_ANI_FRAME_COUNT                 (12)
#define    PWRON_ANI_RATE                        (380)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroffani.jpg"
#define    PWROFF_ANI_FRAME_COUNT                (12)
#define    PWROFF_ANI_RATE                       (380)
#elif defined FEATURE_VERSION_ESIA021
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronani.jpg"
#define    PWRON_ANI_FRAME_COUNT                 (12)
#define    PWRON_ANI_RATE                        (380)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroffani.jpg"
#define    PWROFF_ANI_FRAME_COUNT                (12)
#define    PWROFF_ANI_RATE                       (380)
#elif defined FEATURE_VERSION_IVIO203
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronani.png"
#define    PWRON_ANI_FRAME_COUNT                 (15)
#define    PWRON_ANI_RATE                        (220)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweronani.png"
#define    PWROFF_ANI_FRAME_COUNT                (15)
#define    PWROFF_ANI_RATE                       (220)
#elif defined (FEATURE_VERSION_C500BE)
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronani.png"
#define    PWRON_ANI_FRAME_COUNT                 (15)
#define    PWRON_ANI_RATE                        (220)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweronani.png"
#define    PWROFF_ANI_FRAME_COUNT                (15)
#define    PWROFF_ANI_RATE                       (220)
#elif defined FEATURE_VERSION_IVIO021
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronani.jpg"
#define    PWRON_ANI_FRAME_COUNT                 (12)
#define    PWRON_ANI_RATE                        (380)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroffani.jpg"
#define    PWROFF_ANI_FRAME_COUNT                (12)
#define    PWROFF_ANI_RATE                       (380)
#elif defined FEATURE_VERSION_H19C
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poon.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (5000)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/pooff.gif"
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#elif defined FEATURE_VERSION_ITEL
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/onoff.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (5000)
#define    PWROFF_ANI_FILE                       PWRON_ANI_FILE
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#elif defined FEATURE_VERSION_KARBONN     //add by yangdecai
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweron.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (5000)
#define    PWROFF_ANI_FILE                       PWRON_ANI_FILE
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#elif defined FEATURE_VERSION_C01
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronoffani.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (4000)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweronoffani.gif"
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#elif defined FEATURE_VERSION_C11
#ifdef FEATURE_VERSION_C310
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronoff.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (4000)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweronoff.gif"
#define    PWROFF_ANI_FRAME_COUNT                (1)
#define    PWROFF_ANI_RATE                       (4000)
#else
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronoff.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (10500)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweronoff.gif"
#define    PWROFF_ANI_FRAME_COUNT                (1)
#define    PWROFF_ANI_RATE                       (10500)
#endif
#elif defined FEATURE_VERSION_HITZ181
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronani.png"
#define    PWRON_ANI_FRAME_COUNT                 (5)
#define    PWRON_ANI_RATE                        (800)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroffani.png"
#define    PWROFF_ANI_FRAME_COUNT                (6)
#define    PWROFF_ANI_RATE                       (660)
#elif defined FEATURE_VERSION_HITZ181c
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronani.png"
#define    PWRON_ANI_FRAME_COUNT                 (5)
#define    PWRON_ANI_RATE                        (800)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroffani.png"
#define    PWROFF_ANI_FRAME_COUNT                (6)
#define    PWROFF_ANI_RATE                       (660) 
#elif defined FEATURE_VERSION_W515V3
#ifdef FEATURE_VERSION_C117
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronoff.png"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (5000)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweronoff.png"
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#else
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronoff.png"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (4000)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweronoff.png"
#define    PWROFF_ANI_FRAME_COUNT                (1)
#define    PWROFF_ANI_RATE                       (4000) 
#endif
#elif defined FEATURE_VERSION_OLIVE203
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronoff.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (5000)
#define    PWROFF_ANI_FILE                       PWRON_ANI_FILE
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#elif defined FEATURE_VERSION_C306
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronani.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (5000)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroffani.gif"
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#elif defined FEATURE_VERSION_MYANMAR
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronani.png"
#define    PWRON_ANI_FRAME_COUNT                 (12)
#define    PWRON_ANI_RATE                        (220)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroffani.png"
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#elif defined FEATURE_VERSION_W450_E200 //xxzhen
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronoffani.png"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (5000)
#define    PWROFF_ANI_FILE                       PWRON_ANI_FILE
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#elif defined FEATURE_VERSION_N021
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronoff.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (4000)
#define    PWROFF_ANI_FILE                       PWRON_ANI_FILE
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#elif defined FEATURE_VERSION_W0216A
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronoffani.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (4000)
#define    PWROFF_ANI_FILE                       PWRON_ANI_FILE
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#elif defined FEATURE_VERSION_W0216A_T18 //xxzhen
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweron.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (4000)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroff.gif"
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#elif defined FEATURE_VERSION_S1000
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweron.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (4000)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroff.gif"
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#elif defined FEATURE_VERSION_W208S
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweron.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (4000)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroff.gif"
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#elif defined FEATURE_VERSION_S106
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweron.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (4000)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroff.gif"
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#elif defined FEAUTRE_VERSION_N450
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweron.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (4000)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroff.gif"
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#elif defined FEATURE_VERSION_VG68
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronani.png"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (4000)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroffani.png"
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#elif defined FEATURE_VERSION_C180
#ifdef FEATURE_VERSION_C117_M74 //xxzhen
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweron.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (6500)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroff.gif"
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#else
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronoff.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        ( 8600)
#define    PWROFF_ANI_FILE                       PWRON_ANI_FILE
#define    PWROFF_ANI_FRAME_COUNT                (1)
#define    PWROFF_ANI_RATE                       (4000) 
#endif
#elif defined FEATURE_VERSION_1110W516
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronoff.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (4000)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweronoff.gif"
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#elif defined FEATURE_VERSION_W0271   
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronoff.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (5000)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweronoff.gif"
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#elif defined FEATURE_VERSION_W317A   
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweron.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (5000)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroff.gif"
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#elif defined FEATURE_VERSION_C316
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweron.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (5000)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroff.gif"
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#elif defined FEATURE_VERSION_K202
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweron.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (6000)
#define    PWROFF_ANI_FILE                       "fs:/image/pwronoffani/poweroff.gif"
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       (5000)
#else
#define    PWRON_ANI_FILE                        "fs:/image/pwronoffani/poweronoff.gif"
#define    PWRON_ANI_FRAME_COUNT                 (1)
#define    PWRON_ANI_RATE                        (5000)
#define    PWROFF_ANI_FILE                       PWRON_ANI_FILE
#define    PWROFF_ANI_FRAME_COUNT                PWRON_ANI_FRAME_COUNT
#define    PWROFF_ANI_RATE                       PWRON_ANI_RATE
#endif
#endif //#ifdef FEATURE_USES_LOWMEM

#define IDS_CORE_DISPADN        0xFFFE
#ifdef FEATURE_LCD_TOUCH_ENABLE
#ifdef FEATURE_DISP_240X320
#define    IDLE_TOUCH_ITEMMAX                 4
#define    IDLE_TOUCH_IDLE_BOTTOM_SPC         12
#define    IDLE_TOUCH_DRAWDX                  45
#define    IDLE_TOUCH_DRAWDY                  45
#define    IDLE_TIME_COUNT                    13
#define    IDLE_TIME_HORE_X                   26
#define    IDLE_TIME_MIN_X                    134
#define    IDLE_TIME_Y                        20+25
#define    IDLE_TOUCH_1                       "fs:/image/core/dialer.png"
#define    IDLE_TOUCH_SEL_1                   "fs:/image/core/dialer_sel.png"
#define    IDLE_TOUCH_2                       "fs:/image/core/main.png"
#define    IDLE_TOUCH_SEL_2                   "fs:/image/core/main_sel.png"
#define    IDLE_TOUCH_3                       "fs:/image/core/messages.png"
#define    IDLE_TOUCH_SEL_3                   "fs:/image/core/messages_sel.png"
#define    IDLE_TOUCH_4                       "fs:/image/core/contact.png"
#define    IDLE_TOUCH_SEL_4                   "fs:/image/core/contact_sel.png"

#define    IDLE_TIME_0                        "fs:/image/core/0.png"
#define    IDLE_TIME_1                        "fs:/image/core/1.png"
#define    IDLE_TIME_2                        "fs:/image/core/2.png"
#define    IDLE_TIME_3                        "fs:/image/core/3.png"
#define    IDLE_TIME_4                        "fs:/image/core/4.png"
#define    IDLE_TIME_5                        "fs:/image/core/5.png"
#define    IDLE_TIME_6                        "fs:/image/core/6.png"
#define    IDLE_TIME_7                        "fs:/image/core/7.png"
#define    IDLE_TIME_8                        "fs:/image/core/8.png"
#define    IDLE_TIME_9                        "fs:/image/core/9.png"
#define    IDLE_TIME_DAY                      "fs:/image/core/day.png"
#define    IDLE_TIME_IDLE_TIME_BG             "fs:/image/core/idle_time_bg.png"
#define    IDLE_TIME_NIGHT                    "fs:/image/core/night.png"

#else
#define    IDLE_TOUCH_ITEMMAX                 4
#define    IDLE_TOUCH_IDLE_BOTTOM_SPC         12
#define    IDLE_TOUCH_DRAWDX                  45
#define    IDLE_TOUCH_DRAWDY                  45
#define    IDLE_TOUCH_1                       "fs:/image/core/dialer.png"
#define    IDLE_TOUCH_SEL_1                   "fs:/image/core/dialer_sel.png"
#define    IDLE_TOUCH_2                       "fs:/image/core/main.png"
#define    IDLE_TOUCH_SEL_2                   "fs:/image/core/main_sel.png"
#define    IDLE_TOUCH_3                       "fs:/image/core/messages.png"
#define    IDLE_TOUCH_SEL_3                   "fs:/image/core/messages_sel.png"
#define    IDLE_TOUCH_4                       "fs:/image/core/contact.png"
#define    IDLE_TOUCH_SEL_4                   "fs:/image/core/contact_sel.png"


#endif
#endif
#ifdef FEATURE_USES_BLACKBERRY
#define    IDLE_BLACKBERRY_ITEMMAX                 6
#ifdef FEATURE_DISP_160X128
#define    IDLE_BLACKBERRY_DRAWDX                  32
#define    IDLE_BLACKBERRY_DRAWDY                  32
#define    IDLE_BLACKBERRY_1                       "fs:/image/BlackBerry/messages.png"
#define    IDLE_BLACKBERRY_SEL_1                   "fs:/image/BlackBerry/messages_sel.png"
#define    IDLE_BLACKBERRY_2                       "fs:/image/BlackBerry/contact.png"
#define    IDLE_BLACKBERRY_SEL_2                   "fs:/image/BlackBerry/contact_sel.png"
#define    IDLE_BLACKBERRY_3                       "fs:/image/BlackBerry/alarm.png"
#define    IDLE_BLACKBERRY_SEL_3                   "fs:/image/BlackBerry/alarm_sel.png"
#define    IDLE_BLACKBERRY_4                       "fs:/image/BlackBerry/calendar.png"
#define    IDLE_BLACKBERRY_SEL_4                   "fs:/image/BlackBerry/calendar_sel.png"
#define    IDLE_BLACKBERRY_5                       "fs:/image/BlackBerry/mediaplayer.png"
#define    IDLE_BLACKBERRY_SEL_5                   "fs:/image/BlackBerry/mediaplayer_sel.png"
#define    IDLE_BLACKBERRY_6                       "fs:/image/BlackBerry/fmradio.png"
#define    IDLE_BLACKBERRY_SEL_6                   "fs:/image/BlackBerry/fmradio_sel.png"

#else
#define    IDLE_BLACKBERRY_DRAWDX                  32
#define    IDLE_BLACKBERRY_DRAWDY                  32
#define    IDLE_BLACKBERRY_1                       "fs:/image/BlackBerry/messages.png"
#define    IDLE_BLACKBERRY_SEL_1                   "fs:/image/BlackBerry/messages_sel.png"
#define    IDLE_BLACKBERRY_2                       "fs:/image/BlackBerry/contact.png"
#define    IDLE_BLACKBERRY_SEL_2                   "fs:/image/BlackBerry/contact_sel.png"
#define    IDLE_BLACKBERRY_3                       "fs:/image/BlackBerry/alarm.png"
#define    IDLE_BLACKBERRY_SEL_3                   "fs:/image/BlackBerry/alarm_sel.png"
#define    IDLE_BLACKBERRY_4                       "fs:/image/BlackBerry/calendar.png"
#define    IDLE_BLACKBERRY_SEL_4                   "fs:/image/BlackBerry/calendar_sel.png"
#define    IDLE_BLACKBERRY_5                       "fs:/image/BlackBerry/mediaplayer.png"
#define    IDLE_BLACKBERRY_SEL_5                   "fs:/image/BlackBerry/mediaplayer_sel.png"
#define    IDLE_BLACKBERRY_6                       "fs:/image/BlackBerry/fmradio.png"
#define    IDLE_BLACKBERRY_SEL_6                   "fs:/image/BlackBerry/fmradio_sel.png"

#endif
#endif
// 下列变量为临时定义
#define PHONEPASSWORDLENTH      OEMNV_LOCKCODE_MAXLEN
#define LOCKUIMPASSWORDLENTH      OEMNV_LOCKUIM_MAXLEN
#define IDLE_ICON_HEIGHT        16
#define PHONENUMBER             20

#define PINCODE_LENGTH          UIM_MAX_CHV_DIGITS
#define PUKCODE_LENGTH          UIM_MAX_CHV_DIGITS

#define MAX_STRING_LENGTH      64
#define MAX_BATTERY_POP_COUNT  3

#ifdef FEATURE_VERSION_MYANMAR
#ifdef FEATURE_VERSION_VG68
#define MUSIC_WIDTH            (SCREEN_HEIGHT-52) //88
#define MUSIC_START_WIDTH      (SCREEN_HEIGHT-50) //90
#else
#define MUSIC_WIDTH            (SCREEN_HEIGHT-42) //88
#define MUSIC_START_WIDTH      (SCREEN_HEIGHT-60) //90
#endif
#else
#ifdef FEATURE_USES_BLACKBERRY
#define MUSIC_WIDTH            (SCREEN_HEIGHT-82) //78
#define MUSIC_START_WIDTH      (SCREEN_HEIGHT-80) //80
#else
#ifdef FEATURE_LCD_TOUCH_ENABLE
#define MUSIC_WIDTH            (SCREEN_HEIGHT-137) //78
#define MUSIC_START_WIDTH      (SCREEN_HEIGHT-135) //80
#else
#define MUSIC_WIDTH            (SCREEN_HEIGHT-52) //78
#define MUSIC_START_WIDTH      (SCREEN_HEIGHT-50) //80
#endif
#endif
#endif

#ifdef FEATURE_SPN_FROM_BSMCCMNC
#define OPERATOR_NAME_MAX_SIZE   32

typedef struct _ServiceProviderList
{
    word   mcc;
    byte   mnc;
    char  ids_name[OPERATOR_NAME_MAX_SIZE];
} ServiceProviderList;
#endif //FEATURE_SPN_FROM_BSMCCMNC

#define CORE_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )


#define CORE_ERR(...)

// 关闭对话框宏
#define CLOSE_DIALOG(DlgRet)                    \
{                                               \
    pMe->m_eDlgRet = DlgRet;                    \
    (void) ISHELL_EndDialog(pMe->a.m_pIShell);  \
}

// 状态移动宏：先将前一状态用当前状态更新，再将当前状态设为nextState
#define MOVE_TO_STATE(nextState)            \
{                                           \
    CoreAppState    State;                  \
    State = nextState;                      \
    pMe->m_ePreState = pMe->m_eCurState;    \
    pMe->m_eCurState = State;               \
}

// 画界面底部提示条宏定义
#define COREAPP_DRAW_BOTTOMBAR(x)                           \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = x;                         \
    DrawBottomBar(pMe->m_pDisplay, &BarParam);      \
}
/*==============================================================================
                                 
                                内部数据类型定义
                                 
==============================================================================*/
// Core Applet 对话框关闭时返回值列表
typedef enum DLGRetValue
{
    // 初始值，表明需要创建对话框
    DLGRET_CREATE,/*0*/
    
    DLGRET_ACCEPTED,/*1*/
    DLGRET_REJECTED,/*2*/
    
    DLGRET_EMGCALL,/*3*/
    
    // 继续输入
    DLGRET_CONTINPUT,/*4*/
    
    DLGRET_ENTEROK,/*5*/
    
    DLGRET_DIAL,/*6*/
    DLGRET_MSG,/*7*/
    
    // 短信提示
#if defined(FEATURE_WMS_APP)
    DLGRET_SMSTIPS,/*8*/
    DLGRET_VIEWSMS,/*9*/
    DLGRET_SMSTIPS_OK,/*10*/
#endif    
    
    // 对话框关闭时可返回的通用值
    DLGRET_OK,/*11*/
    DLGRET_MSGOK,/*12*/
    
    DLGRET_INITED,/*13*/
    DLGRET_CANCELED,/*14*/
    DLGRET_BATT_INFO,/*15*/
    DLGRET_RTC,
    DLGRET_ENTER,
    DLGRET_GS_SHAKE

#if (defined(FEATURE_PLANEMODE) || defined(FEATURE_OEMOMH))
    ,DLGRET_YES
    ,DLGRET_NO
#endif
#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)
	,DLGRET_SALES_TRACKER
	//#ifdef FEATURE_VERSION_W317A
	,DLGRET_SALES_SUCESS
	//#endif 
	,DLGRET_WMS_DRAFT_SAVED
#endif
} DLGRetValue;

// Core Applet 状态处理函数返回给状态机函数的值类型
typedef enum NextFSMAction
{
    NFSMACTION_WAIT,
    NFSMACTION_CONTINUE
} NextFSMAction;


// Core Applet 状态机状态：
typedef enum _CoreAppState
{
   COREST_NONE,//0
   
   // 做必要的初始化工作,无界面
   COREST_INIT,/*1*/
   
   COREST_ALARM,/*2*/
   // 关机充电
   COREST_LPM,/*3*/
   
   // 服务编程验证
   COREST_SERVICEREQ,/*4*/
   
   // 手机密码验证
   COREST_VERIFYPHONEPWD,/*5*/
   
   // UIM 相关验证(包括PIN、PUK、合法性验证)
   COREST_VERIFYUIM,/*6*/
   
   // UIM卡错误提示(包括PIN、PUK、合法性验证失败、有卡版本时没插入卡)
   COREST_UIMERR,/*7*/
   
   // 紧急呼叫
   COREST_EMERGENCYCALL,/*8*/
   
   // 开机时,用户相关验证通过后进入开机动画前的过渡状态，
   // 这里做部分耗时少的系统必要的初始化
   COREST_POWERONSYSINIT,/*9*/
   
   // 播放开机动画
   COREST_STARTUPANI,/*10*/
   
   // 开机应用 Cache 数据初始化
   COREST_POWERONAPPSDATAINIT,/*11*/
   
   // 待机状态
   COREST_STANDBY,/*12*/
   
#if defined(FEATURE_WMS_APP)
   COREST_SMSTIP,/*13*/
#endif   
   
   // 通用消息提示、事件通知状态
   COREST_NOTICE,/*14*/
   
   // 睡眠(屏幕保护状态)
   COREST_DEEPSLEEP,/*15*/
   
   // 关机提示
   COREST_POWEROFF,/*16*/

#ifdef FEATURE_UTK2
   //UTK refresh
   COREST_UTKREFRESH,
#endif //FEATURE_UTK2   
#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)
  COREST_SALES_TRAKER,
  COREST_SALES_EDIT,
#endif
//#ifdef	FEATURE_VERSION_W317A
  COREST_SALES_SUCCESS,
//#endif

} CoreAppState;

typedef enum
{
    ENTER_NONE,         // 没有要求输入 PIN 或 PUK 码
    ENTERPIN_ONLY,      // 要求输入 PIN 码
    ENTERPUK_STEP0,     // 要求输入 PUK 码第一步，输入 PUK 码
    ENTERPUK_STEP1,     // 要求输入 PUK 码第二步，输入新 PIN 码
    ENTERPUK_STEP2      // 要求输入 PUK 码第三步，确认新 PIN 码
} Enter_UIMSecurityCode_e_Type;

typedef enum
{
    UIMERR_UNKNOW,       // 未知状态
    UIMERR_NONE,         // UIM 卡正常无误
    UIMERR_NOUIM,        // 没插入 UIM 卡
    UIMERR_BLOCKED,      // UIM 卡无效(原因输PUK码超过规定次数导致卡失效)
    UIMERR_LOCKED        // UIM 卡无效(原因是卡不符合运营商要求而要求锁定)
} UIMERR_e_Type;

typedef enum
{
    QQSTATE_ONLINE,       // 在线
    QQSTATE_AWAYLINE,     // 离开
    QQSTATE_OFFLINE,      // 离线
    QQSTATE_INVISIBLE,    // 隐身
    QQSTATE_HAVEMSG,      // 来消息
    QQSTATE_QUIT          // 退出
} QQ_STATE;


typedef enum
{
    POWERDOWN_NORMAL,               // 正常关机
    POWERDOWN_RESET                 // 关机需重起
#ifdef FEATURE_UIM
    , POWERDOWN_RUIM_DOOR_RESET     // 卡要求关机重起
#endif
} PowerDownType;

#ifdef FEATURE_POWERUP_REGISTER_CHINAUNICOM
typedef struct _REGISTER_ITEM
{
    char        szMobileIMSI[16];   // 手机 IMSI 号
    boolean     m_bRegistered;      // 是否使用该 IMSI 号注册
} REGISTER_ITEM;

// 待机 Applet 配置结构
typedef struct _IdleAPP_Config_Type
{
    REGISTER_ITEM    m_RegItem;
    
    // 用于指示配置信息是否初始化，用特定值 IDLE_CFG_VERSION表示已初始化
    int   nInitIndictor;  
} IdleAPP_Config_Type;
#endif

#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)
typedef struct _MOBILETRACKER_ITEM
{
    char        szMobileIMSI[16];   // 手机 IMSI 号
    
} MOBILETRACKER;

// 待机 Applet 配置结构
typedef struct _IdleAPP_Config_MobileTracker_Type
{
    MOBILETRACKER   m_RegItem;
	int   nMobiletrackerInitIndictor;  
} IdleAPP_Config_MobileTracker_Type;

#endif

// 对话框事件处理函数指针类型定义
typedef boolean (*PFNDLGHANDLER)(void *pUser, AEEEvent evt, uint16 w, uint32 dw);

typedef struct _CCoreApp
{
    // 本结构的第一个数据成员必须是 AEEApplet 型
    AEEApplet a ;
    
    // 外部接口指针
    IDisplay            *m_pDisplay;
    IAnnunciator        *m_pIAnn;
    IConfig             *m_pConfig;
    IBattery            *m_pBatt;
    IRUIM               *m_pIRUIM;
#ifdef FEATURE_ICM
    ICM                 *m_pCM;
#else
    ITelephone          *m_pITelephone;
	ICallMgr            *m_pICallMgr;
	IPhoneCtl           *m_pIPhoneCtl;
#endif
    //IPhone       *m_pPhone;
    
    // 当前活动对话框IDialog接口指针
    IDialog             *m_pActiveDlg;
    
    // 当前活动对话框的事件处理函数指针
    PFNDLGHANDLER       m_pDialogHandler;

    boolean             m_bSuspended; 
    
    boolean             m_bActive;
    
    boolean             m_pAppStart;

    boolean             m_bBatteryActive;
#ifdef FEATURE_UIALARM
    IAlarm       *m_pAlarm;
#endif    
    
    // 用于 Applet 的矩形显示区域
    AEERect             m_rc;
    
    // 活动对话框关闭时的返回结果
    DLGRetValue         m_eDlgRet;
    
    // 当前活动对话框ID
    uint16              m_wActiveDlgID;
    
    // 是否不改写对话框返回结果，一般情况需要改写(FALSE)。
    boolean             m_bNotOverwriteDlgRet;
    
    // Applet 前一状态
    CoreAppState        m_ePreState;
    
    // Applet 当前状态
    CoreAppState        m_eCurState;
    
    // Applet 下一状态
    CoreAppState        m_eNextState;
    
    // 用于保存用户输入的手机密码
    char                m_strPhonePWD[PHONEPASSWORDLENTH + 1];
	#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)
	char                m_strPhoneNUM[PHONENUMBER];
	#endif
    #ifdef FEATURE_LCD_TOUCH_ENABLE
    char                m_strPhonePWDtemp[PHONEPASSWORDLENTH + 1];
    #endif
    char                m_strLockuimPWD[LOCKUIMPASSWORDLENTH + 1];
    uint16             m_strlockuimPWD;
    UIMERR_e_Type       m_eUIMErrCode;
    Enter_UIMSecurityCode_e_Type  m_eRUIMSCode;
    
    char                m_strPIN1[PINCODE_LENGTH + 1];
    char                m_strPIN2[PINCODE_LENGTH + 1];
    char                m_strPUK[PUKCODE_LENGTH + 1];
    AECHAR              m_wPIN[PINCODE_LENGTH + 1];
    AECHAR              m_wPIN2[PINCODE_LENGTH + 1];
    AECHAR              m_wPUK[PUKCODE_LENGTH + 1];
    
    // 消息文本字符串在资源文件中的 ID
    uint16              m_nMsgID;
    
    // 是否捕获到系统时间
    boolean             m_bAcquiredTime;
    
    // 用于记录待机下拨号时第一个按键输入
    AECHAR              m_wstrEnterNum[4];
    
    // LARGE 字体高度
    int                 m_nLargeFontHeight;
    
    // Normal 字体高度
    int                 m_nNormalFontHeight;
    
    // 是否要退出 Core Applet : 只有在关机时才能置为 TRUE
    boolean             m_bExit;
    
    PowerDownType       m_ePowerDownType;

   // 是否外接电源
    boolean             m_bExtPwrState;
    boolean             m_bemergencymode;
    boolean             m_bChargFull;

    IALERT              *m_pAlert;           //IALERT指针
#ifndef FEATURE_USES_LOWMEM
    //开机动画图片
    IImage             *m_pStartupAniImg;
#else
#endif
    //IImage             *m_pSubStartupAniImg;//not used,2008/03/31
    
    // 开机动画每帧显示的毫秒数
    uint16             m_wStartupAniFrameRate;
    
    // 开机动画播放时长
    uint16             m_wStartupAniTime;
#ifdef FEATRUE_AUTO_POWER
    boolean            m_b_autopower_off;
    boolean            m_b_needclose_core;
#endif
#ifndef CUST_EDITION
    PowerDown_Alarm_Cfg   m_PowerDownAlarmCfg;
#endif
    IAddrBook          *m_pAddrPhone;     // IADDRBOOK interface for phone
#ifdef FEATURE_RUIM_PHONEBOOK
    IAddrBook          *m_pAddrRUIM;      // IADDRBOOK interface for RUIM
#endif /* FEATURE_RUIM_PHONEBOOK */	
    boolean             m_SYS_MODE_NO_SRV;
    
#if defined(FEATURE_WMS_APP)
    boolean             m_bsmstipscheck;
#endif
    AECHAR              svc_p_name[UIM_CDMA_HOME_SERVICE_SIZE+1];
    IBacklight*         m_pBacklight;
    byte                m_battery_time;
    boolean             m_battery_state;
	boolean             m_battery_msgup;
#ifdef FEATURE_UTK2
    uint16     wRefreshMask;
#endif
#ifndef FEATURE_USES_LOWMEM
    IImage *            m_battery_Image;
#endif
    byte                m_battery_count;
#ifdef FEATURE_KEYGUARD
    boolean             m_b_set_lock;
#endif
#ifdef FEATRUE_SET_ANN_FULL_SCREEN
    db_capture_type     m_capture;
#endif 
    boolean             m_bProvisioned; 
    boolean             m_bConfigSent;
    AECHAR             *m_cdg_msgptr;

#if defined( FEATURE_POWERDOWN_ALARM)
	boolean             powerupByAlarm;
#endif
    boolean             bunlockuim;

#ifdef FEATURE_PLANEMODE
    boolean             bPlaneModeOn;
#endif
#ifdef FEATURE_APP_MUSICPLAYER
    uint16              m_nAutoScrollIdx;
    AECHAR              *m_pMusicName;
#endif

#ifdef FEATURE_TORCH_SUPPORT
	boolean TorchOn;
#endif

    ICard *             m_pICard;               /* pointer to icard interface */
    uint8               m_nCardStatus;
    /***********************************
          For Security Module
    **********************************/
    AEECallback         m_sCallback;
    AEECardPinCode      m_sPIN;
    AEECardPinCode      m_sPIN2;
    AEECardPinCode      m_sPUK;
    AEECardPinCmdStatus m_sPinActionStatus;
    boolean             m_bVerifying;
	boolean             m_isShift;
	#ifdef FEATURE_USES_BLACKBERRY
	uint8               m_CurMainItems;    //当前主菜单ITEMS
	IImage              *m_pImageIcon[IDLE_BLACKBERRY_ITEMMAX];
	IImage              *m_pImageSelIcon[IDLE_BLACKBERRY_ITEMMAX];
	#endif
	
#ifdef FEATURE_LCD_TOUCH_ENABLE
#ifdef FEATURE_DISP_240X320
	IImage              *m_pImageTouchIcon[IDLE_TOUCH_ITEMMAX];
	IImage              *m_pImageTimeIcon[IDLE_TIME_COUNT];
#else
#endif
#endif
	//wangliang add!
	byte                m_active;
	byte                m_CurProfile;      //当前情景模式
	#ifdef FEATURE_KEYGUARD
	boolean             m_iskeypadtime;
	#endif
    #if defined(FEATURE_VERSION_C11)||defined(FEATURE_VERSION_W317A)
	boolean             m_keyinfoheld;
	#endif
	boolean             m_IsSametime;
    #ifdef FEATURE_QQ_APP 
    QQ_STATE            m_qqstate;
    #endif
    AECHAR             *m_pADNName;
    AECHAR             *m_pADNNumber;
#ifdef FEATURE_OEMOMH 
    AEERect            timeRc;
#endif
#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)
#ifdef FEATURE_UIALARM
   IAlarm      *m_pIAlarm;
#endif
   IMenuCtl       *m_pSmsTrackTime;           //IMenuCtl指针,"状态"
   ITextCtl       *m_pSmsTrackNumber; 
#endif
   boolean         m_IsEanbleBatAlarm;

} CCoreApp;

/*==============================================================================

                                 函数声明

==============================================================================*/
#ifdef FEATURE_UIALARM
void CoreApp_EnableShellAlarms(CCoreApp  *pMe, boolean  bEnabled);
#endif

/*==============================================================================
函数:
    CoreApp_ShowDialog
       
说明:
    函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
    pMe [In]: 指向 Core Applet对象结构的指针,该结构包含小程序的特定信息.
    dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
    none

备注:

==============================================================================*/
void CoreApp_ShowDialog(CCoreApp *pMe,uint16  dlgResId);

/*==============================================================================
函数:
    CoreApp_ShowMsgDialog
       
说明:
    函数用来显示指定消息的对话框。

参数:
    pMe [In]: 指向 Core Applet对象结构的指针,该结构包含小程序的特定信息.
    nResId [in]：消息字符串在资源文件中的 ID 号。

返回值:
    none

备注:

==============================================================================*/
void CoreApp_ShowMsgDialog(CCoreApp *pMe,uint16  nResId);

/*==============================================================================
函数:
    CoreApp_RouteDialogEvent
       
说明:
    函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
    pMe [In]: 指向 Core Applet对象结构的指针,该结构包含小程序的特定信息.
    eCode [in]：事件代码。
    wParam [in]：与事件eCode关联的数据。
    dwParam [in]：与事件eCode关联的数据。

返回值:
    TRUE：传入事件被处理。
    FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean CoreApp_RouteDialogEvent(CCoreApp *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

/*==============================================================================
函数:
    CoreApp_SetDialogHandler
       
说明:
    函数根据 pMe->m_wActiveDlgID 设定对话事件处理函数指针 m_pDialogHandler 相应值。

参数:
    pMe [In]: 指向 Core Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
    none

备注:

==============================================================================*/
void CoreApp_SetDialogHandler(CCoreApp *pMe);

/*==============================================================================
函数:
    CoreApp_ProcessState
       
说明:
    Core Applet状态处理主函数。函数根据 pMe->m_eCurState 将调用路由至
    相应的状态处理函数。

参数:
    pMe [In]: 指向 Core Applet 对象结构的指针,该结构包含小程序的特定信息.

返回值:
    NFSMACTION_CONTINUE: 指示后有子状态，状态机不能停止。
    NFSMACTION_WAIT: 指示因要显示对话框界面给用户，应退出状态机。

备注:

==============================================================================*/
NextFSMAction CoreApp_ProcessState(CCoreApp *pMe);

/*==============================================================================
函数:
    CoreApp_RegisterNotify

说明:
    用于将话机置于在线操作模式。

参数:
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:
       
==============================================================================*/
boolean CoreApp_RegisterNotify(CCoreApp *pMe);

/*==============================================================================
函数:
    CoreApp_InitExtInface

说明:
    用于将话机置于在线操作模式。

参数:
    pMe [in]:指向 Core Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:
       
==============================================================================*/
boolean CoreApp_InitExtInterface(CCoreApp *pMe);

/*==============================================================================
函数      :  CoreApp_IsIdle

说明      :  函数用于获取COREAPP是否处于IDLE状态
==============================================================================*/
boolean CoreApp_IsIdle(void);
#ifdef FEATURE_ICM
boolean CoreApp_IsEmergencyMode(ICM* pICM);
#else
boolean CoreApp_IsEmergencyMode(ITelephone* pICM);
#endif
void CoreApp_UpdateAnnunciator(CCoreApp *pMe);

extern AEE_EXPORTS IShell    *AEE_GetShell(void);

void CoreApp_Draw_Charger_image(void *pp);

void CoreApp_Poweroff_Phone(void *pp);
#ifdef FEATURE_POWERUP_REGISTER_CHINAUNICOM
/*==============================================================================
函数：
    CoreApp_SendReginfoTimer

说明：
    用于发送 EVT_SENDREGINFOR 给 AEECLSID_CORE_APP 通知程序发送手机注册事件。

参数：
    pme [in]：指向CCoreApp Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
    无。

备注：
    若 Applet 被挂起，将不做任何操作直接返回。
       
==============================================================================*/
void CoreApp_SendReginfoTimer(void *pme);

/*==============================================================================
函数：
    CoreApp_SendReginfo

说明：
    函数用来发送注册信息。

参数：
    pMe [in]：指向CCoreApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
    int 

备注:

==============================================================================*/
int CoreApp_SendReginfo(CCoreApp *pMe);
#endif

#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)
/*==============================================================================
函数：
    CoreApp_MobileTracker

说明：
    函数用来发送注册信息。

参数：
    pMe [in]：指向CCoreApp Applet对象结构的指针。该结构包含小程序的特定信息。
       
返回值：
    int 

备注:

==============================================================================*/
int CoreApp_MobileTracker(CCoreApp *pme);
void CoreApp_MobileTrackerTimer(void *pme);
void CoreApp_SalesTrackerTimer(void *pme);
void CoreApp_SmsTrackerTimer(void *pme);
#endif


void CoreApp_load_uim_esn(CCoreApp *pMe);
int CoreApp_GetBatteryLevel(CCoreApp *pMe);
#ifdef FEATURE_SEAMLESS_SMS
void CoreApp_SendSeamlessSMSTimer(void *pme);
int  CoreApp_SendSeamlessSMS(CCoreApp *pMe);
#endif //#ifdef FEATURE_SEAMLESS_SMS
#ifdef FEATURE_ICM
void InitAfterPhInfo(CCoreApp *pMe, AEECMOprtMode mode);
#else
void InitAfterPhInfo(CCoreApp *pMe, AEETOprtMode mode);
#endif
void CoreApp_ProcessSubscriptionStatus (CCoreApp *pMe);
void CoreApp_HandleAlarm(CCoreApp  *pme, uint16 wPermID);
#endif
