
#include "bt_ui_int.h"
#include "bt_ui_ht_win.h"
#include "bt_ui_ht_common.h"
#ifndef AEE_SIMULATOR
#include "AppComFunc.h"
#endif
/* verify if the handle is valid. */
//static boolean  bt_ui_is_valid_handle(UINT32 handle)
//{
//    tHT_WIN_STACK *p_win_stack = bt_ui_get_win_stack();
//    BCM_WIN_MSG("bt_ui_is_valid_handle",0,0,0);
//    if(HtWinStack_FindWindowByHandle(p_win_stack, handle, NULL))
//        return TRUE;
//
//    return FALSE;
//}
static PromptMsg_e_Type bt_msg_type = MESSAGE_NONE;
static uint16 bt_ui_menu_sel = 0;
typedef struct _check_box_menu_cb
{
    uint16 item;
    tBTUI_MENU *f_menu_cb;
}check_box_menu_cb;

static check_box_menu_cb bt_check_box_menu_cb[2];

#define BTUI_MAX_INPUT_CHARS    64
#ifdef UI_DM_EDIT_NAME_EX
/* create a window that contain a edit box. */
UINT32 bt_ui_edit_box_edit_name(UINT32 flags, tHT_KEY_PRESSED_CBACK *p_cback, TCHAR *pw_prompt_text, UINT16 max_size,BottomBar_e_Type soft_key,char* old_name)
{   
    ITextCtl *pTextCtl = NULL;
    AEERect rcClient;
    tHT_WIN_STACK *pWinStack;
    IShell *pIShell = bt_ui_get_shell();
    int title_hi = GetTitleBarHeight(bt_ui_get_display());
    chang_name_string *name_string = NULL;
    BCM_WIN_MSG("bt_ui_edit_box_edit_name",0,0,0);
    if(!pIShell || ISHELL_CreateInstance(pIShell, AEECLSID_TEXTCTL, (void **)&pTextCtl))
        goto LError;
    bt_ui_get_client(&rcClient);

    rcClient.dy = rcClient.dy -title_hi *2 -20;
    rcClient.y = rcClient.y+ 20+title_hi*2;
    rcClient.x = title_hi-2;
    rcClient.dx = rcClient.dx -title_hi+2;
    ITEXTCTL_SetProperties(pTextCtl, TP_FRAME|TP_STARKEY_SWITCH|TP_FOCUS_NOSEL);

    rcClient.dy = rcClient.dy - GetBottomBarHeight(bt_ui_get_display());
    ITEXTCTL_SetRect(pTextCtl, &rcClient);
    ITEXTCTL_SetMaxSize(pTextCtl, max_size);
#ifdef AEE_SIMULATOR
    if(flags == AEE_TM_NUMBERS)
        ITEXTCTL_SetInputMode(pTextCtl, AEE_TM_NUMBERS);
#endif

    if(old_name!= NULL && pw_prompt_text != NULL)
    {
        name_string = BTUI_Malloc(sizeof(chang_name_string));
        if(name_string)
        {
            WSTRLCPY(name_string->title,pw_prompt_text,32);
            UTF8TOWSTR((byte *) old_name, BTUI_DEV_NAME_LENGTH+1, name_string->old_name, (BTUI_DEV_NAME_LENGTH+1)*sizeof(AECHAR));
        }
    }

    pWinStack = bt_ui_get_win_stack();
    if(HtWinStack_PushEx(pWinStack, (uint32)pTextCtl, HT_EDIT_NAME, flags, NULL, NULL, NULL, p_cback,soft_key,0,(void*)name_string))
        goto LError;

    return (UINT32)pTextCtl;

LError:
    BTUI_ReleaseObjIf(pTextCtl);
    return (UINT32)NULL;
}
#endif

/* create a window that contain a edit box. */
UINT32 bt_ui_edit_box(UINT32 flags, tHT_KEY_PRESSED_CBACK *p_cback, TCHAR *pw_prompt_text, UINT16 max_size,BottomBar_e_Type soft_key,char* old_name,boolean is_edit_name)
{   
    ITextCtl *pTextCtl = NULL;
    AEERect rcClient;
    tHT_WIN_STACK *pWinStack;
    IShell *pIShell = bt_ui_get_shell();
    AECHAR old_ae_name[BTUI_DEV_NAME_LENGTH+1] = {0};
    BCM_WIN_MSG("bt_ui_edit_box",0,0,0);
    if(!pIShell || ISHELL_CreateInstance(pIShell, AEECLSID_TEXTCTL, (void **)&pTextCtl))
        goto LError;
    bt_ui_get_client(&rcClient);

    if(is_edit_name)
    {
        if(old_name)
        {
            UTF8TOWSTR((const byte *) old_name, BTUI_DEV_NAME_LENGTH+1, old_ae_name, (BTUI_DEV_NAME_LENGTH+1)*sizeof(AECHAR));
            ITEXTCTL_SetText(pTextCtl, old_ae_name, WSTRLEN(old_ae_name));
        }
        ITEXTCTL_SetProperties(pTextCtl, TP_FRAME|TP_MULTILINE | TP_STARKEY_SWITCH|TP_FOCUS_NOSEL|TP_GRAPHIC_BG);
    }
    else
    {
        ITEXTCTL_SetProperties(pTextCtl, TP_FRAME|TP_T9_MODE|TP_FOCUS_NOSEL|TP_GRAPHIC_BG);
    }
    rcClient.dy = rcClient.dy - GetBottomBarHeight(bt_ui_get_display());
    ITEXTCTL_SetRect(pTextCtl, &rcClient);
    //if(is_edit_name)
    //{
    //    nv_language_enum_type language = 0;
    //    OEM_GetConfig( CFGI_LANGUAGE_SELECTION, &language, sizeof(language));
    //    if(NV_LANGUAGE_CHINESE == language)
    //    {
    //        
    //    }
    //}
    ITEXTCTL_SetMaxSize(pTextCtl, max_size);
#ifdef AEE_SIMULATOR
    if(flags == AEE_TM_NUMBERS)
    {
        ITEXTCTL_SetInputMode(pTextCtl, AEE_TM_NUMBERS);
    }
#else
    if(flags)
    {
        if(HT_EB_NUMBERS == flags)
        {
            ITEXTCTL_SetInputMode(pTextCtl, HT_EB_NUMBERS);
        }
        else
        {
            AEETextInputMode nInputMode; 
            nInputMode = ITEXTCTL_GetInputMode(pTextCtl,NULL);
            ITEXTCTL_SetInputMode(pTextCtl, (AEE_TM_NONE == nInputMode)?(HT_EB_LETTERS):(nInputMode));
        }
    }
#endif
    //if(is_edit_name == FALSE)
    {
        ITEXTCTL_SetTitle(pTextCtl,NULL,0,pw_prompt_text);
    }
    //ITEXTCTL_SetActive(pTextCtl, TRUE);

    pWinStack = bt_ui_get_win_stack();
    if(HtWinStack_PushEx(pWinStack, (uint32)pTextCtl, HT_EDIT, flags, NULL, NULL, NULL, p_cback,soft_key,0,NULL))
        goto LError;

    return (UINT32)pTextCtl;

LError:
    BTUI_ReleaseObjIf(pTextCtl);
    return (UINT32)NULL;
}

/* create a messgaebox. */
UINT32 bt_ui_message_box(TCHAR *p_text, tHT_KEY_PRESSED_CBACK *p_cback,BottomBar_e_Type soft_key)
{
    BCM_WIN_MSG("bt_ui_message_box",0,0,0);
    return bt_ui_message_box_timeout(p_text, p_cback, 0 ,soft_key);
}
UINT32 bt_ui_message_box_ex(UINT16 string_id, tHT_KEY_PRESSED_CBACK *p_cback,BottomBar_e_Type soft_key)
{
    BCM_WIN_MSG("bt_ui_message_box_ex",0,0,0);
    return bt_ui_message_box_timeout(bt_ui_get_brx_string(string_id), p_cback, 0 ,soft_key);
}
/* create a messgaebox. */
UINT32 bt_ui_message_box_timeout(TCHAR *p_text, tHT_KEY_PRESSED_CBACK *p_cback, INT32 dwMSecs,BottomBar_e_Type soft_key)
{
    TCHAR *pMsgText = NULL;
    int msgLen = 0;
    int nRet;

    if(p_text)
        msgLen = WSTRLEN(p_text);
    BCM_WIN_MSG("bt_ui_message_box_timeout %d",msgLen,0,0);
    if(msgLen > 0)
    {
        pMsgText = (TCHAR *)BTUI_Malloc((msgLen + 1) << 1);
        if(pMsgText)
        {           
            WSTRCPY(pMsgText, p_text);
            nRet = HtWinStack_PushEx(bt_ui_get_win_stack(), 
                                            (uint32)pMsgText, 
                                            HT_MSGBOX, 
                                            (dwMSecs > 0) ? (uint32)dwMSecs : 0, 
                                            NULL, 
                                            NULL, 
                                            NULL, 
                                            p_cback,soft_key,(uint8)bt_msg_type,NULL);
            bt_msg_type = MESSAGE_NONE;
            if(SUCCESS == nRet)
                return (UINT32)pMsgText;

            BTUI_Free(pMsgText);
        }
    }

    return (UINT32)NULL;
}

/* create a window that contain a menu. */
UINT32 bt_ui_create_menu(tHT_MENU_ITEM *p_menu_items, UINT16 menu_item_count,
                                tHT_KEY_PRESSED_CBACK *p_cback,  AECHAR *pw_prompt_text  ,BottomBar_e_Type BottomBar ,boolean is_option,uint32 dwProps)
{
    tHT_WIN_STACK *pWinStack;
    IMenuCtl *pMenuCtl = NULL;
    AEERect rcClient;
    uint32 i;
    uint16 win_type = HT_MENU;
    IShell *pIShell = bt_ui_get_shell();
    BCM_WIN_MSG("bt_ui_create_menu %d",menu_item_count,0,0);
    if(!pIShell || ISHELL_CreateInstance(pIShell, AEECLSID_MENUCTL, (void **)&pMenuCtl))
        goto LError;
    
    if(is_option == FALSE)
    {
        bt_ui_get_client(&rcClient);
        IMENUCTL_SetRect(pMenuCtl, &rcClient);
    }
    IMENUCTL_DeleteAll(pMenuCtl);

    if(is_option)
    {
    }
    else
    {
        IMENUCTL_SetTitle(pMenuCtl,NULL,0,pw_prompt_text);
    }

    for(i=0; i<menu_item_count; i++)
    {
        if(p_menu_items[i].item_string_id != 0)
        {
            IMENUCTL_AddItem(pMenuCtl,AEE_BLUETOOTH_RES_FILE,p_menu_items[i].item_string_id,p_menu_items[i].item_index,NULL, p_menu_items[i].item_data);
        }
        else
        {
            CtlAddItem cai;
            MEMSET(&cai, 0, sizeof(cai));
            cai.wItemID = p_menu_items[i].item_index;
            cai.dwData = p_menu_items[i].item_data;
#ifdef FEATURE_BLUETOOTH_DEVICE_IMAGE
            if(p_menu_items[i].item_img_id != 0)
            {
                cai.wImage = p_menu_items[i].item_img_id;
                cai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
            }
#endif
            /*if(p_menu_items[i].item_string_id != 0)
            {
                cai.pText = bt_ui_get_brx_string(p_menu_items[i].item_string_id);
                DBGPRINTF("add menu %S %d",cai.pText ,p_menu_items[i].item_string_id);
            }
            else*/if(p_menu_items[i].p_item_name)
            {
                int szLen;
                TCHAR *szWName = bt_ui_get_comm_wbuffer(&szLen);            

                MEMSET(szWName, 0, szLen<<1);
                UTF8TOWSTR( (const byte*)p_menu_items[i].p_item_name, STRLEN(p_menu_items[i].p_item_name), szWName, szLen*2);
                //STRTOWSTR(p_menu_items[i].p_item_name, szWName, szLen); // 127 chars is enough.
                cai.pText = szWName;
            }
            else if(p_menu_items[i].p_w_item_name)
            {
                cai.pText = p_menu_items[i].p_w_item_name;
            }
            else
            {
                cai.pText = L"";
            }
            IMENUCTL_AddItemEx(pMenuCtl, &cai);
        }
    }

    if(is_option)
    {
#ifndef AEE_SIMULATOR
        IMENUCTL_SetPopMenuRect(pMenuCtl);
        win_type = HT_MENU_OPTION;
#endif
    }

    if(MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT == dwProps)
    {
        /*if set ckeck box,will clear the menu data,so save it*/
        /*note: this only for discovey mode!!!!!!!!!!!!!!!!!!!!!*/
        for(i=0; i<2; i++)
        {
            bt_check_box_menu_cb[i].item = p_menu_items[i].item_index;
            bt_check_box_menu_cb[i].f_menu_cb = (tBTUI_MENU *)p_menu_items[i].item_data;
        }
#ifndef AEE_SIMULATOR
        InitMenuIcons(pMenuCtl);
        SetMenuIcon(pMenuCtl, bt_ui_menu_sel, TRUE);
#endif
        IMENUCTL_SetSel(pMenuCtl, bt_ui_menu_sel);
#ifdef FEATURE_BLUETOOTH_ITEM_TO_NUMBER_KEY
        IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);
#else
        IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);
#endif
    }
#ifdef FEATURE_BLUETOOTH_ITEM_TO_NUMBER_KEY
    else if(~MP_BIND_ITEM_TO_NUMBER_KEY == dwProps)
    {
        if(bt_ui_menu_sel > menu_item_count)
        {
            bt_ui_menu_sel = 0;
        }
        if(FALSE == is_option && 0 != bt_ui_menu_sel)
        {
            IMENUCTL_SetSel(pMenuCtl, bt_ui_menu_sel);
            bt_ui_menu_sel = 0;
        }
        IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
    }
#endif
    else
    {
#ifdef FEATURE_BLUETOOTH_ITEM_TO_NUMBER_KEY
        IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
#else
        IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
#endif
    }
#ifndef AEE_SIMULATOR
    IMENUCTL_SetOemProperties( pMenuCtl, OEMMP_USE_MENU_STYLE);//new style
    IMENUCTL_SetBottomBarType(pMenuCtl,BottomBar);
    BottomBar = BTBAR_NONE;
#endif

    //IMENUCTL_SetActive(pMenuCtl, TRUE);

    pWinStack = bt_ui_get_win_stack();
    if(HtWinStack_PushEx(pWinStack, (uint32)pMenuCtl, win_type, 0, NULL, NULL, NULL, p_cback,BottomBar,0,NULL))
        goto LError;

    return (UINT32)pMenuCtl;

LError:
    BTUI_ReleaseObjIf(pMenuCtl);
    return (UINT32)NULL;
}   


/* create a window that contain a menu. */
UINT32 bt_ui_create_static(TCHAR *string ,  tHT_KEY_PRESSED_CBACK *p_cback,  UINT16 title  ,BottomBar_e_Type BottomBar )
{
    tHT_WIN_STACK *pWinStack;
    IStatic * pStatic = NULL;
    AEERect rcClient;
    IShell *pIShell = bt_ui_get_shell();
    AECHAR w_title[32] = {0};
    BCM_WIN_MSG("bt_ui_create_static ",0,0,0);
    if(!pIShell || ISHELL_CreateInstance(bt_ui_get_shell(),AEECLSID_STATIC, (void **)&pStatic))
        goto LError;

    bt_ui_get_client(&rcClient);
    rcClient.dy = rcClient.dy - GetBottomBarHeight(bt_ui_get_display());
    ISTATIC_SetRect(pStatic, &rcClient);
    //ISTATIC_SetProperties(pStatic, ST_CENTERTEXT|ST_MIDDLETEXT); 
    ISHELL_LoadResString(pIShell, AEE_BLUETOOTH_RES_FILE, title, w_title, sizeof(w_title)) ;
    ISTATIC_SetProperties(pStatic, ST_CENTERTEXT); //wuraojin 20081215
    ISTATIC_SetText(pStatic,w_title,string,AEE_FONT_NORMAL,AEE_FONT_NORMAL);

    pWinStack = bt_ui_get_win_stack();
    if(HtWinStack_PushEx(pWinStack, (uint32)pStatic, HT_STATIC, 0, NULL, NULL, NULL, p_cback, BottomBar,0 ,NULL))
        goto LError;

    return (UINT32)pStatic;

LError:
    BTUI_ReleaseObjIf(pStatic);
    return (UINT32)NULL;
}   

/* create a window that contain a top menu. */
//UINT32 bt_ui_create_top_menu(tHT_MENU_ITEM *p_menu_items, UINT16 menu_item_count,
//                                              tHT_KEY_PRESSED_CBACK *p_cback,  TCHAR *p_prompt_text)
//{
//    UINT32  handle = (UINT32)NULL;
//    BCM_WIN_MSG("bt_ui_create_top_menu",0,0,0);
//    return handle;
//}


/* close a window by handle. */
void bt_ui_close_object(UINT32 handle)
{
    BCM_WIN_MSG("bt_ui_close_object",0,0,0);
    if(BT_UI_NULL_HANDLE != handle)
    {
        HtWinStack_Remove(bt_ui_get_win_stack(), handle);
    }
}

/* The default menu callback funtion, the item data is the handle function's ptr. */
void bt_ui_menu_cback(UINT32 handle, UINT32 key)
{
    BCM_WIN_MSG("bt_ui_menu_cback %x %x",handle,key,0);
    switch(key)
    {
    case HT_KEY_SELECT:
    case HT_KEY_SOFT1:
        {
            tBTUI_MENU *pfnCBItem = NULL;
            uint16 itemSel;

            itemSel = IMENUCTL_GetSel((IMenuCtl *)handle);
            IMENUCTL_GetItemData((IMenuCtl *)handle, itemSel, (uint32 *)(&pfnCBItem));
            //if((UINT32)pfnCBItem == (UINT32)bt_ui_menu_change_name)//wuraojin 20081216
            //if(itemSel == 2)
            //{
            //    bt_ui_device_db.pre_settingmenu_changename_handle = handle;
            //}
            if(pfnCBItem)
            {   
                pfnCBItem();
            }
        }
        break;
    
    case HT_KEY_SOFT2:      
        bt_ui_close_object(handle);
        break;
    }   
}

void bt_ui_menu_cback_for_discovery_mode(UINT32 handle, UINT32 key)
{
    BCM_WIN_MSG("bt_ui_menu_cback_for_discovery_mode %x %x",handle,key,0);
    switch(key)
    {
    case HT_KEY_SELECT:
    case HT_KEY_SOFT1:
        {
            //tBTUI_MENU *pfnCBItem = NULL;
            uint16 itemSel;
            byte i = 0;
            itemSel = IMENUCTL_GetSel((IMenuCtl *)handle);
            
            for(i=0; i<2; i++)
            {
                if(bt_check_box_menu_cb[i].item == itemSel)
                {   
                    if(bt_check_box_menu_cb[i].f_menu_cb != NULL)
                    {
                        bt_check_box_menu_cb[i].f_menu_cb();
                    }
                }
            }
        }
        break;
    
    case HT_KEY_SOFT2:      
        bt_ui_close_object(handle);
        break;
    }   
}

/* bt_ui_set_draw_hold_down. */
void bt_ui_set_draw_hold_down(BOOLEAN f_hold_down)
{
    BCM_WIN_MSG("bt_ui_set_draw_hold_down",0,0,0);
    HtWinStack_SetDrawHoldDown(bt_ui_get_win_stack(), f_hold_down);
}


/* The default message box callback funtion. */
void bt_ui_msgbox_cback(UINT32 handle, UINT32 key)
{
    BCM_WIN_MSG("bt_ui_msgbox_cback %x",key,0,0);
    if(key == HT_KEY_SOFT2)
        bt_ui_close_object(handle);
}

#ifdef AEE_SIMULATOR
/* Set the softkeys of a window. */
void HT_SetSoftKeyMessages(UINT32 handle, BottomBar_e_Type win_sk)
{   
    tHT_WIN *pWin = HtWinStack_FindWindowByHandle(bt_ui_get_win_stack(), handle, NULL);
    BCM_WIN_MSG("HT_SetSoftKeyMessages %x %x",pWin,win_sk,0);
    if(NULL != pWin)
    {
        pWin->win_sk = win_sk;
        HtWin_DrawBottomLineEx(pWin);       
    }
}
#endif
/* Create a message box with time out period. */
UINT32 HT_MessageBoxTimeout(TCHAR *p_text, INT32 dwMSecs)
{
    BCM_WIN_MSG("HT_MessageBoxTimeout",0,0,0);
    return bt_ui_message_box_timeout(p_text, NULL, dwMSecs,BTBAR_NONE);
}
UINT32 HT_MessageBoxTimeout_ex(UINT16 string_id, INT32 dwMSecs)
{
    BCM_WIN_MSG("HT_MessageBoxTimeout_ex",0,0,0);
    if(IDS_FAIL == string_id)
    {
        bt_msg_type = MESSAGE_WARNNING;
    }
    return bt_ui_message_box_timeout(bt_ui_get_brx_string(string_id), NULL, dwMSecs,BTBAR_NONE);
}
UINT32 HT_MessageBoxProcess(TCHAR *p_text, UINT32 bytes_transferred,UINT32 obj_size,tHT_KEY_PRESSED_CBACK *p_cback,BottomBar_e_Type BottomBar)
{
    TCHAR *pMsgText = NULL;
    int msgLen = 0;
    int nRet;
    tBTUI_OPP_PROC_VAL * proc_val = NULL;
    if(p_text)
    {
        msgLen = WSTRLEN(p_text);
    }
    BCM_WIN_MSG("HT_MessageBoxProcess %d",msgLen,0,0);
    if(msgLen > 0)
    {
        proc_val = (tBTUI_OPP_PROC_VAL*)BTUI_Malloc(sizeof(tBTUI_OPP_PROC_VAL));
        pMsgText = (TCHAR *)BTUI_Malloc((msgLen + 1) << 1);
        if(pMsgText && proc_val)
        {
            WSTRCPY(pMsgText, p_text);
            proc_val->bytes_transferred= bytes_transferred;
            proc_val->obj_size = obj_size;
            nRet = HtWinStack_PushEx(bt_ui_get_win_stack(), 
                                            (uint32)pMsgText, 
                                            HT_PROC, 
                                            (uint32)proc_val, 
                                            NULL, 
                                            NULL, 
                                            NULL, 
                                            p_cback,BottomBar,0,NULL);
            if(SUCCESS == nRet)
            {
                return (UINT32)pMsgText;
            }
            BTUI_Free(pMsgText);
        }
    }

    return (UINT32)NULL;
}
/* Create a message box with default time out period. */
//UINT32 HT_MessageBox(TCHAR *p_text)
//{
//    BCM_WIN_MSG("HT_MessageBox",0,0,0);
//    return bt_ui_message_box_timeout(p_text, NULL, BTUI_TIMEOUT_MSG_PERIOD,BTBAR_NONE);
//}

/* Set the edit box max input chars. */
//void HT_SetEditBoxMaxChars(UINT32 handle, uint16 nMaxChars)
//{
//    ITextCtl *pITextCtl = (ITextCtl *)handle;
//    BCM_WIN_MSG("HT_SetEditBoxMaxChars %x",pITextCtl,0,0);
//    if(pITextCtl)
//        ITEXTCTL_SetMaxSize(pITextCtl, nMaxChars);
//}

/* Get the edit box text ptr. */
TCHAR * HT_GetEditBoxText(UINT32 handle)
{
    ITextCtl *pITextCtl = (ITextCtl *)handle;
    BCM_WIN_MSG("HT_GetEditBoxText %x",pITextCtl,0,0);
    if(pITextCtl)
        return ITEXTCTL_GetTextPtr(pITextCtl);

    return NULL;
}

/* Check if the handle is active window's handle. */
BOOLEAN HT_IsActiveHandle(UINT32 handle)
{
    BCM_WIN_MSG("HT_IsActiveHandle",0,0,0);
    return HtWinStack_IsActiveHandle(bt_ui_get_win_stack(), handle);
}

/* Get the current window handle. */
//UINT32 HT_GetCurrHandle(void)
//{
//    tHT_WIN_STACK *p_win_stack = bt_ui_get_win_stack();
//    tHT_WIN *p_win;
//    BCM_WIN_MSG("HT_GetCurrHandle",0,0,0);
//    p_win   = HtWinStack_GetActiveWin(p_win_stack, NULL);
//    if(p_win)
//        return p_win->win_handle;
//
//    return NULL;
//}

/* Remove the prompt message box. */
//void Ht_RemovePromptWindow(void)
//{
//    BCM_WIN_MSG("Ht_RemovePromptWindow",0,0,0);
//    HtWinStack_RemovePromptWindow(bt_ui_get_win_stack());
//}

/* Get the current menu handle. */
UINT32 HT_GetCurrMenuHandle(void)
{
    tHT_WIN_STACK *p_win_stack = bt_ui_get_win_stack();
    tHT_WIN *p_win;
    uint16 win_pos;
    BCM_WIN_MSG("HT_GetCurrMenuHandle",0,0,0);
    p_win   = HtWinStack_GetActiveWin(p_win_stack, &win_pos);
    while(p_win)
    {
        if(HT_MENU == HtWinStack_GetWindowType(p_win) || HT_MENU_OPTION == HtWinStack_GetWindowType(p_win))
            return p_win->win_handle;

        p_win = HtWinStack_FindWindowByPos(p_win_stack, --win_pos);
    }

    return NULL;
}

/* Get the selected item of the menu. */
int HT_GetMenuSelectedItem(UINT32 handle, tHT_MENU_ITEM *p_menu_item)
{
    IMenuCtl *pMenuCtl;
    CtlAddItem cai;
    tHT_WIN *p_win;
    BCM_WIN_MSG("HT_GetMenuSelectedItem",0,0,0);
    if((0==handle) || (NULL==p_menu_item))
        return EBADPARM;

    p_win   = HtWinStack_FindWindowByHandleEx(bt_ui_get_win_stack(), handle);
    if(!p_win || (p_win->win_type != HT_MENU && p_win->win_type != HT_MENU_OPTION))
        return EFAILED;

    pMenuCtl = (IMenuCtl *)handle;
    MEMSET(&cai, 0, sizeof(CtlAddItem));
    p_menu_item->item_index = IMENUCTL_GetSel(pMenuCtl);
    if(IMENUCTL_GetItem(pMenuCtl, p_menu_item->item_index, &cai))
    {   
        p_menu_item->item_data = cai.dwData;
        p_menu_item->p_w_item_name = cai.pText;
        return SUCCESS;
    }   

    return EFAILED;
}

/* Add menu items. */
void HT_AddMenuItems(UINT32 handle, tHT_MENU_ITEM *p_menu_items, UINT16 menu_item_count)
{   
    IMenuCtl *pMenuCtl= (IMenuCtl *)handle;
    CtlAddItem cai;
    UINT16 i;
    BCM_WIN_MSG("HT_AddMenuItems %x",pMenuCtl,0,0);
    if(NULL == pMenuCtl)
        return;

    MEMSET(&cai, 0, sizeof(cai));
    for(i=0; i<menu_item_count; i++)
    {
        cai.wItemID = p_menu_items[i].item_index;
        cai.dwData = p_menu_items[i].item_data;
#ifdef FEATURE_BLUETOOTH_DEVICE_IMAGE        
        if(p_menu_items[i].item_img_id != 0)
        {
            cai.wImage = p_menu_items[i].item_img_id;
            cai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
        }
#endif
        if(p_menu_items[i].p_item_name)
        {
            int szLen;
            TCHAR *szWName = bt_ui_get_comm_wbuffer(&szLen);            

            MEMSET(szWName, 0, szLen<<1);   
            //STRTOWSTR(p_menu_items[i].p_item_name, szWName, szLen); // 127 chars is enough.
            UTF8TOWSTR( (const byte*)p_menu_items[i].p_item_name, STRLEN(p_menu_items[i].p_item_name), szWName, szLen*2);
            cai.pText = szWName;
        }
        else if(p_menu_items[i].p_w_item_name)
        {
            cai.pText = p_menu_items[i].p_w_item_name;
        }
        else
        {
            cai.pText = L"";
        }
        IMENUCTL_AddItemEx(pMenuCtl, &cai);
    }
}

/* Delete the menu item. */
//void HT_DeleteMenuItem(UINT32 handle, UINT16 index)
//{
//    IMenuCtl *pMenuCtl= (IMenuCtl *)handle;
//    BCM_WIN_MSG("HT_DeleteMenuItem %x",pMenuCtl,0,0);
//    if(pMenuCtl)
//        IMENUCTL_DeleteItem(pMenuCtl, index);
//}

/* Clear all the menu items. */
void HT_ClearMenuItems(UINT32 handle)
{
    IMenuCtl *pMenuCtl= (IMenuCtl *)handle;
    BCM_WIN_MSG("HT_ClearMenuItems %x",pMenuCtl,0,0);
    if(pMenuCtl)
        IMENUCTL_DeleteAll(pMenuCtl);
}

void HT_UpdateHandle(UINT32 handle)
{
    BCM_WIN_MSG("HT_UpdateHandle",0,0,0);
    if(HT_IsActiveHandle(handle))
        HtWinStack_Draw(bt_ui_get_win_stack());
}

void HT_UpdateDisplay(void)
{
    BCM_FUN_STAR("HT_UpdateDisplay",0,0,0);
    HtWinStack_Draw(bt_ui_get_win_stack());
}


AECHAR * bt_ui_get_brx_string(uint16 string_id)
{
    int len = 0;
    static AECHAR *buf;
    buf = bt_ui_get_comm_wbuffer(&len);
    ISHELL_LoadResString(bt_ui_get_shell(), AEE_BLUETOOTH_RES_FILE, string_id, buf, 2*len) ;
    return buf;
}

void bt_ui_set_menu_sel(uint16 sel)
{
    bt_ui_menu_sel = sel;
}
//uint16 bt_ui_get_menu_sel(void)
//{
//    return bt_ui_menu_sel;
//}
void bt_ui_set_pop_msg_type(PromptMsg_e_Type sel)
{
    bt_msg_type = sel;
}
///////////////////////////////// file end /////////////////////////////////////////