
#ifndef _BT_APP_H_
#define _BT_APP_H_

#include "bt_ui_ht_win.h"
#include "bluetooth.brh"
#ifndef AEE_SIMULATOR
#include "AEEAnnunciator.h"
#endif
#include "OEMKeyguard.h"
//////////////////////////////////////////////////////////////////////////
#define MAX_BTA_START_UP_TIME	5000
#define BTUI_MAX_MMI_MSG	32
#define AEE_BLUETOOTH_RES_FILE (AEE_RES_LANGDIR BLUETOOTH_RES_FILE)
/*
	tBtuiApp structure. This structure contains information specific to this applet.
*/
typedef struct _tBtuiApp
{
	AEEApplet m_a;	// it is need for brew applet
	
	IDisplay *m_pIDisplay;	// display interface for short hand
	IShell *m_pIShell;	// shell interface for short hand
	IStatic * m_pStatic;
	IFileMgr *m_pIFileMgr;	// file manager interface
#ifndef AEE_SIMULATOR
	IAnnunciator        *m_pIAnn;
#endif
#ifdef AEE_SIMULATOR
	uint16 m_cyLine;
	uint16 m_cyTitle;
	uint16 m_nButtonLine;
	uint16 m_cyClip;
	int m_cxScreen;
	int m_cyScreen;
	AEERect m_rcMsgBox;
#endif
	//AEERect m_rcTitle;
	//AEERect m_rcBottom;
	AEERect m_rcClient;

	//char m_szTrace[128];	// For the debug infomation.
	char m_szCommBuf[128];
	AECHAR m_szWCommBuf[128];
	
	// Window stack
	tHT_WIN_STACK *m_p_win_stack;

	// Menu item buffer.
	tHT_MENU_ITEM m_menu_item_buffer[BTUI_NUM_REM_DEVICE + 1];


	// Msg pointer array.
	tBTUI_BTA_MSG *m_p_msg_array[BTUI_MAX_MMI_MSG];
	uint8 m_msg_type_array[BTUI_MAX_MMI_MSG];
	uint16 m_msg_head, m_msg_tail;
	AEECallback m_cb_dispatch_msg;

	// The startup time.
	uint32 m_dwStartup;

	// Applet is background
	boolean m_bBackground;
	boolean m_bQuicktest;
	boolean m_bsuspend;
}tBtuiApp;

void bcmapp_dm_set_sleep_state(boolean state);
void bt_ui_process_suspend_event(tBTUI_BTA_MSG* pMsg, bt_ui_start_fun * fun);
//////////////////////////////////////////////////////////////////////////


#endif	// _BT_APP_H_