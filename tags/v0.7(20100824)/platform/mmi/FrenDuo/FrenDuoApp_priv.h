/** ----------------------------------------------------------------------------
 * FrenDuoApp_priv.h
 *
 * Copyright(c) 2010 zzg  
 *
 * History:
 *
 * when               who               what and why
 * -----------  -----------    -----------------------------
 * 20010.07.19      zzg                created 
 * -----------------------------------------------------------------------------
 */

#ifndef FRENDUO_PRIV_H
#define FRENDUO_PRIV_H
#endif

#include "frenduo.brh"
#include "AEEConfig.h"

#include "AEEAnnunciator.h"

#include "CallApp.h"
#include "AEERUIM.h"




/*==============================================================================
                                 
                                相关数据类型定义
                                 
==============================================================================*/

typedef struct _FrenDuoAppMod
{
    DECLARE_VTBL( IModule)

    uint32   referenceCounter;
    
} FrenDuoAppMod;


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

typedef struct _FrenDuoApp
{
    DECLARE_VTBL(IFrenDuoApp)
    uint32       referenceCounter;
    
    IModule     *m_pModule;
    IDisplay    *m_pDisplay;
    IShell      *m_pShell;

    IRUIM       *m_pIRUIM;
	ICallApp    *m_pCallApp;     // Reference UI call instance

    IDialog     *m_pActiveIDlg;
    uint32       m_pActivedlgID;
    uint16       m_MainSel;   				//一级菜单光标
        
    boolean     m_bDoNotOverwriteDlgResult; //是否不重写对话框结果  
    
    AEERect     m_rc;
    IAnnunciator *m_pIAnn;
}FrenDuoApp;

/*==============================================================================
                                 
                                 宏定义和常数
                                 
==============================================================================*/

#define  MAX_MATRIX_ITEMS 3

#define  FRENDUO_RES_FILE_LANG        AEE_RES_LANGDIR FRENDUO_RES_FILE


//关闭对话框的宏
#define CLOSE_DIALOG(dlgResult)             \
{                                           \
    pMe->m_eDlgReturn = dlgResult;          \
    (void) ISHELL_EndDialog(pMe->m_pShell); \
}

/*==============================================================================
                                 函数声明
==============================================================================*/
static int FrenDuoAppMod_CreateInstance(IModule *po, IShell *pIShell, AEECLSID  ClsId, void  **ppObj);

static uint32 FrenDuoAppMod_AddRef(IModule *po);

static uint32 FrenDuoAppMod_Release(IModule *po);

static void FrenDuoAppMod_FreeResources(IModule *po, IHeap *ph, IFileMgr *pfm);

