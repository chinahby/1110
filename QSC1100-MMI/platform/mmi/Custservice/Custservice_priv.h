#ifndef _CUSTSERVICEPRIV_H_
#define _CUSTSERVICEPRIV_H_
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
   #ifndef AEECLSID_CUSTSERVICE
      #error AEECLSID_CUSTSERVICE must be defined
   #endif
#endif

#include "AEEShell.h"
#include "AEEModTable.h"
#include "AEEStdLib.h"
#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "OEMClassIDs.h"

#include "CustserviceApp.h"

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
#include "custservice.brh"
/*==============================================================================
                                Marco Customer Service Resource 
==============================================================================*/
#define CUSTSERVICE_URL_SIZE (128)
/*Define Customer Service Resource*/
#define AEE_CUSTSERVICE_RES_FILE AEE_RES_LANGDIR CUSTSERVICE_RES_FILE
#define CUSTSERVICE_ERR(format, code1, code2, code3) 
#define CONTROL_RECT_START_X    (40) 

/*ARABIC*/
#define ARABIC_CONTROL_RECT_START_X   (2)
#define CONTROL_RECT_START_DX        (pMe->m_rc.dx - CONTROL_RECT_START_X - ARABIC_CONTROL_RECT_START_X) //86
#define AUTO_POWER_IMG_DX            (8)

/*DEFINE AVAIBLE*/
#define RUNNING_UPDATE_MSECS  (25)
#define MSECS_PER_DAY         (3600 * 24 * 1000)

#define CUSTSERVICE_MENU_DRAW_BOTTOMBAR(x)                           \
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
	DLGRET_CUSTSERVICE_MAIN,/*Customer Service main*/
	DLGRET_CUSTSERVICE_CUSTHOTLINE,/*Customer Hotline */
	DLGRET_CUSTSERVICE_MOBILEHALL,/*Customer E Hall*/
	DLGRET_CUSTSERVICE_CALLSECRETARY,/*Calling Secretary*/
	DLGRET_CUSTSERVICE_SUBSCRIPTION,/*My Subscription*/
	DLGRET_CUSTSERVICE_PHSERGUIDE,/*Phone Service Guide*/
	DLGRET_MSGBOX,                  
	DLGRET_CANCELED
} DLGRet_Value_e_Type;


/*==============================================================================
                                 
==============================================================================*/
typedef enum _CustServiceState
{
	CUSTSERVICE_STATE_NONE,
	CUSTSERVICE_STATE_INIT,
	CUSTSERVICE_STATE_MAIN,
	CUSTSERVICE_STATE_CUSTHOTLINE,
	CUSTSERVICE_STATE_MOBILEHALL,
	CUSTSERVICE_STATE_CALLSECRETARY,
	CUSTSERVICE_STATE_SUBSCRIPTION,
	CUSTSERVICE_STATE_PHSERGUIDE,
	CUSTSERVICE_STATE_MSGBOX, 
	CUSTSERVICE_STATE_EXIT
}CustServiceState;


typedef enum _NextFSMAction
{
	NFSMACTION_WAIT,
	NFSMACTION_CONTINUE
} NextFSMAction;


typedef struct _CustServiceMod
{
	DECLARE_VTBL(IModule)
	uint32 m_nRefs;
} CustServiceMod;

typedef struct _CCustServiceMenu
{
	DECLARE_VTBL(ICustServiceMenu)
	uint32           m_nRefs;
	IModule          *m_pModule;
	IDisplay         *m_pDisplay;
	IShell           *m_pShell;
	AEERect          m_rc;           
	IDialog          *m_pActiveDlg;     
	uint16           m_pActiveDlgID;   
	DLGRet_Value_e_Type   m_eDlgRet; 
	boolean          m_bNotOverwriteDlgRet;
	CustServiceState m_ePreState; 
	CustServiceState m_eCurState; 
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
    char m_Url[CUSTSERVICE_URL_SIZE];
} CCustServiceMenu;

/*==============================================================================
                                 Marco Define State 
==============================================================================*/
//#define APPISREADY_TIMER       100             //wangxiuqin 20090104 moved to appscommon.h
//#define EVT_APPISREADY   (EVT_USER+1)           //wangxiuqin 20090104 moved to appscommon.h

#define MOVE_TO_STATE(nextState) {            \
            CustServiceState tpState;            \
            tpState = nextState;          \
            pMe->m_ePreState = pMe->m_eCurState; \
            pMe->m_eCurState = tpState;        \
        }

#define CUSTSERVICE_MENU_SOFK_HIGHT 0
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
 void CustService_ShowDialog(CCustServiceMenu *pMe,uint16  dlgResId);

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
 extern int CustService_ShowMsgBox(CCustServiceMenu *pMe, uint16 wTextResId);

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
 boolean CustService_RouteDialogEvent(CCustServiceMenu *pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam);

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
NextFSMAction CustService_ProcessState(CCustServiceMenu *pMe);
#endif