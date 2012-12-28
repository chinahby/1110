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

#ifndef MAINMENU_PRIV_H
#define MAINMENU_PRIV_H
#endif

#include "multimedia.brh"
#include "AEEConfig.h"

#include "AEEAnnunciator.h"
#ifdef FEATRUE_SUPPORT_G_SENSOR
#include "G_Sensor.h"
#endif



/*==============================================================================
                                 
                                相关数据类型定义
                                 
==============================================================================*/

typedef struct _MultimediaMod
{
    DECLARE_VTBL( IModule)

    uint32   referenceCounter;
    
} MultimediaMod;

//MAINMENU  下的几个状态
typedef enum MultimediaState
{
    MULTIMEDIAST_MAIN,
#if defined(FEATURE_VERSION_C316)
   STATE_PWD,	   //Application lock
   STATE_PWDINALD,
#endif	
    MULTIMEDIAST_EXIT
} MultimediaState;

typedef enum
{
    MULTIMEDIA_STOP=0x00,
    MULTIMEDIA_RUNNING,
    MULTIMEDIA_SUSPEND
} MULTIMEDIA_STATUS_e_type;

typedef enum DLGRetValue
{
    DLGRET_CREATE,
#if defined(FEATURE_VERSION_W317A) ||defined(FEATURE_VERSION_C316)
   MGDLGRET_PASS,
   MGDLGRET_FAILD,
   MGDLGRET_MSGBOX_OK,
#endif
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

typedef struct _Multimed
{
    DECLARE_VTBL(IMultimed)
    uint32          referenceCounter;
    
    IModule        *m_pModule;
    IDisplay       *m_pDisplay;
    IShell         *m_pShell;

    IDialog        *m_pActiveIDlg;
    uint32          m_pActivedlgID;
    uint16          m_MainSel;   //一级菜单光标
    DLGRetValue     m_eDlgReturn;

    MULTIMEDIA_STATUS_e_type  m_eAppStatus;   // Applet 当前运行状态
    
    boolean         m_bDoNotOverwriteDlgResult; //是否不重写对话框结果    
    MultimediaState m_prevState;        // Applet前一状态
    MultimediaState m_currState;        // Applet当前状态
    AEERect         m_rc;
    
    IAnnunciator    *m_pIAnn;
#if defined(FEATURE_VERSION_C316)
   char             *m_strPhonePWD;
   boolean          b_pwdWright;
#endif
} Multimed;

/*==============================================================================
                                 
                                 宏定义和常数
                                 
==============================================================================*/

#define MULTIMEDIA_RES_FILE_LANG        (AEE_RES_LANGDIR MULTIMEDIA_RES_FILE)

// 此宏用当前状态更新先前状态，再用nextState状态更新当前状态
#define MOVE_TO_STATE(nextState)            \
{                                           \
    MultimediaState tpState;                    \
    tpState = nextState;                    \
    pMe->m_prevState = pMe->m_currState;    \
    pMe->m_currState = tpState;             \
}

//关闭对话框的宏
#define CLOSE_DIALOG(dlgResult)             \
{                                           \
    pMe->m_eDlgReturn = dlgResult;          \
    (void) ISHELL_EndDialog(pMe->m_pShell); \
}

/*==============================================================================

                                 函数声明

==============================================================================*/

static int MultimedMod_CreateInstance( IModule *po,
    IShell      *pIShell,
    AEECLSID    ClsId,
    void        **ppObj
);

static uint32  MultimedMod_AddRef( IModule *pi);

static uint32  MultimedMod_Release ( IModule *pi);


static void MultimedMod_FreeResources( IModule *po, IHeap *ph, IFileMgr *pfm);

