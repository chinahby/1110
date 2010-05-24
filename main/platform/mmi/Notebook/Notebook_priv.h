#ifndef _NOTEBOOK_H_
#define _NOTEBOOK_H_
/*******************************************************************
*	Copyright(c)2008-2009 TCL CDMA SW developement Team Reserved
*
********************************************************************/
/********************************************************************

	Team		: 	CDMA JRD SW
	Author		: 	Qiong.chen Create 
	Version 	:	1.0.0
	Function Name 	:
	Change Time 	:
		Qiong.chen Create 2009/02/04
		
*********************************************************************/

#ifndef AEE_SIMULATOR
#include "customer.h" 
#else
#include "BREWSimFeatures.h" 
#endif

#if !defined(BREW_STATIC_APP)
   #include "notebook.bid"
#else
   #include "OEMClassIds.h"  
   #ifndef AEECLSID_NOTEBOOK
      #error AEECLSID_NOTEBOOK must be defined
   #endif
#endif

#include "AEEShell.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "OEMClassIDs.h"

#include "NotebookApp.h"
#include "AEEVector.h"
#include "AEEMenu.h"
//#include "AEEStatic.h"
#include "AEEConfig.h"
#include "AppComFunc.h"
#include "AEEText.h"
#include "AEEAnnunciator.h"
#include "SoundMenu.h"
#include "OEMText.h"
#include "OEMFeatures.h"
#include "OEMCFGI.h"
#include "AppComFunc.h"
#include "Appscommon.h"
#include "AEERUIM.h"
#include "OEMText.h"

#include "WMSapp.h"
#include "CallApp.h"
#include "notebook.brh"
/*==============================================================================
                                Marco Notebook Resource 
==============================================================================*/
/*Define Notebook Resource*/
#ifdef FEATURE_NOTEBOOK_TEST
#define SPORT_EFS_FILE  "fs:/notebook/1.dat" 
#define SPORT1_EFS_FILE  "fs:/notebook/2.dat" 
#define SPORT2_EFS_FILE  "fs:/notebook/3.dat" 
#endif
#define NOTEBOOK_MAIN_DIR ("fs:/notebook/")
#define NOTEBOOK_FILE_EXT (".dat")
#define AEE_NOTEBOOK_RES_FILE AEE_RES_LANGDIR NOTEBOOK_RES_FILE
#define NOTEBOOK_ERR(format, code1, code2, code3) 
#define CONTROL_RECT_START_X    (40) 

/*ARABIC*/
#define ARABIC_CONTROL_RECT_START_X   (2)
#define CONTROL_RECT_START_DX        (pMe->m_rc.dx - CONTROL_RECT_START_X - ARABIC_CONTROL_RECT_START_X) //86
#define AUTO_POWER_IMG_DX            (8)

/*DEFINE AVAIBLE*/
#define RUNNING_UPDATE_MSECS  (25)
#define MSECS_PER_DAY         (3600 * 24 * 1000)

#define NOTEBOOK_MENU_DRAW_BOTTOMBAR(x)                           \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = x;                         \
    DrawBottomBar(pMe->m_pDisplay, &BarParam);      \
}

#define APPISREADY_TIMER      100
#define EVT_APPISREADY        (EVT_USER + 30)
/*==============================================================================
                                 
==============================================================================*/
typedef enum DLGRetValue
{
	DLGRET_CREATE,
	DLGRET_OK,
	DLGRET_NOTEBOOK_MAIN,     /*MAIN*/
	DLGRET_NOTEBOOK_MAIN_OPTION, /*MAIN OPTION*/
	DLGRET_NOTEBOOK_EDIT,   /*EDIT*/
	DLGRET_NOTEBOOK_VIEW,   /*VIEW*/
	DLGRET_NOTEBOOK_EDIT_OPTION, /*EDIT OPTION*/
	DLGRET_MSGBOX,                  
	DLGRET_CANCELED
} DLGRet_Value_e_Type;


/*==============================================================================
                                 
==============================================================================*/
typedef enum _NotebookState
{
	NOTEBOOK_STATE_NONE,
	NOTEBOOK_STATE_INIT,
	NOTEBOOK_STATE_MAIN,     /*Main*/
	NOTEBOOK_STATE_MAIN_OPTION, /*Main Option*/
	NOTEBOOK_STATE_EDIT,   /*Edit*/
	NOTEBOOK_STATE_VIEW,
	NOTEBOOK_STATE_EDIT_OPTION, /*Edit Option*/
	NOTEBOOK_STATE_MSGBOX,   /*MessageBox*/
	NOTEBOOK_STATE_EXIT
}NotebookState;


typedef enum _NextFSMAction
{
	NFSMACTION_WAIT,
	NFSMACTION_CONTINUE
} NextFSMAction;

typedef enum _ViewAddNewEntryState
{
   
   ADD_NEW_STATE,
   VIEW_STATE,
   VIEW_EDIT_STATE,
   RENAME_EDIT_STATE,
   NO_RECORD_STATE
}ViewAddNewEntryState;

typedef enum _WarnningMsgBackType
{
    MAIN_STATE_DEL,
    MAIN_STATE_DEL_ALL,
    EDIT_STATE_RENAME,
    EDIT_STATE_ADD_NEW,
    VIEW_STATE_DEL,
    VIEW_STATE_SAVE
}WarnningMsgBackType;

typedef struct _NotebookMod
{
	DECLARE_VTBL(IModule)
	uint32 m_nRefs;
} NotebookMod;

typedef struct _NotebookMenData
{
    char pNoteFileName[256];
    AECHAR pNoteContent[512];
}NotebookMenData;

typedef struct _NotebookMenu
{
	DECLARE_VTBL(INotebookMenu)
	uint32           m_nRefs;
	IModule          *m_pModule;
	IDisplay         *m_pDisplay;
    IFileMgr         *m_pFileMgr;       // IFileMgr interface
	IShell           *m_pShell;
	AEERect          m_rc;           
	IDialog          *m_pActiveDlg;
    IVector          *m_pAddList;
    ITimeCtl         *m_pTime;
	uint16           m_pActiveDlgID;   
	DLGRet_Value_e_Type   m_eDlgRet; 
	boolean          m_bNotOverwriteDlgRet;
	NotebookState m_ePreState; 
	NotebookState m_eCurState; 
	boolean          m_bSuspending;
	boolean          m_bAppIsReady;
	IAnnunciator    *m_pAnn;
	IConfig         *m_pConfig; 
	uint16           m_currDlgId; 
	uint16           m_nResID;
	uint16           m_nSubDlgId;
	uint16           m_sSubDlgId;
    ICallApp        *m_pCallApp; 
    uint16		     m_SelIndx;
    uint16           m_wMsgResID;
    boolean runOnBackgrounds;
    NotebookMenData m_NoteData;

    uint16  m_wSelectCont;//当前选中的记录
    int     am_nCurrIdx;
    uint32  curtime;
    char *m_pszSaveDir;
    char	m_FileName[AEE_MAX_FILE_NAME];
    char m_tempbuffer[256];
    char *m_pszFileExtension;
    int m_nLastOperationError;
  
    ViewAddNewEntryState m_eEntryFlag;
    WarnningMsgBackType m_eWarnningMsgBackFlag;
} CNotebookMenu;

/*==============================================================================
                                 Marco Define State 
==============================================================================*/
#define MOVE_TO_STATE(nextState) {            \
            NotebookState tpState;            \
            tpState = nextState;          \
            pMe->m_ePreState = pMe->m_eCurState; \
            pMe->m_eCurState = tpState;        \
        }

#define NOTEBOOK_MENU_SOFK_HIGHT (0)
#define  BANNER_MAXTXT           (8)
#define MAX_NUM_MENUPOP          (6)

/*****************************************************************************
 * FUNCTION
 *  Morse_ShowDialog
 * DESCRIPTION
 *  Morse Application Show Dialog
 * PARAMETERS
 *  result      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
 void Notebook_ShowDialog(CNotebookMenu *pMe,uint16  dlgResId);

/*****************************************************************************
 * FUNCTION
 *  Morse_ShowMsgBox
 * DESCRIPTION
 *  Show Message Box Dialog
 * PARAMETERS
 *  result      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
 extern int Notebook_ShowMsgBox(CNotebookMenu *pMe, uint16 wTextResId);

/*****************************************************************************
 * FUNCTION
 *  Morse_ShowMsgBox
 * DESCRIPTION
 *  Morse Route Dialog
 * PARAMETERS
 *  result      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
 boolean Notebook_RouteDialogEvent(CNotebookMenu *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam);

/*****************************************************************************
 * FUNCTION
 *  Morse_ProcessState
 * DESCRIPTION
 *  Morse Route State
 * PARAMETERS
 *  result      [IN]        
 * RETURNS
 *  void
 *****************************************************************************/
NextFSMAction Notebook_ProcessState(CNotebookMenu *pMe);
#endif