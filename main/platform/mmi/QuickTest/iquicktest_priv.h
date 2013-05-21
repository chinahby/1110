#ifndef IQUICKTESTPRIV_H
#define IQUICKTESTPRIV_H

/*==============================================================================
// 文件：
//        iquciktest_priv.h
//        版权所有(c) 2007 Anylook
//
// 文件说明：
//
// 作者：Gemsea
// 创建日期：2007-11-13
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------     -----------------------------------------------
07-11-13               初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "OEMFeatures.h"
#include "AEE.h"              // Standard AEE Declarations
#include "AEEModGen.h"
#include "AEEDate.h"
#include "AEEDB.h"            // Standard AEE Declarations
#include "AEEMenu.h"

#include "AEENet.h"           // AEENet services
#include "AEEAlert.h"
#include "AEEStdLib.h"        // AEE StdLib Services
#include "AEEText.h"          // AEE StdLib Services
#include "AEEFile.h"          // AEEFile Services
#include "AEEConfig.h"
#include "AEEBacklight.h"
#include "OEMCFGI.h"
#include "AEERUIM.h"
#include "quicktest.brh"
#if defined(BREW_STATIC_APP)
  #include "OEMClassIds.h"
#else
  #include "quicktest.bid"
#endif

#include "IQuickTest.h"
#ifndef WIN32
#include "Hs.h"
#include "voc.h"
#include "uim.h"
#include "Qdspext.h"
#endif
#include "AEEAnnunciator.h"
#include "AppComFunc.h"
#include "Appscommon.h"
//#include "AEECallList.h"
#include "AEECallHistory.h"
#include "appscommonimages.brh"
#include "OEMNV.h"

/*==============================================================================
                                 类型定义
==============================================================================*/
#define  AEE_APPSCOMMONRES_IMAGESFILE "fs:/sys/appscommonimages.bar"
#if defined(FEATURE_DISP_160X128)
#define SECONDE_LINE  50
#define THRID_LINE    66
#define FOUR_LINE     82
#define FIVE_LINE     98
#define LEFT_START    44
#define RITHT_START   94
#define RTIHT_END     116
#define DOWN_Y        34
#define ENDCALL_START 136
#define ENDCALL_END   140

#elif defined(FEATURE_DISP_220X176)
#define SECONDE_LINE  68
#define THRID_LINE    90
#define FOUR_LINE     112
#define FIVE_LINE     134
#define LEFT_START    60
#define RITHT_START   128
#define RTIHT_END     164
#define DOWN_Y        44
#define ENDCALL_START 192
#define ENDCALL_END   198
#elif defined(FEATURE_DISP_176X220)
#define SECONDE_LINE  68
#define THRID_LINE    90
#define FOUR_LINE     112
#define FIVE_LINE     168
#define LEFT_START    60
#define RITHT_START   128
#define RTIHT_END     164
#define DOWN_Y        44
#define ENDCALL_START 192
#define ENDCALL_END   198

#else
#define SECONDE_LINE  68
#define THRID_LINE    90
#define FOUR_LINE     112
#define FIVE_LINE     134
#define LEFT_START    60
#define RITHT_START   128
#define RTIHT_END     164
#define DOWN_Y        44
#define ENDCALL_START 192
#define ENDCALL_END   198
#endif
#define  AEE_QUICKTEST_RES_FILE (AEE_RES_LANGDIR QUICKTEST_RES_FILE)
typedef void (* qdsp_cmd_isr_func_type) (void);

/*----------------------对话框相关数据类型声明---------------------*/
// 对话框关闭时返回值列表
typedef enum DLGRetValue
{
   // 初始值，表明需要创建对话框
   DLGRET_CREATE,

   // 对话框关闭时可返回的通用值
   DLGRET_OK,
   DLGRET_CANCELED,

   // 主菜单对话框
   DLGRET_KEYTEST,            //按键测试
   DLGRET_TOUCHSCREENTEST,    //触摸屏测试
   DLGRET_YAMAHATEST,         //YAMAHA测试
   DLGRET_VIBRATETEST,        //振动测试
   DLGRET_BACKLIGHTTEST,      //背光测试
   DLGRET_FLIPTEST,           //翻盖测试
   DLGRET_FMTEST,             //FM测试
   DLGRET_CAMERATEST,               //camera 测试
   DLGRET_SDTEST,             //SD测试
   DLGRET_VERTEST,            //VER测试
   DLGRET_LCDTEST,            //LCD测试
   DLGRET_CALLTEST,           //mic测试
   DLGRET_REGULATE,           //检查是否已经校准
   DLGRET_RESTOREFACTORY      //恢复出厂设置   
} DLGRet_Value_e_Type;

/*----------------------状态机相关数据类型声明---------------------*/
// QuickTest Applet 状态机状态：
typedef enum _QuickTestState
{
   QUICKTESTST_NONE,
   QUICKTESTST_INIT,
   QUICKTESTST_MAIN,
   QUICKTESTST_KEYTEST,
   QUICKTESTST_TOUCHSCREENTEST,
   QUICKTESTST_YAMAHATEST,
   QUICKTESTST_VIBRATETEST,
   QUICKTESTST_BACKLIGHTTEST,
   QUICKTESTST_FLIPTEST,
   QUICKTESTST_FMTEST,
   QUICKTESTST_CAMERATEST,
   QUICKTESTST_SDTEST,
   QUICKTESTST_VERTEST,
   QUICKTESTST_LCDTEST,
   QUICKTESTST_CALLTEST,
   QUICKTESTST_REGULATE,
   QUICKTESTST_RESTOREFACTORY,   
   QUICKTESTST_EXIT
} QuickTestState;

// 状态处理函数返回给状态处理主函数的值类型
typedef enum NextFSMAction
{
   NFSMACTION_WAIT,
   NFSMACTION_CONTINUE
} NextFSMAction;
#ifdef FEATURE_SUPPORT_BT_APP
typedef enum _dilag_type
{
   LCD_TEST,
   BT_TEST
} dilag_type;
#endif
// QuickTest Applet对象结构体：
typedef struct _CQuickTest {
   DECLARE_VTBL(IQuickTest)

   uint32         m_nRefs;
   IModule        *m_pModule;
   IDisplay       *m_pDisplay;     // Display interface
   IShell         *m_pShell;       // Shell interface
   

   ISound         *m_pISound;
   IRUIM          *m_pIRUIM;
   IALERT         *m_pIAlert;
   uint16         colorMask;
   IRingerMgr     *m_pRingerMgr;

   AEERect        m_rc;

   // 当前活动对话框IDialog接口指针
   IDialog        *m_pActiveDlg;

   // 主菜单选择项
   uint16         m_mainMenuSel;

   // 当前活动对话框ID
   uint16         m_pActiveDlgID;

   // 活动对话框关闭时的返回结果
   DLGRet_Value_e_Type    m_eDlgRet;

   // 是否不改写对话框返回结果，一般情况需要改写(FALSE)。
   boolean        m_bNotOverwriteDlgRet;

   // Applet 前一状态
   QuickTestState m_ePreState;

   // Applet 当前状态
   QuickTestState m_eCurState;

   // Applet是否处于挂起状态
   boolean        m_bSuspending;

   // Applet是否准备好可以处理按键和命令事件。改变量主要用于快速按键引起的问题
   boolean        m_bAppIsReady;

   IConfig        *m_pConfig;        //IConfig 指针

   IBacklight     *m_pIBacklight;    //用来在设置完背光后立即生效用

   byte            m_nlightlevel;

   qdsp_cmd_isr_func_type m_qdspisr;
   IAnnunciator    *m_pIAnn;

#if defined( FEATURE_FM_RADIO)
    boolean m_fmClockwise;
    boolean m_fmIsPowerupBeforeFmTest;
    uint16  m_fmChannelBeforeFmTest;
#endif
    int     m_lineheight;
#ifdef FEATURE_SUPPORT_BT_APP
    dilag_type m_dilag_type;
#endif
#ifdef FEATURE_BREW_CAMERA 
    boolean m_isFormCamera;
#endif
    uint32  m_testkeycount;
    boolean m_quicktestmic;
} CQuickTest;
/*==============================================================================
                                 宏定义和常数
==============================================================================*/
// 为防止用户快速按键而添加事件EVT_APPISREADY，事件由定时器
// SysMenu_APPIsReadyTimer发出
#define APPISREADY_TIMER       400                       
#define EVT_APPISREADY         (EVT_USER+10)
#define MAX_STRING_LENGTH      64

// 状态移动宏：先将前一状态用当前状态更新，再将当前状态设为nextState
#define MOVE_TO_STATE(nextState) {            \
            QuickTestState tpState;            \
            tpState = nextState;          \
            pMe->m_ePreState = pMe->m_eCurState; \
            pMe->m_eCurState = tpState;        \
        }

/*==============================================================================
                                 全局数据
==============================================================================*/

/*==============================================================================
                                 本地（静态）数据
==============================================================================*/

/*==============================================================================
                                 函数声明
==============================================================================*/
/*----------------------对话框相关函数声明---------------------*/
/*==============================================================================
函数：QuickTest_UpdateActiveDialogInfo

说明：
       更新QuickTest Applet对象内关键的对话框参数。

参数：
       pMe [in]：指向QuickTest Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam [in]：活动对话框资源ID；
       dwParam [in]:：指向活动对话框的IDialog接口指针。

返回值：
        无

备注：:
       更新后的QuickTest Apple对话框属性。QuickTest_RouteDialogEvent函数将
       根据这些属性把对话框事件路由至相应的处理函数。
       此函数仅由QuickTest_HandleEvent调用。

==============================================================================*/
void QuickTest_UpdateActiveDialogInfo
(
    CQuickTest *pMe,
    uint16  wParam,
    uint32 dwParam
);
/*==============================================================================
函数:
       QuickTest_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向QuickTest Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:


==============================================================================*/
void QuickTest_ShowDialog(CQuickTest *pMe,uint16  dlgResId);

/*==============================================================================
函数:
       QuickTest_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向QuickTest Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean QuickTest_RouteDialogEvent(CQuickTest *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

/*----------------------状态机相关函数声明---------------------*/
/*==============================================================================
函数:
       QuickTest_ProcessState
说明:
       QuickTest Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向QuickTest Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction QuickTest_ProcessState(CQuickTest *pMe);

#endif//IQUICKTESTPRIV_H

