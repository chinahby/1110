/** ----------------------------------------------------------------------------
 * IndexZoneApp_priv.h
 *
 * Copyright(c) 2012 zzg  
 *
 * History:
 *
 * when               who               what and why
 * -----------  -----------    -----------------------------
 * 20012.12.15      zzg                created 
 * -----------------------------------------------------------------------------
 */

#ifndef INDEXZONE_PRIV_H
#define INDEXZONE_PRIV_H
#endif

#include "indexzone.brh"
#include "AEEConfig.h"

#include "AEEAnnunciator.h"

/*==============================================================================
                                 
                                相关数据类型定义
                                 
==============================================================================*/

typedef struct _IndexZoneAppMod
{
    DECLARE_VTBL( IModule)

    uint32   referenceCounter;
    
} IndexZoneAppMod;


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

typedef struct _IndexZoneApp
{
    DECLARE_VTBL(IIndexZoneApp)
    uint32       referenceCounter;
    
    IModule     *m_pModule;
    IDisplay    *m_pDisplay;
    IShell      *m_pShell;
    IConfig     *m_pConfig;     //Add by pyuangui 20121231
	
    IDialog     *m_pActiveIDlg;
    uint32       m_pActivedlgID;
    uint16       m_MainSel;   				//一级菜单光标
        
    boolean     m_bDoNotOverwriteDlgResult; //是否不重写对话框结果  
    
    AEERect     m_rc;
    IAnnunciator *m_pIAnn;
}IndexZoneApp;

/*==============================================================================
                                 
                                 宏定义和常数
                                 
==============================================================================*/

#define  MAX_MATRIX_ITEMS 3

#define  INDEXZONE_RES_FILE_LANG        AEE_RES_LANGDIR INDEXZONE_RES_FILE


//关闭对话框的宏
#define CLOSE_DIALOG(dlgResult)             \
{                                           \
    pMe->m_eDlgReturn = dlgResult;          \
    (void) ISHELL_EndDialog(pMe->m_pShell); \
}

/*==============================================================================
                                 函数声明
==============================================================================*/
static int IndexZoneAppMod_CreateInstance(IModule *po, IShell *pIShell, AEECLSID  ClsId, void  **ppObj);

static uint32 IndexZoneAppMod_AddRef(IModule *po);

static uint32 IndexZoneAppMod_Release(IModule *po);

static void IndexZoneAppMod_FreeResources(IModule *po, IHeap *ph, IFileMgr *pfm);

