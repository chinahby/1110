#ifndef _ESURFING_H_
#define _ESURFING_H_
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
   #include "custservice.bid"
#else
   #include "OEMClassIds.h"  
   #ifndef AEECLSID_ESURFING
      #error AEECLSID_ESURFING must be defined
   #endif
#endif

#include "AEEShell.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "OEMClassIDs.h"

#include "EsurfingApp.h"

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
#include "esurfing.brh"
/*==============================================================================
                                Marco Esurfing Resource 
==============================================================================*/
/*Define Customer Service Resource*/
#define MAX_URL_SIZE (128)
#define AEE_ESURFING_RES_FILE AEE_RES_LANGDIR ESURFING_RES_FILE
#define ESURFING_ERR(format, code1, code2, code3) 
#define CONTROL_RECT_START_X    (40) 

/*ARABIC*/
#define ARABIC_CONTROL_RECT_START_X   (2)
#define CONTROL_RECT_START_DX        (pMe->m_rc.dx - CONTROL_RECT_START_X - ARABIC_CONTROL_RECT_START_X) //86
#define AUTO_POWER_IMG_DX            (8)

/*DEFINE AVAIBLE*/
#define RUNNING_UPDATE_MSECS  (25)
#define MSECS_PER_DAY         (3600 * 24 * 1000)

#define ESURFING_MENU_DRAW_BOTTOMBAR(x)                           \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = x;                         \
    DrawBottomBar(pMe->m_pDisplay, &BarParam);      \
}

#define APPISREADY_TIMER      100
#define EVT_APPISREADY       (EVT_USER + 30)

/*==============================================================================
                                 
==============================================================================*/
typedef enum DLGRetValue
{
	DLGRET_CREATE,
	DLGRET_OK,
	DLGRET_ESURFING_MAIN,     /*Main*/
	DLGRET_ESURFING_BESTTONE, /*Best Tone*/
	DLGRET_ESURFING_IMUSIC,   /*iMusic*/
	DLGRET_ESURFING_DOWNLOAD, /*Download*/
	DLGRET_ESURFING_UTK,      /*Utk*/
	DLGRET_ESURFING_ITV,      /*iTv*/
	DLGRET_ESURFING_VNET,     /*Vnet*/
	DLGRET_ESURFING_OFFICE,   /*Mobile office*/
	DLGRET_ESURFING_MEGAEYE,  /*Mega eye*/
	DLGRET_MSGBOX,                  
	DLGRET_CANCELED
} DLGRet_Value_e_Type;


/*==============================================================================
                                 
==============================================================================*/
typedef enum _EsurfingState
{
	ESURFING_STATE_NONE,
	ESURFING_STATE_INIT,
	ESURFING_STATE_MAIN,     /*Main*/
	ESURFING_STATE_BESTTONE, /*Best Tone*/
	ESURFING_STATE_IMUSIC,   /*iMusic*/
	ESURFING_STATE_DOWNLOAD, /*Download*/
	ESURFING_STATE_UTK,      /*Utk*/
	ESURFING_STATE_ITV,      /*iTv*/
	ESURFING_STATE_VNET,     /*Vnet*/
	ESURFING_STATE_OFFICE,   /*Mobile office*/
	ESURFING_STATE_MEGAEYE,  /*Mega eye*/
	ESURFING_STATE_MSGBOX,   /*MessageBox*/
	ESURFING_STATE_EXIT
}EsurfingState;


typedef enum _NextFSMAction
{
	NFSMACTION_WAIT,
	NFSMACTION_CONTINUE
} NextFSMAction;


typedef struct _EsurfingMod
{
	DECLARE_VTBL(IModule)
	uint32 m_nRefs;
} EsurfingMod;

typedef struct _EsurfingMenu
{
    DECLARE_VTBL(IEsurfingMenu)
    uint32           m_nRefs;
    IModule          *m_pModule;
    IDisplay         *m_pDisplay;
    IShell           *m_pShell;
    AEERect          m_rc;           
    IDialog          *m_pActiveDlg;     
    uint16           m_pActiveDlgID;   
    DLGRet_Value_e_Type   m_eDlgRet; 
    boolean          m_bNotOverwriteDlgRet;
    EsurfingState m_ePreState; 
    EsurfingState m_eCurState; 
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

    char m_URL[MAX_URL_SIZE];    
    
} CEsurfingMenu;

/*==============================================================================
                                 Marco Define State 
==============================================================================*/
//#define APPISREADY_TIMER       100             //wangxiuqin 20090104 moved to appscommon.h
//#define EVT_APPISREADY   (EVT_USER+1)           //wangxiuqin 20090104 moved to appscommon.h

#define MOVE_TO_STATE(nextState) {            \
            EsurfingState tpState;            \
            tpState = nextState;          \
            pMe->m_ePreState = pMe->m_eCurState; \
            pMe->m_eCurState = tpState;        \
        }

#define ESURFING_MENU_SOFK_HIGHT 0
#define  BANNER_MAXTXT       8

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
 void Esurfing_ShowDialog(CEsurfingMenu *pMe,uint16  dlgResId);

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
 extern int Esurfing_ShowMsgBox(CEsurfingMenu *pMe, uint16 wTextResId);

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
 boolean Esurfing_RouteDialogEvent(CEsurfingMenu *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam);

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
NextFSMAction Esurfing_ProcessState(CEsurfingMenu *pMe);
#endif