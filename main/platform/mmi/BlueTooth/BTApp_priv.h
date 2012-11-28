#ifndef BTAPPPRIV_H
#define BTAPPPRIV_H

/*==============================================================================
// 文件：
//        BTApp_priv.h
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//
// 文件说明：
//
// 作者：zzg
// 创建日期：2011-01-04
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人    问题单号  修改内容、位置及原因
-----------      ----------  --------  -----------------------------------------
2011-01-04                  初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#ifndef AEE_SIMULATOR
#include "customer.h"   		//Handset Feature Definitions
#else
#include "BREWSimFeatures.h"   //Simulator Feature Definitions
#endif

#if !defined(BREW_STATIC_APP)
// Use the .bid file if compiling as dynamic applet
   #include "BTApp.bid"
#else
   #include "OEMClassIds.h"  // Applet Ids 
   #ifndef AEECLSID_BLUETOOTH_APP
      #error AEECLSID_BLUETOOTH_APP must be defined
   #endif
#endif

#include "AEEShell.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "OEMClassIDs.h"
//#include "OEMMacros.h"

#include "AEEMenu.h"
#include "AEEConfig.h"
#include "AppComFunc.h"
#include "AEEText.h"
#include "AEEAnnunciator.h"
//#include "AEEPhone.h"//add by lzhl20040823
#include "OEMText.h"
#include "OEMFeatures.h"
#include "OEMCFGI.h"
#include "AppComFunc.h"
#include "Appscommon.h"
#include "AEERUIM.h"
#include "OEMText.h"
#include "OEMNV.h"


#include "BTApp.h"

/*==============================================================================
                                 类型定义
==============================================================================*/
//efine AEE_APPSBTAPP_RES_FILE AEE_RES_LANGDIR BTAPP_RES_FILE	//Add By zzg 2010_12_10

#define BTAPP_ERR(format, code1, code2, code3) MSG_FATAL(format, code1, code2, code3)

// 画界面底部提示条宏定义
#define BTAPP_DRAW_BOTTOMBAR(x)                           \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = x;                         \
    DrawBottomBar(pMe->m_pDisplay, &BarParam);      \
}
/*----------------------对话框相关数据类型声明---------------------*/

// 对话框关闭时返回值列表
typedef enum DLGRetValue
{
    DLGRET_CREATE,					//IDD_BT_MAINMENU
    DLGRET_OK,  
    DLGRET_CANCELED,			
	
	DLGRET_DEVICESRH,				//IDD_DEVICE_SEARCH
	DLGRET_SRHRESULT,				//IDD_SEARCH_RESULT
	DLGRET_DEVICEINFO,				//IDD_BT_DEVICE_INFO
	DLGRET_DEVICEINFO_OPITION,		//IDD_BT_DEVICE_INFO_OPITION
	DLGRET_BOND_OPITION,			//IDD_BT_BOND_OPITION
	DLGRET_OPITION,					//IDD_BT_OPITION

	DLGRET_DEVICE,					//IDD_DEVICE
	DLGRET_DEVICE_LIST,				//IDD_DEVICE_LIST
	
	DLGRET_MYINFO,					//IDD_MYINFO
	DLGRET_MYINFO_OPITION,			//IDD_MYINFO_OPITION	
	DLGRET_MYINFO_EDIT,				//IDD_MYINFO_EDIT			
	DLGRET_SET_SECURITY,			//IDD_BT_SECURITY
	DLGRET_SET_DISCOVERABLE,		//IDD_BT_DISCOVERABLE
	DLGRET_SET_IOCAPABILITY,		//IDD_BT_IO_CAPABILITY
	DLGRET_SET_DEBUGKEY,			//IDD_BT_DEBUG_KEY


	DLGRET_FTP,						//IDD_BT_FTP
	DLGRET_FTP_SERVER,				//IDD_BT_FTP_SERVER
	DLGRET_FTP_CLIENT,				//IDD_BT_FTP_CLIENT
	DLGRET_FTP_SETTING,				//IDD_BT_FTP_SETTING
	DLGRET_FTP_SERVER_REGISTER,		//IDD_BT_FTP_SERVER_REGISTER
	DLGRET_FTP_BROWSE,				//IDD_BT_FTP_BROWSE
	DLGRET_FTP_BROWSE_OPITION,		//IDD_BT_FTP_BROWSE_OPITION

	
	DLGRET_BT_MSGBOX,				//IDD_BT_MSGBOX
	DLGRET_MSGBOX_CANCELED,
	DLGRET_PROGRESS_CANCELED,
	DLGRET_PROMPT,            		//IDD_PROMPT_MESSEGE
	DLGRET_PROMPT_CANCELED,			

	DLGRET_BT_SEND_FILE,			//IDD_BT_SEND_FILE
	DLGRET_BT_OBEX_LIST_SERVERS,	//IDD_BT_OBEX_LIST_SERVERS
	DLGRET_BT_FILE_PROGRESS,		//IDD_BT_FILE_PROGRESS

	DLGRET_BT_EDIT				//IDD_BT_EDIT	
} DLGRet_Value_e_Type;


/*----------------------状态机相关数据类型声明---------------------*/
// BTApp Applet 状态机状态：
typedef enum _BTAppState
{
	BTAPPST_NONE,
	BTAPPST_INIT,
	BTAPPST_MAIN,

	BTAPPST_DEVICESRH,
	BTAPPST_SRHRESULT,
	BTAPPST_DEVICEINFO,			
	BTAPPST_DEVICEINFO_OPITION,	
	BTAPPST_BOND_OPITION,			//IDD_BT_BOND_OPITION
	BTAPPST_OPITION,				//IDD_BT_OPITION

	BTAPPST_DEVICE,
	BTAPPST_DEVICE_LIST,	

	BTAPPST_MYINFO,
	BTAPPST_MYINFO_OPITION,

	BTAPPST_MYINFO_EDIT,
	
	BTAPPST_EDIT_NAME,
	BTAPPST_EDIT_SHORTNAME,
	BTAPPST_EDIT_MANUDATA,
	BTAPPST_SET_SECURITY,
	BTAPPST_SET_DISCOVERABLE,
	BTAPPST_SET_IOCAPABILITY,
	BTAPPST_SET_DEBUGKEY,

	BTAPPST_FTP,
	BTAPPST_FTP_SERVER,
	BTAPPST_FTP_CLIENT,
	BTAPPST_FTP_SETTING,
	BTAPPST_FTP_SERVER_REGISTER,
	BTAPPST_FTP_BROWSE,
	BTAPPST_FTP_BROWSE_OPITION,
	
	//BTAPPST_BT_MSGBOX,
	//BTAPPST_PROMPT,

	BTAPPST_BT_EDIT,   

	BTAPPST_BT_SEND_FILE,
	BTAPPST_BT_OBEX_LIST_SERVERS,
	//BTAPPST_BT_FILE_PROGRESS,

	BTAPPST_EXIT
} BTAppState;

// 状态处理函数返回给状态处理主函数的值类型
typedef enum NextFSMAction
{
   NFSMACTION_WAIT,
   NFSMACTION_CONTINUE
} NextFSMAction;

// 保存模块信息和模块引用计数的结构
typedef struct BTAppMod
{
    DECLARE_VTBL(IModule)
    uint32   m_nRefs;
} BTAppMod;



typedef struct _IBTApp  IBTApp;
 QINTERFACE(IBTApp)
{
    INHERIT_IApplet(IBTApp);
};


/*==============================================================================
                                 类接口函数定义
==============================================================================*/
#define IBTApp_AddRef(p)                GET_PVTBL(p,IBTApp)->AddRef(p)

#define IBTApp_Release(p)                GET_PVTBL(p,IBTApp)->Release(p)

#define IBTApp_HandleEvent(p,ec,wp,dw) \
GET_PVTBL(p,IBTApp)->HandleEvent(p, ec, wp, dw)




typedef struct _CBTApp
{
  DECLARE_VTBL(IBTApp)
  	
  //AEEApplet       a;
  IShell           *m_pShell;
  
  IDisplay*       m_pIDisplay;

  AEERect         m_rect;
  uint16          m_nColorDepth;

  //Add For BT_MSG_BOX/BT_PROMPT
  uint16          m_msg_id;
  boolean		  m_bNeedStr;
  AECHAR  		  wMsgBuf[MSGBOX_MAXTEXTLEN];

  uint16          m_prompt_id; 
  boolean		  m_bPromptNeedStr;
  AECHAR  		  wPromptBuf[MSGBOX_MAXTEXTLEN];  

  uint16          m_edit_id;
  uint16		  m_edit_state_id;
  boolean		  m_bEditNeedStr;
  AECHAR  		  wEditBuf[MSGBOX_MAXTEXTLEN];  

  uint16		  m_dialog_id;
  uint16		  m_device_list_id;		//Device list Add By zzg 2011_06_07

  boolean		  m_app_flag;			//标示是否当前应用时BLUETOOTHAPP
  uint16  		  m_user_wParam;		//存储EVT_USER的WPARAM  
  uint16		  m_folder_index;		//FTP_Browse时的文件夹INDEX

  uint16		  m_obex_list_id;		


  AEEBTProgressInfo m_fileprogInfo;
  //Add End

  //Add By zzg 2011_01_04
  uint32           m_nRefs;            // 对象引用计数器
  IModule          *m_pModule;             
  IDialog          *m_pActiveDlg;      // 当前活动对话框ID
  uint16           m_pActiveDlgID;     // 活动对话框关闭时的返回结果
  DLGRet_Value_e_Type   m_eDlgRet;     
  boolean          m_bNotOverwriteDlgRet;	// 是否不改写对话框返回结果，一般情况需要改写(FALSE)。
  BTAppState 		m_ePreState;        // Applet 前一状态
  BTAppState 		m_eCurState;        // Applet 当前状态
  boolean          m_bSuspending;      // Applet是否处于挂起状态
  boolean          m_bAppIsReady;		// Applet是否准备好可以处理按键和命令事件。改变量主要用于快速按键引起的问题

  uint16           m_currDlgId;       //保存当前所选菜单位置
  uint16           m_nSubDlgId;		    //次级菜单位置
  uint16           m_sSubDlgId;       //三级菜单位置

  IRUIM            *m_pIRUIM;
  //Add End

  IMenuCtl*       m_pIMenu;
  IMenuCtl*       m_pISoftMenu;
  IStatic*        m_pStatic;
  ITextCtl*       m_pText;

  IAnnunciator    *m_pIAnn;		//Add By zzg 2010_10_29
  IConfig         *m_pConfig;	//Add By zzg 2010_11_23 

  BTAppMenuType   menuStack[ MENU_STACK_DEPTH ];
  uint16          uMenuSel[ MENU_STACK_DEPTH ];

  char			  m_pfilepath[256];	//AEEBT_MAX_FILE_NAME
  
  uint8           uTopMenu;
  BTApp_Enabling  mEnablingType;
  AG              mAG;
  NA              mNA;
  SD              mSD;
  RM              mRM;
  SPP             mSPP;
  L2              mL2;
#ifdef FEATURE_BT_EXTPF_AV
  A2DP            mA2DP;
  AVRCP           mAVRCP;
#endif
#ifdef FEATURE_BT_EXTPF_OPP
  OPP             mOPP;
#endif
#ifdef FEATURE_BT_EXTPF_FTP
  FTP             mFTP;
#endif
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif  //FEATURE_APP_TEST_AUTOMATION


#ifdef FEATURE_BT_EXTPF_BIP
  BIP             mBIP;
#endif
#ifdef FEATURE_IOBEX
  OBEX            mOBEX;
#endif
#ifdef FEATURE_BT_EXTPF_BPP
  BPP             mBPP;
#endif
#ifdef FEATURE_BT_EXTPF_PBAP
  PBAP            mPBAP;
#endif
#ifdef FEATURE_BT_EXTPF_CTP
  CTP             mCTP;
  ICP             mICP;
#endif

#ifdef FEATURE_BT_EXTPF_SAP
#error code not present
#endif

#ifdef FEATURE_BT_EXTPF_HID_HOST
#error code not present
#endif
  ITelephone*     pIPhone;
  boolean         bFirstLaunch;
  boolean         bEnableAtStart;
  boolean         bSuspended;
  boolean         bBusyIconUp;
  boolean         bVocoderOn;
  boolean         bConfigChanged;
  uint16          uCurrMsgId;
  
  boolean		  bStartFromOtherApp;				//Add By zzg 2010_11_09  
  boolean		  bStartFromAudio;					//Add By zzg 2012_11_28
  boolean		  bStartFromPushReq;				//Add By zzg 2012_11_23
  boolean		  bUpdateProgress;					//Add By zzg 2010_11_27 

  uint16		  uDeviceSrhType;					//Add By zzg 2011_10_19
  uint16		  uDeviceListType;					//Add By zzg 2011_10_26

  boolean		  bMusicPlayerSuspend;				//Add By zzg 2011_11_07
 
  AECHAR*         pText1;
  AECHAR*         pText2;
  uint8*          pMem;
#ifdef FEATURE_BT_2_1
  boolean         bRemoteOOBRead; 
#endif
  boolean         bStartBTaplication;
} CBTApp;



/* Dialog Event Handler function pointer structure */
typedef boolean (*PFNDLGHANDLER) (void *pUser,AEEEvent evt,uint16 w,uint32 dw );

extern boolean BTApp_BuildMenu( CBTApp* pMe, BTAppMenuType menu );
extern boolean BTApp_BuildTopMenu( CBTApp* pMe );
extern boolean BTApp_HandleClearKey( CBTApp *pMe );
extern boolean BTApp_WriteConfigFile( CBTApp* pMe );
extern boolean BTApp_WriteBtAddrConfigFile(CBTApp * pMe);	//Add By zzg 2011_11_08
extern boolean BTApp_AGInit( CBTApp *pMe );
extern void    BTApp_AGDeInit( IApplet *pi  );
extern void    BTApp_DeregisterAGNotif( CBTApp *pMe );
extern void    BTApp_EnableBT( CBTApp* pMe );
extern void    BTApp_DisableBT( CBTApp* pMe );


//Add By zzg 2011_01_10
extern void BTApp_LocalOOBCreated( CBTApp* pMe );
extern void BTApp_CancelMsgBox( CBTApp* pMe );
extern void BTApp_CancelProMptBox(CBTApp *pMe);
extern void BTApp_CancelDevNameRequest( CBTApp* pMe );
extern int BTApp_RemoteOOBWrite( CBTApp* pMe,  AEEBTDeviceInfo *pDev );
extern void BTApp_DoRemoveOne( CBTApp* pMe );
extern void BTApp_PadString(char* Passkey, int passKeyLen);
extern boolean BTApp_UserConfirm( CBTApp* pMe, boolean bConfirmed);
extern boolean BTApp_DoRebond(CBTApp* pMe ,boolean bAuthorized);
extern void BTApp_BuildPrompt( CBTApp* pMe, BTAppMenuType menu );
extern boolean BTApp_DoUnbondAll( CBTApp* pMe );
extern boolean BTApp_DoRemoveAll( CBTApp* pMe );
extern void BTApp_DoUnbondOne( CBTApp* pMe );
extern void BTApp_BuildBondMenu( CBTApp* pMe );
//Add End

//Add By zzg 2011_02_16
extern int BTApp_FTPListFolder( CBTApp* pMe );
extern int BTAppFTP_RemoveDir(CBTApp* pMe, char* pszDir);
extern void BTApp_GetNameOfLocalObjects(CBTApp* pMe, char *pszDir);
//Add End

//Add By zzg 2011_02_25
extern void bt_rm_refresh_efs_param(void);
//Add End

//Add By zzg 2011_03_02
extern void BTApp_UpdateDeviceInfo(CBTApp* pMe);
//Add End



extern const AEEBTBDAddr NULL_BD_ADDR;


 
/*==============================================================================
                                 宏定义和常数
==============================================================================*/

// 状态移动宏：先将前一状态用当前状态更新，再将当前状态设为nextState
#define MOVE_TO_STATE(nextState) {            \
            BTAppState tpState;            \
            tpState = nextState;          \
            pMe->m_ePreState = pMe->m_eCurState; \
            pMe->m_eCurState = tpState;        \
        }


#define APPISREADY_TIMER       100             
#define EVT_APPISREADY   (EVT_USER+1)  

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
       BTApp_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向BTApp Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:


==============================================================================*/
void BTApp_ShowDialog(CBTApp *pMe, uint16  dlgResId);

/*==============================================================================
函数:
       BTApp_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向BTApp Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean BTApp_RouteDialogEvent(CBTApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);

/*----------------------状态机相关函数声明---------------------*/
/*==============================================================================
函数:
       BTApp_ProcessState
说明:
       BTApp Applet状态处理主函数。函数根据pMe->m_currState将调用路由至
       相应的处理函数。

参数:
       pMe [In]: 指向BTApp Applet对象结构的指针,该结构包含小程序的特定信息.

返回值:
       NFSMACTION_CONTINUE：指示后有子状态，状态机不能停止。
       NFSMACTION_WAIT：指示因要显示对话框界面给用户，应挂起状态机。

备注:

==============================================================================*/
NextFSMAction BTApp_ProcessState(CBTApp *pMe);
#endif
//BTAPPPRIV_H

