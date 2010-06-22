/** ----------------------------------------------------------------------------
 * application_priv.h
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

#ifndef APPLICATION_PRIV_H
#define APPLICATION_PRIV_H
#endif

#include "application.brh"
#include "AEEConfig.h"

#ifdef FEATRUE_SUPPORT_G_SENSOR
#include "G_Sensor.h"
#endif



/*==============================================================================
                                 
                                相关数据类型定义
                                 
==============================================================================*/

typedef struct _ApplicationMod
{
    DECLARE_VTBL( IModule)

    uint32   referenceCounter;
    
} ApplicationMod;

//Application  下的几个状态
typedef enum ApplicationState
{
    APPLICATIONST_MAIN,

    APPLICATIONST_GAME,
    
    APPLICATIONST_PLAYER,

    APPLICATIONST_DATA, 
 

    APPLICATIONST_EXIT

} ApplicationState;


typedef enum
{
    APPLICATION_STOP=0x00,
    APPLICATION_RUNNING,
    APPLICATION_SUSPEND
} APPLICATION_STATUS_e_type;

typedef enum DLGRetValue
{
    DLGRET_CREATE,
        
    DLGRET_GAME,

    DLGRET_MEDIA,

    DLGRET_MAIN,

    DLGRET_DATA,


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

typedef struct _Application
{
    DECLARE_VTBL(IApplication)
    uint32       referenceCounter;
    
    IModule     *m_pModule;
    IDisplay    *m_pDisplay;
    IShell      *m_pShell;

    IDialog     *m_pActiveIDlg;
    uint32       m_pActivedlgID;
    uint16       m_MainSel;   //一级菜单光标
    DLGRetValue  m_eDlgReturn;

    APPLICATION_STATUS_e_type  m_eAppStatus;   // Applet 当前运行状态
    
    boolean     m_bDoNotOverwriteDlgResult; //是否不重写对话框结果    
    ApplicationState             m_prevState;        // Applet前一状态
    ApplicationState             m_currState;        // Applet当前状态
    AEERect     m_rc;
} Application;

/*==============================================================================
                                 
                                 宏定义和常数
                                 
==============================================================================*/
#if defined (FEATURE_DISP_128X128)
#define  MAX_MATRIX_ITEMS 9
#elif defined(FEATURE_DISP_160X128)
#define  MAX_MATRIX_ITEMS 6
#endif
#define ICON_ANIMATED_FRAME     1 //焦点图片的帧数


#define APPLICATION_RES_FILE_LANG        AEE_RES_LANGDIR APPLICATION_RES_FILE

#define STARTARGPREFIX_VIEWPLAYER            'P'
#define STARTARGPREFIX_VIEWDATA              'D'




// 此宏用当前状态更新先前状态，再用nextState状态更新当前状态
#define MOVE_TO_STATE(nextState)            \
{                                           \
    ApplicationState tpState;                    \
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

static int ApplicationMod_CreateInstance( IModule *po,
    IShell      *pIShell,
    AEECLSID    ClsId,
    void        **ppObj
);

static uint32 ApplicationMod_AddRef( IModule *po);

static uint32 ApplicationMod_Release( IModule *po);

static void ApplicationMod_FreeResources( IModule *po, IHeap *ph, IFileMgr *pfm);

