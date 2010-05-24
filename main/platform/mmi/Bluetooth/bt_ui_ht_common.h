
#ifndef _BTUI_COMMOM_H_
#define _BTUI_COMMOM_H_

#include "bt_ui_ht_trace.h"

#include "bt_ui_app.h"


//////////////////////////////////////////////////////////////////////////

/* Some utility macros. */
#define BTUI_ReleaseObjIf(po)	do { if(po) {IBASE_Release((IBase*)po); po=NULL;} } while(0)

//////////////////////////////////////////////////////////////////////////
#define BTUI_OffsetOf(s, m)	(size_t)(&(((s *)0)->m))

//////////////////////////////////////////////////////////////////////////
#define BTUI_IS_VALID_ASCII(c)	( ((c) > 0x20) && ((c) < 0x7e) )
#define BTUI_IS_SPACE(c)			( (c) == ' ' )

//////////////////////////////////////////////////////////////////////////

#define BTUI_SUCCESS(val)		((val) ? FALSE: TRUE)

#define BTUI_ASSERT(_Expression)		do { if(!(_Expression)) DBGPRINTF(#_Expression); } while(0)
#define BTUI_ASSERTEX(_Expression, label)	do { if(!(_Expression)) { DBGPRINTF(#_Expression); goto label; } } while(0)

#define BTUI_GotoFailed(ret, val, label)	do {	ret = val;	goto label;	}	while(0)

//////////////////////////////////////////////////////////////////////////

//#define	_BTUI_DEBUG_ 
#ifdef	_BTUI_DEBUG_

#define BTUI_Free(p)	do {DBGPRINTF("BTUI_Free 0x%08X", p); FREE(p);} while(0)
#define BTUI_FreeIf(p)	do {DBGPRINTF("BTUI_FreeIf 0x%08X", p); FREEIF(p);} while(0)

__inline void *Btui_Malloc(int size, char *pszFile, int iLine)
{
	void *p = MALLOC(size);
	if(p)	{	
		__DBGPRINTF("%s, %d", pszFile, iLine);
		__DBGPRINTF("BTUI_Malloc: 0x%08X", p);
	}	
	return p;
}

#define BTUI_Malloc(size)		Btui_Malloc(size, __FILE__, __LINE__)
#define BTUI_Realloc(p, size)	do {DBGPRINTF("Enter BTUI_Realloc, 0x%08X", p); \
											REALLOC(p, size); DBGPRINTF("Left BTUI_Realloc, %x", p);}while(0)
#else	// _BTUI_DEBUG_

	#define BTUI_Free(p)					FREE(p)
	#define BTUI_FreeIf(p)				do{ if(p) { BTUI_Free((p)); (p)=NULL; } }while(0)
	#define BTUI_Malloc					MALLOC
	#define BTUI_Realloc					REALLOC

#endif	// _BTUI_DEBUG_

//////////////////////////////////////////////////////////////////////////

/* The applet handle. */
extern tBtuiApp *g_pBtuiApp;

#define BT_UI_MENU_DRAW_BOTTOMBAR(x)                           \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = x;                         \
    DrawBottomBar(bt_ui_get_display(), &BarParam);      \
}
//////////////////////////////////////////////////////////////////////////

/* bt_ui_str_cpyn. */
__inline void bt_ui_str_cpyn(char *pDest, int cbCpy, char *pSrc)
{
	while(cbCpy-- > 0)
	{
		if('\0' == *pSrc)
			break;

		*pDest++ = *pSrc++;		
	}

	*pDest = 0;
}

/* bt_ui_wstr_cpyn. */
__inline void bt_ui_wstr_cpyn(AECHAR *pDest, int cbCpy, AECHAR *pSrc)
{
	while(cbCpy-- > 0)
	{
		if('\0' == *pSrc)
			break;

		*pDest++ = *pSrc++;		
	}

	*pDest = 0;
}

//////////////////////////////////////////////////////////////////////////

/* bt_ui_get_shell. */
__inline IShell* bt_ui_get_shell(void)
{	
	//return g_pBtuiApp ? g_pBtuiApp->m_pIShell : NULL;
	return g_pBtuiApp->m_pIShell;
}

/* bt_ui_get_display. */
__inline IDisplay* bt_ui_get_display(void)
{	
	//return g_pBtuiApp ? g_pBtuiApp->m_pIDisplay : NULL;
	return g_pBtuiApp->m_pIDisplay;
}

//////////////////////////////////////////////////////////////////////////
/* bt_ui_get_shell. */
__inline IFileMgr* bt_ui_get_FileMgr(void)
{	
	//return g_pBtuiApp ? g_pBtuiApp->m_pIFileMgr : NULL;
	return g_pBtuiApp->m_pIFileMgr;
}

__inline IStatic* bt_ui_get_static(void)
{	
	//return g_pBtuiApp ? g_pBtuiApp->m_pIFileMgr : NULL;
	return g_pBtuiApp->m_pStatic;
}
#ifndef AEE_SIMULATOR
__inline IAnnunciator* bt_ui_get_IAnn(void)
{	
	return g_pBtuiApp->m_pIAnn;
}
#endif
/* bt_ui_open_file */
__inline IFile* bt_ui_open_file(const char *pszFile, OpenFileMode mode)
{
	IFile *pif = NULL;
	IFileMgr* pifmgr = bt_ui_get_FileMgr();

	if(pifmgr)
		pif = IFILEMGR_OpenFile(pifmgr, pszFile, mode);	

#ifdef _BTUI_DEBUG_
	if(NULL == pif)
	{
		int nErr = IFILEMGR_GetLastError(pifmgr);
		BCM_MSG_MED("bt_ui_open_file failed, error code: %d\n", nErr,0,0);
	}
#endif
	
	return pif;
}
/* bt_ui_remove_file */
__inline int bt_ui_remove_file(const char *pszFile)
{
	int  err = EFAILED;
	IFileMgr* pifmgr = bt_ui_get_FileMgr();

	if(pifmgr)
	{   
		err = IFILEMGR_Remove (pifmgr, pszFile);	
	}
#ifdef _BTUI_DEBUG_
	if(EFAILED == err)
	{
		int nErr = IFILEMGR_GetLastError(pifmgr);
		BCM_MSG_MED("bt_ui_open_file failed, error code: %d\n", nErr,0,0);
	}
#endif
	return err;
}
/* bt_ui_create_file */
__inline IFile* bt_ui_create_file(const char *pszFile)
{
	IFile *pif = NULL;
	IFileMgr* pifmgr = bt_ui_get_FileMgr();

	if(pifmgr)
		pif = IFILEMGR_OpenFile(pifmgr, pszFile, _OFM_CREATE);	
	
	return pif;
}

//////////////////////////////////////////////////////////////////////////
/* bt_ui_close_applet. */
__inline void bt_ui_close_applet(void)
{
	if(g_pBtuiApp)	
		ISHELL_CloseApplet(g_pBtuiApp->m_pIShell, FALSE);
#ifdef FEATURE_KEYGUARD
        if(OEMKeyguard_IsEnabled())
        {
            OEMKeyguard_SetState(FALSE);
            bt_ui_cb.b_auto_keyguard = TRUE;
        }
#endif
}

//////////////////////////////////////////////////////////////////////////
/* bt_ui_get_client. */
__inline void bt_ui_get_client(AEERect *prcClient)
{
	// Get the client rectangle from the app data struct.
	if(g_pBtuiApp && prcClient)
		MEMCPY(prcClient, &g_pBtuiApp->m_rcClient, sizeof(AEERect));
}
#ifdef AEE_SIMULATOR
/* bt_ui_get_msgbox_client. */
__inline void bt_ui_get_msgbox_client(AEERect *prcClient)
{
	// Get the messgebox client rectangle from the app data struct.
	//if(g_pBtuiApp && prcClient)
		MEMCPY(prcClient, &g_pBtuiApp->m_rcMsgBox, sizeof(AEERect));
}

/* bt_ui_get_title_rect. */
__inline void bt_ui_get_title_rect(AEERect *prcTitle)
{
	// Set the title rectangle.
	//if(g_pBtuiApp && prcTitle)
		SETAEERECT(prcTitle, 0, 0, g_pBtuiApp->m_cxScreen, g_pBtuiApp->m_cyTitle);
}

/* bt_ui_get_bottom_rect. */
__inline void bt_ui_get_bottom_rect(AEERect *prcBottom)
{
	// Set the bottom rectangle.
	//if(g_pBtuiApp && prcBottom)
	{	
		SETAEERECT(prcBottom, 
			0, g_pBtuiApp->m_nButtonLine, 
			g_pBtuiApp->m_cxScreen, g_pBtuiApp->m_cyTitle);
	}	
}

/* bt_ui_get_cyLine. */
__inline uint16 bt_ui_get_cyLine(void)
{	
	//if(g_pBtuiApp)
		return g_pBtuiApp->m_cyLine;
}
#endif
//////////////////////////////////////////////////////////////////////////
/* bt_ui_get_win_stack. */
__inline tHT_WIN_STACK* bt_ui_get_win_stack(void)
{
	// Get the window stack pointer from the app data struct.
	//return g_pBtuiApp ? g_pBtuiApp->m_pWinStack : NULL;
	return g_pBtuiApp->m_p_win_stack;
}

//////////////////////////////////////////////////////////////////////////
/* bt_ui_get_inq_db. */
__inline tBTUI_INQ_DB* bt_ui_get_inq_db(void)
{
	//return g_pBtuiApp ? &g_pBtuiApp->m_inq_db : NULL;
	//return &g_pBtuiApp->m_inq_db;
	return &bt_ui_inq_db;
}

/* bt_ui_get_device_db. */
__inline tBTUI_DEV_DB* bt_ui_get_device_db(void)
{
	//return g_pBtuiApp ? &g_pBtuiApp->m_device_db : NULL;
	//return &g_pBtuiApp->m_device_db;	
	return &bt_ui_device_db;
}

/* bt_ui_get_control_block. */
//__inline tBTUI_CB* bt_ui_get_control_block(void)
//{
	//return g_pBtuiApp ? &g_pBtuiApp->m_bt_ui_ctrl_block : NULL;
	//return &g_pBtuiApp->m_bt_ui_ctrl_block;
//	return &bt_ui_cb;
//}

/* bt_ui_get_cfg. */
//__inline tBTUI_CFG* bt_ui_get_cfg(void)
//{
//	//return g_pBtuiApp ? &g_pBtuiApp->m_bt_ui_cfg : NULL;
//	//return &g_pBtuiApp->m_bt_ui_cfg;
//	return &bt_ui_cfg;
//}

/* bt_ui_get_suspend. */
__inline boolean  bt_ui_get_suspend(void)
{
	return g_pBtuiApp->m_bsuspend;
}

//////////////////////////////////////////////////////////////////////////
/* bt_ui_get_common_wbuffer. */
__inline TCHAR* bt_ui_get_comm_wbuffer(int *pLen)
{
	//if(g_pBtuiApp)
	{
		if(pLen)
			*pLen = sizeof(g_pBtuiApp->m_szWCommBuf) / sizeof(g_pBtuiApp->m_szWCommBuf[0]);

		return g_pBtuiApp->m_szWCommBuf;
	}
		
	return NULL;
}

/* bt_ui_get_common_buffer. */
__inline char* bt_ui_get_comm_buffer(int *pLen)
{
	//if(g_pBtuiApp)
	{
		if(pLen)
			*pLen = sizeof(g_pBtuiApp->m_szCommBuf);

		return g_pBtuiApp->m_szCommBuf;
	}
		
	return NULL;
}

/* bt_ui_get_common_buffer. */
//__inline char* bt_ui_get_trace_buffer(int *pLen)
//{
//	//if(g_pBtuiApp)
//	{
//		if(pLen)
//			*pLen = sizeof(g_pBtuiApp->m_szTrace);
//
//		return g_pBtuiApp->m_szTrace;
//	}
//		
//	return NULL;
//}

//////////////////////////////////////////////////////////////////////////
/* bt_ui_get_menu_item_buffer. */
__inline tHT_MENU_ITEM* bt_ui_get_menu_item_buffer(int *pLen)
{
	//if(g_pBtuiApp)
	{
		MEMSET(g_pBtuiApp->m_menu_item_buffer, 0, sizeof(g_pBtuiApp->m_menu_item_buffer));

		if(pLen)
			*pLen = sizeof(g_pBtuiApp->m_menu_item_buffer) / sizeof(tHT_MENU_ITEM);

		return g_pBtuiApp->m_menu_item_buffer;
	}
		
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
/* bt_ui_get_dev_alias. */
__inline const char *bt_ui_get_dev_alias(tBTUI_REM_DEVICE *pd)
{
#define BTUI_DEV_NO_ALIAS	"<NO NAME>"	
	return pd->short_name[0] ? pd->short_name : (pd->name[0] ? pd->name : BTUI_DEV_NO_ALIAS);
}

//////////////////////////////////////////////////////////////////////////
#ifdef AEE_SIMULATOR
#define	EVT_BLUETOOTH_EVENT		(EVT_USER+0x80)
#endif
typedef enum
{
	BTUI_DM_MSG_TYPE = 0,
	BTUI_AG_MSG_TYPE,
	BTUI_OPS_MSG_TYPE,
	BTUI_OPC_MSG_TYPE,
	BTUI_FTS_MSG_TYPE,
	BTUI_CTP_MSG_TYPE
}BTUI_MSG_TYPE;

/* mmi message buffer functions. */
tBTUI_BTA_MSG* bt_ui_get_msg(void);
void bt_ui_free_msg(tBTUI_BTA_MSG* pMsg);
void bt_ui_send_msg(BTUI_MSG_TYPE msg_type, tBTUI_BTA_MSG* pMsg);
void bt_ui_dispatch(void *pData);
void bt_ui_handle(tBTUI_BTA_MSG* pMsg, BTUI_MSG_TYPE msg_type);


//////////////////////////////////////////////////////////////////////////
#ifdef AEE_SIMULATOR
/* Alpha the display with RGB_BLACK(r=0, g=0, b=0) except the bottom line. */
void bt_ui_AlphaDisplay(void);

/* GradientDecorator helper for filling a rectangle with a gradient. */
void bt_ui_FillRectGradient(const AEERect *prc, 
									RGBVAL rgbStart, 
									RGBVAL rgbEnd, 
									boolean bVertical, 
									boolean bCenter
									);

#endif
#endif	// _BTUI_COMMOM_H_