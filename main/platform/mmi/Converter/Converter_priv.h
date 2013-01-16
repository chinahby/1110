#ifndef CONVERTERPRIV_H
#define CONVERTERPRIV_H

/*==============================================================================
// 文件：
//        Converter_priv.h
//        2008-10-28 Gemsea草拟版本(Draft Version)
//        版权所有(c) 2008 Anylook
//        
// 文件说明：
//        
// 作者：Gemsea
// 创建日期：2008-10-28
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人     修改内容、位置及原因
-----------      ----------     -----------------------------------------------
08-10-28                   Initial version
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#ifndef AEE_SIMULATOR
#include "customer.h"   //Handset Feature Definitions
#else
#include "BREWSimFeatures.h"   //Simulator Feature Definitions
#endif
#if !defined(BREW_STATIC_APP)
   #include "converter.bid"
#else
   #include "OEMClassIds.h"  // Applet Ids
   #ifndef AEECLSID_CONVERTER
      #error AEECLSID_CONVERTER must be defined
   #endif
#endif
#include "appscommonimages.brh"
#include "Converter.brh"
#include "AEEShell.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "Converter.h"
#include "OEMCFGI.h"
#include "Converter.h"
#include "AEEMenu.h"
#include "AEEConfig.h"
#include "AppComFunc.h"
#include "OEMFeatures.h"
#include "Appscommon.h"
#include "AEE.h"
#ifndef WIN32
#include "ERR.h"
#else
#include "oemhelperfunctype.h"
#endif//WIN32
#include "AEE_OEM.h"
#include "AEEText.h"
#include "AEEAnnunciator.h"

#define CONVERTER_CURRENCY_NUMBER (16) //汇率种类个数
/*==============================================================================
                                 类型定义
==============================================================================*/
/*----------------------对话框相关数据类型声明---------------------*/
// 对话框关闭时返回值列表
typedef enum DLGRetValue
{
   // 初始值，表明需要创建对话框
   DLGRET_CREATE,

   // 对话框IDD_CONVERTERMAIN返回特定值
   DLGRET_CURRENCY,
   DLGRET_LENGTH,
   DLGRET_WEIGHT,
   DLGRET_VOLUME,
   DLGRET_AREA,
   DLGRET_TEMPERATURE,
   //对话框IDD_CHANGECURRENCY返回特定值
   DLGRET_CHANGECURRENCY,
   DLGRET_ERR,
   DLGRET_MSGBOX_OK,
   DLGRET_MSGBOX_CANCEL,
   // 对话框关闭时可返回的通用值
   DLGRET_OK,
   DLGRET_CANCELED,
   
} DLGRetValue;

/*----------------------状态机相关数据类型声明---------------------*/
// Cnverter Applet 状态机状态：
typedef enum _ConverterState
{
    CONVERTER_NONE,
    CONVERTER_INIT,
    CONVERTER_MAIN,
    CONVERTER_CONVERT,
    CONVERTER_CHANGECURRENCY,
    CONVERTER_EXIT
} ConverterState;

// 状态处理函数返回给状态处理主函数的值类型
typedef enum NextFSMAction
{
   NFSMACTION_WAIT,
   NFSMACTION_CONTINUE
} NextFSMAction;

// 保存模块信息和模块引用计数的结构
typedef struct ConverterMod
{
    DECLARE_VTBL(IModule)
    uint32   m_nRefs;
} ConverterMod;

typedef struct _ConverterCurrencyCfg
{
    double customCurrency[CONVERTER_CURRENCY_NUMBER];
} ConverterCurrencyCfg;

typedef enum
{
    CONVERTER_MODE_NONE,
    CONVERTER_MODE_CURRENCY,
    CONVERTER_MODE_LENGTH,
    CONVERTER_MODE_WEIGHT,
    CONVERTER_MODE_VOLUME,
    CONVERTER_MODE_AREA,
    CONVERTER_MODE_TEMPERATURE
} ConverterMode;

// Converter Applet对象结构体：
typedef struct _CConverter
{
    DECLARE_VTBL(IConverter)
    AEECLSID         clsID;
    uint32              m_nRefs; // 对象引用计数器
    IModule            *m_pModule;
    IDisplay            *m_pDisplay;
    IShell               *m_pShell;
    IStatic             *m_pStatic;
    IConfig             *m_pConfig;
    AEERect           m_rc;
    // 当前活动对话框IDialog接口指针
    IDialog             *m_pDialog;
    // 当前活动对话框ID
    uint16              m_pActiveDlgID;
    // 活动对话框关闭时的返回结果
    DLGRetValue      m_eDlgRet;
    // 是否不改写对话框返回结果，一般情况需要改写(FALSE)。
    boolean            m_bNotOverwriteDlgRet;
    // Applet 前一状态
    ConverterState  m_ePreState;
    // Applet 当前状态
    ConverterState  m_eCurState;
    // Applet是否处于挂起状态
    boolean            m_bSuspending;
    // Applet是否准备好可以处理按键和命令事件。改变量主要用于快速按键引起的问题
    boolean            m_bAppIsReady;
    
    IMenuCtl           *pUnitMenu1; //源单位指针
    IMenuCtl           *pUnitMenu2; //目标单位指针
    ITextCtl            *pNumber1;   //源数值显示指针
    ITextCtl            *pNumber2;   //目标数值显示指针
    IMenuCtl           *title;
    IMenuCtl           *currency;
    ITextCtl            *coeff1;
    ITextCtl            *coeff2;
    #if defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_W515V3)|| defined(FEATURE_VERSION_W516)|| defined(FEATURE_VERSION_W027)||defined(FEATURE_VERSION_C316)
    #else
    ITextCtl            *coeff3;
    #endif
    uint16               basiccoefficient;
    uint16               m_wMsgResID;
    ConverterCurrencyCfg m_CurrencyCFG;
    int                    dyMenu;        //Menu高度
    uint16               m_nCtlID;      //控件ID
    uint16               m_selectedItem; //菜单记录当前选择的项
    ConverterMode   m_converterMode; //切换单位换算模式
    boolean             b_overflow; //结果溢出
    boolean            PENUPbRedraw;
    boolean            PENUPbCalc;
    IAnnunciator *m_pIAnn;
    #ifdef FEATURE_LCD_TOUCH_ENABLE
	AEERect      pL_rect1;
	AEERect      pL_rect2;
    AEERect      pR_rect1;
	AEERect      pR_rect2;
	AEERect      pL_rect3;
	AEERect      pR_rect3;
    #endif
} CConverter;

/*==============================================================================
                                 宏定义和常数
==============================================================================*/
// 根据 BREW 3 的需要，重定义资源文件宏
#if defined( AEE_SIMULATOR)
    #define  AEE_CONVERTER_LANGFILE "fs:/mod/converter/en/converter.bar"
#else
    #define  AEE_CONVERTER_LANGFILE (AEE_RES_LANGDIR CONVERTER_RES_FILE)
#endif

// 为防止用户快速按键而添加事件EVT_APPISREADY，事件由定时器
// Converter_APPIsReadyTimer发出

#define APPISREADY_TIMER       200            
#define EVT_APPISREADY   (EVT_USER+1)         
#define EVT_USER_CLOSEAPP (EVT_USER+13)

// 状态移动宏：先将前一状态用当前状态更新，再将当前状态设为nextState
#define MOVE_TO_STATE(nextState) {            \
            ConverterState tpState;            \
            tpState = nextState;          \
            pMe->m_ePreState = pMe->m_eCurState; \
            pMe->m_eCurState = tpState;        \
        }

// 关闭对话框宏
#define CLOSE_DIALOG(DlgRet)  {    \
                                     pMe->m_eDlgRet = DlgRet; \
                                     (void) ISHELL_EndDialog(pMe->m_pShell);  \
                                 }

#define CONVERTER_CFG_CURRENCY   0x1000

#define FRAME_SIZE                 (1)
#define IDC_UNIT_MENU1          (2)
#define IDC_NUMBER1               (3)
#define IDC_UNIT_MENU2          (4)
#define IDC_NUMBER2               (5)

#define MAX_INPUT_NUMBER     (15)
#define MAX_DECIMAL_NUMBER (6)
#define MAX_INT_NUMBER         (9)
#define CONVERTER_PROMPTMSG_TIMER (3000)
#if defined(FEATURE_DISP_160X128)
#define ARROW_WIDTH             (8)
#define ARROW_HEIGHT            (8)
#elif defined(FEATURE_DISP_220X176)
#define ARROW_WIDTH             (11)
#define ARROW_HEIGHT            (8)
#else
#define ARROW_WIDTH             (8)
#define ARROW_HEIGHT            (8)
#endif
#define CHARACTOR_PIC_WIDTH (32)
#define RGB_ORANGE                (MAKE_RGB(0xFF,0x90,0x30))


/*==============================================================================
                                 全局数据
==============================================================================*/

/*==============================================================================
                                 本地（静态）数据
==============================================================================*/
//单位换算系数
static double coefficient[] = {
    //CURRENCY，以欧元为换算分子
    1.0,             // 欧元
    106.33,        // 日元
    8.6501,        // 人民币
    1.2728,        // 美元
    40.6978,        // 台湾元
    0.8258,        // 英镑
    1272.8,        // 缅甸币
    9.2973,        // 瑞典克朗
    7.4446,        // 丹麦克朗
    7.8861,        // 挪威克朗
    1.7115,        // 新加坡元
    59.4207,        // 印度卢比
    4.6747,        // 阿联酋迪拉姆
    1492.49,        // 韩元
    4.8163,        // 以色列新谢克尔
    11442.5,		//印尼币
    //LENGTH，以米为换算分子
    0.001,
    39.37,
    3.2808,
    1.0936,
    0.0006214,
    1000.0,
    100.0,
    1.0,
    //WEIGHT，以千克为换算分子
    35.273,
    1000.0,
    2.20459,
    1.0,
    0.001,
    15432.0,
    //VOLUME，以升为换算分子
    1000.0,
    0.001,
    1.0,
    61.027,
    0.0353165,
    0.0013082,
    0.264186,
    //AREA，以平方米为换算分子
    10000.0,
    1.0,
    0.01,
    1550,
    10.764,
    0.0002471,
    //TEMPERATURE，以摄氏度为换算分母
    1.0,
    1.8
    };

/*==============================================================================
                                 函数声明
==============================================================================*/
/*----------------------对话框相关函数声明---------------------*/
/*==============================================================================
函数:
       Converter_ShowMsgBox
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向Converter Applet对象结构的指针,该结构包含小程序的特定信息.
       wTextResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:
       

==============================================================================*/

void Converter_ShowMsgBox(CConverter *pMe, uint16 wTextResId);

/*==============================================================================
函数:
       Converter_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向Converter Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:
       

==============================================================================*/
void Converter_ShowDialog(CConverter *pMe,uint16  dlgResId);

/*==============================================================================
函数:
       Converter_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向Converter Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean Converter_RouteDialogEvent(CConverter *pMe,
    AEEEvent    eCode,
    uint16  wParam,
    uint32 dwParam
);

/*----------------------状态机相关函数声明---------------------*/
/*==============================================================================
函数:
       Converter_ProcessState
说明:
       Converter Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向Converter Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction Converter_ProcessState(CConverter *pMe);

#endif
//CONVERTERPRIV_H

