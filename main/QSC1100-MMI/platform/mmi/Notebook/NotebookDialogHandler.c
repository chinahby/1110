#ifndef _NOTEBOOK_DIALOG_HANDLER_
#define _NOTEBOOK_DIALOG_HANDLER_
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
		Qiong.chen Create 2009/02/06
		
*********************************************************************/

/*==============================================================================

==============================================================================*/


#include "NotebookAppProt.h"

#include "AEECM.h"

#ifdef FEATURE_SID_LOCK
#include "OEMNV.h"
#endif

#include "OEMRTC.h"

#ifdef FEATURE_COLOR_DISPLAY
#include "appscommon_color.brh"
#else
#include "appscommon_momo.brh"
#endif
#include "appscommon.h" 

/*==============================================================================

==============================================================================*/
// 1?±????°?òoê
#define CLOSE_DIALOG(DlgRet)  {    \
                                     pMe->m_eDlgRet = DlgRet; \
                                     (void) ISHELL_EndDialog(pMe->m_pShell);  \
                                 }
/*==============================================================================

==============================================================================*/
/*Main */
static boolean HandleMainDialogEvent(CNotebookMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

/*MainOption */
static boolean HandleMainOptionDialogEvent(CNotebookMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

/*Edit */
static boolean HandleEditDialogEvent(CNotebookMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

/*View */
static boolean HandleViewDialogEvent(CNotebookMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

/*EditOption */
static boolean HandleEditOptionDialogEvent(CNotebookMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

/*MessageBox */
static boolean  HandleMessegeBoxDialogEvent(CNotebookMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
    );

/*Dialog Time Out */
static void Notebook_DialogTimeout(void *pme);


/*==============================================================================
                                 oˉêy?¨ò?
==============================================================================*/


/*==============================================================================


==============================================================================*/
void Notebook_ShowDialog(CNotebookMenu *pMe,uint16  dlgResId)
{
	int nRet;

	if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
	{
		return;
	}

	nRet = ISHELL_CreateDialog(pMe->m_pShell,AEE_NOTEBOOK_RES_FILE,dlgResId,NULL);

	if (nRet != SUCCESS)
	{
	
	}
}

/*==============================================================================


==============================================================================*/
boolean Notebook_RouteDialogEvent(CNotebookMenu *pMe,
    AEEEvent eCode,
    uint16  wParam,
    uint32 dwParam
)
{
	if (NULL == pMe)
	{
	    return FALSE;
	}

	if (NULL == pMe->m_pActiveDlg)
	{
	    return FALSE;
	}
	
	switch (pMe->m_pActiveDlgID)
	{
		case IDD_DIALOG_MAIN:
		  return HandleMainDialogEvent(pMe,eCode,wParam,dwParam);
			
		case IDD_MAIN_OPTION:
			return HandleMainOptionDialogEvent(pMe,eCode,wParam,dwParam);

		case IDD_DIALOG_EDIT:
			return HandleEditDialogEvent(pMe,eCode,wParam,dwParam);

		case IDD_EDIT_OPTION:
			return HandleEditOptionDialogEvent(pMe,eCode,wParam,dwParam);
            
        case IDD_DIALOG_VIEW:
            return HandleViewDialogEvent(pMe,eCode,wParam,dwParam);
											
		case IDD_DIALOG_MSGBOX:
      return HandleMessegeBoxDialogEvent(pMe,eCode,wParam,dwParam);
            
			default:	
			break;	
	}
}

/*==============================================================================


==============================================================================*/
static boolean  HandleMainDialogEvent(CNotebookMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
	uint16 bytNewData;
    uint32 nListItems;
	PARAM_NOT_REF(dwParam)
		
	IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_MENU_MAIN);
	
        if (pMenu == NULL)
        {
            return FALSE;
        }
	
	switch (eCode)
	{
		case EVT_DIALOG_INIT:
            Notebook_BuildRecListMenu(pMe,pMenu);

            return TRUE;

            case EVT_DIALOG_START:

                IMENUCTL_SetOemProperties(pMenu,OEMMP_USE_MENU_STYLE);
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
                IMENUCTL_SetBottomBarType( pMenu, BTBAR_OPTION_BACK);
                if(pMe->m_ePreState == NOTEBOOK_STATE_INIT)
                {
                    uint16 selected;
                    //DBGPRINTF("ePreState == OTEBOOK_STATE_MAIN");
                    selected = IMENUCTL_GetItemID(pMenu, 0);
                    IMENUCTL_SetSel(pMenu,selected);
                    
                }
                else
                {
                    IMENUCTL_SetSel(pMenu,pMe->m_currDlgId);
                }
            
            (void) ISHELL_PostEvent( pMe->m_pShell,
                                                    AEECLSID_NOTEBOOK,
                                                    EVT_USER_REDRAW,
                                                    0,
                                                    0);
            return TRUE;

            case EVT_USER_REDRAW:
            if(IMENUCTL_GetItemCount(pMenu) > 0)
            {
                IMENUCTL_Redraw(pMenu);
            }
            else
            {
                DBGPRINTF("line 228 NotebookDialoghandler.c Idisplay_update by draw no record");
                Notebook_DrawNorecord(pMe,pMenu);
                NOTEBOOK_MENU_DRAW_BOTTOMBAR(BTBAR_ADD_BACK);
                IDISPLAY_Update(pMe->m_pDisplay);
            }
            return TRUE;

		case EVT_DIALOG_END:
			return TRUE;

		case EVT_KEY:
			
			switch(wParam)
			{
                        case AVK_CLR:
                            CLOSE_DIALOG(DLGRET_CANCELED)
                        return TRUE;
                    
                        case AVK_SELECT:
                        case AVK_INFO:
                            pMe->m_currDlgId = IMENUCTL_GetSel(pMenu);
                            {
                                char* data = 0;
                                if( !( data = Notebook_GetMenuItemName( pMe, pMenu)))
                                {
                                    pMe->m_eEntryFlag = NO_RECORD_STATE;
                                }                    
                            }
                            CLOSE_DIALOG(DLGRET_NOTEBOOK_MAIN_OPTION)
                        return TRUE;

                        default:
                        break;
                    }
            return TRUE;

            case EVT_COMMAND:
            pMe->m_currDlgId = IMENUCTL_GetSel(pMenu);
            {
                char* data = 0;
                if( !( data = Notebook_GetMenuItemName( pMe, pMenu)))
                {
                    pMe->m_eEntryFlag = NO_RECORD_STATE;

                }
            }            
            CLOSE_DIALOG(DLGRET_NOTEBOOK_MAIN_OPTION)
            return TRUE;

            default:
                break;
        }
        return FALSE;
} 

/*==============================================================================

==============================================================================*/
static boolean  HandleMainOptionDialogEvent(CNotebookMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
	uint16 bytNewData;
	PARAM_NOT_REF(dwParam)
		
	IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_MAIN_OPTION);
	
	if (pMenu == NULL)
	{
	    return FALSE;
	}
	
	switch (eCode)
	{
		case EVT_DIALOG_INIT:		
            IMENUCTL_AddItem(pMenu, AEE_NOTEBOOK_RES_FILE, IDS_ITEM_ADDNEW, IDS_ITEM_ADDNEW, NULL, 0);
            if(pMe->m_eEntryFlag != NO_RECORD_STATE)
            {
                IMENUCTL_AddItem(pMenu, AEE_NOTEBOOK_RES_FILE, IDS_ITEM_RENAME, IDS_ITEM_RENAME, NULL, 0);
                IMENUCTL_AddItem(pMenu, AEE_NOTEBOOK_RES_FILE, IDS_ITEM_DELETE, IDS_ITEM_DELETE, NULL, 0);
                IMENUCTL_AddItem(pMenu, AEE_NOTEBOOK_RES_FILE, IDS_ITEM_VIEW, IDS_ITEM_VIEW, NULL, 0);
                IMENUCTL_AddItem(pMenu, AEE_NOTEBOOK_RES_FILE, IDS_ITEM_DELETE_ALL, IDS_ITEM_DELETE_ALL, NULL, 0);
            }
			return TRUE;

		case EVT_DIALOG_START:
            IMENUCTL_SetPopMenuRect(pMenu);
            //IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
            IMENUCTL_SetOemProperties(pMenu,OEMMP_USE_MENU_STYLE);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetBottomBarType( pMenu, BTBAR_SELECT_BACK);
			(void) ISHELL_PostEvent( pMe->m_pShell,
			                         AEECLSID_NOTEBOOK,
			                         EVT_USER_REDRAW,
			                         0,
			                         0);
			return TRUE;

		case EVT_USER_REDRAW:
			IMENUCTL_Redraw(pMenu);
			return TRUE;

		case EVT_DIALOG_END:
			return TRUE;

		case EVT_KEY:
			
			switch(wParam)
			{
				case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_NOTEBOOK_MAIN)
    				return TRUE;
            
                case AVK_INFO:
        
                    return TRUE;

				default:
					break;
			}
	        	return TRUE;

		case EVT_COMMAND:
			
			switch (wParam)
			{
                        case IDS_ITEM_ADDNEW:
                        pMe->m_eEntryFlag = ADD_NEW_STATE;
                        MEMSET(pMe->m_NoteData.pNoteContent,0,sizeof(pMe->m_NoteData.pNoteContent));

                        CLOSE_DIALOG(DLGRET_NOTEBOOK_EDIT) 
                        break;

                case IDS_ITEM_VIEW:
                        pMe->m_eEntryFlag = VIEW_STATE;
                        Notebook_LoadData(pMe,pMe->m_FileName);    
                        CLOSE_DIALOG(DLGRET_NOTEBOOK_VIEW)
                        break;

                case IDS_ITEM_RENAME:
                    pMe->m_eEntryFlag = RENAME_EDIT_STATE;
                    CLOSE_DIALOG(DLGRET_NOTEBOOK_EDIT) 
                    break;
                    
                case IDS_ITEM_DELETE:
                    pMe->m_wMsgResID = IDS_WARNNING_DELETE;
                    pMe->m_eWarnningMsgBackFlag = MAIN_STATE_DEL;
                    CLOSE_DIALOG(DLGRET_MSGBOX)  
                    break;
                    
				case IDS_ITEM_DELETE_ALL:
                    pMe->m_wMsgResID = IDS_WARNNING_DEL_ALL;
                    pMe->m_eWarnningMsgBackFlag = MAIN_STATE_DEL_ALL;
                    CLOSE_DIALOG(DLGRET_MSGBOX)
                    break;
                    
				default:
					break;	
				}
			return TRUE;

		default:
			break;
	}
    	return FALSE;
} 

/*==============================================================================


==============================================================================*/
static boolean  HandleEditDialogEvent(CNotebookMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
	AEERect rc;
	uint16 buffer_len;
        AECHAR *pBuffer = NULL;
	AECHAR text[30] = {0};
	char fmt_str[30] = {0};
	PARAM_NOT_REF(dwParam)
	AECHAR EmptyTitle[2]={' ','\0'};
        AECHAR temp_aedata[128] = {0};
	ITextCtl *pText = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_TEXT_INPUT);

        if (pText == NULL)
        {
            return FALSE;
        }
        switch (eCode)
        {
                case EVT_DIALOG_INIT:
                    rc = pMe->m_rc;
                    rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
                    ITEXTCTL_SetRect(pText, &rc);

                    ITEXTCTL_SetProperties( pText,TP_MULTILINE | TP_FRAME | TP_FIXSETRECT | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT );
                    (void)ITEXTCTL_SetInputMode(pText,AEE_TM_CURRENT);

                    if(pMe->m_eEntryFlag != RENAME_EDIT_STATE)
                    {
                        ITEXTCTL_SetMaxSize( pText,512);
                    }
                    else
                    {
                        ITEXTCTL_SetMaxSize( pText,128);
                    }

                    (void)ITEXTCTL_SetTitle( pText,
                             NULL,
                             NULL,
                             EmptyTitle);
            

            if(pMe->m_eEntryFlag == RENAME_EDIT_STATE)
            {
               
                STRTOWSTR(pMe->m_NoteData.pNoteFileName,temp_aedata,sizeof(temp_aedata));

                if(WSTRLEN(temp_aedata) != 0)
                {
                    ITEXTCTL_SetText(pText, temp_aedata,-1);  
                }
            }
            else
            {
                if(WSTRLEN(pMe->m_NoteData.pNoteContent) != 0)
                {
                    ITEXTCTL_SetText(pText, pMe->m_NoteData.pNoteContent,-1);
                }
            }
            
            if(pMe->m_eEntryFlag == VIEW_STATE)
            {
                DBGPRINTF("Joe Edit Dialog View state 090219");
                ITEXTCTL_SetActive(pText,FALSE);
            }
            return TRUE;

                case EVT_DIALOG_START:
                (void) ISHELL_PostEvent( pMe->m_pShell,
                         AEECLSID_NOTEBOOK,
                         EVT_USER_REDRAW,
                         0,
                         0);

            return TRUE;

                case EVT_USER_REDRAW:
                    if(pMe->m_eEntryFlag != RENAME_EDIT_STATE)
                    {
                        if ( WSTRLEN(ITEXTCTL_GetTextPtr(pText)) > 0)
                        {
                            NOTEBOOK_MENU_DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
                        }
                        else
                        {
                            NOTEBOOK_MENU_DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
                        }
                    }
                    else
                    {
                        NOTEBOOK_MENU_DRAW_BOTTOMBAR(BTBAR_SAVE_BACK);
                    }

                    (void)ITEXTCTL_Redraw(pText);
                    IDISPLAY_Update(pMe->m_pDisplay);  
                    return TRUE;

		case EVT_DIALOG_END:

			return TRUE;

		case EVT_KEY:
			
			switch(wParam)
			{
                        case AVK_CLR:
                        if(pMe->m_eEntryFlag != RENAME_EDIT_STATE)
                        {
                            MEMSET(pMe->m_FileName,0,sizeof(pMe->m_FileName));
                            MEMSET(pMe->m_NoteData.pNoteContent,0,sizeof(pMe->m_NoteData.pNoteContent));
                        }
                        else
                        {
                                MEMSET(pMe->m_NoteData.pNoteFileName,0,sizeof(pMe->m_NoteData.pNoteFileName));
                        }
                        CLOSE_DIALOG(DLGRET_NOTEBOOK_MAIN)
                        return TRUE;
                    
                    case AVK_SELECT:
                    case AVK_INFO:
                    if(pMe->m_eEntryFlag != RENAME_EDIT_STATE)
                    {
                        if(pMe->m_eEntryFlag != VIEW_STATE)
                        {
                            pBuffer = WSTRDUP(ITEXTCTL_GetTextPtr(pText));
                            buffer_len = WSTRLEN(pBuffer);
                            WSTRCPY(pMe->m_NoteData.pNoteContent,pBuffer);     
                        }
                        CLOSE_DIALOG(DLGRET_NOTEBOOK_EDIT_OPTION)
                    }
                    else
                    {

                        char aBuffer[256] = {0};
                        pBuffer = WSTRDUP(ITEXTCTL_GetTextPtr(pText));
                        MEMSET(pMe->m_tempbuffer,0,sizeof(pMe->m_tempbuffer));
                        
                        WSTRTOSTR(pBuffer,aBuffer,sizeof(aBuffer));                        


                        if(STRLEN(aBuffer) == 0)
                        {
                            pMe->m_wMsgResID = IDS_NAME_NULL;
                            pMe->m_eWarnningMsgBackFlag = EDIT_STATE_RENAME;
                            CLOSE_DIALOG(DLGRET_MSGBOX)
                        }
                        else
                        {
                            Notebook_SetFullName(pMe,aBuffer,pMe->m_tempbuffer);
                            pMe->m_wMsgResID = IDS_WARNNING_RENAME;
                            pMe->m_eWarnningMsgBackFlag = EDIT_STATE_RENAME;
                            CLOSE_DIALOG(DLGRET_MSGBOX)
                        }
                    }
                    return TRUE;
                    
                        default:
                            break;
                }
                return TRUE;

                case EVT_COMMAND:
                    return TRUE;

                default:
                    break;
	}
	return FALSE;
} 

/*==============================================================================


==============================================================================*/
static boolean  HandleViewDialogEvent(CNotebookMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    AEERect rc;
    uint16 buffer_len;
    AECHAR *pBuffer = NULL;
    AECHAR text[30] = {0};
    char fmt_str[30] = {0};
    PARAM_NOT_REF(dwParam)
    AECHAR EmptyTitle[2]={' ','\0'};
    
    IStatic * pStatic = (IStatic*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_STATIC_VIEW);
            
    if (pStatic == NULL)
    {
        return FALSE;
    }
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            {
                AEERect rc;
            
                if (pMe == NULL)
                {
                    return TRUE;
                }
               
                SETAEERECT(&rc,  1, 1,
                            pMe->m_rc.dx-2,
                            pMe->m_rc.dy - GetBottomBarHeight(pMe->m_pDisplay)-1);
                ISTATIC_SetRect(pStatic, &rc);
            }
            
            // 设置静态文本控件属性
            ISTATIC_SetProperties(pStatic, ST_CENTERTITLE | ST_NOSCROLL | ST_DISPLATSMS);

            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent( pMe->m_pShell,
                         AEECLSID_NOTEBOOK,
                         EVT_USER_REDRAW,
                         0,
                         0);

            return TRUE;

        case EVT_USER_REDRAW:
            {
                AECHAR wszTitle[32] = {0};
                (void)ISTATIC_SetText(pStatic,
                        NULL,
                        pMe->m_NoteData.pNoteContent,
                        AEE_FONT_NORMAL,
                        AEE_FONT_NORMAL);                
            }
        
            ISTATIC_SetActive(pStatic, TRUE);
            (void) ISTATIC_Redraw(pStatic);
            
            // 绘制底条提示
            // Option       Back
            NOTEBOOK_MENU_DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);

            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);  
            IDISPLAY_Update(pMe->m_pDisplay); 
            return TRUE;

        case EVT_DIALOG_END:

            return TRUE;

        case EVT_KEY:
            
            switch(wParam)
            {
                case AVK_CLR:
                    //MEMSET(pMe->m_FileName,0,sizeof(pMe->m_FileName));
                    //MEMSET(pMe->m_NoteData.pNoteContent,0,sizeof(pMe->m_NoteData.pNoteContent));
                    CLOSE_DIALOG(DLGRET_NOTEBOOK_MAIN)
                    return TRUE;
                    
                case AVK_SELECT:
                case AVK_INFO:
                    //if(pMe->m_eEntryFlag != VIEW_STATE)
                    //{
                    //    pBuffer = WSTRDUP(ITEXTCTL_GetTextPtr(pText));
                    //    buffer_len = WSTRLEN(pBuffer);
                    //    WSTRCPY(pMe->m_NoteData.pNoteContent,pBuffer);     
                    //}
                    pMe->m_eEntryFlag = VIEW_EDIT_STATE;
                    CLOSE_DIALOG(DLGRET_NOTEBOOK_EDIT_OPTION)
                    return TRUE;
                    
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            return TRUE;
            
        default:
            break;
    }
    return FALSE;
}

/*==============================================================================


==============================================================================*/
static boolean  HandleEditOptionDialogEvent(CNotebookMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
	uint16 bytNewData;
	PARAM_NOT_REF(dwParam)
		
	IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_EDIT_OPTION);
	
	
	if (pMenu == NULL)
	{
	    return FALSE;
	}
	
	switch (eCode)
	{
                    case EVT_DIALOG_INIT:
                    if(pMe->m_eEntryFlag != ADD_NEW_STATE)
                    {
                        IMENUCTL_AddItem(pMenu, AEE_NOTEBOOK_RES_FILE, IDS_ITEM_ADDNEW, IDS_ITEM_ADDNEW, NULL, 0);
                        IMENUCTL_AddItem(pMenu, AEE_NOTEBOOK_RES_FILE, IDS_ITEM_DELETE, IDS_ITEM_DELETE, NULL, 0);
                        IMENUCTL_AddItem(pMenu, AEE_NOTEBOOK_RES_FILE, IDS_ITEM_EDIT, IDS_ITEM_EDIT, NULL, 0);
                    }
                
                    if(pMe->m_eEntryFlag != VIEW_STATE)
                    {
                        IMENUCTL_AddItem(pMenu, AEE_NOTEBOOK_RES_FILE, IDS_ITEM_SAVE, IDS_ITEM_SAVE, NULL, 0);
                    }                   
                    return TRUE;

                    case EVT_DIALOG_START:
   
                    IMENUCTL_SetPopMenuRect(pMenu);
                    // IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
                    IMENUCTL_SetOemProperties(pMenu,OEMMP_USE_MENU_STYLE);
                    IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);               
                    IMENUCTL_SetBottomBarType( pMenu, BTBAR_SELECT_BACK);
    			(void) ISHELL_PostEvent(pMe->m_pShell,
    			                         AEECLSID_NOTEBOOK,
    			                         EVT_USER_REDRAW,
    			                         0,
    			                         0);
			return TRUE;

		case EVT_USER_REDRAW:
			IMENUCTL_Redraw(pMenu);
			return TRUE;

		case EVT_DIALOG_END:
			return TRUE;

		case EVT_KEY:
			
			switch(wParam)
			{
				case AVK_CLR:
                    if(pMe->m_eEntryFlag == VIEW_EDIT_STATE)
                    {
                        CLOSE_DIALOG(DLGRET_NOTEBOOK_VIEW)
                    }
                    else
                    {
                        CLOSE_DIALOG(DLGRET_NOTEBOOK_EDIT)
                    }
				    return TRUE;
            
                case AVK_INFO:
        
                    return TRUE;

				default:
					break;
			}
        	return TRUE;

		case EVT_COMMAND:					
			pMe->m_currDlgId = IMENUCTL_GetSel(pMenu);
			switch (wParam)
			{
                    case IDS_ITEM_ADDNEW:
                    
                    pMe->m_eEntryFlag = ADD_NEW_STATE;
                    MEMSET(pMe->m_NoteData.pNoteContent,0,sizeof(pMe->m_NoteData.pNoteContent));
                    CLOSE_DIALOG(DLGRET_NOTEBOOK_EDIT)
                        
                    break;
                    
                    case IDS_ITEM_DELETE:
                        
                    pMe->m_wMsgResID = IDS_WARNNING_DELETE;
                    pMe->m_eWarnningMsgBackFlag = VIEW_STATE_DEL;
                    CLOSE_DIALOG(DLGRET_MSGBOX)

                    #if 0
                    if(IFILEMGR_Remove( pMe->m_pFileMgr, pMe->m_FileName) == SUCCESS)
                    {
                        

                    }
                    else
                    {
                        pMe->m_nLastOperationError = IFILEMGR_GetLastError( pMe->m_pFileMgr);
                        DBGPRINTF("result = %d",pMe->m_nLastOperationError);
                        //return FALSE;
                    }
                    CLOSE_DIALOG(DLGRET_NOTEBOOK_MAIN)
                    #endif
                    
                    break;
                    
                case IDS_ITEM_EDIT:
                    CLOSE_DIALOG(DLGRET_NOTEBOOK_EDIT)
                    break;
                    
                    case IDS_ITEM_SAVE:
                    if(pMe->m_eEntryFlag == VIEW_EDIT_STATE)
                    {   
                        pMe->m_wMsgResID = IDS_WARNNING_SAVE;
                        pMe->m_eWarnningMsgBackFlag = VIEW_STATE_SAVE;
                        CLOSE_DIALOG(DLGRET_MSGBOX)
                    }
                    else
                    {
                        pMe->m_wMsgResID = IDS_WARNNING_ADD;
                        pMe->m_eWarnningMsgBackFlag = EDIT_STATE_ADD_NEW;
                        CLOSE_DIALOG(DLGRET_MSGBOX)
                    }
					break;
				}			
			return TRUE;

		default:
			break;
	}
    	return FALSE;
} 
 /*==============================================================================
函数：
       HandlePersonalInputDialogEvent
说明：
       IDD_DIALOG_PEDOMETER_OPTION 对话框事件处理函数

参数：
       pMe [in]：指向SportSApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
      FALSE：传入事件被忽略。

备注：

==============================================================================*/
 static boolean  HandleMessegeBoxDialogEvent(CNotebookMenu *pMe,
	 AEEEvent eCode,
	 uint16 wParam,
	 uint32 dwParam
	 )
{
	static IStatic * pStatic = NULL;

	if (NULL == pStatic)
	{
	    AEERect rect = {0};
	    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
	                                             AEECLSID_STATIC,
	                                             (void **)&pStatic))
	    
	    {
	        return FALSE;
	    }        
	    ISTATIC_SetRect(pStatic, &rect);  
	}

	if ((NULL == pStatic) ||(NULL == pMe))
	{
	    return FALSE;
	}

	switch(eCode)
	{
	    case EVT_DIALOG_INIT:
	        return TRUE;

	    case EVT_DIALOG_START:
	        (void) ISHELL_PostEventEx(pMe->m_pShell, 
	                                EVTFLG_ASYNC,
	                                AEECLSID_NOTEBOOK,
	                                EVT_USER_REDRAW,
	                                0, 
	                                0);

	        return TRUE;

	    case EVT_USER_REDRAW:
	        {
	            PromptMsg_Param_type  Msg_Param={0};
	            AECHAR  wstrText[MSGBOX_MAXTEXTLEN] = {(AECHAR)'\0'};

	            (void)ISHELL_LoadResString(pMe->m_pShell,
	                            AEE_NOTEBOOK_RES_FILE,                                
	                            pMe->m_wMsgResID,
	                            wstrText,
	                            sizeof(wstrText));
	                            
	            //Msg_Param.ePMsgType = MESSAGE_INFORMATION;
	            Msg_Param.pwszMsg = wstrText;

		        //Msg_Param.eBBarType = BTBAR_NONE;
                Msg_Param.ePMsgType = MESSAGE_CONFIRM;
                Msg_Param.eBBarType =  BTBAR_OK_CANCEL;

                if((STRLEN(pMe->m_tempbuffer) == 0)&&(pMe->m_eWarnningMsgBackFlag == EDIT_STATE_RENAME))
                {
                    Msg_Param.ePMsgType = MESSAGE_WARNNING;
                    Msg_Param.eBBarType =  BTBAR_NONE;                    
                }

	            
	            DrawPromptMessage(pMe->m_pDisplay, pStatic, &Msg_Param);
	        }

	        IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);


                //#if 0
                if((STRLEN(pMe->m_tempbuffer) == 0)&&(pMe->m_eWarnningMsgBackFlag == EDIT_STATE_RENAME))
                {
                    (void) ISHELL_SetTimer(pMe->m_pShell,
                                    PROMPTMSG_TIMER,
                                    Notebook_DialogTimeout,
                                    pMe);

                    }



	        return TRUE;

	    case EVT_DIALOG_END:

                if((STRLEN(pMe->m_tempbuffer) == 0)&&(pMe->m_eWarnningMsgBackFlag == EDIT_STATE_RENAME))
                {
                    (void) ISHELL_CancelTimer(pMe->m_pShell,Notebook_DialogTimeout, pMe);
                }

                ISTATIC_Release(pStatic);
                pStatic = NULL;

	        return TRUE;

	    case EVT_KEY:
                switch(wParam)
                {
                    case AVK_CLR:
                        switch(pMe->m_eWarnningMsgBackFlag)
                        {
                            case MAIN_STATE_DEL:
                            case MAIN_STATE_DEL_ALL:
                            
                                CLOSE_DIALOG(DLGRET_NOTEBOOK_MAIN)                                
                                break;
                                
                            case EDIT_STATE_ADD_NEW:
                            case VIEW_STATE_SAVE:
                                CLOSE_DIALOG(DLGRET_NOTEBOOK_EDIT)
                                break;
                                
                            case EDIT_STATE_RENAME:
                                if(STRLEN(pMe->m_tempbuffer) != 0)
                                {
                                    CLOSE_DIALOG(DLGRET_NOTEBOOK_EDIT)
                                }

                                break;
                            case VIEW_STATE_DEL:
                                if(pMe->m_eEntryFlag == VIEW_EDIT_STATE)
                                {
                                    CLOSE_DIALOG(DLGRET_NOTEBOOK_EDIT)
                                }
                                else
                                {
                                    CLOSE_DIALOG(DLGRET_NOTEBOOK_VIEW)
                                }
                                break;
                                
                                default:
                                    
                                    break;
                        }

                        break;
                            
                    case AVK_SELECT:
                    case AVK_INFO:
                            switch(pMe->m_eWarnningMsgBackFlag)
                            {
                                case MAIN_STATE_DEL:
                                    if( IFILEMGR_Remove( pMe->m_pFileMgr, pMe->m_FileName) == SUCCESS)
                                    {
                                        
                                    }
                                    else
                                    {
                                        pMe->m_nLastOperationError = IFILEMGR_GetLastError( pMe->m_pFileMgr);

                                        DBGPRINTF("result = %d",pMe->m_nLastOperationError);
                                        //return FALSE;
                                    }
                                    CLOSE_DIALOG(DLGRET_NOTEBOOK_MAIN)                                                 
                                    break;
                                 case MAIN_STATE_DEL_ALL:
                                    Notebook_DeleteAllRec(pMe);
                                    CLOSE_DIALOG(DLGRET_NOTEBOOK_MAIN)
                                    break;
                                 case EDIT_STATE_ADD_NEW:
                                    Notebook_CreateNewFile(pMe);
                                    Notebook_SaveData(pMe,pMe->m_FileName);
                                    CLOSE_DIALOG(DLGRET_NOTEBOOK_MAIN)                                    
                                    break;
                                 case VIEW_STATE_SAVE:
                                    Notebook_SaveData(pMe,pMe->m_FileName);
                                    CLOSE_DIALOG(DLGRET_NOTEBOOK_MAIN)
                                    break;
                                 case EDIT_STATE_RENAME:
                                    if(STRLEN(pMe->m_tempbuffer) != 0)
                                    {
                                        if( IFILEMGR_Rename( pMe->m_pFileMgr, pMe->m_FileName, pMe->m_tempbuffer) != SUCCESS)
                                        {
                                            MEMSET(pMe->m_NoteData.pNoteFileName,0,sizeof(pMe->m_NoteData.pNoteFileName));
                                        }                        
                                        
                                        CLOSE_DIALOG(DLGRET_NOTEBOOK_MAIN)
                                    }
                                    
                                    break;
                                  case VIEW_STATE_DEL:
                                        if(IFILEMGR_Remove( pMe->m_pFileMgr, pMe->m_FileName) == SUCCESS)
                                        {
                                            
                                        }
                                        else
                                        {
                                            pMe->m_nLastOperationError = IFILEMGR_GetLastError( pMe->m_pFileMgr);
                                            DBGPRINTF("result = %d",pMe->m_nLastOperationError);
                                            //return FALSE;
                                        }
                                        CLOSE_DIALOG(DLGRET_NOTEBOOK_MAIN)
                                        break;
                                    default:
                                        break;
                                }
                            break;

                        default:
                            break;
                    }
	        return TRUE;
	    
	    case EVT_DISPLAYDIALOGTIMEOUT:
            CLOSE_DIALOG(DLGRET_NOTEBOOK_MAIN)
	        return TRUE;

	    default:
	        break;
	}

	return FALSE;
}


/*==============================================================================
函数:
       SettingMenu_DialogTimeout

说明:
       定时器回调函数。主要用于自动关闭消息对话框。

参数:
       pme [in]：void *类型指针，暗指CSettingMenu结构指针。

返回值:
       无。

备注:

==============================================================================*/
static void Notebook_DialogTimeout(void *pme)
{
    CNotebookMenu *pMe = (CNotebookMenu*)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent(pMe->m_pShell,
                        AEECLSID_NOTEBOOK,
                        EVT_DISPLAYDIALOGTIMEOUT,
                        0,
                        0);
}

#endif
