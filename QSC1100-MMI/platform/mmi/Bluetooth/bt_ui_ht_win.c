
#include "bt_ui_ht_win.h"
#include "bt_ui_ht_common.h"


/*About window stack size*/
#define HT_WIN_STACK_PRIME_SIZE 16
#define HT_WIN_STACK_INC_SIZE       16
#ifdef FEATURE_BT_SHOW_OPP_BITRATE
typedef struct _bt_ui_bitrate
{
    uint32 receive_bit;
    uint32 time;
}bt_ui_bitrate;
static bt_ui_bitrate bt_ui_bitrate_ee;
#endif
#ifdef UI_DM_EDIT_NAME_EX
static void  HtWinStack_DrawIMEIcon(ITextCtl *pTextCtl, IDisplay *pIDisplay,IShell   *pIShell);
#endif
//////////////////////////////////////////////////////////////////////////
/* Find the window by handle and get window pos. */
tHT_WIN* HtWinStack_FindWindowByHandle(tHT_WIN_STACK *p_win_stack, uint32 win_handle, uint16 *p_win_pos)
{
    BCM_WIN_MSG("HtWinStack_FindWindowByHandle %x %x %x",p_win_stack,win_handle,p_win_pos);
    if(NULL != p_win_stack)
    {       
        uint16 i = p_win_stack->stack_pointer;
        while(i-- > 0)
        {
            if(p_win_stack->p_win_array[i].win_handle == win_handle)
            {
                if(p_win_pos)
                    *p_win_pos = i + 1;

                return &p_win_stack->p_win_array[i];
            }
        }
    }

    return NULL;
}

/* Find the window by handle. */
tHT_WIN* HtWinStack_FindWindowByHandleEx(tHT_WIN_STACK *p_win_stack, uint32 win_handle)
{
    BCM_WIN_MSG("HtWinStack_FindWindowByHandleEx %x %x",p_win_stack,win_handle,0);
    return HtWinStack_FindWindowByHandle(p_win_stack, win_handle, NULL);
}

/* Find the window handle by window pos. */
tHT_WIN* HtWinStack_FindWindowByPos(tHT_WIN_STACK *p_win_stack, uint16 win_pos)
{
    BCM_WIN_MSG("HtWinStack_FindWindowByPos %x %x",p_win_stack,win_pos,0);
    if(p_win_stack)
    {
        if((win_pos > 0) && (win_pos <= p_win_stack->stack_pointer))
            return &p_win_stack->p_win_array[win_pos-1];
    }   

    return NULL;
}

/* Get the active window. */
tHT_WIN* HtWinStack_GetActiveWin(tHT_WIN_STACK *p_win_stack, uint16 *p_win_pos)
{
    BCM_WIN_MSG("HtWinStack_GetActiveWin %x %x",p_win_stack,p_win_pos,0);
    if(p_win_stack && (p_win_stack->stack_pointer > 0))
    {
        if(p_win_pos)
            *p_win_pos = p_win_stack->stack_pointer;
        
        return &(p_win_stack->p_win_array[p_win_stack->stack_pointer-1]);
    }

    return NULL;
}

/* Find the window, return the stack pos of the window. */
uint16 HtWinStack_FindWindow(tHT_WIN_STACK *p_win_stack, tHT_WIN *p_win)
{
    uint16 i = p_win_stack->stack_pointer;
    BCM_WIN_MSG("HtWinStack_FindWindow %x %x",p_win_stack,p_win,0);
    while(i-- > 0)
    {
        if(&p_win_stack->p_win_array[i] == p_win)
        {
            return (i + 1);
        }
    }

    return i;
}

/* Check if the handle is active window's handle. */
boolean HtWinStack_IsActiveHandle(tHT_WIN_STACK *p_win_stack, uint32 handle)
{
    BCM_WIN_MSG("HtWinStack_IsActiveHandle %x %x",p_win_stack,handle,0);
    if(p_win_stack && (p_win_stack->stack_pointer > 0))
        return (handle == p_win_stack->p_win_array[p_win_stack->stack_pointer-1].win_handle);

    return FALSE;
}
#ifdef AEE_SIMULATOR
//////////////////////////////////////////////////////////////////////////
/* Draw the window top line. */
static void HtWin_DrawTopLine(tHT_WIN *p_win, IDisplay *pids)
{
    AEERect rcTitle;

#define BTUI_TITLE_RGB_S    MAKE_RGB(240, 240, 255)
#define BTUI_TITLE_RGB_E    MAKE_RGB(96, 160, 212)
    BCM_WIN_MSG("HtWin_DrawTopLine %x %x",p_win,pids,0);
    bt_ui_get_title_rect(&rcTitle);
    bt_ui_FillRectGradient(&rcTitle, BTUI_TITLE_RGB_S, BTUI_TITLE_RGB_E, FALSE, FALSE);
    DBGPRINTF("HtWin_DrawTopLine %S",p_win->sz_title);
    if(p_win->sz_title[0])
    {   
        IDISPLAY_DrawText(pids, 
                                AEE_FONT_NORMAL, 
                                p_win->sz_title, 
                                -1, 
                                rcTitle.x, 
                                rcTitle.y, 
                                &rcTitle, 
                                IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE|IDF_TEXT_TRANSPARENT
                                );
    }   
}
#endif
/* Draw the window bottom line. */
void HtWin_DrawBottomLine(tHT_WIN *p_win, IDisplay *pids)
{
#if 1
    if(BTBAR_NONE != p_win->win_sk)
    {
        BT_UI_MENU_DRAW_BOTTOMBAR(p_win->win_sk);
    }
    IDISPLAY_UpdateEx(pids, TRUE);

#else
    AEERect rcBottom;

// #define BTUI_BOTTOM_RGB_S    MAKE_RGB(240, 240, 240)
// #define BTUI_BOTTOM_RGB_E    MAKE_RGB(160, 160, 160)
    BCM_WIN_MSG("HtWin_DrawBottomLine %x %x",p_win,pids,0);
    bt_ui_get_bottom_rect(&rcBottom);
    bt_ui_FillRectGradient(&rcBottom, BTUI_TITLE_RGB_E, BTUI_TITLE_RGB_S, FALSE, FALSE);

    rcBottom.x += 1;
    rcBottom.dx -= 1;
    
    if(p_win->sz_sk_left[0])
    {   
        IDISPLAY_DrawText(pids, 
                                AEE_FONT_NORMAL, 
                                p_win->sz_sk_left, 
                                -1, 
                                rcBottom.x, 
                                rcBottom.y, 
                                &rcBottom, 
                                IDF_ALIGN_LEFT|IDF_ALIGN_MIDDLE|IDF_TEXT_TRANSPARENT
                                );
    }
    
    if(p_win->sz_sk_right[0])
    {   
        IDISPLAY_DrawText(pids, 
                                AEE_FONT_NORMAL, 
                                p_win->sz_sk_right, 
                                -1, 
                                rcBottom.x, 
                                rcBottom.y, 
                                &rcBottom, 
                                IDF_ALIGN_RIGHT|IDF_ALIGN_MIDDLE|IDF_TEXT_TRANSPARENT
                                );
    }
#endif
}
#ifdef AEE_SIMULATOR
/* Draw the window bottom line and update the display. */
void HtWin_DrawBottomLineEx(tHT_WIN *p_win)
{
    IDisplay    *pids = bt_ui_get_display();
    BCM_WIN_MSG("HtWin_DrawBottomLineEx %x",p_win,0,0);
    if(pids)
    {   
        HtWin_DrawBottomLine(p_win, pids);  
        //IDISPLAY_UpdateEx(pids, TRUE);
    }   
}
#endif
/* The message box timer funtion. */
void HtMsgBox_Timer(void *pData)
{
    tHT_WIN *p_win = (tHT_WIN *)pData;
    if(p_win)
    {
        BCM_WIN_MSG("HtMsgBox_Timer %x",p_win->pfnKeyPressCB,0,0);
        if(p_win->pfnKeyPressCB)
        {
            p_win->pfnKeyPressCB(p_win->win_handle, HT_KEY_SOFT2);
        }
        else
        {
            bt_ui_close_object(p_win->win_handle);
            if(bt_ui_cb.after_resume_menu != BT_UI_NULL_HANDLE)
            {
                tHT_WIN_STACK *stack = bt_ui_get_win_stack();
                if(stack != NULL)
                {
                    BCM_WIN_MSG("HtMsgBox_Timer %x %x ",stack->stack_size,stack->stack_pointer,0);
                    if(stack->stack_pointer == 0)
                    {
                        bt_ui_cb.after_resume_menu();
                    }
                }
                bt_ui_cb.after_resume_menu = BT_UI_NULL_HANDLE;
            }
        }
    }   
}
#ifdef FEATURE_BT_SHOW_OPP_BITRATE
void HtWinStack_Draw_bitrate(uint32 bytes_transferred,boolean is_reset)
{
    if(is_reset)
    {
        bt_ui_bitrate_ee.receive_bit = 0;
        bt_ui_bitrate_ee.time = 0;
    }
    else
    {
        //only for test bitrate
        uint32 current_time = 0;
        uint32 bitrate = 0;
        AECHAR buf[30]= {0};
        AEERect rect = {0};

        bt_ui_get_client(&rect);
        SETAEERECT(&rect, rect.x + 5, 95, rect.dx-10, 15);
        bt_ui_bitrate_ee.receive_bit = bytes_transferred - bt_ui_bitrate_ee.receive_bit;
        current_time = GETUPTIMEMS();
        if(bt_ui_bitrate_ee.time != current_time)
        {
            bitrate = (bt_ui_bitrate_ee.receive_bit*1000)/((current_time-bt_ui_bitrate_ee.time)*1024);
        }
        BCM_MSG_MED("bt_ui_bitrate_ee %d %d %u",bt_ui_bitrate_ee.receive_bit,current_time,bitrate);
        bt_ui_bitrate_ee.time = current_time;
        bt_ui_bitrate_ee.receive_bit = bytes_transferred;
        WSPRINTF(buf, sizeof(buf), L"%d kb/s", bitrate);
        
        IDISPLAY_DrawText(bt_ui_get_display(), AEE_FONT_NORMAL, buf, -1, 
            0, 0, &rect, IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE);
        //only for test bitrate
    }
}
#endif
static void HtWin_DrawProc(IDisplay *pids, AECHAR *pMsgText,tBTUI_OPP_PROC_VAL *data)
{
    AEERect rect = {0};
    AEERect rect_frame ;
    AEERect rect_fill ;
    uint32 rect_fill_dx = 0;
    AECHAR buf[30]= {0};
    bt_ui_get_client(&rect);
    IDISPLAY_FillRect(pids,&rect,MAKE_RGB(255, 255, 255));
    IDISPLAY_DrawText(pids, AEE_FONT_NORMAL, pMsgText, -1, 
            0, 50, NULL, IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER);
    
    SETAEERECT(&rect_frame, rect.x + 5, 75, rect.dx-10, 15);
    if(data->obj_size == 0)
    {
        rect_fill_dx = rect_frame.dx;
    }
    else
    {
        rect_fill_dx = (data->bytes_transferred*rect_frame.dx)/data->obj_size;
        WSPRINTF(buf, sizeof(buf), L"%d /%d", data->bytes_transferred,data->obj_size);
    }
    SETAEERECT(&rect_fill, rect.x + 5, 75, rect_fill_dx, 15);
    IDISPLAY_DrawRect(pids, &rect_frame, RGB_BLACK, RGB_WHITE, IDF_RECT_FRAME/*|IDF_RECT_FILL*/);
    IDISPLAY_FillRect(pids,&rect_fill,MAKE_RGB(111, 183, 255));
    if(buf[0] != 0)
    {
        IDISPLAY_DrawText(pids, AEE_FONT_NORMAL, buf, -1, 
            0, 0, &rect_frame, IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE);
    }
#ifdef FEATURE_BT_SHOW_OPP_BITRATE
    HtWinStack_Draw_bitrate(data->bytes_transferred,FALSE);
#endif
}
/* Draw the message box. */
static void HtWin_DrawMsgBox(IDisplay *pids, AECHAR *pMsgText, tHT_WIN_STACK *p_win_stack,BottomBar_e_Type soft_key, uint8 msg_type)
{
#ifndef AEE_SIMULATOR
    IStatic * pStatic = NULL;
    PromptMsg_e_Type ePMsgType = MESSAGE_NONE;
    PromptMsg_Param_type  Msg_Param={0};
    AEERect rect = {0};
#if 0 
    pStatic = bt_ui_get_static();
    //ISTATIC_SetActive(pStatic, TRUE);
#else
    tHT_WIN * p_win = NULL;
    ISHELL_CreateInstance(bt_ui_get_shell(),AEECLSID_STATIC, (void **)&pStatic);
    p_win = HtWinStack_FindWindowByHandleEx(p_win_stack, (uint32)pMsgText);
    p_win->data = pStatic;
#endif

    if (NULL == pStatic)
    {
        return;
    }
    if(msg_type == 0)
    {
        ePMsgType= MESSAGE_INFORMATIVE;
    }
    else
    {
        ePMsgType= msg_type;
    }
    ISTATIC_SetRect(pStatic, &rect);  
    Msg_Param.ePMsgType = ePMsgType;
    Msg_Param.pwszMsg = pMsgText;
    Msg_Param.eBBarType = soft_key;
    DrawPromptMessage(pids, pStatic, &Msg_Param);
    IDISPLAY_UpdateEx(pids, FALSE);
#else
    AEERect rcClient;
    AECHAR *pszText, *pszBreak;
    int nFits, nChars, nLines;  
    int16 cxClient;
    uint16 cyLine;
    BCM_WIN_MSG("HtWin_DrawMsgBox %x %x %x",pMsgText,p_win_stack,p_win_stack->f_display_is_alpha );
    if(!p_win_stack->f_display_is_alpha)
    {
        p_win_stack->f_display_is_alpha = TRUE;
        bt_ui_AlphaDisplay();
    }

    bt_ui_get_msgbox_client(&rcClient);

    IDISPLAY_DrawRect(pids, &rcClient, RGB_BLACK, RGB_WHITE, IDF_RECT_FRAME/*|IDF_RECT_FILL*/);
    rcClient.y += 1;
    rcClient.dy -= 2;
    rcClient.x += 1;
    rcClient.dx -= 2;

#define BTUI_MSGBOX_RGB_S   MAKE_RGB(240, 240, 255)
#define BTUI_MSGBOX_RGB_E   MAKE_RGB(144, 144, 160)
    bt_ui_FillRectGradient(&rcClient, BTUI_MSGBOX_RGB_S, BTUI_MSGBOX_RGB_E, TRUE, FALSE);

    cyLine = bt_ui_get_cyLine();

    rcClient.y += 1;
    rcClient.dy -= 2;
    rcClient.x += 1;
    rcClient.dx -= 2;

    nLines = (rcClient.dy)/cyLine;
    rcClient.dy = cyLine;

    cxClient = rcClient.dx; 

    pszText = pMsgText;
    nChars = WSTRLEN(pMsgText);
    while( (nChars > 0) && (nLines > 0) )
    {
        IDISPLAY_MeasureTextEx(pids, AEE_FONT_NORMAL, pszText, nChars, cxClient, &nFits);
            
        if(0 == nFits)  
            break;

        pszBreak = pszText + nFits;
        if((nFits < nChars) && BTUI_IS_VALID_ASCII(*pszBreak))
        {
            do {
                pszBreak--;
            }while((pszBreak > pszText) && BTUI_IS_VALID_ASCII(*pszBreak));
            
            if(pszBreak > pszText)
                nFits = pszBreak - pszText + 1;
        }   
        
        IDISPLAY_DrawText(pids, AEE_FONT_NORMAL, pszText, nFits, 
            rcClient.x, rcClient.y, &rcClient, IDF_TEXT_TRANSPARENT);
        rcClient.y += cyLine;
        
        pszText += nFits;
        nChars -= nFits;
        nLines--;       
    }
    BT_UI_MENU_DRAW_BOTTOMBAR(soft_key)
    IDISPLAY_UpdateEx(pids, FALSE);
#endif
}
#ifdef UI_DM_EDIT_NAME_EX
static void HtWinStack_Draw_change_name(IDisplay *pIDisplay,tHT_WIN *p_win)
{
    //AECHAR w_old_name[BTUI_DEV_NAME_LENGTH] = {0};
    //AECHAR w_old_name_tit[30] = {0};
    AEERect rect;
    AEERect rcClient;
    TitleBar_Param_type title;
    ITextCtl *pTextCtl = (ITextCtl *)p_win->win_handle;
    IShell   *pIShell = bt_ui_get_shell();
    chang_name_string *name_string = (chang_name_string*)p_win->data;
    int title_hi = GetTitleBarHeight(bt_ui_get_display());
    if(NULL == p_win ||NULL == p_win->data || NULL == p_win->win_handle)
    {
        return;
    }
    ITEXTCTL_Redraw((ITextCtl *)(p_win->win_handle));
    HtWinStack_DrawIMEIcon(pTextCtl,pIDisplay,pIShell);

    //ISHELL_LoadResString(pIShell, AEE_BLUETOOTH_RES_FILE, IDS_CURRENT_NAME, w_old_name_tit, sizeof(w_old_name_tit)) ;
    bt_ui_get_client(&rcClient);
    SETAEERECT(&rect, rcClient.x, rcClient.y, rcClient.dx, title_hi*2 +20);
    IDISPLAY_EraseRect(pIDisplay,&rect);
#ifdef IDS_CURRENT_NAME
    SETAEERECT(&rect, rcClient.x, rcClient.y, rcClient.dx, title_hi);
    MEMSET(&title,0,sizeof(TitleBar_Param_type));
    title.dwAlignFlags = IDF_TEXT_TRANSPARENT | IDF_ALIGN_LEFT | IDF_ALIGN_MIDDLE;
    title.pwszTitle =  bt_ui_get_brx_string(IDS_CURRENT_NAME);
    title.prc = &rect;
    DrawTitleBar(pIDisplay,&title);
#endif
    SETAEERECT(&rect, rcClient.x, rcClient.y+title_hi, rcClient.dx, 20);
    //STRTOWSTR(old_name,w_old_name,sizeof(w_old_name));
    //UTF8TOWSTR((byte *) old_name, BTUI_DEV_NAME_LENGTH+1, w_old_name, sizeof(w_old_name));
    IDISPLAY_DrawText(pIDisplay, AEE_FONT_NORMAL,  name_string->old_name, -1, rect.x,  rect.y, NULL,  IDF_ALIGN_CENTER);

    SETAEERECT(&rect, rcClient.x, rcClient.y+title_hi+20, rcClient.dx, title_hi);
    MEMSET(&title,0,sizeof(TitleBar_Param_type));
    title.dwAlignFlags = IDF_TEXT_TRANSPARENT | IDF_ALIGN_LEFT | IDF_ALIGN_MIDDLE;
    title.pwszTitle =  name_string->title;
    title.prc = &rect;
    DrawTitleBar(pIDisplay,&title);

    SETAEERECT(&rect, rcClient.x, rcClient.y+2*title_hi+40, rcClient.dx, rcClient.dy-20);
    IDISPLAY_EraseRect(pIDisplay,&rect);
    if ( WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) > 0 &&   ITEXTCTL_GetCursorPos(pTextCtl) != TC_CURSORSTART )
    {
        BT_UI_MENU_DRAW_BOTTOMBAR(BTBAR_OK_DELETE)
    }
    else
    {
        BT_UI_MENU_DRAW_BOTTOMBAR(BTBAR_OK_BACK)
    }

    //ITEXTCTL_Redraw((ITextCtl *)(p_win->win_handle));
    IDISPLAY_UpdateEx(pIDisplay, FALSE);
}
#endif
/* Draw the window.*/
static void HtWin_Draw(tHT_WIN *p_win, tHT_WIN_STACK *p_win_stack,BottomBar_e_Type soft_key)
{
    IDisplay    *pids = bt_ui_get_display();
    
    if(!pids || !p_win->win_handle)
        return; 
    BCM_WIN_MSG("HtWin_Draw %x %x %d",p_win,p_win_stack,p_win->win_type);
    switch(p_win->win_type)
    {
    case HT_MENU:
    case HT_MENU_OPTION:
        if(IMENUCTL_IsActive((IMenuCtl *)(p_win->win_handle)))
            IMENUCTL_Redraw((IMenuCtl *)(p_win->win_handle));
        p_win_stack->f_display_is_alpha = FALSE;
#ifdef AEE_SIMULATOR
        break; //need show softkey
#else
        return;
#endif

    case HT_EDIT:
        ITEXTCTL_Redraw((ITextCtl *)(p_win->win_handle));
        p_win_stack->f_display_is_alpha = FALSE;
        break;

    case HT_EDIT_NAME:
#ifdef UI_DM_EDIT_NAME_EX
        HtWinStack_Draw_change_name(pids,p_win);
        p_win_stack->f_display_is_alpha = FALSE;
#endif
        break;

    case HT_MSGBOX:
        HtWin_DrawMsgBox(pids, (AECHAR *)(p_win->win_handle), p_win_stack,soft_key,p_win->msg_type);
        return;

    case HT_USER_WIN:
        p_win_stack->f_display_is_alpha = FALSE;
        break;
        
    case HT_STATIC:
        ISTATIC_Redraw ((IStatic *)(p_win->win_handle));
        p_win_stack->f_display_is_alpha = FALSE;
        break;

    case HT_PROC:
        p_win_stack->f_display_is_alpha = FALSE;
        HtWin_DrawProc(pids,(AECHAR *)(p_win->win_handle),(tBTUI_OPP_PROC_VAL*)(p_win->win_data));
        break;

    default:
        break;
    }

    if((p_win->win_type != HT_MSGBOX) && (WSTRLEN(p_win->sz_title)!= 0))
    {   
        // draw the top line.
        //HtWin_DrawTopLine(p_win, pids);
        TitleBar_Param_type title;
        MEMSET(&title,0,sizeof(TitleBar_Param_type));
        title.dwAlignFlags = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
        title.pwszTitle = p_win->sz_title;
        DrawTitleBar(pids,&title);
    }   

    // draw the bottom line.
    HtWin_DrawBottomLine(p_win, pids);

    //IDISPLAY_UpdateEx(pids, FALSE);
}

/* destory the window handle*/
static void HtWin_Delete(tHT_WIN *p_win)
{
    if(NULL == p_win->win_handle)
        return; 
    BCM_WIN_MSG("HtWin_Delete %d",p_win->win_type,0,0);
    switch(p_win->win_type)
    {
    case HT_MENU:
    case HT_MENU_OPTION:
        IMENUCTL_Release((IMenuCtl *)(p_win->win_handle));
        break;

    case HT_EDIT:
        ITEXTCTL_Release((ITextCtl *)(p_win->win_handle));
        break;

    case HT_EDIT_NAME:
        ITEXTCTL_Release((ITextCtl *)(p_win->win_handle));
        BTUI_Free((chang_name_string*)p_win->data);
        break;

        
    case HT_STATIC:
        ISTATIC_Release((IStatic *)(p_win->win_handle));
        break;

    case HT_PROC:
        BTUI_Free((tBTUI_OPP_PROC_VAL*)p_win->win_data);
        BTUI_Free((AECHAR *)p_win->win_handle);
        break;

    case HT_MSGBOX:
        if(p_win->win_data)
            ISHELL_CancelTimer(g_pBtuiApp->m_pIShell, HtMsgBox_Timer, (void *)p_win);
#if 1          
        if(p_win->data)
            ISTATIC_Release((IStatic *)(p_win->data));
#else
        ISTATIC_SetActive(bt_ui_get_static(), FALSE);
#endif
        BTUI_Free((AECHAR *)p_win->win_handle);
        break;

    case HT_USER_WIN:
        break;

    default:
        break;
    }

    MEMSET(p_win, 0, sizeof(tHT_WIN));
}
static boolean HtStatic_HandleEvent(tHT_WIN *p_win, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
    BCM_WIN_MSG("HtStatic_HandleEvent %x %x",evt,wParam,0);
    if(EVT_KEY == evt)
    {
        switch(wParam)
        {
        case HT_KEY_SOFT1:
        case HT_KEY_SELECT:
            if(/*p_win->sz_sk_left[0] &&*/ p_win->pfnKeyPressCB)
                p_win->pfnKeyPressCB(p_win->win_handle, (uint32)wParam);
            break;

        case HT_KEY_SOFT2:
            if(/*p_win->sz_sk_right[0] &&*/ p_win->pfnKeyPressCB)
                p_win->pfnKeyPressCB(p_win->win_handle, (uint32)wParam);
            break;

        default:
            ISTATIC_HandleEvent ((IStatic *)(p_win->win_handle), EVT_KEY, wParam, dwParam);
            break;
        }
                
        return TRUE;
    }

    return FALSE;

}
/* menu window handler*/
static boolean HtMenu_HandleEvent(tHT_WIN *p_win, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
    BCM_WIN_MSG("HtMenu_HandleEvent %x %x",evt,wParam,0);
    if(EVT_KEY == evt)
    {
        switch(wParam)
        {
        case HT_KEY_SOFT1:
        case HT_KEY_SELECT:
            if(/*p_win->sz_sk_left[0] &&*/ p_win->pfnKeyPressCB)
                p_win->pfnKeyPressCB(p_win->win_handle, (uint32)wParam);
            break;

        case HT_KEY_SOFT2:
            if(/*p_win->sz_sk_right[0] &&*/ p_win->pfnKeyPressCB)
                p_win->pfnKeyPressCB(p_win->win_handle, (uint32)wParam);
            break;

        default:
            IMENUCTL_HandleEvent((IMenuCtl *)(p_win->win_handle), EVT_KEY, wParam, dwParam);
            break;
        }
                
        return TRUE;
    }
    else if(EVT_COMMAND == evt) //new add for num ->menu
    {
        //BCM_FUN_STAR("%x %x %x HtMenu_HandleEvent",p_win->win_handle,wParam,dwParam);
        if(p_win->win_handle == dwParam)
        {
            IMENUCTL_SetSel((IMenuCtl *)(p_win->win_handle), wParam);
            if( p_win->pfnKeyPressCB)
            {
                p_win->pfnKeyPressCB(p_win->win_handle, (uint32)HT_KEY_SELECT);
            }
        }
        return TRUE;
    }

    return FALSE;
}

/* edit window handler*/
static boolean HtEdit_HandleEvent(tHT_WIN *p_win, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
    AECHAR *pText;
    ITextCtl *pTextCtl = (ITextCtl *)(p_win->win_handle);
#if 1
    AECHAR szBackUp[24] = {0};
    uint16 preCurPos;
    ITEXTCTL_GetText(pTextCtl, szBackUp, 24);
    preCurPos = ITEXTCTL_GetCursorPos(pTextCtl);
    if((ITEXTCTL_HandleEvent(pTextCtl, evt, wParam, dwParam)))
    {
        switch(wParam)
        {
        	case AVK_UP:
        	case AVK_DOWN:
        	case AVK_LEFT:
        	case AVK_RIGHT:
        	case AVK_CLR:
        	break;

        	default:
        	{
        		char szTestLength[24] = {0};
        		pText = ITEXTCTL_GetTextPtr(pTextCtl);
        		WSTRTOUTF8(pText, WSTRLEN(pText), (uint8 *)szTestLength, sizeof(szTestLength));
        		if(STRLEN(szTestLength) > BTUI_DEV_NAME_LENGTH)
        		{
        			ITEXTCTL_SetText(pTextCtl, szBackUp, -1);
        			ITEXTCTL_SetCursorPos(pTextCtl, preCurPos);
        		}
        		break;
        	}
        }
        //BCM_FUN_STAR("second evt:0x%x;wParam:0x%x;dwParam:0x%x", evt, wParam, dwParam);
        return TRUE;
    }
#else
    if((pTextCtl) && (ITEXTCTL_HandleEvent(pTextCtl, evt, wParam, dwParam)))
        return TRUE;
#endif

    if(EVT_KEY == evt)
    {
        switch(wParam)
        {
        case HT_KEY_SOFT1:
        case HT_KEY_SELECT:
            if(/*p_win->sz_sk_left[0] && */p_win->pfnKeyPressCB)
            {
                p_win->pfnKeyPressCB(p_win->win_handle, (uint32)wParam);
            }
            return TRUE;

        case HT_KEY_SOFT2:  // AVK_CLR          
            // If the edit box has text and the cursor pos is not zero, 
            // we transfer the event to TextCtl.
            pText = ITEXTCTL_GetTextPtr(pTextCtl);
            if(pText && pText[0])
            {
                if(0 != ITEXTCTL_GetCursorPos(pTextCtl))
                {                           
                    ITEXTCTL_HandleEvent(pTextCtl, EVT_KEY, wParam, dwParam);
                    return TRUE;
                }   
            }
            
            if(/*p_win->sz_sk_right[0] && */p_win->pfnKeyPressCB)
            {
                p_win->pfnKeyPressCB(p_win->win_handle, (uint32)wParam);
            }
            return TRUE;

        default:
            break;
        }
    }
    else if(EVT_KEY_RELEASE == evt)
    {
        AEETextInputMode nInputMode; 
        nInputMode = ITEXTCTL_GetInputMode(pTextCtl,NULL); 
        if (nInputMode == AEE_TM_SYMBOLS)
        {
            return TRUE;
        }

        if ( WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) > 0 &&   ITEXTCTL_GetCursorPos(pTextCtl) != TC_CURSORSTART )
        {
            BT_UI_MENU_DRAW_BOTTOMBAR(BTBAR_OK_DELETE)
        }
        else
        {
            BT_UI_MENU_DRAW_BOTTOMBAR(BTBAR_OK_BACK)
        }
        IDISPLAY_UpdateEx(bt_ui_get_display(), FALSE);
        //ITEXTCTL_Redraw(pTextCtl);
    }
    else if(EVT_CTL_TEXT_MODECHANGED== evt )
    {
        BCM_MSG_MED("EVT_CTL_TEXT_MODECHANGED",0,0,0);
#ifdef UI_DM_EDIT_NAME_EX
        HtWinStack_DrawIMEIcon(pTextCtl,bt_ui_get_display(),bt_ui_get_shell());
#endif
        IDISPLAY_UpdateEx(bt_ui_get_display(), FALSE);
    }

    //if(pTextCtl)
    //    return ITEXTCTL_HandleEvent(pTextCtl, evt, wParam, dwParam);
    
    return FALSE;
}

/* messgaebox window handler*/
static boolean HtMsgBox_HandleEvent(tHT_WIN *p_win, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
    if(!p_win->pfnKeyPressCB)
        return FALSE;
    BCM_WIN_MSG("HtMsgBox_HandleEvent%x %x",evt,wParam,0);
    if(EVT_KEY == evt)
    {
        boolean bNeedHandle = FALSE;

        switch(wParam)
        {
        case HT_KEY_SOFT1:
        case HT_KEY_SELECT:
            //if(p_win->sz_sk_left[0])
            bNeedHandle = TRUE;
            break;

        case HT_KEY_SOFT2:
            //if(p_win->sz_sk_right[0])
            bNeedHandle = TRUE;
            break;
        }

        if(bNeedHandle && p_win->pfnKeyPressCB)
            p_win->pfnKeyPressCB(p_win->win_handle, (uint32)wParam);            

        return TRUE;
    }

    return FALSE;
}
static boolean HtPorcBox_HandleEvent(tHT_WIN *p_win, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
    if(EVT_KEY == evt && HT_KEY_SOFT2 == wParam)
    {
        if(p_win->pfnKeyPressCB)
        {
            p_win->pfnKeyPressCB(p_win->win_handle, (uint32)wParam);
        }
        else
        {
            bt_ui_close_object(p_win->win_handle);
        }
    }
    return TRUE;
}
/* menu window enable callback. */
static void HtMenu_Enable(tHT_WIN *p_win, boolean bEnable)
{
    IMenuCtl *pMenuCtl = (IMenuCtl *)(p_win->win_handle);
    BCM_WIN_MSG("HtMenu_Enable",0,0,0);
    IMENUCTL_SetActive(pMenuCtl, bEnable);
}

/* edit window enable callback. */
static void HtEdit_Enable(tHT_WIN *p_win, boolean bEnable)
{
    BCM_WIN_MSG("HtEdit_Enable",0,0,0);
    ITEXTCTL_SetActive((ITextCtl *)(p_win->win_handle), bEnable);
}

static void HtStatic_Enable(tHT_WIN *p_win, boolean bEnable)
{
    IStatic * pIStatic = (IStatic *)(p_win->win_handle);
    BCM_WIN_MSG("HtStatic_Enable",0,0,0);
    ISTATIC_SetActive (pIStatic, bEnable);
}
//////////////////////////////////////////////////////////////////////////
/*new a window stack*/
tHT_WIN_STACK* HtWinStack_New(void)
{
    tHT_WIN_STACK *p_win_stack;
    uint32 tmp_size;
    BCM_WIN_MSG("HtWinStack_New",0,0,0);
    tmp_size = sizeof(tHT_WIN_STACK);
    p_win_stack = (tHT_WIN_STACK *)BTUI_Malloc(tmp_size);   
    if(NULL == p_win_stack)
        return NULL;

    MEMSET(p_win_stack, 0, tmp_size);

    tmp_size = HT_WIN_STACK_PRIME_SIZE * sizeof(tHT_WIN);
    p_win_stack->p_win_array = (tHT_WIN *)BTUI_Malloc(tmp_size);
    if(NULL == p_win_stack->p_win_array)
    {
        BTUI_Free(p_win_stack);
        return NULL;
    }

    MEMSET(p_win_stack->p_win_array, 0, tmp_size);

    p_win_stack->stack_size = HT_WIN_STACK_PRIME_SIZE;

    return p_win_stack;
}


/* clear the window stack*/
void HtWinStack_Clear(tHT_WIN_STACK *p_win_stack)
{
    tHT_WIN *p_current_win, *p_win_array;
    uint16 stack_pointer;
    BCM_WIN_MSG("HtWinStack_Clear %x %x",p_win_stack->p_win_array,p_win_stack->stack_pointer,0);
    p_win_array = p_win_stack->p_win_array;
    stack_pointer = p_win_stack->stack_pointer;
    while(stack_pointer > 0)
    {       
        p_current_win = &p_win_array[--stack_pointer];
        HtWin_Delete(p_current_win);
    }
    p_win_stack->stack_pointer = 0;
    bt_ui_cb.search_msgbox_handle = BT_UI_NULL_HANDLE;
    bt_ui_cb.search_menu_handle = BT_UI_NULL_HANDLE;
    bt_ui_cb.current_menu_handle = BT_UI_NULL_HANDLE;
    bt_ui_cb.current_bonding_handle = BT_UI_NULL_HANDLE;
    bt_ui_cb.select_bonded_handle = BT_UI_NULL_HANDLE;
    bt_ui_cb.select_discover_handle = BT_UI_NULL_HANDLE;
}


/* delete the window stack*/
void HtWinStack_Delete(tHT_WIN_STACK *p_win_stack)
{
    if(NULL == p_win_stack)
        return;
    BCM_WIN_MSG("HtWinStack_Delete %x",p_win_stack,0,0);
    HtWinStack_Clear(p_win_stack);

    BTUI_FreeIf(p_win_stack->p_win_array);
    BTUI_Free(p_win_stack);
}

/*HtWinStack_HandleEvent*/
boolean HtWinStack_HandleEvent(tHT_WIN_STACK *p_win_stack, AEEEvent evt, uint16 wParam, uint32 dwParam)
{
    BCM_WIN_MSG("HtWinStack_HandleEvent %x",p_win_stack,0,0);
    if(p_win_stack && p_win_stack->stack_pointer>0)
    {
        tHT_WIN *p_current_win = &p_win_stack->p_win_array[p_win_stack->stack_pointer-1];
        if(p_current_win->pfnHandleEvent)
        {
            if(p_current_win->pfnHandleEvent(p_current_win, evt, wParam, dwParam))
                return TRUE;
        }
    }
    
    return FALSE;
}

/* Enable or disable the HtWinStack. */
void HtWinStack_Enable(tHT_WIN_STACK *p_win_stack, boolean bEnable)
{
    BCM_WIN_MSG("HtWinStack_Enable %x",p_win_stack,0,0);
    if(p_win_stack && p_win_stack->stack_pointer > 0)
    {
        tHT_WIN *p_win = &p_win_stack->p_win_array[p_win_stack->stack_pointer-1];

        if(p_win->pfnEnable)
            p_win->pfnEnable(p_win, bEnable);
    }   
}


/* Draw the HtWinStack. */
void HtWinStack_Draw(tHT_WIN_STACK *p_win_stack)
{
    
    if(p_win_stack)
    {
        BCM_WIN_MSG("HtWinStack_Draw %x %x %x",p_win_stack->f_draw_hold_down,p_win_stack->stack_pointer,p_win_stack->f_need_draw);
        if(p_win_stack->f_draw_hold_down)
        {
            p_win_stack->f_need_draw = TRUE;
            return;
        }
        
        if(p_win_stack->stack_pointer > 0)
            HtWin_Draw(&p_win_stack->p_win_array[p_win_stack->stack_pointer-1], p_win_stack,BTBAR_NONE);
    }   
}

/* Hold down drawing the HtWinStack. */
void HtWinStack_SetDrawHoldDown(tHT_WIN_STACK *p_win_stack, boolean f_hold_down)
{
    if(p_win_stack)
    {
        BCM_WIN_MSG("HtWinStack_SetDrawHoldDown %x %x %x",p_win_stack->stack_pointer,f_hold_down,p_win_stack->f_need_draw);
        //HtWinStack_RemovePromptWindow(p_win_stack);
        if(f_hold_down)
        {
            HtWinStack_RemovePromptWindow(p_win_stack);
            p_win_stack->f_draw_hold_down = TRUE;
        }
        else
        {
            p_win_stack->f_draw_hold_down = FALSE;

            if(p_win_stack->f_need_draw)    
            {   
                p_win_stack->f_need_draw = FALSE;

                if(p_win_stack->stack_pointer > 0)
                {
                    tHT_WIN *p_win = &p_win_stack->p_win_array[p_win_stack->stack_pointer-1];

                    if(p_win->pfnEnable)
                        p_win->pfnEnable(p_win, TRUE);
                    
                    HtWin_Draw(p_win, p_win_stack,p_win->win_sk);
                }
            }   
        }
    }   
}

/* Increase the stack size. */
static int HtWinStack_Increase(tHT_WIN_STACK *p_win_stack)
{
    uint16 old_size = p_win_stack->stack_size;
    uint16 new_size = old_size + HT_WIN_STACK_INC_SIZE;
    tHT_WIN *p_tmp_win_array = (tHT_WIN *)BTUI_Malloc(new_size * sizeof(tHT_WIN));
    
    if(NULL == p_tmp_win_array)
        return ENOMEMORY;
    BCM_WIN_MSG("HtWinStack_Increase %d %d",old_size,new_size,0);
    MEMSET(p_tmp_win_array, 0, (new_size * sizeof(tHT_WIN)) );
    MEMCPY(p_tmp_win_array, p_win_stack->p_win_array, (old_size * sizeof(tHT_WIN)) );
    BTUI_Free(p_win_stack->p_win_array);
    p_win_stack->p_win_array = p_tmp_win_array;
    //BTUI_Free(p_win_stack->p_win_array);
    
    p_win_stack->stack_size = new_size;
    
    return SUCCESS;
}

/* Remove the prompt message box. */
void HtWinStack_RemovePromptWindow(tHT_WIN_STACK *p_win_stack)
{
    if(NULL != p_win_stack)
    {
        uint16 j;
        uint16 i = p_win_stack->stack_pointer;
        //BCM_FUN_STAR("HtWinStack_RemovePromptWindow %d",i,0,0);
        while(i-- > 0)
        {
            tHT_WIN *p_win = &p_win_stack->p_win_array[i];
            BCM_WIN_MSG("HtWinStack_RemovePromptWindow %d %d",p_win->win_type,p_win->pfnKeyPressCB,0);
            if( (HT_MENU_OPTION ==p_win->win_type) || (HT_PROC==p_win->win_type) || (HT_MSGBOX == p_win->win_type && NULL == p_win->pfnKeyPressCB))
            {
                HtWin_Delete(p_win);

                j = i + 1;

                // Move the windows above to below.
                while(j < p_win_stack->stack_pointer)
                {
                    p_win_stack->p_win_array[j-1] = p_win_stack->p_win_array[j];
                    j++;
                }

                p_win_stack->stack_pointer--;

                break;
            }
        }
    }
}

/* Push a win in the stack. */
/*int HtWinStack_Push(tHT_WIN_STACK *p_win_stack, tHT_WIN *p_win)
{
    if( (NULL == p_win_stack) || (NULL == p_win) )
        return EBADPARM;
    BCM_WIN_MSG("HtWinStack_Push",0,0,0);
    HtWinStack_RemovePromptWindow(p_win_stack);

    if(p_win_stack->stack_pointer >= p_win_stack->stack_size)
    {
        int ret = HtWinStack_Increase(p_win_stack);
        if(SUCCESS != ret)
            return ret;
    }

    // Disable the top window at first.
    HtWinStack_Enable(p_win_stack, FALSE);

    p_win_stack->p_win_array[p_win_stack->stack_pointer++] = *p_win;

    // Enable the new top window and draw it.
    HtWinStack_Enable(p_win_stack, TRUE);
    HtWin_Draw(p_win, p_win_stack,BTBAR_NONE);

    return SUCCESS;
}*/

/* Push a win in the stack by every element. */
int HtWinStack_PushEx(  tHT_WIN_STACK *p_win_stack,     
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
                                )
{   
    tHT_WIN *p_win;

    if(NULL == p_win_stack)
        return EBADPARM;
    //BCM_MSG_MED("HtWinStack_PushEx %d %x %x",win_type,pfnHandleEvent,pfnEnable);
    HtWinStack_RemovePromptWindow(p_win_stack);
    BCM_FUN_STAR("HtWinStack_PushEx %x %x %x",p_win_stack->stack_pointer,p_win_stack->stack_size,p_win_stack->f_draw_hold_down);
    if(p_win_stack->stack_pointer >= p_win_stack->stack_size)
    {
        int ret = HtWinStack_Increase(p_win_stack);
        if(SUCCESS != ret)
            return ret;
    }

    p_win = &p_win_stack->p_win_array[p_win_stack->stack_pointer];

    p_win->win_handle = win_handle;
    p_win->pfnKeyPressCB = pfnKeyPressCB;   
    p_win->win_type = win_type; 
    p_win->win_sk = soft_key;
    p_win->msg_type = msg_type;
    p_win->data = data;
    if(!pfnHandleEvent || !pfnEnable)
    {
        switch(win_type)
        {
            case HT_MSGBOX:
                if(!pfnHandleEvent)
                    pfnHandleEvent = HtMsgBox_HandleEvent;
                break;

            case HT_EDIT:
            case HT_EDIT_NAME:
                if(!pfnHandleEvent)
                    pfnHandleEvent = HtEdit_HandleEvent;
                if(!pfnEnable)
                    pfnEnable = HtEdit_Enable;
                break;

            case HT_MENU:
            case HT_MENU_OPTION:
                if(!pfnHandleEvent)
                    pfnHandleEvent = HtMenu_HandleEvent;
                if(!pfnEnable)
                    pfnEnable = HtMenu_Enable;          
                break;

            case HT_STATIC:   
                if(!pfnHandleEvent)
                    pfnHandleEvent = HtStatic_HandleEvent;
                if(!pfnEnable)
                    pfnEnable = HtStatic_Enable;          
                break;

            case HT_PROC:
                if(!pfnHandleEvent)
                {
                    pfnHandleEvent = HtPorcBox_HandleEvent;
                }
                break;

        }
    }
    p_win->pfnHandleEvent = pfnHandleEvent;
    p_win->pfnEnable = pfnEnable;

    p_win->win_data = win_data;

    // Set the message box timer.
    if((win_data != 0) && (HT_MSGBOX == win_type))
    {
        if(ISHELL_SetTimer(g_pBtuiApp->m_pIShell, (INT32)win_data, HtMsgBox_Timer, (void *)p_win))
        {   
            MEMSET(p_win, 0, sizeof(tHT_WIN));
            return EFAILED;
        }
    }

    if(pw_title && pw_title[0])
    {
        bt_ui_wstr_cpyn(p_win->sz_title, MAX_BT_TITLE_LEN, pw_title);
    }
    // Disable the top window at first.
    HtWinStack_Enable(p_win_stack, FALSE);

    p_win_stack->stack_pointer++;

    // Enable the new top window and draw it.
    HtWinStack_Enable(p_win_stack, TRUE);
    HtWin_Draw(p_win, p_win_stack,soft_key);

    return SUCCESS; 
}

/* Pop a win from the stack. */
void HtWinStack_Pop(tHT_WIN_STACK *p_win_stack)
{
    if((NULL == p_win_stack) || (0 == p_win_stack->stack_pointer))
        return ;
    BCM_WIN_MSG("HtWinStack_Pop",0,0,0);
    p_win_stack->stack_pointer--;
    HtWin_Delete(&p_win_stack->p_win_array[p_win_stack->stack_pointer]);

    if(p_win_stack->stack_pointer > 0)
    {
        // Enable the new top window and draw it.
        HtWinStack_Enable(p_win_stack, TRUE);
        HtWinStack_Draw(p_win_stack);
    }
}

/* Remove a win from the stack. */
void HtWinStack_Remove(tHT_WIN_STACK *p_win_stack, uint32 win_handle)
{
    BCM_WIN_MSG("HtWinStack_Remove",0,0,0);
    if(p_win_stack && (0 != p_win_stack->stack_pointer))    
    {
        uint16 i;
        tHT_WIN *p_win = HtWinStack_FindWindowByHandle(p_win_stack, win_handle, &i);
        if(p_win)
        {
            HtWin_Delete(p_win);

            if (p_win_stack->stack_pointer == i)
            {
                // The window to be removed is the active window, so we must switch 
                // the active window and redraw the window stack.

                p_win_stack->stack_pointer--;
                if(p_win_stack->stack_pointer > 0)
                {
                    // Enable the new top window and draw it.
                    HtWinStack_Enable(p_win_stack, TRUE);
                    HtWinStack_Draw(p_win_stack);
                }   
            }
            else
            {
                // Move the windows above to below.
                while(i < p_win_stack->stack_pointer)
                {
                    p_win_stack->p_win_array[i-1] = p_win_stack->p_win_array[i];
                    i++;
                }

                p_win_stack->stack_pointer--;
            }
        }
    }
}


/* Post event from one window to another. */
/*void HtWinStack_PostEvent(tHT_WIN_STACK *p_win_stack, 
                                  uint32 from_win_handle,
                                  uint32 to_win_handle,
                                  AEEEvent evt, 
                                  uint16 wParam, 
                                  uint32 dwParam)
{

}*/

/* Send event from one window to another. */
/*void HtWinStack_SendEvent(tHT_WIN_STACK *p_win_stack, 
                                  uint32 from_win_handle,
                                  uint32 to_win_handle,
                                  AEEEvent evt, 
                                  uint16 wParam, 
                                  uint32 dwParam)
{

}*/
#ifdef UI_DM_EDIT_NAME_EX
/*=============================================================================
*    必须在textctl初始化完毕后,才能获得icon id,
*    而且要在dialog更新完之后再更新图标
=============================================================================*/
static void  HtWinStack_DrawIMEIcon(ITextCtl *pTextCtl, IDisplay *pIDisplay,IShell   *pIShell)
{
    boolean  bRet = TRUE;
    IImage     *RightTopImg = NULL; 
    uint16     nResID;
    AEERect IconRect;
    Theme_Param_type TParam;
    
    if(pTextCtl == NULL ||pIShell == NULL)
        return;

    bRet = ITEXTCTL_GetInfo(pTextCtl, AEE_TMINFO_ICONID, (void *)&nResID);

    if(!bRet)
        return;
    
    Appscom_GetThemeParameters(&TParam);   

    ITEXTCTL_GetRect(pTextCtl, &IconRect);
    IconRect.x = 0;
    IconRect.dx = 28;
    IconRect.dy = GetBottomBarHeight(pIDisplay);/* + SEARCH_INPUTBOX_GAP*/
    IDISPLAY_FillRect(pIDisplay, &IconRect, TParam.textColor); 
    
    RightTopImg = ISHELL_LoadResImage(pIShell, AEE_APPSCOMMONRES_IMAGESFILE, nResID);
    if(RightTopImg != NULL)
    {
        IIMAGE_Draw(RightTopImg, IconRect.x, IconRect.y + 5);
        IIMAGE_Release(RightTopImg);
        RightTopImg = NULL;
    }
}//CContApp_DrawIMEIcon
#endif
///////////////////////////////// File End /////////////////////////////////////////