#ifndef BREWUI_H
#define BREWUI_H
/*=============================================================================
//           U S E R   I N T E R F A C E   M A I N   H E A D E R
// 文件：
//        brewui.h
//        
// 文件说明：
//        本文件是其它模块使用 oenui task 的外部接口。
//
//
// 作者：2007-08-27
// 当前版本：
==============================================================================*/


/*==============================================================================
修改历史记录：
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------

==============================================================================*/

/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "comdef.h"               /* basic definitions */
#include "queue.h"                /* queue services */
#include "rex.h"                  /* REX services */
#include "target.h"               /* Target configuration file */
#include "customer.h"             /* Customer Specific Definitions */

#if !defined(FEATURE_WCDMA) && !defined(FEATURE_GSM)
#include "mc.h"
#else
#error code not present
#endif 

#include "cm.h"
#ifdef FEATURE_MULTIMEDIA
#include "fs.h"
#include "cmx.h"
#endif

#ifdef FEATURE_UIM_TOOLKIT
#include "uim.h"
#endif
#include "hs.h"


/*==============================================================================
                            定义外部变量
===============================================================================*/

/*==============================================================================
                                 宏定义和常数
==============================================================================*/
// oemui task 信号定义
#define UI_CMD_Q_SIG      0x0001UL  /* Something on the command queue */
#define UI_KEY_SIG        0x0002UL  /* Key from HS task */
#define UI_RPT_TIMER_SIG  0x0004UL  /* Time to kick watchdog */
#define UI_NV_SIG         0x0008UL  /* Return from NV */
#define UI_WAIT_SIG       0x00010UL
#define UI_RTC_SIG         0x0020UL  
#define UI_KEY_HELD_SIG     0x0040UL // Last key was held
#define UI_KEY_HELD_PWR_SIG   0x0080UL // Last key was held to make PWR

#define AEE_APP_SIG       0x10000UL
#define UI_RUIM_SIG		  0x20000UL
#define AEE_SLEEP_SIG     0x40000UL /* AEE_SLEEP_SIG for Brew 3.1 Sleep. */
#define AEE_SIO_SIG       0x80000UL
#ifdef CUST_EDITION
#define AEE_MEDIA_SIG     0x0400UL
#endif
#define UI_MULTI_STOP_SIG   0x0100UL  // Multitone has ended
#define UI_RING_SIG         0x0200UL  // End of a ring from SND task

#ifdef FEATURE_TOUCHPAD
#define UI_GET_PEN_SIG      0x0800UL /* return from vc task */
#endif
/*==============================================================================
                                 
                                 数据类型定义
                                 
==============================================================================*/
// oemui task 支持的命令名
typedef enum 
{
    UI_PROACTIVE_UIM_F,
    
    UI_NUM_UI_COMMANDS,              /* End of command list (note no comma!)    */
    UI_ITEMS_ENUM_PAD = 0x7FFF
} ui_name_type;

// 命令 buffer 中， oemui task 返回的状态
typedef enum
{
    UI_DONE_S,                      /* General OK return */
    UI_BUSY_S,                      /* Command is being processed */
    UI_BADCMD_S,                    /* Invalid command */
    UI_BADPARM_S                    /* Bad Parameters */
} ui_status_type;

// 通用命令头类型：所有发给 UI 的命令必须以该头部结构开始
typedef struct 
{
    q_link_type        link;         /* Queue link */                          
    ui_name_type       cmd;          /* Command to execute */                  
    rex_tcb_type       *task_ptr;    /* Pointer to requesting task TCB */
    rex_sigs_type      sigs;         /* Signal to set upon cmd completion */
    ui_status_type     status;       /* Completion code status */
    q_type             *done_q_ptr;  /* Queue to place this cmd on when done */
} ui_hdr_type;

#ifdef FEATURE_UIM_TOOLKIT
// UIM 原语命令类型
typedef struct
{
    ui_hdr_type                hdr;                       /* header */
    byte                       num_bytes;                 /* length */
    byte                       cmd_data[UIM_MAX_CHARS];   /* data */
} ui_proactive_uim_cmd_type;
#endif 

// ui 命令类型
// 全部命令的联合体。命令头总是存在，它指定命令类型和属性。若命令带参数，则
// 参数紧随联合体命令头后
typedef union ui_cmd_u 
{
    ui_hdr_type                   hdr;

#ifdef FEATURE_UIM_TOOLKIT
    ui_proactive_uim_cmd_type     proactive_cmd;
#endif
} ui_cmd_type;

#ifdef FEATURE_TTY
typedef enum 
{
    UI_TTY_FULL,
    UI_TTY_TALK,
    UI_TTY_HEAR,
    UI_TTY_OFF
} ui_tty_option_type;
#endif

// ui task 使用的全局参数变量集合体
typedef struct
{
    boolean offline;                /* phone is offline? */
    boolean pwr;                    /* power on or off? */
    boolean onhook;                 /* phone on hook? */
    boolean headset;                /* Headset attached ? */
    boolean headset_is_mono;        /* TRUE = mono headset */
    boolean extdev;                 /* External device ? */
    boolean mute;                   /* phone muted? */
    boolean spkr;                   /* hands free speaker? */
    boolean txdtmf;                 /* transmit dtmfs? */
    boolean isdebug;                /* debug screen on? */
    boolean powerup;                /* powered up */
    boolean extpwr;                 /* external power */
#ifdef FEATURE_AUDIO_AGC
    boolean       aagc_enable;
#endif /* FEATURE_AUDIO_AGC */

    char  mm_ringer_file[FS_FILENAME_MAX_LENGTH]; /* file to use for Ringer */

#ifdef FEATURE_USB_ISOC_AUDIO
    boolean usb_hfk;               /* USB HFK attached ?   */
#endif /* FEATURE_USB_ISOC_AUDIO */

#ifdef FEATURE_TTY
    ui_tty_option_type tty_option; /* TTY Options */
#endif /* FEATURE_TTY */

#ifdef FEATURE_ANALOG_HFK
    boolean       analog_hfk;
#endif /* FEATURE_ANALOG_HFK */

#ifdef FEATURE_STEREO_DAC
    boolean sdac;                 /* Stereo DAC attached ?        */
#endif
    
    boolean uimunlocked;          /* UIM 卡是否通过PIN或PUK解锁以致卡上信息能够读取 */
} brewui_global_data_type;

#ifndef FEATURE_REX_DYNA_MEM_UI
// free buffers for talking to UI
extern q_type ui_cmd_free_q;      
#endif 


/*==============================================================================
函数: 
    brewui_headsetconnected
       
说明: 
    函数用于判断当前话机是否连上了耳机。
       
参数: 
    none
    
返回值:
    TRUE: 话机连上了耳机
    FALSE: 话机没连上了耳机
       
备注:
    Brew 必须已启动

==============================================================================*/
boolean   brewui_headsetconnected(void);


/*==============================================================================
函数: 
    brewui_isincall
       
说明: 
    函数用于判断当前话机是否处于通话状态。
       
参数: 
    none
    
返回值:
    TRUE: 话机处于通话状态
    FALSE: 话机处于非通话状态
       
备注:
    Brew 必须已启动

==============================================================================*/
boolean   brewui_isincall(void);

/*==============================================================================
函数: 
    IsRunAsFactoryTestMode
       
说明: 
    函数用于判断当前软件是否按厂测模式运行。
       
参数: 
    none
    
返回值:
    TRUE: 当前软件运行为厂测模式
    FALSE: 当前软件运行为非厂测模式
       
备注:
    使用软件的版本是否有卡版本：无论 RTRE 配置如何，使用的软件版本要不无卡、要不
    有卡，二者只能取一，无其它选择!

==============================================================================*/
boolean   IsRunAsFactoryTestMode(void);

/*==============================================================================
函数: 
    IsRunAsUIMVersion
       
说明: 
    函数用于判断当前软件是否按有卡版本运行。
       
参数: 
    none
    
返回值:
    TRUE: 当前软件运行为有卡版本
    FALSE: 当前软件运行为无卡版本
       
备注:
    使用软件的版本是否有卡版本：无论 RTRE 配置如何，使用的软件版本要不无卡、要不
    有卡，二者只能取一，无其它选择!

==============================================================================*/
boolean   IsRunAsUIMVersion(void);

/*==============================================================================
函数: 
    ui_task
       
说明: 
    主要用户接口函数。函数用一个大的无限循环处理各种输入和信号。函数主要将输入
    转变为事件，也处理伴随按键的背光和按键音
       
参数: 
    dummy [in]: 暂不用。
    
返回值:
    从不返回
       
备注:
    REX 、NV 、 DIAG 及 HS 任务必须已运行
    
==============================================================================*/
void ui_task(dword dummy   /*lint -esym(715,dummy) */);

#ifdef FEATURE_REX_DYNA_MEM_UI
/*==============================================================================
函数: 
    ui_get_cmd
       
说明: 
    函数用来从 ui 命令可用缓冲区队列中取一条命令 buffer 返回。
       
参数: 
    none
    
返回值:
    ui_cmd_type 指针。
       
备注:
    REX 、NV 、 DIAG 及 HS 任务必须已运行，TMC heap is initialized.
    
==============================================================================*/
ui_cmd_type* ui_get_cmd(void);
#endif

/*==============================================================================
函数: 
    ui_cmd
       
说明: 
    Allows other tasks to queue up messages to the UI.  Grab a buffer from the
    ui_cmd_free_q, fill it in, and call ui_cmd with it.  All this will
    do is queue it and set a signal for the UI task.  When the command
    is done, the buffer will be queued as directed in the command header,
    and a task you specify may be signaled.
       
参数: 
    cmd_ptr [in]: ui 命令块指针
    
返回值:
    none
       
备注:
    REX 、NV 、 DIAG 及 HS 任务必须已运行，ui_cmd_q already initialized.
    
==============================================================================*/
void ui_cmd(ui_cmd_type *cmd_ptr);

/*==============================================================================
函数: 
    ui_get_lcd_handle
       
说明: 
    Gets the LCD_HANDLE
       
参数: 
    none
    
返回值:
    lcd_handle_type
       
备注:
    
==============================================================================*/
lcd_handle_type ui_get_lcd_handle(void);

/*==============================================================================
函数: 
    ui_get_kpd_handle
       
说明: 
    Gets the KPD_HANDLE
       
参数: 
    none
    
返回值:
    kpd_handle_type
       
备注:
    
==============================================================================*/
kpd_handle_type ui_get_kpd_handle(void);
#ifdef FEATURE_OTASP
/*==============================================================================
函数: 
    VoteForSleep
       
说明: 
    Set the ui_pwrdown_complete static variable. 
       
参数: 
    flag [in]: TRUE-关机完成; FALSE-关机尚未完成
    
返回值:
    none
       
备注:
    
==============================================================================*/
void oemui_setpwrdncomplete(boolean flag);
#endif

/*===========================================================================
FUNCTION   oemui_cm_init

DESCRIPTION
  initialize UI-CM interface

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  None
===========================================================================*/
void oemui_cm_init(void);

/*===========================================================================
FUNCTION InitProvisioning

DESCRIPTION
  initialize provisioning

DEPENDENCIES
  the corresponding initialization functions

SIDE EFFECTS
  None
===========================================================================*/
void InitProvisioning(void);

/*==============================================================================
函数: 
    BrewUI_EnableKeys
       
说明: 
    Tells the UI that the Coreapp is ready to start receiving keys
       
参数: 
    flag [in]: 是否激活向 UI 发送按键事件的标志
    
返回值:
    none
       
备注:
    
==============================================================================*/
void BrewUI_EnableKeys(boolean flag);

/*==============================================================================
函数: 
    oemui_setbusymode
       
说明: 
    Set ui task busy mode value
       
参数: 
    flag [in]: 是否处于忙的模式的标志
    
返回值:
    none
       
备注:
    
==============================================================================*/
void oemui_setbusymode(boolean flag);

int oemui_is_lcd_test_state(int bb);

/*==============================================================================
函数: 
    oemui_unlockuim
       
说明: 
    函数设置 gbrewui.uimunlocked 为真，表示 UI 已通过 PIN/PUK 码相关验证，卡上
    信息可以正确读取了。
       
参数: 
    
返回值:
    none
       
备注:
    
==============================================================================*/
void oemui_unlockuim(void);

/*==============================================================================
函数: 
    oemui_uimisunlocked
       
说明: 
    函数返回 gbrewui.uimunlocked 值，调用者根据返回值来确定卡上信息可以正确读取。
       
参数: 
    
返回值:
    TRUE: UI 已通过 PIN/PUK 验证
    FALSE: UI 尚未通过 PIN/PUK 验证
       
备注:
    
==============================================================================*/
boolean oemui_uimisunlocked(void);
void oemui_set_powerup_state(boolean flg);
boolean oemui_get_powerup_state(void);

#define APP_MEDIA_ALLOW         0    
#define APP_MEDIA_IMPACT_BY_FM  1
#define APP_MEDIA_IMPACT_BY_MP3 2
extern int app_media_scheduler(void);
#endif // BREWUI_H
