#ifndef _BTUI_HT_WIN_
#define _BTUI_HT_WIN_

#include "bt_ui_int.h"

//////////////////////////////////////////////////////////////////////////
typedef struct _tHT_WIN	tHT_WIN;

/* window handler callback prototype. */
typedef boolean (*PFNWINHANDLER)(tHT_WIN *p_win, AEEEvent evt, uint16 wParam, uint32 dwParam);

/* window enabled or disabled callback prototype. */
typedef void (*PFNENABLE)(tHT_WIN *p_win, boolean bEnable);

typedef struct _chang_name_string
{
    AECHAR  title[32];
    AECHAR  old_name[BTUI_DEV_NAME_LENGTH+1];
}chang_name_string;

/*title and softkey name length*/
#define MAX_BT_TITLE_LEN	16
#define MAX_SK_LEN		8

/*window type*/
typedef enum
{
	HT_MENU = 0,
	HT_MENU_OPTION,
	HT_EDIT,
	HT_EDIT_NAME,
	HT_MSGBOX,
	HT_USER_WIN,
	HT_STATIC,
	HT_PROC,
	HT_INVALID		
}_tHT_WIN_TYPE;

/*struct of btui window*/
struct _tHT_WIN
{
	uint32 win_handle;

	tHT_KEY_PRESSED_CBACK *pfnKeyPressCB;/*Key press callback*/

	PFNWINHANDLER pfnHandleEvent;/*Not use*/
	PFNENABLE pfnEnable;                  /*Not use*/

	AECHAR sz_title[MAX_BT_TITLE_LEN + 1];
	//AECHAR sz_sk_left[MAX_SK_LEN + 1];
	//AECHAR sz_sk_right[MAX_SK_LEN + 1];
	BottomBar_e_Type win_sk;
	uint8 msg_type;
	//AECHAR sz_sk_middle[MAX_SK_LEN + 1];

	uint16 win_type;/*windows type*/

	uint32 win_data;/*windows show time*/
	void * data;
};

/*struct of btui windows stack*/
typedef struct tHT_WIN_STACK
{
	tHT_WIN *p_win_array;
	uint16 stack_size;
	uint16 stack_pointer;
//	uint16 active_win;

	boolean f_draw_hold_down;
	boolean f_need_draw;
	boolean f_display_is_alpha;
}tHT_WIN_STACK;

/*tHT_WIN_STACK functions*/
tHT_WIN_STACK* HtWinStack_New(void);

void HtWinStack_Delete(tHT_WIN_STACK *p_win_stack);

void HtWinStack_Clear(tHT_WIN_STACK *p_win_stack);

boolean HtWinStack_HandleEvent(tHT_WIN_STACK *p_win_stack, AEEEvent evt, uint16 wParam, uint32 dwParam);

void HtWinStack_Draw(tHT_WIN_STACK *p_win_stack);

void HtWinStack_SetDrawHoldDown(tHT_WIN_STACK *p_win_stack, boolean f_hold_down);
#ifdef AEE_SIMULATOR
void HtWin_DrawBottomLineEx(tHT_WIN *p_win);
#endif
tHT_WIN* HtWinStack_FindWindowByHandle(tHT_WIN_STACK *p_win_stack, uint32 win_handle, uint16 *p_win_pos);

tHT_WIN* HtWinStack_FindWindowByHandleEx(tHT_WIN_STACK *p_win_stack, uint32 win_handle);

tHT_WIN* HtWinStack_FindWindowByPos(tHT_WIN_STACK *p_win_stack, uint16 win_pos);

tHT_WIN* HtWinStack_GetActiveWin(tHT_WIN_STACK *p_win_stack, uint16 *p_win_pos);

boolean HtWinStack_IsActiveHandle(tHT_WIN_STACK *p_win_stack, uint32 handle);

//int HtWinStack_Push(tHT_WIN_STACK *p_win_stack, tHT_WIN *p_win);

int HtWinStack_PushEx(	tHT_WIN_STACK *p_win_stack, 	
								uint32 win_handle,
								uint16 win_type,
								uint32 win_data,
								AECHAR *pw_title,
								PFNWINHANDLER pfnHandleEvent,
								PFNENABLE pfnEnable,
								tHT_KEY_PRESSED_CBACK *pfnKeyPressCB,
								BottomBar_e_Type soft_key,
								uint8 msg_type,
								void *data
								);

void HtWinStack_Pop(tHT_WIN_STACK *p_win_stack);

void HtWinStack_Remove(tHT_WIN_STACK *p_win_stack, uint32 win_handle);

void HtWinStack_RemovePromptWindow(tHT_WIN_STACK *p_win_stack);

//void HtWinStack_PostEvent(tHT_WIN_STACK *p_win_stack, 
//								  uint32 from_win_handle,
//								  uint32 to_win_handle,
//								  AEEEvent evt, 
//								  uint16 wParam, 
//								  uint32 dwParam);

//void HtWinStack_SendEvent(tHT_WIN_STACK *p_win_stack, 
//								  uint32 from_win_handle,
//								  uint32 to_win_handle,
//								  AEEEvent evt, 
//								  uint16 wParam, 
//								  uint32 dwParam);

void HtWinStack_Enable(tHT_WIN_STACK *p_win_stack, boolean bEnable);

__inline _tHT_WIN_TYPE HtWinStack_GetWindowType(tHT_WIN* p_win)
{
	return p_win->win_type;
}
#ifdef FEATURE_BT_SHOW_OPP_BITRATE
void HtWinStack_Draw_bitrate(uint32 bytes_transferred,boolean is_reset);
#endif
#endif	// _BTUI_HT_WIN_