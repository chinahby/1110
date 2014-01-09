/** ----------------------------------------------------------------------------
 * lemontwist_priv.h
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

#ifndef LEMONTWIST_PRIV_H
#define LEMONTWIST_PRIV_H
#endif

#include "lemontwist.brh"
#include "AEEConfig.h"

#include "AEEAnnunciator.h"
#ifdef FEATRUE_SUPPORT_G_SENSOR
#include "G_Sensor.h"
#endif

#include "AEEConfig.h"
#include "OEMCFGI.h"
#include "AEERUIM.h"




/*==============================================================================
                                 
                                相关数据类型定义
                                 
==============================================================================*/

typedef struct _LemontwistMod
{
    DECLARE_VTBL( IModule)

    uint32   referenceCounter;
    
} LemontwistMod;


typedef enum LemontwistState
{
    LEMONTWIST_MAIN,
    LEMONTWIST_ZONE,
    LEMONTWIST_LIVE,
    LEMONTWIST_PULL,
    LEMONTWIST_EMPLOYMENT,
    LEMONTWIST_VOICE,
    LEMONTWIST_FUN,
    LEMONTWIST_MULTI,
    LEMONTWIST_MVDO,
    LEMONTWIST_HELP,
    LEMONTWIST_LIVE_MSG,
    LEMONTWIST_EXIT
} LemontwistState;

typedef enum
{
    LEMONTWIST_STOP=0x00,
    LEMONTWIST_RUNNING,
    LEMONTWIST_SUSPEND
} LEMONTWIST_STATUS_e_type;

typedef enum DLGRetValue
{
    DLGRET_CREATE,
    DLGRET_ZONE,
    DLGRET_LIVE,
    DLGRET_PULL,
    DLGRET_EMPLOYMENT,
    DLGRET_VOICE,
    DLGRET_FUN,
    DLGRET_MULTI,
    DLGRET_MVDO,
    DLGRET_HELP,
    DLGRET_LIVE_MSG,
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

typedef struct _Lemontwist
{
    DECLARE_VTBL(ILemontwist)
    uint32          referenceCounter;
    
    IModule        *m_pModule;
    IDisplay       *m_pDisplay;
    IShell         *m_pShell;

    IDialog        *m_pActiveIDlg;
    uint32          m_pActivedlgID;
    uint16          m_MainSel;   //一级菜单光标
    DLGRetValue     m_eDlgReturn;

    LEMONTWIST_STATUS_e_type  m_eAppStatus;   // Applet 当前运行状态
    
    boolean         m_bDoNotOverwriteDlgResult; //是否不重写对话框结果    
    LemontwistState m_prevState;        // Applet前一状态
    LemontwistState m_currState;        // Applet当前状态
    AEERect         m_rc;
    
    IRUIM*          m_pIRUIM;
    IConfig*        m_pConfig;

    uint16          m_pActiveitem;
        
    IAnnunciator    *m_pIAnn;
} Lemontwist;

/*==============================================================================
                                 
                                 宏定义和常数
                                 
==============================================================================*/

#define LEMONTWIST_RES_FILE_LANG        (AEE_RES_LANGDIR LEMONTWIST_RES_FILE)

// 此宏用当前状态更新先前状态，再用nextState状态更新当前状态
#define MOVE_TO_STATE(nextState)            \
{                                           \
    LemontwistState tpState;                    \
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

static int LemontwistMod_CreateInstance( IModule *po,
    IShell      *pIShell,
    AEECLSID    ClsId,
    void        **ppObj
);

static uint32  LemontwistMod_AddRef( IModule *pi);

static uint32  LemontwistMod_Release ( IModule *pi);


static void LemontwistMod_FreeResources( IModule *po, IHeap *ph, IFileMgr *pfm);

