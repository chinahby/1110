/*==============================================================================
// 文件：
//        ContAppDialogHandler.c
//        2004-06-09 jyh草拟版本(Draft Version)
//        版权所有(c) 2004 Anylook
//        
// 文件说明：
//        
// 作者：2004-06-09
// 创建日期：jyh
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时    间      修   改   人     修改内容、位置及原因
--------      -----------     -----------------------------------------------
04-06-09      jyh             初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "ContApp_priv.h"
#ifdef FEATURE_USES_ZI
#include "OEMZIText.h"
#else
#include "OEMText.h"
#endif
//#define CONTAPP_STR2KEY_INCLUDE
//#include "ContApp_Str2Key.h"

#include "ContAppBuildMenu.h"
#include "ContAppCommon.h"
#include "SoundMenu.h"

#ifdef FEATURE_SUPPORT_VCARD
#include "ContApp_vCard.h"
#endif

/*==============================================================================
                                 宏定义和常数
==============================================================================*/


/*==============================================================================
                                 类型定义
==============================================================================*/

/*lint -e506 -e774 -e737 -e715*/
/*==============================================================================
                                 函数声明
==============================================================================*/
/////////////////
//Helper function
//////
static void     CContApp_HandleDialogTimer(void *pUser);

/////////////////
//Dialog Event handler
/////////
// 对话框 IDD_MSGBOX 事件处理函数
static boolean  CContApp_HandleMsgBoxDlgEvent( CContApp  *pMe,
                                               AEEEvent   eCode,
                                               uint16     wParam,
                                               uint32     dwParam);

// 对话框 IDD_YESNO 事件处理函数
static boolean  CContApp_HandleYesNoDlgEvent( CContApp  *pMe,
                                              AEEEvent   eCode,
                                              uint16     wParam,
                                              uint32     dwParam);

// 对话框 IDD_LIST 事件处理函数
static boolean  CContApp_HandleListDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam);

// 对话框 IDD_INPUTFLD 事件处理函数
static boolean  CContApp_HandleInputFldDlgEvent( CContApp  *pMe,
                                                 AEEEvent   eCode,
                                                 uint16     wParam,
                                                 uint32     dwParam);

// 对话框 IDD_ADDNEW 事件处理函数
static boolean  CContApp_HandleAddNewDlgEvent( CContApp  *pMe,
                                               AEEEvent   eCode,
                                               uint16     wParam,
                                               uint32     dwParam);

// 对话框 IDD_OPTS 事件处理函数
static boolean  CContApp_HandleOptsDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam);

// 对话框 IDD_MAINMENU  事件处理函数
static boolean  CContApp_HandleMainMenuDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam);


// 对话框 IDD_VIEW 事件处理函数
static boolean  CContApp_HandleViewDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam);

// 对话框 IDD_FLDOPTS 事件处理函数
static boolean  CContApp_HandleFldOptsDlgEvent( CContApp  *pMe,
                                                AEEEvent   eCode,
                                                uint16     wParam,
                                                uint32     dwParam);

// 对话框 IDD_FIND 事件处理函数
static boolean  CContApp_HandleFindDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam);

// 对话框 IDD_SEARCH 事件处理函数
static boolean  CContApp_HandleSearchDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam);

// 对话框 IDD_MANAGEMENT 事件处理函数
static boolean  CContApp_HandleManagementDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam);

// 对话框 IDD_DELETE_SELECT 事件处理函数
static boolean  CContApp_HandleDeleteSelectDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam);

// 对话框 IDD_POPNUMFLD 事件处理函数
static boolean  CContApp_HandlePopNumFldDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam);
// 对话框 IDD_POPNUMFLD 事件处理函数
static boolean  CContApp_HandleNunFldDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam);

// 对话框 IDD_SELECTOPT 事件处理函数
static boolean  CContApp_HandleSelectOptDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam);

// 对话框 IDD_NUMFLDVIEW 事件处理函数
static boolean  CContApp_HandleNunFldViewDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam);


// 对话框 IDD_SEARCHNAME 事件处理函数
static boolean  CContApp_HandleSearchNameDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam);


// 对话框 IDD_GROUP 事件处理函数
static boolean  CContApp_HandleGroupDlgEvent( CContApp  *pMe,
                                              AEEEvent   eCode,
                                              uint16     wParam,
                                              uint32     dwParam);                                              

#ifdef FEATURE_RUIM_PHONEBOOK
// 对话框 IDD_POSITION 事件处理函数
static boolean  CContApp_HandlePositionDlgEvent( CContApp  *pMe,
                                                 AEEEvent   eCode,
                                                 uint16     wParam,
                                                 uint32     dwParam);

// 对话框 IDD_COPYMOVE 事件处理函数
static boolean  CContApp_HandleCopyMoveDlgEvent( CContApp  *pMe,
                                                 AEEEvent   eCode,
                                                 uint16     wParam,
                                                 uint32     dwParam);

// 对话框 IDD_COPY 事件处理函数
static boolean  CContApp_HandleCopyDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam);

#endif// FEATURE_RUIM_PHONEBOOK

//// 对话框 IDD_RING 事件处理函数
//static boolean  CContApp_HandleRingDlgEvent( CContApp  *pMe,
//                                             AEEEvent   eCode,
//                                             uint16     wParam,
//                                             uint32     dwParam);

// 对话框 IDD_ONOFF 事件处理函数
static boolean  CContApp_HandleOnOffDlgEvent( CContApp  *pMe,
                                              AEEEvent   eCode,
                                              uint16     wParam,
                                              uint32     dwParam);

// 对话框 IDD_ONEDIAL 事件处理函数
static boolean  CContApp_HandleOneDialDlgEvent( CContApp  *pMe,
                                                AEEEvent   eCode,
                                                uint16     wParam,
                                                uint32     dwParam);

// 对话框 IDD_ONEDIALOPT 事件处理函数
static boolean  CContApp_HandleOneDialOptDlgEvent( CContApp  *pMe,
                                                   AEEEvent   eCode,
                                                   uint16     wParam,
                                                   uint32     dwParam);

// 对话框 IDD_ONEDIAL_SET_NUM_SEL 事件处理函数
static boolean  CContApp_HandleOneDialNumFldSelDlgEvent( CContApp  *pMe,
                                                   AEEEvent   eCode,
                                                   uint16     wParam,
                                                   uint32     dwParam);

// 对话框 IDD_CAPACITY 事件处理函数
static boolean  CContApp_HandleCapacityDlgEvent( CContApp  *pMe,
                                                 AEEEvent   eCode,
                                                 uint16     wParam,
                                                 uint32     dwParam);

// 对话框 IDD_EDIT 事件处理函数
static boolean  CContApp_HandleEditDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam);

#ifdef FEATURE_RUIM_PHONEBOOK
// 对话框 IDD_SETTING 事件处理函数
static boolean  CContApp_HandleSettingDlgEvent( CContApp  *pMe,
                                                AEEEvent   eCode,
                                                uint16     wParam,
                                                uint32     dwParam);
#endif /* FEATURE_RUIM_PHONEBOOK */

// 对话框 IDD_INPUT 事件处理函数
static boolean  CContApp_HandleInputDlgEvent( CContApp  *pMe,
                                              AEEEvent   eCode,
                                              uint16     wParam,
                                              uint32     dwParam);

// 对话框 IDD_DELETE 事件处理函数
static boolean  CContApp_HandleDeleteDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam);

// 对话框 IDD_SELECT 事件处理函数
static boolean  CContApp_HandleSelectDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam);

// 对话框 IDD_DETAIL 事件处理函数
static boolean  CContApp_HandleDetailDlgEvent( CContApp  *pMe,
                                                 AEEEvent   eCode,
                                                 uint16     wParam,
                                                 uint32     dwParam);
/*智能查找联系人处理函数，在联系人列表，删除/复制/选择联系人列表中输入关键字快速查找联系人*/
static boolean CContApp_SmartMenuHandle( CContApp *pMe, 
                                         IMenuCtl *pMenuCtl,
                                         ITextCtl *pTextCtl,
                                         AEEEvent  eCode,
                                         uint16    wParam);
/*给智能查找输入框前加个当前的输入法图标*/
static void  CContApp_DrawIMEIcon(ITextCtl *pTextCtl, IDisplay *pIDisplay);
/*快速查找时，当为multitap输入时，定时器的回调函数，以便设置焦点。*/
void CContApp_SmartMenuSetFocus(void *pUser);
static boolean  CContApp_HandleCopyingDlgEvent( CContApp  *pMe,
                                                AEEEvent   eCode,
                                                uint16     wParam,
                                                uint32     dwParam);

// 对话框 IDD_GROUPOPT 事件处理函数
static boolean  CContApp_HandleGroupOptDlgEvent( CContApp  *pMe,
                                                   AEEEvent   eCode,
                                                   uint16     wParam,
                                                   uint32     dwParam);

// 对话框 IDD_GROUPOPT_EDIT 事件处理函数
static boolean  CContApp_HandleGroupOptEditDlgEvent( CContApp  *pMe,
                                                   AEEEvent   eCode,
                                                   uint16     wParam,
                                                   uint32     dwParam);

// 对话框 IDD_DELETING 事件处理函数
static boolean  CContApp_HandleDeletingDlgEvent( CContApp  *pMe,
                                                   AEEEvent   eCode,
                                                   uint16     wParam,
                                                   uint32     dwParam);

static boolean  CContApp_HandleSelectRecordDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam);

static boolean  CContApp_HandleDetailMultiDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam);

static void CContApp_ShowEditItem(CContApp  *pMe, IMenuCtl  *pMenuCtl, ITextCtl *pTextCtl);
static boolean CContApp_SaveLocal_Input(CContApp  *pMe, ITextCtl *pTextCtl);
static void CContApp_ShowGroupSelect(CContApp  *pMe, IMenuCtl  *pMenuCtl, IMenuCtl  *groupList);
static void CContApp_DrawNorecord(CContApp *pMe, IMenuCtl *pMenuCtl);
static void CContApp_SetGroupItemText(CContApp *pMe, IMenuCtl *pMenuCtl);
static void CContApp_SetEditGroup(CContApp *pMe, IMenuCtl *pMenuCtl, boolean onlySaveValue, int selectGroup);
static void CContApp_SetMenuItemText(CContApp *pMe, IMenuCtl *pMenuCtl, int selectEdit);
static void CContApp_SetInputMode(CContApp *pMe);

//Add By zzg 2011_12_15
static boolean CContApp_CheckNumberIsInValid(CContApp  *pMe, ITextCtl *pTextCtl);
static boolean CContApp_Search_HasExceptionalChar(AECHAR* szContNumber);
//Add End
static void drawImage( CContApp *pMe, char *resFile, int16 resId, int x, int y);

#ifdef FEATURE_OEMOMH 
static boolean  CContApp_HandleEmergencyCallDlgEvent( CContApp  *pMe,
                                                AEEEvent   eCode,
                                                uint16     wParam,
                                                uint32     dwParam);
#endif

#ifdef FEATURE_SUPPORT_BT_APP
//#define DEFAULT_VCARD_NAME  "fs:/address/vCard.vcf"
//#define DEFAULT_VCARD_SENDNAME ("F"DEFAULT_VCARD_NAME)

//static boolean BTApp_OPPCreateVCard(CContApp *pMe);
//static boolean BTApp_OPPReadVCard(CContApp *pMe);
#endif
/*==============================================================================
                                 全局数据
==============================================================================*/
#define KEYEVENT(e)                 ((e) == EVT_KEY || (e) == EVT_KEY || (e) == EVT_KEY_PRESS || (e) == EVT_KEY_RELEASE)

/*==============================================================================
                                 本地（静态）数据
==============================================================================*/


/*==============================================================================
                                 函数定义
==============================================================================*/
/*==============================================================================
函数:
       CContApp_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向ContApp Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       SUCCESS: 创建对话框成功
       非SUCCESS: 创建对话框失败

备注:
       

==============================================================================*/
int CContApp_ShowDialog(CContApp *pMe, uint16  dlgResId)
{
    int nRet;
    
    // At most one dialog open at once
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // Looks like there is one dialog already opened.
        // Flag an error an return without doing anything.
        MSG_FATAL("Try to create a dialog without closing the previous",0,0,0);
        return EFAILED;
    }
    
    MSG_FATAL("Show cont dialog %d", dlgResId,0,0);
    
    nRet = ISHELL_CreateDialog( pMe->m_pShell, 
                                CONTAPP_RES_FILE_LANG,
                                dlgResId, 
                                NULL);
    if(pMe->m_pIAnn != NULL)
    {
        IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
    }
#if defined(AEE_STATIC)
    if (nRet != SUCCESS)
    {
        MSG_FATAL("Failed to create the dialog in the ContApp applet",0,0,0);
    }
#endif

    return nRet;
}// CContApp_ShowDialog

/*==============================================================================
函数:
    CContApp_ShowMsgBox

说明:
    函数由状态处理函数用来唤起消息对话框，显示消息提示用户。

参数:
    pMe [in]: 指向 Contact Applet 对象结构的指针。该结构包含小程序的特定信息。
    wTextResId [in]: 消息文本资源 ID。

返回值:
    none

备注:

==============================================================================*/
int CContApp_ShowMsgBox(CContApp *pMe, uint16 wTextResId)
{
    pMe->m_wMsgResID = wTextResId;
    
    // 创建消息对话框.
    return CContApp_ShowDialog(pMe, IDD_MSGBOX);
}

/*==============================================================================
函数:
       CContApp_ShowYesNoDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向CContApp Applet对象结构的指针,该结构包含小程序的特定信息.
       msgResId [in]：对话框字符串资源ID，函数根据该ID显示提示内容。
       msg : 指向需要显示的字符串，优先于msgResId
       msglen: 现实的字符串长度wstrlen而来.-1 Auto caculate the lenght

返回值:
       int : SUCCESS if create dialog

备注:
       

==============================================================================*/
int  CContApp_ShowYesNoDialog( CContApp   *pMe, 
                               uint16      msgResId ,
                               AECHAR     *msg, 
                               int         msglen )
{
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    FREEIF(pMe->m_pMsgBox_Msg);
    
    if( NULL == msg )
    {
        pMe->m_pMsgBox_Msg = (AECHAR *)MALLOC(sizeof(AECHAR)*MAX_LEN_MSGBOX);
        if( NULL == pMe->m_pMsgBox_Msg )
        {
            return EFAILED;
        }
        
        msglen = ISHELL_LoadResString ( pMe->m_pShell, 
                                        CONTAPP_RES_FILE_LANG, 
                                        msgResId,
                                        pMe->m_pMsgBox_Msg,
                                        sizeof(AECHAR)*(MAX_LEN_MSGBOX -1));
        if( msglen <= 0 )
        {
            return EFAILED;
        }
    }
    else
    {
        if( msglen == -1 )
        {
            msglen = WSTRLEN(msg);
        }
        
        if( msglen <= 0 )
        {
            return EFAILED;
        }
        
        pMe->m_pMsgBox_Msg = (AECHAR *)MALLOC(sizeof(AECHAR)*((uint32)msglen+1));
        (void)MEMCPY(pMe->m_pMsgBox_Msg, msg, sizeof(AECHAR)*(uint32)msglen);
        pMe->m_pMsgBox_Msg[msglen] = (AECHAR)'\0';
    }
    
    return CContApp_ShowDialog( pMe, IDD_YESNO );
}// CContApp_ShowYesNoDialog

/*==============================================================================
函数:
       CContApp_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向ContApp Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean CContApp_RouteDialogEvent( CContApp  *pMe,
                                   AEEEvent   eCode,
                                   uint16     wParam,
                                   uint32     dwParam)
{
    MSG_FATAL("CContApp_RouteDialogEvent Start",0,0,0);
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif

#ifdef FEATURE_GIVEUP_EDIT
    if(eCode == EVT_KEY && wParam == AVK_CONTACTS_GIVEUP_EDIT)
    {
        if(STATE_EDIT != pMe->m_eCurState && STATE_EDIT_INPUT != pMe->m_eCurState)
        {
            MSG_FATAL("CContApp_RouteDialogEvent ISHELL_CloseApplet",0,0,0);
            ISHELL_CloseApplet(pMe->m_pShell, TRUE);
            return TRUE;
        }
        else
        {
            pMe->m_bIsEndKey = TRUE;
        }
    }   
#endif
    if (NULL == pMe->m_pActiveDlg)
    {
        return FALSE;
    }
    
    switch (pMe->m_pActiveDlgID)
    {
        case IDD_MSGBOX:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_MSGBOX",0,0,0);
            return CContApp_HandleMsgBoxDlgEvent(pMe,eCode,wParam,dwParam);
            
        case IDD_YESNO:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_YESNO",0,0,0);
            return CContApp_HandleYesNoDlgEvent(pMe,eCode,wParam,dwParam);
            
        case IDD_LIST:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_LIST",0,0,0);
            return CContApp_HandleListDlgEvent(pMe,eCode,wParam,dwParam);
            
        case IDD_INPUTFLD:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_INPUTFLD",0,0,0);
            return CContApp_HandleInputFldDlgEvent(pMe,eCode,wParam,dwParam);
            
        case IDD_ADDNEW:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_ADDNEW",0,0,0);
            return CContApp_HandleAddNewDlgEvent(pMe,eCode,wParam,dwParam);
            
        case IDD_OPTS:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_OPTS",0,0,0);
            return CContApp_HandleOptsDlgEvent(pMe,eCode,wParam,dwParam);

        case IDD_MAINMENU:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_MAINMENU",0,0,0);
            return CContApp_HandleMainMenuDlgEvent(pMe,eCode,wParam,dwParam);
            
        case IDD_VIEW:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_VIEW",0,0,0);
            return CContApp_HandleViewDlgEvent(pMe,eCode,wParam,dwParam);
            
        case IDD_FLDOPTS:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_FLDOPTS",0,0,0);
            return CContApp_HandleFldOptsDlgEvent(pMe,eCode,wParam,dwParam);
            
        case IDD_FIND:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_FIND",0,0,0);
            return CContApp_HandleFindDlgEvent(pMe,eCode,wParam,dwParam);

        case IDD_SEARCH:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_SEARCH",0,0,0);
            return CContApp_HandleSearchDlgEvent(pMe,eCode,wParam,dwParam);     

        case IDD_MANAGEMENT:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_MANAGEMENT",0,0,0);
            return CContApp_HandleManagementDlgEvent(pMe,eCode,wParam,dwParam);     

        case IDD_DELETE_SELECT:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_DELETE_SELECT",0,0,0);
            return CContApp_HandleDeleteSelectDlgEvent(pMe,eCode,wParam,dwParam);   

        case IDD_POPNUMFLD:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_POPNUMFLD",0,0,0);
            return CContApp_HandlePopNumFldDlgEvent(pMe,eCode,wParam,dwParam);  

        case IDD_VIEW_NUMFLD:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_VIEW_NUMFLD",0,0,0);
            return CContApp_HandleNunFldDlgEvent(pMe,eCode,wParam,dwParam);  

        case IDD_SELECTOPT:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_SELECTOPT",0,0,0);
            return CContApp_HandleSelectOptDlgEvent(pMe,eCode,wParam,dwParam);

        case IDD_NUMFLDVIEW:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_NUMFLDVIEW",0,0,0);
            return CContApp_HandleNunFldViewDlgEvent(pMe,eCode,wParam,dwParam);

        case IDD_SEARCHNAME:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_SEARCHNAME",0,0,0);
            return CContApp_HandleSearchNameDlgEvent(pMe,eCode,wParam,dwParam);

        case IDD_GROUP:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_GROUP",0,0,0);
            return CContApp_HandleGroupDlgEvent(pMe,eCode,wParam,dwParam);

#ifdef FEATURE_RUIM_PHONEBOOK
        case IDD_POSITION:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_POSITION",0,0,0);
            return CContApp_HandlePositionDlgEvent(pMe,eCode,wParam,dwParam);

        case IDD_COPYMOVE:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_COPYMOVE",0,0,0);
            return CContApp_HandleCopyMoveDlgEvent(pMe,eCode,wParam,dwParam);
            
        case IDD_COPY:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_COPY",0,0,0);
            return CContApp_HandleCopyDlgEvent(pMe,eCode,wParam,dwParam);
#endif /* FEATURE_RUIM_PHONEBOOK*/
           
//        case IDD_RING:
//            return CContApp_HandleRingDlgEvent(pMe,eCode,wParam,dwParam);
            
        case IDD_ONOFF:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_ONOFF",0,0,0);
            return CContApp_HandleOnOffDlgEvent(pMe,eCode,wParam,dwParam);

        case IDD_ONEDIAL:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_ONEDIAL",0,0,0);
            return CContApp_HandleOneDialDlgEvent(pMe,eCode,wParam,dwParam);
            
        case IDD_ONEDIALOPT:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_ONEDIALOPT",0,0,0);
            return CContApp_HandleOneDialOptDlgEvent(pMe,eCode,wParam,dwParam);

        case IDD_ONEDIAL_SET_NUM_SEL:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_ONEDIAL_SET_NUM_SEL",0,0,0);
            return CContApp_HandleOneDialNumFldSelDlgEvent(pMe,eCode,wParam,dwParam);
            
        case IDD_CAPACITY:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_CAPACITY",0,0,0);
            return CContApp_HandleCapacityDlgEvent(pMe,eCode,wParam,dwParam);
        
        case IDD_EDIT:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_EDIT",0,0,0);
            return CContApp_HandleEditDlgEvent(pMe,eCode,wParam,dwParam);

#ifdef FEATURE_RUIM_PHONEBOOK
        case IDD_SETTING:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_SETTING",0,0,0);
            return CContApp_HandleSettingDlgEvent(pMe,eCode,wParam,dwParam);
#endif /* FEATURE_RUIM_PHONEBOOK */            
        
        case IDD_INPUT:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_INPUT",0,0,0);
            return CContApp_HandleInputDlgEvent(pMe,eCode,wParam,dwParam);

        case IDD_DELETE:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_DELETE",0,0,0);
            return CContApp_HandleDeleteDlgEvent(pMe,eCode,wParam,dwParam);

        case IDD_SELECT:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_SELECT",0,0,0);
            return CContApp_HandleSelectDlgEvent(pMe,eCode,wParam,dwParam);

        case IDD_DETAIL:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_DETAIL",0,0,0);
            return CContApp_HandleDetailDlgEvent(pMe,eCode,wParam,dwParam);
        case IDD_COPYING:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_COPYING",0,0,0);
            return CContApp_HandleCopyingDlgEvent(pMe,eCode,wParam,dwParam);

        case IDD_GROUPOPT:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_GROUPOPT",0,0,0);
            return CContApp_HandleGroupOptDlgEvent(pMe,eCode,wParam,dwParam);

        case IDD_GROUPOPT_EDIT:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_GROUPOPT_EDIT",0,0,0);
            return CContApp_HandleGroupOptEditDlgEvent(pMe,eCode,wParam,dwParam);            

        case IDD_DELETING:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_DELETING",0,0,0);
            return CContApp_HandleDeletingDlgEvent(pMe,eCode,wParam,dwParam);

        case IDD_SELECT_RECORD:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_SELECT_RECORD",0,0,0);
            return CContApp_HandleSelectRecordDlgEvent(pMe,eCode,wParam,dwParam);

        case IDD_DETAIL_MULTI:
            MSG_FATAL("CContApp_RouteDialogEvent IDD_DETAIL_MULTI",0,0,0);
            return CContApp_HandleDetailMultiDlgEvent(pMe,eCode,wParam,dwParam);
            
#ifdef FEATURE_OEMOMH             
        case IDD_EMERGENCY_CALL:
            MSG_FATAL("CContApp_HandleEmergencyCallDlgEvent IDD_EMERGENCY_CALL",0,0,0);
            return CContApp_HandleEmergencyCallDlgEvent(pMe,eCode,wParam,dwParam);
#endif
            
        default:
            MSG_FATAL("CContApp_RouteDialogEvent default",0,0,0);
            return FALSE;
    }
}// CContApp_RouteDialogEvent

/////////////////
//Helper function
/////
/*=============================================================================
FUNCTION: CContApp_HandleDialogTimer

DESCRIPTION: Generic timer used by several dialogs.  When this callback
             is invoked, it sends a EVT_USER_DIALOG_TIMEOUT event
             to the Idle applet (which will be routed to the active dialog)

PARAMETERS:
   *pUser: CContApp object pointer

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static void CContApp_HandleDialogTimer(void *pUser)
{
    CContApp *pMe = (CContApp *)pUser;
    
    ASSERT(pMe->m_pActiveDlg != NULL);
    (void) ISHELL_PostEventEx(pMe->m_pShell,
                              EVTFLG_ASYNC,
                              AEECLSID_APP_CONTACT,
                              EVT_DISPLAYDIALOGTIMEOUT, 
                              0, 
                              0);
}// CContApp_HandleDialogTimer

/*=============================================================================

FUNCTION:CContApp_LoadPhoneBook

DESCRIPTION:
    载入电话本记录
    
PARAMETERS:
    pMe       :
    nViewType : Load the record from where
    
RETURN VALUE:
    SUCCESS 
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int CContApp_LoadPhoneBook(CContApp *pMe, byte nViewType)
{
    int  nRet = SUCCESS;
    
    ASSERT(pMe != NULL);
    MSG_FATAL("Load Phone book viewType: %d", nViewType,0,0);
    // Load phone record according the viewtype
    switch(nViewType)
    {
        case CONTCFG_VIEWTYPE_ALL:
        case CONTCFG_VIEWTYPE_PHONE:
            MSG_FATAL("Load Phone book CONTCFG_VIEWTYPE_PHONE", 0,0,0);
            // Save search data
            pMe->m_wFindCat   = AEE_ADDR_CAT_NONE;
            pMe->m_wFindFldID = AEE_ADDRFIELD_NONE;
            FREEIF(pMe->m_pFindData);
            pMe->m_pFindData  = NULL;
            
            // Search record
            if (CONTCFG_VIEWTYPE_ALL == nViewType)
            {
                nRet = CContApp_LoadByCondition(pMe,0);
            }
            else
            {
                nRet = CContApp_LoadSingleStoreCont(pMe,
                                                    pMe->m_wFindCat,
                                                    pMe->m_wFindFldID,
                                                    pMe->m_pFindData,
                                                    ADDR_STORE_DBFILE);
            }
            break;
            
#ifdef FEATURE_RUIM_PHONEBOOK
        case CONTCFG_VIEWTYPE_RUIM:
            MSG_FATAL("Load Phone book CONTCFG_VIEWTYPE_RUIM", 0,0,0);
            if (IsRunAsUIMVersion() && pMe->m_bADNRUIMSupport)
            {
                // Save search data
                pMe->m_wFindCat   = AEE_ADDR_CAT_NONE;
                pMe->m_wFindFldID = AEE_ADDRFIELD_NONE;
                FREEIF(pMe->m_pFindData);
                pMe->m_pFindData  = NULL;
                
                nRet = CContApp_LoadSingleStoreCont(pMe,
                                                    pMe->m_wFindCat,
                                                    pMe->m_wFindFldID,
                                                    pMe->m_pFindData,
                                                    ADDR_STORE_RUIM);
            }
            break;
#endif// FEATURE_RUIM_PHONEBOOK

        default:
            break;
    }

    return nRet;
}// CContApp_LoadPhoneBook

/*==============================================================================
函数:
    CContApp_LoadByCondition

说明:
    函数根据设定的条件载入符合条件的全部电话本记录 Cache 。

参数:
    pMe [in]: 指向 CContApp Applet对象结构的指针。该结构包含小程序的特定信息。
    wDataSize[in]: 条件数据大小。
         
返回值:
    SUCCESS ：成功
    其它值失败

备注:
    条件 pMe->m_wFindCat, pMe->m_wFindFldID, pMe->m_pFindData 请在调用本函数前
    设置好, pMe->m_pFindData 的大小以参数 wDataSize 传入。
    本函数搜索范围为全部电话本记录(含卡),结果存于队列 pMe->m_pAddList

==============================================================================*/
int CContApp_LoadByCondition(CContApp *pMe, uint16 wDataSize)
{
    int                 i, j, k, nRet = SUCCESS;
    AEEAddCacheInfo     *pCache = NULL;
    AEEAddCacheInfo     *pCache1 = NULL;
    AEEAddCacheInfo     *pCache2 = NULL;
    uint32              nCapacity, nCount;
    IAddrBook           *pAddrBook[2] = {NULL,NULL};
    boolean             bNoInsert = TRUE;
    
    ASSERT(pMe != NULL);
    
    IVector_RemoveAllElements(pMe->m_pAddList);
    nCapacity = (uint32)IADDRBOOK_GetNumRecs(pMe->m_pAddrPhone);
    pAddrBook[0] = pMe->m_pAddrPhone;
#ifdef FEATURE_RUIM_PHONEBOOK
    if (IsRunAsUIMVersion() && pMe->m_bADNRUIMSupport)
    {
        nCapacity += (uint32)IADDRBOOK_GetNumRecs(pMe->m_pAddrRUIM);
        pAddrBook[1] = pMe->m_pAddrRUIM;
    }
#endif// FEATURE_RUIM_PHONEBOOK
    if (SUCCESS != IVector_EnsureCapacity(pMe->m_pAddList, nCapacity))
    {
        FARF(ADDR, ("Failed to IVector_EnsureCapacity"));
        return EFAILED;
    }
    
    // 变量 k 用来标记插入操作发生后，下次插入排序将从此位置开始，这样可省掉
    // 大量的比较操作而提高效率。(能如此做的理由是全部 Cache 是已排序的)
    k = 0;
    // 从电话本Cache载入记录
    for (i=0; (i<2 && NULL!=pAddrBook[i]); i++)
    {
        nRet = IADDRBOOK_EnumCacheInit(pAddrBook[i],
                                       pMe->m_wFindCat,
                                       pMe->m_wFindFldID,
                                       (void*)(pMe->m_pFindData),
                                       wDataSize);
        if (SUCCESS != nRet)
        {
            FARF(ADDR, ("Failed to Enum init %d", nRet));
            return nRet;
        }
        if (SUCCESS != IADDRBOOK_EnumNextCache(pAddrBook[i], (void **)&pCache))
        {
            continue;
        }
    
        while (NULL != pCache)
        {
            if (bNoInsert)
            {
                (void)IVector_AddElement(pMe->m_pAddList, pCache);
            }
            else
            {
                // 按姓名升序排序插入队列
                nCount = IVector_Size(pMe->m_pAddList);
                
                for (j=k; j<nCount; j++)
                {
                    pCache1 = (AEEAddCacheInfo *)IVector_ElementAt(pMe->m_pAddList, j);
                    
                    if (NULL == pCache1)
                    {
                        (void)IVector_AddElement(pMe->m_pAddList, pCache);
                        break;
                    }
                    else
                    {
                         if (Appscommon_CompareName(pCache->szName,pCache1->szName ) <= 0)
                        {
                            (void)IVector_InsertElementAt(pMe->m_pAddList, pCache, j);
                            k = j+1;
                            break;
                        }
                        else
                        {
                            if ((j+1)<nCount)
                            {
                                pCache2 = (AEEAddCacheInfo *)IVector_ElementAt(pMe->m_pAddList, (j+1));
                            }
                            else
                            {
                                pCache2 = NULL;
                            }
                            
                            if (NULL == pCache2)
                            {
                                (void)IVector_AddElement(pMe->m_pAddList, pCache);
                                break;
                            }
                            
                            // 否则进入下轮比较
                        }
                    }
                }
            }
            
            pCache = NULL;
            (void)IADDRBOOK_EnumNextCache(pAddrBook[i], (void **)&pCache);
        }
        
        if (IVector_Size(pMe->m_pAddList)>0)
        {   // 注意：因 Cache 已排好序，故对第一个接口的 Cache 无需插入排序操作
            //       。若前面的记录为空，后面的接口也用不着排序。
            bNoInsert = FALSE;
        }
    } // for

    return SUCCESS;
}// CContApp_LoadByCondition

/*=============================================================================

FUNCTION:CContApp_LoadByName

DESCRIPTION:
    按姓名查找记录
    
PARAMETERS:
    pMe   :
    pName : search name data
    
RETURN VALUE:
    SUCCESS 
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int CContApp_LoadByName(CContApp *pMe, AECHAR *pName)
{
    if (NULL==pMe || NULL==pName)
    {
        return EFAILED;
    }
    
    FARF(ADDR, ("Load Phone book by name "));
    
    // Save search data
    pMe->m_wFindCat   = AEE_ADDR_CAT_NONE;
    pMe->m_wFindFldID = AEE_ADDRFIELD_NAME;
    FREEIF(pMe->m_pFindData);
    pMe->m_pFindData  = WSTRDUP(pName);
        
    return CContApp_LoadByCondition(pMe, WSTRSIZE(pMe->m_pFindData));
}// CContApp_LoadByName

/*=============================================================================

FUNCTION:CContApp_LoadByNum

DESCRIPTION:
    按号码查找记录
    
PARAMETERS:
    pMe  :
    pNum :
        
RETURN VALUE:
    SUCCESS 
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int CContApp_LoadByNum(CContApp *pMe, AECHAR *pNum)
{
    ASSERT(pMe != NULL);
    
    if (NULL==pMe || NULL==pNum)
    {
        return EFAILED;
    }
    
    FARF(ADDR, ("Load Phone book by number "));
    
    // Save search data
    pMe->m_wFindCat   = AEE_ADDR_CAT_NONE;
    pMe->m_wFindFldID = AEE_ADDRFIELD_PHONE_GENERIC;
    FREEIF(pMe->m_pFindData);
    pMe->m_pFindData  = WSTRDUP(pNum);
    
    return CContApp_LoadByCondition(pMe, WSTRSIZE(pMe->m_pFindData));
}// CContApp_LoadByNum

/*=============================================================================

FUNCTION:CContApp_LoadByCat

DESCRIPTION:
    按类别查找记录
    
PARAMETERS:
    pMe   :
    pName : search name data
    
RETURN VALUE:
    SUCCESS 
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int CContApp_LoadByCat(CContApp *pMe, AEEAddrCat wCategory)
{
    ASSERT(pMe != NULL);
    
    FARF(ADDR, ("Load Phone book by cat %x", wCategory));

#ifdef FEATURE_RUIM_PHONEBOOK
    if( IsRunAsUIMVersion() && pMe->m_bADNRUIMSupport )
    {
        if(AEE_ADDR_CAT_UIM == wCategory)
        {
            return CContApp_LoadPhoneBook(pMe, CONTCFG_VIEWTYPE_RUIM); 
        }
    }
#endif

    if (wCategory != AEE_ADDR_CAT_NONE)
    {
        // Save search data
        pMe->m_wFindCat   = wCategory;
        pMe->m_wFindFldID = AEE_ADDRFIELD_NONE;
        FREEIF(pMe->m_pFindData);
        pMe->m_pFindData  = NULL;
        
        // Load phone record
        return CContApp_LoadSingleStoreCont(pMe,
                                            pMe->m_wFindCat,
                                            pMe->m_wFindFldID,
                                            pMe->m_pFindData,
                                            ADDR_STORE_DBFILE);
    }
#ifdef FEATURE_RUIM_PHONEBOOK
    else
    {
        if( IsRunAsUIMVersion() && pMe->m_bADNRUIMSupport )
        {
            // Save search data
            pMe->m_wFindCat   = AEE_ADDR_CAT_NONE;
            pMe->m_wFindFldID = AEE_ADDRFIELD_NONE;
            FREEIF(pMe->m_pFindData);
            pMe->m_pFindData  = NULL;
            return CContApp_LoadPhoneBook(pMe, pMe->m_nViewType); 
        }
        else 
        {
            // Save search data
            pMe->m_wFindCat   = wCategory;
            pMe->m_wFindFldID = AEE_ADDRFIELD_NONE;
            FREEIF(pMe->m_pFindData);
            pMe->m_pFindData  = NULL;
            
            // Load phone record
            return CContApp_LoadSingleStoreCont(pMe,
                                                pMe->m_wFindCat,
                                                pMe->m_wFindFldID,
                                                pMe->m_pFindData,
                                                ADDR_STORE_DBFILE);

        }
    }
#endif// FEATURE_RUIM_PHONEBOOK

    return EFAILED;
}// CContApp_LoadByCat

/*=============================================================================

FUNCTION:CContApp_LoadbySmart

DESCRIPTION:
    载入智能查找的记录到电话本IVector接口
    
PARAMETERS:
    pMe   :
    pStr  : search data Alpha string '*' is wildcard
    
RETURN VALUE:
    SUCCESS 
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int CContApp_LoadbySmart(CContApp *pMe, AECHAR *pStr)
{
    int             i;
    AEEAddCacheInfo *pCache;
    int32           nItems;
    AECHAR          alpha[MAX_INPUT_NAME_EN +1];
    
    ASSERT(pMe != NULL);
    
    if (!pStr)
    {
        return EBADPARM;
    }
    
    FARF(ADDR, ("Load Phone book by smart."));
    
    // First load all the phonebook
    if (SUCCESS != CContApp_LoadPhoneBook(pMe, CONTCFG_VIEWTYPE_ALL))
    {
        FARF(ADDR, ("Failed to Load Phone book."));
        return EFAILED;
    }
    
    nItems = IVector_Size(pMe->m_pAddList);
    
    // Search from phone vector
    for (i=nItems-1; i>=0; i--)
    {
        pCache = (AEEAddCacheInfo *)IVector_ElementAt(pMe->m_pAddList, (uint32)i);
        if (NULL != pCache)
        {
            WStr2Alpha(alpha, MAX_INPUT_NAME_EN, pCache->szName);
            
            // Compare the string
            if (0 != WSTRCMP(alpha, pStr))
            {
                // Remove the record
                (void)IVector_RemoveElementAt(pMe->m_pAddList, (uint32)i);
            }
        }
    }
    
    return SUCCESS;
}// CContApp_LoadbySmart


/*=============================================================================

FUNCTION:CContApp_LoadAddrFlds

DESCRIPTION:
    将记录的指定字段内容载入到 m_pFldIv 中
    
PARAMETERS:
    pMe     :
    wRecID  : The ID suit of the OEM
    eFldType: 载入的字段类型，对姓名无效 SELECT_ALL 载入全部
    
RETURN VALUE:
    SUCCESS:
    
COMMENTS:
    
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int CContApp_LoadAddrFlds( CContApp      *pMe,
                           uint16         wContID,
                           SelectFldType  eFldType)
{
    AEEAddrField  *pTemp;
    AEEAddrField  *pfld = NULL;
    int            ret = SUCCESS;
    int            i,wTotalFields;
    
    ASSERT(pMe != NULL);
    
    FARF(ADDR, ("Load Addr fields %d", wContID));
    
    // 释放链表
    CContApp_FreeAddrFlds(pMe);
    
    if ( SUCCESS != CContApp_FindAddrRec(pMe, wContID) )
    {
        FARF(ADDR, ("Failed to find the pMe->m_pCurrRec"));
        return EFAILED;
    }
    
    //First get the field count and itereate through the
    //fields to get the data
    wTotalFields = IADDRREC_GetFieldCount(pMe->m_pCurrRec);

    for (i = 0; i < wTotalFields; i++)
    {
        pTemp = IADDRREC_GetField( pMe->m_pCurrRec, i );
        if ( pTemp == NULL )
        {
            FARF(ADDR, ("Failed to get the field"));
            ret = EFAILED;
            break;
        }
        
        // check field id
        if(pTemp->fID != AEE_ADDRFIELD_NAME && eFldType != SELECT_ALL)
        {
            switch(pTemp->fID)
            {
                case AEE_ADDRFIELD_PHONE_PREF:
                case AEE_ADDRFIELD_PHONE_HOME:
                case AEE_ADDRFIELD_PHONE_WORK:
                case AEE_ADDRFIELD_PHONE_FAX:
                case AEE_ADDRFIELD_PHONE_CELL:
                case AEE_ADDRFIELD_PHONE_GENERIC:
                    if((eFldType & SINGLE_SELECT_NUMBER)!= SINGLE_SELECT_NUMBER)//lint !e655
                    {
                        continue;
                    }
                    break;

                default:
                    continue;
            }
        }
        
        // Duplicate the field
        pfld = MALLOC( sizeof(*pfld) );
        if (NULL == pfld)
        {
            FARF(ADDR, ("Failed to Malloc the mem"));
            ret = EFAILED;
            break;
        }
        MEMCPY( pfld, pTemp, sizeof(AEEAddrField) );
        
        if (pTemp->pBuffer != NULL)
        {
            pfld->pBuffer = MALLOC(pTemp->wDataLen);
            if (NULL == pfld->pBuffer)
            {
                FARF(ADDR, ("Failed to malloc the pBuffer"));
                ret = EFAILED;
                break;
            }
            
            MEMCPY( pfld->pBuffer,
                    pTemp->pBuffer,
                    pTemp->wDataLen);
        }
        
        if (IVector_AddElement(pMe->m_pFldIv, pfld) != SUCCESS)
        {
            FARF(ADDR, ("Failed to Add the element"));
            ret = EFAILED;
            break;
        }

        pfld = NULL;
    }

    if (pfld != NULL)
    {
        if (pfld->pBuffer != NULL)
        {
            FREEIF(pfld->pBuffer);
        }
        FREE(pfld);
        pfld = NULL;
    }
    
    if(SUCCESS == ret)
    {
        // sort by field type
        IVector_Sort(pMe->m_pFldIv, CContApp_SortByFldIDCB);
    }
    
    return ret;
}// CContApp_LoadAddrFlds

/*=============================================================================

FUNCTION:CContApp_FreeAddrFlds

DESCRIPTION: 
    释放存储当前记录字段信息的IVector接口 m_pFldIv 的空间

PARAMETERS:
   pMe:

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
void CContApp_FreeAddrFlds(CContApp *pMe)
{
    uint32        i;
    uint32        currNum;
    AEEAddrField *pFld;
    
    ASSERT(pMe != NULL);
    
    currNum = IVector_Size(pMe->m_pFldIv);
    
    // free the data first
    for (i = 0; i < currNum; i++)
    {
        pFld = (AEEAddrField*)IVector_ElementAt(pMe->m_pFldIv, i);
        
        if ( pFld == NULL ) 
        {
            continue;
        }
        
        FREE_AEEAddrField(*pFld);
        FREE(pFld);
        pFld = NULL;
    }
    
    IVector_RemoveAllElements(pMe->m_pFldIv);
}// CContApp_FreeAddrFlds


/*=============================================================================

FUNCTION:CContApp_GetFldID

DESCRIPTION:
    获取制定字段的 Field ID 类型
    
PARAMETERS:
    pMe     :
    idx     : 选择字段的 index
        
RETURN VALUE:
    SUCCESS: 
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
AEEAddrFieldID CContApp_GetFldID(CContApp *pMe, int idx)
{
    AEEAddrField *pFld;
    
    ASSERT(pMe != NULL); 
    
    pFld = IVector_ElementAt( pMe->m_pFldIv, (uint32)idx );
    if(pFld == NULL)
    {
        return AEE_ADDRFIELD_NONE;
    }
    
    FARF(ADDR, ("Get %d Field ID %d", pFld->fID, idx));
    
    return pFld->fID;
}// CContApp_GetFldID

/*=============================================================================

FUNCTION:CContApp_GetFldBuf

DESCRIPTION:
    获取制定字段的 Field 数据Buffer
    
PARAMETERS:
    pMe     :
    idx     : 选择字段的 index
    
RETURN VALUE:
    SUCCESS: 
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
void *CContApp_GetFldBuf(CContApp *pMe, int idx)
{
    AEEAddrField *pFld;
    
    ASSERT(pMe != NULL); 
    
    FARF(ADDR, ("Get Field ID %d", idx));
    
    pFld = IVector_ElementAt( pMe->m_pFldIv, (uint32)idx );
    if(pFld == NULL)
    {
        return NULL;
    }
    
    return pFld->pBuffer;
}// CContApp_GetFldBuf

/*=============================================================================

FUNCTION:CContApp_GetBufbyFldID

DESCRIPTION:
    获取指定字段的 Field 数据Buffer
    
PARAMETERS:
    pMe     :
    wFldID  : 选择字段的field ID
    
RETURN VALUE:
    SUCCESS: 
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
void *CContApp_GetBufbyFldID(CContApp *pMe, AEEAddrFieldID wFldID)
{
    uint32        i;
    uint32        currNum;
    AEEAddrField *pFld;
    
    ASSERT(pMe != NULL);
    
    FARF(ADDR, ("Get Field buffer by field ID %d  %d", wFldID, IVector_Size(pMe->m_pFldIv)));
    
    currNum = IVector_Size(pMe->m_pFldIv);
    
    // free the data first
    for (i = 0; i < currNum; i++)
    {
        pFld = (AEEAddrField*)IVector_ElementAt(pMe->m_pFldIv, (uint32)i);

        if(pFld->fID == AEE_ADDRFIELD_RINGTONE)
        {
            //ringID* ringer = (ringID*)pFld->pBuffer;
        }
        if ( pFld == NULL ) 
        {
            continue;
        }
        
        if(pFld->fID == wFldID)
        {
            return pFld->pBuffer;
        }
    }
    
    return NULL;
}// CContApp_GetBufbyFldID

/*=============================================================================

FUNCTION:CContApp_GetFldMaxNum

DESCRIPTION:
    获得对应 Field 的最大尺寸
    
PARAMETERS:
    nFldID  : 字段类型
    
RETURN VALUE:
    The max number of the fields
    
COMMENTS:
    
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint16 CContApp_GetFldMaxNum(AEEAddrFieldID wFldID)
{
    switch(wFldID)
    {
        case AEE_ADDRFIELD_PHONE_PREF:
        case AEE_ADDRFIELD_PHONE_HOME:
        case AEE_ADDRFIELD_PHONE_WORK:
        case AEE_ADDRFIELD_PHONE_FAX:
        case AEE_ADDRFIELD_PHONE_CELL:
        case AEE_ADDRFIELD_PHONE_GENERIC:
            return MAX_NUM_NUM;
            
        case AEE_ADDRFIELD_NAME:
            return MAX_NUM_NAME;
            
        case AEE_ADDRFIELD_EMAIL:
            return MAX_NUM_EMAIL;
            
        case AEE_ADDRFIELD_URL:
            return MAX_NUM_URL;
            
        case AEE_ADDRFIELD_NOTES:
            return MAX_NUM_NOTES;
            
        default:
            return MAX_NUM_OTHER;
    }
}// CContApp_GetFldMaxNum

/*=============================================================================

FUNCTION:CContApp_GetCurrFldNum

DESCRIPTION:
    获得对应 Field 的当前数量
    
PARAMETERS:
    pMe     :
    nFldID  : 字段类型
    
RETURN VALUE:
    The number of the fields
    
COMMENTS:
    
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint16 CContApp_GetCurrFldNum(CContApp *pMe, AEEAddrFieldID wFldID)
{
    uint16        nCnt =0;
    int           i;
    uint32        nFldsNum;
    AEEAddrField *pFld;
    
    ASSERT(pMe != NULL); 
    
    nFldsNum = IVector_Size(pMe->m_pFldIv);
      MSG_FATAL("CContApp_GetCurrFldNum---nFldsNum=%d",nFldsNum,0,0);
    if(nFldsNum == 0)
    {
        FARF(ADDR, ("No fields to count"));
        MSG_FATAL("CContApp_GetCurrFldNum---%d",nCnt,0,0);
        return nCnt;
    }
    
    for(i=0; i<(int)nFldsNum; i++)
    {
        pFld = IVector_ElementAt( pMe->m_pFldIv, (uint32)i );
        if(NULL == pFld)
        {
            continue;
        }
        
        if(IS_NUM_FIELD(wFldID))
        {
            if(IS_NUM_FIELD(pFld->fID))
            {
                nCnt++;
            }
        }
        else
        {
            if(pFld->fID == wFldID)
            {
                nCnt++;
            }
        }
    }
    
    FARF(ADDR, ("Get current %d field number %d", wFldID, nCnt));
      MSG_FATAL("CContApp_GetCurrFldNum---wFldID=%d---nCnt=%d",wFldID,nCnt,0);
    return nCnt;
}// CContApp_GetCurrFldNum



/*=============================================================================

FUNCTION: CContApp_AddField

DESCRIPTION:
    增加一个字段
    This function will add a new field to the current contact.
   
PARAMETERS:
    pMe    : pointer to CContApp object
    wContID: contact id of add field
    wFldID : type of field
    pData  : data of field
    
RETURN VALUE:
    SUCCESS  : if successful otherwise EFAILED
    EBADPARM : if parameter is bad
    EEXIST   : if the content have exsit
    ENOMEMORY: if no memory
    EMAXFIELD: if reach the max field
    
COMMENTS:

SIDE EFFECTS:
    pMe->m_pFldIv
    
SEE ALSO:
=============================================================================*/
int CContApp_AddField( CContApp       *pMe,
                       uint16          wContID,
                       AEEAddrFieldID  wFldID,
                       void           *pData)
{
    AEEAddrField  field;
    AEEAddrField *pFld;
    
    ASSERT(pMe != NULL);
    
    if (NULL == pData)
    {
        return EBADPARM;
    }
    
    if (SUCCESS != CContApp_FindAddrRec(pMe, wContID))
    {
        FARF(ADDR, ("Failed to find the record!"));
        return EFAILED;
    }
    
    // Check the same field
    if(CONTAPP_FLDID_NULL != CContApp_CheckSameFld( pMe, wFldID, pData))
    {
        // Assume we have save it
        FARF(ADDR, ("Find the same field in add field"));
        return EEXIST;
    }
    
    // Check the field number
    if(IS_MAX_FLDS(pMe, wFldID))
    {
        FARF(ADDR, ("Reach the Max number of this field"));
        return EMAXFIELD;
    }

    
    if (SUCCESS != CContApp_BuildAddrField(wFldID, pData, &field, FALSE))
    {
        FARF(ADDR, ("Failed to build the field in add field!"));
        return EFAILED;
    }
    
    // Duplicate the field
    pFld = MALLOC(sizeof(*pFld));
    if (NULL == pFld)
    {
        FARF(ADDR, ("Failed to Malloc the mem in add field"));
        return ENOMEMORY;
    }
    
    MEMCPY( pFld, &field, sizeof(AEEAddrField) );
    
    if (field.pBuffer != NULL)
    {
        pFld->pBuffer = MALLOC(field.wDataLen);
        if (NULL == pFld->pBuffer)
        {
            FARF(ADDR, ("Failed to malloc the pBuffer in add field"));
            FREEIF(pFld);
            return ENOMEMORY;
        }
        
        MEMCPY( pFld->pBuffer,
                field.pBuffer,
                field.wDataLen);
    }
    
    if(SUCCESS != IVector_AddElement(pMe->m_pFldIv, pFld))
    {
        FARF(ADDR, ("Failed to Add the element"));
        FREE_AEEAddrField(*pFld);
        FREEIF(pFld);
        return EFAILED;
    }
    
    // sort by field type
    IVector_Sort(pMe->m_pFldIv, CContApp_SortByFldIDCB);
    
    return CContApp_UpdateAddrFields(pMe, wContID);
}// CContApp_AddField


/*=============================================================================

FUNCTION: CContApp_SaveField

DESCRIPTION:
    Saves the field info into the
    IAddrRec at pMe->m_pFldIv[idx]
   
PARAMETERS:
    pMe    : pointer to CContApp object
    idx    : index of field in m_pFldIv
    wContID: Contact ID of saved
    wFldID : type of field
    pData  : data of field. if pData == NULL means update the number type only
    
RETURN VALUE:
    SUCCESS
   
COMMENTS:

SIDE EFFECTS:
    pMe->m_pFldIv
    
SEE ALSO:
=============================================================================*/
int CContApp_SaveField( CContApp       *pMe,
                        uint16          wContID,
                        uint16          idx,
                        AEEAddrFieldID  wFldID,
                        void           *pData)
{
    AEEAddrField  *pcf;
    AEEAddrField   cf;
    uint16         wSameIdx;
    
    ASSERT(pMe != NULL);
    
    if (SUCCESS != CContApp_FindAddrRec(pMe, wContID))
    {
        FARF(ADDR, ("Failed to find the record!"));
        return EFAILED;
    }
    
    // check the same field
    wSameIdx = CContApp_CheckSameFld(pMe, wFldID, pData);
    
    // find the same field in current record
    // Note : pData == NULL means update the number type
    //        so we must update the record
    if(wSameIdx != CONTAPP_FLDID_NULL && pData)
    {
        // same content have saved
        if(idx == wSameIdx)
        {
            FARF(ADDR, ("Find the same field and return directly"));
            return SUCCESS;
        }// else continue and the record must not be RUIM card
        
        // delete the current fields and return TRUE;
        return CContApp_DeletePhoneFld(pMe, wContID, idx);
    }//else continue
    
    pcf = IVector_ElementAt(pMe->m_pFldIv, idx);
    
    if ( NULL == pcf )
    {
        FARF(ADDR, ("Failed to get the field from m_pFldIv!"));
        return EFAILED;
    }
    
    if(pData)
    {
		
        if ( SUCCESS != CContApp_BuildAddrField(wFldID, pData, &cf, TRUE) )
        {
            FARF(ADDR, ("Failed to build the field!"));
            FREE_AEEAddrField(cf);
            return EFAILED;
        }
        
        // 注意：此处故意没有释放cf中分配的内容，因为经过替换pcf后，将会在
        //       CContApp_FreeAddrFlds()中释放
        FREE_AEEAddrField(*pcf);
        *pcf = cf;
    }
    else
    {
        // 更改字段的类型
        if(IS_NUM_FIELD(wFldID))
        {
            pcf->fID = wFldID;
        }
        else
        {
            return EFAILED;
        }
    }
    
    return CContApp_UpdateAddrFields(pMe, wContID);
} // CContApp_SaveField

/*=============================================================================

FUNCTION: CContApp_SetCat

DESCRIPTION:
    Set current select cont category
   
PARAMETERS:
    pMe    : pointer to CContApp object
    wContID: Contact ID of saved
    wCat   : category that be set
    
RETURN VALUE:
    SUCCESS
   
COMMENTS:

SIDE EFFECTS:
    
SEE ALSO:
=============================================================================*/
int CContApp_SetCat(CContApp *pMe, uint16 wContID, AEEAddrCat wCat)
{
    int nRet;
    AEEAddrCat tmp_cat = 0;
    
    ASSERT(pMe != NULL);

#ifdef FEATURE_RUIM_PHONEBOOK
    if(IsRunAsUIMVersion() && pMe->m_bADNRUIMSupport)
    {
        // RUIM card not support this
        if(IS_RUIM_REC(wContID))
        {
            return EFAILED;
        }
    }
#endif// FEATURE_RUIM_PHONEBOOK
/*
    if (SUCCESS != CContApp_FindAddrRec(pMe, wContID))
    {
        FARF(ADDR, ("Failed to find the record in set cat!"));
        return EFAILED;
    }
    */
    tmp_cat = CContApp_GetCat(pMe, wContID);    
    if(tmp_cat == wCat)
    {
        FARF(ADDR, ("find the same cat: %d", tmp_cat));
        return SUCCESS;
    }

    nRet = IADDRREC_SetCategory(pMe->m_pCurrRec, wCat);
    CContApp_Phonebook_Load(pMe);
    /*
    if (nRet == SUCCESS)
    {
        (void)CContApp_LoadSingleStoreCont(pMe, 
                                           pMe->m_wFindCat,
                                           pMe->m_wFindFldID, 
                                           pMe->m_pFindData,
                                           ADDR_STORE_DBFILE);
    }
    */
    return nRet;
} // CContApp_SetCat

/*=============================================================================

FUNCTION: CContApp_GetCat

DESCRIPTION:
    Get current select cont category
   
PARAMETERS:
    pMe    : pointer to CContApp object
    wContID: Contact ID of saved
    
RETURN VALUE:
    SUCCESS
   
COMMENTS:

SIDE EFFECTS:
    
SEE ALSO:
=============================================================================*/
AEEAddrCat CContApp_GetCat(CContApp *pMe, uint16 wContID)
{
    int nRet;
    
    ASSERT(pMe != NULL);

#ifdef FEATURE_RUIM_PHONEBOOK
    if( IsRunAsUIMVersion() && pMe->m_bADNRUIMSupport )
    {
        // RUIM card not support this
        if(IS_RUIM_REC(wContID))
        {
            return EFAILED;
        }
    }
#endif// FEATURE_RUIM_PHONEBOOK

    if (SUCCESS != CContApp_FindAddrRec(pMe, wContID))
    {
        FARF(ADDR, ("Failed to find the record in set cat!"));
        return EFAILED;
    }
    
    nRet = IADDRREC_GetCategory(pMe->m_pCurrRec);
  
    return nRet;
} // CContApp_GetCat

/*=============================================================================

FUNCTION: CContApp_DeletePhoneFld

DESCRIPTION:
    Deletes the field pMe->m_fieldSelId from the current record in phone
    
PARAMETERS:
    pMe    : pointer to CContApp object
    wContID: contact id of delete field
    idx    : delete index in m_pFldIv
    
RETURN VALUE:
    SUCCESS
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
int CContApp_DeletePhoneFld(CContApp *pMe, uint16 wContID, uint16 idx)
{
    AEEAddrField  *pcf;
    AEEAddrFieldID  wFldID;
    int i;
    
    ASSERT(pMe != NULL);
    
    if (SUCCESS != CContApp_FindAddrRec(pMe, wContID))
    {
        FARF(ADDR, ("Failed to find the record in delete field!"));
        return EFAILED;
    }

    if(IS_NUM_FIELD(pMe->m_nFldInputID))
    {
        WSTRCPY(pMe->m_tmpBuf,(AECHAR *)CContApp_GetFldBuf(pMe, idx));
    }
    wFldID = CContApp_GetFldID(pMe, pMe->m_wSelectFld);
    if(IS_NUM_FIELD(wFldID))
    {
        // 检查当前单键拨号中是否存在当前被编辑的记录
        for(i=CONTCFG_ONEDIAL2; i<=CONTCFG_ONEDIAL8; i++)	////CONTCFG_ONEDIAL1
        {
            if(SUCCESS != CContApp_GetConfig( pMe,
                                      (ContAppCFG)i,
                                      pMe->m_pOneDialBuf,
                                      sizeof(pMe->m_pOneDialBuf)))
            {
                MSG_FATAL("Get Config Failed~",0,0,0);
                continue;
            }
            // 存在单键拨号则需要修改单键拨号
            if(WSTRCMP(pMe->m_tmpBuf, pMe->m_pOneDialBuf) == 0)
            {
                MSG_FATAL("pMe->m_pOneDialBuf[0] = ONEDIAL_NULL",0,0,0);
                // Clear the onedial config
                pMe->m_pOneDialBuf[0] = ONEDIAL_NULL;
				
                (void)CContApp_SetConfig( pMe,
                                          (ContAppCFG)i,
                                          pMe->m_pOneDialBuf,
                                          sizeof(pMe->m_pOneDialBuf));
            }
        }
    }
    
    pcf = IVector_RemoveElementAt(pMe->m_pFldIv, idx);
    if (NULL == pcf)
    {
        FARF(ADDR, ("Failed to remove the field!"));
        return EFAILED;
    }
    
    FREE_AEEAddrField(*pcf);
    FREE(pcf);
    pcf = NULL;
    
    return CContApp_UpdateAddrFields(pMe, wContID);
}// CContApp_DeletePhoneFld



/*=============================================================================

FUNCTION: CContApp_DeleteCont

DESCRIPTION:
    Deletes the record according the cont ID
    
PARAMETERS:
    pMe     : pointer to CContApp object
    wContID : the cont id deleted
    
RETURN VALUE:
    SUCCESS
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
int CContApp_DeleteCont(CContApp *pMe, uint16 wContID)
{
    uint16 wRecID;
    int           i;
    ContInfo      pDelContInfo;
    ContInfo      pContInfo;
    AECHAR        szDelTmp[MAX_INPUT_NAME_CN];
    AECHAR        szTmp[MAX_INPUT_NAME_CN];

    ASSERT(pMe != NULL);
    
    if (SUCCESS != CContApp_FindAddrRec(pMe, wContID))
    {
        FARF(ADDR, ("Failed to find the record in delete Cont!"));
        return EFAILED;
    }
    
    MEMSET(&pContInfo, 0, sizeof(ContInfo));
    MEMSET(&pDelContInfo, 0, sizeof(ContInfo));
    szTmp[0] = 0;
    szDelTmp[0] = 0;
    pDelContInfo.pName = szDelTmp;
    pDelContInfo.wNameLen = MAX_INPUT_NAME_CN; 
    pContInfo.pName = szTmp;
    pContInfo.wNameLen = MAX_INPUT_NAME_CN;

    wRecID = CContApp_ContToRawID(pMe, wContID);

    if(wRecID == CONTAPP_RECID_NULL)
    {
        // ID out of range
        FARF(ADDR, ("Out of range CONTAPP_RECID_NULL in delete cont!"));
        return EFAILED;
    }


    CContApp_BuildContInfo(pMe->m_pCurrRec, &pDelContInfo);

    // 检查当前单键拨号中是否存在当前被删除的记录
    for(i=CONTCFG_ONEDIAL2; i<=CONTCFG_ONEDIAL8; i++)	////CONTCFG_ONEDIAL1
    {
        if(SUCCESS != CContApp_GetConfig( pMe,
                                      (ContAppCFG)i,
                                      pMe->m_pOneDialBuf,
                                      sizeof(pMe->m_pOneDialBuf)))
        {
            MSG_FATAL("Get Config Failed~",0,0,0);
            continue;
        }
         MSG_FATAL("Deletecont--------------1",0,0,0);
        if(SUCCESS != CContApp_GetNameByNum(pMe, 
                                 pMe->m_pOneDialBuf, 
                                 &pContInfo))
        {
            MSG_FATAL("Get Name Failed~",0,0,0);
            continue;
        }
         MSG_FATAL("Deletecont--------------2",0,0,0);
      
        // 存在单键拨号则清除单键拨号
        if(WSTRCMP(pDelContInfo.pName, pContInfo.pName) == 0)
        {
            MSG_FATAL("pMe->m_pOneDialBuf[0] = ONEDIAL_NULL",0,0,0);
            // Clear the onedial config
            pMe->m_pOneDialBuf[0] = ONEDIAL_NULL;			
            (void)CContApp_SetConfig( pMe,
                                      (ContAppCFG)i,
                                      pMe->m_pOneDialBuf,
                                      sizeof(pMe->m_pOneDialBuf));
        }
        
    }

    //Remove the record from the database
    if(SUCCESS != IADDRREC_RemoveRec(pMe->m_pCurrRec))
    {
        return EFAILED;
    } 
    // IADDRREC_RemoveRec()将会释放pMe->m_pCurrRec
    // 因此，这里就不应再次释放了
    //IADDRREC_Release(pMe->m_pCurrRec);
    pMe->m_pCurrRec = NULL;

    return SUCCESS;
}// CContApp_DeleteCont

/*=============================================================================
FUNCTION:CContApp_CheckPhoneName

DESCRIPTION: 
    检查当前话机记录是否存在与输入的新姓名重名
    
PARAMETERS:
    pMe:
    
RETURN VALUE:
    boolean: TRUE if exist ,otherwise FALSE.
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
/*static boolean CContApp_CheckPhoneName(CContApp *pMe, AECHAR *pName)
{
    IAddrRec            *pAddrRec;
    uint16      i;
    ASSERT(pMe != NULL);

    if ( pName == NULL ) 
    {
        return FALSE;
    }

    if (SUCCESS != IADDRBOOK_EnumRecInit( pMe->m_pAddrPhone,
                              AEE_ADDR_CAT_NONE,
                              AEE_ADDRFIELD_NAME,
                              pName,
                              (uint16)WSTRSIZE(pName)))
    {
        return FALSE;
    }

    pAddrRec = IADDRBOOK_EnumNextRec(pMe->m_pAddrPhone);

    // find the same name record
    if(pAddrRec)
    {
        i = IADDRREC_GetRecID(pAddrRec);//guoys modified for PR 461EXT1
        IADDRREC_Release(pAddrRec);
        if(i != pMe->m_wSelectCont)
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    }    
    return FALSE;
}// CContApp_CheckPhoneName*/

#ifdef FEATURE_RUIM_PHONEBOOK
/*=============================================================================

FUNCTION:CContApp_CopyField

DESCRIPTION: 
    复制当前选择的字段到另一面（RUIM记录则复制到话机，话机则复制到卡）
    此函数将自动识别记录的存储位置

PARAMETERS:
    pMe    :
    wContID: the Cont ID of current record
    wFldIdx: the index of field that will be copied

RETURN VALUE:
    SUCCESS
    EMAXRECORD : target device is full
    EMAXFIELD  : Reach the max fields number
    EEXIST     : if field have exist
    EBADPARM   : if parameter is bad
    
COMMENTS:
    此函数假设 m_pFldIv 是可用的
    
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
int CContApp_CopyField(CContApp *pMe, uint16 wContID, uint16 wFldIdx)
{
    AEEAddrField  *pNameFld;
    AEEAddrField  *pNumFld;
    int            nRet;
    
    ASSERT(pMe != NULL);
    
    // Get name field
    pNameFld = IVector_ElementAt(pMe->m_pFldIv, IDX_NAME_FLD);
    if(pNameFld == NULL)
    {
        FARF(ADDR, ("Failed to get name when copy field"));
        return EFAILED;
    }
    
    // get number field
    pNumFld  = IVector_ElementAt(pMe->m_pFldIv, wFldIdx);
    if(pNumFld == NULL)
    {
        FARF(ADDR, ("Failed to get number when copy field"));
        return EFAILED;
    }
    
    // RUIM Record then merge to phone
    if(IS_RUIM_REC(wContID))
    {
        nRet = CContApp_Merge2Phone(pMe, pNameFld, pNumFld);
    }
    else
    {
        nRet = CContApp_Copy2RUIM(pMe, pNameFld, pNumFld);
    }
    
    // Refresh the cache data
    if (nRet == SUCCESS)
    {
        switch (pMe->m_nViewType)
        {
            case CONTCFG_VIEWTYPE_ALL:
                (void)CContApp_LoadByCondition(pMe,(pMe->m_pFindData==NULL ? 0 : WSTRSIZE(pMe->m_pFindData)));
                break;
                
            case CONTCFG_VIEWTYPE_RUIM:
                if (!IS_RUIM_REC(wContID))
                {
                    (void)CContApp_LoadSingleStoreCont(pMe, 
                                                       pMe->m_wFindCat,
                                                       pMe->m_wFindFldID, 
                                                       pMe->m_pFindData,
                                                       ADDR_STORE_RUIM);
                }
                break;
                
            case CONTCFG_VIEWTYPE_PHONE:
                if (IS_RUIM_REC(wContID))
                {
                    (void)CContApp_LoadSingleStoreCont(pMe, 
                                                       pMe->m_wFindCat,
                                                       pMe->m_wFindFldID, 
                                                       pMe->m_pFindData,
                                                       ADDR_STORE_DBFILE);
                }
                break;
                
            default:
                break;
        }
    }
    
    return nRet;
}// CContApp_CopyField


/*=============================================================================

FUNCTION:CContApp_CopyAllToRUIM

DESCRIPTION: 
    将当前话机的记录全部复制到 RUIM 卡

PARAMETERS:
    pMe    :
    wStartID: the Cont ID of Start copy
    
RETURN VALUE:
    SUCCESS   : 
    EMAXRECORD: if the target device is full
    ENORECORD : if no record to copy
    
COMMENTS:
    
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
int CContApp_CopyAllToRUIM(CContApp *pMe, uint16 wStartID)
{
    ASSERT(pMe != NULL);
    
    pMe->m_wCopyRecCnt       = 0;
    pMe->m_bForceComplete    = FALSE;
    
    if(IS_MAX_REC(pMe->m_pAddrRUIM))
    {
        return EMAXRECORD;
    }
    
    // Wether or not have the records to copy
    if(IADDRBOOK_GetNumRecs(pMe->m_pAddrPhone) == 0)
    {
        return ENORECORD;
    }
    
    if(wStartID >= 1)
    {
        pMe->m_wCopyStartID = wStartID -1;
    }
    
    if(pMe->m_wCopyStartID >= IADDRBOOK_GetCapacity(pMe->m_pAddrPhone))
    {
        return EFAILED;
    }
    
    pMe->m_wCopyCurrID = pMe->m_wCopyStartID;
    
    pMe->m_wTotalNum = IADDRBOOK_GetCapacity(pMe->m_pAddrPhone);
    
    CALLBACK_Init( &pMe->m_nCopyCB, 
                  (PFNNOTIFY)CContApp_CopyAllToRUIMCB,
                   (void *)pMe);
                   
    pMe->m_bCopyInited = TRUE;
    
    if (!CALLBACK_IsQueued(&pMe->m_nCopyCB))
    {
        CONTAPP_RESUMECB(&pMe->m_nCopyCB);
    }
    return SUCCESS;
}// CContApp_CopyAllToRUIM


/*=============================================================================

FUNCTION:CContApp_CopyAllToPhone

DESCRIPTION: 
    将当前话机的记录全部复制到 RUIM 卡

PARAMETERS:
    pMe    :
    wStartID: the Cont ID of Start copy
    
RETURN VALUE:
    SUCCESS   : 
    ENORECORD : if no record to copy
    
COMMENTS:
    
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
int CContApp_CopyAllToPhone(CContApp *pMe, uint16 wStartID)
{
    ASSERT(pMe != NULL);
    
    pMe->m_wCopyRecCnt       = 0;
    pMe->m_bForceComplete    = FALSE;
    if(IS_MAX_REC(pMe->m_pAddrPhone))
    {
        return EMAXRECORD;
    }
    
    // Wether or not have the records to copy
    if(IADDRBOOK_GetNumRecs(pMe->m_pAddrRUIM) == 0)
    {
        return ENORECORD;
    }
    
    if(wStartID >= 1)
    {
        pMe->m_wCopyStartID = wStartID -1;
    }
    
    if(pMe->m_wCopyStartID >= IADDRBOOK_GetCapacity(pMe->m_pAddrRUIM))
    {
        return EFAILED;
    }
    
    pMe->m_wCopyCurrID = pMe->m_wCopyStartID;
    
    pMe->m_wTotalNum = IADDRBOOK_GetCapacity(pMe->m_pAddrRUIM);
    
    CALLBACK_Init( &pMe->m_nCopyCB, 
                  (PFNNOTIFY)CContApp_CopyAllToPhoneCB,
                   (void *)pMe);
                   
    pMe->m_bCopyInited = TRUE;
    
    if (!CALLBACK_IsQueued(&pMe->m_nCopyCB))
    {
        CONTAPP_RESUMECB(&pMe->m_nCopyCB);
    }
    return SUCCESS;
}// CContApp_CopyAllToPhone
#endif// FEATURE_RUIM_PHONEBOOK

/*=============================================================================

FUNCTION:CContApp_MakeCall

DESCRIPTION:
    对指定的字段发起呼叫
    
PARAMETERS:
    pMe     :
    idx     : 选择字段的 index
        
RETURN VALUE:
    SUCCESS: 
    
COMMENTS:
    This function only used in view dialog and.
    Assume the m_pFldIv is available.
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
int CContApp_MakeCall(CContApp *pMe, int idx)
{
    AEEAddrField *pFld;
    
    ASSERT(pMe != NULL); 
    
    if ( SUCCESS != ISHELL_CreateInstance( pMe->m_pShell,
                                           AEECLSID_DIALER,
                                           (void **)&pMe->m_pCallApp))
    {
        return EFAILED;
    }
    
    pFld = IVector_ElementAt( pMe->m_pFldIv, (uint32)idx );
    if(pFld == NULL)
    {
        return EFAILED;
    }
    
    FARF(ADDR, ("Contact application MakeCall"));
    
    // Is number field ?
    if(!IS_NUM_FIELD(pFld->fID))
    {
        return EFAILED;
    }
    
    // Make a voice call
    ICallApp_CallNumber(pMe->m_pCallApp, (AECHAR *)pFld->pBuffer);
    
    if (pMe->m_pCallApp != NULL) 
    {
        ICallApp_Release(pMe->m_pCallApp);
        pMe->m_pCallApp = NULL;
    }
    
    return SUCCESS;
}// CContApp_MakeCall



/*=============================================================================

FUNCTION: CContApp_SmartMenuHandle

DESCRIPTION:
    滚动添加智能查找菜单
    
PARAMETERS:
    pMe     :
    pMenuCtl: 指向Menu Control的指针
    eCode   : 
    wParam
    
RETURN VALUE:
    boolean : TRUE if handle the event
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
static boolean CContApp_SmartMenuHandle( CContApp *pMe, 
                                         IMenuCtl *pMenuCtl,
                                         ITextCtl *pTextCtl,
                                         AEEEvent  eCode,
                                         uint16    wParam)
{
    uint16 wLastItem, wFirstItem;
    //static uint16 wField = MASK_ADDRFIELD_PHONE_GENERIC;
    uint16 rtn = TYPE_NO_MATCH; 
    uint32 smartmaxpop = MAX_NUM_MENUPOP;
    sSelectFieldInfo  curSelectFieldInfo;  
    
    // Check parameter
    ASSERT(pMe != NULL);
    (void) ISHELL_CancelTimer( pMe->m_pShell, CContApp_SmartMenuSetFocus, (void *)pMe);   

  
    if(pMenuCtl == NULL || pTextCtl == NULL)
    {
        MSG_FATAL("CContApp_SmartMenuHandle-----FALSE",0,0,0);
        return FALSE;
    }
    MSG_FATAL("ITEXTCTL_GetInputMode(pTextCtl,NULL)=%d",ITEXTCTL_GetInputMode(pTextCtl,NULL),0,0);
    if(ITEXTCTL_GetInputMode(pTextCtl,NULL) == AEE_TM_SYMBOLS)
    {
        return ITEXTCTL_HandleEvent( pTextCtl, eCode, wParam, 0);;
    }        
#ifdef FEATURE_LANG_TCHINESE
//因为现在中文版本的快速查找没有中文输入了, 但是台湾版本的ZHU音还存在。
    if(KEYEVENT(eCode))
    {
        uint32 b_InputCHNChar = FALSE;  
        if(ITEXTCTL_GetInfo(pTextCtl, AEE_TMINFO_NOCCINPUT, &b_InputCHNChar) && TRUE == b_InputCHNChar)
        {
            if(wParam == AVK_CLR)
            {
                IMENUCTL_SetActive(pMenuCtl, TRUE);
                ITEXTCTL_SetActive(pTextCtl, TRUE);
                 MSG_FATAL("CContApp_SmartMenuHandle-----FALSE",0,0,0);
                return FALSE;
            }
            else
            {
                return TRUE;
            }
        }
    }
#endif
    MEMSET(&curSelectFieldInfo, 0, sizeof(curSelectFieldInfo));         
    
    switch(eCode)
    {
        case EVT_KEY_PRESS:            
            if((pMe->m_bInsmartnofind == TRUE)&&(wParam != AVK_CLR))
            {
                return TRUE;
            }
			
            //if(SMART_STATE_IDD_LIST == pMe->m_nSmartStateType)
            //{
                //return TRUE;                
            //}
            
            //if(wParam == AVK_SEND && ITEXTCTL_IsActive(pTextCtl) || wParam == AVK_INFO)
            if(wParam == AVK_SEND && ITEXTCTL_IsActive(pTextCtl)) //Add By zzg 2010_09_16
            {
                ITEXTCTL_SetActive(pTextCtl, FALSE);
                IMENUCTL_SetActive(pMenuCtl, TRUE);
            }

		
            //if(SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType &&  wParam== AVK_RIGHT)
			if(SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType &&  wParam== AVK_INFO)//Add By zzg 2010_09_16
            {
                ITEXTCTL_SetActive(pTextCtl, FALSE);
                IMENUCTL_SetActive(pMenuCtl, TRUE);
            }
            // Fix the bug that after delete a record
            // 第一次进来直接按右键勾选
            pMe->m_wSelectSmart = IMENUCTL_GetSel(pMenuCtl);
            pMe->m_wSelectCont = IMENUCTL_GetSel(pMenuCtl);
            
            if(SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType)
            {
                //CContApp_GetRecByID(pMe, pMe->m_wSelectSmart);
            }
                                            
#if defined(FEATURE_LANG_CHINESE)
            if(wParam == AVK_POUND && !IS_ZERO_REC())
            {
                pMe->m_nCurrentInputMode++;
                pMe->m_nCurrentInputMode %= 2;
                ITEXTCTL_SetInputMode( pTextCtl, pMe->m_nInputModeTable[pMe->m_nCurrentInputMode]);
                ITEXTCTL_SetActive(pTextCtl, FALSE);
				
                if(FALSE == CContApp_SetConfig( pMe,
                                          CONTTCFG_QUICKSEARCH_INPUT_MODE,
                                          &(pMe->m_nCurrentInputMode),
                                          sizeof(byte)))
                {
                }
                
                return TRUE;
            }

#endif

            switch (wParam)
            {
                case AVK_0:
                case AVK_1:
                case AVK_2:
                case AVK_3:
                case AVK_4:  
                case AVK_5:
                case AVK_6:
                case AVK_7:
                case AVK_8:
                case AVK_9: 
                case AVK_Q:
                case AVK_W:
                case AVK_E:
                case AVK_R:
                case AVK_A:
                case AVK_S:
                case AVK_D:
                case AVK_F:
                case AVK_Z:
                case AVK_X:
                case AVK_C:
                case AVK_T:
                case AVK_Y:
                case AVK_U:
                case AVK_I:
                case AVK_O:
                case AVK_P:
                case AVK_G:
                case AVK_H:
                case AVK_J:
                case AVK_K:
                case AVK_L:
                case AVK_V:
                case AVK_B:
                case AVK_N:
                case AVK_M:
                case AVK_SPACE:
                case AVK_RWD:                    
                case AVK_POUND:
                case AVK_STAR:				
                if(SMART_STATE_IDD_LIST == pMe->m_nSmartStateType && IS_ZERO_REC())
                {
                    return TRUE;
                }
               // if(!ITEXTCTL_IsActive(pTextCtl))
                {
                    ITEXTCTL_SetActive(pTextCtl, TRUE);
                    IMENUCTL_SetActive(pMenuCtl, FALSE);
                }
                return TRUE;
                
                case AVK_CLR:
                    if(pMe->m_bInSmartFind == TRUE && pMe->m_bInsmartnofind == FALSE) 
                    {
                        if(!ITEXTCTL_IsActive(pTextCtl))
                        {
                            ITEXTCTL_SetActive(pTextCtl, TRUE);
                            IMENUCTL_SetActive(pMenuCtl, FALSE);
                        }
                    }
                    return TRUE;
                case AVK_LEFT:
                {    
                    if(SMART_STATE_IDD_LIST == pMe->m_nSmartStateType && IS_ZERO_REC())
                    {
                        CContApp_DrawNorecord(pMe, pMenuCtl);
                        return TRUE;
                    }
                    
                }              
                //break; 
                return TRUE;

				//Add By zzg 2010_09_16
				case AVK_RIGHT:
                {    
                    if(SMART_STATE_IDD_LIST == pMe->m_nSmartStateType && IS_ZERO_REC())
                    {
                        CContApp_DrawNorecord(pMe, pMenuCtl);
                        return TRUE;
                    }
                    return TRUE;
                }          
				//Add End
				case AVK_INFO:	//Add By zzg 2010_09_16
                 //case AVK_RIGHT:
                 {    
                     //IMENUCTL_SetActive(pMenuCtl, FALSE);
                     if(SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType)
                     {
                         if(SUCCESS != CContApp_LoadAddrFlds( pMe, pMe->m_wSelectSmart, SELECT_ALL))
                         {
                             CLOSE_DIALOG(DLGRET_ERR);
                             return TRUE;
                         }
                         // 防止有多个号码的记录先打勾再弹出numfield                        
                         if(CContApp_GetCurrFldNum(pMe, AEE_ADDRFIELD_PHONE_GENERIC) > 1)
                         {
                             uint32    dwMask = IMENUCTL_GetProperties(pMenuCtl);
                             IMENUCTL_SetProperties(pMenuCtl, dwMask |MP_NO_REDRAW);
                         }
                          MSG_FATAL("pMe->m_wSelectSmart==%d",pMe->m_wSelectSmart,0,0);
                         /*
                         if(pMe->m_nleftNum <= pMe->m_eTotalSelectNum)
                         {
                             curSelectFieldInfo.wIndex = pMe->m_wSelectSmart;
                             rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                             if (rtn == TYPE_NO_MATCH)
                             {
                                // 可以去掉勾选
                                 CLOSE_DIALOG(DLGRET_SENDLISTFULL);
                             }
                             return TRUE;
                         }
                         */
                     }
                     
                 }              
                 return TRUE;


				 /*
                 case AVK_INFO:
                    if(pMe->m_bInsmartnofind == TRUE)
                    {
                        return TRUE;
                    }

                    switch(pMe->m_nSmartStateType)
                    {
                        case SMART_STATE_IDD_LIST:
                            return TRUE;

                        case SMART_STATE_IDD_SELECT:                            
                            {
                                if((pSelectFieldListRoot == pCurSelectFieldNode)&&(pCurSelectFieldNode == NULL))
                                {
                                    if(pMe->m_wSelFldType == MULTI_SELECT_CONTENT)// prompt no selection when inser content into wms
                                    {
                                        CLOSE_DIALOG(DLGRET_NO_SELECTION);
                                        return TRUE;
                                    }
                                    //curSelectFieldInfo.wIndex = pMe->m_wSelectCont;
                                    curSelectFieldInfo.wIndex = IMENUCTL_GetSel(pMenuCtl);
                                    curSelectFieldInfo.wFieldMask = CContApp_GetPrimaryNumFld(pMe);
                                    CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                                }
                            }
                            CLOSE_DIALOG(DLGRET_OK);
                            break;
                        case SMART_STATE_IDD_DELETE:    
                        case SMART_STATE_IDD_COPY:
                        case SMART_STATE_IDD_SELECT_RECORD:
                            if((pSelectFieldListRoot == pCurSelectFieldNode)&&(pCurSelectFieldNode == NULL))
                            {
                                //pMe->m_eTotalSelectNum++;
                                //curSelectFieldInfo.wIndex = IMENUCTL_GetSel(pMenuCtl);
                                //CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                                CLOSE_DIALOG(DLGRET_NO_SELECTION);
                                return TRUE;
                            }
                            pMe->m_wMainListSel = MENU_SELECT_NULL;
                            pMe->m_wMainListIdx = 0;

                            if(SMART_STATE_IDD_SELECT_RECORD == pMe->m_nSmartStateType)
                            {
                                char buf[20] = {0};
                                char* str = NULL;
                                
                                SPRINTF(buf, "%c%d", 'A', (uint32)pSelectFieldListRoot);
                            
                                str = STRDUP(buf);
                                ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_BTUIAPP, str);
                                FREEIF(str);
                                return TRUE;
                            }

                            CLOSE_DIALOG(DLGRET_OK);
                            break;
                            
                        default:
                            return TRUE;
                    }                     
                    return TRUE;
                    */
                            
                default:
                    return TRUE;                    
            }   

            // App handle all key event 
            // Build the list Menu
            if(!pMe->m_bInSmartFind  || pMe->m_pSmartBufLen == 0)
            {       
                (void)CContApp_BuildSmartStateReturnMenu(pMe, pMenuCtl); 
            }
            else
            {
                (void)CContApp_BuildSmartMenu(pMe, pMenuCtl);
            }
            IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectSmart);
            (void)IMENUCTL_Redraw(pMenuCtl);
            return TRUE;
            
        case EVT_KEY:
            
            if((pMe->m_bInsmartnofind == TRUE)&&(wParam != AVK_CLR))
            {
                return TRUE;
            }
            
            switch(wParam)
            {
                case AVK_UP:
                    if(SMART_STATE_IDD_LIST == pMe->m_nSmartStateType && IS_ZERO_REC())
                    {
                        return TRUE;
                    }
                    
                    if(!pMe->m_bInSmartFind )
                        break;

                    if(pMe->m_bInsmartnofind)
                    {
                        pMe->m_bInSmartFind = FALSE;    
                        ITEXTCTL_SetText(pTextCtl, pMe->m_lastAlpha, 0);
                        IMENUCTL_SetActive(pMenuCtl, TRUE);
                        CONTAPP_DRAW_BOTTOMBAR( (SMART_STATE_IDD_LIST == pMe->m_nSmartStateType) ? 
                                                                                    BTBAR_SELECT_BACK : BTBAR_SELECT_BACK);
                        break;
                    }
                    
                    if( pMe->m_pSmartBufLen == 0)
                    {
                        break;
                    }
                    
                    wFirstItem = IMENUCTL_GetItemID(pMenuCtl, 0);
                    // Fix the bug that after delete a record
                    pMe->m_wSelectSmart = IMENUCTL_GetSel(pMenuCtl);
                    MSG_FATAL("wFirstItem==%d,pMe->m_wSelectSmart==%d,m_pSmartBufLen=%d",wFirstItem,pMe->m_wSelectSmart,pMe->m_pSmartBufLen);
                    if(wFirstItem == pMe->m_wSelectSmart)//在最上面的一个选项上
                    {
                        if (pMe->m_pSmartBufLen <= smartmaxpop)
                        {
                             
                            IMENUCTL_SetSel(pMenuCtl, 
                                                IMENUCTL_GetItemID( pMenuCtl, pMe->m_pSmartBufLen -1));
                             MSG_FATAL("pMe->m_nCurrSmartIdx   = %d",pMe->m_nCurrSmartIdx,0,0);
                        }
                        else 
                        {             
                            if(pMe->m_nCurrSmartIdx == 0)
                            {
                                pMe->m_nCurrSmartIdx = pMe->m_pSmartBufLen - MAX_NUM_MENUPOP;
                                (void)CContApp_BuildSmartMenu(pMe, pMenuCtl);
                                IMENUCTL_SetSel(pMenuCtl, IMENUCTL_GetItemID( pMenuCtl, MAX_NUM_MENUPOP - 1));
                                
                                MSG_FATAL("pMe->m_nCurrSmartIdx   = %d",pMe->m_nCurrSmartIdx,0,0);
                            }
                            else
                            {
                                pMe->m_nCurrSmartIdx--;
                                (void)CContApp_BuildSmartMenu(pMe, pMenuCtl);
                                IMENUCTL_SetSel(pMenuCtl, IMENUCTL_GetItemID( pMenuCtl, 0));
                                MSG_FATAL("pMe->m_nCurrSmartIdx   = %d",pMe->m_nCurrSmartIdx,0,0);
                            }
  
                        }
                        (void)IMENUCTL_Redraw(pMenuCtl);
                    }
                    
                    return TRUE;
                    
                case AVK_DOWN:   
                    if(!pMe->m_bInSmartFind)
                        break;

                    if(pMe->m_bInsmartnofind)
                    {
                        pMe->m_bInSmartFind = FALSE;
                        ITEXTCTL_SetText(pTextCtl, pMe->m_lastAlpha, 0);
                        IMENUCTL_SetActive(pMenuCtl, TRUE);
                        CONTAPP_DRAW_BOTTOMBAR( (SMART_STATE_IDD_LIST == pMe->m_nSmartStateType) ? 
                                                                                    BTBAR_SELECT_BACK : BTBAR_SELECT_BACK);
                        break;
                    }

                    if( pMe->m_pSmartBufLen == 0)
                    {
                        break;
                    }
                    
                    wLastItem  = IMENUCTL_GetItemID( pMenuCtl, 
                                                                CONSTRAIN(pMe->m_pSmartBufLen, 1, smartmaxpop) -1);
                    pMe->m_wSelectSmart = IMENUCTL_GetSel(pMenuCtl);
                    
                    if(wLastItem == pMe->m_wSelectSmart)//在最下面的一个选项上
                    {
                        if (pMe->m_pSmartBufLen <= smartmaxpop)
                        {
                            IMENUCTL_SetSel(pMenuCtl, IMENUCTL_GetItemID(pMenuCtl, 0));
                        }
                        else 
                        {
                            if(pMe->m_nCurrSmartIdx + MAX_NUM_MENUPOP == pMe->m_pSmartBufLen )
                            {
                                pMe->m_nCurrSmartIdx = 0;
                                (void)CContApp_BuildSmartMenu(pMe, pMenuCtl);
                                IMENUCTL_SetSel(pMenuCtl, IMENUCTL_GetItemID(pMenuCtl, 0));
                            }
                            else
                            {
                                pMe->m_nCurrSmartIdx++;
                                (void)CContApp_BuildSmartMenu(pMe, pMenuCtl);
                                IMENUCTL_SetSel(pMenuCtl, IMENUCTL_GetItemID(pMenuCtl, MAX_NUM_MENUPOP -1));
                            }
                        }
                        (void)IMENUCTL_Redraw(pMenuCtl);                        
                    }                   
                    return TRUE;
                                    
                case AVK_CLR:
                {
                    FREEIF(pMe->m_szAlpha);   
                    break;
                }
                
               default:
                    break;
            }
            break;
            //return TRUE;
            
        case EVT_KEY_RELEASE:
            
#ifdef FEATURE_LANG_CHINESE
if(wParam == AVK_POUND && !IS_ZERO_REC())
{
    ITEXTCTL_SetActive(pTextCtl, TRUE);
}
#endif                   

            switch(wParam)
            {
                case AVK_0:
                case AVK_1:
                case AVK_2:
                case AVK_3:
                case AVK_4:  
                case AVK_5:
                case AVK_6:
                case AVK_7:
                case AVK_8:
                case AVK_9: 
                case AVK_Q:
                case AVK_W:
                case AVK_E:
                case AVK_R:
                case AVK_A:
                case AVK_S:
                case AVK_D:
                case AVK_F:
                case AVK_Z:
                case AVK_X:
                case AVK_C:
                case AVK_T:
                case AVK_Y:
                case AVK_U:
                case AVK_I:
                case AVK_O:
                case AVK_P:
                case AVK_G:
                case AVK_H:
                case AVK_J:
                case AVK_K:
                case AVK_L:
                case AVK_V:
                case AVK_B:
                case AVK_N:
                case AVK_M:
                case AVK_SPACE:
                case AVK_RWD:
                case AVK_CLR:
                case AVK_UP:
                case AVK_DOWN:
                case AVK_LEFT:
                case AVK_STAR:
                case AVK_POUND:
                case AVK_RIGHT:
                {
                    boolean b_TextctlActive = ITEXTCTL_IsActive(pTextCtl);
                    int           n_KeywordsLen =0;
                    boolean isSearchTextChg = FALSE;
                    // save this alpha,and all param
                    //pMe->m_szAlpha = ITEXTCTL_GetTextPtr(pTextCtl);
                    
                    if(WSTRCMP(pMe->m_szAlpha, ITEXTCTL_GetTextPtr(pTextCtl)) != 0)
                    {
                        isSearchTextChg = TRUE; 
                        FREEIF(pMe->m_szAlpha);
                        pMe->m_szAlpha = WSTRDUP(ITEXTCTL_GetTextPtr(pTextCtl));
                    }
                    
                    if(NULL != pMe->m_szAlpha && WSTRLEN(pMe->m_szAlpha) > 0)
                    {
                        n_KeywordsLen = WSTRLEN(pMe->m_szAlpha);
                    }
                    else
                    {
                        FREEIF(pMe->m_szAlpha);
                    }

                    if((AVK_CLR == wParam ) && (n_KeywordsLen== 0) )
                    {
                        /*if(SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType)
                        {
                            CContApp_DisplaySelectField(pMe, pMe->m_wFieldIndex);    
                        }*/
                        uint32    dwMask = IMENUCTL_GetProperties(pMenuCtl);
                        IMENUCTL_SetProperties(pMenuCtl, dwMask & (~MP_NO_REDRAW));

                        // 防止从IDLE界面的KEY_HELD事件到这里才释放
                        if(IS_ZERO_REC())
                        {
                            return TRUE;
                        }
                        
                        pMe->m_pSmartBufLen = 0;
                        if(SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType)
                        {
                            /*
                            if(IVector_Size(pMe->m_pAddList) > pMe->m_nleftNum)
                            {
                                CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_BACK);
                            }
                            else*/
                            {
                                CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
                            }
                        }
                        else if(SMART_STATE_IDD_COPY == pMe->m_nSmartStateType
                            || SMART_STATE_IDD_DELETE == pMe->m_nSmartStateType
                            || SMART_STATE_IDD_SELECT_RECORD == pMe->m_nSmartStateType)
                        {
                            CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
                        }
                        else
                        {
                            if(STATE_ONEDIAL_SET == pMe->m_eCurState
                                || (pMe->m_wSelFldType == SINGLE_SELECT_NUMBER && STARTMETHOD_SELECTFIELD == pMe->m_eStartMethod)
                                || (pMe->m_wSelFldType == SINGLE_SELECT_GROUPLIST && STARTMETHOD_GROUPLIST == pMe->m_eStartMethod))
                            {
                                CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
                            }
                            else
                            {
                                CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
                            }
                        }
                        
                        if(pMe->m_bInSmartFind == TRUE)
                        {
                            pMe->m_bInSmartFind = FALSE;            
                            pMe->m_bInsmartnofind = FALSE;           
                            (void)CContApp_BuildSmartStateReturnMenu(pMe, pMenuCtl); 
                               
                            ITEXTCTL_SetActive(pTextCtl, FALSE);
                            IMENUCTL_SetActive(pMenuCtl, TRUE);
                            (void)IMENUCTL_Redraw(pMenuCtl);     
                            
                            return TRUE;
                        }
                        else
                        {
                            IMENUCTL_Redraw(pMenuCtl);
                             MSG_FATAL("CContApp_SmartMenuHandle-----FALSE",0,0,0);
                            return FALSE;
                        }
                    }
                    else if(wParam == AVK_UP ||wParam == AVK_DOWN )
                    {
                        if(pMe->m_bInsmartnofind == TRUE)
                        {
                        	#ifndef FEATURE_ALL_KEY_PAD   //add by yangdecai
                            if ( WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) > 0 && ITEXTCTL_GetT9End(pTextCtl) != TC_CURSORSTART )
                            {
                                CONTAPP_DRAW_BOTTOMBAR(BTBAR_DELETE);
                            }
                            else
                            #endif
                            {
                                CONTAPP_DRAW_BOTTOMBAR(BTBAR_BACK);            
                            }   
                            IDISPLAY_Update(pMe->m_pDisplay);  
                        }
                        else
                        {
                            IMENUCTL_SetActive(pMenuCtl, TRUE); // 防止在timer时间到之前安闲up或者down,menu会失去焦点
                            if(SMART_STATE_IDD_LIST == pMe->m_nSmartStateType)
                            {
                                ITEXTCTL_SetActive(pTextCtl, FALSE);
                            }
                        }
                        if(!b_TextctlActive || pMe->m_bInsmartnofind)
                            return FALSE;                            
                    }
                    else if( wParam == AVK_LEFT || wParam == AVK_RIGHT)
                    {
						return TRUE;
					}
                    else if( wParam == AVK_INFO)
                    {
                         //if(b_TextctlActive)
                         //{
                         //   ITEXTCTL_SetCursorPos(pTextCtl, TC_CURSOREND);
                         //   return TRUE;
                         //}
                         //else
                        if(pMe->m_bInsmartnofind == TRUE)
                        {
                        	#ifndef FEATURE_ALL_KEY_PAD   //add by yangdecai
                            if ( WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) > 0 && ITEXTCTL_GetT9End(pTextCtl) != TC_CURSORSTART )
                            {
                                CONTAPP_DRAW_BOTTOMBAR(BTBAR_DELETE);
                            }
                            else
                            #endif
                            {
                                CONTAPP_DRAW_BOTTOMBAR(BTBAR_BACK);            
                            }   
                            //IDISPLAY_Update(pMe->m_pDisplay);  
                        }                        
                        else
                        {
                            //if(ITEXTCTL_IsActive(pTextCtl))
                            {
                                IMENUCTL_SetActive(pMenuCtl, TRUE); // 让menu不失去焦点,选中条不消失
                                ITEXTCTL_SetActive(pTextCtl, TRUE);
                                //IMENUCTL_SetActive(pMenuCtl, TRUE);
                            }
                            
                            if ( WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) > 0 && ITEXTCTL_GetT9End(pTextCtl) != TC_CURSORSTART )
                            {
                                if(SMART_STATE_IDD_LIST == pMe->m_nSmartStateType)
                                {
                                    if(STATE_ONEDIAL_SET == pMe->m_eCurState
                                        || (pMe->m_wSelFldType == SINGLE_SELECT_NUMBER && STARTMETHOD_SELECTFIELD == pMe->m_eStartMethod)
                                        || (pMe->m_wSelFldType == SINGLE_SELECT_GROUPLIST && STARTMETHOD_GROUPLIST == pMe->m_eStartMethod))
                                    {
                                        CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_DEL);
                                    }
                                    else
                                    {
                                        CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_DEL);
                                    }
                                    //CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_DELETE);
                                }
                                else if(SMART_STATE_IDD_COPY == pMe->m_nSmartStateType
                                    || SMART_STATE_IDD_DELETE == pMe->m_nSmartStateType
                                     || SMART_STATE_IDD_SELECT_RECORD == pMe->m_nSmartStateType)
                                {
                                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_DEL);
                                }
                                else
                                {
                                    // SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType
                                    /*
                                    if(IVector_Size(pMe->m_pAddList) > pMe->m_nleftNum)
                                    {
                                        CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_DEL);
                                    }
                                    else*/
                                    {
                                        CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_DEL);
                                    }
                                }
                            }
                            else
                            {
                                if(SMART_STATE_IDD_LIST == pMe->m_nSmartStateType)
                                {
                                    if(STATE_ONEDIAL_SET == pMe->m_eCurState
                                        || (pMe->m_wSelFldType == SINGLE_SELECT_NUMBER && STARTMETHOD_SELECTFIELD == pMe->m_eStartMethod)
                                        || (pMe->m_wSelFldType == SINGLE_SELECT_GROUPLIST && STARTMETHOD_GROUPLIST == pMe->m_eStartMethod))
                                    {
                                        CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
                                    }
                                    else
                                    {
                                        ERR("tangwq before draw botton",0,0,0);
                                        CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
                                    }
                                }
                                else if(SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType)
                                {
                                    /*
                                    if(IVector_Size(pMe->m_pAddList) > pMe->m_nleftNum)
                                    {
                                        CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_BACK);
                                    }
                                    else*/
                                    {
                                        CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
                                    }
                                }
                                else
                                {
                                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
                                }
                                //CONTAPP_DRAW_BOTTOMBAR(BTBAR_BACK);            
                            }   
                        }

                        if(SMART_STATE_IDD_LIST == pMe->m_nSmartStateType && IS_ZERO_REC())
                        {
                            ITEXTCTL_SetActive(pTextCtl, FALSE);
                            CONTAPP_DRAW_BOTTOMBAR(BTBAR_ADD_BACK);
                            CContApp_DrawNorecord(pMe, pMenuCtl);
                        }
                        IDISPLAY_Update(pMe->m_pDisplay);  
                        /*if( wParam == AVK_LEFT || wParam == AVK_RIGHT)
                        {
                            return TRUE;
                        }*/
                    }
                    
                                            
                    // research the record
                    if(n_KeywordsLen > 0 && isSearchTextChg == TRUE)
                    {
                        pMe->m_bInSmartFind = TRUE;
                        if(SUCCESS != CContApp_FilterSmartItems( pMe,  pMe->m_szAlpha) )
                        {   
                            uint32    dwMask = IMENUCTL_GetProperties(pMenuCtl);
                            MSG_FATAL("SUCCESS != CContApp_FilterSmartItems 111",0,0,0);
                            IMENUCTL_SetProperties(pMenuCtl, dwMask |MP_NO_REDRAW);
                            
                            pMe->m_bInsmartnofind = TRUE;
                            IMENUCTL_SetActive(pMenuCtl, FALSE);
                            /*guoys add @2008.10.20 for no match dont use select*/    
                            #ifndef FEATURE_ALL_KEY_PAD
                 			CONTAPP_DRAW_BOTTOMBAR(BTBAR_DELETE);
                 			#else
                 			CONTAPP_DRAW_BOTTOMBAR(BTBAR_BACK);
                 			#endif
                            CContApp_RestoreMenuRect(pMe, pMenuCtl);
                            CContApp_DisplayRightTopStr(pMe,pMenuCtl,wParam);               
                        }
                        else
                        {
                            uint32    dwMask = IMENUCTL_GetProperties(pMenuCtl);
                            IMENUCTL_SetProperties(pMenuCtl, dwMask & (~MP_NO_REDRAW));
                            
                            (void)CContApp_BuildSmartMenu(pMe, pMenuCtl);
                            
                            (void) ISHELL_SetTimer( pMe->m_pShell,
                                                    200,
                                                    CContApp_SmartMenuSetFocus,
                                                    (void *)pMe);     
                            IMENUCTL_Redraw(pMenuCtl);
                            pMe->m_bInsmartnofind = FALSE;

                            if(SMART_STATE_IDD_LIST == pMe->m_nSmartStateType)
                            {
                                if(STATE_ONEDIAL_SET == pMe->m_eCurState
                                    || (pMe->m_wSelFldType == SINGLE_SELECT_NUMBER && STARTMETHOD_SELECTFIELD == pMe->m_eStartMethod)
                                    || (pMe->m_wSelFldType == SINGLE_SELECT_GROUPLIST && STARTMETHOD_GROUPLIST == pMe->m_eStartMethod))
                                {
                                    //CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
                                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_DEL);
                                }
                                else
                                {
                                    //CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
                                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_DEL);
                                }
                            }
                            else if(SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType)
                            {
                                /*
                                if(pMe->m_pSmartBufLen > pMe->m_nleftNum)
                                {
                                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_DEL);
                                }
                                else*/
                                {
                                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_DEL);
                                }
                            }
                            else
                            {
                                CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_DEL);
                            }
                            
                        }
                      //  CContApp_DrawIMEIcon(pTextCtl, pMe->m_pDisplay);
                    }
                }
                return TRUE;

                //case AVK_RIGHT :
                case AVK_INFO:	//Add By zzg 2010_09_16
                    /*
                    if(SMART_STATE_IDD_SELECT != pMe->m_nSmartStateType)
                    {
                        return TRUE;               
                    } */
                        //if(ITEXTCTL_IsActive(pTextCtl))
                         MSG_FATAL("pMe->m_wMainListSel=%d----pMe->m_wMainListIdx=%d---",pMe->m_wMainListSel,pMe->m_wMainListIdx,0);
                         MSG_FATAL("--pMe->m_nSmartStateType=%d",pMe->m_nSmartStateType,0,0);
                        if(SMART_STATE_IDD_LIST == pMe->m_nSmartStateType)
                        {

                            //IMENUCTL_SetActive(pMenuCtl, TRUE);
                        //}
                            if(pMe->m_bInsmartnofind == TRUE)
                            {
                            	#ifndef FEATURE_ALL_KEY_PAD   //add by yangdecai
                                if ( WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) > 0 && ITEXTCTL_GetT9End(pTextCtl) != TC_CURSORSTART )
                                {
                                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_DELETE);
                                }
                                else
                                #endif
                                {
                                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_BACK);            
                                }   
                                IDISPLAY_Update(pMe->m_pDisplay);
                                return TRUE;
                            }
                            else
                            {
                                IMENUCTL_SetActive(pMenuCtl, TRUE); // 让menu不失去焦点
                                ITEXTCTL_SetActive(pTextCtl, TRUE);
                                if ( WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) > 0 && ITEXTCTL_GetT9End(pTextCtl) != TC_CURSORSTART )
                                {
                                    //CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_DELETE);
                                    if(STATE_ONEDIAL_SET == pMe->m_eCurState
                                        || (pMe->m_wSelFldType == SINGLE_SELECT_NUMBER && STARTMETHOD_SELECTFIELD == pMe->m_eStartMethod)
                                        || (pMe->m_wSelFldType == SINGLE_SELECT_GROUPLIST && STARTMETHOD_GROUPLIST == pMe->m_eStartMethod))
                                    {
                                        //CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
                                        CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_DEL);
                                    }
                                    else
                                    {
                                        //CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
                                        CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_DEL);
                                    }
                                }
                                else
                                {
                                    if(STATE_ONEDIAL_SET == pMe->m_eCurState
                                        || (pMe->m_wSelFldType == SINGLE_SELECT_NUMBER && STARTMETHOD_SELECTFIELD == pMe->m_eStartMethod)
                                        || (pMe->m_wSelFldType == SINGLE_SELECT_GROUPLIST && STARTMETHOD_GROUPLIST == pMe->m_eStartMethod))
                                    {
                                        //CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
                                        CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
                                    }
                                    else
                                    {
                                        //CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
                                        CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
                                    }
                                    //CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);            
                                }   
                                IDISPLAY_Update(pMe->m_pDisplay);
                            }
                            // 空间的切换导致menu刷新，把norecord给覆盖掉，在按键释放的时候再画出来，
                            // 这个办法不是很好。
                            if(SMART_STATE_IDD_LIST == pMe->m_nSmartStateType && IS_ZERO_REC())
                            {
                                ITEXTCTL_SetActive(pTextCtl, FALSE);
                                CONTAPP_DRAW_BOTTOMBAR(BTBAR_ADD_BACK);            
                                CContApp_DrawNorecord(pMe, pMenuCtl);
                            }
                            return TRUE;
                        }
                        
                    curSelectFieldInfo.wIndex = pMe->m_wSelectSmart;
                    MSG_FATAL("--pMe->m_wSelectSmart=%d",pMe->m_wSelectSmart,0,0);

                    if(SMART_STATE_IDD_COPY == pMe->m_nSmartStateType 
                        || SMART_STATE_IDD_DELETE == pMe->m_nSmartStateType
                        || SMART_STATE_IDD_SELECT_RECORD == pMe->m_nSmartStateType)
                    {
                        //curSelectFieldInfo.wIndex = pMe->m_wSelectSmart;
                        rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                        if (rtn == TYPE_NO_MATCH)
                        {
                            pMe->m_eTotalSelectNum++;
                            CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                        }
                        else 
                        {
                            pMe->m_eTotalSelectNum--;
                            CContApp_DeleteSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                        }
                        //return TRUE;
                    }
                    else if(SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType)
                    {
                        CContApp_GetRecByID(pMe, pMe->m_wSelectSmart);
                        
                        curSelectFieldInfo.wFieldCount = pMe->m_wFieldCount;
                        
                        //curSelectFieldInfo.wFieldMask = pMe->m_wFieldIndex;
                        if(CContApp_GetCurrFldNum(pMe, AEE_ADDRFIELD_PHONE_GENERIC) > 1)
                        {  
                            MSG_FATAL("pMe->m_wMainListSel=%d----pMe->m_wMainListIdx=%d---",pMe->m_wMainListSel,pMe->m_wMainListIdx,0);
                            // store the menu select
                            PUSH_LISTMENU_SEL(pMe->m_wMainListSel);
                            
                            // store the menu index
                            PUSH_LISTMENU_IDX(pMe->m_wMainListIdx);
                           
                            // insert  content into wms
                            if(pMe->m_eStartMethod == STARTMETHOD_SELECTFIELD && pMe->m_wSelFldType == MULTI_SELECT_CONTENT)
                            {
                                //这里不考虑中国电信版本的只有姓名没有号码的情况，统一跳到多选界面选择
                                CLOSE_DIALOG(DLGRET_SELECT_FLD);
                                return TRUE;
                            }
                            
                            CLOSE_DIALOG(DLGRET_POPNUMFLD);
                            return TRUE;
                        }
                        else
                        {
                             MSG_FATAL("pMe->m_wMainListSel=%d----pMe->m_wMainListIdx=%d---",pMe->m_wMainListSel,pMe->m_wMainListIdx,0);
                            pMe->m_wFieldIndex = CContApp_GetOneNumberFld(pMe, IDX_NUM_RUIM_FLD);
                            curSelectFieldInfo.wFieldMask = pMe->m_wFieldIndex;
                            rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                            if (rtn == TYPE_NO_MATCH)
                            {
                                pMe->m_eTotalSelectNum++;
                                CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                                if (pMe->m_wSelFldType == SINGLE_SELECT_NUMBER)
                                {
                                    //CLOSE_DIALOG(DLGRET_OK);
                                    return TRUE;
                                }
                            }
                            else if (rtn == TYPE_INDEX_MATCH)
                            {
                                pMe->m_eTotalSelectNum--;
                                CContApp_UpdateSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                                if(pCurSelectFieldNode->SelectFieldInfo.wFieldMask == 1)
                                {
                                    CContApp_DeleteSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                                }
                            }
                            else if (rtn == TYPE_FIELD_MATCH)
                            {
                                pMe->m_eTotalSelectNum--;
                                CContApp_UpdateSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                                if(pCurSelectFieldNode->SelectFieldInfo.wFieldMask == 1)
                                {
                                    CContApp_DeleteSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                                }
                            }
                        }
                    }
                    return TRUE;
                case AVK_SELECT:
                {
                    if(pMe->m_bInsmartnofind == TRUE)
                    {
                        return TRUE;
                    }
                    //pMe->m_wSelectSmart = wParam;
                    //pMe->m_wSelectCont = wParam;
                    //发送列表最多支持100条记录
                    if(SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType)
                    {
                        CContApp_GetRecByID(pMe, pMe->m_wSelectSmart);
                        
                        {
                            /*
                            if(IVector_Size(pMe->m_pAddList) > pMe->m_nleftNum && (pMe->m_pSmartBufLen == 0 || pMe->m_pSmartBufLen > pMe->m_nleftNum))
                            {
                                if((pSelectFieldListRoot == pCurSelectFieldNode)&&(pCurSelectFieldNode == NULL))
                                {
                                    curSelectFieldInfo.wIndex = pMe->m_wSelectSmart;
                                    curSelectFieldInfo.wFieldMask = CContApp_GetPrimaryNumFld(pMe);
                                    CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                                }
                            }
                            else*/
                            {
                               // if(SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType)
                               // {
                                IBitmap *tmpBitmap;
                                AEEBitmapInfo info;
                                MEMSET(&info, 0, sizeof(info));
                                
                                if(SUCCESS != IDISPLAY_GetDeviceBitmap(pMe->m_pDisplay, &tmpBitmap))
                                {
                                   FARF(ADDR, ("IDISPLAY_GetDeviceBitmap() failed"));
                                   return FALSE;
                                }
        
                                if(SUCCESS != IBITMAP_GetInfo(tmpBitmap, &info, sizeof(info)))
                                {
                                   FARF(ADDR, ("IBITMAP_GetInfo() failed"));
                                   return FALSE;
                                }
        
                                if(NULL != pMe->m_eBitmap)
                                {
                                    IBITMAP_Release(pMe->m_eBitmap);
                                    pMe->m_eBitmap = NULL;
                                }
                                if(SUCCESS != IBITMAP_CreateCompatibleBitmap(tmpBitmap, &pMe->m_eBitmap, info.cx, info.cy))
                                {
                                    FARF(ADDR, ("IBITMAP_CreateCompatibleBitmap() failed"));
                                    return FALSE;
                                }
                                //save the bitmap
                                IBITMAP_BltIn(pMe->m_eBitmap,
                                            0,
                                            0,
                                            info.cx,
                                            info.cy,
                                            tmpBitmap,
                                            0,
                                            0,
                                            AEE_RO_COPY
                                            );
                                if(NULL != tmpBitmap)
                                {
                                    IBITMAP_Release(tmpBitmap);
                                    tmpBitmap = NULL;
                                }

                                // store the menu select
                                PUSH_LISTMENU_SEL(pMe->m_wMainListSel);
                                
                                // store the menu index
                                PUSH_LISTMENU_IDX(pMe->m_wMainListIdx);
                                
                                CLOSE_DIALOG(DLGRET_SELECT);
                                return TRUE;
                            }
                        }
                        
                        CLOSE_DIALOG(DLGRET_OK);
                        return TRUE;
                    }
                    else if(SMART_STATE_IDD_COPY == pMe->m_nSmartStateType
                        || SMART_STATE_IDD_DELETE== pMe->m_nSmartStateType
                        || SMART_STATE_IDD_SELECT_RECORD== pMe->m_nSmartStateType)
                    {
                        // store the menu select
                        PUSH_LISTMENU_SEL(pMe->m_wMainListSel);
                        
                        // store the menu index
                        PUSH_LISTMENU_IDX(pMe->m_wMainListIdx);
                        
                        CLOSE_DIALOG(DLGRET_SELECT);
                        return TRUE;
                    }
                    else/*SMART_STATE_IDD_COPY == pMe->m_nSmartStateType*/
                    {
                        if(!IMENUCTL_IsActive(pMenuCtl))
                        {
                            ITEXTCTL_SetActive(pTextCtl, TRUE);
                            IMENUCTL_SetActive(pMenuCtl, TRUE);
                        }
                    }

                }
                
                return TRUE;

#if 0
                case AVK_STAR:
                    if(ITEXTCTL_IsActive(pTextCtl))
                    {
                        ITEXTCTL_SetActive(pTextCtl, FALSE);
                        IMENUCTL_SetActive(pMenuCtl, TRUE);
                    }
                    return TRUE;
#endif          
				//一个dialog里面有两个控件，AVK_RIGHT事件menu不处理，dialog自动转给text处理，同时自动在key_presss时把text激活
                //这里不需要激活text，只有按数字键和#时text才被激活
                /*case AVK_RIGHT:
                case AVK_LEFT:
                    if(ITEXTCTL_IsActive(pTextCtl))
                    {
                        ITEXTCTL_SetActive(pTextCtl, FALSE);
                        IMENUCTL_SetActive(pMenuCtl, TRUE);
                    }
                    return TRUE;
                    */

                default:
                    break;
            }
            //break;
            return TRUE;
      
         case EVT_UPDATE_ARROW_BUTTON:
            // UPDATE prompt bar here
            if( pMe->m_szAlpha != NULL && WSTRLEN(pMe->m_szAlpha) != 0)
            {
                if(SMART_STATE_IDD_LIST == pMe->m_nSmartStateType)
                {
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_DEL);
                }
                else
                 {
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_DEL);
                }
                if(pMe->m_bInsmartnofind == TRUE)
                {
                	#ifndef FEATURE_ALL_KEY_PAD   //add by yangdecai
                    if ( WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) > 0 && ITEXTCTL_GetT9End(pTextCtl) != TC_CURSORSTART )
                    {
                        CONTAPP_DRAW_BOTTOMBAR(BTBAR_DELETE);
                    }
                    else
                    #endif
                    {
                        CONTAPP_DRAW_BOTTOMBAR(BTBAR_BACK);            
                    }  
                    IDISPLAY_Update(pMe->m_pDisplay);  
                }                        
            }
            else if(pMe->m_bInSmartFind == TRUE)
            {
                if(SMART_STATE_IDD_LIST == pMe->m_nSmartStateType)
                {
                    //CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
                }
                else
                {
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
                }
            }
            return TRUE;
            
        case EVT_CTL_TEXT_MODECHANGED:
            CContApp_DrawIMEIcon(pTextCtl, pMe->m_pDisplay);
            ITEXTCTL_SetActive(pTextCtl, TRUE);
            if(pMe->m_bInsmartnofind == TRUE)
            {
                return TRUE;
            }
            IMENUCTL_SetActive(pMenuCtl, TRUE);
            IMENUCTL_Redraw(pMenuCtl);     //dele by yangdecai
            return TRUE;
            
        case EVT_DIALOG_INIT:
            // only set the smart find here,and don't handle this event
            //pMe->m_nCurrSmartIdx = 0;
            pMe->m_bInSmartFind  = FALSE;
            pMe->m_bInsmartnofind = FALSE;      
            //pMe->m_szAlpha = NULL;
            MEMSET(pMe->m_lastAlpha, NULL, sizeof(pMe->m_lastAlpha)); 
            //pMe->m_bAllowSelChange = FALSE; // 防止因设置多选的打勾调用 IMENUCTL_SetSelEx而引起的没必要的 EVT_CTL_SEL_CHANGED 事件。wuquan.tang
            return TRUE;

        case EVT_DIALOG_START:
        {
            AEERect   textrc;
            AEERect   menurc;
			AEERect       m_drawline;
            int       nBarHeight = GetBottomBarHeight(pMe->m_pDisplay);
            uint32    dwMask = IMENUCTL_GetProperties(pMenuCtl);
            byte      inputHeight = IDISPLAY_GetFontMetrics(pMe->m_pDisplay, AEE_FONT_BOLD, NULL, NULL); 	//AEE_FONT_NORMAL
			
            menurc = pMe->m_rc;
            //menurc.dy =  pMe->m_rc.dy- nBarHeight - inputHeight; 
            menurc.dy =  pMe->m_rc.dy- 2*nBarHeight;
			
            /*if(SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType)
            {
                menurc.dy -= nBarHeight;//在复选界面中，要显示CContApp_DisplaySelectField
            }
            */
            IMENUCTL_SetRect(pMenuCtl, &menurc);
            
            if(SMART_STATE_IDD_LIST == pMe->m_nSmartStateType)
            {
                dwMask = (dwMask & (~MP_WRAPSCROLL)) | MP_UNDERLINE_TITLE;
            }
            else
            {
                dwMask = (dwMask & (~MP_WRAPSCROLL)) | MP_UNDERLINE_TITLE | MP_MULTI_SEL | MP_NO_REDRAW;
            }
            IMENUCTL_SetProperties(pMenuCtl, dwMask);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE);   
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif

            /*if(SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType)
                menurc.dy += nBarHeight;//留空间给CContApp_DisplaySelectField
            */
            SETAEERECT(&textrc,  
                       SEARCH_IMEICON_WIDTH,   menurc.y + menurc.dy, // - 2,    
                       pMe->m_rc.dx - SEARCH_IMEICON_WIDTH, nBarHeight);
#ifdef FEATURE_LANG_CHINESE
            {
                byte inputMode=0;
                
				#ifdef FEATURE_VERSION_K202
				ITEXTCTL_SetProperties(pTextCtl, TP_NOUPDATE|TP_FOCUS_NOSEL);
				m_drawline.y = textrc.y;
				m_drawline.x = 0;
				m_drawline.dy = 1;
				m_drawline.dx = 176;
				textrc.y = textrc.y+1;
				textrc.dy = textrc.dy-1;
				IDISPLAY_FillRect(pMe->m_pDisplay, &m_drawline, RGB_BLACK);
				#else
				ITEXTCTL_SetProperties(pTextCtl, TP_FIXOEM |TP_STARKEY_SWITCH|TP_FOCUS_NOSEL | TP_GRAPHIC_BG);
				#endif
                
                CContApp_GetConfig(pMe, CONTTCFG_QUICKSEARCH_INPUT_MODE, &inputMode, sizeof(byte));
                pMe->m_nCurrentInputMode = inputMode;
                //ITEXTCTL_SetActive(pTextCtl, TRUE);
                (void)ITEXTCTL_SetInputMode( pTextCtl, pMe->m_nInputModeTable[inputMode]);
                ITEXTCTL_SetRect(pTextCtl, &textrc);
                //IMENUCTL_SetActive(pMenuCtl, TRUE);
                CContApp_DrawIMEIcon(pTextCtl, pMe->m_pDisplay);
                ITEXTCTL_SetProperties(pTextCtl, TP_FIXOEM|TP_FOCUS_NOSEL | TP_GRAPHIC_BG);
            }
#else
			#ifdef FEATURE_VERSION_C337
			ITEXTCTL_SetProperties(pTextCtl, TP_STARKEY_SWITCH | TP_FIXOEM|TP_FOCUS_NOSEL);
			#else
            ITEXTCTL_SetProperties(pTextCtl, TP_STARKEY_SWITCH | TP_FIXOEM|TP_FOCUS_NOSEL |TP_GRAPHIC_BG);
            #endif
			
            ITEXTCTL_SetRect(pTextCtl, &textrc);
            CContApp_DrawIMEIcon(pTextCtl, pMe->m_pDisplay);
            #if defined(FEATURE_LANG_ARABIC) ||defined(FEATURE_LANG_THAI)
    	    {
    	    	nv_language_enum_type language;
    	    	OEM_GetConfig( CFGI_LANGUAGE_SELECTION,&language,sizeof(language));
                if(NV_LANGUAGE_ARABIC == language)
                {
    	    		(void)ITEXTCTL_SetInputMode( pTextCtl, AEE_TM_ARABIC);
    	    	}
    	    	else if(NV_LANGUAGE_THAI== language)
    	    	{
    	    		(void)ITEXTCTL_SetInputMode(pTextCtl, AEE_TM_THAI);
    	    	}
    	    	else
    	    	{
    	    		(void)ITEXTCTL_SetInputMode(pTextCtl, AEE_TM_LETTERS);
    	    	}
    	    }
    	    #else
	         		(void)ITEXTCTL_SetInputMode(pTextCtl, AEE_TM_LETTERS);
			#endif
#endif
            ITEXTCTL_SetMaxSize(pTextCtl, MAX_INPUT_NAME_EN); 
            
            if(NULL != pMe->m_szAlpha)
            {
                uint32    dwMask;
                if(IS_ZERO_REC())
                {
                    FREEIF(pMe->m_szAlpha);
                    ITEXTCTL_SetText(pTextCtl, pMe->m_szAlpha, -1);
                     MSG_FATAL("CContApp_SmartMenuHandle-----FALSE",0,0,0);
                    return FALSE;
                }
                
                dwMask = IMENUCTL_GetProperties(pMenuCtl);
                IMENUCTL_SetProperties(pMenuCtl, dwMask|MP_NO_REDRAW);
                ITEXTCTL_SetText(pTextCtl, pMe->m_szAlpha, -1);
                if(SUCCESS != CContApp_FilterSmartItems( pMe,  pMe->m_szAlpha) )
                {
                    pMe->m_bInsmartnofind = TRUE;
                    IMENUCTL_SetActive(pMenuCtl, FALSE);
                    /*guoys add @2008.10.20 for no match dont use select*/    
                    #ifndef FEATURE_ALL_KEY_PAD
                 	CONTAPP_DRAW_BOTTOMBAR(BTBAR_DELETE);
                 	#else
                 	CONTAPP_DRAW_BOTTOMBAR(BTBAR_BACK);
                 	#endif
                    // 智能查找得一条记录，通过option删除，回到这里就不用画no matches,menu redraw会覆盖掉。
                    // 改到EVT_USER_REDRAW 里画 
                    if(FALSE == pMe->m_bDelOk)
                    {
                        CContApp_DisplayRightTopStr(pMe,pMenuCtl,wParam);     
                    }
                }
                else
                {                    
                    (void)CContApp_BuildSmartMenu(pMe, pMenuCtl);            
                    /* MULTITAP_TIMEOUT */
                    (void) ISHELL_SetTimer( pMe->m_pShell,
                                            100, 
                                            CContApp_SmartMenuSetFocus,
                                            (void *)pMe);  
                    
                    //IMENUCTL_Redraw(pMenuCtl);
                    pMe->m_bInsmartnofind = FALSE;
 
                    if(SMART_STATE_IDD_LIST == pMe->m_nSmartStateType)
                    {
                        if(STATE_ONEDIAL_SET == pMe->m_eCurState
                            || (pMe->m_wSelFldType == SINGLE_SELECT_NUMBER && STARTMETHOD_SELECTFIELD == pMe->m_eStartMethod)
                            || (pMe->m_wSelFldType == SINGLE_SELECT_GROUPLIST && STARTMETHOD_GROUPLIST == pMe->m_eStartMethod))
                        {
                            //CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
                            CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_DEL);
                        }
                        else
                        {
                            //CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
                            CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_DEL);
                        }
                    }
                    else if(SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType)
                    {
                        /*
                        if(pMe->m_pSmartBufLen >pMe->m_nleftNum)
                        {
                            CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_DEL);
                        }
                        else*/
                        {
                            CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_DEL);
                        }
                    }
                    else
                    {
                        // SMART_STATE_IDD_COPY == pMe->m_nSmartStateType || SMART_STATE_IDD_DELETE == pMe->m_nSmartStateType
                        CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_DEL);
                    }
                }
                pMe->m_bInSmartFind = TRUE;
            }
            break;
        }
        
        case EVT_CTL_SEL_CHANGED:
            //if( FALSE == pMe->m_bAllowSelChange)
            //{
            //    return TRUE;
            //}
            // In smart find mode ?
            if(!pMe->m_bInSmartFind)
            {
                 MSG_FATAL("CContApp_SmartMenuHandle-----FALSE",0,0,0);
                return FALSE;
            }
            if(pMe->m_wSelectSmart != wParam)
            {
                pMe->m_wSelectSmart = wParam;
               
                pMe->m_wSelectCont = wParam;
/*
                if(SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType)
                {
                    //CContApp_GetRecByID(pMe, pMe->m_wSelectSmart);
                    wField = MASK_ADDRFIELD_NAME;

                    if(!pMe->m_bInSmartFind  || pMe->m_pSmartBufLen == 0)
                    {       
                        (void)CContApp_BuildSmartStateReturnMenu(pMe, pMenuCtl); 
                    }
                    else
                    {
                        (void)CContApp_BuildSmartMenu(pMe, pMenuCtl);
                    }            
                    //CContApp_DisplaySelectField(pMe, pMe->m_wFieldIndex);
             //   IDISPLAY_Update(pMe->m_pDisplay); 
                    IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectSmart);
                    (void)IMENUCTL_Redraw(pMenuCtl);
                }
                */
            } 
            return TRUE;
            
        default:
            break;
    }
     MSG_FATAL("CContApp_SmartMenuHandle-----FALSE",0,0,0);
    return FALSE;
}// CContApp_SmartMenuHandle

void CContApp_SmartMenuSetFocus(void *pUser)
{
    CContApp *pMe = (CContApp *)pUser; 
    IMenuCtl        *pMenuCtl = NULL;    
    ITextCtl          *pTextCtl = NULL; 
    int16               MenuCtlID;
    int16               TextCtlID;

    if( NULL == pMe )
    {
        return ;
    }

    switch(pMe->m_nSmartStateType)
    {
      case SMART_STATE_IDD_LIST:
          MenuCtlID =  IDC_LIST_MENU;
          TextCtlID =IDC_LIST_TEXT;
          break;
    
      case SMART_STATE_IDD_SELECT:
          MenuCtlID = IDC_SELECT_MENU;
          TextCtlID =IDC_SELECT_TEXT;
          break;
    
       case SMART_STATE_IDD_COPY:
          MenuCtlID = IDC_COPY_MENU;
          TextCtlID =IDC_COPY_TEXT;
          break;
    
      case SMART_STATE_IDD_DELETE:
          MenuCtlID = IDC_DELETE_MENU;
          TextCtlID =IDC_DELETE_TEXT;
          break; 

      case SMART_STATE_IDD_SELECT_RECORD:
          MenuCtlID = IDC_SELECT_RECORD;
          TextCtlID =IDC_SELECT_RECORD_TEXT;
          break;
          
      default:
          return ;         
    }

    if( NULL != pMe->m_pActiveDlg)
    {
        pMenuCtl = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, MenuCtlID);
        pTextCtl = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, TextCtlID);
        if(pMenuCtl)
        {
            ITEXTCTL_SetActive(pTextCtl, TRUE);// 加这一句可以在text为 FALSE后还保留光标
            IMENUCTL_SetActive(pMenuCtl, TRUE);
        }
        
        if(pTextCtl)                       
            ITEXTCTL_SetActive(pTextCtl, FALSE);

    }
}//CContApp_SmartMenuSetFocus

/*=============================================================================
*    必须在textctl初始化完毕后,才能获得icon id,
*    而且要在dialog更新完之后再更新图标
=============================================================================*/
static void  CContApp_DrawIMEIcon(ITextCtl *pTextCtl, IDisplay *pIDisplay)
{
    boolean     bRet = TRUE;
    IImage     *RightTopImg = NULL; 
    uint32      nResID=0;
    AEERect     IconRect={0};
    IShell      *pIShell =AEE_GetShell();
    Theme_Param_type TParam={0};
    AEEImageInfo myInfo;
    
    if(pTextCtl == NULL ||pIShell == NULL)
        return;
    ITEXTCTL_GetRect(pTextCtl, &IconRect);
    if(IconRect.dx == 0 || IconRect.dy == 0)
    {
        return;
    }
    
    IconRect.x  = 0;
    IconRect.dx = SEARCH_IMEICON_WIDTH;
    
    bRet = ITEXTCTL_GetInfo(pTextCtl, AEE_TMINFO_ICONID, (void *)&nResID);

    if(!bRet)
        return;
    
    Appscom_GetThemeParameters(&TParam);
    IDISPLAY_FillRect(pIDisplay, &IconRect, TParam.seltextColor);
    RightTopImg = ISHELL_LoadResImage(pIShell, AEE_APPSCOMMONRES_IMAGESFILE, nResID);
    if(RightTopImg != NULL)
    {
        IIMAGE_GetInfo(RightTopImg, &myInfo);
        if(myInfo.cy < IconRect.dy)
        {
            IconRect.y += (IconRect.dy-myInfo.cy)>>1;
        }
        IIMAGE_Draw(RightTopImg, IconRect.x, IconRect.y);
        IIMAGE_Release(RightTopImg);
        RightTopImg = NULL;
    }
}//CContApp_DrawIMEIcon

/*=============================================================================

FUNCTION:CContApp_BuildRingMenu

DESCRIPTION:
    向菜单中添加铃声内容
    
PARAMETERS:
    pMe     :
    pMenuCtl: 指向Menu Control的指针
    
RETURN VALUE:
    SUCCESS: 
    EFAILED: If the number of current cache is 0
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
/*static int CContApp_BuildRingMenu(CContApp *pMe, IMenuCtl *pMenuCtl)
{
    AEERingerInfo    ri;
    AECHAR          *pData;
    byte    RingVol;
    ASSERT(pMe != NULL); 

    FARF(ADDR, ("Build Ring menu"));
    
    if(pMenuCtl == NULL)
    {
        return EBADPARM;
    }
    
    // Add the currently available ringers to the menu control
    if(SUCCESS != IRINGERMGR_EnumRingerInit(pMe->m_pRingerMgr))
    {
        return EFAILED;
    }
    
    // 添加铃声到当前菜单
    while (IRINGERMGR_EnumNextRinger(pMe->m_pRingerMgr, &ri))
    {
        if (ri.id > IDI_RING_MENU_USESYSTEM)
        {
            break;
        }
        
        (void)IMENUCTL_AddItem( pMenuCtl, 
                                NULL, 
                                0,
                                (uint16) ri.id,
                                ri.szName,
                                0);
    } 
    
    pMe->m_wSelectRing = IDI_RING_MENU_USESYSTEM;
    
    //get current ringer
    pData = CContApp_GetBufbyFldID(pMe, AEE_ADDRFIELD_RINGTONE);
    
    // have set the ringer
    // 注意：在这里铃声设置的值也是按照字符串来存储的，
    //       因此使用*pData就可以表示所设置的铃声。
    if(pData)
    {
        pMe->m_wSelectRing = *(uint16 *)pData;
    }

    //初始化音量
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                      CFGI_RINGER_VOL,
                      &RingVol,
                      sizeof(byte));

    if(pMe->m_wSelectRing != IDI_RING_MENU_USESYSTEM &&
       RingVol != OEMSOUND_MUTE_VOL)    
    {
        (void)IRINGERMGR_Play(pMe->m_pRingerMgr, pMe->m_wSelectRing, 0);
    }

    // Set menu select    
    MENU_INIT_SELICONS(pMenuCtl);
    MENU_SET_SELICON(pMenuCtl, pMe->m_wSelectRing, TRUE);
    
    IMENUCTL_SetSel(pMenuCtl,pMe->m_wSelectRing);
    
    return SUCCESS;
}// CContApp_BuildRingMenu*/

/*=============================================================================

FUNCTION:CContApp_SaveRingField

DESCRIPTION:
    保存铃声字段
    
PARAMETERS:
    pMe     :
    pMenuCtl: 指向Menu Control的指针
    
RETURN VALUE:
    SUCCESS: 
    EFAILED: If the number of current cache is 0
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
 int CContApp_SaveRingField(CContApp *pMe, AECHAR* ringName)
{
    int     nRet;
    //ringID *ringer=NULL;
    AECHAR  *ring= NULL;
    uint16  wIdx=0;
    AEEAddrField  *pcf=NULL;
    AEEAddrField  cf={0};
    
    ASSERT(pMe != NULL);

    ring = (AECHAR*)CContApp_GetBufbyFldID(pMe, AEE_ADDRFIELD_RINGTONE);
    if(NULL == ring)
    {
        // Add a new field
        nRet = CContApp_AddField(pMe, pMe->m_wEditCont, AEE_ADDRFIELD_RINGTONE, (void*)ringName);
    }
    else
    {        
        /*nRet = CContApp_SaveField( pMe,
                                   pMe->m_wEditCont,
                                   wIdx,
                                   AEE_ADDRFIELD_RINGTONE,
                                   (void *)pMe->m_nRingToneID);*/
        if(WSTRCMP(ring, ringName) == 0)
        {
            ERR("same ringer,return directly",0,0,0);
            return SUCCESS;
        }

        // Update the field
        wIdx = CContApp_GetIdxbyFldID(pMe, AEE_ADDRFIELD_RINGTONE);
        //CContApp_DeletePhoneFld(pMe, pMe->m_wEditCont, wIdx);
        
        pcf = IVector_ElementAt(pMe->m_pFldIv, wIdx);
        
        if ( NULL == pcf )
        {
            FARF(ADDR, ("Failed to get the field from m_pFldIv!"));
            return EFAILED;
        }
        if(ringName[0] == (AECHAR)'\0')
        {
            FREE_AEEAddrField(*pcf);
            return CContApp_DeletePhoneFld( pMe, pMe->m_wEditCont, wIdx);
        }            
        
        if(NULL != ringName)
        {
			
            if ( SUCCESS != CContApp_BuildAddrField(AEE_ADDRFIELD_RINGTONE, (void*)ringName, &cf, TRUE) )
            {
                FARF(ADDR, ("Failed to build the field!"));
                FREE_AEEAddrField(cf);
                return EFAILED;
            }
            
            // 注意：此处故意没有释放cf中分配的内容，因为经过替换pcf后，将会在
            //       CContApp_FreeAddrFlds()中释放
            FREE_AEEAddrField(*pcf);
            *pcf = cf;
            nRet = CContApp_UpdateAddrFields(pMe, pMe->m_wEditCont);
        }
        else
        {
            return EFAILED;
        }
    }
    return nRet;
}// CContApp_SaveRingField

/*=============================================================================

FUNCTION:CContApp_MergeField

DESCRIPTION: 
    复制字段到话机

PARAMETERS:
    pMe     :
    pNameFld: name field
    pNumFld : number field

RETURN VALUE:
    SUCCESS   : 
    EMAXFIELD : if field type is full in current record
    EEXIST    : if field have exist
    EBADPARM  : if parameter is bad
    
COMMENTS:

SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
int CContApp_MergeField( CContApp       *pMe, 
                         uint16          wRecID,
                         void           *pFldData,
                         AEEAddrFieldID  wFldID)
{
    IAddrRec     *pAddrRec;
    AEEAddrField  addrFld;
    int           nRet;
    
    ASSERT(pMe != NULL);
    
    // Check parameter
    if(pFldData == NULL)
    {
        return EBADPARM;
    }

	
    // Set field data
    if(SUCCESS != CContApp_BuildAddrField( wFldID,
                                           pFldData,
                                           &addrFld,
                                           FALSE))
    {
        return EFAILED;
    }
    
    pAddrRec = IADDRBOOK_GetRecByID(pMe->m_pAddrPhone, wRecID);
    
    // find the same name record then merge the field number
    if(pAddrRec)
    {
        // Check field number
        nRet = CContApp_CheckFldInRec( pMe,
                                       pAddrRec,
                                      &addrFld);
        
        if(nRet != SUCCESS)
        {
            IADDRREC_Release(pAddrRec);
            return nRet;
        }//else pass the check
        
        // Add the field
        if(SUCCESS != IADDRREC_AddField(pAddrRec, &addrFld))
        {
            IADDRREC_Release(pAddrRec);
            return EFAILED;
        }
        
        IADDRREC_Release(pAddrRec);
        return SUCCESS;
    }
    
    return EFAILED;
}// CContApp_MergeField


/*=============================================================================

FUNCTION:CContApp_LoadIconInfo

DESCRIPTION:
    载入Icon的信息
    
PARAMETERS:
    pMe     :

RETURN VALUE:
    SUCCESS: 
    EFAILED: If the number of current cache is 0
    
COMMENTS:
    
SIDE EFFECTS:
    pMe->m_wSelectFld
    pMe->m_wSelectCont
    
SEE ALSO:

=============================================================================*/
int CContApp_LoadIconInfo(CContApp *pMe)
{
    FileInfo  fi;
    char     *pszName;
    
    pMe->m_nFileNum = 0;
    
    (void)IFILEMGR_EnumInit(pMe->m_pFileMgr, MP_CONTPIC_DIR, FALSE); 
    
    while(IFILEMGR_EnumNext(pMe->m_pFileMgr, &fi))
    {
        //如果当前文件是BMP文件才读出
        if(STRENDS(SUFFIX_PIC, fi.szName))
        {
            pszName = STRRCHR(fi.szName, DIRECTORY_CHAR);
            pszName++;
            UTF8TOWSTR( (byte*)(pszName),
                        STRLEN(pszName)+1,
                        pMe->m_wFileList[pMe->m_nFileNum], 
                        sizeof(AECHAR) * STRLEN(pszName));
            pMe->m_nFileNum++;
        }
    }
    
    if(pMe->m_nFileNum)
    {
        return SUCCESS;
    }
    else
    {
        return EFAILED;
    }
}// CContApp_LoadIconInfo

/*=============================================================================

FUNCTION:CContApp_DrawJpegImage

DESCRIPTION:
    用于加速画Jpeg图片
    
PARAMETERS:
    pMe     :
    
RETURN VALUE:
    Image pointer
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
/*static int CContApp_DrawJpegImage(CContApp *pMe, uint16 wIconID, AEERect *prc)
{
    ICamera    *pCamera;
    IBitmap    *pFrame;
    int         actw=0,acth=0;
    char        szName[AEE_MAX_FILE_NAME] = {0};
    char        szPhoto[AEE_MAX_FILE_NAME] = {0};
    int         len;
    uint16      xPos = prc->x,yPos = prc->y;
    uint16      xOffSet = 0,yOffSet = 0;
    
    ASSERT(pMe != NULL);
    
    if(wIconID > pMe->m_nFileNum)
    {
        return EFAILED;
    }
    
    len = WSTRLEN(pMe->m_wFileList[wIconID]);
    
    if(len == 0)
    {
        return EFAILED;
    }
    
    // Generate the filename
    STRCPY(szPhoto, MP_CONTPIC_DIR);
    STRCAT(szPhoto, DIRECTORY_STR);
    (void)WSTRTOUTF8( (AECHAR *) pMe->m_wFileList[wIconID],
                      (int)      len, 
                      (byte *)   szName, 
                      (int)      sizeof(szName));;
    STRCAT(szPhoto, szName);
    
    // 创建Camera接口
    if (AEE_SUCCESS != ISHELL_CreateInstance( pMe->m_pShell, 
                                              AEECLSID_CAMERA, 
                                              (void **)&pCamera))
    {
        return EFAILED; 
    }
    
    // 硬件解码Jpeg
    ICAMERA_DecodeJpeg(pCamera,szPhoto,&actw,&acth,&pFrame,pMe->m_pDisplay);
    if(pFrame)
    {
        AEEBitmapInfo sInfo;
        IBITMAP_GetInfo(pFrame, &sInfo, sizeof(sInfo));
        
        // Caculate the Draw erea
        if((uint16)prc->dx > sInfo.cx)
        {
            xPos += (((uint16)prc->dx - sInfo.cx)/2);
        }
        else
        {
            xOffSet += ((sInfo.cx - (uint16)prc->dx)/2);
        }
        
        if((uint16)prc->dy > sInfo.cy)
        {
            yPos += (((uint16)prc->dy - sInfo.cy)/2);
        }
        else
        {
            yOffSet += ((sInfo.cy - (uint16)prc->dy)/2);
        }
        
        IDISPLAY_BitBlt( pMe->m_pDisplay, 
                         xPos,
                         yPos,
                         prc->dx, 
                         prc->dy, 
                         pFrame,
                         xOffSet,
                         yOffSet, 
                         AEE_RO_COPY);
        
        IBITMAP_Release(pFrame);
    }
    else
    {
        ICAMERA_Release(pCamera);
        pCamera = NULL;
        return EFAILED;
    }
    
    // Release camera interface
    ICAMERA_Release(pCamera);
    pCamera = NULL;
    
    // Draw frame
    IDISPLAY_FrameRect(pMe->m_pDisplay, prc);
    IDISPLAY_Update(pMe->m_pDisplay);
    return SUCCESS;
}// CContApp_DrawJpegImage*/



/*=============================================================================

FUNCTION:CContApp_ImportVCF

DESCRIPTION: 
    导入vCard文件

PARAMETERS:
    pMe     :
    pszFile : 导入的VCF文件名

RETURN VALUE:
    SUCCESS    :
    EMAXRECORD : Reach the device max capacity
    EEXIST     : Record name is exist
    EBADPARM   : Parameter is bad
    
COMMENTS:

SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
/*int CContApp_ImportVCF(CContApp *pMe, const char *pszFile)
{
    IAddrRec     *pAddrRec;
    AEEAddrField *pAddrFld;
    int           nFldNum;
    int           i;
    
    ASSERT(pMe != NULL);
    
    // Check parameter
    if(pszFile == NULL)
    {
        FARF(ADDR, ("No set the file name parameter!"));
        return EBADPARM;
    }
    
    // Check the capcity
    if(IS_MAX_REC(pMe->m_pAddrPhone))
    {
        return EMAXRECORD;
    }
    // Check the Name
    for(i=0; i<nFldNum; i++)
    {
        if(pAddrFld[i].fID == AEE_ADDRFIELD_NAME)
        {
            if(CContApp_CheckPhoneName(pMe, (AECHAR *)pAddrFld[i].pBuffer))
            {
                return EEXIST;
            }
            break;
        }
    }
    
    // Build the record
    pAddrRec = IADDRBOOK_CreateRec( pMe->m_pAddrPhone,
                                    AEE_ADDR_CAT_PERSONAL,
                                    pAddrFld,
                                    nFldNum);
    
    if(NULL == pAddrRec)
    {
        FARF(ADDR, ("Failed to create the record!"));
        return EFAILED;
    }
    
    IADDRREC_Release(pAddrRec);
    return SUCCESS;
}// CContApp_ImportVCF*/

/*=============================================================================

FUNCTION:CContApp_ExportVCF

DESCRIPTION: 
    导入vCard文件

PARAMETERS:
    pMe     :
    wContID : 导出的记录号
    pszPath : 导出的路径
    
RETURN VALUE:
    SUCCESS:
    
COMMENTS:

SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
/*int CContApp_ExportVCF(CContApp *pMe, uint16 wContID, char *pszPath)
{
    AEEAddrField  *pFld;
    int            i,wTotalFields;
    char          *pName = NULL;
    char           Fmt[] = "%s.vcf";
    
    ASSERT(pMe != NULL);
    
    if ( SUCCESS != CContApp_FindAddrRec(pMe, wContID) )
    {
        FARF(ADDR, ("Failed to find the pMe->m_pCurrRec"));
        return EFAILED;
    }
    
    //First get the field count and itereate through the
    //fields to get the data
    wTotalFields = IADDRREC_GetFieldCount(pMe->m_pCurrRec);
    
    // Get the name string
    for (i = 0; i < wTotalFields; i++)
    {
        pFld = IADDRREC_GetField( pMe->m_pCurrRec, i );
        if ( pFld == NULL )
        {
            FARF(ADDR, ("Failed to get the field"));
            return EFAILED;
        }
        
        // check field id
        if(pFld->fID == AEE_ADDRFIELD_NAME)
        {
            pName = (char *)MALLOC(MAX_LEN_COMMBUF);
            if(!pName)
            {
                return ENOMEMORY;
            }
            
            (void)WSTRTOUTF8((AECHAR *) pFld->pBuffer, 
                             (int)      WSTRLEN(pFld->pBuffer),
                             (byte *)   pName, 
                             (int)      MAX_LEN_COMMBUF);
            
            // Generate the filename
            SPRINTF(pName, Fmt, pName);
            break;
        }
    }
    
    MEMSET(pMe->m_pBuf, 0, sizeof(pMe->m_pBuf));
    STRCPY(pMe->m_pBuf, pszPath);           // Link the path string
    STRCAT(pMe->m_pBuf, DIRECTORY_STR);     // Link the split
    STRCAT(pMe->m_pBuf, pName);             // Link the name string
    FREEIF(pName);

    return SUCCESS;
}// CContApp_ExportVCF*/

/*==============================================================================
函数：
       CContApp_HandleMsgBoxDlgEvent
说明：
       IDD_MSGBOX 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleMsgBoxDlgEvent( CContApp  *pMe,
                                               AEEEvent   eCode,
                                               uint16     wParam,
                                               uint32     dwParam)
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
            ERR("ISHELL_CreateInstance,AEECLSID_STATIC 2",0,0,0);
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
			 {
                PromptMsg_Param_type  Msg_Param={0};
                AECHAR  wstrText[MSGBOX_MAXTEXTLEN] = {(AECHAR)'\0'};
                AECHAR temp[MAX_LEN_COMMBUF];
                
                if( 0 != pMe->m_wMsgResID)
                {
                    // 从资源文件取消息内容
                    (void)ISHELL_LoadResString(pMe->m_pShell,
                                    CONTAPP_RES_FILE_LANG,                                
                                    pMe->m_wMsgResID,
                                    wstrText,
                                    sizeof(wstrText));
                }
                else
                {                    
                    //STRTOWSTR("%d records done", wstrFormat, sizeof(wstrFormat));
                    (void)ISHELL_LoadResString(pMe->m_pShell,
                                    CONTAPP_RES_FILE_LANG,                                
                                    IDS_RECORDS_DONE,
                                    pMe->m_szFmtBuf,
                                    sizeof(pMe->m_szFmtBuf));
                    if(pMe->m_eCurState == STATE_DELETING)
                    {
                        WSPRINTF(temp,sizeof(temp),pMe->m_szFmtBuf,pMe->m_wDeleteRecCnt);
                        WSTRCPY(wstrText, temp);
                    }

                    else if(pMe->m_eCurState == STATE_COPYING)
                    {
                        if(pMe->m_eErrType == PHONE_FULL) /*  memory full */
                        {
                            MEMSET(pMe->m_szFmtBuf, 0, sizeof(pMe->m_szFmtBuf));
                            (void)ISHELL_LoadResString(pMe->m_pShell,
                                            CONTAPP_RES_FILE_LANG,                                
                                            IDS_FULL_PHONE_DONE,
                                            pMe->m_szFmtBuf,
                                            sizeof(pMe->m_szFmtBuf));
                            
                            WSPRINTF(temp,sizeof(temp),pMe->m_szFmtBuf,pMe->m_wCopyRecCnt);
                            // Copy to comm buffer
                            WSTRCPY(wstrText, temp);
                        }
                        else if(pMe->m_eErrType == UIMCARD_FULL)
                        {
                            MEMSET(pMe->m_szFmtBuf, 0, sizeof(pMe->m_szFmtBuf));
                            (void)ISHELL_LoadResString(pMe->m_pShell,
                                            CONTAPP_RES_FILE_LANG,                                
                                            IDS_FULL_UIM_DONE,
                                            pMe->m_szFmtBuf,
                                            sizeof(pMe->m_szFmtBuf));
                            
                            WSPRINTF(temp,sizeof(temp),pMe->m_szFmtBuf,pMe->m_wCopyRecCnt);
                            WSTRCPY(wstrText, temp);
                        }
                        else if(pMe->m_nFailNum > 0)
                        {
                            MEMSET(pMe->m_szFmtBuf, 0, sizeof(pMe->m_szFmtBuf));
                            (void)ISHELL_LoadResString(pMe->m_pShell,
                                            CONTAPP_RES_FILE_LANG,                                
                                            IDS_COPY_EXPECT,
                                            pMe->m_szFmtBuf,
                                            sizeof(pMe->m_szFmtBuf));
                            
                            WSPRINTF(temp,sizeof(temp),pMe->m_szFmtBuf,pMe->m_wCopyRecCnt, pMe->m_nFailNum);
                            WSTRCPY(wstrText, temp);
                            pMe->m_nFailNum = 0;
                        }
                        else
                        {
                            if(pMe->m_bIsNotComplete == TRUE)
                            {
                                pMe->m_bIsNotComplete = FALSE;
                                WSPRINTF(temp,sizeof(temp),pMe->m_szFmtBuf,pMe->m_wCopyRecCnt);
                                
                                WSTRCPY(wstrText, temp);
                            }
                        }
                    }
                }
                
                //Msg_Param.ePMsgType = MESSAGE_INFORMATION;
                Msg_Param.ePMsgType = pMe->m_eMsgType;
                Msg_Param.pwszMsg = wstrText;
                Msg_Param.eBBarType = BTBAR_NONE;
                DrawPromptMessage(pMe->m_pDisplay, pStatic, &Msg_Param);
            }
            // 更新界面
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);

            return TRUE;

        case EVT_USER_REDRAW:
           
            
            // 启动发送关闭对话框事件的定时器
            (void) ISHELL_SetTimer(pMe->m_pShell,
                                PROMPTMSG_TIMER,
                                CContApp_HandleDialogTimer,
                                pMe);
            
            return TRUE;

        case EVT_DIALOG_END:
            (void) ISHELL_CancelTimer(pMe->m_pShell, CContApp_HandleDialogTimer, pMe);
  
            // 此对话框返回值仅为 DLGRET_MSGBOX_OK ，为防止挂起 Applet 
            // 关闭对话框回到错误状态，显示给对话框返回值赋值
            pMe->m_eDlgRet = DLGRET_MSGBOX_OK;
            
            ISTATIC_Release(pStatic);
            pStatic = NULL;
            
            return TRUE;

        case EVT_KEY_PRESS:
            CLOSE_DIALOG(DLGRET_MSGBOX_OK);
            return TRUE;
        
        case EVT_DISPLAYDIALOGTIMEOUT:
            CLOSE_DIALOG(DLGRET_MSGBOX_OK);
            return TRUE;

        default:
            break;
    }

    return FALSE;
}

/*==============================================================================
函数：
       CContApp_HandleYesNoDlgEvent
说明：
       IDD_YESNO 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleYesNoDlgEvent( CContApp  *pMe,
                                              AEEEvent   eCode,
                                              uint16     wParam,
                                              uint32     dwParam)
{
   static IStatic *pStatic = NULL;
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif

   if (NULL == pStatic)
   {
        AEERect rect = {0};
        if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                                 AEECLSID_STATIC,
                                                 (void **)&pStatic))
        
        {
            return FALSE;
            ERR("ISHELL_CreateInstance,AEECLSID_STATIC 2",0,0,0);
        }        
        ISTATIC_SetRect(pStatic, &rect);  
   }

    if ((NULL == pStatic) ||(NULL == pMe))
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        {
            return TRUE;
        }
        
        case EVT_DIALOG_START:
            
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
                                    
            // Note: Fall through to the EVT_USER_REDRAW
            //lint -fallthrough
            return TRUE;
        case EVT_USER_REDRAW:
            // Draw prompt bar here
            {
                PromptMsg_Param_type  Msg_Param={0};                                
                Msg_Param.ePMsgType = MESSAGE_CONFIRM;
                Msg_Param.pwszMsg = pMe->m_pMsgBox_Msg;
                Msg_Param.eBBarType = BTBAR_OK_BACK;
                DrawPromptMessage(pMe->m_pDisplay, pStatic, &Msg_Param);
            }
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            return TRUE;
            
        case EVT_DIALOG_END:
            (void) ISHELL_CancelTimer( pMe->m_pShell,  CContApp_HandleDialogTimer, pMe );
            if(pStatic != NULL)
            {
            	ISTATIC_Release(pStatic);
            	pStatic = NULL;
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                /*case AVK_INFO:*/
                case AVK_SELECT:
                    // 如下代码限制在此界面的快速按键
                    (void) ISHELL_SetTimer( pMe->m_pShell, TIMEOUT_MS_QUICK,
                                            CContApp_HandleDialogTimer, pMe );
                    return TRUE;
                    
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_NO);
                    return TRUE;
                    
                default:
                    break;
                    
            }
            break;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif                       
        case EVT_DISPLAYDIALOGTIMEOUT:
            CLOSE_DIALOG(DLGRET_YES);
            return TRUE;
            
        default:
            break;
            
    }
    
    return FALSE;
} // CContApp_HandleYesNoDlgEvent

/*==============================================================================
函数：
       CContApp_HandleListDlgEvent
说明：
       IDD_LIST 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleListDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam)
{
    IMenuCtl        *pMenuCtl = NULL;    
    ITextCtl          *pTextCtl = NULL; 
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    MSG_FATAL("CContApp_HandleListDlgEvent...........",0,0,0);
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg, IDC_LIST_MENU);
    pTextCtl = (ITextCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg, IDC_LIST_TEXT);  
   
	MSG_FATAL("CContApp_HandleListDlgEvent ITEXTCTL_GetInputMode=%x",ITEXTCTL_GetInputMode(pTextCtl,NULL),0,0);

    if(ITEXTCTL_GetInputMode(pTextCtl,NULL) == AEE_TM_SYMBOLS)
    {
        //IMENUCTL_SetActive(pMenuCtl,FALSE);
        return ITEXTCTL_HandleEvent( pTextCtl, eCode, wParam, dwParam);
    }
    pMe->m_nSmartStateType = SMART_STATE_IDD_LIST;
	#ifdef FEATURE_VERSION_K202
	ITEXTCTL_SetProperties(pTextCtl, TP_FIXSETRECT|TP_FIXOEM|TP_USELESS_UPDOWN|TP_FOCUS_NOSEL);
	#endif
    if(CContApp_SmartMenuHandle(pMe, pMenuCtl, pTextCtl, eCode,wParam))
    {
        return TRUE;
    }
    
    CContApp_ListMenuScroll(pMe, pMenuCtl, eCode, wParam);

	MSG_FATAL("CContApp_HandleListDlgEvent eCode=%x",eCode,0,0);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
            pMe->m_wYPos=0;
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            // Build the list Menu
            if(NULL == pMe->m_szAlpha)
            {
                (void)CContApp_BuildListMenu(pMe, pMenuCtl);
            }
            if(MENU_SELECT_NULL == pMe->m_wSelectCont)
            {
                pMe->m_wSelectCont = IMENUCTL_GetItemID(pMenuCtl, 0);
            }

            if(TRUE == pMe->m_bDelOk && TRUE != pMe->m_bInsmartnofind)
            {
                pMe->m_bDelOk = FALSE;
                pMe->m_wSelectCont = pMe->m_wDelReturnSelId;
            }
			
            #ifdef FEATURE_MYANMAR_INPUT_MOD
			ITEXTCTL_SetInputMode( pTextCtl,AEE_TM_LETTERS);
			ITEXTCTL_SetActive(pTextCtl, FALSE);
			#else
			#if defined(FEATURE_LANG_ARABIC) ||defined(FEATURE_LANG_THAI)  //add by yangdecai
            {
				nv_language_enum_type language;
				OEM_GetConfig( CFGI_LANGUAGE_SELECTION,&language,sizeof(language));
				if(NV_LANGUAGE_ARABIC == language)
				{
					MSG_FATAL("seting........................AEE_TM_ARABIC",0,0,0);
					(void)ITEXTCTL_SetInputMode( pTextCtl, AEE_TM_ARABIC);
				}
				else if(NV_LANGUAGE_THAI == language)
				{
					(void)ITEXTCTL_SetInputMode(pTextCtl, AEE_TM_THAI);
				}
				else
				{
					(void)ITEXTCTL_SetInputMode( pTextCtl, AEE_TM_LETTERS);
				}
         	}
         	#else
         	(void)ITEXTCTL_SetInputMode( pTextCtl, AEE_TM_LETTERS);
			#endif
			#endif
            //IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY);   
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
        {
            //MP_NO_REDRAW
            uint32    dwMask = IMENUCTL_GetProperties(pMenuCtl);
            IMENUCTL_SetProperties(pMenuCtl, dwMask & (~MP_NO_REDRAW));
            MSG_FATAL("--pMenuCtl=%x",IMENUCTL_GetSel(pMenuCtl),0,0);

			MSG_FATAL("***zzg contack list EVT_USER_REDRAW***", 0,0,0);
            
			{
                //add by xuhui
                AECHAR WTitle[40] = {0};
                //IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);

				#ifdef FEATURE_VERSION_C337
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        CONTAPP_RES_FILE_LANG,                                
                        IDS_PHONEBOOK,
                        WTitle,
                        sizeof(WTitle));
				#else
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        CONTAPP_RES_FILE_LANG,                                
                        IDS_APPLET,
                        WTitle,
                        sizeof(WTitle));
				#endif
                
                if(pMe->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
            }
            #ifdef FEATURE_VERSION_SKY
            {
            IImage *image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);
            Appscommon_ResetBackground(pMe->m_pDisplay,image, APPSCOMMON_BG_COLOR, 
                                                    &pMe->m_rc, 0, 0);
             if( image != NULL)
             {
               IIMAGE_Draw( image, 0, 0);
               IIMAGE_Release( image);
             }
              IMENUCTL_Redraw(pMenuCtl);
            }
            #endif
            if(IS_ZERO_REC())
            {
                CContApp_DrawNorecord(pMe, pMenuCtl);
                CONTAPP_DRAW_BOTTOMBAR(BTBAR_ADD_BACK);                
            }
            else if(TRUE == pMe->m_bDelOk && TRUE == pMe->m_bInsmartnofind)
            {
                // 智能查找到一个记录，并且把这一条删除掉。
                 pMe->m_bDelOk = FALSE;
                ITEXTCTL_SetActive(pTextCtl, TRUE);
                 CContApp_DisplayRightTopStr(pMe,pMenuCtl,0);
                 #ifndef FEATURE_ALL_KEY_PAD
                 CONTAPP_DRAW_BOTTOMBAR(BTBAR_DELETE);
                 #else
                 CONTAPP_DRAW_BOTTOMBAR(BTBAR_BACK);
                 #endif
            }
            else
            {
                 
                // Restore the menu select from suspend
                if(pMe->m_wSelectStore != MENU_SELECT_NULL)
                {
                    IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
                }
                else
                {
                    IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectCont);
                }
                if(NULL == pMe->m_szAlpha)
                {
                    // Draw prompt bar here
                    if(STARTMETHOD_ADDFIELD == pMe->m_eStartMethod
                        || STARTMETHOD_ADDEMAIL == pMe->m_eStartMethod
                        || STARTMETHOD_ADDURL == pMe->m_eStartMethod
                        || STATE_ONEDIAL_SET == pMe->m_eCurState
                        || (pMe->m_wSelFldType == SINGLE_SELECT_NUMBER && STARTMETHOD_SELECTFIELD == pMe->m_eStartMethod)
                        || (pMe->m_wSelFldType == SINGLE_SELECT_GROUPLIST && STARTMETHOD_GROUPLIST == pMe->m_eStartMethod))
                    {
                        CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
                    }
                    else
                    {
                        CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
                    }
                }
            }


			//Add By zzg  2010_08_14..
			// 在调用了其他界面(如Symbol)返回后要重新加载CONTACT
			{
				boolean b_TextctlActive = ITEXTCTL_IsActive(pTextCtl);
	            int           n_KeywordsLen =0;
	            boolean isSearchTextChg = FALSE;

				MSG_FATAL("***zzg reload contack after symbol***", 0,0,0);
	                   
	            isSearchTextChg = TRUE; 
	            FREEIF(pMe->m_szAlpha);
	            pMe->m_szAlpha = WSTRDUP(ITEXTCTL_GetTextPtr(pTextCtl));
	                 
	            if(NULL != pMe->m_szAlpha && WSTRLEN(pMe->m_szAlpha) > 0)
	            {
	                n_KeywordsLen = WSTRLEN(pMe->m_szAlpha);	                
	            }
	            else
	            {
	                FREEIF(pMe->m_szAlpha);
	            }  
	            if(b_TextctlActive)  
	            {
                  IMENUCTL_SetActive(pMenuCtl, FALSE);
                }
	            // research the record
	            if(n_KeywordsLen > 0 && isSearchTextChg == TRUE)
	            {
	                pMe->m_bInSmartFind = TRUE;
					
	                if(SUCCESS != CContApp_FilterSmartItems( pMe,  pMe->m_szAlpha))
	                {   
	                    uint32    dwMask = IMENUCTL_GetProperties(pMenuCtl);	                    
	                    IMENUCTL_SetProperties(pMenuCtl, dwMask |MP_NO_REDRAW);
	                    
	                    pMe->m_bInsmartnofind = TRUE;
	                    IMENUCTL_SetActive(pMenuCtl, FALSE);							
	                    
						#ifndef FEATURE_ALL_KEY_PAD
						CONTAPP_DRAW_BOTTOMBAR(BTBAR_DELETE);
                 		#else
						CONTAPP_DRAW_BOTTOMBAR(BTBAR_BACK);
                 		#endif
	                    CContApp_RestoreMenuRect(pMe, pMenuCtl);
	                    CContApp_DisplayRightTopStr(pMe,pMenuCtl,wParam);               
	                }
	                else
	                {
	                    uint32    dwMask = IMENUCTL_GetProperties(pMenuCtl);
	                    IMENUCTL_SetProperties(pMenuCtl, dwMask & (~MP_NO_REDRAW));
	                    
	                    (void)CContApp_BuildSmartMenu(pMe, pMenuCtl);
	                    
	                    (void) ISHELL_SetTimer( pMe->m_pShell,
	                                            200,
	                                            CContApp_SmartMenuSetFocus,
	                                            (void *)pMe);     
	                    IMENUCTL_Redraw(pMenuCtl);
	                    pMe->m_bInsmartnofind = FALSE;

	                    if(SMART_STATE_IDD_LIST == pMe->m_nSmartStateType)
	                    {
	                        if(STATE_ONEDIAL_SET == pMe->m_eCurState
	                            || (pMe->m_wSelFldType == SINGLE_SELECT_NUMBER && STARTMETHOD_SELECTFIELD == pMe->m_eStartMethod)
	                            || (pMe->m_wSelFldType == SINGLE_SELECT_GROUPLIST && STARTMETHOD_GROUPLIST == pMe->m_eStartMethod))
	                        {                                    
	                            CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_DEL);
	                        }
	                        else
	                        {                                    
	                            CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_DEL);
	                        }
	                    }
	                    else if(SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType)
	                    {                               
	                        {
	                            CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_DEL);
	                        }
	                    }
	                    else
	                    {
	                        CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_DEL);
	                    }
	                    
	                }                      
	            }
			}
			//Add End

			#ifdef FEATURE_VERSION_C337
			if (pMe->m_bSpeedDialParam == TRUE)
			{
				CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_BACK);
			}
			#endif
			
            /*必须在textctl初始化完毕后,才能获得icon id,而且要在dialog更新完之后再更新图标*/
            CContApp_DrawIMEIcon(pTextCtl, pMe->m_pDisplay);
			ITEXTCTL_Redraw(pTextCtl);
            CContApp_DrawScrollBar(pMe, pMenuCtl);
            IDISPLAY_Update(pMe->m_pDisplay); 

			return TRUE;			
        }            

        case EVT_DISPLAYDIALOGTIMEOUT:
            if(IS_ZERO_REC())
            {
                CLOSE_DIALOG(DLGRET_OK);
                return TRUE;
            }
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(IS_ZERO_REC())
            {
                (void) ISHELL_CancelTimer( pMe->m_pShell,  CContApp_HandleDialogTimer, pMe );
            }
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    if(dwParam ==1)
                    {
                    	pMe->m_wSelectCont = IMENUCTL_GetSel(pMenuCtl);
                    	CLOSE_DIALOG(DLGRET_DELETE);
                    }
                    else
                    {
                    	pMe->m_wMainListSel = MENU_SELECT_NULL;
                    	pMe->m_wMainListIdx = 0;
                    	CLOSE_DIALOG(DLGRET_CANCELED);
                    }
                    return TRUE;
                case AVK_LCTRL:   //add by yangdecai
                	{
                		ITEXTCTL_SetActive(pTextCtl, TRUE);
                	}
                    return TRUE;
                case AVK_CAMERA:
                {
                	 #if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)|| defined(FEATURE_VERSION_1110W516) || defined(FEATURE_VERSION_W027)
                	 {
						nv_item_type	SimChoice;
						OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
						if(SimChoice.sim_select==AVK_SEND_TWO)
						{
							if(IS_ZERO_REC())
		                    {
		                        return TRUE;
		                    }
		                    pMe->m_wSelectCont = IMENUCTL_GetSel(pMenuCtl);
		                    // store the menu select
		                    PUSH_LISTMENU_SEL(pMe->m_wMainListSel);
		                    
		                    // store the menu index
		                    PUSH_LISTMENU_IDX(pMe->m_wMainListIdx);
		                    
		                    // Load the current fields
		                    if(SUCCESS != CContApp_LoadAddrFlds( pMe, 
		                                                         pMe->m_wSelectCont,
		                                                         SINGLE_SELECT_NUMBER))
		                    {
		                        pMe->m_wErrStrID = IDS_ERR_LOADFIELDS;
		                        CLOSE_DIALOG(DLGRET_ERR);
		                        return NFSMACTION_CONTINUE;
		                    }
		                    // 如果当前记录仅有一个号码则直接发起呼叫
		                    if(1 == CContApp_GetCurrFldNum(pMe, AEE_ADDRFIELD_PHONE_GENERIC))
		                    {
		                        (void)CContApp_MakeCall(pMe, IDX_NUM_RUIM_FLD);
		                        return TRUE;
		                    }
		                    
		                    CLOSE_DIALOG(DLGRET_CALL);
		                    return TRUE;
						}
					}
					#endif
                	break;
                }
                case AVK_SEND:
                    if(IS_ZERO_REC())
                    {
                        return TRUE;
                    }
                    #if defined(FEATURE_VERSION_C01) 
					{
						nv_item_type	SimChoice;
						OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
						if(SimChoice.sim_select==AVK_SEND_TWO)
						{
							return TRUE;
						}
					}
					#endif
                    pMe->m_wSelectCont = IMENUCTL_GetSel(pMenuCtl);
                    // store the menu select
                    PUSH_LISTMENU_SEL(pMe->m_wMainListSel);
                    
                    // store the menu index
                    PUSH_LISTMENU_IDX(pMe->m_wMainListIdx);
                    
                    // Load the current fields
                    if(SUCCESS != CContApp_LoadAddrFlds( pMe, 
                                                         pMe->m_wSelectCont,
                                                         SINGLE_SELECT_NUMBER))
                    {
                        pMe->m_wErrStrID = IDS_ERR_LOADFIELDS;
                        CLOSE_DIALOG(DLGRET_ERR);
                        return NFSMACTION_CONTINUE;
                    }
                    // 如果当前记录仅有一个号码则直接发起呼叫
                    if(1 == CContApp_GetCurrFldNum(pMe, AEE_ADDRFIELD_PHONE_GENERIC))
                    {
                        (void)CContApp_MakeCall(pMe, IDX_NUM_RUIM_FLD);
                        return TRUE;
                    }
                    
                    CLOSE_DIALOG(DLGRET_CALL);
                    return TRUE;

                case AVK_SELECT:
                case AVK_INFO:
                    pMe->m_wSelectCont = IMENUCTL_GetSel(pMenuCtl);
                    pMe->m_wEditCont = pMe->m_wSelectCont;
                    
                    // store the menu select
                    PUSH_LISTMENU_SEL(pMe->m_wMainListSel);
                    
                    // store the menu index
                    PUSH_LISTMENU_IDX(pMe->m_wMainListIdx);

					MSG_FATAL("***zzg pMe->m_eCurState=%x", pMe->m_eCurState, 0, 0);
					
                    #ifdef FEATURE_VERSION_C337
					MSG_FATAL("***zzg pMe->m_bSpeedDialParam=%x", pMe->m_bSpeedDialParam, 0, 0);
					if (pMe->m_bSpeedDialParam == TRUE)
					{						
						CLOSE_DIALOG(DLGRET_OK);
                        return TRUE;
					}
					#endif
					
                    if(STATE_ONEDIAL_SET == pMe->m_eCurState)
                    {
                        CLOSE_DIALOG(DLGRET_OK);
                        return TRUE;
                    }
                    
                    if(STARTMETHOD_ADDFIELD == pMe->m_eStartMethod
                        || STARTMETHOD_ADDEMAIL == pMe->m_eStartMethod
                        || STARTMETHOD_ADDURL == pMe->m_eStartMethod)
                    {
                        if(IS_RUIM_REC(pMe->m_wSelectCont)
                            && STARTMETHOD_ADDFIELD == pMe->m_eStartMethod
#ifndef WIN32
                            && WSTRLEN(pMe->m_pFldInputBuf) > OEMRUIMAddr_GetRUIMMaxNumberSize()
#else
                            && WSTRLEN(pMe->m_pFldInputBuf) > MAX_INPUT_NUM
#endif
                            )
                        {
                        	
                            CLOSE_DIALOG(DLGRET_NUMBER_TOOLONG);
                            return TRUE;
                        }
                        
                        CLOSE_DIALOG(DLGRET_OK);
                        return TRUE;
                    }

                    if (pMe->m_wSelFldType == SINGLE_SELECT_NUMBER && STARTMETHOD_SELECTFIELD == pMe->m_eStartMethod
                        || (pMe->m_wSelFldType == SINGLE_SELECT_GROUPLIST && STARTMETHOD_GROUPLIST == pMe->m_eStartMethod))
                    {
                        sSelectFieldInfo  curSelectFieldInfo = {0};  
                        
                        CContApp_GetRecByID(pMe, pMe->m_wSelectSmart);
                                                
                        curSelectFieldInfo.wFieldCount = pMe->m_wFieldCount;
                        if(SUCCESS != CContApp_LoadAddrFlds( pMe, 
                                                             pMe->m_wSelectCont,
                                                             SELECT_ALL))
                        {
                            CLOSE_DIALOG(DLGRET_ERR);
                            return TRUE;
                        }

                        curSelectFieldInfo.wIndex = pMe->m_wSelectSmart;
                        if(CContApp_GetCurrFldNum(pMe, AEE_ADDRFIELD_PHONE_GENERIC) > 1)
                        {                           
                            // store the menu select
                            PUSH_LISTMENU_SEL(pMe->m_wMainListSel);
                            
                            // store the menu index
                            PUSH_LISTMENU_IDX(pMe->m_wMainListIdx);
                            CLOSE_DIALOG(DLGRET_POPNUMFLD);
                            return TRUE;
                        }
                        else
                        {
                            pMe->m_wFieldIndex = CContApp_GetOneNumberFld(pMe, IDX_NUM_RUIM_FLD);
                            curSelectFieldInfo.wFieldMask = pMe->m_wFieldIndex;
                            CContApp_FreeSelectFieldListNode();
                            CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                            CLOSE_DIALOG(DLGRET_OK);
                            return TRUE;
                            
                        }
                    }
                    
                    if(IS_ZERO_REC())
                    {
                        pMe->m_nGroupCat = AEE_ADDR_CAT_OTHER;
                        pMe->m_nAddnewOrEdit = ADDOREDIT_ADD;
                        pMe->m_wSelectEdit = IDI_ADDNEW_MENU_NAME;
                        CLOSE_DIALOG(DLGRET_ADDNEW);
                        return TRUE;
                    }
                    /*pMe->m_wSelectCont = IMENUCTL_GetSel(pMenuCtl);
                    CLOSE_DIALOG(DLGRET_OK);*/

                    // Reset select field
                    pMe->m_boptaleadyView = FALSE;
                    pMe->m_eOptsRetState = STATE_OPTS;
                    CLOSE_DIALOG(DLGRET_DETAIL);
                    return TRUE;
                    
                default:
                    break;
                    
            }
            // App handle all key event
            return TRUE;
            
        case EVT_COMMAND:
            if(IS_ZERO_REC())
            {
                pMe->m_nGroupCat = AEE_ADDR_CAT_OTHER;
                pMe->m_nAddnewOrEdit = ADDOREDIT_ADD;
                pMe->m_wSelectEdit = IDI_ADDNEW_MENU_NAME;
                CLOSE_DIALOG(DLGRET_ADDNEW);
                return TRUE;
            }
            
            if(pMe->m_bInsmartnofind == TRUE)
            {
                return TRUE;
            }
            pMe->m_wSelectCont = wParam;
            pMe->m_wEditCont = wParam;
            MSG_FATAL("pMe->m_wSelectCont======%d",pMe->m_wSelectCont,0,0);
            MSG_FATAL("--pMenuCtl=%x ===%d",IMENUCTL_GetSel(pMenuCtl),IMENUCTL_GetItemID(pMenuCtl,pMe->m_wSelectCont),0);
            
            if(IS_RUIM_REC(pMe->m_wSelectCont)
                && STARTMETHOD_ADDFIELD == pMe->m_eStartMethod
#ifndef WIN32
                && WSTRLEN(pMe->m_pFldInputBuf) > OEMRUIMAddr_GetRUIMMaxNumberSize()
#else
                && WSTRLEN(pMe->m_pFldInputBuf) > MAX_INPUT_NUM
#endif
                )
            {
            	
                CLOSE_DIALOG(DLGRET_NUMBER_TOOLONG);
                return TRUE;
            }
            
            if ((pMe->m_wSelFldType == SINGLE_SELECT_NUMBER && STARTMETHOD_SELECTFIELD == pMe->m_eStartMethod)
                || (pMe->m_wSelFldType == SINGLE_SELECT_GROUPLIST && STARTMETHOD_GROUPLIST == pMe->m_eStartMethod))
            {
                sSelectFieldInfo  curSelectFieldInfo = {0};  
                CContApp_GetRecByID(pMe, pMe->m_wSelectSmart);
                                        
                curSelectFieldInfo.wFieldCount = pMe->m_wFieldCount;
                if(SUCCESS != CContApp_LoadAddrFlds( pMe, 
                                                     pMe->m_wSelectCont,
                                                     SELECT_ALL))
                {
                    CLOSE_DIALOG(DLGRET_ERR);
                    return TRUE;
                }

                curSelectFieldInfo.wIndex = pMe->m_wSelectSmart;
                if(CContApp_GetCurrFldNum(pMe, AEE_ADDRFIELD_PHONE_GENERIC) > 1)
                {                           
                    // store the menu select
                    PUSH_LISTMENU_SEL(pMe->m_wMainListSel);
                    
                    // store the menu index
                    PUSH_LISTMENU_IDX(pMe->m_wMainListIdx);
                    CLOSE_DIALOG(DLGRET_POPNUMFLD);
                    return TRUE;
                }
                else
                {
                    pMe->m_wFieldIndex = CContApp_GetOneNumberFld(pMe, IDX_NUM_RUIM_FLD);
                    curSelectFieldInfo.wFieldMask = pMe->m_wFieldIndex;
                    CContApp_FreeSelectFieldListNode();
                    CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                    CLOSE_DIALOG(DLGRET_OK);
                    return TRUE;
                    
                }
            }
            
            FARF(ADDR, ("pMe->m_wSelectCont = %d",wParam));
            pMe->m_wDelReturnSelId = CContApp_GetListReturnSel(pMe, pMenuCtl, &pMe->m_nCurrMenuIdx);
            MSG_FATAL("0pMe->m_wMainListSel===%d,pMe->m_wMainListIdx==%d",pMe->m_wMainListSel,pMe->m_wMainListIdx,0);
            // store the menu select
            PUSH_LISTMENU_SEL(pMe->m_wMainListSel);
            
            // store the menu index
            PUSH_LISTMENU_IDX(pMe->m_wMainListIdx);
            MSG_FATAL("1pMe->m_wMainListSel===%d,pMe->m_wMainListIdx==%d",pMe->m_wMainListSel,pMe->m_wMainListIdx,0);
            
            CLOSE_DIALOG(DLGRET_OK);
            return TRUE;
            
        case EVT_CTL_SEL_CHANGED:
            pMe->m_wEditCont = wParam;
            pMe->m_wSelectCont = wParam;
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
                pMe->m_wYPos=0;
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt;
						 pMe->m_wSelectCont = IMENUCTL_GetSel(pMenuCtl);
						rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,pMe->m_wSelectCont);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}
                else if(CONTAPP_PT_IN_RECT(wXPos,wYPos,pMe->m_ScrollBarrc))
                {
                    int totalNum=IVector_Size(pMe->m_pAddList);
                    if((wYPos-(int)pMe->m_ScrollBarrc.y)> 0)
                    {
                     int pagenum = (wYPos-pMe->m_ScrollBarrc.y)/(pMe->m_ScrollBarrc.dy/(totalNum/(MAX_NUM_MENUPOP-1) +1));
                     int pageitemp = ((pagenum*(MAX_NUM_MENUPOP-1)));
                     //pageitemp = pageitemp;
                     MSG_FATAL("pagenum-----=%d---pageitemp=%d",pagenum,pageitemp,0);
                     MSG_FATAL("---pMenuCtl=%d",IMENUCTL_GetItemID(pMenuCtl,pageitemp) ,0,0);
                     //IMENUCTL_SetSel(pMenuCtl,10); 
                     if(IMENUCTL_GetItemID(pMenuCtl, pageitemp) ==0)
                     {
                          pageitemp --;
                     }
                     IMENUCTL_SetSel(pMenuCtl, IMENUCTL_GetItemID(pMenuCtl, pageitemp)); 
                     MSG_FATAL("0-----=%x---pMenuCtl=%x=====2==%x",IMENUCTL_GetItemID(pMenuCtl,15),IMENUCTL_GetItemID(pMenuCtl,1),IMENUCTL_GetItemID(pMenuCtl,2));
                     MSG_FATAL("0-----=%x---pMenuCtl=%x=====2==%x",IMENUCTL_GetItemID(pMenuCtl,7),IMENUCTL_GetItemID(pMenuCtl,8),IMENUCTL_GetItemID(pMenuCtl,16));
                    }
                    else
                    {
                     IMENUCTL_SetSel(pMenuCtl,0); 
                    }
                     pMe->m_wYPos=wYPos;
                     (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
                }

			}
			break;
#endif                        
        default:
            break;
            
    }
    
    return FALSE;
} // CContApp_HandleListDlgEvent

/*==============================================================================
函数：
       CContApp_HandleInputFldDlgEvent
说明：
       IDD_INPUTFLD 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleInputFldDlgEvent( CContApp  *pMe,
                                                 AEEEvent   eCode,
                                                 uint16     wParam,
                                                 uint32     dwParam)
{
    ITextCtl *pTextCtl = NULL;
    int      i;
    boolean responseLS;
    AECHAR  at_email[2] = {'@', '\0'};
#ifdef FEATURE_LANG_ARABIC
    AECHAR EmptyTitle[2]={' ','\0'};
#endif

#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    MSG_FATAL("CContApp_HandleInputFldDlgEvent eCode=0x%x, wParam=0x%x", eCode,wParam,0);
    pTextCtl = (ITextCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_INPUTFLD_TEXTCTL);

    if(NULL == pTextCtl)
    {
        return TRUE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};

            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
            ITEXTCTL_SetRect(pTextCtl, &rc);
         
            ITEXTCTL_SetProperties( pTextCtl, 
                     CContApp_GetFldInputProp(pMe,pMe->m_nFldInputID)|TP_FOCUS_NOSEL);
                     
            (void)ITEXTCTL_SetInputMode(pTextCtl,
                     CContApp_GetFldInputMode(pMe->m_nFldInputID));
/*                     
#ifdef FEATURE_LANG_ARABIC  // 有鉴于部分外语文字过长，作为标题显示效果不好，干脆就都不显示，以免用户误会
            (void)ITEXTCTL_SetTitle( pTextCtl,
                                     NULL,
                                     NULL,
                                     EmptyTitle);
#else*/     
            if(pMe->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, FALSE);
                //IANNUNCIATOR_Redraw(pMe->m_pIAnn);
            }

            (void)ITEXTCTL_SetTitle( pTextCtl,
                                     CONTAPP_RES_FILE_LANG,
                     CContApp_GetFldTitleID(pMe->m_nFldInputID),
                                     NULL);
//#endif
             CContApp_SetFldMaxSize(pMe,pTextCtl,pMe->m_nFldInputID);
                     
            //if(pMe->m_nAddnewOrEdit == ADDOREDIT_EDIT)
            //{
                if(pMe->m_pFldInputBuf)
                {
                    ITEXTCTL_SetText(pTextCtl, pMe->m_pFldInputBuf, -1);
                    CContApp_SetFldMaxSize(pMe,pTextCtl,pMe->m_nFldInputID);
                    if(IS_NUM_FIELD(pMe->m_nFldInputID))
                    {  
                        (void)WSTRCPY(pMe->m_tmpBuf,pMe->m_pFldInputBuf);
                    }
                }
                else if(AEE_ADDRFIELD_EMAIL == pMe->m_nFldInputID)
                {
                    // always have a "@" for email
                    ITEXTCTL_SetText(pTextCtl, at_email, 2);
                    ITEXTCTL_SetCursorPos(pTextCtl, TC_CURSORSTART);
                }
            //}

            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_KEY_RELEASE:
        case EVT_USER_REDRAW:
  //bw:<< 071014 can't draw Battom when display Symbol dialog
            {
                AEETextInputMode nInputMode; 
                nInputMode = ITEXTCTL_GetInputMode(pTextCtl,NULL);
                if (nInputMode == AEE_TM_SYMBOLS)
                {
                    return TRUE;
                }
                {
                    AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pTextCtl);
                    if (NULL != pwstrText)
                    {
                        static int nLen = 0 ;
                        if ( nLen != WSTRLEN(pwstrText) )
                        {
                            nLen = WSTRLEN(pwstrText);
                            CContApp_SetFldMaxSize(pMe,pTextCtl,pMe->m_nFldInputID);
                        }
                    }
                }            
                
                if ( WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) > 0 && ITEXTCTL_GetT9End(pTextCtl) != TC_CURSORSTART )
                {
                    responseLS = TRUE;
                    #ifndef FEATURE_ALL_KEY_PAD   //add by yagndecai
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_DELETE);
                    #else
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_BACK);  
                    #endif
                }
                else

                {
                    if(AEE_ADDRFIELD_NAME == pMe->m_nFldInputID)
                    {
                        responseLS = FALSE;
                        CONTAPP_DRAW_BOTTOMBAR(BTBAR_BACK);          
                    }
                    else
                    {
                        CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_BACK);            
                    }
                }
                IDISPLAY_Update(pMe->m_pDisplay);  
            }
            return TRUE;
            
        case EVT_CTL_TEXT_MODECHANGED:  //切换输入法
            {
                CContApp_SetFldMaxSize(pMe,pTextCtl,pMe->m_nFldInputID);             
            }
            return TRUE; 
        
        case EVT_DIALOG_END:
            if(pMe->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
                IANNUNCIATOR_Redraw(pMe->m_pIAnn);    
            }
            if (pMe->m_bSuspending)
            {
                FREEIF(pMe->m_pFldInputBuf);
                pMe->m_pFldInputBuf = WSTRDUP(ITEXTCTL_GetTextPtr(pTextCtl));
            }
            return TRUE;

        case EVT_KEY_PRESS:
#ifdef FEATURE_GIVEUP_EDIT
            if(AVK_CONTACTS_GIVEUP_EDIT == wParam)
            {
                // 名字为空的时候不处理，用原来的名字
                if ( !(AEE_ADDRFIELD_NAME == pMe->m_nFldInputID && WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) == 0))
                {

                    FREEIF(pMe->m_pFldInputBuf);
                    
                    if(WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) > 0)
                    {
                        pMe->m_pFldInputBuf = WSTRDUP(ITEXTCTL_GetTextPtr(pTextCtl));
                        
                        // 邮件格式有问题直接不保存该field,提示保存成功
                       if(AEE_ADDRFIELD_EMAIL == pMe->m_nFldInputID)
                       {
                           int len;
                           int i;
                           boolean rightemail=FALSE;
                           
                           len =WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl));
                           for(i=0; i<len; i++)
                           {
                               if((AECHAR)'@' == pMe->m_pFldInputBuf[i])
                               {
                                   rightemail =  TRUE;
                                   break;
                               }
                           }
    
                           if(FALSE == rightemail)
                           {
                              FREEIF(pMe->m_pFldInputBuf);
                           }
                       }
                    }
                }

                CContApp_SetRecByFLDID(pMe, pMe->m_nFldInputID);
                CLOSE_DIALOG(DLGRET_GIVEUP_EDIT);
                return TRUE;
            }
#endif
            break;
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_INFO:
                case AVK_SELECT:
                    if ( WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) == 0 && AEE_ADDRFIELD_NAME == pMe->m_nFldInputID)
                    {
                        return TRUE;
                    }
                    FREEIF(pMe->m_pFldInputBuf);
                    
                    if(WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) > 0)
                    {
                        pMe->m_pFldInputBuf = WSTRDUP(ITEXTCTL_GetTextPtr(pTextCtl));
                      /*  if(pMe->m_nFldInputID == AEE_ADDRFIELD_NAME)
                        {
                            if(!IS_RUIM_REC(pMe->m_wSelectCont)||IS_ADDNEW_PHONE_REC(pMe->m_eCurState,pMe->m_nSaveType))
                            {
                                // Check the same record
                                if(CContApp_CheckPhoneName(pMe,
                                                           pMe->m_pFldInputBuf))
                                {
                                    CLOSE_DIALOG(DLGRET_SAME_REC);
                                    return TRUE;
                                }
                            }
                        }
                        else if(IS_NUM_FIELD(pMe->m_nFldInputID)) */
                        
                        if(AEE_ADDRFIELD_EMAIL == pMe->m_nFldInputID)
                        {
                            int len;
                            int i;
                            boolean rightemail=FALSE;
                            
                            len =WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl));
                            for(i=0; i<len; i++)
                            {
                                if((AECHAR)'@' == pMe->m_pFldInputBuf[i])
                                {
                                    rightemail =  TRUE;
                                    break;
                                }
                            }

                            if(FALSE == rightemail)
                            {
                               //FREEIF(pMe->m_pFldInputBuf);
                               CLOSE_DIALOG(DLGRET_EMAIL_UNAVAILD); 
                               return TRUE;
                            }
                        }
    
                        if(IS_NUM_FIELD(pMe->m_nFldInputID))
                        {
                            if(!CContApp_CheckNumber(pMe->m_pFldInputBuf))
                            {
                                CLOSE_DIALOG(DLGRET_NUM_UNAVAILD);
                                return TRUE;
                            }
                            if(pMe->m_tmpBuf[0] != NULL)
                            {
                                // 检查当前单键拨号中是否存在当前被编辑的记录
                                for(i=CONTCFG_ONEDIAL2; i<=CONTCFG_ONEDIAL8; i++)//CONTCFG_ONEDIAL1
                                {
                                    if(SUCCESS != CContApp_GetConfig( pMe,
                                                              (ContAppCFG)i,
                                                              pMe->m_pOneDialBuf,
                                                              sizeof(pMe->m_pOneDialBuf)))
                                    {
                                        MSG_FATAL("Get Config Failed~",0,0,0);
                                        continue;
                                    }

                                    // 存在单键拨号则需要修改单键拨号
                                    if(WSTRCMP(pMe->m_tmpBuf, pMe->m_pOneDialBuf) == 0)
                                    {
                                        ERR("==========",0,0,0);										
                                        (void)CContApp_SetConfig( pMe,
                                                      (ContAppCFG)i,
                                                      pMe->m_pFldInputBuf,
                                                      sizeof(pMe->m_pFldInputBuf));
                                    }
                                }
                            }
                        }
                        
                        //CLOSE_DIALOG(DLGRET_OK);
                    }
                    /*else
                    {
                        //FREEIF(pMe->m_pFldInputBuf);
                        //CLOSE_DIALOG(DLGRET_INPUT_NULL);
                        CLOSE_DIALOG(DLGRET_OK);
                    }*/
                    
                    CLOSE_DIALOG(DLGRET_OK);
                    return TRUE;
                
                case AVK_CLR:
                    //FREEIF(pMe->m_pFldInputBuf);
                    //WSTRCPY(pMe->m_pFldInputBuf, pMe->m_tmpBuf);
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                    
                default:
                    break;
                    
            }
            break;
            
        case EVT_COMMAND:
            break;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif             
        default:
            break;
            
    }

    return FALSE;
} // CContApp_HandleInputFldDlgEvent

/*==============================================================================
函数：
       CContApp_HandleAddNewDlgEvent
说明：
       IDD_ADDNEW 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleAddNewDlgEvent( CContApp  *pMe,
                                               AEEEvent   eCode,
                                               uint16     wParam,
                                               uint32     dwParam)
{
    IMenuCtl *pMenuCtl = NULL;
    ITextCtl *pTextCtl = NULL;
    IMenuCtl *pGroupList = NULL;
    uint32 dwMask;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    MSG_FATAL("CContApp_HandleAddNewDlgEvent eCode=%X, wParam=%d",eCode,wParam,0);
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg, IDC_ADDNEW_MENU);
    
    pTextCtl = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_ADD_LOCAL_INPUT);
    pGroupList = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_ADD_GROUPLIST);
    MSG_FATAL("CContApp_HandleAddNewDlgEvent...000000",0,0,0);
    if(NULL == pMenuCtl || NULL == pTextCtl || NULL == pGroupList)
    {
        return TRUE;
    }
    MSG_FATAL("CContApp_HandleAddNewDlgEvent...0000001",0,0,0);
    //MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);
    MSG_FATAL("CContApp_HandleAddNewDlgEvent...0000111",0,0,0);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            dwMask = IDIALOG_GetProperties(pMe->m_pActiveDlg);
            dwMask |= DLG_NOT_SET_FOCUS_AUTO;
            IDIALOG_SetProperties(pMe->m_pActiveDlg, dwMask);
            pMe->m_nInputMode =  EDIT_MENU_MODE; 
			MSG_FATAL("CContApp_HandleAddNewDlgEvent...EVT_DIALOG_INIT",0,0,0);
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect rc={0};
            //AEERingerInfo  ri;
            AEEDeviceInfo devinfo;
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
			MSG_FATAL("CContApp_HandleAddNewDlgEvent...EVT_DIALOG_START",0,0,0);
            IMENUCTL_SetRect(pMenuCtl, &rc);
			#if 0
            (void)IMENUCTL_SetTitle( pMenuCtl,CONTAPP_RES_FILE_LANG,IDS_ONEDIAL_SET,NULL);
			#else
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
						CONTAPP_RES_FILE_LANG,								  
						IDS_ONEDIAL_SET,
						WTitle,
						sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
				    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
			}
			#endif
            
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif

            // build menu
            CContApp_BuildAddMenuMenu(pMe, pMenuCtl);
            //if(pMe->m_ePreState == STATE_ADDNEW_INPUT || (TRUE == pMe->m_bInputNotComplete) )
            {
                // Update the menu item text
                if (pMe->m_pAddNewName ) // && WSTRLEN(pMe->m_pAddNewName) != 0)
                {
                    IMENUCTL_SetItemText(pMenuCtl, IDI_ADDNEW_MENU_NAME, NULL, 0, pMe->m_pAddNewName);
                }
                else// if(pMe->m_wSelectEdit != IDI_ADDNEW_MENU_NAME)
                {
                    IMENUCTL_SetItemText(pMenuCtl, IDI_ADDNEW_MENU_NAME, CONTAPP_RES_FILE_LANG, IDS_NAME, NULL);
                }
                
                if (pMe->m_pAddNewMobile)
                {
                    IMENUCTL_SetItemText(pMenuCtl, IDI_ADDNEW_MENU_MOBILE, NULL, 0, pMe->m_pAddNewMobile);
                }
                else
                {
                    IMENUCTL_SetItemText(pMenuCtl, IDI_ADDNEW_MENU_MOBILE, CONTAPP_RES_FILE_LANG, IDS_MOBILE, NULL);
                }
                
                if(!IsRunAsUIMVersion() || pMe->m_nSaveType == CONTCFG_SAVETYPE_PHONE)
                {  
                    //AECHAR *m_pString = NULL;
                    //int        len = 0;
                    //uint16 ResID = 0;
                    //byte *ringToneId  =NULL;
                    
                    if (pMe->m_pAddNewHome)
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_ADDNEW_MENU_HOME, NULL, 0, pMe->m_pAddNewHome);
                    }
                    else
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_ADDNEW_MENU_HOME, CONTAPP_RES_FILE_LANG, IDS_HOME, NULL);
                    }
                    
                    if (pMe->m_pAddNewOffice)
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_ADDNEW_MENU_OFFICE, NULL, 0, pMe->m_pAddNewOffice);
                    }
                    else
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_ADDNEW_MENU_OFFICE, CONTAPP_RES_FILE_LANG, IDS_OFFICE, NULL);
                    }
                    
                    if (pMe->m_pAddNewFax)
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_ADDNEW_MENU_FAX, NULL, 0, pMe->m_pAddNewFax);
                    }
                    else
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_ADDNEW_MENU_FAX, CONTAPP_RES_FILE_LANG, IDS_FAX, NULL);
                    }
                    
                    if (pMe->m_pAddNewEMAIL)
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_ADDNEW_MENU_EMAIL, NULL, 0, pMe->m_pAddNewEMAIL);
                    }
                    else if(pMe->m_wSelectEdit != IDI_ADDNEW_MENU_EMAIL)
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_ADDNEW_MENU_EMAIL, CONTAPP_RES_FILE_LANG, IDS_EMAIL, NULL);
                    }
                    
                    if (pMe->m_pAddNewAddress)
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_ADDNEW_MENU_ADDRESS, NULL, 0, pMe->m_pAddNewAddress);
                    }
                    else if(pMe->m_wSelectEdit != IDI_ADDNEW_MENU_ADDRESS)
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_ADDNEW_MENU_ADDRESS, CONTAPP_RES_FILE_LANG, IDS_ADDRESS, NULL);
                    }
                    
                    if (pMe->m_pAddNewURL)
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_ADDNEW_MENU_URL, NULL, 0, pMe->m_pAddNewURL);
                    }
                    else if(pMe->m_wSelectEdit != IDI_ADDNEW_MENU_URL)
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_ADDNEW_MENU_URL, CONTAPP_RES_FILE_LANG, IDS_URL, NULL);
                    }
                    
                    if (pMe->m_pAddNewRemark)
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_ADDNEW_MENU_REMARK, NULL, 0, pMe->m_pAddNewRemark);
                    } 
                    else if(pMe->m_wSelectEdit != IDI_ADDNEW_MENU_REMARK)
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_ADDNEW_MENU_REMARK, CONTAPP_RES_FILE_LANG, IDS_NOTES, NULL);
                    }
                    
                    CContApp_SetGroupItemText(pMe, pMenuCtl);
					#ifndef FEATURE_VERSION_K212
                    if (pMe->m_nRingToneID && pMe->m_nRingToneID[0] != (AECHAR)'\0')
                    {
                        AECHAR name[128]={0};
						char nametemp[128] = {0};
                        AECHAR *fileName=NULL;

                        fileName = WSTRRCHR(pMe->m_nRingToneID, (AECHAR)'/');
                        if(NULL != fileName)
                        {
                            fileName++;
                            WSTRCPY(name, fileName);
                        }
                        else
                        {
                            WSTRCPY(name, pMe->m_nRingToneID);
                        }
						WSTRTOSTR(name,nametemp,128);
						DBGPRINTF("nametemp====%s",nametemp);
						DBGPRINTF("name====%s",name);
						MEMSET(name,0,128);
						UTF8TOWSTR((byte *)nametemp,STRLEN(nametemp),name,128);
						DBGPRINTF("name====%s",name);
                        IMENUCTL_SetItemText(pMenuCtl, IDI_ADDNEW_MENU_SELECTRINGTONE, NULL, 0, name);
                        
                    } 
                    else
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_ADDNEW_MENU_SELECTRINGTONE, CONTAPP_RES_FILE_LANG, IDS_RING, NULL);
                    }
					#endif
                }
            }

            IMENUCTL_Redraw(pMenuCtl);
            CContApp_SetInputMode(pMe);
            IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectEdit);    
						
			CContApp_ShowEditItem(pMe, pMenuCtl, pTextCtl);
			IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
			
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;

        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
        {
             // Restore the menu select from suspend
             MSG_FATAL("EVT_USER_REDRAW....................",0,0,0);
           if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            else
            {
                // Set menu select
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectEdit);
            }
            #ifdef FEATURE_VERSION_SKY
            {
            IImage *image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);
            Appscommon_ResetBackground(pMe->m_pDisplay,image, APPSCOMMON_BG_COLOR, 
                                                    &pMe->m_rc, 0, 0);
             if( image != NULL)
             {
               IIMAGE_Draw( image, 0, 0);
               IIMAGE_Release( image);
             }
            }
            #endif
            (void)IMENUCTL_Redraw(pMenuCtl);
            // Draw prompt bar here
            if( IDI_ADDNEW_MENU_SELECTRINGTONE == IMENUCTL_GetSel(pMenuCtl))
            {
                CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_OK_BACK);
            }
            else
            {
                //CONTAPP_DRAW_BOTTOMBAR(BTBAR_SAVE_BACK);
               #ifndef FEATURE_ALL_KEY_PAD
               if(WSTRLEN( ITEXTCTL_GetTextPtr(pTextCtl)) > 0)
               {
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_SAVE_DELETE);
               }
               else
               #endif
               {
                   CONTAPP_DRAW_BOTTOMBAR(BTBAR_SAVE_BACK);
               }
            }

            if(IDI_ADDNEW_MENU_NAME == pMe->m_wSelectEdit)
            {
                pMe->m_nFldInputID = AEE_ADDRFIELD_NAME;				
                CContApp_ShowEditItem(pMe, pMenuCtl, pTextCtl);
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            }
            else if(IDI_ADDNEW_MENU_SELECTRINGTONE == pMe->m_wSelectEdit)
            {
                IMENUCTL_SetActive(pMenuCtl, TRUE);
            }
            else
            {
                ITEXTCTL_SetActive(pTextCtl, TRUE);
            }
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
        }
        
        case EVT_DIALOG_END:
        {
             // Store the menu select if applet is suspended.
			  MSG_FATAL("EVT_DIALOG_END............1........",0,0,0);
            if(pMe->m_bSuspending)
            {
                // 中断处理
                if(EDIT_GROUP == pMe->m_nInputMode)
                {
                    CContApp_SetEditGroup(pMe, pMenuCtl, FALSE, IMENUCTL_GetSel(pGroupList));
                }
                else if( LOCAL_NUMBER_INPUT == pMe->m_nInputMode )
                {
                    CContApp_SaveLocal_Input(pMe, pTextCtl);
                }

                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            pMe->m_bInputNotComplete = FALSE;
			 MSG_FATAL("EVT_DIALOG_END..........2..........",0,0,0);
#ifdef FEATURE_LCD_TOUCH_ENABLE
            TSIM_NumberKeypad(FALSE);
#endif
			 MSG_FATAL("EVT_DIALOG_END..........3..........",0,0,0);
        }
        return TRUE;
        
        /*下面有对4个数字域 m_nFldInputID 的邋付值， 其他的文字域在按任意键邋(1 - #)的时候编辑*/
        case EVT_CTL_SEL_CHANGED :
            MSG_FATAL("EVT_CTL_SEL_CHANGED---0",0,0,0);
            if(EDIT_GROUP == pMe->m_nInputMode)
            {
                return TRUE;
            }
            MSG_FATAL("EVT_CTL_SEL_CHANGED----1----wParam=%d",wParam,0,0);
            pMe->m_wSelectEdit = wParam;
            CContApp_SetInputMode(pMe);
            switch(wParam)
            {
                case IDI_ADDNEW_MENU_NAME:
                     // Set EDIT field
                    pMe->m_nFldInputID = AEE_ADDRFIELD_NAME;
                    break;
                    
                case IDI_ADDNEW_MENU_MOBILE:
                     // Set EDIT field
                    pMe->m_nFldInputID = AEE_ADDRFIELD_PHONE_GENERIC;
                    break;
                    
                case IDI_ADDNEW_MENU_HOME:
                    // Set EDIT field
                    pMe->m_nFldInputID = AEE_ADDRFIELD_PHONE_HOME;
                    break;
                    
                case IDI_ADDNEW_MENU_OFFICE:
                    // Set EDIT field
                    pMe->m_nFldInputID = AEE_ADDRFIELD_PHONE_WORK;
                    break;
                    
                case IDI_ADDNEW_MENU_FAX:
                    // Set EDIT field
                    pMe->m_nFldInputID = AEE_ADDRFIELD_PHONE_FAX;
                    break;

                case IDI_ADDNEW_MENU_EMAIL:
                     // Set EDIT field
                    pMe->m_nFldInputID = AEE_ADDRFIELD_EMAIL;
                    break;
                    
                case IDI_ADDNEW_MENU_ADDRESS:
                    // Set EDIT field
                    pMe->m_nFldInputID = AEE_ADDRFIELD_ADDRESS;
                    break;
                    
                case IDI_ADDNEW_MENU_URL:
                    // Set EDIT field
                    pMe->m_nFldInputID = AEE_ADDRFIELD_URL;
                    break;
                    
                case IDI_ADDNEW_MENU_REMARK:
                    // Set EDIT field
                    pMe->m_nFldInputID = AEE_ADDRFIELD_NOTES;
                    break;
                    
                case IDI_ADDNEW_MENU_SELECTGROUP:
                    pMe->m_nInputMode = EDIT_GROUP;
                    CContApp_BuildGroupList(pMe, pGroupList);
                    CContApp_ShowGroupSelect(pMe, pMenuCtl, pGroupList);
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_SAVE_BACK);
                    break;

                case IDI_ADDNEW_MENU_SELECTRINGTONE:
                    pMe->m_nInputMode = EDIT_MENU_MODE;
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_OK_BACK);
                    IDISPLAY_Update(pMe->m_pDisplay);  
                    break;
                    
                default:
                    pMe->m_nInputMode = EDIT_MENU_MODE;
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_SAVE_BACK);
                    return TRUE;
            }

            if(wParam !=  IDI_ADDNEW_MENU_SELECTGROUP && wParam != IDI_ADDNEW_MENU_SELECTRINGTONE)
            {            	
                CContApp_ShowEditItem(pMe, pMenuCtl, pTextCtl);
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            }

            return TRUE;

        case EVT_KEY_PRESS:
            switch(wParam)
            {
                case AVK_UP:
                case AVK_DOWN:
                {
                    MSG_FATAL("m_nInputMode=%d",pMe->m_nInputMode,0,0);

					//Add By zzg 2011_12_15
					if ((LOCAL_NUMBER_INPUT == pMe->m_nInputMode) 
						|| (EDIT_MENU_MODE == pMe->m_nInputMode)
						|| (EDIT_GROUP == pMe->m_nInputMode))
					{
	                	if (CContApp_CheckNumberIsInValid(pMe, pTextCtl))
	                	{
	                		 pMe->m_bNumberInvalid = TRUE;		
	                		 CLOSE_DIALOG(DLGRET_NUM_UNAVAILD);
							 return TRUE;
						}
						else
						{
							DBGPRINTF("***zzg CContApp_FixPlusNumber 2***");
							CContApp_FixPlusNumber(pMe);	
						}
					}
                	//Add End
                	
                    if(EDIT_GROUP == pMe->m_nInputMode)
                    {                    
                        CContApp_SetEditGroup(pMe, pMenuCtl, FALSE, IMENUCTL_GetSel(pGroupList));
                        IMENUCTL_SetActive(pGroupList, FALSE);
                        IMENUCTL_SetActive(pMenuCtl, TRUE);
                    }
                    if(OPT_TEXT_INPUT == pMe->m_nInputMode || LOCAL_NUMBER_INPUT == pMe->m_nInputMode)
                    {
                        if(TRUE == CContApp_SaveLocal_Input(pMe, pTextCtl))
                        {
                            MSG_FATAL("CContApp_SaveLocal_Input=TRUE",0,0,0);
                           // itemTextBuf = pMe->m_pFldInputBuf;
                            if (AEE_ADDRFIELD_NONE != pMe->m_nFldInputID)
                            {
                                CContApp_SetRecByFLDID(pMe, pMe->m_nFldInputID);
                            } 
                            
                            CContApp_SetMenuItemText(pMe, pMenuCtl, pMe->m_wSelectEdit);
                        }
                        else
                        {   
                            MSG_FATAL("CContApp_SaveLocal_Input=FALSE",0,0,0);
                            return TRUE;
                        }
                        
                        ITEXTCTL_SetActive(pTextCtl, FALSE);
                        IMENUCTL_SetActive(pMenuCtl, TRUE);
						//IMENUCTL_Redraw(pMenuCtl);
                        IDIALOG_SetFocus(pMe->m_pActiveDlg, IDC_ADDNEW_MENU);
                    }
                    ITEXTCTL_SetActive(pTextCtl, FALSE);
                }
                return TRUE;
                
                case AVK_0:
                case AVK_1:
                case AVK_2:
                case AVK_3:
                case AVK_4:
                case AVK_5:
                case AVK_6:
                case AVK_7:
                case AVK_8:
                case AVK_9:
                case AVK_T:
                case AVK_Q:
                case AVK_W:
                case AVK_E:
                case AVK_R:
                case AVK_A:
                case AVK_S:
                case AVK_D:
                case AVK_F:
                case AVK_Z:
                case AVK_X:
                case AVK_C:
                case AVK_Y:
                case AVK_U:
                case AVK_I:
                case AVK_O:
                case AVK_P:
                case AVK_G:
                case AVK_H:
                case AVK_J:
                case AVK_K:
                case AVK_L:
                case AVK_V:
                case AVK_B:
                case AVK_N:
                case AVK_M:
                case AVK_STAR:
                case AVK_POUND:
                {
                    if(OPT_TEXT_INPUT != pMe->m_nInputMode)
                    {
                        return TRUE;
                    }
                    
                    CContApp_SaveLocal_Input(pMe, pTextCtl);// 本地是可以先删除一些字符后，在按任意键进去编辑的
                    CLOSE_DIALOG(DLGRET_EDIT);
                }
                return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                {   
                    FREEIF(pMe->m_nRingToneID);
                    pMe->m_nGroupCat = AEE_ADDR_CAT_OTHER;
                    pMe->m_nFldInputID = AEE_ADDRFIELD_NONE;
                    pMe->m_wSelectEdit = IDI_ADDNEW_MENU_NAME;
                    
                    CLOSE_DIALOG(DLGRET_CANCELED);
                }
                return TRUE;
                
                case AVK_INFO:
                case AVK_SELECT:
                {
					MSG_FATAL("***zzg ContApp AddNewDlg m_nInputMode=%x***", pMe->m_nInputMode, 0, 0);

					//Add By zzg 2011_12_15
					if ((LOCAL_NUMBER_INPUT == pMe->m_nInputMode) 
						|| (EDIT_MENU_MODE == pMe->m_nInputMode)
						|| (EDIT_GROUP == pMe->m_nInputMode))
					{
	                	if (CContApp_CheckNumberIsInValid(pMe, pTextCtl))
	                	{
	                		 pMe->m_bNumberInvalid = TRUE;		
	                		 CLOSE_DIALOG(DLGRET_NUM_UNAVAILD);
							 return TRUE;
						}
						else
						{
							DBGPRINTF("***zzg CContApp_FixPlusNumber 3***");
							CContApp_FixPlusNumber(pMe);	
						}
					}
                	//Add End
					
                    if( LOCAL_NUMBER_INPUT == pMe->m_nInputMode || OPT_TEXT_INPUT == pMe->m_nInputMode)
                    {                    	
                        if(TRUE != CContApp_SaveLocal_Input(pMe, pTextCtl))
                        {
                            return TRUE;
                        }
                        
                        if (AEE_ADDRFIELD_NONE != pMe->m_nFldInputID)
                        {
                            CContApp_SetRecByFLDID(pMe, pMe->m_nFldInputID);
                        } 
                    }
                    else if( EDIT_GROUP == pMe->m_nInputMode)
                    {
                        // 不需要返回菜单，直接弹出DONE， 传第三个参数TRUE
                        CContApp_SetEditGroup(pMe, pMenuCtl, TRUE, IMENUCTL_GetSel(pGroupList));
                    }
                    MSG_FATAL("CLOSE_DIALOG(DLGRET_OK);",0,0,0);
                    CLOSE_DIALOG(DLGRET_OK);
                }
                return TRUE;

                default: 
                    break;
            }

            return TRUE;
            
        case EVT_KEY_RELEASE:
        {
           if(pMe->m_nInputMode == LOCAL_NUMBER_INPUT || pMe->m_nInputMode == OPT_TEXT_INPUT )
           {
           	   #ifndef FEATURE_ALL_KEY_PAD
               if ( WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) > 0 && ITEXTCTL_GetT9End(pTextCtl) != TC_CURSORSTART )
               {
                   CONTAPP_DRAW_BOTTOMBAR(BTBAR_SAVE_DELETE);
               }
               else
               #endif
               {
                   CONTAPP_DRAW_BOTTOMBAR(BTBAR_SAVE_BACK);            
               }   
               IDISPLAY_Update(pMe->m_pDisplay);  
           }
        }
        return TRUE;

        case EVT_COMMAND:
        {
            pMe->m_wSelectEdit = wParam;

            switch(wParam)
            {
                case IDS_GROUP_NONE_STRING:
                case IDS_GROUP_VIP:
                case IDS_GROUP_HOME:
                case IDS_GROUP_WORK:
                case IDS_GROUP_FRIEND:
                case IDS_GROUP_PERSONAL:
                case IDS_GROUP_BUSINESS:
                {
                    MSG_FATAL("CContApp_HandleAddNewDlgEvent wParam=%d",wParam,0,0);
                    if( EDIT_GROUP == pMe->m_nInputMode)
                    {
                        CContApp_SetEditGroup(pMe, pMenuCtl, TRUE, wParam);
                        CLOSE_DIALOG(DLGRET_OK);
                        return TRUE;
                    }
                }
                break;

                case IDI_ADDNEW_MENU_SELECTRINGTONE:
                {
                    ISoundMenu  *pSoundMenu = NULL;
                    pMe->m_nFldInputID = AEE_ADDRFIELD_RINGTONE;
                    
                    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_APP_SOUNDMENU, (void **)&pSoundMenu))
                    {
                        return FALSE;
                    }
                    if(pMe->m_nRingToneID == NULL)
                    {
                        pMe->m_nRingToneID = (AECHAR*)MALLOC(MAX_FILE_NAME * sizeof(AECHAR));
                    }
                    if(pMe->m_nRingToneID)
                    {
                        ISoundMenu_Ringer_List(pSoundMenu, (void*)pMe->m_nRingToneID);
                    }
                    if(pSoundMenu)
                    {
                        ISoundMenu_Release(pSoundMenu);
                        pSoundMenu = NULL;
                    }
                    return TRUE;
                 }

                default: 
                    MSG_FATAL("CLOSE_DIALOG(DLGRET_OK);",0,0,0);
                    break;
                    //CLOSE_DIALOG(DLGRET_OK);
                    //return TRUE;
            }

        }
        return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
        case EVT_PEN_DOWN:
            {
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);
                if(ITEXTCTL_IsActive(pTextCtl))
				{
				   ITEXTCTL_GetRect(pTextCtl,&rc); 
                   if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
			        {
                     if(OPT_TEXT_INPUT != pMe->m_nInputMode)
                     {
                         TSIM_NumberKeypad(TRUE);
                         return TRUE;
                     }                    
                     CContApp_SaveLocal_Input(pMe, pTextCtl);// 本地是可以先删除一些字符后，在按任意键进去编辑的
                     CLOSE_DIALOG(DLGRET_EDIT);
				    }         
                }
            	
            }
            break;
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);
				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt= ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						boolean rt;
						int len = WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl));

						if((ITEXTCTL_IsActive(pTextCtl)) && (len > 0))
							rt = ITEXTCTL_HandleEvent(pTextCtl,EVT_KEY,AVK_CLR,0);
						else
							rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}
                #if 1
				else
				{

					if(ITEXTCTL_IsActive(pTextCtl))
					{
							ITEXTCTL_GetRect(pTextCtl,&rc); 
                            MSG_FATAL("CONTAPP_PT_IN_RECT------0",0,0,0);
							if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
							{
                                ;
							}
							else
							{
								/*if((!IMENUCTL_IsActive(pMenuCtl)))
									IMENUCTL_SetActive(pMenuCtl, TRUE);
                                //ITEXTCTL_HandleEvent(pTextCtl,EVT_KEY,AVK_UP,0);
                                MSG_FATAL("pMenuCtleCode=%x---wParam%x---dwParam%x",eCode,wParam,dwParam);
								IMENUCTL_HandleEvent(pMenuCtl,eCode,wParam,dwParam);  */
                                #if 1
                                 if(EDIT_GROUP == pMe->m_nInputMode)
                                    {                    
                                        CContApp_SetEditGroup(pMe, pMenuCtl, FALSE, IMENUCTL_GetSel(pGroupList));
                                        IMENUCTL_SetActive(pGroupList, FALSE);
                                        IMENUCTL_SetActive(pMenuCtl, TRUE);
                                    }
                                    if(OPT_TEXT_INPUT == pMe->m_nInputMode || LOCAL_NUMBER_INPUT == pMe->m_nInputMode)
                                    {
                                        if(TRUE == CContApp_SaveLocal_Input(pMe, pTextCtl))
                                        {
                                            MSG_FATAL("CContApp_SaveLocal_Input=TRUE",0,0,0);
                                           // itemTextBuf = pMe->m_pFldInputBuf;
                                            if (AEE_ADDRFIELD_NONE != pMe->m_nFldInputID)
                                            {
                                                CContApp_SetRecByFLDID(pMe, pMe->m_nFldInputID);
                                            } 
                                            
                                            CContApp_SetMenuItemText(pMe, pMenuCtl, pMe->m_wSelectEdit);
                                        }
                                        else
                                        {   
                                            MSG_FATAL("CContApp_SaveLocal_Input=FALSE",0,0,0);
                                            return TRUE;
                                        }
                                        
                                        ITEXTCTL_SetActive(pTextCtl, FALSE);
                                        IMENUCTL_SetActive(pMenuCtl, TRUE);
                                        IDIALOG_SetFocus(pMe->m_pActiveDlg, IDC_ADDNEW_MENU);
                                    }
                                    ITEXTCTL_SetActive(pTextCtl, FALSE);
                                #endif
							}

					}
					else if(IMENUCTL_IsActive(pGroupList))
					{
							IMENUCTL_GetRect(pGroupList,&rc); 
							if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
							{
								;
							}
							else
							{
                                if(CONTAPP_PT_IN_RECT(wXPos,wYPos,pMe->M_RGroupRect))
                                {
                                    IMENUCTL_HandleEvent(pGroupList,EVT_KEY,AVK_RIGHT,0); 
                                }
                                else if(CONTAPP_PT_IN_RECT(wXPos,wYPos,pMe->M_LGroupRect))
                                {
                                    IMENUCTL_HandleEvent(pGroupList,EVT_KEY,AVK_LEFT,0); 
                                }
                                else
                                {                                
                              #if 1
                                 if(EDIT_GROUP == pMe->m_nInputMode)
                                    {                    
                                        CContApp_SetEditGroup(pMe, pMenuCtl, FALSE, IMENUCTL_GetSel(pGroupList));
                                        IMENUCTL_SetActive(pGroupList, FALSE);
                                        IMENUCTL_SetActive(pMenuCtl, TRUE);
                                    }
                                    if(OPT_TEXT_INPUT == pMe->m_nInputMode || LOCAL_NUMBER_INPUT == pMe->m_nInputMode)
                                    {
                                        if(TRUE == CContApp_SaveLocal_Input(pMe, pTextCtl))
                                        {
                                            MSG_FATAL("CContApp_SaveLocal_Input=TRUE",0,0,0);
                                           // itemTextBuf = pMe->m_pFldInputBuf;
                                            if (AEE_ADDRFIELD_NONE != pMe->m_nFldInputID)
                                            {
                                                CContApp_SetRecByFLDID(pMe, pMe->m_nFldInputID);
                                            } 
                                            
                                            CContApp_SetMenuItemText(pMe, pMenuCtl, pMe->m_wSelectEdit);
                                        }
                                        else
                                        {   
                                            MSG_FATAL("CContApp_SaveLocal_Input=FALSE",0,0,0);
                                            return TRUE;
                                        }
                                        
                                        ITEXTCTL_SetActive(pTextCtl, FALSE);
                                        IMENUCTL_SetActive(pMenuCtl, TRUE);
                                        IDIALOG_SetFocus(pMe->m_pActiveDlg, IDC_ADDNEW_MENU);
                                    }
                                    IMENUCTL_SetActive(pGroupList, FALSE);
                               #endif
                                }
							}

					}
					else
					{
						/* if((!IMENUCTL_IsActive(pMenuCtl)))
							IMENUCTL_SetActive(pMenuCtl, TRUE);
                        MSG_FATAL("pMenuCtleCode=%x---wParam%x---dwParam%x",eCode,wParam,dwParam);

						IMENUCTL_HandleEvent(pMenuCtl,eCode,wParam,dwParam);
						if((!IMENUCTL_IsActive(pGroupList)))
							IMENUCTL_SetActive(pGroupList, TRUE);
                        MSG_FATAL("pMenuCtleCode=%x---wParam%x---dwParam%x",eCode,wParam,dwParam);

						IMENUCTL_HandleEvent(pGroupList,eCode,wParam,dwParam); */
						
#if 1
                         if(EDIT_GROUP == pMe->m_nInputMode)
                            {                    
                                CContApp_SetEditGroup(pMe, pMenuCtl, FALSE, IMENUCTL_GetSel(pGroupList));
                                IMENUCTL_SetActive(pGroupList, FALSE);
                                IMENUCTL_SetActive(pMenuCtl, TRUE);
                            }
                            if(OPT_TEXT_INPUT == pMe->m_nInputMode || LOCAL_NUMBER_INPUT == pMe->m_nInputMode)
                            {
                                if(TRUE == CContApp_SaveLocal_Input(pMe, pTextCtl))
                                {
                                    MSG_FATAL("CContApp_SaveLocal_Input=TRUE",0,0,0);
                                   // itemTextBuf = pMe->m_pFldInputBuf;
                                    if (AEE_ADDRFIELD_NONE != pMe->m_nFldInputID)
                                    {
                                        CContApp_SetRecByFLDID(pMe, pMe->m_nFldInputID);
                                    } 
                                    
                                    CContApp_SetMenuItemText(pMe, pMenuCtl, pMe->m_wSelectEdit);
                                }
                                else
                                {   
                                    MSG_FATAL("CContApp_SaveLocal_Input=FALSE",0,0,0);
                                    return TRUE;
                                }
                                
                                ITEXTCTL_SetActive(pTextCtl, FALSE);
                                IMENUCTL_SetActive(pMenuCtl, TRUE);
                                IDIALOG_SetFocus(pMe->m_pActiveDlg, IDC_ADDNEW_MENU);
                            }
                            ITEXTCTL_SetActive(pTextCtl, FALSE);
                            IMENUCTL_SetActive(pGroupList, FALSE);
#endif
					}
                    IMENUCTL_HandleEvent(pMenuCtl,eCode,wParam,dwParam); 
				}
                #endif

			}
			break;
#endif             
        default:
            break;
            
    }
    
    return FALSE;
} // CContApp_HandleAddNewDlgEvent

/*==============================================================================
函数：
       CContApp_HandleOptsDlgEvent
说明：
       IDD_OPTS 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleOptsDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam)
{
    IMenuCtl  *pMenuCtl;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_OPTS_MENU);

	MSG_FATAL("***zzg CContApp_HandleOptsDlgEvent eCode=%x, wParam=%x***", eCode, wParam, 0);
    
    MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
            MSG_FATAL("rc.y=%d, rc.dx=%d, rc.dy=%d", rc.y, rc.dx, rc.dy);
            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_START",0,0,0);
            IMENUCTL_SetPopMenuRect(pMenuCtl); 

            // Draw prompt bar here
            //CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
            IMENUCTL_SetBottomBarType(pMenuCtl, BTBAR_SELECT_BACK);
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;                        
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:

            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            else
            {
                // Set menu select
                IMENUCTL_SetSel(pMenuCtl, IMENUCTL_GetItemID(pMenuCtl, 0));
            }
            
            // Draw prompt bar here
            //CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                    
                default:
                    break;
                    
            }
            break;
            
        case EVT_COMMAND:
            pMe->m_wSelectOpts = wParam;
            switch (wParam)
            {
                case IDI_OPTS_MENU_VIEW:
                     // Reset select field
                     CLOSE_DIALOG(DLGRET_DETAIL);
                     //CLOSE_DIALOG(DLGRET_VIEW);
                     return TRUE;

                case IDI_OPTS_MENU_EDIT:
                    pMe->m_nAddnewOrEdit = ADDOREDIT_EDIT;
                    pMe->m_wSelectEdit = IDI_EDIT_MENU_NAME;
                    CLOSE_DIALOG(DLGRET_EDIT);
                    return TRUE;
                    
                case IDI_OPTS_MENU_CALL:
                    
                    // Load the current fields
                    if(SUCCESS != CContApp_LoadAddrFlds( pMe, 
                                                         pMe->m_wSelectCont,
                                                         SINGLE_SELECT_NUMBER))
                    {
                        pMe->m_wErrStrID = IDS_ERR_LOADFIELDS;
                        CLOSE_DIALOG(DLGRET_ERR);
                        return NFSMACTION_CONTINUE;
                    }
                    
                    // 如果当前记录仅有一个号码则直接发起呼叫
                    if(1 == CContApp_GetCurrFldNum(pMe, AEE_ADDRFIELD_PHONE_GENERIC))
                    {
                        (void)CContApp_MakeCall(pMe, IDX_NUM_RUIM_FLD);
                    }
                    else
                    {
                        // Reset select field
                        //pMe->m_eOptsRetState = STATE_OPTS;
                        pMe->m_wFiledOptState = FIELDCALL;
                        CLOSE_DIALOG(DLGRET_VIEW);
                    }                    
                    return TRUE;

                case IDI_OPTS_MENU_ADDNEW:
                {
                    pMe->m_nGroupCat = AEE_ADDR_CAT_OTHER;
                    pMe->m_nAddnewOrEdit = ADDOREDIT_ADD;
                    pMe->m_wSelectEdit = IDI_ADDNEW_MENU_NAME;
                    //CLOSE_DIALOG(DLGRET_ADDNEW);
                    CLOSE_DIALOG(DLGRET_ADDNEW);
                    return TRUE;
                }

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
                case IDI_OPTS_MENU_SENDSMS:
                    // Load the current fields
                    if(SUCCESS != CContApp_LoadAddrFlds( pMe, 
                                                         pMe->m_wSelectCont,
                                                         SINGLE_SELECT_NUMBER))
                    {
                        pMe->m_wErrStrID = IDS_ERR_LOADFIELDS;
                        CLOSE_DIALOG(DLGRET_ERR);
                        return NFSMACTION_CONTINUE;
                    }
                    
                    // 如果当前记录仅有一个号码则直接发短信
                    if(1 == CContApp_GetCurrFldNum(pMe, AEE_ADDRFIELD_PHONE_GENERIC))
                    {
                        // Send SMS
                        (void)CContApp_SendSMS(pMe,  (AECHAR *)CContApp_GetNumberByField(pMe, IDX_NUM_RUIM_FLD), NULL); 
                    }
                    else
                    {
                        // Reset select field
                        pMe->m_eOptsRetState = STATE_OPTS;
                        pMe->m_wFiledOptState = FIELDSENDSMS;
                        CLOSE_DIALOG(DLGRET_VIEW);
                    }                    
                    return TRUE;
                    
                case IDI_OPTS_MENU_SENDDIRECTORY:
                    // Load the current fields
                    if(SUCCESS != CContApp_LoadAddrFlds( pMe, 
                                                         pMe->m_wSelectCont,
                                                         SINGLE_SELECT_NUMBER))
                    {
                        pMe->m_wErrStrID = IDS_ERR_LOADFIELDS;
                        CLOSE_DIALOG(DLGRET_ERR);
                        return NFSMACTION_CONTINUE;
                    }
                    
                    // Load the current fields
                    CContApp_GetRecByID(pMe, pMe->m_wSelectCont);
                    CContApp_FormatSendDirectoryContent(pMe);                    
                    
                    // 如果当前记录仅有一个号码则直接发短信
                    //if(1 == CContApp_GetCurrFldNum(pMe, AEE_ADDRFIELD_PHONE_GENERIC))
                    //{
                        // Send SMS
                        //(void)CContApp_SendSMS(pMe,  (AECHAR *)CContApp_GetNumberByField(pMe, IDX_NUM_RUIM_FLD), pMe->m_pBuf); 
                        (void)CContApp_SendSMS(pMe,  NULL, pMe->m_pBuf); 
                    //}
                   // else
                    //{
                        // Reset select field
                    //    pMe->m_eOptsRetState = STATE_OPTS;
                    //    pMe->m_wFiledOptState = FIELDSENDCONTACT;
                    //    CLOSE_DIALOG(DLGRET_VIEW);
                    //}                    
                    return TRUE;
#endif                    

#if defined(FEATURE_SUPPORT_BT_APP) && defined(FEATURE_SUPPORT_VCARD)
//#if defined(FEATURE_SUPPORT_VCARD)		//Modify by zzg 2011_10_25
                case IDI_OPTS_MENU_BLUETOOTH:
                    //CLOSE_DIALOG(DLGRET_DELETE);
                    //if( TRUE == BTApp_OPPCreateVCard(pMe))
                    // fileName 为空，因为vcard文件路径前面要加一个'F'，bt那边需要的。
                    //if(TRUE == Vcard_CreateVcfFile(pMe, pMe->m_wSelectCont, NULL))
                    {
                        char buf[20] = {0};
                        char* str = NULL;
                        sSelectFieldInfo  curSelectFieldInfo = {0};  

                        curSelectFieldInfo.wIndex = pMe->m_wSelectCont;
                        CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                        SPRINTF(buf, "%c%d", 'A', (uint32)pSelectFieldListRoot);

                        str = STRDUP(buf);

						MSG_FATAL("***zzg IDI_OPTS_MENU_BLUETOOTH ***", 0, 0, 0);			
						DBGPRINTF("***zzg Send Via Bluetooth str=%s***", str);
						
                        //ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_BTUIAPP, str);
                        ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_BLUETOOTH_APP, str);	//Modify by zzg 2011_10_26
                        return TRUE;
                        
                        //ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_BTUIAPP, DEFAULT_VCARD_SENDNAME);
                    }
                    return TRUE;                                                //fs:/cfg/default.vcf
#endif
                case IDI_OPTS_MENU_DELETE:
                    CLOSE_DIALOG(DLGRET_DELETE);
                    return TRUE;
                    
                case IDI_OPTS_MENU_COPY:
                    pMe->m_nCopyMoveType = SINGLECOPY;
                    CLOSE_DIALOG(DLGRET_COPY);
                    return TRUE;
                    
                case IDI_OPTS_MENU_MOVE:
                    pMe->m_nCopyMoveType = SINGLEMOVE;
                    CLOSE_DIALOG(DLGRET_COPY);
                    return TRUE;

/*
                case IDI_OPTS_MENU_SEARCHOTHERDIRECTORY:
                    CLOSE_DIALOG(DLGRET_SEARCHOTHERDIRECTORY);
                    return TRUE;
*/
                default:
                    break;
                    
            }
            break;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif             
        default:
            break;
            
    }
    
    return FALSE;
} // CContApp_HandleOptsDlgEvent


/*==============================================================================
函数：
       CContApp_HandleMainMenuDlgEvent
说明：
       IDD_OPTS 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleMainMenuDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam)
{
    IMenuCtl  *pMenuCtl;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_MAINMENU_MENU);
    if(pMenuCtl == NULL)
    {
        return FALSE;
    }
#ifndef FEATURE_VERSION_K212
    MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);
#endif
    switch (eCode)
    {
        case EVT_DIALOG_INIT:            
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenuCtl,OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif            
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
#ifndef FEATURE_VERSION_K212
            IMENUCTL_SetRect(pMenuCtl, &rc);
#endif
            MSG_FATAL("EVT_DIALOG_START",0,0,0);
            
            
            // build options menu
            (void)CContApp_BuildMainMenuMenu(pMe, pMenuCtl);
            
            // restore the menu select
            POP_OPTSMENU_SEL(pMe->m_wOptsStatSel);
            
            {
                AECHAR WTitle[40] = {0};
                //IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);

				#ifdef FEATURE_VERSION_C337
				(void)ISHELL_LoadResString(pMe->m_pShell,
							                CONTAPP_RES_FILE_LANG,                                
							                IDS_PHONEBOOK,
							                WTitle,
							                sizeof(WTitle));
				#else
                (void)ISHELL_LoadResString(pMe->m_pShell,
							                CONTAPP_RES_FILE_LANG,                                
							                IDS_APPLET,
							                WTitle,
							                sizeof(WTitle));
				#endif
                if(pMe->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
            }            

            //(void)IMENUCTL_Redraw(pMenuCtl);

           
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            else
            {
                // Set menu select
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wMainMenuSel);
            }
            
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;                        
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
            // Restore the menu select from suspend
            /*
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            else
            {
                // Set menu select
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wMainMenuSel);
            }
            */
            #ifdef FEATURE_VERSION_SKY
            {
            IImage *image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);
            Appscommon_ResetBackground(pMe->m_pDisplay,image, APPSCOMMON_BG_COLOR, 
                                                    &pMe->m_rc, 0, 0);
             if( image != NULL)
             {
               IIMAGE_Draw( image, 0, 0);
               IIMAGE_Release( image);
             }
            }
            #endif
            
            // Draw prompt bar here
            CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                    
                default:
                    break;                    
            }
            break;
            
        case EVT_COMMAND:
            pMe->m_wMainMenuSel = wParam;
            switch (wParam)
            {
                case IDI_MAINMENU_MENU_DIRECTORTLIST:
                    CLOSE_DIALOG(DLGRET_DIRECTORTLIST);
                    return TRUE;
                    
                case IDI_MAINMENU_MENU_FIND:
                    CLOSE_DIALOG(DLGRET_SEARCH);
                    return TRUE;
                    
                case IDI_MAINMENU_MENU_ADDNEW:
                    pMe->m_nGroupCat = AEE_ADDR_CAT_OTHER;
                    pMe->m_nAddnewOrEdit = ADDOREDIT_ADD;
                    pMe->m_wSelectEdit = IDI_ADDNEW_MENU_NAME;
                    CLOSE_DIALOG(DLGRET_ADDNEW);
                    return TRUE;

                case IDI_MAINMENU_MENU_GROUP:
                    //
                    CLOSE_DIALOG(DLGRET_GROUPVIEW);
                    return TRUE;

                case IDI_MAINMENU_MENU_CONTACT_MANAGEMENT:
                    CLOSE_DIALOG(DLGRET_CONTACTS_MANAGEMENT);
                    return TRUE;
              
                case IDI_MAINMENU_MENU_SPEEDDIAL:
                    CLOSE_DIALOG(DLGRET_ONEDIAL);
                    return TRUE;

#ifdef FEATURE_OEMOMH 
                case IDI_MAINMENU_MENU_EMERGENCY_CALL:
                    MSG_FATAL("IDI_MAINMENU_MENU_EMERGENCY_CALL",0,0,0);
                    CLOSE_DIALOG(DLGRET_EMERGENCY_CALL);
                    return TRUE;
#endif
                default:
                    break;
                    
            }
            break;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt;
						pMe->m_wMainMenuSel = IMENUCTL_GetSel(pMenuCtl);
					    rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,pMe->m_wMainMenuSel);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif             
        default:
            break;
            
    }
    
    return FALSE;
} // CContApp_HandleOptsDlgEvent
/*==============================================================================
函数：
       CContApp_HandleViewDlgEvent
说明：
       IDD_VIEW 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleViewDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam)
{
    IMenuCtl *pMenuCtl;
//    IStatic  *pStatic;
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif

    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_VIEW_MENU);

    MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
    if(eCode==EVT_PEN_UP)
    {
        AEEDeviceInfo devinfo;
        int nBarH ;
        AEERect rc;
        int16 wXPos = (int16)AEE_GET_X(dwParam);
        int16 wYPos = (int16)AEE_GET_Y(dwParam);

        nBarH = GetBottomBarHeight(pMe->m_pDisplay);

        MEMSET(&devinfo, 0, sizeof(devinfo));
        ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
        SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
        MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

        if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
        {
            if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
            {
                eCode=EVT_KEY_PRESS;
                wParam=AVK_SELECT;
            }
            else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
            {
                 eCode=EVT_KEY_PRESS;
                 wParam=AVK_INFO;
            }
            else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
            {                       
                 eCode=EVT_KEY_PRESS;
                 wParam=AVK_CLR;
            }
        }
    }
#endif             
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            // Set the edit menu title is first field
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
            
            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_START",0,0,0);
			#if 0
            (void)IMENUCTL_SetTitle( pMenuCtl, NULL, 0,
                                     (AECHAR *)CContApp_GetFldBuf(pMe, 0));  
			#else
			{
			    if(pMe->m_pIAnn != NULL)
                {         
				    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,(AECHAR *)CContApp_GetFldBuf(pMe, 0));
                }
			}
			#endif
            
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL);
            if(!((pMe->m_wSelFldType == SINGLE_SELECT_NUMBER && STARTMETHOD_SELECTFIELD == pMe->m_eStartMethod)
                || (pMe->m_wSelFldType == SINGLE_SELECT_GROUPLIST && STARTMETHOD_GROUPLIST == pMe->m_eStartMethod)))
            {
                IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif
            }
            else
            {
                IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif
            }

            // build menu
            if(SUCCESS != CContApp_BuildViewMenu(pMe, pMenuCtl))
            {
                CLOSE_DIALOG(DLGRET_ERR);
                return TRUE;
           }
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
             return TRUE;                       
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
            
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
               IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            
            // Draw prompt bar here
            if(pMe->m_wSelectOpts == IDI_OPTS_MENU_VIEW)
            {
               CONTAPP_DRAW_BOTTOMBAR(BTBAR_BACK);  
            }
            else
            {
               CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
            }
            IDISPLAY_Update(pMe->m_pDisplay);
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            return TRUE;
            
        case EVT_KEY_PRESS:
            if((pMe->m_wSelFldType == SINGLE_SELECT_NUMBER && STARTMETHOD_SELECTFIELD == pMe->m_eStartMethod)
                || (pMe->m_wSelFldType == SINGLE_SELECT_GROUPLIST && STARTMETHOD_GROUPLIST == pMe->m_eStartMethod))
            {
                if(AVK_CLR == wParam)
                {
                    CLOSE_DIALOG(DLGRET_CANCELED);
                }
                return FALSE;
            }
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                case AVK_CAMERA:
                {
                	#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| defined(FEATURE_VERSION_C01)
                	{
					nv_item_type	SimChoice;
					OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
					if(SimChoice.sim_select ==AVK_SEND_TWO)
					{
						if(pMe->m_wSelectOpts != IDI_OPTS_MENU_VIEW)
	                    {
	                        CtlAddItem ai;
	                        uint16   wID;
	                            
	                        wID = IMENUCTL_GetSel(pMenuCtl);
	                        if (IMENUCTL_GetItem(pMenuCtl, wID, &ai))
	                        {
	                            switch(pMe->m_wFiledOptState) 
	                            {
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
	                                case FIELDSENDSMS:
	                                    // Send SMS
	                                    (void)CContApp_SendSMS(pMe,  (AECHAR *)ai.pText, NULL);  
	                                    break;
	                                case FIELDSENDCONTACT:
	                                    // Send SMS
	                                    (void)CContApp_SendSMS(pMe,  (AECHAR *)ai.pText, pMe->m_pBuf); 
	                                    break;
#endif
	                                    
	                                case FIELDCALL:
	                                    // Make voice call if it is number
	                                    //(void)MakeVoiceCall(pMe->m_pShell, FALSE, (AECHAR *)ai.pText);
	                                (void)CContApp_MakeCall(pMe, IMENUCTL_GetSel(pMenuCtl));
	                                    break;
#ifdef FEATRUE_SET_IP_NUMBER
	                                case FIELDIPCALL:
	                                {
	                                    ICallApp         *pCallApp = NULL;
	                                    if( SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
	                                                                            AEECLSID_DIALER,
	                                                                            (void **)&pCallApp))
	                                    {
	                                        return FALSE;
	                                    }
	                                    ICallApp_Make_IP_Call_F(pCallApp,(AECHAR *)ai.pText);
	                                    if (pCallApp) 
	                                    {
	                                        ICallApp_Release(pCallApp);
	                                        pCallApp = NULL;
	                                    }
	                                }
	                                break;
#endif
	                                default:
	                                    break;
	                            }
	                        }
	                    } 
					}
					}
					#endif
					return TRUE;
                }
                case AVK_SEND:
                case AVK_INFO:
                case AVK_SELECT:
                    {
                    #if defined(FEATURE_VERSION_C01) 
					{
						nv_item_type	SimChoice;
						OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
						if(SimChoice.sim_select==AVK_SEND_TWO && wParam == AVK_SEND)
						{
							break;
						}
					}
					#endif
                    if(pMe->m_wSelectOpts != IDI_OPTS_MENU_VIEW)
                    {
                        CtlAddItem ai;
                        uint16   wID;
                            
                        wID = IMENUCTL_GetSel(pMenuCtl);
                        if (IMENUCTL_GetItem(pMenuCtl, wID, &ai))
                        {
                            switch(pMe->m_wFiledOptState) 
                            {
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
                                case FIELDSENDSMS:
                                    // Send SMS
                                    (void)CContApp_SendSMS(pMe,  (AECHAR *)ai.pText, NULL);  
                                    break;
                                case FIELDSENDCONTACT:
                                    // Send SMS
                                    (void)CContApp_SendSMS(pMe,  (AECHAR *)ai.pText, pMe->m_pBuf); 
                                    break;
#endif
                                    
                                case FIELDCALL:
                                    // Make voice call if it is number
                                    //(void)MakeVoiceCall(pMe->m_pShell, FALSE, (AECHAR *)ai.pText);
                                (void)CContApp_MakeCall(pMe, IMENUCTL_GetSel(pMenuCtl));
                                    break;
#ifdef FEATRUE_SET_IP_NUMBER
                                case FIELDIPCALL:
                                {
                                    ICallApp         *pCallApp = NULL;
                                    if( SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                                                            AEECLSID_DIALER,
                                                                            (void **)&pCallApp))
                                    {
                                        return FALSE;
                                    }
                                    ICallApp_Make_IP_Call_F(pCallApp,(AECHAR *)ai.pText);
                                    if (pCallApp) 
                                    {
                                        ICallApp_Release(pCallApp);
                                        pCallApp = NULL;
                                    }
                                }
                                break;
#endif
                                default:
                                    break;
                            }
                        }
                    } 
                }
                //CLOSE_DIALOG(DLGRET_OK);
                return TRUE;
                    
                default:
                    break;                    
            }
            break;
            
        case EVT_COMMAND:
            if(pMe->m_wSelectOpts != IDI_OPTS_MENU_VIEW)
            {
                pMe->m_wSelectFld  = wParam;
            }

            if((pMe->m_wSelFldType == SINGLE_SELECT_NUMBER && STARTMETHOD_SELECTFIELD == pMe->m_eStartMethod)
                || (pMe->m_wSelFldType == SINGLE_SELECT_GROUPLIST && STARTMETHOD_GROUPLIST == pMe->m_eStartMethod))
            {
                sSelectFieldInfo  curSelectFieldInfo = {0};  
                CtlAddItem ai;
                uint16   wID;
                dword       mask = 0;
                
                wID = IMENUCTL_GetSel(pMenuCtl);
                if (IMENUCTL_GetItem(pMenuCtl, wID, &ai))
                {
                    switch(ai.wImage)
                    {
                        case  IDB_PHONE:
                            mask = MASK_ADDRFIELD_PHONE_GENERIC;
                            break;

                        case IDB_HOMENUM:
                            mask = MASK_ADDRFIELD_PHONE_HOME;
                            break;
                            
                        case IDB_WORKNUM:
                            mask = MASK_ADDRFIELD_PHONE_WORK;
                            break;
                        
                        case IDB_FAXNUM:
                            mask = MASK_ADDRFIELD_PHONE_FAX;
                            break;
                            
                        default:
                            return TRUE;
                    }
                }

                curSelectFieldInfo.wIndex = pMe->m_wSelectCont;
                curSelectFieldInfo.wFieldMask = mask;

                CContApp_FreeSelectFieldListNode();
                CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);

                CLOSE_DIALOG(DLGRET_SELECT_SINGLE_NUMBER);
            }
            return TRUE;
            
        case EVT_CTL_SEL_CHANGED:
            pMe->m_wSelectFld = wParam;
            return TRUE;
        default:
            break;
            
    }
    
    return FALSE;
} // CContApp_HandleViewDlgEvent

/*==============================================================================
函数：
       CContApp_HandleFldOptsDlgEvent
说明：
       IDD_FLDOPTS 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleFldOptsDlgEvent( CContApp  *pMe,
                                                AEEEvent   eCode,
                                                uint16     wParam,
                                                uint32     dwParam)
{
    IMenuCtl *pMenuCtl;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_FLDOPTS_MENU);
                                              
    MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect        rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
           
            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_START",0,0,0);
            
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif
            
            {
                AECHAR WTitle[40] = {0};
                //IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
                #ifdef FEATURE_VERSION_C337
				(void)ISHELL_LoadResString(pMe->m_pShell,
							                CONTAPP_RES_FILE_LANG,                                
							                IDS_PHONEBOOK,
							                WTitle,
							                sizeof(WTitle));
				#else
                (void)ISHELL_LoadResString(pMe->m_pShell,
							                CONTAPP_RES_FILE_LANG,                                
							                IDS_APPLET,
							                WTitle,
							                sizeof(WTitle));
				#endif
                if(pMe->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
            }


            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
                                    
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            else
            {
                // Set menu select
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectFldOpts);
            }
            
            // Draw prompt bar here
            CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                    
                default:
                    break;
                    
            }
            break;
            
        case EVT_COMMAND:
            pMe->m_wSelectFldOpts = wParam;
            switch(wParam)
            {
                case IDI_FLDOPTS_MENU_EDIT:
                    FREEIF(pMe->m_pFldInputBuf);
                    pMe->m_nFldInputID  = CContApp_GetFldID(pMe, 
                                                  pMe->m_wSelectFld);
                                                  
                    pMe->m_pFldInputBuf = WSTRDUP((AECHAR *)
                         CContApp_GetFldBuf(pMe, pMe->m_wSelectFld));

                    if(pMe->m_nFldInputID == AEE_ADDRFIELD_BIRTHDAY)
                    {
                        CLOSE_DIALOG(DLGRET_SELBIRTHDAY);
                    }
                    else
                    {
                        CLOSE_DIALOG(DLGRET_EDIT);
                    }
                    return TRUE;
                    
                case IDI_FLDOPTS_MENU_DELETE:
                    CLOSE_DIALOG(DLGRET_DELETE);
                    return TRUE;

#ifdef FEATURE_RUIM_PHONEBOOK
                case IDI_FLDOPTS_MENU_COPYTORUIM:
                case IDI_FLDOPTS_MENU_COPYTOPHONE:
                    CLOSE_DIALOG(DLGRET_COPY);
                    return TRUE;
#endif// FEATURE_RUIM_PHONEBOOK

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
                case IDI_FLDOPTS_MENU_SENDSMS:
                    (void)CContApp_SendSMS(pMe, (AECHAR *)
                                   CContApp_GetFldBuf( pMe, 
                                                       pMe->m_wSelectFld), NULL);
                    return TRUE;
#endif
                    
                case IDI_FLDOPTS_MENU_SETTYPE:
                    CLOSE_DIALOG(DLGRET_SETTYPE);
                    return TRUE;
                
                default:
                    break;
            }
            break;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif             
        default:
            break;
            
    }

    return FALSE;
} // CContApp_HandleFldOptsDlgEvent


/*==============================================================================
函数：
       CContApp_HandleFindDlgEvent
说明：
       IDD_FIND 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleFindDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam)
{
    ITextCtl *pTextCtl;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pTextCtl = (ITextCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_FIND_TEXTCTL);
                                              
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
            ITEXTCTL_SetRect(pTextCtl, &rc);
            
            ITEXTCTL_SetProperties( pTextCtl, TP_MULTILINE | TP_FRAME | TP_FIXSETRECT | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT|TP_FOCUS_NOSEL | TP_GRAPHIC_BG);
            
            (void)ITEXTCTL_SetInputMode(pTextCtl,
                     CContApp_GetFldInputMode(AEE_ADDRFIELD_NAME));
                     
            (void)ITEXTCTL_SetTitle( pTextCtl,
                                     CONTAPP_RES_FILE_LANG,
                     CContApp_GetFldTitleID(AEE_ADDRFIELD_NAME),
                                     NULL);
                                     
            ITEXTCTL_SetMaxSize(pTextCtl, MAX_INPUT_NAME_EN);
                     
            if(pMe->m_pFldInputBuf)
            {
                (void)ITEXTCTL_SetText(pTextCtl, pMe->m_pFldInputBuf, -1);
            }
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
                                    
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_KEY_RELEASE:
        case EVT_USER_REDRAW:
            // Draw prompt bar here
  //bw:<< 071014 can't draw Battom when display Symbol dialog
            {
                AEETextInputMode nInputMode; 
                nInputMode = ITEXTCTL_GetInputMode(pTextCtl,NULL);
                if (nInputMode == AEE_TM_SYMBOLS)
                {
                    return TRUE;
                }
                //bw:>>
                #ifdef FEATURE_ALL_KEY_PAD
                CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_BACK);   
                #else
                if ( WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) > 0 && ITEXTCTL_GetT9End(pTextCtl) != TC_CURSORSTART )
                {
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_DELETE);        
                }
                else
                {
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_BACK);                                
                }
                #endif
                IDISPLAY_Update(pMe->m_pDisplay);  
            }         
            return TRUE;
            
        case EVT_DIALOG_END:
            if (pMe->m_bSuspending)
            {
                FREEIF(pMe->m_pFldInputBuf);
                pMe->m_pFldInputBuf = WSTRDUP(ITEXTCTL_GetTextPtr(pTextCtl));
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_INFO:
                case AVK_SELECT:
                    FREEIF(pMe->m_pFldInputBuf);
                    pMe->m_pFldInputBuf = 
                           WSTRDUP(ITEXTCTL_GetTextPtr(pTextCtl));
                    
                    if(WSTRLEN(pMe->m_pFldInputBuf) > 0)
                    {
                        CLOSE_DIALOG(DLGRET_OK);
                    }
                    else
                    {
                        CLOSE_DIALOG(DLGRET_INPUT_NULL);
                    }
                    return TRUE;
                    
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                    
                default:
                    break;
                    
            }
            break;
            
        case EVT_COMMAND:
            break;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif             
        default:
            break;
            
    }
    
    return FALSE;
} // CContApp_HandleFindDlgEvent

/*==============================================================================
函数：
       CContApp_HandleGroupDlgEvent
说明：
       IDD_GROUP 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleGroupDlgEvent( CContApp  *pMe,
                                              AEEEvent   eCode,
                                              uint16     wParam,
                                              uint32     dwParam)
{
    IMenuCtl *pMenuCtl;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_GROUP_MENU);
                                              
    MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);

            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_START",0,0,0);
            
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif

            CContApp_BuildGroupMenu(pMe, pMenuCtl);
    
            // Set menu select
            //(void)CContApp_SetGroupMenuSel(pMe, pMenuCtl);

            {
                AECHAR WTitle[40] = {0};
                //IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);

                
#ifdef FEATURE_VERSION_C337
				(void)ISHELL_LoadResString(pMe->m_pShell,
							                CONTAPP_RES_FILE_LANG, 
											IDS_CALLER_GROUPS,
											WTitle,
							                sizeof(WTitle));
#else                
				(void)ISHELL_LoadResString(pMe->m_pShell,
							                CONTAPP_RES_FILE_LANG, 
											IDS_INPUT_GROUP,
											WTitle,
							                sizeof(WTitle));
#endif                
                
                if(pMe->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
            }

            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            else
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wselGroupId);
            }

            // Draw prompt bar here
            if(IDS_GROUP_NONE_STRING == IMENUCTL_GetSel(pMenuCtl) 
                || IDS_GROUP_UIM == IMENUCTL_GetSel(pMenuCtl) 
                || STARTMETHOD_GROUPLIST == pMe->m_eStartMethod)
            {
                CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
            }
            else
            {
                CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
            }
            IMENUCTL_Redraw(pMenuCtl);
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    pMe->m_wselGroupId = 0;
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;

                case AVK_INFO:
                    pMe->m_wselGroupId = IMENUCTL_GetSel(pMenuCtl);
                    
                    CContApp_GetGroupCat(pMe, pMe->m_wselGroupId);
                    CLOSE_DIALOG(DLGRET_VIEW);
                    return TRUE;

                    
                default:
                    break;
                    
            }
            break;
            
        case EVT_COMMAND:
            pMe->m_wselGroupId = wParam;
            CContApp_GetGroupCat(pMe, pMe->m_wselGroupId);
            CLOSE_DIALOG(DLGRET_OK);
            return TRUE;
            
        case EVT_CTL_SEL_CHANGED:
            pMe->m_wselGroupId = wParam;
            if(wParam == IDS_GROUP_NONE_STRING || wParam == IDS_GROUP_UIM || STARTMETHOD_GROUPLIST == pMe->m_eStartMethod)
            {
                CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
            }
            else
            {
                CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt ;
						pMe->m_wselGroupId = IMENUCTL_GetSel(pMenuCtl);
						rt=ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,pMe->m_wselGroupId);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif           
        default:
            break;
            
    }

    return FALSE;
} // CContApp_HandleGroupDlgEvent

#ifdef FEATURE_RUIM_PHONEBOOK
/*==============================================================================
函数：
       CContApp_HandlePositionDlgEvent
说明：
       IDD_POSITION 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandlePositionDlgEvent( CContApp  *pMe,
                                                 AEEEvent   eCode,
                                                 uint16     wParam,
                                                 uint32     dwParam)
{
    IMenuCtl *pMenuCtl;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_POSITION_MENU);
                                              
    MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);

            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_START",0,0,0);
            if(STATE_SELECTPOS == pMe->m_eCurState)
            {
                IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL | MP_BIND_ITEM_TO_NUMBER_KEY);
            }
            else
            {
                IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL | MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);
            }
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif
            {
                AECHAR WTitle[40] = {0};
                //IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
                (void)ISHELL_LoadResString(pMe->m_pShell,
                CONTAPP_RES_FILE_LANG,                                
                IDS_POSITION,
                WTitle,
                sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
            }

            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
            #ifdef FEATURE_VERSION_SKY
            {
            IImage *image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);
            Appscommon_ResetBackground(pMe->m_pDisplay,image, APPSCOMMON_BG_COLOR, 
                                                    &pMe->m_rc, 0, 0);
             if( image != NULL)
             {
               IIMAGE_Draw( image, 0, 0);
               IIMAGE_Release( image);
             }
             IMENUCTL_Redraw(pMenuCtl);
            }
            #endif
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            else
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wPositionSel);
            }
            
            // Draw prompt bar here
            if(STATE_SAVETO == pMe->m_eCurState)
            {
                CONTAPP_DRAW_BOTTOMBAR(BTBAR_SAVE_BACK);
            }
            else
            {
                CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
            }
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    pMe->m_wPositionSel = 0;
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                    
                default:
                    break;
                    
            }
            break;
            
        case EVT_COMMAND:
            pMe->m_wPosCmd = wParam;
            if((IDS_UIMCARD == wParam 
                && STARTMETHOD_SAVEFIELD == pMe->m_eStartMethod
                && WSTRLEN(pMe->m_pAddNewMobile) > OEMRUIMAddr_GetRUIMMaxNumberSize())
                ||((IDS_POSITION_PHONE == wParam) && (WSTRLEN(pMe->m_pAddNewMobile)> MAX_INPUT_NUM))
                )
            {
                CLOSE_DIALOG(DLGRET_NUMBER_TOOLONG);
                return TRUE;
            }
            
            if(STATE_SELECTPOS != pMe->m_eCurState)
            {
                InitMenuIcons(pMenuCtl);
                SetMenuIcon(pMenuCtl, wParam, TRUE);
            }
            CLOSE_DIALOG(DLGRET_OK);
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
			case EVT_PEN_UP:
				{
					AEEDeviceInfo devinfo;
					int nBarH ;
					AEERect rc;
					int16 wXPos = (int16)AEE_GET_X(dwParam);
					int16 wYPos = (int16)AEE_GET_Y(dwParam);
	
					nBarH = GetBottomBarHeight(pMe->m_pDisplay);
			
					MEMSET(&devinfo, 0, sizeof(devinfo));
					ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
					SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
					MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);
	
					if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
					{
						if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
						{
							boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,pMe->m_wSelectStore);
							return rt;
						}
						else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
						{						
							 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
							 return rt;
						}
					}
	
				}
				break;
#endif 

        default:
            break;
            
    }

    return FALSE;
} // CContApp_HandlePositionDlgEvent

/*==============================================================================
函数：
       CContApp_HandleCopyMoveDlgEvent
说明：
       IDD_COPYMOVE 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleCopyMoveDlgEvent( CContApp  *pMe,
                                                 AEEEvent   eCode,
                                                 uint16     wParam,
                                                 uint32     dwParam)
{
    IMenuCtl *pMenuCtl;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_COPYMOVE_MENU);
                                              
    MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);

            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_START",0,0,0);
            
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL | MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif
            if(COPYMULTIPE == pMe->m_nCopyOrMove)
            {
            	#if 0
                (void)IMENUCTL_SetTitle( pMenuCtl,CONTAPP_RES_FILE_LANG,IDS_COPY,NULL);
				#else
				{
					AECHAR WTitle[40] = {0};
					(void)ISHELL_LoadResString(pMe->m_pShell,
							CONTAPP_RES_FILE_LANG,								  
							IDS_COPY,
							WTitle,
							sizeof(WTitle));
                    if(pMe->m_pIAnn != NULL)
                    {
					    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                    }
				}
				#endif
            }
            else if(MOVEMULTIPE == pMe->m_nCopyOrMove)
            {
            	#if 0
                (void)IMENUCTL_SetTitle( pMenuCtl,CONTAPP_RES_FILE_LANG,IDS_MOVE,NULL);        
				#else
				{
					AECHAR WTitle[40] = {0};
					(void)ISHELL_LoadResString(pMe->m_pShell,
							CONTAPP_RES_FILE_LANG,								  
							IDS_MOVE,
							WTitle,
							sizeof(WTitle));
                    if(pMe->m_pIAnn != NULL)
                    {
					    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                    }
				}
				#endif
            }
            
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
             return TRUE;                        
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
            #ifdef FEATURE_VERSION_SKY
            {
            IImage *image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);
            Appscommon_ResetBackground(pMe->m_pDisplay,image, APPSCOMMON_BG_COLOR, 
                                                    &pMe->m_rc, 0, 0);
             if( image != NULL)
             {
               IIMAGE_Draw( image, 0, 0);
               IIMAGE_Release( image);
             }
             IMENUCTL_Redraw(pMenuCtl);
            }
            #endif
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            else
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectCopyMove);
            }
            // Draw prompt bar here
            CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    pMe->m_wSelectCopyMove = 0;
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                    
                default:
                    break;
                    
            }
            break;
            
        case EVT_COMMAND:
            pMe->m_wSelectCopyMove = wParam;
            switch(wParam)
            {
                case IDI_PHONE_TO_UIM:
                    if(COPYMULTIPE == pMe->m_nCopyOrMove)
                    {
                        pMe->m_nCopyMoveType = COPYMULTIPE_TOUIM;
                    }
                    else
                    {
                        pMe->m_nCopyMoveType = MOVEMULTIPE_TOUIM;
                    }
                    //pMe->m_nViewType = CONTCFG_VIEWTYPE_PHONE;
                    CContApp_LoadPhoneBook(pMe, CONTCFG_VIEWTYPE_PHONE);
                    CLOSE_DIALOG(DLGRET_COPY);
                    return TRUE;
          
                case IDI_UIM_TO_PHONE:
                    if(COPYMULTIPE == pMe->m_nCopyOrMove)
                    {
                        pMe->m_nCopyMoveType = COPYMULTIPE_TOPHONE;
                    }
                    else
                    {
                        pMe->m_nCopyMoveType = MOVEMULTIPE_TOPHONE;
                    }
                    CContApp_LoadPhoneBook(pMe, CONTCFG_VIEWTYPE_RUIM);
                    CLOSE_DIALOG(DLGRET_COPY);
                    return TRUE;
/*
                case IDI_COPYMOVE_MENU_MOVETOPHONE:
                    pMe->m_nCopyMoveType = MOVEALLTOPHONE;
                    CLOSE_DIALOG(DLGRET_ALLCOPYMOVE);
                    return TRUE;
                case IDI_COPYMOVE_MENU_MOVETORUIM:
                    pMe->m_nCopyMoveType = MOVEALLTOUIM;
                    CLOSE_DIALOG(DLGRET_ALLCOPYMOVE);
                    return TRUE;
                case IDI_COPYMOVE_MENU_COPYTOPHONE:
                    pMe->m_nCopyMoveType = COPYALLTOPHONE;
                    CLOSE_DIALOG(DLGRET_ALLCOPYMOVE);
                    return TRUE;
                case IDI_COPYMOVE_MENU_COPYTORUIM:
                    pMe->m_nCopyMoveType = COPYALLTOUIM;
                    CLOSE_DIALOG(DLGRET_ALLCOPYMOVE);
                    return TRUE;
                    
                case IDI_COPYMOVE_MENU_COPY:
                    CLOSE_DIALOG(DLGRET_COPY);
                    return TRUE;
      */
                default:
                    break;
            }
            break;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt;
						pMe->m_wSelectCopyMove = IMENUCTL_GetSel(pMenuCtl);
						rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,pMe->m_wSelectCopyMove);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif  
        default:
            break;
            
    }

    return FALSE;
} // CContApp_HandleCopyMoveDlgEvent

/*==============================================================================
函数：
       CContApp_HandleCopyDlgEvent
说明：
       IDD_COPY 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleCopyDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam)
{
    IMenuCtl  *pMenuCtl = NULL;  
    ITextCtl    *pTextCtl = NULL; 
    uint16 rtn = TYPE_NO_MATCH;
    sSelectFieldInfo  curSelectFieldInfo;        
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg, IDC_COPY_MENU);
    pTextCtl = (ITextCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg, IDC_COPY_TEXT);   
    
    pMe->m_nSmartStateType = SMART_STATE_IDD_COPY; 
    MEMSET(&curSelectFieldInfo, 0, sizeof(curSelectFieldInfo));  

    if(CContApp_SmartMenuHandle(pMe, pMenuCtl, pTextCtl, eCode,wParam))
    {
        return TRUE;
    }
	
    CContApp_CopyMenuScroll(pMe, pMenuCtl, eCode, wParam); 
    
    // Scroll the menu if record is less than loaded
    if (IVector_Size(pMe->m_pAddList) <= MAX_NUM_MENUPOP)
    {
        MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        {
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            uint32 dwMask = IMENUCTL_GetProperties(pMenuCtl);
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
  
            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_INIT",0,0,0);
      
            dwMask = (dwMask & (~MP_WRAPSCROLL)) | MP_UNDERLINE_TITLE | MP_MULTI_SEL;
            IMENUCTL_SetProperties(pMenuCtl, dwMask);
            //IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL);
           // IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE);
           IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif
            
        }
        return TRUE;
            
        case EVT_DIALOG_START:
        {            
            //MP_NO_REDRAW
            uint32    dwMask = IMENUCTL_GetProperties(pMenuCtl);
            IMENUCTL_SetProperties(pMenuCtl, dwMask & (~MP_NO_REDRAW));
            //IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
            if(COPYMULTIPE_TOUIM == pMe->m_nCopyMoveType || COPYMULTIPE_TOPHONE == pMe->m_nCopyMoveType)
            {
            	#if 0
                (void)IMENUCTL_SetTitle( pMenuCtl,CONTAPP_RES_FILE_LANG,IDS_COPY,NULL);
				#else
				{
					AECHAR WTitle[40] = {0};
					(void)ISHELL_LoadResString(pMe->m_pShell,
							CONTAPP_RES_FILE_LANG,								  
							IDS_COPY,
							WTitle,
							sizeof(WTitle));
                    if(pMe->m_pIAnn != NULL)
                    {
					    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                    }
				}
				#endif
            }
            else
            {
            	#if 0
                (void)IMENUCTL_SetTitle( pMenuCtl,CONTAPP_RES_FILE_LANG,IDS_MOVE,NULL); 
				#else
				{
					AECHAR WTitle[40] = {0};
					(void)ISHELL_LoadResString(pMe->m_pShell,
							CONTAPP_RES_FILE_LANG,								  
							IDS_MOVE,
							WTitle,
							sizeof(WTitle));
                    if(pMe->m_pIAnn != NULL)
                    {
					    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                    }
				}
				#endif
            }
            // Build the list Menu
            if(NULL == pMe->m_szAlpha)
            {
                (void)CContApp_BuildCopyMenu(pMe, pMenuCtl);
            }

            if(CONTAPP_RECID_NULL == pMe->m_wSelectCont)
            {
                pMe->m_wSelectCont = IMENUCTL_GetItemID(pMenuCtl, 0);
            }
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;                        
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
            #ifdef FEATURE_VERSION_SKY
            {
            IImage *image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);
            Appscommon_ResetBackground(pMe->m_pDisplay,image, APPSCOMMON_BG_COLOR, 
                                                    &pMe->m_rc, 0, 0);
             if( image != NULL)
             {
               IIMAGE_Draw( image, 0, 0);
               IIMAGE_Release( image);
             }
             IMENUCTL_Redraw(pMenuCtl);
            }
            #endif
           /*必须在textctl初始化完毕后,才能获得icon id,而且要在dialog更新完之后再更新图标*/
           CContApp_DrawIMEIcon(pTextCtl, pMe->m_pDisplay);
                       
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            else
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectCont);
            }
            pMe->m_wSelectSmart= IMENUCTL_GetSel(pMenuCtl);
            
            // Draw prompt bar here
            
            if(NULL == pMe->m_szAlpha)
            {
                CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
            }
            CContApp_DrawScrollBar(pMe, pMenuCtl);
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }

            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    pMe->m_wMainListSel = MENU_SELECT_NULL;
                    pMe->m_wMainListIdx = 0;
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;

                default:
                    break;
                    
            }
            // App handle all key event
            return TRUE;

        case EVT_KEY_PRESS:
            pMe->m_wSelectCont = IMENUCTL_GetSel(pMenuCtl);  
            // store the menu select
            PUSH_LISTMENU_SEL(pMe->m_wMainListSel);
            
            // store the menu index
            PUSH_LISTMENU_IDX(pMe->m_wMainListIdx);
            switch (wParam)
            {
                case AVK_SELECT:
                    {
                        curSelectFieldInfo.wIndex = pMe->m_wSelectCont;
                        
                        rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                        if (rtn == TYPE_NO_MATCH)
                        {
                            CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                        }
                        else 
                        {
                            CContApp_DeleteSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                        }
                    }
                
                                   // Build the list Menu
                   pMe->m_wSelectCont = IMENUCTL_GetSel(pMenuCtl);
                   (void)CContApp_BuildCopyMenu(pMe, pMenuCtl);
                   IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectCont);
                   (void)IMENUCTL_Redraw(pMenuCtl);
                   
                    break;
                    
              //  case AVK_INFO:                    
              //      CLOSE_DIALOG(DLGRET_OK);
              //      return TRUE;

                default:
                    break;
            }
            return TRUE;
            
        case EVT_COMMAND:
            pMe->m_wSelectSmart = wParam;
            pMe->m_wSelectCont = wParam;
            #ifdef FEATURE_VERSION_X3
            if(SUCCESS != CContApp_LoadAddrFlds( pMe, pMe->m_wSelectSmart, SELECT_ALL))
             {
                 CLOSE_DIALOG(DLGRET_ERR);
                 return TRUE;
             }
             curSelectFieldInfo.wIndex = pMe->m_wSelectSmart;
             pMe->m_wFieldIndex = CContApp_GetOneNumberFld(pMe, IDX_NUM_RUIM_FLD);
             curSelectFieldInfo.wFieldMask = pMe->m_wFieldIndex;
             rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
            if (rtn == TYPE_NO_MATCH)
            {
                pMe->m_eTotalSelectNum++;
                CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
            }
            #endif
            CLOSE_DIALOG(DLGRET_SELECT);
            return TRUE;
            
        case EVT_CTL_SEL_CHANGED:
            pMe->m_wSelectSmart = wParam;
            pMe->m_wSelectCont = wParam;
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,EVT_KEY_RELEASE);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif             
        default:
            break;            
   } // CContApp_HandleCopyDlgEvent
    return FALSE;
}
#endif /* FEATURE_RUIM_PHONEBOOK */

/*==============================================================================
函数：
       CContApp_HandleOnOffDlgEvent
说明：
       IDD_ONOFF 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleOnOffDlgEvent( CContApp  *pMe,
                                              AEEEvent   eCode,
                                              uint16     wParam,
                                              uint32     dwParam)
{
    IMenuCtl *pMenuCtl;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_ONOFF_MENU);
                                              
    MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);

            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_START",0,0,0);

            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif
            // 根据 pMe->m_bOnOff 初始化菜单
            //MENU_INIT_SELICONS(pMenuCtl);
            InitCheckBox(pMenuCtl);
            if(pMe->m_bOnOff)
            {
                //MENU_SET_SELICON(pMenuCtl, IDI_ONOFF_MENU_ON, TRUE);
                SetCheckBoxItem(pMenuCtl, IDI_ONOFF_MENU_ON, TRUE);
                IMENUCTL_SetSel(pMenuCtl, IDI_ONOFF_MENU_ON);
            }
            else
            {
                //MENU_SET_SELICON(pMenuCtl, IDI_ONOFF_MENU_ON, TRUE);
                SetCheckBoxItem(pMenuCtl, IDI_ONOFF_MENU_OFF, TRUE);
                IMENUCTL_SetSel(pMenuCtl, IDI_ONOFF_MENU_OFF);
            }
            
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
                                    
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            
            // Draw prompt bar here
            CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
            IDISPLAY_Update(pMe->m_pDisplay); 
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                    
                default:
                    break;
                    
            }
            break;
            
        case EVT_COMMAND:
            switch(wParam)
            {
                case IDI_ONOFF_MENU_ON:
                    CLOSE_DIALOG(DLGRET_YES);
                    return TRUE;
                    
                case IDI_ONOFF_MENU_OFF:
                    CLOSE_DIALOG(DLGRET_NO);
                    return TRUE;
                default:
                    break;
            }
            break;
            
        default:
            break;
            
    }

    return FALSE;
} // CContApp_HandleOnOffDlgEvent

/*==============================================================================
函数：
       CContApp_HandleOneDialDlgEvent
说明：
       IDD_ONEDIAL 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleOneDialDlgEvent( CContApp  *pMe,
                                                AEEEvent   eCode,
                                                uint16     wParam,
                                                uint32     dwParam)
{
    IMenuCtl *pMenuCtl;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_ONEDIAL_MENU);
                                              
    MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);

	MSG_FATAL("***zzg CContApp_HandleOneDialDlgEvent eCode=%x", eCode, 0, 0);
	
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);

            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_START",0,0,0);
            
            // list the one dial menu
            (void)CContApp_BuildOneDialMenu(pMe, pMenuCtl);
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_USE_MENU_STYLE|OEMMP_DISTINGUISH_INFOKEY_SELECTKEY);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif
            //MENU_SET_COMICON(pMenuCtl);
            {
                AECHAR WTitle[40] = {0};
                //IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
                (void)ISHELL_LoadResString(pMe->m_pShell,
                CONTAPP_RES_FILE_LANG,                                
                IDS_ONEDIAL,
                WTitle,
                sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
            }


            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return  TRUE;                        
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            else
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectOneDial);
            }
            
            // Draw prompt bar here
            if( TRUE == CContApp_OnedialExit(pMe))
            {
                CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
            }
            else
            {
				CONTAPP_DRAW_BOTTOMBAR(BTBAR_ADD_BACK);   	//Add By zzg 2012_11_09         	
                //CONTAPP_DRAW_BOTTOMBAR(BTBAR_EDIT_BACK);
            }
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    pMe->m_wSelectOneDial = CONTCFG_ONEDIAL2;//CONTCFG_ONEDIAL1
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                case AVK_INFO:
                    CLOSE_DIALOG(DLGRET_SET);
                    break;
                    
                default:
                    break;
                    
            }
            break;
            
        case EVT_COMMAND:
            pMe->m_wSelectOneDial = wParam;
            CLOSE_DIALOG(DLGRET_OK);
            return TRUE;

        case EVT_CTL_SEL_CHANGED:
            pMe->m_wSelectOneDial = wParam;
            // Draw prompt bar here
            if( TRUE == CContApp_OnedialExit(pMe))
            {
                CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
            }
            else
            {
            	CONTAPP_DRAW_BOTTOMBAR(BTBAR_ADD_BACK);   	//Add By zzg 2012_11_09       
                //CONTAPP_DRAW_BOTTOMBAR(BTBAR_EDIT_BACK);
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						int i =  IMENUCTL_GetSel(pMenuCtl);
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,i);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif             
        default:
            break;
            
    }
    
    return FALSE;
} // CContApp_HandleOneDialDlgEvent


/*==============================================================================
函数：
       CContApp_HandleOneDialOptDlgEvent
说明：
       IDD_ONEDIALOPT 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleOneDialOptDlgEvent( CContApp  *pMe,
                                                   AEEEvent   eCode,
                                                   uint16     wParam,
                                                   uint32     dwParam)
{
    IMenuCtl *pMenuCtl;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_ONEDIALOPT_MENU);
                                              
    MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);
#ifdef FEATURE_LCD_TOUCH_ENABLE
    if(eCode == EVT_PEN_UP)
        {
            int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
            int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
            AEEDeviceInfo devinfo;
            int nBarH ;
            AEERect rc;
            MSG_FATAL("CContApp_HandleOneDialNumFldSelDlgEvent wXPos=%d ,wYPos=%d",wXPos,wYPos,0);
             
            nBarH = GetBottomBarHeight(pMe->m_pDisplay);
            MEMSET(&devinfo, 0, sizeof(devinfo));
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);  
            if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
            {
                if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
                {
                    eCode = EVT_COMMAND;
                     wParam = IMENUCTL_GetSel(pMenuCtl);
                } 
                else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//右
                { 
                    eCode = EVT_KEY;
                     wParam = AVK_CLR;
                }
            }  
        
        }
                                            
#endif    
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);

            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_START",0,0,0);
            IMENUCTL_SetPopMenuRect(pMenuCtl); 
            
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL);
            //MENU_SET_COMICON(pMenuCtl);
            
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;                        
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            
            // Draw prompt bar here
            CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                    
                default:
                    break;
                    
            }
            break;
            
        case EVT_COMMAND:
            switch(wParam)
            {
                case IDI_ONEDIALOPT_MENU_SET:
                    CLOSE_DIALOG(DLGRET_SET);
                    return TRUE;
                    
                case IDI_ONEDIALOPT_MENU_CLR:
                    CLOSE_DIALOG(DLGRET_CLR);
                    return TRUE;
                    
                default:
                    break;
            }
            break;
            
        default:
            break;
            
    }
    
    return FALSE;
} // CContApp_HandleOneDialOptDlgEvent


/*==============================================================================
函数：
       CContApp_HandleOneDialNumFldSelDlgEvent
说明：
       IDD_ONEDIAL_SET_NUM_SEL 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleOneDialNumFldSelDlgEvent( CContApp  *pMe,
                                                   AEEEvent   eCode,
                                                   uint16     wParam,
                                                   uint32     dwParam)
{
    IMenuCtl *pMenuCtl;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_ONEDIAL_SET_NUM_SEL);
                                              
    MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);
#ifdef FEATURE_LCD_TOUCH_ENABLE
    if(eCode == EVT_PEN_UP)
        {
            int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
            int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
            AEEDeviceInfo devinfo;
            int nBarH ;
            AEERect rc;
            MSG_FATAL("CContApp_HandleOneDialNumFldSelDlgEvent wXPos=%d ,wYPos=%d",wXPos,wYPos,0);
             
            nBarH = GetBottomBarHeight(pMe->m_pDisplay);
            MEMSET(&devinfo, 0, sizeof(devinfo));
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);  
            if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
            {
                if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
                {
                     eCode = EVT_COMMAND;
                     wParam = IMENUCTL_GetSel(pMenuCtl);
                } 
                else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//右
                { 
                    eCode = EVT_KEY;
                     wParam = AVK_CLR;
                }
            }  
        
        }
                                            
#endif    
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_START",0,0,0);
            
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif
            // build menu
            if(SUCCESS != CContApp_BuildOneDialNumFldSelMenu(pMe, pMenuCtl))
            {
                CLOSE_DIALOG(DLGRET_ERR);
                return TRUE;
            }
            
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
                                    
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            
            // Draw prompt bar here
            CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
				case AVK_CAMERA:
				{
					#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)
					{
					nv_item_type	SimChoice;
					OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
					if(SimChoice.sim_select ==AVK_SEND_TWO)
					{
						FREEIF(pMe->m_pOneDial);
	                    pMe->m_wSelectCont = IMENUCTL_GetSel(pMenuCtl);
	                    pMe->m_pOneDial = WSTRDUP((AECHAR *)
	                        CContApp_GetFldBuf(pMe, pMe->m_wSelectFld));
	                    if(WSTRLEN(pMe->m_pOneDial)<=3)
	                    {
	                        CLOSE_DIALOG(DLGRET_ERR);
	                    }
	                    else
	                    {
	                        (void)CContApp_MakeCall(pMe, pMe->m_wSelectFld);
	                        //CLOSE_DIALOG(DLGRET_CALL);
	                    }
	                    return TRUE;
                    }
                    }
                    #endif
				}
				break;
                case AVK_SEND:
                	#if defined(FEATURE_VERSION_C01) 
					{
						nv_item_type	SimChoice;
						OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
						if(SimChoice.sim_select==AVK_SEND_TWO)
						{
							return TRUE;
						}
					}
					#endif
                    FREEIF(pMe->m_pOneDial);
                    pMe->m_wSelectCont = IMENUCTL_GetSel(pMenuCtl);
                    pMe->m_pOneDial = WSTRDUP((AECHAR *)
                        CContApp_GetFldBuf(pMe, pMe->m_wSelectFld));
                    if(WSTRLEN(pMe->m_pOneDial)<=3)
                    {
                        CLOSE_DIALOG(DLGRET_ERR);
                    }
                    else
                    {
                        (void)CContApp_MakeCall(pMe, pMe->m_wSelectFld);
                        //CLOSE_DIALOG(DLGRET_CALL);
                    }
                    return TRUE;
                    
                default:
                    break;
                    
            }
            break;
            
        case EVT_COMMAND:
            FREEIF(pMe->m_pOneDial);
            pMe->m_wSelectFld  = wParam;
            ERR("pMe->m_wSelectFld = %d",pMe->m_wSelectFld,0,0);
            pMe->m_pOneDial = WSTRDUP((AECHAR *)
                CContApp_GetFldBuf(pMe, pMe->m_wSelectFld));
            if(WSTRLEN(pMe->m_pOneDial)<=3)
            {
                CLOSE_DIALOG(DLGRET_ERR);
            }
            else
            {
                CLOSE_DIALOG(DLGRET_OK);
            }
            return TRUE;
            
            break;
            
        default:
            break;
            
    }
    
    return FALSE;
} // CContApp_HandleOneDialOptDlgEvent


/*==============================================================================
函数：
       CContApp_HandleCapacityDlgEvent
说明：
       IDD_CAPACITY 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleCapacityDlgEvent( CContApp  *pMe,
                                                 AEEEvent   eCode,
                                                 uint16     wParam,
                                                 uint32     dwParam)
{
    IStatic *pStatic={0};
    RGBVAL              scrollbarFillColor  = 0;
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif

    pStatic = (IStatic*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                            IDC_CAPACITY_STATIC);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        {
            //ISTATIC_SetProperties( pStatic, ST_TRANSPARENT|ST_NOSCROLL);
            ISTATIC_SetProperties( pStatic,  ST_CENTERTEXT
                                            |ST_MIDDLETEXT
                                            |ST_UNDERLINE
                                            |ST_CENTERTITLE
                                            |ST_TRANSPARENTBACK);
            
            // set static text
            (void)CContApp_SetCapacityStatic( pMe, pStatic);
            return TRUE;
        }
        
        case EVT_DIALOG_START:
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;                        
            // Note: Fall through to the EVT_USER_REDRAW
            //lint -fallthrough
            
        case EVT_USER_REDRAW:
            {
                AEEDeviceInfo devinfo={0};
                AEERect rc={0};
                
                ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
                rc = pMe->m_rc;
                rc.dy = devinfo.cyScreen;
                rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);// + GetTitleBarHeight(pMe->m_pDisplay) );
                rc.y = 0;//GetTitleBarHeight(pMe->m_pDisplay);
                
                ISTATIC_SetRect(pStatic, &rc);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                {
                    IImage *pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
                    
                    Appscommon_ResetBackground(pMe->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &rc, 0, 0);
                    if(pImageBg != NULL)
                    {
                        IImage_Release(pImageBg);
                    }
                }
#else
                Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &rc, TRUE);
#endif
            }
        
            {
                Theme_Param_type    themeParms          = {0};
            
                Appscom_GetThemeParameters( &themeParms);
                scrollbarFillColor  =   themeParms.themeColor;
                scrollbarFillColor = IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_BACKGROUND, scrollbarFillColor);
#ifdef FEATURE_WHITE_BG
                IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);
#else
                IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
#endif
                ISTATIC_Redraw( pStatic);
            }
            //draw titlebar
            {
                AECHAR  title[32]={0};
                TitleBar_Param_type TitleBar = {0};
                (void)ISHELL_LoadResString(pMe->m_pShell, CONTAPP_RES_FILE_LANG,
                              IDS_CAPACITY, title,sizeof(title));
                TitleBar.pwszTitle = title;
                TitleBar.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER;
				#if 0
                DrawTitleBar(pMe->m_pDisplay, &TitleBar);
				#else
                if(pMe->m_pIAnn != NULL)
                {
				    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,title);
                }
				#endif
            }
            // Draw prompt bar here
            CONTAPP_DRAW_BOTTOMBAR(BTBAR_BACK);
            IDISPLAY_Update(pMe->m_pDisplay);
            IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_BACKGROUND, scrollbarFillColor);
            IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                //case AVK_INFO:
                    //CContApp_Create500Cont(pMe);                    
                case AVK_CLR:
                   // CContApp_DeleteAllCont(pMe);
                //case AVK_SELECT:
                    CLOSE_DIALOG(DLGRET_OK);
                    return TRUE;
                    
                default:
                    break;                    
            }
            break;
            
        case EVT_COMMAND:
            break;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif             
        default:
            break;            
    }
    return FALSE;
} // CContApp_HandleCapacityDlgEvent

/*==============================================================================
函数：
       CContApp_HandleEditDlgEvent
说明：
       IDD_EDIT 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleEditDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam)
{
    IMenuCtl *pMenuCtl = NULL;
    ITextCtl *pTextCtl = NULL;
    IMenuCtl *pGroupList = NULL; 
    
    uint32 dwMask;
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    MSG_FATAL("CContApp_HandleEditDlgEvent Start",0,0,0);
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg, IDC_EDIT_MENU);
    pTextCtl = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_TEXT_LOCAL_INPUT);
    pGroupList = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_GROUP_LIST);
    
    if(NULL == pMenuCtl || NULL == pTextCtl || NULL == pGroupList)
    {
        return TRUE;
    }

    MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            #ifdef FEATURE_VERSION_S1000T
            IMENUCTL_SetActive(pMenuCtl, TRUE);
            ITEXTCTL_SetActive(pTextCtl, FALSE);
            #endif
            dwMask = IDIALOG_GetProperties(pMe->m_pActiveDlg);
            dwMask |= DLG_NOT_SET_FOCUS_AUTO;
            IDIALOG_SetProperties(pMe->m_pActiveDlg, dwMask);
            #ifdef FEATURE_VERSION_S1000T
            IDIALOG_SetFocus(pMe->m_pActiveDlg, IDC_EDIT_MENU);
            #endif
            pMe->m_nInputMode =  EDIT_MENU_MODE; 

            return TRUE;
            
        case EVT_DIALOG_START:
        {
            // Set the edit menu title is first field
            AEERect rc={0};
            //AEERingerInfo  ri={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
            
            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_START",0,0,0);
			#if 0
            (void)IMENUCTL_SetTitle( pMenuCtl, NULL, 0, (AECHAR *)CContApp_GetFldBuf(pMe, 0));  
            #else
			{
				if(pMe->m_pIAnn != NULL)
                {            
				    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,(AECHAR *)CContApp_GetFldBuf(pMe, 0));
                }
			}
			#endif
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY|OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif
            // build menu
            CContApp_BuildEditMenuMenu(pMe, pMenuCtl, TRUE);

            if(pMe->m_nAddnewOrEdit == ADDOREDIT_EDIT || (TRUE == pMe->m_bInputNotComplete) )
            {
                // Update the menu item text
                //if(pMe->m_wSelectEdit != IDI_EDIT_MENU_NAME)
                {
                    if (pMe->m_pAddNewName && WSTRLEN(pMe->m_pAddNewName) != 0)
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_EDIT_MENU_NAME, NULL, 0, pMe->m_pAddNewName);
                    }
                    else if(pMe->m_wSelectEdit != IDI_EDIT_MENU_NAME)
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_EDIT_MENU_NAME, CONTAPP_RES_FILE_LANG, IDS_NAME, NULL);
                    }
                }
                
                if (pMe->m_pAddNewMobile)
                {
                    IMENUCTL_SetItemText(pMenuCtl, IDI_EDIT_MENU_MOBILE, NULL, 0, pMe->m_pAddNewMobile);
                }
                else if(pMe->m_wSelectEdit != IDI_EDIT_MENU_MOBILE)
                {
                    IMENUCTL_SetItemText(pMenuCtl, IDI_EDIT_MENU_MOBILE, CONTAPP_RES_FILE_LANG, IDS_MOBILE, NULL);
                }
                
                //when add uim card recored, following menu items have been deleted
                if(!IS_RUIM_REC(pMe->m_wEditCont))
                {  
                    //AECHAR *m_pString = NULL;
                    //int        len = 0;
                    //uint16 ResID = 0;
                    //byte *pData  =NULL;
                    
                    if (pMe->m_pAddNewHome)
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_EDIT_MENU_HOME, NULL, 0, pMe->m_pAddNewHome);
                    }
                    else
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_EDIT_MENU_HOME, CONTAPP_RES_FILE_LANG, IDS_HOME, NULL);
                    }
                    
                    if (pMe->m_pAddNewOffice)
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_EDIT_MENU_OFFICE, NULL, 0, pMe->m_pAddNewOffice);
                    }
                    else
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_EDIT_MENU_OFFICE, CONTAPP_RES_FILE_LANG, IDS_OFFICE, NULL);
                    }
                    
                    if (pMe->m_pAddNewFax)
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_EDIT_MENU_FAX, NULL, 0, pMe->m_pAddNewFax);
                    }
                    else
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_EDIT_MENU_FAX, CONTAPP_RES_FILE_LANG, IDS_FAX, NULL);
                    }
                    
                    if (pMe->m_pAddNewEMAIL)
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_EDIT_MENU_EMAIL, NULL, 0, pMe->m_pAddNewEMAIL);
                    }
                    else if(pMe->m_wSelectEdit != IDI_EDIT_MENU_EMAIL)
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_EDIT_MENU_EMAIL, CONTAPP_RES_FILE_LANG, IDS_EMAIL, NULL);
                    }
                    
                    if (pMe->m_pAddNewAddress)
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_EDIT_MENU_ADDRESS, NULL, 0, pMe->m_pAddNewAddress);
                    }
                    else if(pMe->m_wSelectEdit != IDI_EDIT_MENU_ADDRESS)
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_EDIT_MENU_ADDRESS, CONTAPP_RES_FILE_LANG, IDS_ADDRESS, NULL);
                    }
                    
                    if (pMe->m_pAddNewURL)
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_EDIT_MENU_URL, NULL, 0, pMe->m_pAddNewURL);
                    }
                    else if(pMe->m_wSelectEdit != IDI_EDIT_MENU_URL)
                    {
                        // 增加该条件是为了CContApp_HandleInputFldDlgEvent中清空再确定回来的闪烁
                        IMENUCTL_SetItemText(pMenuCtl, IDI_EDIT_MENU_URL, CONTAPP_RES_FILE_LANG, IDS_URL, NULL);
                    }
                    
                    if (pMe->m_pAddNewRemark)
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_EDIT_MENU_REMARK, NULL, 0, pMe->m_pAddNewRemark);
                    }    
                    else if(pMe->m_wSelectEdit != IDI_EDIT_MENU_REMARK)
                    {
                        // 增加该条件是为了CContApp_HandleInputFldDlgEvent中清空再确定回来的闪烁
                        IMENUCTL_SetItemText(pMenuCtl, IDI_EDIT_MENU_REMARK, CONTAPP_RES_FILE_LANG, IDS_NOTES, NULL);
                    }
                        
                    CContApp_SetGroupItemText(pMe, pMenuCtl);
    				#ifndef FEATURE_VERSION_K212
                    if (pMe->m_nRingToneID && pMe->m_nRingToneID[0] != (AECHAR)'\0')
                    {
                        AECHAR name[128]={0};
                        AECHAR *fileName=NULL;
                        char nametemp[128] = {0};
                        fileName = WSTRRCHR(pMe->m_nRingToneID, (AECHAR)'/');
                        if(NULL != fileName)
                        {
                            fileName++;
                            WSTRCPY(name, fileName);
                        }
                        else
                        {
                            WSTRCPY(name, pMe->m_nRingToneID);
                        }
                        WSTRTOSTR(name,nametemp,128);
						DBGPRINTF("nametemp====%s",nametemp);
						MEMSET(name,0,128);
						UTF8TOWSTR((byte *)nametemp,STRLEN(nametemp),name,128);
                        IMENUCTL_SetItemText(pMenuCtl, IDI_EDIT_MENU_RINGTONE, NULL, 0, name);
                    }
                    else
                    {
                        IMENUCTL_SetItemText(pMenuCtl, IDI_EDIT_MENU_RINGTONE, CONTAPP_RES_FILE_LANG, IDS_RING, NULL);
                    }
					#endif
                }
            }
            
            //IMENUCTL_SetActive(pMenuCtl, FALSE);

            //IMENUCTL_Redraw(pMenuCtl);
            CContApp_SetInputMode(pMe);
            IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectEdit);

			CContApp_ShowEditItem(pMe, pMenuCtl, pTextCtl);
			IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
			
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;
                                    
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
        {
            #ifdef FEATURE_VERSION_SKY
            {
            IImage *image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);
            Appscommon_ResetBackground(pMe->m_pDisplay,image, APPSCOMMON_BG_COLOR, 
                                                    &pMe->m_rc, 0, 0);
             if( image != NULL)
             {
               IIMAGE_Draw( image, 0, 0);
               IIMAGE_Release( image);
             }
            }
            #endif
             // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            else
            {
                // Set menu select
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectEdit);
            }
            (void)IMENUCTL_Redraw(pMenuCtl);
            // Draw prompt bar here
            if( IDI_EDIT_MENU_RINGTONE == IMENUCTL_GetSel(pMenuCtl))
            {
                CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_OK_BACK);
            }
            else
            {
                //CONTAPP_DRAW_BOTTOMBAR(BTBAR_SAVE_BACK);
               #ifndef FEATURE_ALL_KEY_PAD
               if(WSTRLEN( ITEXTCTL_GetTextPtr(pTextCtl)) > 0)
               {
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_SAVE_DELETE);
               }
               else
               #endif
               {
                   CONTAPP_DRAW_BOTTOMBAR(BTBAR_SAVE_BACK);
               }
            }

            if(IDI_EDIT_MENU_NAME == pMe->m_wSelectEdit)
            {
                pMe->m_nFldInputID = AEE_ADDRFIELD_NAME;				
                CContApp_ShowEditItem(pMe, pMenuCtl, pTextCtl);
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            }
            else if(IDI_EDIT_MENU_RINGTONE == pMe->m_wSelectEdit)
            {
                IMENUCTL_SetActive(pMenuCtl, TRUE);
            }
            else
            {
                ITEXTCTL_SetActive(pTextCtl, TRUE);
            }
            //IMENUCTL_Redraw(pMenuCtl);
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
        }

        case EVT_DIALOG_END:
        {
             // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                // 中断处理
                if(EDIT_GROUP == pMe->m_nInputMode)
                {
                    CContApp_SetEditGroup(pMe, pMenuCtl, FALSE, IMENUCTL_GetSel(pGroupList));
                }
                else if( LOCAL_NUMBER_INPUT == pMe->m_nInputMode )
                {
                    CContApp_SaveLocal_Input(pMe, pTextCtl);
                }
        
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            pMe->m_bInputNotComplete = FALSE;
            #ifdef FEATURE_LCD_TOUCH_ENABLE
            TSIM_NumberKeypad(FALSE);
            #endif
        }
        return TRUE;

        /*下面有对4个数字域 m_nFldInputID 的邋付值， 其他的文字域在按任意键邋(1 - #)的时候编辑*/
        case EVT_CTL_SEL_CHANGED : 
            if(EDIT_GROUP == pMe->m_nInputMode)
            {
                return TRUE;
            }
            
            pMe->m_wSelectEdit = wParam;
            CContApp_SetInputMode(pMe);
            
            switch(wParam)
            {
                case IDI_EDIT_MENU_NAME:
                     // Set EDIT field
                    pMe->m_nFldInputID = AEE_ADDRFIELD_NAME;
                    break;
                    
                case IDI_EDIT_MENU_MOBILE:
                     // Set EDIT field
                    pMe->m_nFldInputID = AEE_ADDRFIELD_PHONE_GENERIC;
                    break;
                    
                case IDI_EDIT_MENU_HOME:
                    // Set EDIT field
                    pMe->m_nFldInputID = AEE_ADDRFIELD_PHONE_HOME;
                    break;
                    
                case IDI_EDIT_MENU_OFFICE:
                    // Set EDIT field
                    pMe->m_nFldInputID = AEE_ADDRFIELD_PHONE_WORK;
                    break;
                    
                case IDI_EDIT_MENU_FAX:
                    // Set EDIT field
                    pMe->m_nFldInputID = AEE_ADDRFIELD_PHONE_FAX;
                    break;

                case IDI_EDIT_MENU_EMAIL:
                    // Set EDIT field
                    pMe->m_nFldInputID = AEE_ADDRFIELD_EMAIL;
                    break;    

                case IDI_EDIT_MENU_ADDRESS:
                    // Set EDIT field
                    pMe->m_nFldInputID = AEE_ADDRFIELD_ADDRESS;
                    break;    

                case IDI_EDIT_MENU_URL:
                    // Set EDIT field
                    pMe->m_nFldInputID = AEE_ADDRFIELD_URL;
                    break;    

                case IDI_EDIT_MENU_REMARK:
                    // Set EDIT field
                    pMe->m_nFldInputID = AEE_ADDRFIELD_NOTES;
                    break;   
                    
                case IDI_EDIT_MENU_GROUP:
                   // pMe->m_nInputMode = EDIT_GROUP;
                    CContApp_BuildGroupList(pMe, pGroupList);
                    CContApp_ShowGroupSelect(pMe, pMenuCtl, pGroupList);
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_SAVE_BACK);
                    break;

                case IDI_EDIT_MENU_RINGTONE:
                    //pMe->m_nInputMode = EDIT_MENU_MODE;
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_OK_BACK);
                    IDISPLAY_Update(pMe->m_pDisplay);  
                    break;
                    
                default:
                    pMe->m_nInputMode = EDIT_MENU_MODE;
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_SAVE_BACK);
                    return TRUE;
            }

            if(wParam !=  IDI_EDIT_MENU_GROUP && wParam != IDI_EDIT_MENU_RINGTONE)
            {            	
                CContApp_ShowEditItem(pMe, pMenuCtl, pTextCtl);
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            }

            return TRUE;

        case EVT_KEY_PRESS:
            {
                switch(wParam)
                {
                    case AVK_UP:
                    case AVK_DOWN:
                    {                   
                        if(EDIT_GROUP == pMe->m_nInputMode)
                        {                    
                            CContApp_SetEditGroup(pMe, pMenuCtl, FALSE, IMENUCTL_GetSel(pGroupList));
                            IMENUCTL_SetActive(pGroupList, FALSE);
                            IMENUCTL_SetActive(pMenuCtl, TRUE);
                        }

						//Add By zzg 2011_12_15
						if ((LOCAL_NUMBER_INPUT == pMe->m_nInputMode) 
							|| (EDIT_MENU_MODE == pMe->m_nInputMode)
							|| (EDIT_GROUP == pMe->m_nInputMode))
						{
		                	if (CContApp_CheckNumberIsInValid(pMe, pTextCtl))
		                	{
		                		 pMe->m_bNumberInvalid = TRUE;		
		                		 CLOSE_DIALOG(DLGRET_NUM_UNAVAILD);
								 return TRUE;
							}
							else
							{
								DBGPRINTF("***zzg CContApp_FixPlusNumber 4***");
								CContApp_FixPlusNumber(pMe);	
							}
						}
	                	//Add End
                	
                        if(OPT_TEXT_INPUT == pMe->m_nInputMode || LOCAL_NUMBER_INPUT == pMe->m_nInputMode)
                        {
                            if(TRUE == CContApp_SaveLocal_Input(pMe, pTextCtl))
                            {
                                if (AEE_ADDRFIELD_NONE != pMe->m_nFldInputID)
                                {
                            		IMENUCTL_SetActive(pMenuCtl, TRUE);
                            		IDIALOG_SetFocus(pMe->m_pActiveDlg, IDC_ADDNEW_MENU);
                                    CContApp_SetRecByFLDID(pMe, pMe->m_nFldInputID);
                                } 
                                
                                CContApp_SetMenuItemText(pMe, pMenuCtl, pMe->m_wSelectEdit);
                            }
                            else
                            {   
                                return TRUE;
                            }
                            
                            ITEXTCTL_SetActive(pTextCtl, FALSE);
                            IMENUCTL_SetActive(pMenuCtl, TRUE);
                            IDIALOG_SetFocus(pMe->m_pActiveDlg, IDC_EDIT_MENU);
                        }
                        
                    }
                    return TRUE;

                    case AVK_0:
                    case AVK_1:
                    case AVK_2:
                    case AVK_3:
                    case AVK_4:
                    case AVK_5:
                    case AVK_6:
                    case AVK_7:
                    case AVK_8:
                    case AVK_9:
                    case AVK_Q:
		            case AVK_W:
		            case AVK_E:
		            case AVK_R:
		            case AVK_A:
		            case AVK_S:
		            case AVK_D:
		            case AVK_F:
		            case AVK_Z:
		            case AVK_X:
		            case AVK_C:
                    case AVK_T:
                    case AVK_Y:
                    case AVK_U:
                    case AVK_I:
                    case AVK_O:
                    case AVK_P:
                    case AVK_G:
                    case AVK_H:
                    case AVK_J:
                    case AVK_K:
                    case AVK_L:
                    case AVK_V:
                    case AVK_B:
                    case AVK_N:
                    case AVK_M:
                    case AVK_STAR:
                    case AVK_POUND:
                    {
                        if(OPT_TEXT_INPUT != pMe->m_nInputMode)
                        {
                            return TRUE;
                        }

                        CContApp_SaveLocal_Input(pMe, pTextCtl);// 本地是可以先删除一些字符后，在按任意键进去编辑的
                        
                        CLOSE_DIALOG(DLGRET_EDIT);
                    }
                    return TRUE;

                    default:
                        break;
                }
            }
            break;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                {   
                    FREEIF(pMe->m_nRingToneID);
                    pMe->m_nGroupCat = AEE_ADDR_CAT_OTHER;
                    pMe->m_nFldInputID = AEE_ADDRFIELD_NONE;
                    pMe->m_wSelectEdit = IDI_EDIT_MENU_NAME;
                    
                    CLOSE_DIALOG(DLGRET_CANCELED);
                }
                return TRUE;

                            
#ifdef FEATURE_GIVEUP_EDIT
                case AVK_CONTACTS_GIVEUP_EDIT:
                    if( LOCAL_NUMBER_INPUT == pMe->m_nInputMode )
                    {
                        CContApp_SaveLocal_Input(pMe, pTextCtl);
                        if (AEE_ADDRFIELD_NONE != pMe->m_nFldInputID)
                        {
                            CContApp_SetRecByFLDID(pMe, pMe->m_nFldInputID);
                        } 
                    }
                    CLOSE_DIALOG(DLGRET_GIVEUP_EDIT);
                    return TRUE;
#endif
                case AVK_INFO:
                case AVK_SELECT:
                {
					//Add By zzg 2011_12_15
                	if (CContApp_CheckNumberIsInValid(pMe, pTextCtl)&&((pMe->m_nFldInputID == AEE_ADDRFIELD_PHONE_GENERIC)
							||(pMe->m_nFldInputID == AEE_ADDRFIELD_PHONE_HOME)||(pMe->m_nFldInputID == AEE_ADDRFIELD_PHONE_WORK)
							||(pMe->m_nFldInputID == AEE_ADDRFIELD_PHONE_FAX)))
                	{
                		 pMe->m_bNumberInvalid = TRUE;		
                		 CLOSE_DIALOG(DLGRET_NUM_UNAVAILD);
						 return TRUE;
					}
					else
					{
						DBGPRINTF("***zzg CContApp_FixPlusNumber 5***");
						CContApp_FixPlusNumber(pMe);	
					}
                	//Add End
                    	
                    if( LOCAL_NUMBER_INPUT == pMe->m_nInputMode || OPT_TEXT_INPUT == pMe->m_nInputMode)
                    {
                        if(TRUE != CContApp_SaveLocal_Input(pMe, pTextCtl))
                        {
                            return TRUE;
                        }
                        if (AEE_ADDRFIELD_NONE != pMe->m_nFldInputID)
                        {
                            CContApp_SetRecByFLDID(pMe, pMe->m_nFldInputID);
                        } 
                    }
                    else if( EDIT_GROUP == pMe->m_nInputMode)
                    {
                        // 不需要返回菜单，直接弹出DONE， 传第三个参数TRUE
                        CContApp_SetEditGroup(pMe, pMenuCtl, TRUE, IMENUCTL_GetSel(pGroupList));
                    }
                    
                    CLOSE_DIALOG(DLGRET_OK);
                }
                return TRUE;

                default: 
                    break;
            }
            return TRUE;
            
            case EVT_KEY_RELEASE:
            {
                if(pMe->m_nInputMode == LOCAL_NUMBER_INPUT || pMe->m_nInputMode == OPT_TEXT_INPUT )
               {
               	   #ifndef FEATURE_ALL_KEY_PAD
                   if ( WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) > 0 && ITEXTCTL_GetT9End(pTextCtl) != TC_CURSORSTART )
                   {
                       // Save      Delete
                       CONTAPP_DRAW_BOTTOMBAR(BTBAR_SAVE_DELETE);
                   }
                   else
                   #endif
                   {
                       // Save      Back
                       CONTAPP_DRAW_BOTTOMBAR(BTBAR_SAVE_BACK);            
                   }   
                   IDISPLAY_Update(pMe->m_pDisplay);  
               }
            }
            return TRUE;

        case EVT_COMMAND:
        {
            pMe->m_wSelectEdit = wParam;

            switch(wParam)
            {
                case IDS_GROUP_NONE_STRING:
                case IDS_GROUP_VIP:
                case IDS_GROUP_HOME:
                case IDS_GROUP_WORK:
                case IDS_GROUP_FRIEND:
                case IDS_GROUP_PERSONAL:
                case IDS_GROUP_BUSINESS:
                {
                    MSG_FATAL("CContApp_HandleEditDlgEvent wParam=%d",wParam,0,0);
                    if( EDIT_GROUP == pMe->m_nInputMode)
                    {
                        // 不需要返回菜单，直接弹出DONE， 传第三个参数TRUE
                        CContApp_SetEditGroup(pMe, pMenuCtl, TRUE, wParam);
                        CLOSE_DIALOG(DLGRET_OK);
                    }
                    return TRUE;
                }
                break;

                case IDI_EDIT_MENU_RINGTONE:
                {
                    ISoundMenu  *pSoundMenu = NULL;
                    pMe->m_nFldInputID = AEE_ADDRFIELD_RINGTONE;
                    
                    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_APP_SOUNDMENU, (void **)&pSoundMenu))
                    {
                        return FALSE;
                    }

                    if(pMe->m_nRingToneID == NULL)
                    {
                        pMe->m_nRingToneID = (AECHAR*)MALLOC(MAX_FILE_NAME * sizeof(AECHAR));
                    }
                    if(pMe->m_nRingToneID)
                    {
                        ISoundMenu_Ringer_List(pSoundMenu, (void*)pMe->m_nRingToneID);
                    }
                    
                    if(pSoundMenu)
                    {
                        ISoundMenu_Release(pSoundMenu);
                        pSoundMenu = NULL;
                    }
                    return TRUE;
                 }

                default:
                    break;
                    //CLOSE_DIALOG(DLGRET_OK);
                    //return TRUE;
            }
        }
        return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
        case EVT_PEN_DOWN:
            {
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);
                if(ITEXTCTL_IsActive(pTextCtl))
				{
				   ITEXTCTL_GetRect(pTextCtl,&rc); 
                   if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
			        {
                     if(OPT_TEXT_INPUT != pMe->m_nInputMode)
                     {
                         TSIM_NumberKeypad(TRUE);
                         return TRUE;
                     }                    
                     CContApp_SaveLocal_Input(pMe, pTextCtl);// 本地是可以先删除一些字符后，在按任意键进去编辑的
                     CLOSE_DIALOG(DLGRET_EDIT);
				    }         
                }
            	
            }
            break;
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}
                #if 1
				else
				{

					if(ITEXTCTL_IsActive(pTextCtl))
					{
							ITEXTCTL_GetRect(pTextCtl,&rc); 
                            MSG_FATAL("CONTAPP_PT_IN_RECT------0",0,0,0);
							if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
							{
                                ;
							}
							else
							{
								/*if((!IMENUCTL_IsActive(pMenuCtl)))
									IMENUCTL_SetActive(pMenuCtl, TRUE);
                                //ITEXTCTL_HandleEvent(pTextCtl,EVT_KEY,AVK_UP,0);
                                MSG_FATAL("pMenuCtleCode=%x---wParam%x---dwParam%x",eCode,wParam,dwParam);
								IMENUCTL_HandleEvent(pMenuCtl,eCode,wParam,dwParam);  */
                                #if 1
                                 if(EDIT_GROUP == pMe->m_nInputMode)
                                    {                    
                                        CContApp_SetEditGroup(pMe, pMenuCtl, FALSE, IMENUCTL_GetSel(pGroupList));
                                        IMENUCTL_SetActive(pGroupList, FALSE);
                                        IMENUCTL_SetActive(pMenuCtl, TRUE);
                                    }
                                    if(OPT_TEXT_INPUT == pMe->m_nInputMode || LOCAL_NUMBER_INPUT == pMe->m_nInputMode)
                                    {
                                        if(TRUE == CContApp_SaveLocal_Input(pMe, pTextCtl))
                                        {
                                            MSG_FATAL("CContApp_SaveLocal_Input=TRUE",0,0,0);
                                           // itemTextBuf = pMe->m_pFldInputBuf;
                                            if (AEE_ADDRFIELD_NONE != pMe->m_nFldInputID)
                                            {
                                                CContApp_SetRecByFLDID(pMe, pMe->m_nFldInputID);
                                            } 
                                            
                                            CContApp_SetMenuItemText(pMe, pMenuCtl, pMe->m_wSelectEdit);
                                        }
                                        else
                                        {   
                                            MSG_FATAL("CContApp_SaveLocal_Input=FALSE",0,0,0);
                                            return TRUE;
                                        }
                                        
                                        ITEXTCTL_SetActive(pTextCtl, FALSE);
                                        IMENUCTL_SetActive(pMenuCtl, TRUE);
                                        IDIALOG_SetFocus(pMe->m_pActiveDlg, IDC_ADDNEW_MENU);
                                    }
                                    ITEXTCTL_SetActive(pTextCtl, FALSE);
                                #endif
							}

					}
					else if(IMENUCTL_IsActive(pGroupList))
					{
							IMENUCTL_GetRect(pGroupList,&rc); 
							if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
							{
								;
							}
							else
							{
                               if(CONTAPP_PT_IN_RECT(wXPos,wYPos,pMe->M_RGroupRect))
                                {
                                    IMENUCTL_HandleEvent(pGroupList,EVT_KEY,AVK_RIGHT,0); 
                                }
                                else if(CONTAPP_PT_IN_RECT(wXPos,wYPos,pMe->M_LGroupRect))
                                {
                                    IMENUCTL_HandleEvent(pGroupList,EVT_KEY,AVK_LEFT,0); 
                                }
                                else
                                {
                              #if 1
                                 if(EDIT_GROUP == pMe->m_nInputMode)
                                    {                    
                                        CContApp_SetEditGroup(pMe, pMenuCtl, FALSE, IMENUCTL_GetSel(pGroupList));
                                        IMENUCTL_SetActive(pGroupList, FALSE);
                                        IMENUCTL_SetActive(pMenuCtl, TRUE);
                                    }
                                    if(OPT_TEXT_INPUT == pMe->m_nInputMode || LOCAL_NUMBER_INPUT == pMe->m_nInputMode)
                                    {
                                        if(TRUE == CContApp_SaveLocal_Input(pMe, pTextCtl))
                                        {
                                            MSG_FATAL("CContApp_SaveLocal_Input=TRUE",0,0,0);
                                           // itemTextBuf = pMe->m_pFldInputBuf;
                                            if (AEE_ADDRFIELD_NONE != pMe->m_nFldInputID)
                                            {
                                                CContApp_SetRecByFLDID(pMe, pMe->m_nFldInputID);
                                            } 
                                            
                                            CContApp_SetMenuItemText(pMe, pMenuCtl, pMe->m_wSelectEdit);
                                        }
                                        else
                                        {   
                                            MSG_FATAL("CContApp_SaveLocal_Input=FALSE",0,0,0);
                                            return TRUE;
                                        }
                                        
                                        ITEXTCTL_SetActive(pTextCtl, FALSE);
                                        IMENUCTL_SetActive(pMenuCtl, TRUE);
                                        IDIALOG_SetFocus(pMe->m_pActiveDlg, IDC_ADDNEW_MENU);
                                    }
                                    IMENUCTL_SetActive(pGroupList, FALSE);
                               #endif
                                }
							}

					}
					else
					{
						/* if((!IMENUCTL_IsActive(pMenuCtl)))
							IMENUCTL_SetActive(pMenuCtl, TRUE);
                        MSG_FATAL("pMenuCtleCode=%x---wParam%x---dwParam%x",eCode,wParam,dwParam);

						IMENUCTL_HandleEvent(pMenuCtl,eCode,wParam,dwParam);
						if((!IMENUCTL_IsActive(pGroupList)))
							IMENUCTL_SetActive(pGroupList, TRUE);
                        MSG_FATAL("pMenuCtleCode=%x---wParam%x---dwParam%x",eCode,wParam,dwParam);

						IMENUCTL_HandleEvent(pGroupList,eCode,wParam,dwParam); */
						
#if 1
                         if(EDIT_GROUP == pMe->m_nInputMode)
                            {                    
                                CContApp_SetEditGroup(pMe, pMenuCtl, FALSE, IMENUCTL_GetSel(pGroupList));
                                IMENUCTL_SetActive(pGroupList, FALSE);
                                IMENUCTL_SetActive(pMenuCtl, TRUE);
                            }
                            if(OPT_TEXT_INPUT == pMe->m_nInputMode || LOCAL_NUMBER_INPUT == pMe->m_nInputMode)
                            {
                                if(TRUE == CContApp_SaveLocal_Input(pMe, pTextCtl))
                                {
                                    MSG_FATAL("CContApp_SaveLocal_Input=TRUE",0,0,0);
                                   // itemTextBuf = pMe->m_pFldInputBuf;
                                    if (AEE_ADDRFIELD_NONE != pMe->m_nFldInputID)
                                    {
                                        CContApp_SetRecByFLDID(pMe, pMe->m_nFldInputID);
                                    } 
                                    
                                    CContApp_SetMenuItemText(pMe, pMenuCtl, pMe->m_wSelectEdit);
                                }
                                else
                                {   
                                    MSG_FATAL("CContApp_SaveLocal_Input=FALSE",0,0,0);
                                    return TRUE;
                                }
                                
                                ITEXTCTL_SetActive(pTextCtl, FALSE);
                                IMENUCTL_SetActive(pMenuCtl, TRUE);
                                IDIALOG_SetFocus(pMe->m_pActiveDlg, IDC_ADDNEW_MENU);
                            }
                            ITEXTCTL_SetActive(pTextCtl, FALSE);
                            IMENUCTL_SetActive(pGroupList, FALSE);
#endif
					}
                    IMENUCTL_HandleEvent(pMenuCtl,eCode,wParam,dwParam); 
				}
                #endif
			}
			break;
#endif         
        default:
            break;
    }

    return FALSE;
} // CContApp_HandleEditDlgEvent

#ifdef FEATURE_RUIM_PHONEBOOK
/*==============================================================================
函数：
       CContApp_HandleSettingDlgEvent
说明：
       IDD_SETTING 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleSettingDlgEvent( CContApp  *pMe,
                                                AEEEvent   eCode,
                                                uint16     wParam,
                                                uint32     dwParam)
{
    IMenuCtl *pMenuCtl;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_SETTING_MENU);
                                              
    MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);

            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_START",0,0,0);

            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif
            MENU_SET_COMICON(pMenuCtl);
            
            IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectSetting);
            {
                AECHAR WTitle[40] = {0};
                //IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
                (void)ISHELL_LoadResString(pMe->m_pShell,
                CONTAPP_RES_FILE_LANG,                                
                IDS_SETTING,
                WTitle,
                sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
            }

               
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
                                    
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            
            // Draw prompt bar here
            CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                    
                default:
                    break;
                    
            }
            break;
            
        case EVT_COMMAND:
            pMe->m_wSelectSetting = wParam;
            switch(wParam)
            {
                case IDI_SETTING_MENU_SAVETO:
                    CLOSE_DIALOG(DLGRET_SAVETO);
                    return TRUE;
                    
                case IDI_SETTING_MENU_VIEWTYPE:
                    CLOSE_DIALOG(DLGRET_VIEWTYPE);
                    return TRUE;
                    
                case IDI_SETTING_MENU_COPYMOVE:
                    CLOSE_DIALOG(DLGRET_COPY);
                    return TRUE;

                case IDI_SETTING_MENU_MEMORY:
                    CLOSE_DIALOG(DLGRET_CHECKCAPACITY);
                    return TRUE;                    
                
                default:
                    break;
            }
            break;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif             
        default:
            break;
            
    }

    return FALSE;
} // CContApp_HandleSettingDlgEvent

#endif /* FEATURE_RUIM_PHONEBOOK */

/*==============================================================================
函数：
       CContApp_HandleInputDlgEvent
说明：
       IDD_INPUT 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleInputDlgEvent( CContApp  *pMe,
                                              AEEEvent   eCode,
                                              uint16     wParam,
                                              uint32     dwParam)
{
    //static char   *m_strPhonePWD = NULL;
    int            nLen = 0;
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            if(NULL == pMe->m_strPhonePWD)
            {
                pMe->m_strPhonePWD = (char *)MALLOC((OEMNV_LOCKCODE_MAXLEN + 1)* sizeof(char));
            }
			 {
			 	AECHAR  text[32] = {0}; 
                #ifdef FEATURE_VERSION_C337
				(void)ISHELL_LoadResString(pMe->m_pShell, 
                                            CONTAPP_RES_FILE_LANG,
                                            IDS_PHONEBOOK, 
                                            text,
                                            sizeof(text));
				#else
                (void)ISHELL_LoadResString(pMe->m_pShell, 
                                            CONTAPP_RES_FILE_LANG,
                                            IDS_APPLET, 
                                            text,
                                            sizeof(text));
				#endif
				
                if(pMe->m_pIAnn != NULL)
                {
				    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,text);
                }
            }
            return TRUE;
            
        case EVT_DIALOG_START:
			 // 绘制相关信息
            {
                AECHAR  wstrDisplay[OEMNV_LOCKCODE_MAXLEN+1] = {0};
                char    strDisplay[OEMNV_LOCKCODE_MAXLEN+1] = {0};
                AECHAR  text[32] = {0};
                int xOffset = 5;
                RGBVAL nOldFontColor;
                TitleBar_Param_type  TitleBar_Param = {0};
                
                // 先清屏
#ifdef FEATURE_CARRIER_CHINA_VERTU
                {
                    IImage *pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SECURITY_BACKGROUND);
                    
                    Appscommon_ResetBackground(pMe->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &pMe->m_rc, 0, 0);
                    if(pImageBg != NULL)
                    {
                        IImage_Release(pImageBg);
                    }
                }
#else
                Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
#endif
                IDISPLAY_FillRect  (pMe->m_pDisplay,&pMe->m_rc, RGB_BLACK);
                    
                // 画标题条
                #ifdef FEATURE_VERSION_C337
				(void)ISHELL_LoadResString(pMe->m_pShell, 
                                            CONTAPP_RES_FILE_LANG,
                                            IDS_PHONEBOOK, 
                                            text,
                                            sizeof(text));
				#else
                (void)ISHELL_LoadResString(pMe->m_pShell, 
                                            CONTAPP_RES_FILE_LANG,
                                            IDS_APPLET, 
                                            text,
                                            sizeof(text));
				#endif
				
                TitleBar_Param.pwszTitle = text;
                TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
				#if 0
                DrawTitleBar(pMe->m_pDisplay, &TitleBar_Param);
				//#else
                if(pMe->m_pIAnn != NULL)
                {
				    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,text);
                }
				#endif
                    
                (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                CONTAPP_RES_FILE_LANG,
                                                IDS_PASSWORD, 
                                                text,
                                                sizeof(text));
                nOldFontColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                    AEE_FONT_NORMAL, //AEE_FONT_BOLD, 
                                    text,
                                    -1, 
                                    xOffset, 
                                    TITLEBAR_HEIGHT + MENUITEM_HEIGHT*1/2,
                                    NULL, 
                                    IDF_TEXT_TRANSPARENT);
                
                // 绘制输入
                nLen = (pMe->m_strPhonePWD == NULL)?(0):(STRLEN(pMe->m_strPhonePWD));
                MEMSET(strDisplay, '*', nLen);
                strDisplay[nLen] = '|';
                strDisplay[nLen + 1] = '\0';
                (void) STRTOWSTR(strDisplay, wstrDisplay, sizeof(wstrDisplay));
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                AEE_FONT_NORMAL, //AEE_FONT_BOLD, 
                                wstrDisplay,
                                -1, 
                                2*xOffset, 
                                TITLEBAR_HEIGHT + MENUITEM_HEIGHT*3/2,
                                NULL, 
                                IDF_TEXT_TRANSPARENT);
                (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, nOldFontColor);
            
                // 绘制底条提示
                if (nLen > 3)
                {// 确定-----删除
                	#ifdef FEATURE_ALL_KEY_PAD
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_DELETE)
                    #else
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_BACK)
                    #endif
                }
                else if(nLen > 0)
                {// 删除
                	#ifdef FEATURE_ALL_KEY_PAD
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_DELETE)
                    #else
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_CANCEL)
                    #endif
                }
                else
                {// 取消
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_CANCEL)
                }
            }
            
            // 更新显示
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);  
            (void) ISHELL_PostEvent(pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    NULL,
                                    NULL);

            return TRUE;
            
        case EVT_USER_REDRAW:
            // 绘制相关信息
            {
                AECHAR  wstrDisplay[OEMNV_LOCKCODE_MAXLEN+1] = {0};
                char    strDisplay[OEMNV_LOCKCODE_MAXLEN+1] = {0};
                AECHAR  text[32] = {0};
                int xOffset = 5;
                RGBVAL nOldFontColor;
                TitleBar_Param_type  TitleBar_Param = {0};
                
                // 先清屏
#ifdef FEATURE_CARRIER_CHINA_VERTU
                {
                    IImage *pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SECURITY_BACKGROUND);
                    
                    Appscommon_ResetBackground(pMe->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &pMe->m_rc, 0, 0);
                    if(pImageBg != NULL)
                    {
                        IImage_Release(pImageBg);
                    }
                }
#else
                Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
#endif
                IDISPLAY_FillRect  (pMe->m_pDisplay,&pMe->m_rc, RGB_BLACK);
                    
                // 画标题条
                #ifdef FEATURE_VERSION_C337
				(void)ISHELL_LoadResString(pMe->m_pShell, 
                                            CONTAPP_RES_FILE_LANG,
                                            IDS_PHONEBOOK, 
                                            text,
                                            sizeof(text));
				#else
                (void)ISHELL_LoadResString(pMe->m_pShell, 
                                            CONTAPP_RES_FILE_LANG,
                                            IDS_APPLET, 
                                            text,
                                            sizeof(text));
				#endif
				
                TitleBar_Param.pwszTitle = text;
                TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
				#if 0
                DrawTitleBar(pMe->m_pDisplay, &TitleBar_Param);
				//#else
                if(pMe->m_pIAnn != NULL)
                {
				    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,text);
                }
				#endif
                    
                (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                CONTAPP_RES_FILE_LANG,
                                                IDS_PASSWORD, 
                                                text,
                                                sizeof(text));
                nOldFontColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                    AEE_FONT_NORMAL, //AEE_FONT_BOLD, 
                                    text,
                                    -1, 
                                    xOffset, 
                                    TITLEBAR_HEIGHT + MENUITEM_HEIGHT*1/2,
                                    NULL, 
                                    IDF_TEXT_TRANSPARENT);
                
                // 绘制输入
                nLen = (pMe->m_strPhonePWD == NULL)?(0):(STRLEN(pMe->m_strPhonePWD));
                MEMSET(strDisplay, '*', nLen);
                strDisplay[nLen] = '|';
                strDisplay[nLen + 1] = '\0';
                (void) STRTOWSTR(strDisplay, wstrDisplay, sizeof(wstrDisplay));
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                AEE_FONT_NORMAL, //AEE_FONT_BOLD, 
                                wstrDisplay,
                                -1, 
                                2*xOffset, 
                                TITLEBAR_HEIGHT + MENUITEM_HEIGHT*3/2,
                                NULL, 
                                IDF_TEXT_TRANSPARENT);
                (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, nOldFontColor);
            
                // 绘制底条提示
                if (nLen > 3)
                {// 确定-----删除
                	#ifdef FEATURE_ALL_KEY_PAD
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_DELETE)
                    #else
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_BACK)
                    #endif
                }
                else if(nLen > 0)
                {// 删除
                	#ifdef FEATURE_ALL_KEY_PAD
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_DELETE)
                    #else
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_CANCEL)
                    #endif
                }
                else
                {// 取消
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_CANCEL)
                }
            }
            
            // 更新显示
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);  
            return TRUE;
            
        case EVT_DIALOG_END:
            if(!pMe->m_bSuspending)
            {
                FREEIF(pMe->m_strPhonePWD);
            }
            return TRUE;

        case EVT_KEY:
            {
                char  chEnter = 0;
                int   nLen = 0;
                boolean bRedraw = FALSE;
                
                switch (wParam)
                {
                    case AVK_0:
                    case AVK_1:
                    case AVK_2:
                    case AVK_3:
                    case AVK_4:
                    case AVK_5:
                    case AVK_6:
                    case AVK_7:
                    case AVK_8:
                    case AVK_9:
                        chEnter = '0' + (wParam - AVK_0);
                        break;

                    case AVK_STAR:
                        chEnter = '*';
                        break;
 
                    case AVK_POUND:
                        chEnter = '#';
                        break;
                        
                    case AVK_CLR:
                        chEnter = 0;
                        #ifdef FEATURE_ALL_KEY_PAD  //add by yangdecai 2010-11-29
                        if(dwParam == 0)
                        {
                        	CLOSE_DIALOG(DLGRET_CANCELED)
                            return TRUE;
                        }
                        else
                        {
                        	if (pMe->m_strPhonePWD == NULL || STRLEN(pMe->m_strPhonePWD) == 0)
                        	{
                            	CLOSE_DIALOG(DLGRET_CANCELED)
                            	return TRUE;
                        	}
                        }
                        #else
                        if (pMe->m_strPhonePWD == NULL || STRLEN(pMe->m_strPhonePWD) == 0)
                        {
                            CLOSE_DIALOG(DLGRET_CANCELED)
                            return TRUE;
                        }
                        #endif
                        break;

                    case AVK_DEL:    
                        chEnter = 0;
#ifdef FEATURE_ALL_KEY_PAD 
                        if (pMe->m_strPhonePWD == NULL || STRLEN(pMe->m_strPhonePWD) == 0)
                        {
                            CLOSE_DIALOG(DLGRET_CANCELED)
                            return TRUE;
                        }
#endif
                    break;
                        
                    case AVK_SELECT:
                    case AVK_INFO:
                        if (pMe->m_strPhonePWD == NULL|| STRLEN(pMe->m_strPhonePWD) < 4)
                        {
                            return TRUE;
                        }
                        else
                        {
                            uint16 wPWD=0;
                            
                            (void) ICONFIG_GetItem(pMe->m_pConfig, 
                                                   CFGI_PHONE_PASSWORD,
                                                   &wPWD,
                                                   sizeof(uint16));
                        // pMe->m_pFldInputBuf = (AECHAR*)m_strPhonePWD;
                            if (wPWD == EncodePWDToUint16(pMe->m_strPhonePWD))
                            {// 密码符合
                                CLOSE_DIALOG(DLGRET_OK)
                            }
                            else
                            {// 密码错误
                                CLOSE_DIALOG(DLGRET_INPUT_NULL)
                            }
                        }
                        return TRUE;
                        
                    default:
                        return TRUE;
                }
                
                nLen = (pMe->m_strPhonePWD == NULL)?(0):(STRLEN(pMe->m_strPhonePWD));
                if (chEnter == 0)
                {// 删除字符
                    if (nLen > 0)
                    {
                        bRedraw = TRUE;
                        pMe->m_strPhonePWD[nLen-1] = chEnter;
                    }
                }
                else if (nLen < OEMNV_LOCKCODE_MAXLEN)
                {
                    pMe->m_strPhonePWD[nLen] = chEnter;
                    nLen++;
                    pMe->m_strPhonePWD[nLen] = 0;
                    bRedraw = TRUE;
                }
                
                if (bRedraw)
                {
                    (void) ISHELL_PostEvent(pMe->m_pShell,
                                            AEECLSID_APP_CONTACT,
                                            EVT_USER_REDRAW,
                                            NULL,
                                            NULL);
                }
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif             
        default:
            break;
    }
    
    return FALSE;
} // CContApp_HandleInputDlgEvent


/*==============================================================================
函数：
       CContApp_HandleSearchDlgEvent
说明：
       IDD_SEARCH 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleSearchDlgEvent( CContApp  *pMe,
                                                AEEEvent   eCode,
                                                uint16     wParam,
                                                uint32     dwParam)
{
    IMenuCtl *pMenuCtl;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_SEARCH_MENU);
                                              
    MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect        rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
           
            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_START",0,0,0);
            
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif
            {
                AECHAR WTitle[40] = {0};
                //IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);

				#ifdef FEATURE_VERSION_C337
				(void)ISHELL_LoadResString(pMe->m_pShell,
							                CONTAPP_RES_FILE_LANG,                                
							                IDS_PHONEBOOK,
							                WTitle,
							                sizeof(WTitle));
				#else
                (void)ISHELL_LoadResString(pMe->m_pShell,
							                CONTAPP_RES_FILE_LANG,                                
							                IDS_APPLET,
							                WTitle,
							                sizeof(WTitle));
				#endif
                if(pMe->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
            }

            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
                                    
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            else
            {
                // Set menu select
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectFldOpts);
            }
            // Draw prompt bar here
            CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                    
                default:
                    break;
                    
            }
            break;
            
        case EVT_COMMAND:
            pMe->m_wSelectFldOpts = wParam;
            switch(wParam)
            {
                case IDI_SEARCH_MENU_NAME:
                    CLOSE_DIALOG(DLGRET_SEARCH_NAME);
                    return TRUE;

                case IDI_SEARCH_MENU_GROUP:
                    CLOSE_DIALOG(DLGRET_SEARCH_GROUP);
                    return TRUE;
     
                default:
                    break;
            }
            break;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif             
        default:
            break;
            
    }

    return FALSE;
} // CContApp_HandleFldOptsDlgEvent

/*==============================================================================
函数：
       CContApp_HandleManagementDlgEvent
说明：
       IDD_MANAGEMENT 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleManagementDlgEvent( CContApp  *pMe,
                                                AEEEvent   eCode,
                                                uint16     wParam,
                                                uint32     dwParam)
{
    IMenuCtl *pMenuCtl;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_MENU_CONTACTS_MANAGEMENT);
                                              
    MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect        rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
           
            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_START",0,0,0);
            
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL | MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif
			#if 0
            IMENUCTL_SetTitle( pMenuCtl,CONTAPP_RES_FILE_LANG,IDS_CONTACTS_MANAGEMENT,NULL);
			#else
			{
				AECHAR WTitle[40] = {0};

				#ifdef FEATURE_VERSION_W317A
				(void)ISHELL_LoadResString(pMe->m_pShell,
											CONTAPP_RES_FILE_LANG,								  
											IDS_PHONEBOOK_SETTINGS,
											WTitle,
											sizeof(WTitle));
				#else
				(void)ISHELL_LoadResString(pMe->m_pShell,
											CONTAPP_RES_FILE_LANG,								  
											IDS_CONTACTS_MANAGEMENT,
											WTitle,
											sizeof(WTitle));
				#endif
				
                if(pMe->m_pIAnn != NULL)
                {
				    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
			}
			#endif

            CContApp_BuildManagementMenu(pMe, pMenuCtl);
            
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;
                                    
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
            #ifdef FEATURE_VERSION_SKY
            {
            IImage *image = ISHELL_LoadResImage( pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);
            Appscommon_ResetBackground(pMe->m_pDisplay,image, APPSCOMMON_BG_COLOR, 
                                                    &pMe->m_rc, 0, 0);
             if( image != NULL)
             {
               IIMAGE_Draw( image, 0, 0);
               IIMAGE_Release( image);
             }
             IMENUCTL_Redraw(pMenuCtl);
            }
            #endif
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            else
            {
                // Set menu select
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wManagementSel);
            }
            
            // Draw prompt bar here
            CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    pMe->m_wManagementSel = 0;
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                    
                default:
                    break;
                    
            }
            break;
            
        case EVT_COMMAND:
            pMe->m_wManagementSel = wParam;
            switch(wParam)
            {
                case IDS_MOVE_CONTACTS:
                   // if(IS_ZERO_REC())
                    //{
                        //防止卡或手机其中一个是空的，而且之前的一次load是load了空的，这样m_nAddList就是空的
                        //CContApp_LoadPhoneBook(pMe, CONTCFG_VIEWTYPE_ALL);
                    //}
                    pMe->m_nCopyOrMove = MOVEMULTIPE;
                    CLOSE_DIALOG(DLGRET_COPYORMOVE);
                    return TRUE;

                case IDS_COPY_CONTACTS:
                    /*if(IS_ZERO_REC())
                    {
                        //防止卡或手机其中一个是空的，而且之前的一次load是load了空的，这样m_nAddList就是空的
                        CContApp_LoadPhoneBook(pMe, CONTCFG_VIEWTYPE_ALL);
                    }*/
                    pMe->m_nCopyOrMove = COPYMULTIPE;
                    CLOSE_DIALOG(DLGRET_COPYORMOVE);
                    return TRUE;
                    
                case IDS_DELETE_CONTACTS:
                    CLOSE_DIALOG(DLGRET_DELETE_SELECT);
                    return TRUE;

				#ifdef FEATURE_VERSION_W317A
				case IDS_DISPLAY_OPTION:
				#endif
                case IDS_VIEWTYPE:
                    CLOSE_DIALOG(DLGRET_VIEWTYPE);
                    return TRUE;
				#ifdef FEATURE_VERSION_W317A
				case IDS_STORAGE:
				#endif
                case IDS_SAVETO:
                    CLOSE_DIALOG(DLGRET_SAVETO);
                    return TRUE;
                    
                case IDS_CONTACTS_MEMORY_STUTAS:
                    CLOSE_DIALOG(DLGRET_CHECKCAPACITY);
                    return TRUE;
                    
#if defined(FEATURE_SUPPORT_BT_APP) && defined(FEATURE_SUPPORT_VCARD)
//#if defined(FEATURE_SUPPORT_VCARD) 	//Modify by zzg 2011_10_25
                case IDS_BT_COPYN:
                    CContApp_LoadPhoneBook(pMe, CONTCFG_VIEWTYPE_ALL);
                    CLOSE_DIALOG(DLGRET_SEND_BY_BT);
                    return TRUE;
#endif

                default:
                    break;
            }
            break;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt;
						pMe->m_wManagementSel = IMENUCTL_GetSel(pMenuCtl);
						rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,pMe->m_wManagementSel);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif             
        default:
            break;
            
    }

    return FALSE;
} // CContApp_HandleFldOptsDlgEvent

/*==============================================================================
函数：
       CContApp_HandleSearchNameDlgEvent
说明：
       IDD_SEARCHNAME 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleSearchNameDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam)
{
    ITextCtl *pTextCtl;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pTextCtl = (ITextCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_SEARCHNAME_TEXTCTL);
                                              
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
            ITEXTCTL_SetProperties(pTextCtl, TP_MULTILINE | TP_FRAME | TP_FIXSETRECT | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT | TP_GRAPHIC_BG|TP_FOCUS_NOSEL);
           // ITEXTCTL_SetProperties(pTextCtl, TP_GRAPHIC_BG | TP_MULTILINE | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT | TP_DISPLAY_SMSCOUNT | TP_NOUPDATE|TP_FOCUS_NOSEL);
            ITEXTCTL_SetRect(pTextCtl, &rc);
                     
            (void)ITEXTCTL_SetInputMode(pTextCtl,
                     CContApp_GetFldInputMode(AEE_ADDRFIELD_NAME));
                     
            (void)ITEXTCTL_SetTitle( pTextCtl,
                                     CONTAPP_RES_FILE_LANG,
                                     IDS_FIND,
                                     NULL);
            /*
            {
                AECHAR WTitle[40] = {0};
                //IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
                (void)ISHELL_LoadResString(pMe->m_pShell,
                CONTAPP_RES_FILE_LANG,                                
                IDS_FIND,
                WTitle,
                sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
            }                   */      
            if(pMe->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, FALSE);
                //IANNUNCIATOR_Redraw(pMe->m_pIAnn);
            }
            
            ITEXTCTL_SetMaxSize( pTextCtl, MAX_INPUT_NAME_EN);
                     
            if(pMe->m_sSearchStr)
            {
                (void)ITEXTCTL_SetText(pTextCtl, pMe->m_sSearchStr, -1);
            }
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_KEY_RELEASE:
        {
            ITextCtl * pIText = (ITextCtl *)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_SEARCHNAME_TEXTCTL);
            
            if ((NULL != pIText) &&
                (ITEXTCTL_IsActive(pIText)))
            {
                AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pIText);
                int nLen=0;
                
                if (NULL != pwstrText)
                {
                    nLen = WSTRLEN(pwstrText);
                }
                
                if (nLen > 0)
                {
                	#ifndef FEATURE_ALL_KEY_PAD
                    // Send       Earse
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_DELETE);
                    #else
                    // Send       Back
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_BACK);
                    #endif
                    IDisplay_UpdateEx(pMe->m_pDisplay,FALSE); 
                }
                else if (nLen <= 0)
                {
                    // Search      Back
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_BACK);
                    IDisplay_UpdateEx(pMe->m_pDisplay, FALSE);
                }
            }
        }
        
        return TRUE;
            
        case EVT_USER_REDRAW:
            // Draw prompt bar here
            //bw:<< 071014 can't draw Battom when display Symbol dialog
            {
                AEETextInputMode nInputMode; 
                nInputMode = ITEXTCTL_GetInputMode(pTextCtl,NULL);
                if (nInputMode == AEE_TM_SYMBOLS)
                {
                    return TRUE;
                }
				ITEXTCTL_SetActive(pTextCtl,TRUE);
                ITEXTCTL_Redraw(pTextCtl);
                //bw:>>
                
                if ( WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) > 0 || ITEXTCTL_GetT9End(pTextCtl) != TC_CURSORSTART )
                {
                	#ifndef FEATURE_ALL_KEY_PAD
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_DELETE);  
                    #else
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_BACK); 
                    #endif
                }
                else
                
                {
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_BACK);                            
                }
                IDisplay_UpdateEx(pMe->m_pDisplay,FALSE);  
            }
            return TRUE;
            
        case EVT_DIALOG_END:
            if(pMe->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, TRUE);
                IANNUNCIATOR_Redraw(pMe->m_pIAnn);
            }            
            if (pMe->m_bSuspending)
            {
                FREEIF(pMe->m_sSearchStr);
                pMe->m_sSearchStr = WSTRDUP(ITEXTCTL_GetTextPtr(pTextCtl));
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_INFO:
                case AVK_SELECT:
                    FREEIF(pMe->m_sSearchStr);
                    pMe->m_sSearchStr = 
                           WSTRDUP(ITEXTCTL_GetTextPtr(pTextCtl));
                    
                    if( NULL != pMe->m_sSearchStr && WSTRLEN(pMe->m_sSearchStr) > 0)
                    {
                        CLOSE_DIALOG(DLGRET_OK);
                    }/*
                    else
                    {
                        CLOSE_DIALOG(DLGRET_INPUT_NULL);
                    }*/
                    return TRUE;
                    
                case AVK_CLR:
                    FREEIF(pMe->m_sSearchStr);
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                    
                default:
                    break;
                    
            }
            break;
            
        case EVT_COMMAND:
            break;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif                       
        default:
            break;
            
    }
    
    return FALSE;
} // CContApp_HandleSearchNameDlgEvent

/*==============================================================================
函数：
       CContApp_HandleDeleteDlgEvent
说明：
       IDD_DELETE 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleDeleteDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam)
{
    IMenuCtl  *pMenuCtl = NULL;  
    ITextCtl          *pTextCtl = NULL; 
    uint16 rtn = TYPE_NO_MATCH;
    sSelectFieldInfo  curSelectFieldInfo;     
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,IDC_DELETE_MENU);
    pTextCtl = (ITextCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg, IDC_DELETE_TEXT);   

    pMe->m_nSmartStateType = SMART_STATE_IDD_DELETE;
    MEMSET(&curSelectFieldInfo,0,sizeof(curSelectFieldInfo));
    if(CContApp_SmartMenuHandle(pMe, pMenuCtl, pTextCtl, eCode,wParam))
    {
        return TRUE;
    }
    CContApp_DeleteMenuScroll(pMe, pMenuCtl, eCode, wParam);

    //pMe->m_wSelectCont = IMENUCTL_GetSel(pMenuCtl);    
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        {
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            uint32 dwMask = IMENUCTL_GetProperties(pMenuCtl);

            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay); 
            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_INIT",0,0,0);

            dwMask = (dwMask & (~MP_WRAPSCROLL)) | MP_UNDERLINE_TITLE;
            IMENUCTL_SetProperties(pMenuCtl, dwMask);

            //IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL);
                
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY|OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif
        }
        return TRUE;
            
        case EVT_DIALOG_START:
        {          
            //MP_NO_REDRAW
            uint32    dwMask = IMENUCTL_GetProperties(pMenuCtl);
            IMENUCTL_SetProperties(pMenuCtl, dwMask & (~MP_NO_REDRAW));
            
            // Build the list Menu
            if(NULL == pMe->m_szAlpha)
            {
                (void)CContApp_BuildDeleteMenu(pMe, pMenuCtl);
            }
            if(CONTAPP_RECID_NULL == pMe->m_wSelectCont)
            {
                pMe->m_wSelectCont = IMENUCTL_GetItemID(pMenuCtl, 0);
            }
            {
                AECHAR WTitle[40] = {0};
                //IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
                (void)ISHELL_LoadResString(pMe->m_pShell,
                CONTAPP_RES_FILE_LANG,                                
                IDS_DELETE,
                WTitle,
                sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
            }
            
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
           /*必须在textctl初始化完毕后,才能获得icon id,而且要在dialog更新完之后再更新图标*/
           CContApp_DrawIMEIcon(pTextCtl, pMe->m_pDisplay);
                       
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            else
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectCont);
            }
            
            pMe->m_wSelectSmart= IMENUCTL_GetSel(pMenuCtl);
            // Draw prompt bar here
            
            if(NULL == pMe->m_szAlpha)
            {
                CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
            }
            CContApp_DrawScrollBar(pMe, pMenuCtl);
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    pMe->m_wMainListSel = MENU_SELECT_NULL;
                    pMe->m_wMainListIdx = 0;
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;

                default:
                    break;
                    
            }
            // App handle all key event
            return TRUE;

        case EVT_KEY_PRESS:
            pMe->m_wSelectCont = IMENUCTL_GetSel(pMenuCtl);    
            switch (wParam)
            {
                case AVK_SELECT:
                    {
                        curSelectFieldInfo.wIndex = pMe->m_wSelectCont;
                        rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                        if (rtn == TYPE_NO_MATCH)
                        {
                            CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                        }
                        else 
                        {
                            CContApp_DeleteSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                        }
                        pMe->m_wSelectCont = IMENUCTL_GetSel(pMenuCtl);
                        (void)CContApp_BuildDeleteMenu(pMe, pMenuCtl);
                        IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectCont);
                        (void)IMENUCTL_Redraw(pMenuCtl);
                    }
                    break;

                case AVK_INFO:                    
                    CLOSE_DIALOG(DLGRET_OK);
                    return TRUE;

                default:
                    break;
            }
            // App handle all key event
            
            // Build the list Menu
            return TRUE;            
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,EVT_KEY_RELEASE);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif  
        case EVT_COMMAND:
            pMe->m_wSelectSmart = wParam;
            pMe->m_wSelectCont = wParam;
            #ifdef FEATURE_VERSION_X3
            if(SUCCESS != CContApp_LoadAddrFlds( pMe, pMe->m_wSelectSmart, SELECT_ALL))
             {
                 CLOSE_DIALOG(DLGRET_ERR);
                 return TRUE;
             }
             curSelectFieldInfo.wIndex = pMe->m_wSelectSmart;
             pMe->m_wFieldIndex = CContApp_GetOneNumberFld(pMe, IDX_NUM_RUIM_FLD);
             curSelectFieldInfo.wFieldMask = pMe->m_wFieldIndex;
             rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
            if (rtn == TYPE_NO_MATCH)
            {
                pMe->m_eTotalSelectNum++;
                CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
            }
           #endif
            CLOSE_DIALOG(DLGRET_SELECT);
            return TRUE;
            
        case EVT_CTL_SEL_CHANGED:
            
            pMe->m_wSelectSmart = wParam;
            pMe->m_wSelectCont = wParam;
            return TRUE;
            
        default:
            break;
            
    }
    
    return FALSE;
} // CContApp_HandleDeleteDlgEvent

/*==============================================================================
函数：
       CContApp_HandleSelectRecordDlgEvent
说明：
       IDD_SELECT_RECORD 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleSelectRecordDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam)
{
    IMenuCtl  *pMenuCtl = NULL;  
    ITextCtl          *pTextCtl = NULL; 
    uint16 rtn = TYPE_NO_MATCH;
    sSelectFieldInfo  curSelectFieldInfo;     
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    MSG_FATAL("CContApp_HandleSelectRecordDlgEvent...........",0,0,0);
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,IDC_SELECT_RECORD);
    pTextCtl = (ITextCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg, IDC_SELECT_RECORD_TEXT);   

    pMe->m_nSmartStateType = SMART_STATE_IDD_SELECT_RECORD;
    MEMSET(&curSelectFieldInfo,0,sizeof(curSelectFieldInfo));
//#if !defined( FEATURE_CARRIER_TAIWAN_APBW) && !defined( FEATURE_CARRIER_VENEZUELA_MOVILNET) 
    if(CContApp_SmartMenuHandle(pMe, pMenuCtl, pTextCtl, eCode,wParam))
    {
        return TRUE;
    }
//#endif
    CContApp_Select_record_MenuScroll(pMe, pMenuCtl, eCode, wParam);

    //pMe->m_wSelectCont = IMENUCTL_GetSel(pMenuCtl);    
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        {
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            uint32 dwMask = IMENUCTL_GetProperties(pMenuCtl);

            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay); 
            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_INIT",0,0,0);

            dwMask = (dwMask & (~MP_WRAPSCROLL)) | MP_UNDERLINE_TITLE;
            IMENUCTL_SetProperties(pMenuCtl, dwMask);

            //IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL);
                
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY|OEMMP_USE_MENU_STYLE);
        }
        return TRUE;
            
        case EVT_DIALOG_START:
        {          
            //MP_NO_REDRAW
            uint32    dwMask = IMENUCTL_GetProperties(pMenuCtl);
            IMENUCTL_SetProperties(pMenuCtl, dwMask & (~MP_NO_REDRAW));
            
            // Build the list Menu
            #if 0
            IMENUCTL_SetTitle( pMenuCtl,CONTAPP_RES_FILE_LANG,IDS_BT_COPYN,NULL);
			#else
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
			            CONTAPP_RES_FILE_LANG,                                
			            IDS_BT_COPYN,
			            WTitle,
			            sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
				    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
			}
			#endif
            if(NULL == pMe->m_szAlpha)
            {
                // 这里就没必要重新copy一个相同的函数了
                (void)CContApp_BuildDeleteMenu(pMe, pMenuCtl);
            }
            if(CONTAPP_RECID_NULL == pMe->m_wSelectCont)
            {
                pMe->m_wSelectCont = IMENUCTL_GetItemID(pMenuCtl, 0);
            }
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
           /*必须在textctl初始化完毕后,才能获得icon id,而且要在dialog更新完之后再更新图标*/
           CContApp_DrawIMEIcon(pTextCtl, pMe->m_pDisplay);
                       
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            else
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectCont);
            }
            
            pMe->m_wSelectSmart= IMENUCTL_GetSel(pMenuCtl);
            // Draw prompt bar here
            
            if(NULL == pMe->m_szAlpha)
            {
                CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
            }
            CContApp_DrawScrollBar(pMe, pMenuCtl);
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }

            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    pMe->m_wMainListSel = MENU_SELECT_NULL;
                    pMe->m_wMainListIdx = 0;
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;

                default:
                    break;
                    
            }
            // App handle all key event
            return TRUE;

        case EVT_KEY_PRESS:
            pMe->m_wSelectCont = IMENUCTL_GetSel(pMenuCtl);    
            switch (wParam)
            {
                case AVK_SELECT:
                    {
                        curSelectFieldInfo.wIndex = pMe->m_wSelectCont;
                        rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                        if (rtn == TYPE_NO_MATCH)
                        {
                            CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                        }
                        else 
                        {
                            CContApp_DeleteSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                        }
                        pMe->m_wSelectCont = IMENUCTL_GetSel(pMenuCtl);
                        (void)CContApp_BuildDeleteMenu(pMe, pMenuCtl);
                        IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectCont);
                        (void)IMENUCTL_Redraw(pMenuCtl);
                    }
                    break;

                case AVK_INFO:                    
                    CLOSE_DIALOG(DLGRET_OK);
                    return TRUE;

                default:
                    break;
            }
            // App handle all key event
            
            // Build the list Menu
            return TRUE;            
            
        case EVT_COMMAND:
            pMe->m_wSelectSmart = wParam;
            pMe->m_wSelectCont = wParam;
            return TRUE;
            
        case EVT_CTL_SEL_CHANGED:
            
            pMe->m_wSelectSmart = wParam;
            pMe->m_wSelectCont = wParam;
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,EVT_KEY_PRESS);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif    
        default:
            break;
            
    }
    
    return FALSE;
} // CContApp_HandleSelectRecordDlgEvent

/*==============================================================================
函数：
       CContApp_HandleSelectDlgEvent
说明：
       IDD_SELECT 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleSelectDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam)
{
    IMenuCtl  *pMenuCtl = NULL;  
    ITextCtl          *pTextCtl = NULL; 
    sSelectFieldInfo curSelectFieldInfo;
    static uint16 wField = MASK_ADDRFIELD_PHONE_GENERIC;
    uint16 rtn = TYPE_NO_MATCH;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
	 MSG_FATAL("CContApp_HandleSelectDlgEvent...........eCode=%d--wParam=%d--dwParam=%d",eCode,wParam,dwParam);
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg, IDC_SELECT_MENU);
    pTextCtl = (ITextCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg, IDC_SELECT_TEXT);   

    pMe->m_nSmartStateType = SMART_STATE_IDD_SELECT;
    MEMSET(&curSelectFieldInfo,0,sizeof(curSelectFieldInfo));

    if(CContApp_SmartMenuHandle(pMe, pMenuCtl, pTextCtl, eCode,wParam))
    {
        MSG_FATAL("CContApp_HandleSelectDlgEvent...........eCode=%d--wParam=%d--dwParam=%d",eCode,wParam,dwParam);
        return TRUE;
    }
    
    CContApp_SelectMenuScroll(pMe, pMenuCtl, eCode, wParam);
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        {   
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            uint32 dwMask = IMENUCTL_GetProperties(pMenuCtl);
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= 2*GetBottomBarHeight(pMe->m_pDisplay);
            
            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_INIT",0,0,0);
            
            dwMask = (dwMask & (~MP_WRAPSCROLL)) | MP_UNDERLINE_TITLE;
            IMENUCTL_SetProperties(pMenuCtl, dwMask);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY|OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif
            {
                AECHAR WTitle[40] = {0};
                //IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);

				#ifdef FEATURE_VERSION_C337
				(void)ISHELL_LoadResString(pMe->m_pShell,
							                CONTAPP_RES_FILE_LANG,                                
							                IDS_PHONEBOOK,
							                WTitle,
							                sizeof(WTitle));
				#else
                (void)ISHELL_LoadResString(pMe->m_pShell,
							                CONTAPP_RES_FILE_LANG,                                
							                IDS_APPLET,
							                WTitle,
							                sizeof(WTitle));
				#endif
				
                if(pMe->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
            }            
            //    IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL);
            //    pMe->m_wSelectCont = IMENUCTL_GetSel(pMenuCtl);
            //    CContApp_DisplaySelectField(pMe, pMe->m_wFieldIndex);         
            //    (void)CContApp_BuildSelectMenu(pMe, pMenuCtl);
            
            return TRUE;
        }   
        case EVT_DIALOG_START:
        {   
            if(NULL == pMe->m_szAlpha)
            {
                (void)CContApp_BuildSelectMenu(pMe, pMenuCtl);
            }
            if(CONTAPP_RECID_NULL == pMe->m_wSelectCont)
            {
                pMe->m_wSelectCont = IMENUCTL_GetItemID(pMenuCtl, 0);
            }
            
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
        {
            //MP_NO_REDRAW
            uint32    dwMask = IMENUCTL_GetProperties(pMenuCtl);
            IMENUCTL_SetProperties(pMenuCtl, dwMask & (~MP_NO_REDRAW));
            
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            else
            {              
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectCont);
            }
            
            pMe->m_wSelectSmart = IMENUCTL_GetSel(pMenuCtl);
            //(void)CContApp_BuildSelectMenu(pMe, pMenuCtl);
            
            //发送列表最多支持100条记录
            /*
            if(IVector_Size(pMe->m_pAddList) > pMe->m_nleftNum && (pMe->m_pSmartBufLen == 0 || pMe->m_pSmartBufLen >pMe->m_nleftNum))
            {
                if (WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) > 0)
                {
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_DEL);
                }
                else
                {
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_BACK);
                }
            }
            else*/
            {
                if (WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) > 0)
                {
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_DEL);
                }
                else
                {
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
                }
            }
            //(void)IMENUCTL_Redraw(pMenuCtl);

            /*必须在textctl初始化完毕后,才能获得icon id,而且要在dialog更新完之后再更新图标*/
            CContApp_DrawIMEIcon(pTextCtl, pMe->m_pDisplay);
            CContApp_DrawScrollBar(pMe, pMenuCtl);
            IDISPLAY_Update(pMe->m_pDisplay);  
        }
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }

            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    pMe->m_wMainListSel = MENU_SELECT_NULL;
                    pMe->m_wMainListIdx = 0;
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;

                default:
                    break;
                    
            }
            // App handle all key event
            return TRUE;

        case EVT_KEY_PRESS:
            
            // Fix the bug that after delete a record
            pMe->m_wSelectCont = IMENUCTL_GetSel(pMenuCtl);
            CContApp_GetRecByID(pMe, pMe->m_wSelectCont);
            
            switch (wParam)
            {
                case AVK_LEFT:
                {
                    /*
                    if ((wField>>1) < MASK_ADDRFIELD_PHONE_GENERIC)
                    {
                        wField = MASK_ADDRFIELD_NOTES;
                    }
                    while(1)
                    {
                        if (pMe->m_wFieldMask & (wField>>1))
                        {
                            wField = wField>>1;
                            pMe->m_wFieldIndex = wField;
                            break;
                        }
                        else
                        {
                            wField = wField>>1;
                        }
                    }

                    if (wField == MASK_ADDRFIELD_NAME)
                    {
                        wField = MASK_ADDRFIELD_NOTES;

                         while(1)
                        {
                            if (pMe->m_wFieldMask & (wField>>1))
                            {
                                wField = wField>>1;
                                pMe->m_wFieldIndex = wField;
                                break;
                            }
                            else
                            {
                                wField = wField>>1;
                            }
                        }
                    }
                    */
                }
                break;

                case AVK_RIGHT:
                {
                    /*
                    if ((wField<<1) > pMe->m_wFieldMask)
                    {
                        wField = MASK_ADDRFIELD_NAME;
                        pMe->m_wFieldIndex = wField;
                    }
                    
                    while(1)
                    {
                        if (pMe->m_wFieldMask & (wField<<1))
                        {
                            wField = wField<<1;
                            pMe->m_wFieldIndex = wField;
                            break;
                        }
                        else
                        {
                            wField = wField<<1;
                        }
                    }
                    */
                }
                break;

                case AVK_UP:
                case AVK_DOWN:
                    return TRUE;

                case AVK_SELECT:
                    curSelectFieldInfo.wIndex = pMe->m_wSelectCont;
                    curSelectFieldInfo.wFieldCount = pMe->m_wFieldCount;
                    curSelectFieldInfo.wFieldMask = pMe->m_wFieldIndex;
                    
                    if(CContApp_GetCurrFldNum(pMe, AEE_ADDRFIELD_PHONE_GENERIC) > 1)
                    {                    	
                        CLOSE_DIALOG(DLGRET_POPNUMFLD);
                    }
                    else
                    {
                        rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);						
						
                        if (rtn == TYPE_NO_MATCH)
                        {
                            CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);							
							
                            if (pMe->m_wSelFldType == SINGLE_SELECT_NUMBER)
                            {
                                CLOSE_DIALOG(DLGRET_OK);
                                return TRUE;
                            }
                        }
                        else if (rtn == TYPE_INDEX_MATCH)
                        {
                            CContApp_UpdateSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                            if(pCurSelectFieldNode->SelectFieldInfo.wFieldMask == 1)
                            {
                                CContApp_DeleteSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                            }
                        }
                        else if (rtn == TYPE_FIELD_MATCH)
                        {
                             CContApp_UpdateSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                            if(pCurSelectFieldNode->SelectFieldInfo.wFieldMask == 1)
                            {
                                CContApp_DeleteSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                            }
                        }
                    }
                    break;

                case AVK_END:    
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                    
                case AVK_INFO:					
                    if (pMe->m_wSelFldType == SINGLE_SELECT_NUMBER)
                    {
                        curSelectFieldInfo.wIndex = pMe->m_wSelectCont;
                        curSelectFieldInfo.wFieldCount = pMe->m_wFieldCount;
                        curSelectFieldInfo.wFieldMask = pMe->m_wFieldIndex;
                        
                        rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                        if (rtn == TYPE_NO_MATCH)
                        {
                            CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                        }
                    }

                    CLOSE_DIALOG(DLGRET_OK);
                    return TRUE;
                    
                default:
                    break;                    
            }   
            // App handle all key event           
            
            // Build the list Menu
            pMe->m_wSelectCont = IMENUCTL_GetSel(pMenuCtl);
            (void)CContApp_BuildSelectMenu(pMe, pMenuCtl);
            IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectCont);
            (void)IMENUCTL_Redraw(pMenuCtl);
            //CContApp_DisplaySelectField(pMe, pMe->m_wFieldIndex);
            IDISPLAY_Update(pMe->m_pDisplay); 
            return TRUE;            
            
        case EVT_COMMAND:
            pMe->m_wSelectCont = wParam;
            pMe->m_wSelectSmart = wParam;
            #ifdef FEATURE_VERSION_X3
            if(SUCCESS != CContApp_LoadAddrFlds( pMe, pMe->m_wSelectSmart, SELECT_ALL))
             {
                 CLOSE_DIALOG(DLGRET_ERR);
                 return TRUE;
             }
             curSelectFieldInfo.wIndex = pMe->m_wSelectSmart;
             pMe->m_wFieldIndex = CContApp_GetOneNumberFld(pMe, IDX_NUM_RUIM_FLD);
             curSelectFieldInfo.wFieldMask = pMe->m_wFieldIndex;
             rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
            if (rtn == TYPE_NO_MATCH)
            {
                pMe->m_eTotalSelectNum++;
                CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
            }
           #endif
            CLOSE_DIALOG(DLGRET_SELECT);
            return TRUE;
            
        case EVT_CTL_SEL_CHANGED:
            pMe->m_wSelectCont = wParam;
            pMe->m_wSelectSmart = wParam;
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,EVT_KEY_RELEASE);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif             
        default:
            break;
            
    }
    
    return FALSE;
} // CContApp_HandleDeleteDlgEvent

/*==============================================================================
函数：
       CContApp_HandleDetailDlgEvent
说明：
       IDD_DETAIL 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleDetailDlgEvent( CContApp  *pMe,
                                                 AEEEvent   eCode,
                                                 uint16     wParam,
                                                 uint32     dwParam)
{
    IMenuCtl *pMenuCtl;
    //IStatic *pStatic;
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_MENU_TITLE);

    /*pStatic = (IStatic*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                            IDC_DETAIL_STATIC);*/

	
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            {
            // Set the edit menu title is first field
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            
          // Check Parameter
            if(!pMenuCtl)
            {
                return EBADPARM;
            }
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
            
            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_START",0,0,0);
			#if 0
            (void)IMENUCTL_SetTitle( pMenuCtl,
                                     NULL,
                                     0,
                                     (AECHAR *)CContApp_GetFldBuf(pMe, 0));   
			#else
			{
			    if(pMe->m_pIAnn != NULL)
                {         
				    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,(AECHAR *)CContApp_GetFldBuf(pMe, 0));
                }
			}
			#endif

            CContApp_FreeBuffer(pMe);

            CContApp_GetRecByID(pMe, pMe->m_wSelectCont);
            // build menu
            CContApp_BuildEditMenuMenu(pMe, pMenuCtl, FALSE);
            /*if(SUCCESS != CContApp_BuildEditMenuMenu(pMe, pMenuCtl, FALSE))
            {
                CLOSE_DIALOG(DLGRET_ERR);
                return TRUE;
            }*/
            
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY|OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif

            
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
             return TRUE;                       
            // Note: Fall through to the EVT_USER_REDRAW
            //lint -fallthrough
        }
        case EVT_USER_REDRAW:
            // Draw prompt bar here
            // set static text
            //(void)CContApp_SetDetailStatic( pMe, pStatic);
            
            /*CContApp_GetRecByID(pMe, pMe->m_wSelectCont);
            // build menu
            if(SUCCESS != CContApp_BuildEditMenuMenu(pMe, pMenuCtl, FALSE))
            {
                CLOSE_DIALOG(DLGRET_ERR);
                return TRUE;
            }*/

            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            else
            {
                // Set menu select
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectEdit);
            }
            
            // Draw prompt bar here
            CONTAPP_DRAW_BOTTOMBAR(BTBAR_BACK);	
#ifdef FEATURE_VERSION_C337
            if (pMe->m_bSpeedDialParam == TRUE)
            {
            	CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_CANCEL);	
            }
#endif
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY_RELEASE:
        {
        	 switch (wParam)
             {
             	#ifdef FEATURE_VERSION_C337
				case AVK_SELECT:					
					if ((pMe->m_bSpeedDialParam == TRUE) && (pMe->m_nSpeedDialNumber != 0))
					{
						AECHAR* callNum;	                    
	                    if ( SUCCESS != ISHELL_CreateInstance( pMe->m_pShell,
	                                                           AEECLSID_DIALER,
	                                                           (void **)&pMe->m_pCallApp))
	                    {
	                        return EFAILED;
	                    }
	                    
	                    switch(IMENUCTL_GetSel(pMenuCtl))
	                    {
	                        case IDI_EDIT_MENU_MOBILE:
	                            callNum = pMe->m_pAddNewMobile;
	                            break;
	                            
	                        case IDI_EDIT_MENU_HOME:
	                            callNum = pMe->m_pAddNewHome;
	                            break;
	                            
	                        case IDI_EDIT_MENU_OFFICE:
	                            callNum = pMe->m_pAddNewOffice;
	                            break;
	                            
	                        case IDI_EDIT_MENU_FAX:
	                            callNum = pMe->m_pAddNewFax;
	                            break;

	                        default:
	                            return TRUE;
	                    }

						
						if(WSTRLEN(callNum) <= 3)
		                {
		                    CLOSE_DIALOG(DLGRET_OK)		                   
		                    return TRUE;
		                }						
						
                        (void)CContApp_SetConfig( pMe,
                                                  (ContAppCFG)(CONTCFG_SMARTDIAL+pMe->m_nSpeedDialNumber),
                                                  callNum,
                                                  sizeof(AECHAR)*(MAX_INPUT_NAME_EN+1));   
						

	                    // Make a voice call
	                    ICallApp_CallNumber(pMe->m_pCallApp, (AECHAR *)callNum);

	                    if (pMe->m_pCallApp != NULL) 
	                    {
	                        ICallApp_Release(pMe->m_pCallApp);
	                        pMe->m_pCallApp = NULL;
	                    }         

						pMe->m_bSpeedDialParam = FALSE;
						pMe->m_nSpeedDialNumber = 0;

						(void) ISHELL_CloseApplet(pMe->m_pShell,FALSE);

						return TRUE;
					}
					break;
				#else
             	case AVK_SELECT:
					//CLOSE_DIALOG(DLGRET_OK);
              		MSG_FATAL("AVK_SELECT........",0,0,0);
              		pMe->m_boptaleadyView = TRUE;
                    CLOSE_DIALOG(DLGRET_SELECT);
                    return TRUE;
				#endif	
                    
                default:
                    break; 
             }
             break;
        }
        case EVT_KEY:
            switch (wParam)
            {
                //case AVK_INFO:
                case AVK_CLR:
                	//CLOSE_DIALOG(DLGRET_OK);
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;		

				#ifdef FEATURE_VERSION_C337
				case AVK_INFO:
				case AVK_SELECT:
                    MSG_FATAL("***zzg Detail m_bSpeedDialParam=%x",pMe->m_bSpeedDialParam,0,0);
					if ((pMe->m_bSpeedDialParam == TRUE) && (pMe->m_nSpeedDialNumber != 0))
					{
						AECHAR* callNum;	                    
	                    if ( SUCCESS != ISHELL_CreateInstance( pMe->m_pShell,
	                                                           AEECLSID_DIALER,
	                                                           (void **)&pMe->m_pCallApp))
	                    {
	                        return EFAILED;
	                    }
	                    
	                    switch(IMENUCTL_GetSel(pMenuCtl))
	                    {
	                        case IDI_EDIT_MENU_MOBILE:
	                            callNum = pMe->m_pAddNewMobile;
	                            break;
	                            
	                        case IDI_EDIT_MENU_HOME:
	                            callNum = pMe->m_pAddNewHome;
	                            break;
	                            
	                        case IDI_EDIT_MENU_OFFICE:
	                            callNum = pMe->m_pAddNewOffice;
	                            break;
	                            
	                        case IDI_EDIT_MENU_FAX:
	                            callNum = pMe->m_pAddNewFax;
	                            break;

	                        default:
	                            return TRUE;
	                    }

						
						if(WSTRLEN(callNum) <= 3)
		                {
		                    CLOSE_DIALOG(DLGRET_OK)		                   
		                    return TRUE;
		                }						
						
                        (void)CContApp_SetConfig( pMe,
                                                  (ContAppCFG)(CONTCFG_SMARTDIAL+pMe->m_nSpeedDialNumber),
                                                  callNum,
                                                  sizeof(AECHAR)*(MAX_INPUT_NAME_EN+1));   
						

	                    // Make a voice call
	                    ICallApp_CallNumber(pMe->m_pCallApp, (AECHAR *)callNum);

	                    if (pMe->m_pCallApp != NULL) 
	                    {
	                        ICallApp_Release(pMe->m_pCallApp);
	                        pMe->m_pCallApp = NULL;
	                    }         

						pMe->m_bSpeedDialParam = FALSE;
						pMe->m_nSpeedDialNumber = 0;

						(void) ISHELL_CloseApplet(pMe->m_pShell,FALSE);

						return TRUE;
					}
					CLOSE_DIALOG(DLGRET_CANCELED)
					return TRUE;
				#else
				case AVK_SELECT:
                    //CLOSE_DIALOG(DLGRET_OK);
              		MSG_FATAL("AVK_SELECT........",0,0,0);
                    CLOSE_DIALOG(DLGRET_SELECT);
                    return TRUE;
				#endif
                
                case AVK_CAMERA:
				#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)
				{
				nv_item_type	SimChoice;
				OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
				if(SimChoice.sim_select ==AVK_SEND_TWO)
				{
					AECHAR* callNum;
                    
                    if ( SUCCESS != ISHELL_CreateInstance( pMe->m_pShell,
                                                           AEECLSID_DIALER,
                                                           (void **)&pMe->m_pCallApp))
                    {
                        return EFAILED;
                    }
                    
                    switch(IMENUCTL_GetSel(pMenuCtl))
                    {
                        case IDI_EDIT_MENU_MOBILE:
                            callNum = pMe->m_pAddNewMobile;
                            break;
                            
                        case IDI_EDIT_MENU_HOME:
                            callNum = pMe->m_pAddNewHome;
                            break;
                            
                        case IDI_EDIT_MENU_OFFICE:
                            callNum = pMe->m_pAddNewOffice;
                            break;
                            
                        case IDI_EDIT_MENU_FAX:
                            callNum = pMe->m_pAddNewFax;
                            break;

                        default:
                            return TRUE;
                    }

                    // Make a voice call
                    ICallApp_CallNumber(pMe->m_pCallApp, (AECHAR *)callNum);

                    if (pMe->m_pCallApp != NULL) 
                    {
                        ICallApp_Release(pMe->m_pCallApp);
                        pMe->m_pCallApp = NULL;
                    }     
                    return TRUE;
                
				}
				}
				#endif
				break;
				
                case AVK_SEND:
                {
                    AECHAR* callNum;
                    #if defined(FEATURE_VERSION_C01) 
					{
						nv_item_type	SimChoice;
						OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
						if(SimChoice.sim_select==AVK_SEND_TWO)
						{
							return TRUE;
						}
					}
					#endif
                    if ( SUCCESS != ISHELL_CreateInstance( pMe->m_pShell,
                                                           AEECLSID_DIALER,
                                                           (void **)&pMe->m_pCallApp))
                    {
                        return EFAILED;
                    }
                    
                    switch(IMENUCTL_GetSel(pMenuCtl))
                    {
                        case IDI_EDIT_MENU_MOBILE:
                            callNum = pMe->m_pAddNewMobile;
                            break;
                            
                        case IDI_EDIT_MENU_HOME:
                            callNum = pMe->m_pAddNewHome;
                            break;
                            
                        case IDI_EDIT_MENU_OFFICE:
                            callNum = pMe->m_pAddNewOffice;
                            break;
                            
                        case IDI_EDIT_MENU_FAX:
                            callNum = pMe->m_pAddNewFax;
                            break;

                        default:
                            return TRUE;
                    }

                    // Make a voice call
                    ICallApp_CallNumber(pMe->m_pCallApp, (AECHAR *)callNum);

                    if (pMe->m_pCallApp != NULL) 
                    {
                        ICallApp_Release(pMe->m_pCallApp);
                        pMe->m_pCallApp = NULL;
                    }                    
                }
                    return TRUE;
                    
                default:
                    break;                    
            }
            break;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif     
        case EVT_COMMAND:
            break;
            
        default:
            break;            
    }
    return FALSE;
} // CContApp_HandleDetailDlgEvent

/*==============================================================================
函数：
       CContApp_HandleDetailMultiDlgEvent
说明：
       IDD_DETAIL_MULTI 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleDetailMultiDlgEvent( CContApp  *pMe,
                                                 AEEEvent   eCode,
                                                 uint16     wParam,
                                                 uint32     dwParam)
{
    IMenuCtl *pMenuCtl;
    static sSelectFieldInfo curSelectFieldInfo = {0};
    
    //IStatic *pStatic;
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_MENU_DETAIL_MULTI);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            {
            // Set the edit menu title is first field
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            
          // Check Parameter
            if(!pMenuCtl)
            {
                return EBADPARM;
            }
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
            
            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_START",0,0,0);
			#if 0
            (void)IMENUCTL_SetTitle( pMenuCtl,
                                     NULL,
                                     0,
                                     (AECHAR *)CContApp_GetFldBuf(pMe, 0));   
			#else
			{
				if(pMe->m_pIAnn != NULL)
                {            
				    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,(AECHAR *)CContApp_GetFldBuf(pMe, 0));
                }
			}
			#endif

            CContApp_FreeBuffer(pMe);

            CContApp_GetRecByID(pMe, pMe->m_wSelectCont);
            // build menu
            CContApp_BuildEditMenuMenu(pMe, pMenuCtl, FALSE);
            /*if(SUCCESS != CContApp_BuildEditMenuMenu(pMe, pMenuCtl, FALSE))
            {
                CLOSE_DIALOG(DLGRET_ERR);
                return TRUE;
            }*/
            
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL | MP_MULTI_SEL);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY|OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif

            InitCheckBox(pMenuCtl);
                      
            MEMSET(&curSelectFieldInfo, 0, sizeof(curSelectFieldInfo));  
            curSelectFieldInfo.wIndex = pMe->m_wSelectCont;
            
            if((SUCCESS == CContApp_GetSelectNode(pSelectFieldListRoot, &curSelectFieldInfo)) && (curSelectFieldInfo.wFieldMask != 0))
            {
                int i = 0;
                while(i < 32)
                {
                    switch(curSelectFieldInfo.wFieldMask & (1<<i))
                    {
                        case MASK_ADDRFIELD_NAME:
                            IMENUCTL_SetSelEx(pMenuCtl, IDI_EDIT_MENU_NAME, TRUE);
                            break;
                            
                        case MASK_ADDRFIELD_PHONE_GENERIC:
                            IMENUCTL_SetSelEx(pMenuCtl, IDI_EDIT_MENU_MOBILE, TRUE);
                            break;
                            
                        case MASK_ADDRFIELD_PHONE_HOME:
                            IMENUCTL_SetSelEx(pMenuCtl, IDI_EDIT_MENU_HOME, TRUE);
                            break;
                            
                        case MASK_ADDRFIELD_PHONE_WORK:
                            IMENUCTL_SetSelEx(pMenuCtl, IDI_EDIT_MENU_OFFICE, TRUE);
                            break;
                            
                        case MASK_ADDRFIELD_PHONE_FAX:
                            IMENUCTL_SetSelEx(pMenuCtl, IDI_EDIT_MENU_FAX, TRUE);
                            break;
                            
                        case MASK_ADDRFIELD_EMAIL:
                            IMENUCTL_SetSelEx(pMenuCtl, IDI_EDIT_MENU_EMAIL, TRUE);
                            break;
                            
                        case MASK_ADDRFIELD_ADDRESS:
                            IMENUCTL_SetSelEx(pMenuCtl, IDI_EDIT_MENU_ADDRESS, TRUE);
                            break;
                            
                        case MASK_ADDRFIELD_URL:
                            IMENUCTL_SetSelEx(pMenuCtl, IDI_EDIT_MENU_URL, TRUE);
                            break;
                            
                        case MASK_ADDRFIELD_NOTES:
                            IMENUCTL_SetSelEx(pMenuCtl, IDI_EDIT_MENU_REMARK, TRUE);
                            break;
                            
                        default:
                            break;
                    }

                    i++;
                }
            }
            
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
             return TRUE;                       
            // Note: Fall through to the EVT_USER_REDRAW
            //lint -fallthrough
        }
        case EVT_USER_REDRAW:

            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }

            // Draw prompt bar here
            CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_BACK);
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                //case AVK_INFO:
                case AVK_CLR:
                    CContApp_FreeBuffer(pMe);
                    FREEIF(pMe->m_nRingToneID);
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                    
                default:
                    break;                    
            }
            break;

        case EVT_KEY_RELEASE:
            
            if(AVK_INFO == wParam)	//AVK_RIGHT
            {
                uint16 selItem = IMENUCTL_GetSel(pMenuCtl);

                switch(selItem)
                {
                    case IDI_EDIT_MENU_NAME:
                        curSelectFieldInfo.wFieldMask ^=  MASK_ADDRFIELD_NAME; 
                        break;
                        
                    case IDI_EDIT_MENU_MOBILE:
                        curSelectFieldInfo.wFieldMask ^=  MASK_ADDRFIELD_PHONE_GENERIC; 
                        break;

                    case IDI_EDIT_MENU_HOME:
                        curSelectFieldInfo.wFieldMask ^=  MASK_ADDRFIELD_PHONE_HOME; 
                        break;
                        
                    case IDI_EDIT_MENU_OFFICE:
                        curSelectFieldInfo.wFieldMask ^=  MASK_ADDRFIELD_PHONE_WORK; 
                        break;

                    case IDI_EDIT_MENU_FAX:
                        curSelectFieldInfo.wFieldMask ^=  MASK_ADDRFIELD_PHONE_FAX; 
                         break;
                         
                    case IDI_EDIT_MENU_EMAIL:
                        curSelectFieldInfo.wFieldMask ^=  MASK_ADDRFIELD_EMAIL; 
                        break;
                   
                    case IDI_EDIT_MENU_ADDRESS:
                        curSelectFieldInfo.wFieldMask ^=  MASK_ADDRFIELD_ADDRESS; 
                        break;
                        
                    case IDI_EDIT_MENU_URL:
                        curSelectFieldInfo.wFieldMask ^=  MASK_ADDRFIELD_URL; 
                        break;
                        
                    case IDI_EDIT_MENU_REMARK:
                        curSelectFieldInfo.wFieldMask ^=  MASK_ADDRFIELD_NOTES; 
                        break;

                    default:
                        break;
                }
                return TRUE;
            }
            else if(AVK_SELECT == wParam || AVK_INFO == wParam)
            {
                uint16  rtn = TYPE_NO_MATCH;
                
                rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                if (rtn == TYPE_NO_MATCH && curSelectFieldInfo.wFieldMask != 0)
                {
                    CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                }
                else
                {
                    CContApp_SetSelectNodeFieldMark(pSelectFieldListRoot, &curSelectFieldInfo);
                    if(pCurSelectFieldNode->SelectFieldInfo.wFieldMask == 1)
                    {// only have name fld
                        CContApp_DeleteSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                    }
                }
                CLOSE_DIALOG(DLGRET_OK);
                return TRUE;
            }
            break;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,EVT_KEY_PRESS);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,EVT_KEY_PRESS);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif         
        default:
            break;            
    }
    return FALSE;
} // CContApp_HandleDetailMultiDlgEvent

/*==============================================================================
函数：
       CContApp_HandleCopyingDlgEvent
说明：
       IDD_COPYING 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleCopyingDlgEvent( CContApp  *pMe,
                                                AEEEvent   eCode,
                                                uint16     wParam,
                                                uint32     dwParam)
{
     static IStatic * pStatic = NULL;
    if (NULL == pMe)
    {
        return FALSE;
    }
    /*
    if (NULL == pMe->m_pStatic)
    {
        return FALSE;
    }
    */

   if (NULL == pStatic)
   {
        AEERect rect = {0};
        if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                                 AEECLSID_STATIC,
                                                 (void **)&pStatic))
        
        {
            return FALSE;
            ERR("ISHELL_CreateInstance,AEECLSID_STATIC 2",0,0,0);
        }        
        ISTATIC_SetRect(pStatic, &rect);  
   }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
        
        case EVT_DIALOG_START:            
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
                                    
            // Note: Fall through to the EVT_USER_REDRAW
            //lint -fallthrough
            
        case EVT_USER_REDRAW:
            {
            PromptMsg_Param_type  Msg_Param={0};
            AECHAR  wstrText[MSGBOX_MAXTEXTLEN];

            if((pMe->m_nCopyMoveType == COPYMULTIPE_TOUIM)||(pMe->m_nCopyMoveType == COPYMULTIPE_TOPHONE))    
            {
                // 从资源文件取消息内容
                (void)ISHELL_LoadResString(pMe->m_pShell,
                                        CONTAPP_RES_FILE_LANG,                                
                                        IDS_COPYING,
                                        wstrText,
                                        sizeof(wstrText));              
            }
            else if((pMe->m_nCopyMoveType == MOVEMULTIPE_TOUIM)||(pMe->m_nCopyMoveType == MOVEMULTIPE_TOPHONE))
            {
                // 从资源文件取消息内容
                (void)ISHELL_LoadResString(pMe->m_pShell,
                                        CONTAPP_RES_FILE_LANG,                                
                                        IDS_MOVING,
                                        wstrText,
                                        sizeof(wstrText));  
            }
            Msg_Param.ePMsgType = MESSAGE_WAITING;
            Msg_Param.pwszMsg = wstrText;
            Msg_Param.eBBarType = BTBAR_NONE;
            DrawPromptMessage(pMe->m_pDisplay, pStatic, &Msg_Param);   
            IDISPLAY_Update(pMe->m_pDisplay);  
            }           
            return TRUE;
            
        case EVT_FLIP:
            FORCE_COMPLETE_COPY();
            CContApp_FreeSelectFieldListNode();
            ISTATIC_Release(pStatic);
            pStatic = NULL;
            CLOSE_DIALOG(DLGRET_CANCELED);
            return TRUE;

        case EVT_DIALOG_END:
            FORCE_COMPLETE_COPY();
            CContApp_FreeSelectFieldListNode();
            ISTATIC_Release(pStatic);
            pStatic = NULL;
            return TRUE;
            
        case EVT_USER_COMPLETE:                      
            CLOSE_DIALOG(DLGRET_OK);            
            return TRUE;
            
        case EVT_USER_MAX:
            CLOSE_DIALOG(DLGRET_FULL);            
            return TRUE;
            
        case EVT_USER_ERR:
            CLOSE_DIALOG(DLGRET_ERR);
            return TRUE;
            
        default:
            break;
            
    }
    
    return FALSE;
} // CContApp_HandleCopyingDlgEvent

/*==============================================================================
函数：
       CContApp_HandleGroupOptDlgEvent
说明：
       IDD_GROUPOPT 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleGroupOptDlgEvent( CContApp  *pMe,
                                                   AEEEvent   eCode,
                                                   uint16     wParam,
                                                   uint32     dwParam)
{
    IMenuCtl *pMenuCtl;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_MENU_GROUPOPT);
                                              
    MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);

            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_START",0,0,0);
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL | MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif
            CContApp_BuildGroupOptMenu(pMe, pMenuCtl);
            IMENUCTL_SetPopMenuRect(pMenuCtl); //wuqan.tang add 

/*
            if(0 == pMe->m_wSelectGroup)
            {
                (void)IMENUCTL_DeleteItem(pMenuCtl, IDI_MENU_GROUPOPT_EDIT);                
            }

*/

            MENU_SET_COMICON(pMenuCtl);
            
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
                                    
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:            
            // Draw prompt bar here
            CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                    
                default:
                    break;
                    
            }
            break;
            
        case EVT_COMMAND:
            switch(wParam)
            {
                case IDS_VIEW:
                    CLOSE_DIALOG(DLGRET_VIEW);
                    return TRUE;
                    
                case IDS_RENAME:
                    CLOSE_DIALOG(DLGRET_EDIT);
                    return TRUE;
                    
                default:
                    break;
            }
            break;
        #ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        		MSG_FATAL("wXPos====%d,wYPos=====%d",wXPos,wYPos,0);
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt;
						uint16 m_wManagementSel = IMENUCTL_GetSel(pMenuCtl);
						rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,m_wManagementSel);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif   
        default:
            break;
            
    }
    
    return FALSE;
} // CContApp_HandleGroupOptDlgEvent


/*==============================================================================
函数：
       CContApp_HandleGroupOptEditDlgEvent
说明：
       IDD_GROUPOPT_EDIT 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleGroupOptEditDlgEvent( CContApp  *pMe,
                                                 AEEEvent   eCode,
                                                 uint16     wParam,
                                                 uint32     dwParam)
{
    ITextCtl *pTextCtl;
    AECHAR *sGroup = NULL; 
    AECHAR pGroup[MAX_INPUT_NAME_EN + 1];

#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif

    MSG_FATAL("CContApp_HandleGroupOptEditDlgEvent Start eCode=0x%x",eCode,0,0);
    pTextCtl = (ITextCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_GROUPOPT_EDIT);
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
            ITEXTCTL_SetRect(pTextCtl, &rc);
            
            ITEXTCTL_SetProperties( pTextCtl, TP_MULTILINE | TP_FRAME | TP_FIXSETRECT | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT|TP_FOCUS_NOSEL | TP_GRAPHIC_BG);
                     
            (void)ITEXTCTL_SetInputMode(pTextCtl,
            CContApp_GetFldInputMode(AEE_ADDRFIELD_NAME));
            /*
            (void)ITEXTCTL_SetTitle( pTextCtl,
                                     CONTAPP_RES_FILE_LANG,
                                     IDS_INPUT_GROUP,
                                     NULL);*/
            {
                AECHAR WTitle[40] = {0};
                //IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
                
#ifdef FEATURE_VERSION_C337
				(void)ISHELL_LoadResString(pMe->m_pShell,
							                CONTAPP_RES_FILE_LANG,
											IDS_CALLER_GROUPS,
											WTitle,
							                sizeof(WTitle));
#else               
                (void)ISHELL_LoadResString(pMe->m_pShell,
							                CONTAPP_RES_FILE_LANG,
											IDS_INPUT_GROUP,
											WTitle,
							                sizeof(WTitle));
#endif                
                

                (void)ITEXTCTL_SetTitle( pTextCtl, NULL,0,WTitle);

                
                if(pMe->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_SetHasTitleText(pMe->m_pIAnn, FALSE);
                    //IANNUNCIATOR_Redraw(pMe->m_pIAnn);                    
                  //  IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
            }                                     
            
            ITEXTCTL_SetMaxSize( pTextCtl,MAX_INPUT_NAME_EN);
            
            pGroup[0] = ONEDIAL_NULL;   
            MSG_FATAL("pGroup[0] = ONEDIAL_NULL",0,0,0);
            if(SUCCESS != CContApp_GetConfig( pMe,
                                              (ContAppCFG)pMe->m_wSelectGroup,
                                              pGroup,
                                              sizeof(pGroup)))
            {
                return EFAILED;
            }

            if(pGroup[0] == ONEDIAL_NULL)
            {
                
                uint16        ResID = 0;
                MSG_FATAL("CContApp_HandleGroupOptEditDlgEvent m_wSelectGroup=%d",pMe->m_wSelectGroup,0,0);
                switch(pMe->m_wSelectGroup)
                {
                    case CONTCFG_GROUP1:
                        ResID = IDS_GROUP_VIP;
                        break;
                        
                    case CONTCFG_GROUP2:
                        ResID = IDS_GROUP_HOME;
                        break;
                        
                    case CONTCFG_GROUP3:
                        ResID = IDS_GROUP_WORK;
                        break;
                        
                    case CONTCFG_GROUP4:
                        ResID = IDS_GROUP_FRIEND;
                        break;
                        
                    case CONTCFG_GROUP5:
                        ResID = IDS_GROUP_PERSONAL;
                        break;
                        
                    case CONTCFG_GROUP6:
                        ResID = IDS_GROUP_BUSINESS;
                        break;
                    default:
                        ResID = IDS_GROUP_NONE_STRING;
                        break;
                }

                ISHELL_LoadResString ( pMe->m_pShell, 
                                    CONTAPP_RES_FILE_LANG, 
                                    ResID,
                                    pGroup,
                                    sizeof(pGroup));          
            }
            
            (void)ITEXTCTL_SetText(pTextCtl, pGroup, -1);
            
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;                        
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_KEY_RELEASE:
        case EVT_USER_REDRAW:
            {          
                AEETextInputMode nInputMode; 
                nInputMode = ITEXTCTL_GetInputMode(pTextCtl,NULL); 
                if (nInputMode == AEE_TM_SYMBOLS)
                {
                    return TRUE;
                }
                #ifndef FEATURE_ALL_KEY_PAD   //add by yangdecai
                if ( WSTRLEN(ITEXTCTL_GetTextPtr(pTextCtl)) > 0 && ITEXTCTL_GetT9End(pTextCtl) != TC_CURSORSTART )
                {
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_DELETE);
                }
                else
                #endif
                {
                    CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_BACK);            
                }
                IDISPLAY_Update(pMe->m_pDisplay);  
            }
            return TRUE;

        case EVT_CTL_TEXT_MODECHANGED:  //切换输入法
            {
                //CContApp_SetFldMaxSize(pMe,pTextCtl,pMe->m_nFldInputID);  //Del by zzg 2012_02_29
            }
            return TRUE; 
            
        case EVT_DIALOG_END:
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_INFO:
                case AVK_SELECT:
                    sGroup = WSTRDUP(ITEXTCTL_GetTextPtr(pTextCtl));                    
                    if(WSTRLEN(sGroup) > 0)
                    {                    	
                        (void)CContApp_SetConfig( pMe,
                                                  (ContAppCFG)pMe->m_wSelectGroup,
                                                  sGroup,
                                                  sizeof(AECHAR)*(MAX_INPUT_NAME_EN+1));                       

                    }
                    CLOSE_DIALOG(DLGRET_OK);                     
                    return TRUE;
                
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                    
                default:
                    break;
                    
            }
            break;
            
        case EVT_COMMAND:
            break;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						//pMe->m_wselGroupId = IMENUCTL_GetSel(pMenuCtl);
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif             
        default:
            break;
            
    }

    return FALSE;
} // CContApp_HandleGroupOptEditDlgEvent

/*==============================================================================
函数：
       CContApp_HandleDeleteSelectDlgEvent
说明：
       IDD_DELETE_SELECT 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleDeleteSelectDlgEvent( CContApp  *pMe,
                                                AEEEvent   eCode,
                                                uint16     wParam,
                                                uint32     dwParam)
{
    IMenuCtl *pMenuCtl;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_MENU_DELETE_SELECT);
                                              
    MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect        rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
           
            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_START",0,0,0);
            
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL | MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif
			#if 0
            IMENUCTL_SetTitle( pMenuCtl,CONTAPP_RES_FILE_LANG,IDS_DELETE_CONTACTS,NULL);
            #else
			{
				AECHAR WTitle[40] = {0};
                //IANNUNCIATOR_SetFieldIsActiveEx(pMe->m_pIAnn,FALSE);
				(void)ISHELL_LoadResString(pMe->m_pShell,
						CONTAPP_RES_FILE_LANG,								  
						IDS_DELETE_CONTACTS,
						WTitle,
						sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
				    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
			}
			#endif

            CContApp_BuildDeleteSelectMenu(pMe,pMenuCtl);
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;                         
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            else
            {
                // Set menu select
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wDeleteSelectSel);
            }
            
            // Draw prompt bar here
            CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    pMe->m_wDeleteSelectSel = 0;
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                    
                default:
                    break;
                    
            }
            break;
            
        case EVT_COMMAND:
            pMe->m_wDeleteSelectSel = wParam;
            switch(wParam)
            {
                case IDS_PHONE_CAPACITY:
                    //pMe->m_nViewType = CONTCFG_VIEWTYPE_PHONE;
                    CContApp_LoadPhoneBook(pMe, CONTCFG_VIEWTYPE_PHONE);
                    CLOSE_DIALOG(DLGRET_PHONE_MEMORY);
                    return TRUE;

                case IDS_UIM_CARD:
                    //pMe->m_nViewType = CONTCFG_VIEWTYPE_RUIM;
                    CContApp_LoadPhoneBook(pMe, CONTCFG_VIEWTYPE_RUIM);
                    CLOSE_DIALOG(DLGRET_UIM_CARD);
                    return TRUE;

                case IDS_DELETE_ALL:
                    CContApp_LoadPhoneBook(pMe, CONTCFG_VIEWTYPE_ALL);
                    CLOSE_DIALOG(DLGRET_DELETE_ALL);
                    return TRUE;
                default:
                    break;
            }
            break;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
	case EVT_PEN_UP:
		{
			AEEDeviceInfo devinfo;
			int nBarH ;
			AEERect rc;
			int16 wXPos = (int16)AEE_GET_X(dwParam);
			int16 wYPos = (int16)AEE_GET_Y(dwParam);

			nBarH = GetBottomBarHeight(pMe->m_pDisplay);
	
			MEMSET(&devinfo, 0, sizeof(devinfo));
			ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
			SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
			MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

			if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
			{
				if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
				{
					boolean rt;
					pMe->m_wDeleteSelectSel = IMENUCTL_GetSel(pMenuCtl);
				    rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,pMe->m_wDeleteSelectSel);
					return rt;
				}
				else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
				{						
					 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
					 return rt;
				}
			}

		}
		break;
#endif 

        default:
            break;
            
    }

    return FALSE;
} // CContApp_HandleFldOptsDlgEvent

/*==============================================================================
函数：
       CContApp_HandlePopNumFldDlgEvent
说明：
       IDD_POPNUMFLD 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandlePopNumFldDlgEvent( CContApp  *pMe,
                                                AEEEvent   eCode,
                                                uint16     wParam,
                                                uint32     dwParam)
{
    IMenuCtl *pMenuCtl;
    sSelectFieldInfo  curSelectFieldInfo;  
    AEEAddrField *pFld;
    uint16 rtn = TYPE_NO_MATCH; 
    static dword newMask = 0;
    static int maskNum = 0;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_POPNUMFLD_MENU);
                                              
    MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);

	MSG_FATAL("***zzg ContApp PopNumFldGld eCode=%x***", eCode, 0, 0);
	
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect        rc={0};
            AEERect        topLine={0};
            
            AEEDeviceInfo devinfo;
            int i, index;
            AEEAddrField *pFld;

            maskNum = 0;
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = CContApp_GetCurrFldNum(pMe, AEE_ADDRFIELD_PHONE_GENERIC) * MENUITEM_HEIGHT; 
            rc.y = devinfo.cyScreen - rc.dy - GetBottomBarHeight(pMe->m_pDisplay);
            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_START",0,0,0);

            topLine.x = 0;
            topLine.y = devinfo.cyScreen - GetBottomBarHeight(pMe->m_pDisplay) - rc.dy - 2;;
            topLine.dy = 2;
            topLine.dx = rc.dx;
            
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL | MP_MULTI_SEL);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif
            // build menu
            if(SUCCESS != CContApp_BuildViewMenu(pMe, pMenuCtl))
            {
                CLOSE_DIALOG(DLGRET_ERR);
                return TRUE;
            }

            //(CContApp_GetCurrFldNum(pMe, AEE_ADDRFIELD_PHONE_GENERIC) > 1)
            //IMENUCTL_SetPopMenuRect(pMenuCtl); 
 /*           IMENUCTL_GetRect(pMenuCtl, &rc);
            rc.dx = devinfo.cxScreen;
            IMENUCTL_SetRect(pMenuCtl, &rc);
*/
            InitCheckBox(pMenuCtl);
            
            MEMSET(&curSelectFieldInfo, 0, sizeof(curSelectFieldInfo));  
            curSelectFieldInfo.wIndex = pMe->m_wSelectCont;
            
            if(TYPE_INDEX_MATCH <= CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo))
            {
                curSelectFieldInfo.wIndex = pMe->m_wSelectCont;
                //curSelectFieldInfo.wFieldCount = pMe->m_wFieldCount;
                // check phone number
                curSelectFieldInfo.wFieldMask = MASK_ADDRFIELD_PHONE_GENERIC;
                if(TYPE_FIELD_MATCH == CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo))
                {
                    for(i=0; i<IMENUCTL_GetItemCount(pMenuCtl); i++)
                    {
                        index = IMENUCTL_GetItemID(pMenuCtl, i);
                        pFld = IVector_ElementAt( pMe->m_pFldIv, (uint32)index );
                        if(pFld->fID == AEE_ADDRFIELD_PHONE_GENERIC)
                        {
                             maskNum++;
                             newMask |= MASK_ADDRFIELD_PHONE_GENERIC; 
                             //SetCheckBoxItem(pMenuCtl, index, TRUE);
                             IMENUCTL_SetSelEx(pMenuCtl, index, TRUE);
                             break;
                        }
                    }
                }

                //check home number
                curSelectFieldInfo.wFieldMask = MASK_ADDRFIELD_PHONE_HOME;
                if(TYPE_FIELD_MATCH == CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo))
                {
                    for(i=0; i<IMENUCTL_GetItemCount(pMenuCtl); i++)
                    {
                        index = IMENUCTL_GetItemID(pMenuCtl, i);
                        pFld = IVector_ElementAt( pMe->m_pFldIv, (uint32)index );
                        
                        if(pFld->fID == AEE_ADDRFIELD_PHONE_HOME)
                        {
                            maskNum++;
                             newMask |= MASK_ADDRFIELD_PHONE_HOME; 
                             //SetCheckBoxItem(pMenuCtl, index, TRUE);
                             IMENUCTL_SetSelEx(pMenuCtl, index, TRUE);
                             break;
                        }
                    }
                }

                //check work number
                curSelectFieldInfo.wFieldMask = MASK_ADDRFIELD_PHONE_WORK;
                if(TYPE_FIELD_MATCH == CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo))
                {
                    for(i=0; i<IMENUCTL_GetItemCount(pMenuCtl); i++)
                    {
                        index = IMENUCTL_GetItemID(pMenuCtl, i);
                        pFld = IVector_ElementAt( pMe->m_pFldIv, (uint32)index );
                        if(pFld->fID == AEE_ADDRFIELD_PHONE_WORK)
                        {
                             maskNum++;
                             newMask |= MASK_ADDRFIELD_PHONE_WORK; 
                             //SetCheckBoxItem(pMenuCtl, index, TRUE);
                             IMENUCTL_SetSelEx(pMenuCtl, index, TRUE);
                             break;
                        }
                    }
                }

                //check fax number
                curSelectFieldInfo.wFieldMask = MASK_ADDRFIELD_PHONE_FAX;
                if(TYPE_FIELD_MATCH == CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo))
                {
                    for(i=0; i<IMENUCTL_GetItemCount(pMenuCtl); i++)
                    {
                        index = IMENUCTL_GetItemID(pMenuCtl, i);
                        pFld = IVector_ElementAt( pMe->m_pFldIv, (uint32)index );
                        if(pFld->fID == AEE_ADDRFIELD_PHONE_FAX)
                        {
                             maskNum++;
                             newMask |= MASK_ADDRFIELD_PHONE_FAX; 
                             //SetCheckBoxItem(pMenuCtl, index, TRUE);
                             IMENUCTL_SetSelEx(pMenuCtl, index, TRUE);
                             break;
                        }
                    }
                }
            }
            //这个判断是为了避免重复绘画背影图片IDB_BGMASK
            //if(pMe->m_ePreState != STATE_POPNUMFLD)
            {
                IImage*    BgImg;
                //Draw shadow for screen
                BgImg = ISHELL_LoadResImage(pMe->m_pShell,
                                        AEE_APPSCOMMONRES_IMAGESFILE,
                                        IDB_BACKGROUND);

                if(STATE_SELECTOPT == pMe->m_ePreState)
                {
                    if(NULL != pMe->m_eBitmap)
                    {
                        IDISPLAY_BitBlt(pMe->m_pDisplay, 
                                        0, 
                                        0,
                                        devinfo.cxScreen,
                                        devinfo.cyScreen - GetBottomBarHeight(pMe->m_pDisplay),
                                        pMe->m_eBitmap,
                                        0,
                                        0,
                                        AEE_RO_COPY);
                        IBITMAP_Release(pMe->m_eBitmap);
                        pMe->m_eBitmap = NULL;
                    }
                }

                if(BgImg != NULL)
                {                  
                    IIMAGE_Draw(BgImg, 0, 0);
                    IIMAGE_Draw(BgImg, 0, 0);
                    IIMAGE_Release(BgImg);
                    BgImg = NULL;
                }
                else
                {
                    pMe->m_wErrStrID = IDS_ERR_LOADPICTURE;
                    CLOSE_DIALOG(DLGRET_ERR);
                    return TRUE;
                }
            }

			//IDisplay_FillRect(pMe->m_pDisplay, &topLine, RGB_WHITE);

            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;                                    
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            else
            {
                // Set menu select
                pMe->m_wSelectFldOpts = IMENUCTL_GetItemID(pMenuCtl, 0);
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectFldOpts);
            }
            
            // Draw prompt bar here
            CONTAPP_DRAW_BOTTOMBAR(BTBAR_OK_BACK);
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            newMask = 0;
            maskNum = 0;
            return TRUE;

        case EVT_KEY_PRESS :
            break;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    newMask = 0;
                    pMe->m_wSelectFldOpts = 0;
                    pMe->m_eTotalSelectNum += (maskNum - CContApp_GetSelNodeMarkNum(newMask));
                    CLOSE_DIALOG(DLGRET_OK);
                    return TRUE;
                    
                default:
                    break;
                    
            }
            break;
            
        case EVT_KEY_RELEASE:
            // 如果在EVT_USER_REDRAW事件到来之前，EVT_KEY_PRESS:AVK_RIGHT事件起来，因为pMe->m_bAppReady == FALSE
            // EVT_KEY_PRESS:AVK_RIGHT就不会上传到本dialog, 而且本dialog的menu是多选的，EVT_KEY:AVK_RIGHT在控件上
            // 做了勾选的处理，看起来菜单是被勾选了的，实际上因为没有EVT_KEY_PRESS:AVK_RIGHT，逻辑上菜单对应的号码
            // 还没有标记到newMask。
            
           //if(AVK_SELECT == wParam || AVK_INFO == wParam)
           if(AVK_SELECT == wParam )		//Add By zzg 2012_01_05
            {
            	MSG_FATAL("***zzg HandlePopNumFldDlg EVT_KEY_RELEASE wParam=%x***", wParam, 0, 0);
				
                //pMe->m_wSelectFldOpts = wParam;
                curSelectFieldInfo.wIndex = pMe->m_wSelectCont;
                curSelectFieldInfo.wFieldMask = newMask;
                pMe->m_wFieldIndex = newMask;
                                
                rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);

				MSG_FATAL("***zzg CContApp_FindSelectFieldListNode rtn=%x***", rtn, 0, 0);
				
                if (rtn == TYPE_NO_MATCH && newMask != 0)
                {
                    CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                }
                else
                {

                    CContApp_SetSelectNodeFieldMark(pSelectFieldListRoot, &curSelectFieldInfo);
                    if(pCurSelectFieldNode->SelectFieldInfo.wFieldMask == 0)
                    {
                        CContApp_DeleteSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                    }
                }
               
                newMask = 0;
                maskNum = 0;
                pMe->m_wSelectFldOpts = 0;
                CLOSE_DIALOG(DLGRET_OK);
                return TRUE;
            }

            //if(AVK_RIGHT == wParam)
            if(AVK_RIGHT == wParam || AVK_INFO == wParam)	//Add By zzg 2012_01_05
            {
                dword oldMask = newMask;

				boolean bsel = FALSE;	//Add By zzg 2012_01_05
                
                pFld = IVector_ElementAt( pMe->m_pFldIv, (uint32)pMe->m_wSelectFldOpts );
                
                switch(pFld->fID)
                {
                    //phone number
                    case AEE_ADDRFIELD_PHONE_GENERIC :
                        newMask ^= MASK_ADDRFIELD_PHONE_GENERIC;
                        bsel = newMask & MASK_ADDRFIELD_PHONE_GENERIC;
                        break;
                        
                    //home number
                    case AEE_ADDRFIELD_PHONE_HOME :
                        newMask ^= MASK_ADDRFIELD_PHONE_HOME;
                        bsel = newMask & MASK_ADDRFIELD_PHONE_HOME;
                        break;
                
                    //work number
                    case AEE_ADDRFIELD_PHONE_WORK :
                        newMask ^= MASK_ADDRFIELD_PHONE_WORK;
                        bsel = newMask & MASK_ADDRFIELD_PHONE_WORK;
                        break;

                    //fax number
                    case AEE_ADDRFIELD_PHONE_FAX :
                        newMask ^= MASK_ADDRFIELD_PHONE_FAX;
                        bsel = newMask & MASK_ADDRFIELD_PHONE_FAX;
                        break;
                        
                    default :
                        return TRUE;
                }
                pMe->m_eTotalSelectNum += (CContApp_GetSelNodeMarkNum(newMask) - CContApp_GetSelNodeMarkNum(oldMask));

				//Add By zzg 2012_01_05	
				IMENUCTL_SetSelEx(pMenuCtl, (uint32)pMe->m_wSelectFldOpts, bsel);	
				IMENUCTL_Redraw(pMenuCtl);
				//Add By zzg 2012_01_05	
				
            }
            break;
       #ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,EVT_KEY_RELEASE);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,EVT_KEY_RELEASE);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif 
        case EVT_CTL_SEL_CHANGED :
			MSG_FATAL("EVT_CTL_SEL_CHANGED......",0,0,0);
            pMe->m_wSelectFldOpts = wParam;
            return TRUE;
            
        default:
            break;
            
    }

    return FALSE;
} // CContApp_HandlePopNumFldDlgEvent

//在当前选中菜单位置编辑电话本内容
static void CContApp_ShowEditItem(CContApp  *pMe, IMenuCtl  *pMenuCtl, ITextCtl *pTextCtl)
{
    AEERect     rect={0};
    AECHAR *pwstrText = NULL;
    int nLen=0;
    
        //if(pMe->m_nAddnewOrEdit == ADDOREDIT_EDIT)
        //{
    IMENUCTL_GetSelItemRect( pMenuCtl, &rect);
    DBGPRINTF("ItemRECT %d %d %d %d",rect.x,rect.y,rect.dx,rect.dy);
    #if defined( FEATURE_DISP_320X240)
    rect.x  = 38;//大概的一个数字
    rect.dx = pMe->m_rc.dx - 44;//five pixels for right edge, 
    #elif defined(FEATURE_DISP_240X320)
	rect.x  = 23;//大概的一个数字
    rect.dx = pMe->m_rc.dx - 28;//five pixels for right edge, 
    #else
    rect.x  = 23;//大概的一个数字
    rect.dx = pMe->m_rc.dx - 27;//five pixels for right edge, 
    #endif
    #if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)//add by yangdecai 2011-01-06
	{
	   	nv_language_enum_type language;
       	OEM_GetConfig( CFGI_LANGUAGE_SELECTION,&language,sizeof(language));
       	if(NV_LANGUAGE_ARABIC == language )
       	{
	    	MSG_FATAL("...............................1",0,0,0);
	    	rect.x  = 3;
	    }
    }
	#endif
#ifdef FEATURE_OEMOMH 
    if(pMe->m_pActiveDlgID == IDD_EMERGENCY_CALL)
    {
        IMENUCTL_GetSelItemRect( pMenuCtl, &rect);
    }
#endif                
    //rect.dy -= 1;
    ITEXTCTL_SetRect( pTextCtl, &rect);
    CContApp_SetFldMaxSize(pMe,pTextCtl,pMe->m_nFldInputID);
    // 只有数字域才真正在本地编辑，按数字域的属性设置
    ITEXTCTL_SetProperties( pTextCtl, CContApp_GetFldInputProp(pMe,AEE_ADDRFIELD_PHONE_GENERIC));	
		
    (void)ITEXTCTL_SetInputMode(pTextCtl, CContApp_GetFldInputMode(pMe->m_nFldInputID));

    switch(pMe->m_wSelectEdit)
    {
        case IDI_EDIT_MENU_NAME:
        case IDI_ADDNEW_MENU_NAME:
            //pMe->m_pFldInputBuf = pMe->m_pAddNewName;// 为了item改变以后，把原来的字符串显示出来
            ITEXTCTL_SetText( pTextCtl, pMe->m_pAddNewName, WSTRLEN(pMe->m_pAddNewName));
            //(void)WSTRCPY(pMe->m_tmpBuf,pMe->m_pAddNewName);//check change or not to restore for onedial
            break;
        
        case IDI_EDIT_MENU_MOBILE:
        case IDI_ADDNEW_MENU_MOBILE:
            ITEXTCTL_SetText( pTextCtl, pMe->m_pAddNewMobile, WSTRLEN(pMe->m_pAddNewMobile));
            (void)WSTRCPY(pMe->m_tmpBuf,pMe->m_pAddNewMobile);//check change or not to restore for onedial
            break;
    
        case IDI_EDIT_MENU_HOME:
        case IDI_ADDNEW_MENU_HOME:
            ITEXTCTL_SetText( pTextCtl, pMe->m_pAddNewHome, WSTRLEN(pMe->m_pAddNewHome));
            (void)WSTRCPY(pMe->m_tmpBuf,pMe->m_pAddNewHome);
            break;
    
        case IDI_EDIT_MENU_OFFICE:
        case IDI_ADDNEW_MENU_OFFICE:
            ITEXTCTL_SetText( pTextCtl, pMe->m_pAddNewOffice, WSTRLEN(pMe->m_pAddNewOffice));
            (void)WSTRCPY(pMe->m_tmpBuf,pMe->m_pAddNewOffice);
            break;

        case IDI_EDIT_MENU_FAX:
        case IDI_ADDNEW_MENU_FAX:
            ITEXTCTL_SetText( pTextCtl, pMe->m_pAddNewFax, WSTRLEN(pMe->m_pAddNewFax));
            (void)WSTRCPY(pMe->m_tmpBuf,pMe->m_pAddNewFax);
            break;
          
        case IDI_EDIT_MENU_EMAIL:
        case IDI_ADDNEW_MENU_EMAIL:
            //pMe->m_pFldInputBuf = pMe->m_pAddNewEMAIL;// 为了item改变以后，把原来的字符串显示出来
            ITEXTCTL_SetText( pTextCtl, pMe->m_pAddNewEMAIL, WSTRLEN(pMe->m_pAddNewEMAIL));
            break;

        case IDI_EDIT_MENU_ADDRESS:
        case IDI_ADDNEW_MENU_ADDRESS:
            //pMe->m_pFldInputBuf = pMe->m_pAddNewAddress;// 为了item改变以后，把原来的字符串显示出来
            ITEXTCTL_SetText( pTextCtl, pMe->m_pAddNewAddress, WSTRLEN(pMe->m_pAddNewAddress));
            break;

        case IDI_EDIT_MENU_URL:
        case IDI_ADDNEW_MENU_URL:
            //pMe->m_pFldInputBuf = pMe->m_pAddNewURL;// 为了item改变以后，把原来的字符串显示出来
            ITEXTCTL_SetText( pTextCtl, pMe->m_pAddNewURL, WSTRLEN(pMe->m_pAddNewURL));
            break;

        case IDI_EDIT_MENU_REMARK:
        case IDI_ADDNEW_MENU_REMARK:
            //pMe->m_pFldInputBuf = pMe->m_pAddNewRemark;// 为了item改变以后，把原来的字符串显示出来
            ITEXTCTL_SetText( pTextCtl, pMe->m_pAddNewRemark, WSTRLEN(pMe->m_pAddNewRemark));
            break;
            
        default: 
#ifdef FEATURE_OEMOMH 
            if(pMe->m_pActiveDlgID == IDD_EMERGENCY_CALL)
            {
             //   AECHAR emergency_call[10] = {0};
             //   uint16 menu_id = IMENUCTL_GetSel(pMenuCtl); 
             //   MSG_FATAL("menu_id=%d",menu_id,0,0);
             //   ITEXTCTL_GetText(pTextCtl, emergency_call, 10);
             //   IMENUCTL_SetItemText(pMenuCtl, menu_id, NULL, 0, emergency_call);
                //ITEXTCTL_SetText( pTextCtl, emergency_call, WSTRLEN(emergency_call));
                break;
            }
            else
#endif            
                return;
    }
   
    pwstrText = ITEXTCTL_GetTextPtr(pTextCtl);
    if (NULL != pwstrText)
    {
         nLen = WSTRLEN(pwstrText);
    }
    #ifndef FEATURE_ALL_KEY_PAD           
    if (nLen > 0)
    {
       // Save       Delete
       CONTAPP_DRAW_BOTTOMBAR(BTBAR_SAVE_DELETE)
    }
    else if (nLen <1)
    #endif
    {
        // Save      Back
        CONTAPP_DRAW_BOTTOMBAR(BTBAR_SAVE_BACK)
    }

    //IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);
    IMENUCTL_SetActive(pMenuCtl, FALSE);
    ITEXTCTL_SetActive(pTextCtl, TRUE);
    ITEXTCTL_SetCursorPos(pTextCtl, TC_CURSOREND);
#ifdef FEATURE_OEMOMH     
    if(pMe->m_pActiveDlgID == IDD_EMERGENCY_CALL)
    {
        MSG_FATAL("pMe->m_pActiveDlgID == IDD_EMERGENCY_CALL",0,0,0);
        IDIALOG_SetFocus(pMe->m_pActiveDlg, IDC_TEXT_EMERGENCY_CALL);
    }
    else
#endif
    {
        if(pMe->m_nAddnewOrEdit == ADDOREDIT_ADD)
        {
            IDIALOG_SetFocus(pMe->m_pActiveDlg, IDC_ADD_LOCAL_INPUT);
        }
        else
        {
            IDIALOG_SetFocus(pMe->m_pActiveDlg, IDC_TEXT_LOCAL_INPUT);
        }
    }
    //ITEXTCTL_Redraw(pTextCtl);
}

static boolean CContApp_SaveLocal_Input(CContApp  *pMe, ITextCtl *pTextCtl)
{
    FREEIF(pMe->m_pFldInputBuf);
    pMe->m_pFldInputBuf = WSTRDUP(ITEXTCTL_GetTextPtr(pTextCtl));

                    
    if(WSTRLEN(pMe->m_pFldInputBuf) == 0)
    {
        FREEIF(pMe->m_pFldInputBuf);
    }
    else
    {
        if(IS_NUM_FIELD(pMe->m_nFldInputID))
        {
            if(!CContApp_CheckNumber(pMe->m_pFldInputBuf))
            {
                 CLOSE_DIALOG(DLGRET_NUM_UNAVAILD);
                 return FALSE;
            }
            if(pMe->m_tmpBuf[0] != (AECHAR)'\0' && pMe->m_nAddnewOrEdit == ADDOREDIT_EDIT)
            {
                int i = 0;
                // 检查当前单键拨号中是否存在当前被编辑的记录
                for(i=CONTCFG_ONEDIAL2; i<=CONTCFG_ONEDIAL8; i++)//CONTCFG_ONEDIAL1
                {
                    if(SUCCESS != CContApp_GetConfig( pMe,
                                                    (ContAppCFG)i,
                                                    pMe->m_pOneDialBuf,
                                                    sizeof(pMe->m_pOneDialBuf)))
                    {
                        continue;
                    }

                    // 存在单键拨号则需要修改单键拨号
                    if(WSTRCMP(pMe->m_tmpBuf, pMe->m_pOneDialBuf) == 0)
                    {
                        byte numFldIndx; 
                        for(numFldIndx = 0; numFldIndx<CONTAPP_NUMBERFLDS; numFldIndx++)
                        {
                            //index 重0开始
                            if(TRUE == pMe->m_eOneDialFld[numFldIndx].bMask && pMe->m_eOneDialFld[numFldIndx].fldindx != pMe->m_nFldInputID)
                            {
                                continue;
                            }
                            else
                            {
                                pMe->m_eOneDialFld[numFldIndx].bMask = TRUE;
                                pMe->m_eOneDialFld[numFldIndx].oneDialIndx = i;
                                pMe->m_eOneDialFld[numFldIndx].fldindx = pMe->m_nFldInputID;
                                break;
                            }
                        }
                    }
                }
             }
         }
     }

     //pMe->m_nInputMode = EDIT_MENU_MODE;
     //CLOSE_DIALOG(DLGRET_CONTACT_EDIT);
     return TRUE;
}

static void CContApp_ShowGroupSelect(CContApp  *pMe, IMenuCtl  *pMenuCtl, IMenuCtl  *groupList)
{
    AEERect         rect = {0};
    IImage          *left_arrow=NULL;
    IImage          *right_arrow=NULL;
    AEEImageInfo    info = {0};
    int16           curGroup=0;
    AEEMenuColors color  = {0};
    MSG_FATAL("CContApp_ShowGroupSelect m_nGroupCat=%d",pMe->m_nGroupCat,0,0);
    switch(pMe->m_nGroupCat)
    {
        case AEE_ADDR_CAT_VIP:
            curGroup = IDS_GROUP_VIP;
            break;
            
        case AEE_ADDR_CAT_HOME:
            curGroup = IDS_GROUP_HOME;
            break;

        case AEE_ADDR_CAT_WORK:
            curGroup = IDS_GROUP_WORK;
            break;

        case AEE_ADDR_CAT_FRIEND:
            curGroup = IDS_GROUP_FRIEND;
            break;

        case AEE_ADDR_CAT_PERSONAL:
            curGroup = IDS_GROUP_PERSONAL;
            break;

        case AEE_ADDR_CAT_BUSINESS:
            curGroup = IDS_GROUP_BUSINESS;
            break;

        case AEE_ADDR_CAT_OTHER:
        default:
            curGroup = IDS_GROUP_NONE_STRING;
            break;
    }
    
    //IMENUCTL_SetBottomBarType(groupList, BTBAR_SELECT_BACK);
    //CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
    IMENUCTL_SetOemProperties(groupList, OEMMP_SWITCHNAVIGATIONKEY | OEMMP_IDF_ALIGN_CENTER | OEMMP_DISTINGUISH_INFOKEY_SELECTKEY); 
    IMENUCTL_SetProperties(groupList, MP_NO_REDRAW);
    
    IMENUCTL_SetActive(pMenuCtl, FALSE);
    IMENUCTL_SetActive(groupList, TRUE);

    IMENUCTL_GetSelItemRect( pMenuCtl, &rect);
    rect.x  = 20;//20是大概的一个数字,一个偏移值，是群组那张图片的宽度
    rect.dx = pMe->m_rc.dx - 25;
    rect.dy -= 1;
    IDISPLAY_FillRect(pMe->m_pDisplay, &rect, RGB_BLACK);
    if( NULL == (left_arrow  = ISHELL_LoadResImage(pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDB_LEFTARROW) ) )
    {
        return;
    }
    if( NULL == (right_arrow  = ISHELL_LoadResImage(pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDB_RIGHTARROW) ) )
    {
        return;
    }

    IIMAGE_GetInfo(right_arrow, &info);

    rect.x += (info.cx + 4);
    rect.dx -= (info.cx + 4) * 2;
    IMENUCTL_SetRect(groupList, &rect);
    MSG_FATAL("CContApp_ShowGroupSelect",0,0,0);

    IIMAGE_Draw( left_arrow, rect.x - info.cx - 2, rect.y + 4);
    IIMAGE_Draw( right_arrow, rect.x + rect.dx + 2 , rect.y + 4);
#ifdef FEATURE_LCD_TOUCH_ENABLE
    SETAEERECT(&pMe->M_LGroupRect, rect.x - info.cx - 7, rect.y, info.cx+10, info.cy+10); 
    SETAEERECT(&pMe->M_RGroupRect, rect.x + rect.dx -7 , rect.y , info.cx+10, info.cy+10); 
#endif
    color.wMask = MC_BACK | MC_SEL_BACK;
    color.cBack = RGB_WHITE;
    color.cSelBack = RGB_WHITE;
    IMENUCTL_SetColors( groupList, &color);
    
    IMENUCTL_SetSel( groupList, curGroup);
    IMENUCTL_Redraw(groupList);
    IIMAGE_Release(left_arrow);
    IIMAGE_Release(right_arrow);
    left_arrow = NULL;
    right_arrow = NULL;
    
}

/*==============================================================================
函数：
       CContApp_HandleNunFldDlgEvent
说明：
       IDD_VIEW_NUMFLD 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleNunFldDlgEvent( CContApp  *pMe,
                                             AEEEvent   eCode,
                                             uint16     wParam,
                                             uint32     dwParam)
{
    IMenuCtl *pMenuCtl;
//    IStatic  *pStatic;
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif

    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_VIEWNUMFLD_MENU);

    MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            // Set the edit menu title is first field
            AEERect rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
            
            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("CContApp_HandleNunFldDlgEvent",0,0,0);
			#if 0
            (void)IMENUCTL_SetTitle( pMenuCtl, NULL, 0,
                                     (AECHAR *)CContApp_GetFldBuf(pMe, 0)); 
			#else
			{
			    if(pMe->m_pIAnn != NULL)
                {         
				    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,(AECHAR *)CContApp_GetFldBuf(pMe, 0));
                }
			}
			#endif
            
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY|OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif

            // build menu
            if(SUCCESS != CContApp_BuildViewMenu(pMe, pMenuCtl))
            {
                CLOSE_DIALOG(DLGRET_ERR);
                return TRUE;
           }
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
           return TRUE;
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
            
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
               IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            
            // Draw prompt bar here
            if(pMe->m_wSelectOpts == IDI_OPTS_MENU_VIEW)
            {
               CONTAPP_DRAW_BOTTOMBAR(BTBAR_BACK);  
            }
            else
            {
               CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
            }
            IDISPLAY_Update(pMe->m_pDisplay);
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            return TRUE;
            
        case EVT_KEY_PRESS:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                case AVK_CAMERA:
                
                #if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)
                {
				nv_item_type	SimChoice;
				OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
				if(SimChoice.sim_select ==AVK_SEND_TWO)
				{
                    if(pMe->m_wSelectOpts != IDI_OPTS_MENU_VIEW)
                    {
                        CtlAddItem ai;
                        uint16   wID;
                            
                        wID = IMENUCTL_GetSel(pMenuCtl);
                        if (IMENUCTL_GetItem(pMenuCtl, wID, &ai))
                        {
                            switch(pMe->m_wFiledOptState) 
                            {
#ifndef WIN32 
                                case FIELDSENDSMS:
                                    // Send SMS
                                    (void)CContApp_SendSMS(pMe,  (AECHAR *)ai.pText, NULL);  
                                    break;
                                case FIELDSENDCONTACT:
                                    // Send SMS
                                    (void)CContApp_SendSMS(pMe,  (AECHAR *)ai.pText, pMe->m_pBuf); 
                                    break;
                                case FIELDCALL:
                                    // Make voice call if it is number
                                    //(void)MakeVoiceCall(pMe->m_pShell, FALSE, (AECHAR *)ai.pText);
                                (void)CContApp_MakeCall(pMe, IMENUCTL_GetSel(pMenuCtl));
                                    break;
#ifdef FEATRUE_SET_IP_NUMBER
                                case FIELDIPCALL:
                                {
                                    ICallApp         *pCallApp = NULL;
                                    if( SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                                                            AEECLSID_DIALER,
                                                                            (void **)&pCallApp))
                                    {
                                        return FALSE;
                                    }
                                    ICallApp_Make_IP_Call_F(pCallApp,(AECHAR *)ai.pText);
                                    if (pCallApp) 
                                    {
                                        ICallApp_Release(pCallApp);
                                        pCallApp = NULL;
                                    }
                                }
                                break;
#endif
#endif//win32
                                default:
                                    break;
                            }
                        }
                    } 
                CLOSE_DIALOG(DLGRET_OK);
                return TRUE;
				}
				}
				#endif
				break;
                case AVK_SEND:
                case AVK_INFO:
                case AVK_SELECT:
                    {
                    #if defined(FEATURE_VERSION_C01) 
					{
						nv_item_type	SimChoice;
						OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
						if(SimChoice.sim_select==AVK_SEND_TWO)
						{
							break;
						}
					}
					#endif
                    if(pMe->m_wSelectOpts != IDI_OPTS_MENU_VIEW)
                    {
                        CtlAddItem ai;
                        uint16   wID;
                            
                        wID = IMENUCTL_GetSel(pMenuCtl);
                        if (IMENUCTL_GetItem(pMenuCtl, wID, &ai))
                        {
                            switch(pMe->m_wFiledOptState) 
                            {
#ifndef WIN32 
#ifdef FEATURE_WMS_APP

                                case FIELDSENDSMS:
                                    // Send SMS
                                    (void)CContApp_SendSMS(pMe,  (AECHAR *)ai.pText, NULL);  
                                    break;
                                case FIELDSENDCONTACT:
                                    // Send SMS
                                    (void)CContApp_SendSMS(pMe,  (AECHAR *)ai.pText, pMe->m_pBuf); 
                                    break;
#endif									
                                case FIELDCALL:
                                    // Make voice call if it is number
                                    //(void)MakeVoiceCall(pMe->m_pShell, FALSE, (AECHAR *)ai.pText);
                                (void)CContApp_MakeCall(pMe, IMENUCTL_GetSel(pMenuCtl));
                                    break;
#ifdef FEATRUE_SET_IP_NUMBER
                                case FIELDIPCALL:
                                {
                                    ICallApp         *pCallApp = NULL;
                                    if( SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                                                            AEECLSID_DIALER,
                                                                            (void **)&pCallApp))
                                    {
                                        return FALSE;
                                    }
                                    ICallApp_Make_IP_Call_F(pCallApp,(AECHAR *)ai.pText);
                                    if (pCallApp) 
                                    {
                                        ICallApp_Release(pCallApp);
                                        pCallApp = NULL;
                                    }
                                }
                                break;
#endif
#endif//win32
                                default:
                                    break;
                            }
                        }
                    } 
                }
                CLOSE_DIALOG(DLGRET_OK);
                return TRUE;
                    
                default:
                    break;                    
            }
            break;
            
        case EVT_COMMAND:
            if(pMe->m_wSelectOpts != IDI_OPTS_MENU_VIEW)
            {
                pMe->m_wSelectFld  = wParam;
            }
            return TRUE;
            
        case EVT_CTL_SEL_CHANGED:
            pMe->m_wSelectFld = wParam;
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif             
        default:
            break;
            
    }
    
    return FALSE;
} // CContApp_HandleNunFldDlgEvent

/*==============================================================================
函数：
       CContApp_HandleSelectOptDlgEvent
说明：
       IDD_SELECTOPT 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleSelectOptDlgEvent( CContApp  *pMe,
                                                AEEEvent   eCode,
                                                uint16     wParam,
                                                uint32     dwParam)
{
    IMenuCtl *pMenuCtl;
    sSelectFieldInfo  curSelectFieldInfo;  
    uint16 rtn = TYPE_NO_MATCH; 
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_SELECTOPT_MENU);
    MSG_FATAL("CContApp_HandleSelectOptDlgEvent-----eCode=%d-----wparam=%d----dwparam=%d",eCode,wParam,dwParam);                                          
    MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);
#ifdef FEATURE_LCD_TOUCH_ENABLE
        if(eCode == EVT_PEN_UP)
            {
                int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
                int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
                AEEDeviceInfo devinfo;
                int nBarH ;
                AEERect rc;
                MSG_FATAL("CContApp_HandleOneDialNumFldSelDlgEvent wXPos=%d ,wYPos=%d",wXPos,wYPos,0);
                 
                nBarH = GetBottomBarHeight(pMe->m_pDisplay);
                MEMSET(&devinfo, 0, sizeof(devinfo));
                ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
                SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);  
                if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
                {
                    if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
                    {
                         eCode = EVT_COMMAND;
                         wParam = IMENUCTL_GetSel(pMenuCtl);
                    } 
                    else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//右
                    { 
                        eCode = EVT_KEY;
                         wParam = AVK_CLR;
                    }
                }  
            }                               
#endif  
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect        rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
           
            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_START",0,0,0);
            
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL | MP_BIND_ITEM_TO_NUMBER_KEY);

            CContApp_BuildSelectOptMenu(pMe, pMenuCtl);
            IMENUCTL_SetPopMenuRect(pMenuCtl);
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;                        
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            else
            {
                // Set menu select
                //IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectFldOpts);
                IMENUCTL_SetSel(pMenuCtl, 0);
            }
            
            // Draw prompt bar here
            CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    if(SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType)
                    {
                        if(NULL != pMe->m_eBitmap)
                        {
                            IBITMAP_Release(pMe->m_eBitmap);
                            pMe->m_eBitmap = NULL;
                        }
                    }
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                    
                default:
                    break;
                    
            }
            break;
            
        case EVT_COMMAND:
            pMe->m_wSelectFldOpts = wParam;
            if(wParam != IDS_MARK && wParam != IDS_UNMARK && SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType)
            {
                if(NULL != pMe->m_eBitmap)
                {
                    IBITMAP_Release(pMe->m_eBitmap);
                    pMe->m_eBitmap = NULL;
                }
            }
            switch(wParam)
            {
                case IDS_EXECUTE:		
                    
                    if (SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType)
                    {
                        if(pMe->m_wSelFldType == SINGLE_SELECT_NUMBER)
                        {
                            curSelectFieldInfo.wIndex = pMe->m_wSelectCont;
                            curSelectFieldInfo.wFieldCount = pMe->m_wFieldCount;
                            curSelectFieldInfo.wFieldMask = pMe->m_wFieldIndex;
                            
                            rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                            if (rtn == TYPE_NO_MATCH)
                            {
                                CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                            }
                        }
                        else
                        {
                            if((pSelectFieldListRoot == pCurSelectFieldNode)&&(pCurSelectFieldNode == NULL))
                            {
                                if(pMe->m_wSelFldType == MULTI_SELECT_CONTENT)// prompt no selection when inser content into wms
                                {
                                    CLOSE_DIALOG(DLGRET_NO_SELECTION);
                                    return TRUE;
                                }
                                pMe->m_eTotalSelectNum++;
                                curSelectFieldInfo.wIndex = pMe->m_wSelectCont;
                                curSelectFieldInfo.wFieldMask = CContApp_GetPrimaryNumFld(pMe);
                                CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                            }
                        }
                    }
                    
                    if(SMART_STATE_IDD_COPY == pMe->m_nSmartStateType
                        || SMART_STATE_IDD_DELETE == pMe->m_nSmartStateType
                        || SMART_STATE_IDD_SELECT_RECORD == pMe->m_nSmartStateType)
                    {
                        if((pSelectFieldListRoot == pCurSelectFieldNode)&&(pCurSelectFieldNode == NULL))
                        {
                            CLOSE_DIALOG(DLGRET_NO_SELECTION);
                            return TRUE;
                        }
                        
                        pMe->m_wMainListSel = MENU_SELECT_NULL;
                        pMe->m_wMainListIdx = 0;
                    }

                    if(SMART_STATE_IDD_SELECT_RECORD == pMe->m_nSmartStateType)
                    {
                        char buf[20] = {0};
                        char* str = NULL;
                        
                        SPRINTF(buf, "%c%d", 'A', (uint32)pSelectFieldListRoot);

                        str = STRDUP(buf);
                        ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_BTUIAPP, str);
                        return TRUE;
                    }
                    
                    CLOSE_DIALOG(DLGRET_OK);
                    break;					

                case IDS_MARK:
                case IDS_UNMARK:
                    
                    MEMSET(&curSelectFieldInfo, 0, sizeof(curSelectFieldInfo));    
                    
                    curSelectFieldInfo.wIndex = pMe->m_wSelectSmart;
                    //curSelectFieldInfo.wFieldCount = pMe->m_wFieldCount;
                    //curSelectFieldInfo.wFieldMask = pMe->m_wFieldIndex;
                    if(SUCCESS != CContApp_LoadAddrFlds( pMe, 
                                                         pMe->m_wSelectCont,
                                                         SELECT_ALL))
                    {
                        pMe->m_wErrStrID = IDS_ERR_LOADFIELDS;
                        CLOSE_DIALOG(DLGRET_ERR);
                        return TRUE;
                    }

                    if(CContApp_GetCurrFldNum(pMe, AEE_ADDRFIELD_PHONE_GENERIC) > 1)
                    {
                        if(SMART_STATE_IDD_COPY == pMe->m_nSmartStateType
                            || SMART_STATE_IDD_DELETE == pMe->m_nSmartStateType
                            || SMART_STATE_IDD_SELECT_RECORD == pMe->m_nSmartStateType)
                        {
                            pMe->m_wFieldIndex = CContApp_GetOneNumberFld(pMe, IDX_NUM_RUIM_FLD);
                            curSelectFieldInfo.wFieldMask = CContApp_GetPrimaryNumFld(pMe);
                            
                            rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                            if (rtn == TYPE_NO_MATCH)
                            {
                                pMe->m_eTotalSelectNum++;
                                CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                            }
                            else
                            {
                                pMe->m_eTotalSelectNum--;
                                CContApp_DeleteSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                            }
                        }

                        // insert  content into wms, if only have one numfld don't need to pop another dialog
                        if(pMe->m_eStartMethod == STARTMETHOD_SELECTFIELD && pMe->m_wSelFldType == MULTI_SELECT_CONTENT)
                        {
                            //这里不考虑中国电信版本的只有姓名没有号码的情况，统一跳到多选界面选择
                            CLOSE_DIALOG(DLGRET_SELECT_FLD);
                            return TRUE;
                        }
                        if(SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType)
                        {
                            CLOSE_DIALOG(DLGRET_POPNUMFLD);
                            return TRUE;
                        }
                    }
                    else
                    {
                        if(SMART_STATE_IDD_COPY == pMe->m_nSmartStateType
                            || SMART_STATE_IDD_DELETE == pMe->m_nSmartStateType
                            || SMART_STATE_IDD_SELECT_RECORD == pMe->m_nSmartStateType)
                        {
                            pMe->m_wFieldIndex = CContApp_GetOneNumberFld(pMe, IDX_NUM_RUIM_FLD);
                            //curSelectFieldInfo.wFieldMask = CContApp_GetPrimaryNumFld(pMe);
                            
                            rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                            if (rtn == TYPE_NO_MATCH)
                            {
                                pMe->m_eTotalSelectNum++;
                                CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                            }
                            else
                            {
                                pMe->m_eTotalSelectNum--;
                                CContApp_DeleteSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                            }
                            
                        }
                        else if(SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType)
                        {
                            if(NULL != pMe->m_eBitmap)
                            {
                                IBITMAP_Release(pMe->m_eBitmap);
                                pMe->m_eBitmap = NULL;
                            }
                            
                            pMe->m_wFieldIndex = CContApp_GetOneNumberFld(pMe, IDX_NUM_RUIM_FLD);
                            curSelectFieldInfo.wFieldMask = pMe->m_wFieldIndex;
                            rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                            if (rtn == TYPE_NO_MATCH)
                            {
                                pMe->m_eTotalSelectNum++;
                                CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                                if (pMe->m_wSelFldType == SINGLE_SELECT_NUMBER && SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType)
                                {
                                    CLOSE_DIALOG(DLGRET_OK);
                                    return TRUE;
                                }
                            }
                            else if (rtn == TYPE_INDEX_MATCH)
                            {
                                CContApp_UpdateSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                                if(pCurSelectFieldNode->SelectFieldInfo.wFieldMask == 1)
                                {
                                    pMe->m_eTotalSelectNum--;
                                    CContApp_DeleteSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                                }
                            }
                            else if (rtn == TYPE_FIELD_MATCH)
                            {
                                CContApp_UpdateSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                                if(pCurSelectFieldNode->SelectFieldInfo.wFieldMask == 1)
                                {
                                    CContApp_DeleteSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                                }
                            }
                        }
                    }
                    CLOSE_DIALOG(DLGRET_MARKCHG);
                    break;
                    
                case IDS_MARKALL:
                    CContApp_MarkAll(pMe);
                    CLOSE_DIALOG(DLGRET_MARKCHG);
                    break;

                case IDS_UNMARKALL:
                    CContApp_FreeSelectFieldListNode();
                    pMe->m_eTotalSelectNum = 0;
                    CLOSE_DIALOG(DLGRET_MARKCHG);
                    break;
                    
                default:
                    break;
            }
            break;
        default:
            return FALSE;
            
    }

    return TRUE;
} // CContApp_HandleSelectOptDlgEvent

/*==============================================================================
函数：
       CContApp_HandleNunFldViewDlgEvent
说明：
       IDD_NUMFLDVIEW 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleNunFldViewDlgEvent( CContApp  *pMe,
                                                AEEEvent   eCode,
                                                uint16     wParam,
                                                uint32     dwParam)
{
    IMenuCtl *pMenuCtl;
    
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_NUMFLDVIEW_MENU);
                                              
    MENU_AUTO_SCROLL(pMenuCtl, eCode, wParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
            
        case EVT_DIALOG_START:
        {
            AEERect        rc={0};
            AEEDeviceInfo devinfo={0};
            
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
           
            IMENUCTL_SetRect(pMenuCtl, &rc);
            MSG_FATAL("EVT_DIALOG_START",0,0,0);
            
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL);
            IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenuCtl, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
#endif
			#if 0
            (void)IMENUCTL_SetTitle( pMenuCtl,
                                     NULL,
                                     0,
                                     (AECHAR *)CContApp_GetFldBuf(pMe, 0));
			#else
			{
				if(pMe->m_pIAnn != NULL)
                {            
				    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,(AECHAR *)CContApp_GetFldBuf(pMe, 0));
                }
			}
			#endif


            CContApp_BuildNumFldMenuMenu(pMe, pMenuCtl);
            
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
                                    
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
            // Restore the menu select from suspend
            if(pMe->m_wSelectStore != MENU_SELECT_NULL)
            {
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
            }
            else
            {
                // Set menu select
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectFld);
            }
            
            // Draw prompt bar here
            CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
            IDISPLAY_Update(pMe->m_pDisplay);  
            return TRUE;
            
        case EVT_DIALOG_END:
            // Store the menu select if applet is suspended.
            if(pMe->m_bSuspending)
            {
                pMe->m_wSelectStore = IMENUCTL_GetSel(pMenuCtl);
            }
            else
            {
                pMe->m_wSelectStore = MENU_SELECT_NULL;
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                    
                default:
                    break;
                    
            }
            break;
            
        case EVT_COMMAND:
            //pMe->m_wSelectFldOpts = wParam;
            pMe->m_wSelectFld = wParam;
            switch(wParam)
            {
                case IDI_ADDNEW_MENU_MOBILE:
                    pMe->m_wNewNumFldMask = MASK_ADDRFIELD_PHONE_GENERIC;
                    break;

                case IDI_ADDNEW_MENU_HOME:
                    pMe->m_wNewNumFldMask = MASK_ADDRFIELD_PHONE_HOME;
                    break;

                case IDI_ADDNEW_MENU_OFFICE:
                    pMe->m_wNewNumFldMask = MASK_ADDRFIELD_PHONE_WORK;
                    break;

                case IDI_ADDNEW_MENU_FAX:
                    pMe->m_wNewNumFldMask = MASK_ADDRFIELD_PHONE_FAX;
                    break;

                default:
                    pMe->m_wErrStrID = IDS_ERR_LOADFIELDS;
                    CLOSE_DIALOG(DLGRET_ERR);
                    return TRUE;
            }
            CLOSE_DIALOG(DLGRET_OK);
            break;
      #ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);

				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
                MSG_FATAL("devinfo.cyScreen-nBarH=%d----devinfo.cxScreen=%d----nBarH=%d",devinfo.cyScreen-nBarH,devinfo.cxScreen,nBarH);

				if(CONTAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						
						boolean rt ;
						pMe->m_wSelectFld = IMENUCTL_GetSel(pMenuCtl);
						rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_SELECT,pMe->m_wSelectFld);
						return rt;
					}
					
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif        
        default:
            break;
            
    }

    return FALSE;
} // CContApp_HandleFldOptsDlgEvent

static void CContApp_DrawNorecord(CContApp *pMe, IMenuCtl *pMenuCtl)
{
    AEERect rc={0};
    AECHAR wStrBuf[64] = {(AECHAR)'\0'};//use but find other err :key seem no active when second press
    RGBVAL  oldColor=0; 
    
    //CContApp_DisplayRightTopStr(pMe,pMenuCtl, wParam);
    if(pMenuCtl)
    {
        IMENUCTL_GetRect(pMenuCtl, &rc);
    }
    
    rc.y = 0;//pMe->m_nLineHeight * 2;
    /*
    if (SMART_STATE_IDD_SELECT != pMe->m_nSmartStateType)
    {
        rc.dy -= GetTitleBarHeight(pMe->m_pDisplay);//pMe->m_nLineHeight * 2;
    }
    */

	//rc.dy -= (2); 
	
    //IDISPLAY_FillRect(pMe->m_pDisplay, &rc, RGB_BLACK);
#ifdef FEATURE_CARRIER_CHINA_VERTU
    {
        IImage *pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
        
        Appscommon_ResetBackground(pMe->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &rc, 0, 0);
        if(pImageBg != NULL)
        {
            IImage_Release(pImageBg);
        }
    }
#else
    Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &rc, TRUE);
#endif
    
    (void)ISHELL_LoadResString(pMe->m_pShell,
                                CONTAPP_RES_FILE_LANG,
                                IDS_MSG_FINDNULL,
                                wStrBuf,sizeof(wStrBuf));
                                
    oldColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
    (void)IDISPLAY_DrawText( pMe->m_pDisplay,
                            AEE_FONT_NORMAL, //AEE_FONT_BOLD,
                            wStrBuf,
                            -1,
                            rc.x,
                            rc.y,
                            &rc,
                            IDF_TEXT_TRANSPARENT);
    
    IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, oldColor);
}
static void CContApp_SetGroupItemText(CContApp *pMe, IMenuCtl *pMenuCtl)
{
    int ResID, len;
    AECHAR pGroup[MAX_INPUT_NAME_EN + 1];
    pGroup[0] = ONEDIAL_NULL;
    MSG_FATAL("CContApp_SetGroupItemText m_nGroupCat=%d",pMe->m_nGroupCat,0,0);
    switch(pMe->m_nGroupCat)
    {
        case AEE_ADDR_CAT_VIP:
            pMe->m_wSelectGroup = CONTCFG_GROUP1;
            ResID = IDS_GROUP_VIP;
            break;
        case AEE_ADDR_CAT_HOME:
            pMe->m_wSelectGroup = CONTCFG_GROUP2;
            ResID = IDS_GROUP_HOME;
            break;
        case AEE_ADDR_CAT_WORK:
            pMe->m_wSelectGroup = CONTCFG_GROUP3;
            ResID = IDS_GROUP_WORK;
            break;
        case AEE_ADDR_CAT_FRIEND:
            pMe->m_wSelectGroup = CONTCFG_GROUP4;
            ResID = IDS_GROUP_FRIEND;
            break;
        case AEE_ADDR_CAT_PERSONAL:
            pMe->m_wSelectGroup = CONTCFG_GROUP5;                       
            ResID = IDS_GROUP_PERSONAL;  
            break;
        case AEE_ADDR_CAT_BUSINESS:
            pMe->m_wSelectGroup = CONTCFG_GROUP6;                       
            ResID = IDS_GROUP_BUSINESS;  
            break;
        case AEE_ADDR_CAT_OTHER:
            pMe->m_wSelectGroup = CONTCFG_GROUP7;                       
            ResID = IDS_GROUP_NONE_STRING;  
            break;
        default:
            pMe->m_wSelectGroup = 0;                      
            ResID = IDS_GROUP_NONE_STRING;
            break;
    }
    if(pMe->m_wSelectGroup != 0)
    {
        if(SUCCESS != CContApp_GetConfig( pMe,
                                          (ContAppCFG)pMe->m_wSelectGroup,
                                          pGroup,
                                          sizeof(pGroup)))
        {
            return ;
        } 
    }
    if(pGroup[0] == ONEDIAL_NULL)
    {                        
        len = ISHELL_LoadResString ( pMe->m_pShell, 
                            CONTAPP_RES_FILE_LANG, 
                            ResID,
                            pGroup,
                            sizeof(pGroup)); 
        if(len <= 0 )
        {
            return ;
        }                        
    }    
    if(ADDOREDIT_ADD == pMe->m_nAddnewOrEdit)
    {
        IMENUCTL_SetItemText(pMenuCtl, IDI_ADDNEW_MENU_SELECTGROUP, NULL, 0, pGroup);
    }
    else
    {
        IMENUCTL_SetItemText(pMenuCtl, IDI_EDIT_MENU_GROUP, NULL, 0, pGroup);
    }
}

//在edit界面，保存select group， selectGroup是当前选中的group
static void CContApp_SetEditGroup(CContApp *pMe, IMenuCtl *pMenuCtl, boolean onlySaveValue, int selectGroup)
{
    MSG_FATAL("CContApp_SetEditGroup selectGroup=%d",selectGroup,0,0);
    switch(selectGroup)
    { 
        case IDS_GROUP_VIP:
            pMe->m_nGroupCat = AEE_ADDR_CAT_VIP;
            break;
            
        case IDS_GROUP_HOME:
            pMe->m_nGroupCat = AEE_ADDR_CAT_HOME;
            break;
        
        case IDS_GROUP_WORK:
            pMe->m_nGroupCat = AEE_ADDR_CAT_WORK;
            break;
        
        case IDS_GROUP_FRIEND:
            pMe->m_nGroupCat = AEE_ADDR_CAT_FRIEND;
            break;
        
        case IDS_GROUP_PERSONAL:
            pMe->m_nGroupCat = AEE_ADDR_CAT_PERSONAL;
            break;
            
        case IDS_GROUP_BUSINESS:
            pMe->m_nGroupCat = AEE_ADDR_CAT_BUSINESS;
            break;
            
        case IDS_GROUP_NONE_STRING:
            pMe->m_nGroupCat = AEE_ADDR_CAT_OTHER;
            break;
            
        default:
            pMe->m_nGroupCat = AEE_ADDR_CAT_NONE;
            break;
    }

    if(onlySaveValue)
    {
        return ;
    }
    
    CContApp_SetGroupItemText(pMe, pMenuCtl);
    //IMENUCTL_SetActive(groupList, FALSE);
    IMENUCTL_SetActive(pMenuCtl, TRUE);
    //IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectEdit);
    pMe->m_nInputMode = EDIT_MENU_MODE;
    //CLOSE_DIALOG(DLGRET_CONTACT_EDIT);
}

static void CContApp_SetMenuItemText(CContApp *pMe, IMenuCtl *pMenuCtl, int selectEdit)
{
    int ResID = 0;
    int len;
    AECHAR* itemText = NULL;
    AECHAR text[MAX_INPUT_ADDRESS + 1];
    
    text[0] = ONEDIAL_NULL;
    MSG_FATAL("text[0] = ONEDIAL_NULL",0,0,0);
    switch(selectEdit)
    {
        case IDI_ADDNEW_MENU_NAME:
        case IDI_EDIT_MENU_NAME:
            itemText = pMe->m_pAddNewName;
            ResID = IDS_NAME;
            break;
            
        case IDI_EDIT_MENU_MOBILE:
        case IDI_ADDNEW_MENU_MOBILE:      
            itemText = pMe->m_pAddNewMobile;
            ResID = IDS_MOBILE;
            break;
            
        case IDI_EDIT_MENU_HOME:
        case IDI_ADDNEW_MENU_HOME:
            itemText = pMe->m_pAddNewHome;
            ResID = IDS_HOME;
            break;
            
        case IDI_EDIT_MENU_OFFICE:
        case IDI_ADDNEW_MENU_OFFICE:  
            itemText = pMe->m_pAddNewOffice;
            ResID = IDS_OFFICE;
            break;
            
        case IDI_EDIT_MENU_FAX:
        case IDI_ADDNEW_MENU_FAX:
            itemText = pMe->m_pAddNewFax;
            ResID = IDS_FAX;
            break;
            
        case IDI_EDIT_MENU_EMAIL:
        case IDI_ADDNEW_MENU_EMAIL:
            itemText = pMe->m_pAddNewEMAIL;
            ResID = IDS_EMAIL;
            break;
            
        case IDI_EDIT_MENU_ADDRESS:
        case IDI_ADDNEW_MENU_ADDRESS:
            itemText = pMe->m_pAddNewAddress;
            ResID = IDS_ADDRESS;
            break;
            
        case IDI_EDIT_MENU_URL:
        case IDI_ADDNEW_MENU_URL: 
            itemText = pMe->m_pAddNewURL;
            ResID = IDS_URL;
            break;
            
        case IDI_EDIT_MENU_REMARK:
        case IDI_ADDNEW_MENU_REMARK:
            itemText = pMe->m_pAddNewRemark;
            ResID = IDS_NOTES;
            break;

        default: 
            break;
    }

    if(!itemText)
    {
        len = ISHELL_LoadResString ( pMe->m_pShell, 
                            CONTAPP_RES_FILE_LANG, 
                            ResID,
                            text,
                            sizeof(text)); 
        if(len <= 0 )
        {
            return ;
        }    

        itemText = text;
     }

 #ifdef FEATURE_OEMOMH 
    if(pMe->m_pActiveDlgID == IDD_EMERGENCY_CALL)
    {
        ITextCtl *pTextCtl = NULL;
        AECHAR emergency_call[10] = {0};
        pTextCtl = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_TEXT_EMERGENCY_CALL);    
        
        selectEdit = IMENUCTL_GetSel(pMenuCtl); 
        MSG_FATAL("menu_id=%d",selectEdit,0,0);
        ITEXTCTL_GetText(pTextCtl, emergency_call, 10);
        itemText = emergency_call;
        //ITEXTCTL_SetText( pTextCtl, emergency_call, WSTRLEN(emergency_call));
    }
#endif     
     IMENUCTL_SetItemText(pMenuCtl, selectEdit, NULL, 0, itemText);

}

/*==============================================================================
函数：
       CContApp_HandleDeletingDlgEvent
说明：
       IDD_COPYING 对话框事件处理函数
       
参数：
       pMe [in]：指向ContApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CContApp_HandleDeletingDlgEvent( CContApp  *pMe,
                                                AEEEvent   eCode,
                                                uint16     wParam,
                                                uint32     dwParam)
{
     static IStatic * pStatic = NULL;
    if (NULL == pMe)
    {
        return FALSE;
    }
    /*
    if (NULL == pMe->m_pStatic)
    {
        return FALSE;
    }
    */

   if (NULL == pStatic)
   {
        AEERect rect = {0};
        if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                                 AEECLSID_STATIC,
                                                 (void **)&pStatic))
        
        {
            return FALSE;
            ERR("ISHELL_CreateInstance,AEECLSID_STATIC 2",0,0,0);
        }        
        ISTATIC_SetRect(pStatic, &rect);  
   }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;
        
        case EVT_DIALOG_START:            
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_APP_CONTACT,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);
            return TRUE;                        
            // Note: Fall through to the EVT_USER_REDRAW
            //lint -fallthrough
            
        case EVT_USER_REDRAW:
            {
                PromptMsg_Param_type  Msg_Param={0};
                AECHAR  wstrText[MSGBOX_MAXTEXTLEN];

                // 从资源文件取消息内容
                (void)ISHELL_LoadResString(pMe->m_pShell,
                                        CONTAPP_RES_FILE_LANG,                                
                                        IDS_DELETING,
                                        wstrText,
                                        sizeof(wstrText));              

                Msg_Param.ePMsgType = MESSAGE_WAITING;
                Msg_Param.pwszMsg = wstrText;
                Msg_Param.eBBarType = BTBAR_NONE;
                DrawPromptMessage(pMe->m_pDisplay, pStatic, &Msg_Param);   
                IDISPLAY_Update(pMe->m_pDisplay);  

            }           
            return TRUE;
            
        case EVT_FLIP:
            FORCE_COMPLETE_DELETE();
            CContApp_FreeSelectFieldListNode();
            ISTATIC_Release(pStatic);
            pStatic = NULL;
            CLOSE_DIALOG(DLGRET_CANCELED);
            return TRUE;

        case EVT_DIALOG_END:
            FORCE_COMPLETE_DELETE();
            CContApp_FreeSelectFieldListNode();
            ISTATIC_Release(pStatic);
            pStatic = NULL;
            return TRUE;
            
        case EVT_USER_COMPLETE: 
            //CContApp_FreeSelectFieldListNode();
            CLOSE_DIALOG(DLGRET_OK);            
            return TRUE;        

        case EVT_USER_ERR:
            //CContApp_FreeSelectFieldListNode();
            pMe->m_wErrStrID = IDS_MARK_ERR;
            CLOSE_DIALOG(DLGRET_ERR);
            return TRUE;
            
        default:
            break;
            
    }
    
    return FALSE;
} // CContApp_HandleDeletingDlgEvent


/*=============================================================================

FUNCTION:CContApp_DeleteMultipe

DESCRIPTION: 
    将当前话机的记录全部复制到 RUIM 卡

PARAMETERS:
    pMe    :
    wStartID: the Cont ID of Start copy
    
RETURN VALUE:
    SUCCESS   : 
    EMAXRECORD: if the target device is full
    ENORECORD : if no record to copy
    
COMMENTS:
    
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
int CContApp_DeleteMultipe(CContApp *pMe)
{
    ASSERT(pMe != NULL);
    
    pMe->m_wDeleteRecCnt       = 0;
    pMe->m_bForceComplete    = FALSE;

    pMe->m_wDeleteCurrID = 0;
    
    pMe->m_wTotalNum = pMe->m_eTotalSelectNum;
    
    pCurSelectFieldNode = pSelectFieldListRoot;
    CALLBACK_Init( &pMe->m_nDeleteCB, 
                  (PFNNOTIFY)CContApp_DeleteMultipe_CB,
                   (void *)pMe);
                   
    pMe->m_bDeleteInited = TRUE;
    
    if (!CALLBACK_IsQueued(&pMe->m_nDeleteCB))
    {
        CONTAPP_RESUMECB(&pMe->m_nDeleteCB);
    }
    
    return SUCCESS;
}// CContApp_DeleteMultipe

/*=============================================================================

FUNCTION:CContApp_CopyMultipe

DESCRIPTION: 
    copy or move
    
PARAMETERS:
    pMe    :
    wStartID: the Cont ID of Start copy
    
RETURN VALUE:
    SUCCESS   : 
    EMAXRECORD: if the target device is full
    ENORECORD : if no record to copy
    
COMMENTS:
    
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
int CContApp_CopyMultipe(CContApp *pMe)
{
    ASSERT(pMe != NULL);
    
    pMe->m_wCopyRecCnt       = 0;
    pMe->m_bForceComplete    = FALSE;
    pMe->m_nFailNum = 0;

    if(COPYMULTIPE_TOUIM == pMe->m_nCopyMoveType || MOVEMULTIPE_TOUIM == pMe->m_nCopyMoveType)
    {
        if(IS_MAX_REC(pMe->m_pAddrRUIM))
        {
            CContApp_FreeSelectFieldListNode();
            return EMAXRECORD;
        }
    }
    else
    {
        if(IS_MAX_REC(pMe->m_pAddrPhone))
        {
            CContApp_FreeSelectFieldListNode();
            return EMAXRECORD;
        }
    }
    
    pMe->m_eErrType = FIRST_VALUE;
    pMe->m_wTotalNum = pMe->m_eTotalSelectNum;
    pMe->m_wCopyCurrID = 0;
    
    pCurSelectFieldNode = pSelectFieldListRoot;
    
    CALLBACK_Init( &pMe->m_nCopyCB, 
                  (PFNNOTIFY)CContApp_CopyMultipe_CB,
                   (void *)pMe);
                   
    pMe->m_bCopyInited = TRUE;
    
    if (!CALLBACK_IsQueued(&pMe->m_nCopyCB))
    {
        CONTAPP_RESUMECB(&pMe->m_nCopyCB);
    }
    return SUCCESS;
}// CContApp_CopyMultipe

static void CContApp_SetInputMode(CContApp *pMe)
{
    

    switch(pMe->m_wSelectEdit)
    {
        case IDI_ADDNEW_MENU_NAME:
        case IDI_ADDNEW_MENU_EMAIL:
        case IDI_ADDNEW_MENU_ADDRESS:
        case IDI_ADDNEW_MENU_URL: 
        case IDI_ADDNEW_MENU_REMARK:
            
        case IDI_EDIT_MENU_NAME:
        case IDI_EDIT_MENU_EMAIL:
        case IDI_EDIT_MENU_ADDRESS:
        case IDI_EDIT_MENU_URL:
        case IDI_EDIT_MENU_REMARK:
			pMe->m_nInputMode = OPT_TEXT_INPUT;
            break;
            
        case IDI_ADDNEW_MENU_MOBILE:   
        case IDI_ADDNEW_MENU_HOME:
        case IDI_ADDNEW_MENU_OFFICE:  
        case IDI_ADDNEW_MENU_FAX:
            
        case IDI_EDIT_MENU_MOBILE:
        case IDI_EDIT_MENU_HOME:
        case IDI_EDIT_MENU_OFFICE:
        case IDI_EDIT_MENU_FAX:
            pMe->m_nInputMode = LOCAL_NUMBER_INPUT;
            break;

        case IDI_ADDNEW_MENU_SELECTRINGTONE:
        case IDI_EDIT_MENU_RINGTONE:
            pMe->m_nInputMode = EDIT_MENU_MODE;
            break;

        case IDI_ADDNEW_MENU_SELECTGROUP:
        case IDI_EDIT_MENU_GROUP:
            pMe->m_nInputMode = EDIT_GROUP;
            
        default: 
            break;
    }

}

//Add By zzg 2011_12_15
static boolean CContApp_CheckNumberIsInValid(CContApp  *pMe, ITextCtl *pTextCtl)
{
	FREEIF(pMe->m_pFldInputBuf);
	pMe->m_pFldInputBuf = WSTRDUP(ITEXTCTL_GetTextPtr(pTextCtl));

	return CContApp_Search_HasExceptionalChar(pMe->m_pFldInputBuf);	                  	
}


static boolean CContApp_Search_HasExceptionalChar(AECHAR * szContNumber)
{
    int   len = 0;
    int i;
    boolean have = FALSE;
    len = WSTRLEN(szContNumber);

    for(i = 0; i < len; i++)
    {
        if((szContNumber[i] == CONT_PAUSE_AECHAR)
            ||((i != 0) && (szContNumber[i] == CONT_PLUS_AECHAR))
            )
        {
            have = TRUE;
            break;
        }

    }
	MSG_FATAL("***zzg CContApp_Search_HasExceptionalChar i=%d len=%d have=%d***",i,len,have);
    return have;
}
//Add End

static void drawImage( CContApp *pMe, char *resFile, int16 resId, int x, int y)
{

    IImage *image = ISHELL_LoadResImage( pMe->m_pShell, resFile, resId);
    if( image != NULL)
    {

        IIMAGE_Draw( image, x, y);
        IIMAGE_Release( image);
    }
}

#ifdef FEATURE_OEMOMH 
static boolean  CContApp_HandleEmergencyCallDlgEvent(CContApp  *pMe,
                                 AEEEvent   eCode,
                                 uint16     wParam,
                                 uint32     dwParam)
{
    IMenuCtl *pMenu = NULL;
    ITextCtl *pTextCtl = NULL;
    IMenuCtl *pGroupList = NULL; 
    uint32    dwMask;
    static int nEmgRUIMIdx;
    if (NULL == pMe)
    {
        return FALSE;
    }

    pMenu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_MENU_EMERGENCY_CALL);
    pTextCtl = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_TEXT_EMERGENCY_CALL);
    pGroupList = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_GROUP_EMERGENCY_LIST);
    if ((pMenu == NULL) || (pTextCtl == NULL) || (NULL == pGroupList))
    {
        MSG_FATAL("(pMenu == NULL) || (pTextCtl == NULL)",0,0,0);
        return FALSE;
    }
    MENU_AUTO_SCROLL(pMenu, eCode, wParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            dwMask = IDIALOG_GetProperties(pMe->m_pActiveDlg);
            dwMask |= DLG_NOT_SET_FOCUS_AUTO;
            IDIALOG_SetProperties(pMe->m_pActiveDlg, dwMask);
            //IDIALOG_SetFocus(pMe->m_pActiveDlg, IDC_EDIT_MENU);
            return TRUE;

        case EVT_DIALOG_START:            
        {
            AEERect rc={0};
            //AEERingerInfo  ri;
            AEEDeviceInfo devinfo;
            AECHAR  wstrNum[MAX_EMERGENCY_NUM_LEN+1];
            EmergencyNum_Table emerg_tab;
            int   i = 0;
            int nErr;
            MSG_FATAL("EVT_DIALOG_START",0,0,0);
            ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
            rc = pMe->m_rc;
            rc.dy = devinfo.cyScreen;
            rc.dy -= GetBottomBarHeight(pMe->m_pDisplay); 

            IMENUCTL_SetRect(pMenu, &rc);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE |MP_WRAPSCROLL);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE);
            ICONFIG_GetItem(pMe->m_pConfig, CFGI_EMERGENCYNUM_TABLE, (void*)&emerg_tab, sizeof(EmergencyNum_Table));
            nEmgRUIMIdx = -1;
            for(i=0; i<emerg_tab.emert_size; i++)
            {
                wstrNum[0] = 0;
                STRTOWSTR(emerg_tab.emerg_num[i].num_buf, wstrNum, sizeof(wstrNum));
                DBGPRINTF("wstrNum =%S", wstrNum);
                if(WSTRLEN(wstrNum) == 0)
                {
                    wstrNum[0]=L'N';
                    wstrNum[1]=L'O';
                    wstrNum[2]=L'N';
                    wstrNum[3]=L'E';
                    wstrNum[4]=L'\0';
                }
                IMENUCTL_AddItem(pMenu, NULL, NULL, i, wstrNum, NULL);
            }

#ifndef FEATURE_VERSION_C337
            // ADD RUIM Number
            {
                byte  Assnum[RUIM_ECC_NUMBER][RUIM_ECC_MAXSIZE+1];
                int   nNum;
                MEMSET(Assnum, 0x00, sizeof(Assnum));
#ifndef WIN32   
                MSG_FATAL("IRUIM_Get_Ecc_Code",0,0,0);
                if (pMe->m_pIRUIM != NULL)
                {
                    if(SUCCESS == IRUIM_Get_Ecc_Code(pMe->m_pIRUIM,&Assnum[0][0],&nNum,RUIM_ECC_MAXSIZE+1))
                    {
                        int i;
                        int k = 0;
                        byte *temp;
                        nEmgRUIMIdx = emerg_tab.emert_size;
                        MSG_FATAL("Assnum's nNum = %d",nNum,0,0);
                        for(i=0;i<nNum;i++)
                        {
                            temp = &Assnum[i][0];
                            if(STRLEN((char *)temp)>0)
                            {
                                STRTOWSTR((char *)temp, wstrNum, sizeof(wstrNum));
                                IMENUCTL_AddItem(pMenu, NULL, NULL, emerg_tab.emert_size + k, wstrNum, NULL);
                                k++;
                            }
                        }
                    }
                }
#endif//WIN32                
            }
#endif
            
            IMENUCTL_Redraw(pMenu);
            
            if(emerg_tab.emert_size>0)
            {
                AECHAR emergency_call[MAX_EMERGENCY_NUM_LEN+1] = {0};
                pMe->m_nInputMode = LOCAL_NUMBER_INPUT;
                pMe->m_wSelectEdit = 0;
                pMe->m_nFldInputID = AEE_ADDRFIELD_PHONE_GENERIC;
                CContApp_ShowEditItem(pMe, pMenu, pTextCtl);
                STRTOWSTR(emerg_tab.emerg_num[0].num_buf, wstrNum, sizeof(wstrNum));
                ITEXTCTL_SetText( pTextCtl, wstrNum, WSTRLEN(wstrNum));
            }
            else
            {
                pMe->m_nFldInputID = AEE_ADDRFIELD_NONE;
            }
            IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);
            
            // For redraw the dialog
            ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER_REDRAW, 0,0);
            return TRUE;
        }
        case EVT_USER_REDRAW:
        {
            IMENUCTL_SetSel(pMenu, IMENUCTL_GetSel(pMenu));
            (void)IMENUCTL_Redraw(pMenu);
            
            if(nEmgRUIMIdx>=0 && IMENUCTL_GetSel(pMenu)>=nEmgRUIMIdx)
            {
                CONTAPP_DRAW_BOTTOMBAR(BTBAR_SAVE_BACK)
                pMe->m_nFldInputID = AEE_ADDRFIELD_NONE;
                return TRUE;
            }
            
            CContApp_ShowEditItem(pMe, pMenu, pTextCtl);
            ITEXTCTL_SetMaxSize( pTextCtl,MAX_EMERGENCY_NUM_LEN-1);
            ITEXTCTL_SetActive(pTextCtl, TRUE);
            IDISPLAY_Update(pMe->m_pDisplay);
            return TRUE;
        }

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_CTL_SEL_CHANGED:
        {
            CtlAddItem ai;
            
            if(nEmgRUIMIdx>=0 && IMENUCTL_GetSel(pMenu)>=nEmgRUIMIdx)
            {
                CONTAPP_DRAW_BOTTOMBAR(BTBAR_SAVE_BACK)
                pMe->m_nFldInputID = AEE_ADDRFIELD_NONE;
                return TRUE;
            }
            
            pMe->m_nInputMode = LOCAL_NUMBER_INPUT;
            pMe->m_wSelectEdit = wParam;
            pMe->m_nFldInputID = AEE_ADDRFIELD_PHONE_GENERIC;
            CContApp_ShowEditItem(pMe, pMenu, pTextCtl);
            ITEXTCTL_SetMaxSize( pTextCtl,MAX_EMERGENCY_NUM_LEN-1);
            if(IMENUCTL_GetItem(pMenu, wParam, &ai))
            {
                DBGPRINTF("UP ai.pText %d s=%S", wParam, (AECHAR *)ai.pText);
                ITEXTCTL_SetText( pTextCtl, (AECHAR *)ai.pText, WSTRLEN((AECHAR *)ai.pText));
            }
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);        
            return TRUE;
        }

        case EVT_KEY_PRESS:
            switch (wParam)
            {
                case AVK_UP:
                case AVK_DOWN:
                    if(pMe->m_nFldInputID != AEE_ADDRFIELD_NONE)
                    {
                        AECHAR emergency_call[MAX_EMERGENCY_NUM_LEN+1] = {0};
                        
                        ITEXTCTL_GetText(pTextCtl, emergency_call, MAX_EMERGENCY_NUM_LEN);
                        if(!CContApp_CheckNumber(emergency_call))
                        {
                            CLOSE_DIALOG(DLGRET_NUM_UNAVAILD);
                            return TRUE;
                        }
                        
                        DBGPRINTF("emergency_call=%S", emergency_call);
                        IMENUCTL_SetItemText(pMenu, IMENUCTL_GetSel(pMenu), NULL, 0, emergency_call);
                        ITEXTCTL_SetActive(pTextCtl, FALSE);
                        IMENUCTL_SetActive(pMenu, TRUE);
                        return TRUE;
                    }
                    break;
                default:
                    break;
            }
            break;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_END:
                
                //case AVK_SOFT2:		//Add By zzg 2010_09_08 for smart and m8
                case AVK_CLR:
                    pMe->m_nFldInputID = AEE_ADDRFIELD_NONE;
                    pMe->m_wSelectEdit = 0;
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                    
                case AVK_INFO:
                case AVK_SELECT:
                {                          
                    CtlAddItem ai;
                    int i;
                    char m_vmnuber[MAX_EMERGENCY_NUM_LEN] = {0};
                    EmergencyNum_Table m_entable;
                    AECHAR emergency_call[MAX_EMERGENCY_NUM_LEN+1] = {0};
                    uint16 selectEdit = IMENUCTL_GetSel(pMenu); 
                    m_entable.emert_size = OEMNV_EMERT_SEZE;
                    ICONFIG_GetItem(pMe->m_pConfig, CFGI_EMERGENCYNUM_TABLE, (void*)&m_entable, sizeof(EmergencyNum_Table));
                    MSG_FATAL("AVK_SELECT Start, selectEdit=%d",selectEdit,0,0);
                    ITEXTCTL_GetText(pTextCtl, emergency_call, MAX_EMERGENCY_NUM_LEN);
                    if(!CContApp_CheckNumber(emergency_call))
                    {
                        CLOSE_DIALOG(DLGRET_NUM_UNAVAILD);
                        return TRUE;
                    }
                    
                    DBGPRINTF("emergency_call=%S", emergency_call);
                    IMENUCTL_SetItemText(pMenu, selectEdit, NULL, 0, emergency_call);                    
                    for(i=0; i<m_entable.emert_size; i++)
                    {
                        if(IMENUCTL_GetItem(pMenu, i, &ai))
                        {
                           	m_entable.emerg_num[i].num_len = WSTRLEN((AECHAR *)ai.pText);    
                            MSG_FATAL("num_len=%d",m_entable.emerg_num[i].num_len,0,0);
                            WSTRTOSTR((AECHAR *)ai.pText, m_vmnuber, MAX_EMERGENCY_NUM_LEN);
                            DBGPRINTF("m_vmnuber s=%s", m_vmnuber);
                            DBGPRINTF("ai.pText s=%S", (AECHAR *)ai.pText);
                            MSG_FATAL("m_vmnuber d= %d",ATOI(m_vmnuber),0,0);
                            STRCPY(m_entable.emerg_num[i].num_buf,m_vmnuber); 
                            DBGPRINTF("num_buf s=%s", m_entable.emerg_num[i].num_buf);
                        }
                    }
                    
                    (void)OEM_SetConfig(CFGI_EMERGENCYNUM_TABLE,
                                       (void*)&m_entable,
                                       sizeof(EmergencyNum_Table));      
                    
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    MSG_FATAL("AVK_SELECT End",0,0,0);
                }
                return TRUE;
                
  				case AVK_CAMERA:
            	#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_C01)
            	{
                    CtlAddItem ai;
                    uint16   wID;
                    
                    wID = IMENUCTL_GetSel(pMenu);
                    if (IMENUCTL_GetItem(pMenu, wID, &ai))
                    {
                       (void)MakeVoiceCall(pMe->m_pShell, FALSE, (AECHAR *)ai.pText);
                    }
                    return TRUE;
				}
				#endif
				break;
                case AVK_SEND:
                    {
                        CtlAddItem ai;
                        uint16   wID;
                        #if defined(FEATURE_VERSION_C01) 
						{
							nv_item_type	SimChoice;
							OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
							if(SimChoice.sim_select==AVK_SEND_TWO)
							{
								return TRUE;
							}
						}
						#endif
                        wID = IMENUCTL_GetSel(pMenu);
                        if (IMENUCTL_GetItem(pMenu, wID, &ai))
                        {
                            if(WSTRNCMP((AECHAR *)ai.pText, L"NONE", 4) == 0)
                            {
                                CLOSE_DIALOG(DLGRET_NUM_UNAVAILD);
                            }
                            else
                            {
                                (void)MakeVoiceCall(pMe->m_pShell, FALSE, (AECHAR *)ai.pText);
                            }
                        }
                    }
                    return TRUE;
                    
                default:
                    break;
            }
            return TRUE;
            
        case EVT_COMMAND:
            {
                
                CtlAddItem ai;
                if (IMENUCTL_GetItem(pMenu, wParam, &ai))
                {
                     (void)MakeVoiceCall(pMe->m_pShell, FALSE, (AECHAR *)ai.pText);
                }
            }
            CLOSE_DIALOG(DLGRET_OK);
            return TRUE;
            
        default:
            break;
    }

    return FALSE;
} // IDD_EMERGENCYNUMLIST_Handler
#endif
