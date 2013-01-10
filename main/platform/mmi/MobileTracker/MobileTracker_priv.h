/** ----------------------------------------------------------------------------
 * mainmenu_priv.h
 *
 * Copyright(c) 2007 Anylook
 *
 *
 * History:
 *
 * when            who               what and why
 * -----------  -----------    -----------------------------
 * 2008.12.24                created  for  Q1
 *
 * -----------------------------------------------------------------------------
 */

#ifndef MOBILETRACKER_PRIV_H
#define MOBILETRACKER_PRIV_H
#endif

#include "mobiletracker.brh"
#include "AEEConfig.h"
#include "AEEText.h"

#include "AEEAnnunciator.h"
#ifdef FEATRUE_SUPPORT_G_SENSOR
#include "G_Sensor.h"
#endif



/*==============================================================================
                                 
                                相关数据类型定义
                                 
==============================================================================*/
/*==============================================================================
									 
									 宏定义和常数
									 
==============================================================================*/
	
#define MOBILETRACKER_RES_FILE_LANG        AEE_RES_LANGDIR MOBILETRACKER_RES_FILE
	
	// 此宏用当前状态更新先前状态，再用nextState状态更新当前状态
#define MOVE_TO_STATE(nextState)            \
	{											\
		MobileTrackerState tpState; 				   \
		tpState = nextState;					\
		pMe->m_prevState = pMe->m_currState;	\
		pMe->m_currState = tpState; 			\
	}
	
	//关闭对话框的宏
#define CLOSE_DIALOG(dlgResult)             \
	{											\
		pMe->m_eDlgReturn = dlgResult;			\
		(void) ISHELL_EndDialog(pMe->m_pShell); \
	}



typedef struct _MobileTrackerMod
{
    DECLARE_VTBL( IModule)

    uint32   referenceCounter;
    
} MobileTrackerMod;

//MAINMENU  下的几个状态
typedef enum MobileTrackerState
{
    MOBILETRACKERIAST_MAIN,
    STATE_PWD,	   //Application lock
    STATE_PWDINALD,
    STATE_ENABLE,
    STATE_D_NUMBER,
    STATE_VIEW_IMSI,
    STATE_OWER,
    STATE_CHAGNEPASSWOWRD,
    STATE_CONTENT,
    MULTIMEDIAST_EXIT
} MobileTrackerState;

typedef enum
{
    MOBILETRACKER_STOP=0x00,
    MOBILETRACKER_RUNNING,
    MOBILETRACKER_SUSPEND
} MOBILETRACKER_STATUS_e_type;

typedef enum DLGRetValue
{
    DLGRET_CREATE,
    MGDLGRET_PASS,
    MGDLGRET_FAILD,
    MGDLGRET_MSGBOX_OK,
    DLGRET_ENABLE,
    DLGRET_D_NUMBER,
    DLGRET_VIEW_IMSI,
    DLGRET_OWER,
    DLGRET_CHAGNEPASSWOWRD,
    DLGRET_CONTENT,
    DLGRET_CANCELED
}DLGRetValue;

typedef enum NextFSMAction
{
   NFSMACTION_WAIT,
   NFSMACTION_CONTINUE
   
} NextFSMAction;


typedef struct _Point
{
    int x;
    int y;
} Point;

typedef struct _MobileTracker
{
    DECLARE_VTBL(IMobileTracker)
    uint32          referenceCounter;
    
    IModule        *m_pModule;
    IDisplay       *m_pDisplay;
    IShell         *m_pShell;

    IDialog        *m_pActiveIDlg;
    uint32          m_pActivedlgID;
    uint16          m_MainSel;   //一级菜单光标
    DLGRetValue     m_eDlgReturn;

    MOBILETRACKER_STATUS_e_type  m_eAppStatus;   // Applet 当前运行状态
    
    boolean         	m_bDoNotOverwriteDlgResult; //是否不重写对话框结果    
    MobileTrackerState  m_prevState;        // Applet前一状态
    MobileTrackerState  m_currState;        // Applet当前状态
    AEERect             m_rc;
    
    IAnnunciator        *m_pIAnn;
    char             *m_strPhonePWD;

	char             *m_strOwerName;

	char             *m_strPhoneNumber1;
	char             *m_strPhoneNumber2;
	char             *m_strPhoneNumeer3;
	
    boolean          b_pwdWright;
	ITextCtl         *m_ppwdword; 
	ITextCtl         *m_cpwdword;

	ITextCtl         *m_Owner_Name;

	ITextCtl         *m_Number_one;
	ITextCtl         *m_Number_tow;
	ITextCtl         *m_Number_three;
	int              m_nitems;
	ITextCtl         *m_Content;
	boolean          m_Rturnpass;
} MobileTracker;



/*==============================================================================

                                 函数声明

==============================================================================*/

static int MobileTrackerMod_CreateInstance( IModule *po,
    IShell      *pIShell,
    AEECLSID    ClsId,
    void        **ppObj
);

static uint32  MobileTrackerMod_AddRef( IModule *pi);

static uint32  MobileTrackerMod_Release ( IModule *pi);


static void MobileTrackerMod_FreeResources( IModule *po, IHeap *ph, IFileMgr *pfm);

