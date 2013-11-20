#ifndef CONTAPP_PRIV_H
#define CONTAPP_PRIV_H

/*==============================================================================
// 文件：
//        ContApp_priv.h
//        2004-06-08 jyh草拟版本(Draft Version)
//        版权所有(c) 2004 Anylook
//        
// 文件说明：
//        
// 作者：2004-06-08
// 创建日期：jyh
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时    间      修   改   人     修改内容、位置及原因
--------      -----------     -----------------------------------------------
04-06-08      jyh             初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#ifndef AEE_SIMULATOR
#include "OEMFeatures.h"   //Handset Feature Definitions
#else
#include "BREWSimFeatures.h"   //Simulator Feature Definitions
#endif

#include "AEE.h"
#include "AEEShell.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "AEEConfig.h"
#include "AEEFile.h"
#include "AEEText.h"
#include "AEERinger.h"
#include "AEEDate.h"
#include "AEE_OEM.h"

#if defined(AEE_STATIC)
// For BREW3.1
//#include "AEE_static.h"
#include "OEMClassIDs.h"
//#include "OEMMacros.h"
//#include "uiutils.h"
#include "AppComFunc.h"
#ifndef WIN32
#include "err.h"
#include "ui.h"
#else
#include "oemhelperfunctype.h"
#endif//WIN32

#include "AEEVector.h"
#endif

#include "ContApp.h"
#include "contactapp.brh"
#include "CallApp.h"
//#include "SMSApp.h"
#include "Appscommon.h"

#include "OEMCFGI.h"
#ifdef FEATURE_ICM
#include "AEECM.h"
#else
#include "AEETelephone.h"
#include "AEETelDef.h"
#endif
#include "AEEAddrBookExt.h"


// 图片资源文件
#include "appscommonimages.brh"

#include "AEEAnnunciator.h"

#include "AEERUIM.h"
#ifdef FEATURE_OEMOMH
#include "AEEConfig.h"
#include "AEECardSession.h"
#include "AEEDB.h"
#include "AEECardSessionNotifier.h"
#include "AEEDownload.h"
#include "OEMCardSessionNotifier.h"
#include "AEECARDSESSION.BID"
#endif

extern sSelectFieldInfo  curSelectFieldInfo;
extern sSelectFieldListNode* pSelectFieldListRoot;
extern sSelectFieldListNode* pCurSelectFieldNode;

#define FEATURE_SMARTSEARCH

#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch 
#define CONTAPP_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )
#endif

/*==============================================================================
                                 常量定义
==============================================================================*/
#ifdef FEATURE_SUPPORT_VCARD
#define DEFAULT_VCARD_NAME  "fs:/address/vCard.vcf"
#define DEFAULT_VCARD_SENDNAME ("F"DEFAULT_VCARD_NAME)
#define VCARD_PATH "fs:/address/\0"
#define VCARD_EXTEND ".vcf"
#define VCARD_PATH_LEN   12
#endif

#define MAX_LEN_WMS_BUF                 (1024)
#define MAX_LEN_COMMBUF                 (256)
#define MAX_LEN_MSGBOX                  (256)
#define MAX_LEN_PASSWORD                (6)
#define MAX_LEN_TITLE                   (20)
#define MAX_LEN_POSITION                (3)

#define TIME_ONE_SEC                    (1000)
#define TIMEOUT_MS_MSGBOX               (3*TIME_ONE_SEC)
#define TIMEOUT_MS_QUICK                (500)
#define TIMEOUT_SMARTKEY                (1000)

#if defined(FEATURE_DISP_160X128)
#define MAX_NUM_MENUPOP                 (5) //(6)
#elif defined(FEATURE_DISP_128X160)
#define MAX_NUM_MENUPOP                 (7) //(6)
#elif defined(FEATURE_DISP_128X128)
#define MAX_NUM_MENUPOP                 (5) //(6)
#elif defined(FEATURE_DISP_176X220)
#define MAX_NUM_MENUPOP                 (7) //(6)
#elif defined(FEATURE_DISP_220X176)
#ifdef FEATURE_VERSION_EC99
#define MAX_NUM_MENUPOP                 (3)
#else
#define MAX_NUM_MENUPOP                 (7) //(6)
#endif
#elif defined(FEATURE_DISP_240X320)
#define MAX_NUM_MENUPOP                 (9) //(6)
#elif defined(FEATURE_DISP_320X240)
#define MAX_NUM_MENUPOP                 (7) //(6)
#else
#define MAX_NUM_MENUPOP                 (5) //(6)
#endif
#define MAX_NUM_NAME                    (1)
#define MAX_NUM_NUM                     (5)
#define MAX_NUM_EMAIL                   (2)
#define MAX_NUM_URL                     (2)
#define MAX_NUM_BIRTHDAY                (1)
#define MAX_NUM_NOTES                   (1)
#define MAX_NUM_OTHER                   (1)
#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)
#define MAX_INPUT_NAME_CARD             (19)
#define MAX_INPUT_NAME_CN               (19)
#define MAX_INPUT_NAME_EN               (19)
#elif defined(FEATURE_VERSION_C11)
#define MAX_INPUT_NAME_CARD             (28)
#define MAX_INPUT_NAME_CN               (28)
#define MAX_INPUT_NAME_EN               (28)
#elif defined (FEATURE_VERSION_C337)  || defined(FEATURE_VERSION_IC241A_MMX)
#define MAX_INPUT_NAME_CARD             (20)
#define MAX_INPUT_NAME_CN               (20)
#define MAX_INPUT_NAME_EN               (20)
#else
#define MAX_INPUT_NAME_CARD             (14)
#define MAX_INPUT_NAME_CN               (16)
#define MAX_INPUT_NAME_EN               (16)
#endif

#define MAX_INPUT_NAME_TH               (32)
#define MAX_INPUT_NUM_CARD              (20)
#define MAX_INPUT_NUM                   (32)
#define MAX_INPUT_EMAIL                 (64)
#define MAX_INPUT_URL                   (128)
#define MAX_INPUT_NOTES                 (128)
#define MAX_INPUT_ADDRESS               (128)

#ifdef FEATURE_VERSION_1110W516
#define MAX_PHONEBOOK_CAPACITY          (1000)
#elif defined(FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
#define MAX_PHONEBOOK_CAPACITY          (1000)
#elif defined(FEATURE_VERSION_EC99)
#define MAX_PHONEBOOK_CAPACITY          (150)  /*(300)*/
#elif defined(FEATURE_VERSION_K212_HUALU)
#define MAX_PHONEBOOK_CAPACITY          (200)  /*(300)*/
#else
#define MAX_PHONEBOOK_CAPACITY          (500)
#endif

#define MAX_RUIMBOOK_CAPACITY           (250)

#define MAX_PHONE_FIELD    (0x0B)
#define MAX_RUIM_FIELD    (0x02)


// 配置文件名
#define FILE_CONTAPP_CFG                (AEE_ADDRBOOK_DIR"\\contapp.cfg")

// 配置文件版本
#define CFG_VERSION                     (0x0001)

// View type 0 is default
#define CONTCFG_VIEWTYPE_ALL            (0)
#define CONTCFG_VIEWTYPE_PHONE          (1)
#ifdef FEATURE_RUIM_PHONEBOOK
#define CONTCFG_VIEWTYPE_RUIM           (2)
#endif /* FEATURE_RUIM_PHONEBOOK*/
#define CONTCFG_VIEWTYPE_NONE           (3)

// Save type 0 is default
#define CONTCFG_SAVETYPE_SELECT         (0)//保存时选择
#define CONTCFG_SAVETYPE_PHONE          (1)
#define CONTCFG_SAVETYPE_RUIM           (2)

// Default password
#define DEFAULT_PASSWORD                ('\0')

// Onedial default, RUIM 卡的起始记录是1，因此这里使用0
#define ONEDIAL_NULL                    ('\0')

// 空记录号
#define CONTAPP_RECID_NULL              (0xFFFF)

// 空菜单选择号
#define MENU_SELECT_NULL                (0xFFFF)

// 空字段索引
#define CONTAPP_FLDID_NULL              (0xFFFF)

// RUIM卡记录起始号
#define CONTAPP_RUIM_START_IDX          (0x4000)

#define AEE_ADDR_CAT_MAX_CHAR (0x20)

#define CONTAPP_NUMBERFLDS              (4)

// 字段优先级
#define PRI_NAME                        (0x01)
#define PRI_NUM_PREF                    (0x02)
#define PRI_NUM_HOME                    (0x03)
#define PRI_NUM_WORK                    (0x04)
#define PRI_NUM_FAX                     (0x05)
#define PRI_NUM_CELL                    (0x06)
#define PRI_NUM_GENERIC                 (0x07)
#define PRI_EMAIL                       (0x08)
#define PRI_URL                         (0x09)
#define PRI_BIRTHDAY                    (0x0A)
#define PRI_NOTES                       (0x0B)
#define PRI_PHOTO                       (0x0C)
#define PRI_RING                        (0x0D)
#define PRI_DEFAULT                     (0xFF)
#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)
#define AVK_SEND_TWO     2
#elif defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_W516) || defined(FEATURE_VERSION_W027)
#define AVK_SEND_TWO     2
#else
#define AVK_SEND_TWO     1
#endif

// 字段掩码
#define AEE_ADDR_KEY_LEFT (0x1)
#define AEE_ADDR_KEY_RIGHT (0x2)
#define AEE_ADDR_KEY_UP (0x3)
#define AEE_ADDR_KEY_DOWN (0x4)


// Error value
#define EEXIST                          (ERROR_USER+1)
#define EMAXRECORD                      (ERROR_USER+2)
#define EMAXFIELD                       (ERROR_USER+3)
#define ENORECORD                       (ERROR_USER+4)

// number fields when create record
#define NUM_CREATE_FLDS                 (2)
#define IDX_NAME_FLD                    (0)
#define IDX_NUM_RUIM_FLD                (1)
#define STEP_COPY                       (10)

// Speed dial Max len
#define MAX_LEN_SPEEDDIAL               (6)
#define RINGID_USESYSTEM                (IDI_RING_MENU_USESYSTEM)

// 肖像照片(作小屏墙纸及来电个人显示)最多张数         
#define PHOTO_MAX_FILES                 (30)

// 肖像照片目录
#define MP_CONTPIC_DIR                  (AEE_SHARED_DIR"/cont")

// 肖像图片扩展名
#define SUFFIX_PIC                      (".bmp")

#define CONTAPP_APPREADY_MS             (400)

#if defined(FEATURE_DISP_160X128)
#define SEARCH_INPUTBOX_GAP             (6)
#define SEARCH_IMEICON_WIDTH            (28)
#elif defined(FEATURE_DISP_220X176)
#define SEARCH_INPUTBOX_GAP             (6)
#define SEARCH_IMEICON_WIDTH            (40)
#else
#define SEARCH_INPUTBOX_GAP             (6)
#define SEARCH_IMEICON_WIDTH            (28)

#endif
/*==============================================================================
                                 宏定义
==============================================================================*/
// 画界面底部提示条宏定义
#define CONTAPP_DRAW_BOTTOMBAR(x)                           \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = x;                         \
    DrawBottomBar(pMe->m_pDisplay, &BarParam);      \
}
// menu auto scroll
#define MENU_AUTO_SCROLL(p, e, w)   //MenuAutoScroll(p, e, w)

// Select menu common functions
#define MENU_INIT_SELICONS(p)       InitMenuIcons(p);
#define MENU_SET_SELICON(p, i, b)   SetMenuIcon(p, i, b)

#define MENU_SET_COMICON(p)         

// Call back macros
#define CONTAPP_RESUMECB(p)         ISHELL_Resume(pMe->m_pShell, p)

#define FREE_AEEAddrField(cf)           \
        if ((cf).pBuffer != NULL)       \
        {                               \
            FREE((cf).pBuffer);         \
            (cf).pBuffer = NULL;        \
        }

// Current list menu items is zero
#define IS_ZERO_REC()    (0 == IVector_Size(pMe->m_pAddList))

// Indicate whether it is RUIM record
#define IS_RUIM_REC(n)   (n >= CONTAPP_RUIM_START_IDX)

// Indicate whether it is number field
#define IS_NUM_FIELD(n)  (n>=AEE_ADDRFIELD_PHONE_WORK       \
                          && n<= AEE_ADDRFIELD_PHONE_OTHER)

// Indicate whether it is addnew phone record
#define IS_ADDNEW_PHONE_REC(n,m)  (n== STATE_ADDNEW_INPUT       \
                          && m== CONTCFG_SAVETYPE_PHONE)

// Indicate whether it is not name generic fld
#define ISNT_NAME_GENERIC_RING_FLD()  ((pMe->m_nFldInputID != AEE_ADDRFIELD_NAME)       \
                          &&( pMe->m_nFldInputID != AEE_ADDRFIELD_RINGTONE))

// Indicate whether it is not name generic fld
#define IS_VALID_RINGTONE()  (pMe->m_nRingToneID != -1) 
                          
// Indicate whether it is not name generic fld
#define IS_NO_PHONENUMBER()  ((pMe->m_pAddNewMobile == NULL)	\
                          &&( pMe->m_pAddNewHome == NULL)              \
                          &&( pMe->m_pAddNewOffice == NULL)              \
                          &&(pMe->m_pAddNewFax == NULL))
	                         
// Is it the max capacity?
#define IS_MAX_REC(p)    (IADDRBOOK_GetNumRecs(p) >= IADDRBOOK_GetCapacity(p))

// Is it the max fields ?
#define IS_MAX_FLDS(p, f)         \
        (CContApp_GetCurrFldNum(p, f) >= CContApp_GetFldMaxNum(f))

#define FORCE_COMPLETE_COPY()  (pMe->m_bForceComplete = TRUE)
#define FORCE_COMPLETE_DELETE()  (pMe->m_bForceComplete = TRUE)

// Options menu select
#define PUSH_OPTSMENU_SEL(w)   (w = (uint16)pMe->m_wSelectOpts)
#define POP_OPTSMENU_SEL(w)    (pMe->m_wSelectOpts = (uint16)w)

// list menu select
#define PUSH_LISTMENU_SEL(w)   (w = (uint16)pMe->m_wSelectCont)
#define POP_LISTMENU_SEL(w)    (pMe->m_wSelectCont = (uint16)w)

// list menu build index
#define PUSH_LISTMENU_IDX(w)   (w = (uint16)pMe->m_nCurrIdx)
#define POP_LISTMENU_IDX(w)    (pMe->m_nCurrIdx = (uint16)w)


//Add By zzg 2011_12_15
#define CONT_PAUSE_AECHAR              ((AECHAR)'P')
#define CONT_PLUS_AECHAR               ((AECHAR)'+')
//Add End

/*==============================================================================
                                 类型定义
==============================================================================*/

/*--------------------------配置信息项目---------------------------*/
// 配置项 CFG = Config
typedef enum _ContAppCFG
{
    CONTCFG_LOCKED,
    CONTCFG_VIEWTYPE,
    CONTCFG_SAVETYPE,
    CONTCFG_PASSWORD,
    CONTCFG_SMARTDIAL,
    CONTCFG_ONEDIAL1,
    CONTCFG_ONEDIAL2,
    CONTCFG_ONEDIAL3,
    CONTCFG_ONEDIAL4,
    CONTCFG_ONEDIAL5,
    CONTCFG_ONEDIAL6,
    CONTCFG_ONEDIAL7,
    CONTCFG_ONEDIAL8,
    CONTCFG_ONEDIAL9,
    CONTCFG_GROUP1,
    CONTCFG_GROUP2,
    CONTCFG_GROUP3,
    CONTCFG_GROUP4,
    CONTCFG_GROUP5,
    CONTCFG_GROUP6, 
    CONTCFG_GROUP7, 
#ifdef FEATURE_LANG_CHINESE
    CONTTCFG_QUICKSEARCH_INPUT_MODE,
#endif
    
    CONTCFG_LAST
}ContAppCFG;

typedef enum _ContAppCopyMoveType
{
    COPYALLTOUIM,
    COPYALLTOPHONE,
    MOVEALLTOUIM,
    MOVEALLTOPHONE,
    COPYMULTIPE,
    MOVEMULTIPE,
    COPYMULTIPE_TOUIM,
    COPYMULTIPE_TOPHONE,
    MOVEMULTIPE_TOPHONE,
    MOVEMULTIPE_TOUIM,
    SINGLECOPY,
    SINGLEMOVE
}ContAppCopyMoveType;

typedef enum _InputMode
{
    LOCAL_NUMBER_INPUT,  //在当前位置编辑
    OPT_TEXT_INPUT,  //弹出另外的text控件编辑
    EDIT_MENU_MODE,  //菜单模式
    EDIT_GROUP        
}Input_mode;

typedef enum _AddorEdit
{
    ADDOREDIT_ADD,  //在当前位置编辑
    ADDOREDIT_EDIT  //弹出另外的text控件编辑
}AddorEdit;

typedef enum _ContAppDeleteType
{
    DELETE_ALL,
    DELETE_PHONE,
    DELETE_UIMCARD,
    DELETE_MULTIPE
}ContAppDeleteType;

typedef enum _ContAppCopyErrType
{
    FIRST_VALUE, //didn't use
    PHONE_FULL,
    UIMCARD_FULL,
    OTHER_ERR
}ContAppCopyErrType;

typedef enum _ContAppAddFldType
{
    ADDFLDTOPHONE,
    ADDFLDTORUIMCARD
}ContAppAddFldType;

typedef enum _ContAppListFrom
{
    LISTFROMNONE,// list have not started
    LISTFROMNOMAL,//list startes from nomal 
    LISTFROMMAINMENU, // list start from mainmenu
    LISTFROMSEARCH,    // list start from search
    LISTFROMGROUP,       // list start from group
    LISTFROMONEDIAL
}ContAppListFrom;

typedef struct _ContAppOneDialFldPair
{
    ContAppCFG  oneDialIndx;
    int16       fldindx;
    boolean     bMask;
}ContAppOneDialFldPair;


typedef enum _ContAppFieldOptType
{
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
    FIELDSENDSMS,
    FIELDSENDCONTACT,
#endif    
    FIELDCALL,
    FIELDIPCALL
}ContAppFiledOptType;

typedef enum _ContAppSmartStateType
{
    SMART_STATE_IDD_LIST,
    SMART_STATE_IDD_SELECT,
    SMART_STATE_IDD_COPY,
    SMART_STATE_IDD_DELETE,
    SMART_STATE_IDD_SELECT_RECORD
}ContAppSmartStateType;

    
// 配置项Cache
typedef struct _ContAppCFGCache
{
    AECHAR      wPassWord[MAX_LEN_PASSWORD+2];
    boolean     bLocked;
    byte        nViewType;
    byte        nSaveType;
    boolean     bSmartDial;
    AECHAR      wOneDial1[MAX_INPUT_NUM + 1];
    AECHAR      wOneDial2[MAX_INPUT_NUM + 1];
    AECHAR      wOneDial3[MAX_INPUT_NUM + 1];
    AECHAR      wOneDial4[MAX_INPUT_NUM + 1];
    AECHAR      wOneDial5[MAX_INPUT_NUM + 1];
    AECHAR      wOneDial6[MAX_INPUT_NUM + 1];
    AECHAR      wOneDial7[MAX_INPUT_NUM + 1];
    AECHAR      wOneDial8[MAX_INPUT_NUM + 1];
    AECHAR      wOneDial9[MAX_INPUT_NUM + 1];
    AECHAR      wGroup1[MAX_INPUT_NAME_EN + 1];
    AECHAR      wGroup2[MAX_INPUT_NAME_EN + 1];
    AECHAR      wGroup3[MAX_INPUT_NAME_EN + 1];
    AECHAR      wGroup4[MAX_INPUT_NAME_EN + 1];
    AECHAR      wGroup5[MAX_INPUT_NAME_EN + 1];
    AECHAR      wGroup6[MAX_INPUT_NAME_EN + 1];    
    AECHAR      wGroup7[MAX_INPUT_NAME_EN + 1];
#ifdef FEATURE_LANG_CHINESE
    byte inputMode;
#endif
}ContAppCFGCache;

/*----------------------对话框相关数据类型声明---------------------*/
// 对话框关闭时返回值列表
typedef enum _DLGRetValue
{
    // 初始值，表明需要创建对话框
    DLGRET_CREATE,
    DLGRET_MSGBOX_OK,
    DLGRET_YES,
    DLGRET_NO,
    DLGRET_ERR,
    DLGRET_VIEW,
    DLGRET_FIND,
    DLGRET_GROUPVIEW,
    DLGRET_DELETE,
    DLGRET_DELETE_SELECT,
    DLGRET_PHONE_MEMORY, //WUQUQN.TANG 20081014 ADD
    DLGRET_UIM_CARD, //WUQUQN.TANG 20081014 ADD
    DLGRET_DELETE_ALL, //WUQUQN.TANG 20081014 ADD
    DLGRET_EDIT,
    DLGRET_POPNUMFLD,
    DLGRET_SELECT,
    DLGRET_SELECT_FLD,
    DLGRET_MARKCHG,
#ifdef FEATURE_RUIM_PHONEBOOK
    DLGRET_COPYMOVE,
    DLGRET_ALLCOPYMOVE,
    DLGRET_COPY,
    DLGRET_CONTACTS_MANAGEMENT,
    DLGRET_COPYORMOVE,
    DLGRET_CONTACT_EDIT,
#endif /* FEATURE_RUIM_PHONEBOOK */
    DLGRET_ADDNEW,
    DLGRET_CHECKCAPACITY,
    DLGRET_SETTING,
    DLGRET_HELP,
    DLGRET_SETTYPE,
    DLGRET_INPUT_NULL,
    DLGRET_NUM_UNAVAILD,
    DLGRET_SAME_REC,
    DLGRET_SELECTCAT,
    DLGRET_SELBIRTHDAY,
    DLGRET_SETICON,
//    DLGRET_SETRING,
#ifdef FEATURE_RUIM_PHONEBOOK
    DLGRET_MOVE,
    DLGRET_TORUIM,
    DLGRET_TOPHONE,
    DLGRET_SAVETO,
    DLGRET_VIEWTYPE,
#endif /* FEATURE_RUIM_PHONEBOOK */
    DLGRET_SETLOCK,
    DLGRET_ONEDIAL,
    DLGRET_CALL,
    DLGRET_SET,
    DLGRET_CLR,
    DLGRET_SENDSMS,
    DLGRET_IPCALL,
    DLGRET_SENDDIRECTORY,
    DLGRET_SEARCHOTHERDIRECTORY,
    DLGRET_DIRECTORTLIST,
    DLGRET_SEARCH,
    DLGRET_SEARCH_NAME,
    DLGRET_SEARCH_GROUP,
    DLGRET_SELECT_SINGLE_NUMBER,
    // 对话框关闭时可返回的通用值
    DLGRET_OK,
    DLGRET_CANCELED,
    DLGRET_DETAIL,
    DLGRET_EMAIL_UNAVAILD,
    DLGRET_FULL,
    DLGRET_SENDLISTFULL,
    DLGRET_NO_SELECTION,
    DLGRET_NUMBER_TOOLONG,
#if defined(FEATURE_SUPPORT_BT_APP) && defined(FEATURE_SUPPORT_VCARD)
//#if defined(FEATURE_SUPPORT_VCARD)		//Modify by zzg 2011_10_25
    DLGRET_SEND_BY_BT,
#endif
#ifdef FEATURE_GIVEUP_EDIT
    DLGRET_GIVEUP_EDIT,
#endif
#ifdef FEATURE_OEMOMH 
    DLGRET_EMERGENCY_CALL,
#endif
} DLGRetValue;

/*----------------------状态机相关数据类型声明---------------------*/
// ContApp Applet 状态机状态：
typedef enum _FSMState
{
    STATE_NONE,			//0x0
    STATE_INIT,			//0x1
    STATE_LOAD,			//0x2
    STATE_LOCKED,		//0x3
    STATE_MAINLIST,		//0x4
    STATE_MAINMENU ,      //0x5
    STATE_OPTS,			//0x6
    STATE_VIEW,			//0x7
    STATE_FIND,			//0x8
    STATE_FIND_LIST,		//0x9
    STATE_FIND_OPTS,	//0x10
    STATE_GROUPVIEW,	//0x11
    STATE_GROUPVIEW_LIST,	//0x12
    STATE_GROUPVIEW_OPTS,	//0x13
    STATE_ADDNEW,			//0x14
    STATE_ADDNEW_INPUT,    //0x15
    STATE_SELECTGROUP,	//0x16
#ifdef FEATURE_RUIM_PHONEBOOK
    STATE_SELECTPOS,		//0x17
#endif /* FEATURE_RUIM_PHONEBOOK */
    STATE_EDIT,				//0x18
    STATE_EDIT_INPUT,		//0x19
    STATE_EDIT_SELECTGROUP,	//0x20
//    STATE_EDIT_RING,	
    STATE_DELETE,	//0x21
#ifdef FEATURE_RUIM_PHONEBOOK
    STATE_COPYMOVE,	//0x22
    STATE_COPYMOVEALL,	//0x23
    STATE_COPY,	//0x24
    STATE_SETTING,	//0x25
    STATE_SAVETO,	//0x26
    STATE_VIEWTYPE,	//0x27
#endif /* FEATURE_RUIM_PHONEBOOK*/
    STATE_CAPACITY,		//0x28
    STATE_ONEDIAL,	//0x29
    STATE_ONEDIAL_OPT,//0x30
    STATE_ONEDIAL_SET,//0x31
    STATE_ONEDIAL_SET_NUMFLDSEL,//0x32
    STATE_SAVEFLD,//0x33
    STATE_SAVEFLD_LIST,//0x34
    STATE_SELECT,//0x35
    STATE_SELECTFLD_VIEW,//0x36
    STATE_SELECT_RETURN,//0x37
    STATE_FINDNUM,//0x38
    STATE_FINDNUM_OPTS, //0x39
    STATE_SMARTCALL, //0x40
    STATE_SMARTCALL_VIEW,	//0x41
    STATE_SEARCH,//0x42
    STATE_SEARCH_NAME,//0x43
    STATE_DETAIL,//0x44
    STATE_SUCCESS, //0x45
    STATE_MSG, //0x46
    STATE_ERROR, //0x47
    STATE_EXIT,  //0x48
    STATE_COPYING,
    STATE_GROUPOPT,
    STATE_GROUPOPT_EDIT,
    STATE_MANAGEMENT,
    STATE_DELETE_SELECT,
    STATE_POPNUMFLD,
    STATE_SELECTOPT,
    STATE_NUMFLDVIEW,
    STATE_DELETING,
    STATE_SELECT_RECORD,
    STATE_DETAIL_MULTI,
#ifdef FEATURE_OEMOMH 
    STATE_EMERGENCY_CALL
#endif
} FSMState;

// 状态处理函数返回给状态处理主函数的值类型
typedef enum _NextFSMAction
{
    NFSMACTION_WAIT,
    NFSMACTION_CONTINUE
} NextFSMAction;


// ContApp Applet对象结构体：
typedef struct _CContApp
{
    DECLARE_VTBL(IContApp)
    uint32              m_nRefs;          // 对象引用计数器
    IModule            *m_pModule;
    IDisplay           *m_pDisplay;
    IShell             *m_pShell;
    AEERect             m_rc;
    int                 m_nLineHeight;    // 当前的行高
    
    // 当前活动对话框IDialog接口指针
    IDialog            *m_pActiveDlg;
    
    // 当前活动对话框ID
    uint16              m_pActiveDlgID;
    
    // 活动对话框关闭时的返回结果
    DLGRetValue         m_eDlgRet;
    
    // 是否不改写对话框返回结果，一般情况需要改写(FALSE)。
    boolean             m_bNotOverwriteDlgRet;
    
    // Applet 前一状态
    FSMState            m_ePreState;
    
    // Applet 当前状态
    FSMState            m_eCurState;
    
    // Applet 返回状态
    FSMState            m_eOptsRetState;
    
    // Applet 返回状态
    FSMState            m_eSuccessRetState;
    
    // Applet 返回状态
    FSMState            m_eMsgRetState;

    // Applet 返回状态
    FSMState            m_eBirthdayRetState;

    // Applet 返回状态
    FSMState            m_eEditRetState;
    
    // Applet是否处于挂起状态
    boolean             m_bSuspending;
    
    // Applet是否处于活动状态
    boolean             m_bActive;
    
    /////////////
    // Instance
    /////
    IFileMgr           *m_pFileMgr;       // IFileMgr interface
    IAddrBook          *m_pAddrPhone;     // IADDRBOOK interface for phone
#ifdef FEATURE_RUIM_PHONEBOOK
    IAddrBook          *m_pAddrRUIM;      // IADDRBOOK interface for RUIM
#endif /* FEATURE_RUIM_PHONEBOOK */
    ICallApp         *m_pCallApp;     // Reference UI call instance
    IAddrRec           *m_pCurrRec;       // Current record
    boolean             m_bIsPhoneRec;    // Indicate if m_pCurrRec is Phone record
    
    // 当前查看的电话本记录 Cache 列表
    IVector            *m_pAddList;

    // IVector interfaces that holds a list of AEEAddrField records
    IVector            *m_pFldIv;
    
    // 通用字符串Buffer
    AECHAR              m_szBuf[MAX_LEN_COMMBUF+1];
    AECHAR              m_szFmtBuf[MAX_LEN_COMMBUF+1];
    
    // 指向MessageBox信息的指针，动态分配需释放
    AECHAR             *m_pMsgBox_Msg;
    
    // Applet start type
    AppletStartMethod   m_eStartMethod;
    
    // Applet config cache
    ContAppCFGCache     m_sCFGCache;
    
    // Search data
    AECHAR             *m_pSrchData;
    uint16              m_wSearchCont;//当前选中的记录
    
    // Error resource ID
    uint16              m_wErrStrID;
    
    // message resource ID
    uint16              m_wMsgResID;
    
    // List menu select index
    uint16              m_wSelectCont;//当前选中的记录
    uint16              m_wSelectFld; //当前记录的选中字段
    uint16              m_wEditCont;  //当前编辑的记录
    int                 m_nCurrIdx;
    uint32              m_nEnumIdx;
    int                 m_nCurrMenuIdx;
    // menu select item
    uint16              m_wSelectOpts;
    uint16              m_wSelectFldOpts;
    uint16              m_wSelectSetting;
    uint16              m_wSelectEdit;
    uint16              m_wSelectOneDial;
    uint16              m_wSelectPhoto;
    uint16              m_wSelectStore;
    uint16              m_wSelectCopyMove;
    uint16              m_wDeleteSelectSel;
    uint16              m_wPositionSel;
    uint16              m_wMainMenuSel;
    
    // Options menu state select store
    uint16              m_wOptsStatSel;
    uint16              m_wFindOptsSel;
    uint16              m_wGroupOptsSel;
    
    // list menu state select store
    uint16              m_wManagementSel;
    uint16              m_wMainListSel;
    uint16              m_wOneDialStatSel;
    uint16              m_wGroupViewSel;
    uint16              m_wFindListSel;
    uint16              m_wDeleteListSel;
    
    // list menu current index store
    uint16              m_wMainListIdx;
    uint16              m_wOneDialStatIdx;
    uint16              m_wGroupViewIdx;
    uint16              m_wFindListIdx;
    uint16              m_wDeleteListIdx;
    
    
    // List view type
    byte                m_nViewType;
    byte                m_nSaveType;
    uint16              m_wPosCmd;
    
    // 智能查找使用的内容。其中存储着当前字头查找所匹配的内容
    uint32             *m_pSmartFindBuf;//需要显示的匹配记录的索引
    uint32              m_pSmartBufLen;//找到的匹配的记录数目
    int                 m_nCurrSmartIdx;//当前菜单的第一个记录
    uint16              m_wSelectSmart;//当前选中的菜单
    AECHAR              *m_szAlpha;//智能查找中右上脚显示的字符串
    boolean             m_bInSmartFind;//如果右上脚有字符串,则处在智能查找模式
    boolean             m_bInsmartnofind;//mark it when don't find the records
    AECHAR              m_lastAlpha[MAX_INPUT_NAME_EN+1];//remember the last alpha when don't find the key
    AECHAR              *m_sSearchStr; // 按名字搜索的字符串
    AEERect             m_ScrollBarrc;
#ifdef FEATURE_LCD_TOUCH_ENABLE    
    AEERect             M_RGroupRect;
    AEERect             M_LGroupRect;
#endif    
    int                 m_wYPos;
    // Field input type
    AEEAddrFieldID      m_nFldInputID;
    AECHAR             *m_pFldInputBuf;
    
    AECHAR             *m_pAddNewName;
    AECHAR             *m_pAddNewMobile;
    AECHAR             *m_pAddNewHome;
    AECHAR             *m_pAddNewOffice;
    AECHAR             *m_pAddNewFax;    
    AECHAR             *m_pAddNewEMAIL;
    AECHAR             *m_pAddNewAddress;
    AECHAR             *m_pAddNewURL;
    AECHAR             *m_pAddNewRemark;
    AECHAR             *m_nRingToneID;
    AEEAddrCat          m_nGroupCat;
    uint16                   m_wFieldIndex;
    uint16                   m_wFieldCount;
    dword                   m_wFieldMask;
    uint16                   m_wKeyType;

    AEEAddrFieldID      m_nAddNewFldID;
    AEEAddrFieldID      m_nNumFldType;  //字段类型
    int16               m_wAddnewFocus;
    uint16              m_wFiledOptState;
    
    // For find data
    AEEAddrCat          m_wFindCat;
    AEEAddrFieldID      m_wFindFldID;
    void               *m_pFindData;
    
    // Copy all need variable
    uint16              m_wCopyRecCnt;
    uint16              m_wCopyStartID;
    uint16              m_wCopyCurrID;
    uint16              m_wTotalNum;
    AEECallback         m_nCopyCB;
    boolean             m_bCopyInited;
    boolean             m_bForceComplete;
    int                 m_nFailNum;
#ifdef FEATURE_RUIM_PHONEBOOK
    ContAppCopyMoveType             m_nCopyMoveType;
    ContAppCopyMoveType             m_nCopyOrMove;
#endif /* FEATURE_RUIM_PHONEBOOK */
    // For onoff dialog
    boolean             m_bOnOff;
    
    // For IContApp interface
    //Phone number save type. (see enum PhoneNumberSaveType).
    //will be used in case start  method is save number.
    //This indicates whether the number should be added to the new contact
    // or existing contact or RUIM.
    PhoneNumberSaveType m_eFldSaveType;
    //AECHAR             *m_pSaveEMAIL;
//    AECHAR             *m_pSaveURL;
    
    // Select field type id
    SelectFldType       m_wSelFldType;
    
    // Callback function used with IContApp_SelectField()
    PFNSELECTEDCONTACTCB    m_pSelFldCB;
    
    // Callback function used with vcard select
    PFNNOTIFY           m_pNotify;
    
    // for mailer call back buffer
    AECHAR                m_pBuf[MAX_LEN_WMS_BUF +1];
    
    // For find number used
    AECHAR             *m_pFindNum;
    
    // For store the file list
    AECHAR              m_wFileList[PHOTO_MAX_FILES][AEE_MAX_FILE_NAME];
    int                 m_nFileNum;
    
    // AECHAR for controls title
    AECHAR              m_pTitle[MAX_LEN_TITLE+1];

    IConfig            *m_pConfig;
    boolean             m_bAppReady;

    AECHAR              m_pOneDialBuf[MAX_INPUT_NUM + 1];
    AECHAR              *m_pOneDial;
    AECHAR              m_tmpBuf[MAX_INPUT_NUM + 1];
    boolean       m_bUnLock;
    uint16              m_wSelectGroup;

    byte                m_nSmartStateType;
    IRingerMgr      *m_pRingerMgr;

    // Delete need variable
    uint16              m_wDeleteRecCnt;
    boolean              m_bDeleteInited;
    AEECallback         m_nDeleteCB;
    uint16              m_wDeleteCurrID;   

    //Delete need end 
    ContAppDeleteType  m_nDeleteType;
    ITextCtl            *pText;
    Input_mode     m_nInputMode;
    AddorEdit     m_nAddnewOrEdit;
    int16           m_wselGroupId;
    int16           m_wNewNumFldMask;
    boolean         m_bInputNotComplete;
    boolean         m_nDeleteAllCheck;
    IBitmap * m_eBitmap;
    ContAppCopyErrType      m_eErrType;
    int16           m_eTotalSelectNum;
    ContAppAddFldType            m_eAddFldType;
    ContAppOneDialFldPair        m_eOneDialFld[CONTAPP_NUMBERFLDS];
    ContAppListFrom                         m_eListFrom;    // 区别mainmenu ->add 和 mainmenu -> contacts ->opt -> add
    //boolean m_bAllowSelChange;                              // 防止因设置多选的打勾调用 IMENUCTL_SetSelEx而引起的没必要的 EVT_CTL_SEL_CHANGED 事件。
    boolean m_bIsNotComplete;                                  // 判断 doing事件是否已经结束 或者 被中断。
    PromptMsg_e_Type m_eMsgType;
    uint16 m_wDelReturnSelId;                                      // 删除一条记录后，返回list该选中的id
    boolean m_bDelOk;                                          // 删除一条记录成功
    int m_nleftNum;
#ifdef FEATURE_GIVEUP_EDIT
	boolean m_bIsEndKey;
#endif
#ifdef FEATURE_LANG_CHINESE
    uint16 m_nInputModeTable[2];                                   // only support two input mode: AEE_TM_LETTERS and AEE_TM_NUMBERS
    byte  m_nCurrentInputMode;                                  
    boolean m_bIsEndKey;
    
#endif
    char  *m_strPhonePWD;
    IAnnunciator *m_pIAnn;
	IRUIM        *m_pIRUIM;
	boolean      m_isdel;
#ifdef FEATURE_RUIM_PHONEBOOK
    boolean      m_bADNRUIMSupport;
#endif
	boolean      m_boptaleadyView;
	boolean		 m_bNumberInvalid;		//Add By zzg 2011_12_15
#ifdef FEATURE_OEMOMH        
   // AEECardSessionReadTpStatus	*m_pReadStatus;
#endif
	boolean 	m_bNameLengthLonger;				//Add By zzg 2012_10_31
	boolean		m_bSpeedDialParam;					//Add By zzg 2012_10_31
	uint16		m_nSpeedDialNumber;					//Add By zzg 2012_11_01	
	boolean     m_isCalling;
} CContApp;

/*==============================================================================
                                 宏定义和常数
==============================================================================*/
//For BREW3.0 support the muti-language re-define the RES maco
#define CONTAPP_RES_FILE_LANG  AEE_RES_LANGDIR CONTACTAPP_RES_FILE

// 用户重新更新界面事件
#define EVT_USER_DIALOG_TIMEOUT   (EVT_USER+11)
#define EVT_USER_COMPLETE         (EVT_USER+12)
#define EVT_USER_ERR              (EVT_USER+13)
#define EVT_USER_MAX              (EVT_USER+14)

// 关闭对话框宏
#define CLOSE_DIALOG(DlgRet)                         \
        {                                            \
            pMe->m_eDlgRet = DlgRet;                 \
            (void) ISHELL_EndDialog(pMe->m_pShell);  \
        }

// 状态移动宏：先将前一状态用当前状态更新，再将当前状态设为nextState
#define MOVE_TO_STATE(nextState)                 \
        {                                        \
            FSMState tpState;                    \
            tpState = nextState;                 \
            pMe->m_ePreState = pMe->m_eCurState; \
            pMe->m_eCurState = tpState;          \
        }

//
// Conditional Debug Output
//
#define FARF_STATE            1
#define FARF_EVENT            1
#define FARF_ADDR             1

#ifdef FARF
#undef FARF
#endif

#define FARF(x, p)

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
函数:
       CContApp_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向ContApp Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       SUCCESS: 创建对话框成功
       非SUCCESS: 创建对话框失败

备注:
       

==============================================================================*/
int CContApp_ShowDialog(CContApp *pMe, uint16  dlgResId);

/*==============================================================================
函数:
    CContApp_ShowMsgBox

说明:
    函数由状态处理函数用来唤起消息对话框，显示消息提示用户。

参数:
    pMe [in]: 指向 Contact Applet 对象结构的指针。该结构包含小程序的特定信息。
    wTextResId [in]: 消息文本资源 ID。

返回值:
    none

备注:

==============================================================================*/
int CContApp_ShowMsgBox(CContApp *pMe, uint16 wTextResId);
                             
/*==============================================================================
函数:
       CContApp_ShowYesNoDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向CContApp Applet对象结构的指针,该结构包含小程序的特定信息.
       msgResId [in]：对话框字符串资源ID，函数根据该ID显示提示内容。
       msg : 指向需要显示的字符串，优先于msgResId
       msglen: 现实的字符串长度wstrlen而来.-1 Auto caculate the lenght

返回值:
       int : SUCCESS if create dialog

备注:
       

==============================================================================*/
int  CContApp_ShowYesNoDialog( CContApp   *pMe, 
                               uint16      msgResId ,
                               AECHAR     *msg, 
                               int         msglen );
                               
/*==============================================================================
函数:
       CContApp_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向ContApp Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean CContApp_RouteDialogEvent( CContApp  *pMe,
                                   AEEEvent   eCode,
                                   uint16     wParam,
                                   uint32     dwParam);

/*----------------------状态机相关函数声明---------------------*/
/*==============================================================================
函数:
       CContApp_ProcessState
说明:
       ContApp Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向ContApp Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction CContApp_ProcessState(CContApp *pMe);

/*==============================================================================
函数: 
       CContApp_GetConfig
       
说明: 
       获取Config项目的值
       
参数: 
       pMe       [in] :指向ContApp Applet对象结构的指针。
       eCFG      [in] :获取的配置项
       data      [out]:数据存储区
       dataLen   [in] :数据存储区的大小,in bytes
       
返回值:
       SUCCESS : 操作成功
       EFAILED : 操作失败
       EBADPARM: 参数错误
       
备注:
       
==============================================================================*/
int CContApp_GetConfig( CContApp        *pMe, 
                        ContAppCFG       eCFG, 
                        void            *data,
                        int              dataLen);
                       
/*==============================================================================
函数: 
       CContApp_SetConfig
       
说明: 
       设置Config项目的值
       
参数: 
       pMe       [in] :指向ContApp Applet对象结构的指针。
       eCFG      [in] :获取的配置项
       data      [out]:数据存储区
       dataLen   [in] :数据存储区的大小,in bytes
       
返回值:
       SUCCESS : 操作成功
       EFAILED : 操作失败
       EBADPARM: 参数错误
       
备注:
       
==============================================================================*/
int CContApp_SetConfig(CContApp        *pMe, 
                       ContAppCFG       eCFG, 
                       void            *data,
                       int              dataLen);
                       
/*=============================================================================

FUNCTION:CContApp_LoadPhoneBook

DESCRIPTION:
    载入电话本记录
    
PARAMETERS:
    pMe       :
    nViewType : Load the record from where
    
RETURN VALUE:
    SUCCESS 
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int CContApp_LoadPhoneBook(CContApp *pMe, byte nViewType);

/*=============================================================================

FUNCTION:CContApp_LoadbySmart

DESCRIPTION:
    载入智能查找的记录到电话本IVector接口
    
PARAMETERS:
    pMe   :
    pStr  : search data Alpha string '*' is wildcard
    
RETURN VALUE:
    SUCCESS 
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int CContApp_LoadbySmart(CContApp *pMe, AECHAR *pStr);

/*=============================================================================

FUNCTION:CContApp_FreeAddrFlds

DESCRIPTION: 
    释放存储当前记录字段信息的IVector接口 m_pFldIv 的空间

PARAMETERS:
   pMe:

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
void CContApp_FreeAddrFlds(CContApp *pMe);

/*=============================================================================

FUNCTION: CContApp_SaveField

DESCRIPTION:
    Saves the field info into the
    IAddrRec at pMe->m_pFldIv[idx]
   
PARAMETERS:
    pMe    : pointer to CContApp object
    idx    : index of field in m_pFldIv
    wContID: Contact ID of saved
    wFldID : type of field
    pData  : data of field
    
RETURN VALUE:
    SUCCESS
   
COMMENTS:

SIDE EFFECTS:
    pMe->m_pFldIv
    
SEE ALSO:
=============================================================================*/
int CContApp_SaveField( CContApp       *pMe,
                        uint16          wContID,
                        uint16          idx,
                        AEEAddrFieldID  wFldID,
                        void           *pData);

/*=============================================================================

FUNCTION: CContApp_DeletePhoneFld

DESCRIPTION:
    Deletes the field pMe->m_fieldSelId from the current record in phone
    
PARAMETERS:
    pMe    : pointer to CContApp object
    wContID: contact id of delete field
    idx    : delete index in m_pFldIv
    
RETURN VALUE:
    boolean: TRUE if successful otherwise false
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
int CContApp_DeletePhoneFld(CContApp *pMe, uint16 wContID, uint16 idx);

/*=============================================================================

FUNCTION:CContApp_GetFldBuf

DESCRIPTION:
    获取制定字段的 Field 数据Buffer
    
PARAMETERS:
    pMe     :
    idx     : 选择字段的 index
    
RETURN VALUE:
    SUCCESS: 
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
void *CContApp_GetFldBuf(CContApp *pMe, int idx);

/*=============================================================================

FUNCTION:CContApp_LoadAddrFlds

DESCRIPTION:
    将记录的字段内容载入到 m_pFldIv 中
    
PARAMETERS:
    pMe   :
    wRecID: The ID suit of the OEM
    eFldType: 载入的字段类型，对姓名无效 SELECT_ALL 载入全部
    
RETURN VALUE:
    SUCCESS:
    
COMMENTS:
    
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int CContApp_LoadAddrFlds( CContApp      *pMe,
                           uint16         wContID,
                           SelectFldType  eFldType);

/*=============================================================================

FUNCTION: CContApp_DeleteCont

DESCRIPTION:
    Deletes the record according the cont ID
    
PARAMETERS:
    pMe     : pointer to CContApp object
    wContID : the cont id deleted
    
RETURN VALUE:
    SUCCESS
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
int CContApp_DeleteCont(CContApp *pMe, uint16 wContID);

/*=============================================================================

FUNCTION:CContApp_GetFldID

DESCRIPTION:
    获取制定字段的 Field ID 类型
    
PARAMETERS:
    pMe     :
    idx     : 选择字段的 index
        
RETURN VALUE:
    SUCCESS: 
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
AEEAddrFieldID CContApp_GetFldID(CContApp *pMe, int idx);
                        
/*=============================================================================
FUNCTION: CContApp_AddField

DESCRIPTION:
    增加一个字段
    This function will add a new field to the current contact.
   
PARAMETERS:
    pMe    : pointer to CContApp object
    wContID: contact id of add field
    wFldID : type of field
    pData  : data of field
    
RETURN VALUE:
    SUCCESS  : if successful otherwise EFAILED
    EBADPARM : if parameter is bad
    EEXIST   : if the content have exsit
    ENOMEMORY: if no memory
    EMAXFIELD: if reach the max field
   
COMMENTS:

SIDE EFFECTS:
    pMe->m_pFldIv
    
SEE ALSO:
=============================================================================*/
int CContApp_AddField( CContApp       *pMe,
                       uint16          wContID,
                       AEEAddrFieldID  wFldID,
                       void           *pData);

/*=============================================================================

FUNCTION:CContApp_GetBufbyFldID

DESCRIPTION:
    获取指定字段的 Field 数据Buffer
    
PARAMETERS:
    pMe     :
    wFldID  : 选择字段的field ID
    
RETURN VALUE:
    SUCCESS: 
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
void *CContApp_GetBufbyFldID(CContApp *pMe, AEEAddrFieldID wFldID);

/*=============================================================================

FUNCTION:CContApp_GetCurrFldNum

DESCRIPTION:
    获得对应 Field 的当前数量
    
PARAMETERS:
    pMe     :
    nFldID  : 字段类型
    
RETURN VALUE:
    The number of the fields
    
COMMENTS:
    
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint16 CContApp_GetCurrFldNum(CContApp *pMe, AEEAddrFieldID wFldID);

/*=============================================================================

FUNCTION:CContApp_GetFldMaxNum

DESCRIPTION:
    获得对应 Field 的最大尺寸
    
PARAMETERS:
    nFldID  : 字段类型
    
RETURN VALUE:
    The max number of the fields
    
COMMENTS:
    
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint16 CContApp_GetFldMaxNum(AEEAddrFieldID wFldID);

#ifdef FEATURE_RUIM_PHONEBOOK
/*=============================================================================

FUNCTION:CContApp_CopyField

DESCRIPTION: 
    复制当前选择的字段到另一面（RUIM记录则复制到话机，话机则复制到卡）
    此函数将自动识别记录的存储位置

PARAMETERS:
    pMe    :
    wContID: the Cont ID of current record
    wFldIdx: the index of field that will be copied

RETURN VALUE:
    SUCCESS
    EMAXRECORD : target device is full
    EMAXFIELD  : Reach the max fields number
    EEXIST     : if field have exist
    EBADPARM   : if parameter is bad
    
COMMENTS:
    此函数假设 m_pFldIv 是可用的
    
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
int CContApp_CopyField(CContApp *pMe, uint16 wContID, uint16 wFldIdx);
extern boolean OEM_IsCDMASVCSupport(uim_cdma_svc_table_enum_type eType);
#endif /* FEATURE_RUIM_PHONEBOOK */

/*==============================================================================
函数:
    CContApp_LoadByCondition

说明:
    函数根据设定的条件载入符合条件的全部电话本记录 Cache 。

参数:
    pMe [in]: 指向 CContApp Applet对象结构的指针。该结构包含小程序的特定信息。
    wDataSize[in]: 条件数据大小。
         
返回值:
    SUCCESS ：成功
    其它值失败

备注:
    条件 pMe->m_wFindCat, pMe->m_wFindFldID, pMe->m_pFindData 请在调用本函数前
    设置好, pMe->m_pFindData 的大小以参数 wDataSize 传入。
    本函数搜索范围为全部电话本记录(含卡),结果存于队列 pMe->m_pAddList

==============================================================================*/
int CContApp_LoadByCondition(CContApp *pMe, uint16 wDataSize);


/*=============================================================================

FUNCTION:CContApp_LoadByName

DESCRIPTION:
    按姓名查找记录
    
PARAMETERS:
    pMe   :
    pName : search name data
    
RETURN VALUE:
    SUCCESS 
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int CContApp_LoadByName(CContApp *pMe, AECHAR *pName);

/*=============================================================================

FUNCTION:CContApp_LoadByCat

DESCRIPTION:
    按姓名查找记录
    
PARAMETERS:
    pMe   :
    pName : search name data
    
RETURN VALUE:
    SUCCESS 
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int CContApp_LoadByCat(CContApp *pMe, AEEAddrCat wCategory);

/*=============================================================================

FUNCTION: CContApp_SetCat

DESCRIPTION:
    Set current select cont category
   
PARAMETERS:
    pMe    : pointer to CContApp object
    wContID: Contact ID of saved
    wCat   : category that be set
    
RETURN VALUE:
    SUCCESS
   
COMMENTS:

SIDE EFFECTS:
    
SEE ALSO:
=============================================================================*/
int CContApp_SetCat(CContApp *pMe, uint16 wContID, AEEAddrCat wCat);

/*=============================================================================

FUNCTION: CContApp_GetCat

DESCRIPTION:
    Get current select cont category
   
PARAMETERS:
    pMe    : pointer to CContApp object
    wContID: Contact ID of saved
    
RETURN VALUE:
    SUCCESS
   
COMMENTS:

SIDE EFFECTS:
    
SEE ALSO:
=============================================================================*/
AEEAddrCat CContApp_GetCat(CContApp *pMe, uint16 wContID);

/*=============================================================================

FUNCTION:CContApp_CopyAllToRUIM

DESCRIPTION: 
    将当前话机的记录全部复制到 RUIM 卡

PARAMETERS:
    pMe    :
    wStartID: the Cont ID of Start copy
    
RETURN VALUE:
    SUCCESS   : 
    EMAXRECORD: if the target device is full
    
COMMENTS:
    
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
int CContApp_CopyAllToRUIM(CContApp *pMe, uint16 wStartID);

/*=============================================================================

FUNCTION:CContApp_CopyAllToPhone

DESCRIPTION: 
    将当前话机的记录全部复制到 RUIM 卡

PARAMETERS:
    pMe    :
    wStartID: the Cont ID of Start copy
    
RETURN VALUE:
    SUCCESS   : 
    EMAXRECORD: if the target device is full
    
COMMENTS:
    
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
int CContApp_CopyAllToPhone(CContApp *pMe, uint16 wStartID);

/*=============================================================================

FUNCTION:CContApp_MergeField

DESCRIPTION: 
    复制字段到话机

PARAMETERS:
    pMe     :
    pNameFld: name field
    pNumFld : number field

RETURN VALUE:
    SUCCESS   : 
    EMAXFIELD : if field type is full in current record
    EEXIST    : if field have exist
    EBADPARM  : if parameter is bad
    
COMMENTS:

SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
int CContApp_MergeField( CContApp       *pMe, 
                         uint16          wRecID,
                         void           *pFldData,
                         AEEAddrFieldID  wFldID);

/*=============================================================================

FUNCTION:CContApp_LoadByNum

DESCRIPTION:
    按号码查找记录
    
PARAMETERS:
    pMe  :
    pNum :
        
RETURN VALUE:
    SUCCESS 
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int CContApp_LoadByNum(CContApp *pMe, AECHAR *pNum);

/*=============================================================================

FUNCTION:CContApp_MakeCall

DESCRIPTION:
    对指定的字段发起呼叫
    
PARAMETERS:
    pMe     :
    idx     : 选择字段的 index
        
RETURN VALUE:
    SUCCESS: 
    
COMMENTS:
    This function only used in view dialog and.
    Assume the m_pFldIv is available.
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
int CContApp_MakeCall(CContApp *pMe, int idx);

/*=============================================================================

FUNCTION: WStr2Alpha

DESCRIPTION: 
    将字符串转换为对应的按键字符串
    
PARAMETERS:
    pDest:
    nLen : 目的字符串长度 in AECHAR
    pSrc :
    
RETURN VALUE:
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
//void WStr2Alpha(AECHAR *pDest, int nLen , AECHAR *pSrc);

/*=============================================================================

FUNCTION: WStr2Letter

DESCRIPTION: 
    将字符串转换为对应的 ASCII 字符串
    
PARAMETERS:
    pDest:
    nLen : 目的字符串长度 in AECHAR
    pSrc :
    
RETURN VALUE:
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
//void WStr2Letter(AECHAR *pDest, int nLen , AECHAR *pSrc);

/*=============================================================================

FUNCTION:CContApp_LoadIconInfo

DESCRIPTION:
    载入Icon的信息
    
PARAMETERS:
    pMe     :

RETURN VALUE:
    SUCCESS: 
    EFAILED: If the number of current cache is 0
    
COMMENTS:
    
SIDE EFFECTS:
    pMe->m_wSelectFld
    pMe->m_wSelectCont
    
SEE ALSO:

=============================================================================*/
int CContApp_LoadIconInfo(CContApp *pMe);

/*==============================================================================

函数: CContApp_CmpRegionNumber
       
说明: 
    匹配号码的区号
       
参数: 
    pAddrRec: 
    pNum    : 
    
返回值:
    boolean
       
备注:
       
==============================================================================*/
boolean  CContApp_CmpRegionNumber(IAddrRec *pAddrRec, AECHAR *pNumber);

/*==============================================================================

函数: CContApp_BuildContInfo
       
说明: 
    构建 Contact info 结构
       
参数: 
    pAddrRec  : 
    pContInfo : 
    
返回值:
    None
       
备注:
       
==============================================================================*/
void CContApp_BuildContInfo(IAddrRec *pAddrRec, ContInfo *pContInfo);

/*==============================================================================

函数: CContApp_CmpNumber
       
说明: 
    精确匹配号码
       
参数: 
    pAddrRec: 
    pNum    : 
    
返回值:
    boolean
       
备注:
       
==============================================================================*/
boolean CContApp_CmpNumber(IAddrRec *pAddrRec, AECHAR *pNumber);

/*=============================================================================

FUNCTION:CContApp_DeleteMultipe

DESCRIPTION: 
    将当前话机的记录全部复制到 RUIM 卡

PARAMETERS:
    pMe    :
    wStartID: the Cont ID of Start copy
    
RETURN VALUE:
    SUCCESS   : 
    EMAXRECORD: if the target device is full
    ENORECORD : if no record to copy
    
COMMENTS:
    
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
int CContApp_DeleteMultipe(CContApp *pMe);

/*=============================================================================

FUNCTION:CContApp_CopyMultipe

DESCRIPTION: 
    copy or move
    
PARAMETERS:
    pMe    :
    wStartID: the Cont ID of Start copy
    
RETURN VALUE:
    SUCCESS   : 
    EMAXRECORD: if the target device is full
    ENORECORD : if no record to copy
    
COMMENTS:
    
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/

int CContApp_CopyMultipe(CContApp *pMe);
#ifndef FEATURE_LOW_MEM_BIGFONT
extern void OEM_SetNotUpdateScreen(boolean bOn);
#endif
#endif// CONTAPP_PRIV_H

