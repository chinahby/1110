/*==============================================================================
//           W M S   A P P L E T  M O D U L E
// 文件:
//        WMSDialogHandler.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明：
//        短信息模块对话框事件处理函数相关文件
//                 
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间       修 改 人     修改内容、位置及原因
-----------      ----------    -----------------------------------------------
==============================================================================*/


/*==============================================================================
                             
                             本文件包含的外部文件
                             
==============================================================================*/
#include "WmsApp_priv.h" 
#include "WMSUtil.h" 
#include "OEMCFGI.h"
#include "Appscommon.h"
#include "AppComFunc.h"
#include "AEEDate.h "
#include "recentcalls.h"
#ifdef FEATURE_APP_MUSICPLAYER
#include "MusicPlayer.h"
#include "MediaGallery.h"
#endif 

#ifdef FEATURE_USES_MMS
#include "WMSMmsTest.h"
#include "WMSMms.h"
#include "AEEImage.h"
#include "AEEImageCtl.h"
#include "AEEMedia.h"
#include "AEEMimeTypes.h"
#include "AEEMenu.h"
extern uint8  g_mmsDataInfoMax;
#define IMAGE_MENU_INDEX 10    //Image彩信元素在显示时的菜单项起始索引
#define SOUND_MENU_INDEX 20
#define VIDEO_MENU_INDEX 30
#define OTHER_MENU_INDEX 40
#define MAX_BREWSETINT_STRING    64
#endif
#if defined(FEATURE_VIDEOPLAYER)
#include "VideoPlayer.h"
#endif

/*==============================================================================
                                 
                                 宏定义和常数
                                 
==============================================================================*/
#define MAX_MMS_PROXY    20
#define MAX_MMS_PORT    6

#define CONTACTAPP_RES_FILE "contactapp.bar"
#define CONTAPP_RES_FILE_LANG  AEE_RES_LANGDIR CONTACTAPP_RES_FILE

// 发送短信动画图片文件定义
#ifndef FEATURE_USES_LOWMEM
#define SENDINGSMS_ANI      "fs:/image/notice/sendingsms.png"
// 动画帧数
#define SENDINGSMS_ANI_N    7
#else
#define SENDINGSMS_ANI_N    2

#define SENDINGSMS_ANI      "fs:/image/notice/sendingsmslow.png"
#endif
#if defined(FEATURE_SUPPORT_ID)
// 动画起点 Y 坐标
#define SENDINGSMS_ANI_Y    10
#elif defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM) || defined(FEATURE_VERSION_C500BE)
// 动画起点 Y 坐标
#define SENDINGSMS_ANI_Y    10
#else
// 动画起点 Y 坐标
#define SENDINGSMS_ANI_Y    20
#endif
// 动画播放速率 ms
#define SENDINGSMS_ANI_R    150
// 填充背景色
#define SENDINGSMS_ANI_G    (RGB_BLACK) //MAKE_RGB(0, 0, 255)

#define SENDINGSMS_TEXT_COLOR  (RGB_WHITE)

#define WMS_PIXELS_BETWEEN_LINE (3)

// 短信预约时间选取界面相关颜色定义
#define BK_COLOR            MAKE_RGB(214, 223, 247)
#define FRAME_COLOR         MAKE_RGB(127, 157, 185)

// 预约短信到时提醒时长
#define RESERVEDMSGALERT_TIMEOUT 30000

// Time (in ms) before a MsgBox will close automatically
#define MSGBOX_TIMEOUT 500

#define SEND_FAILURE_MSG_TIMEOUT 1000

// Time (in ms) before a YesNo Box will close automatically
#define YESNO_TIMEOUT 6000

// sendopt 前面标题栏长度
#ifdef FEATURE_DISP_220X176
#define SENDOPT_TITLELENGTH   85
#else
#define SENDOPT_TITLELENGTH   65
#endif

// sendopt 英文时标题条较长长度
#if defined(FEATURE_DISP_220X176)
#define SENDOPT_TITLELONGER   140
#elif defined(FEATURE_DISP_128X160)
#define SENDOPT_TITLELONGER   80

#else
#define SENDOPT_TITLELONGER   110
#endif

// 画界面底部提示条宏定义
#define DRAW_BOTTOMBAR(x)                           \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = x;                         \
    DrawBottomBar(pMe->m_pDisplay, &BarParam);      \
}

// 设置界面底部提示条宏定义
#ifdef FEATURE_CUSTOMIZED_MENU_STYLE 
#define MENU_SETBOTTOMBAR(p, t)                     \
{                                                   \
    IMENUCTL_SetBottomBarType((p), (t));            \
}

#else
#define MENU_SETBOTTOMBAR(p, t)
#endif /* FEATURE_CUSTOMIZED_MENU_STYLE */

// 初始化静态文本控件宏定义
#define INIT_STATIC                                             \
if (NULL == pMe->m_pStatic)                                     \
{                                                               \
    AEERect rc = {0};                                           \
                                                                \
    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_STATIC,   \
            (void**)&(pMe->m_pStatic)) == SUCCESS)              \
    {                                                           \
        ISTATIC_SetRect(pMe->m_pStatic, &rc);                   \
    }                                                           \
}           

// 添加无图标菜单项宏定义
#define MENU_ADDITEM(pMenu,ID)                                                \
    (void)IMENUCTL_AddItem((pMenu), AEE_WMSAPPRES_LANGFILE, (ID), (ID), 0, 0) 

#define CONTROL_SETRECT(pControl,pRc,x,y,dx,dy) \
                SETAEERECT(pRc,  x, y,dx,dy); \
                ICONTROL_SetRect((IControl*)pControl,pRc);
                            


/*==============================================================================

                                 函数声明

==============================================================================*/
//static void WmsApp_PlaySendingAni(WmsApp *pMe);
static void WmsApp_PlaySendingAni(void *pUser);
static void WmsApp_ReadMsg(void *pUser);

//static void WmsApp_StopSendingAni(WmsApp *pMe);

static AECHAR * FormatMessageForDisplay(WmsApp  *pMe, WMSMessageStruct *pMessage);
static AECHAR * FormatFlashSMSForDisplay(WmsApp  *pMe, WMSMessageStruct *pMessage);

static void DialogTimeoutCallback(void *pUser);
static void WMSApp_DialogTimeout(void *pme);

static boolean IDD_MAIN_Handler(void        *pUser, 
                                AEEEvent    eCode, 
                                uint16      wParam, 
                                uint32      dwParam);
                                
static boolean  IDD_PWD_Handler(void       *pUser,
                                AEEEvent   eCode,
                                uint16     wParam,
                                uint32     dwParam);

static boolean IDD_CONFIRM_Handler(void *pUser, 
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_MESSAGELIST_Handler(void *pUser, 
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_VIEWMSG_Handler(void         *pUser, 
                                   AEEEvent     eCode, 
                                   uint16       wParam, 
                                   uint32       dwParam);

static boolean IDD_DELETEMSGS_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_SETTING_Handler(void   *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_RESERVEDMSG_Handler(void   *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_GETDT_Handler(void   *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_RESERVEDMSGALERT_Handler(void        *pUser,
                                            AEEEvent    eCode,
                                            uint16      wParam,
                                            uint32      dwParam);

static boolean IDD_OUTMSGSAVEOPT_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_PRIORITY_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_REPORTS_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_SENDMODE_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_SELECTFROMOPT_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

#ifdef FEATURE_AUTOREPLACE
static boolean IDD_AUTOREPLACE_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);
#endif

static boolean IDD_CALLBACKNUMSWITCH_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_CALLBACKNUM_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

#ifdef FEATURE_CDSMS_RUIM
static boolean IDD_STORAGE_Handler(void   *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);
#endif

static boolean IDD_MESSAGEVALIDITY_Handler(void   *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

//Add By zzg 2012_02_04
static boolean IDD_TIME_STAMP_Handler(void   *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);
//Add End

static boolean IDD_SENDING_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);
#ifdef FEATURE_USES_MMS
static boolean IDD_GETTING_Handler(void* pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);
#endif

static boolean IDD_TONUMLIST_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_SENDOPTS_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_WRITEMSG_Handler(void *pUser, 
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_TEMPLATES_Handler(void   *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_EDITTEMPLATE_Handler(void *pUser, 
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static void SetControlRect(WmsApp *pMe,void *Ctl);

static boolean IDD_MSGOPTS_Handler(void *pUser,
    AEEEvent eCode,
    uint16  wParam,
    uint32  dwParam
);

#ifdef FEATURE_CDSMS_RUIM
static boolean IDD_MSGCOPY_Handler(void *pUser,
    AEEEvent eCode,
    uint16  wParam,
    uint32  dwParam
);

static boolean IDD_MSGCOPY_HandlerExt(void *pUser,
    AEEEvent eCode,
    uint16  wParam,
    uint32  dwParam
);

#endif /* FEATURE_CDSMS_RUIM */

static boolean IDD_MEMSTATUS_Handler(void *pUser, 
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_DELETING_Handler(void        *pUser,
                                    AEEEvent    eCode,
                                    uint16      wParam,
                                    uint32      dwParam);

static boolean IDD_MANAGEMENT_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_COPYINBOX_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_MOVEINBOX_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_MSGBOX_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

#ifdef FEATURE_CONTACT_APP
static boolean IDD_EXTARCTDETAILS_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_EXTARCTEDITEMLIST_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_EXTARCTEDITEM_OPT_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);
#endif /* FEATURE_CONTACT_APP */

static boolean IDD_AUTODELETE_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_RSVDMSGALERTTIMEOUT_SETTING_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_VOICEMAIL_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean IDD_VMNUM_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
);

static boolean  gbWMSDialogLock = FALSE;

static void WMSDialog_keypadtimer(void *pUser);
static boolean IDD_LOADINGMSG_Handler(void   *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
);

//add by yangdecai
static boolean IDD_RESEND_CONFIRM_Handler(void   *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
);

// 对话框 IDD_WMSNEWMSG 事件处理函数
static boolean  IDD_WMSNEWMSG_Handler(void *pUser,
                                		 AEEEvent   eCode,
                                 		 uint16     wParam,
                                         uint32     dwParam);

//对话框IDD_POPMESAGE事件处理函数
static boolean IDD_POPMSG_Handler(void *pUser,
									 AEEEvent eCode,
									 uint16 wParam,
									 uint32 dwParam);  

//对话框IDD_FLASHSMS事件处理函数
static boolean IDD_FLASHSMS_Handler(void *pUser,
									 AEEEvent eCode,
									 uint16 wParam,
									 uint32 dwParam);

#ifdef FEATURE_USES_MMS
static boolean IDD_VIEWMSG_MMS_Handler(void         *pUser, 
                                   AEEEvent     eCode, 
                                   uint16       wParam, 
                                   uint32       dwParam);
                                
static boolean IDD_MMSNOTIFY_Handler(void *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
);
static boolean IDD_MMSDELIVERYREPORT_Handler(void *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
);       
static boolean IDD_READREPLY_Handler(void *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
);       
static boolean IDD_REPORTALLOWED_Handler(void *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
);       
static boolean IDD_SENDERVISIBILITY_Handler(void *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
);       





static boolean IDD_OPTION_YESNO_Handle(void *pUser,
    int nCFIG_Id,
    int8* pItem,
    int nTitleTextId,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam);
    
boolean WmsApp_SaveToFile(char* pFileName,void* pData,uint32 nDataLen);
static IImage* WmsLoadImageFromData(WmsApp *pMe,int nFragIndex,char* pMimeType);
static void WmsLoadSoundFromData(WmsApp *pMe,int nFragIndex,char* pMimeType);
char* MimeResCheckTypeExist(WmsApp *pMe,char* pType,uint8** ppBuf,uint32* pBufLen);
int AddMimeResIntoMms(WmsApp *pMe,char* pPath);
int DeleteMimeResFromMms(WmsApp *pMe,char* pPath);
int MimeResCheckFileExist(WmsApp *pMe,char* pPath);
int WMSMMS_GetResByExplorer(void* pv, FileNamesBuf pBuf, uint32 nBufSize);

static void WMSMMS_MediaNotify(void * pUser, AEEMediaCmdNotify *pCmdNotify);
static boolean IDD_EDIT_ALBUMOREMAIN_Handler(void *pUser, AEEEvent eCode,uint16 wParam, uint32 dwParam);
static boolean IDD_MMS_SERVER_ADDRESS_Handler(void *pUser, AEEEvent eCode,uint16 wParam, uint32 dwParam);
static boolean IDD_MMS_PROXY_Handler(void *pUser, AEEEvent eCode,uint16 wParam, uint32 dwParam);
static boolean IDD_MMS_PORT_Handler(void *pUser, AEEEvent eCode,uint16 wParam, uint32 dwParam);
static boolean IDD_MMS_MEMSTATUS_Handler(void   *pUser,AEEEvent eCode,uint16 wParam, uint32 dwParam);

#endif
/*==============================================================================

                                 函数定义
                                 
==============================================================================*/
/*==============================================================================
函数:
    WmsApp_ShowDialog

说明:
    函数由状态处理函数用来唤起相应对话框资源ID的对话框。

参数:
    pMe [in]: 指向 WMS Applet 对象结构的指针。该结构包含小程序的特定信息。
    dlgResId [in]: 对话框资源ID，函数根据该ID创建对话框。

返回值:
    none

备注:

==============================================================================*/
void WmsApp_ShowDialog(WmsApp  *pMe,  uint16 dlgResId)
{
    int nRet;

    // 每次最多打开一个对话框
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // 如果当前已经有对话框被打开，直接返回
        ERR("Trying to create dialog %d without closing previous one",dlgResId,0,0);
        return;
    }
    
    nRet = ISHELL_CreateDialog(pMe->m_pShell,AEE_WMSAPPRES_LANGFILE,dlgResId,NULL);
    if (nRet != SUCCESS)
    {
        ERR("Failed to create the dialog %d in the WMS applet.",dlgResId,0,0);
    }
}

/*==============================================================================
函数:
    WmsApp_ShowMsgBox

说明:
    函数由状态处理函数用来唤起消息对话框，显示消息提示用户。

参数:
    pMe [in]: 指向 WMS Applet 对象结构的指针。该结构包含小程序的特定信息。
    wTextResId [in]: 消息文本资源 ID。

返回值:
    none

备注:

==============================================================================*/
void WmsApp_ShowMsgBox(WmsApp *pMe, uint16 wTextResId)
{
    pMe->m_wMsgResID = wTextResId;
    
    // 创建消息对话框.
    WmsApp_ShowDialog(pMe, IDD_MSGBOX);
}

/*==============================================================================
函数:
    WmsApp_SetDialogHandler
       
说明:
    函数根据 pMe->m_wActiveDlgID 设定对话事件处理函数指针 m_pDialogHandler 相应值。

参数:
    pMe [In]: 指向 WMS Applet 对象结构的指针。该结构包含小程序的特定信息。

返回值:
    none

备注:
    若找不到对应的对话框处理函数, m_pDialogHandler 被置为 NULL
==============================================================================*/
void WmsApp_SetDialogHandler(WmsApp *pMe)
{
    switch (pMe->m_wActiveDlgID)
    {
        case IDD_MAIN:
            pMe->m_pDialogHandler = IDD_MAIN_Handler;
            break;
            
        case IDD_PWD:
            pMe->m_pDialogHandler = IDD_PWD_Handler;
            break;
            
        case IDD_CONFIRM:
            INIT_STATIC
            pMe->m_pDialogHandler = IDD_CONFIRM_Handler;
            break;
        
        case IDD_MESSAGELIST:
            pMe->m_pDialogHandler = IDD_MESSAGELIST_Handler;
            break;
            
        case IDD_VIEWMSG:
            pMe->m_pDialogHandler = IDD_VIEWMSG_Handler;
            break;
            
        case IDD_DELETEMSGS:
            pMe->m_pDialogHandler = IDD_DELETEMSGS_Handler;
            break;
            
        case IDD_SETTING:
            pMe->m_pDialogHandler = IDD_SETTING_Handler;
            break;
            
        case IDD_RESERVEDMSG:
            pMe->m_pDialogHandler = IDD_RESERVEDMSG_Handler;
            break;
            
        case IDD_GETDT:
            pMe->m_pDialogHandler = IDD_GETDT_Handler;
            break;
            
        case IDD_RESERVEDMSGALERT:
            pMe->m_pDialogHandler = IDD_RESERVEDMSGALERT_Handler;
            break;
            
        case IDD_OUTMSGSAVEOPT:
            pMe->m_pDialogHandler = IDD_OUTMSGSAVEOPT_Handler;
            break;
            
        case IDD_PRIORITY:
            pMe->m_pDialogHandler = IDD_PRIORITY_Handler;
            break;
            
        case IDD_REPORTS:
            pMe->m_pDialogHandler = IDD_REPORTS_Handler;
            break;
            
#ifdef FEATURE_USES_MMS  
        case IDD_MMSOPTION_YESNO:
			{
            	switch(pMe->m_nDlgID)
				{
					case WMSST_SENDERVISIBILITY:
					{
			            pMe->m_pDialogHandler = IDD_SENDERVISIBILITY_Handler;
					}
					break;
					case WMSST_MMSDELIVERYREPORT:
					{
            			pMe->m_pDialogHandler = IDD_MMSDELIVERYREPORT_Handler;
					}
					break;
					case WMSST_READREPLY:
					{
           				 pMe->m_pDialogHandler = IDD_READREPLY_Handler;
					}
					break;
					case WMSST_REPORTALLOWED:
					{
			            pMe->m_pDialogHandler = IDD_REPORTALLOWED_Handler;
					}
					break;
					case WMSST_MMSNOTIFY:
					{
						pMe->m_pDialogHandler = IDD_MMSNOTIFY_Handler;
					}
					break;
				}
			}
            break;
#endif
#ifdef FEATURE_AUTOREPLACE
        case IDD_AUTOREPLACE:
            pMe->m_pDialogHandler = IDD_AUTOREPLACE_Handler;
            break;
#endif            
#if !defined(FEATURE_VERSION_C316)&&!defined(FEATURE_VERSION_K202_LM129C)
           
        case IDD_CALLBACKNUMSWITCH:
            pMe->m_pDialogHandler = IDD_CALLBACKNUMSWITCH_Handler;
            break;
            
        case IDD_CALLBACKNUM:
            pMe->m_pDialogHandler = IDD_CALLBACKNUM_Handler;
            break;
#endif            
        case IDD_AUTODELETE:
            pMe->m_pDialogHandler = IDD_AUTODELETE_Handler;
            break;
            
        case IDD_RSVDMSGALERTTIMEOUT_SETTING:
            pMe->m_pDialogHandler = IDD_RSVDMSGALERTTIMEOUT_SETTING_Handler;
            break;

#ifdef FEATURE_CDSMS_RUIM
        case IDD_STORAGE:
            pMe->m_pDialogHandler = IDD_STORAGE_Handler;
            break;
#endif  

        case IDD_MESSAGEVALIDITY:
            pMe->m_pDialogHandler = IDD_MESSAGEVALIDITY_Handler;
            break;

		//Add By zzg 2012_02_04
		case IDD_TIME_STAMP:
            pMe->m_pDialogHandler = IDD_TIME_STAMP_Handler;
            break;
		//Add End
            
        case IDD_SENDING:
            pMe->m_pDialogHandler = IDD_SENDING_Handler;
            break;
#ifdef FEATURE_USES_MMS            
        case IDD_GETTING:
            pMe->m_pDialogHandler = IDD_GETTING_Handler;
            break;
#endif            
        case IDD_TONUMLIST:
            pMe->m_pDialogHandler = IDD_TONUMLIST_Handler;
            break;
            
        case IDD_SENDOPTS:
            pMe->m_pDialogHandler = IDD_SENDOPTS_Handler;
            break;
            
        case IDD_WRITEMSG:
            pMe->m_pDialogHandler = IDD_WRITEMSG_Handler;
            break;
            
        case IDD_TEMPLATES:
            pMe->m_pDialogHandler = IDD_TEMPLATES_Handler;
            break;
            
        case IDD_EDITTEMPLATE:
            pMe->m_pDialogHandler = IDD_EDITTEMPLATE_Handler;
            break;
            
        case IDD_MSGOPTS:
            pMe->m_pDialogHandler = IDD_MSGOPTS_Handler;
            break;
            
#ifdef FEATURE_CDSMS_RUIM
        case IDD_MSGCOPY:
            INIT_STATIC
#if defined(FEATURE_CDSMS_RUIM)
            if (COPYALLMTTORUIM == pMe->m_CopyType ||
                COPYALLRUIMTOPHONE == pMe->m_CopyType ||
                MOVEALLMTTORUIM == pMe->m_CopyType ||
                MOVEALLRUIMTOPHONE == pMe->m_CopyType)
            {
                pMe->m_pDialogHandler = IDD_MSGCOPY_HandlerExt;
            }
            else
#endif 
            {
                pMe->m_pDialogHandler = IDD_MSGCOPY_Handler;
            }
            break;
#endif /* FEATURE_CDSMS_RUIM */
            
        case IDD_MEMSTATUS:
            pMe->m_pDialogHandler = IDD_MEMSTATUS_Handler;
            break;
            
        case IDD_DELETING:
            INIT_STATIC
            pMe->m_pDialogHandler = IDD_DELETING_Handler;
            break;

        case IDD_MSGBOX:
            INIT_STATIC
            pMe->m_pDialogHandler = IDD_MSGBOX_Handler;
            break;
        
#ifdef FEATURE_CONTACT_APP
        case IDD_EXTARCTDETAILS:
            pMe->m_pDialogHandler = IDD_EXTARCTDETAILS_Handler;
            break;
            
        case IDD_EXTARCTEDITEMLIST:
            pMe->m_pDialogHandler = IDD_EXTARCTEDITEMLIST_Handler;
            break;
            
        case IDD_EXTARCTEDITEM_OPT:
            pMe->m_pDialogHandler = IDD_EXTARCTEDITEM_OPT_Handler;
            break;
#endif 

        case IDD_VOICEMAIL:
            pMe->m_pDialogHandler = IDD_VOICEMAIL_Handler;
            break;
            
        case IDD_VMNUM:
            pMe->m_pDialogHandler = IDD_VMNUM_Handler;
            break;
            
        case IDD_LOADINGMSG:
            INIT_STATIC
            pMe->m_pDialogHandler = IDD_LOADINGMSG_Handler;
            break;
            
        case IDD_MANAGEMENT:
            pMe->m_pDialogHandler = IDD_MANAGEMENT_Handler;
            break;
        
        case IDD_MOVEINBOX:
            pMe->m_pDialogHandler = IDD_MOVEINBOX_Handler;
            break;
            
        case IDD_COPYINBOX:
            pMe->m_pDialogHandler = IDD_COPYINBOX_Handler;
            break;
            
        case IDD_SENDMODE:
            pMe->m_pDialogHandler = IDD_SENDMODE_Handler;
            break;

        case IDD_SELECTFROMOPT:
            pMe->m_pDialogHandler = IDD_SELECTFROMOPT_Handler;
            break;
		//add by yangdecai
		case IDD_RESEND_CONFIRM:
			INIT_STATIC
			pMe->m_pDialogHandler = IDD_RESEND_CONFIRM_Handler;
            break;
		case IDD_WMSNEWMSG:
            INIT_STATIC
			pMe->m_pDialogHandler = IDD_WMSNEWMSG_Handler;
			break;
		case IDD_FLASHSMS:
			INIT_STATIC
			pMe->m_pDialogHandler = IDD_FLASHSMS_Handler;
			break;
		case IDD_NOPOPMSG:
			INIT_STATIC
			pMe->m_pDialogHandler = IDD_POPMSG_Handler;
			break;
#ifdef FEATURE_USES_MMS
        case IDD_VIEWMSG_MMS:
            pMe->m_pDialogHandler = IDD_VIEWMSG_MMS_Handler;
            break;

        case IDD_ALBUMOREMAIN:
            pMe->m_pDialogHandler = IDD_EDIT_ALBUMOREMAIN_Handler;
            break;

        case IDD_MMS_SERVER_ADDRESS:
            pMe->m_pDialogHandler = IDD_MMS_SERVER_ADDRESS_Handler;
            break;

        case IDD_MMS_PROXY:
            pMe->m_pDialogHandler = IDD_MMS_PROXY_Handler;
            break;

        case IDD_MMS_PORT:
            pMe->m_pDialogHandler = IDD_MMS_PORT_Handler;
            break;

        case IDD_MMS_MEMSTATUS:
            pMe->m_pDialogHandler = IDD_MMS_MEMSTATUS_Handler;
            break;            
#endif            
        default:
            pMe->m_pDialogHandler = NULL;
            break;
    }
}

/*==============================================================================
函数:
    WmsApp_RouteDialogEvt

说明:
    函数将 BREW 事件路由给当前活动对话框的事件处理函数。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    dwParam [in]: 与事件eCode关联的数据。

返回值:
    TRUE:传入事件被处理。
    FALSE:传入事件没被处理。

备注:

==============================================================================*/
boolean WmsApp_RouteDialogEvt(WmsApp *pMe, 
                              AEEEvent eCode, 
                              uint16  wParam, 
                              uint32  dwParam)
{
    if (NULL == pMe->m_pDialogHandler) 
    {
        return FALSE;
    }
    
    return pMe->m_pDialogHandler((void *)pMe, eCode, wParam, dwParam);
}

/*==============================================================================
函数:
    DialogTimeoutCallback

说明:
    传递给BREW层的回调函数，以便设定的定时器超时发出超时事件通知对话框。

参数:
    pUser [in]: 必须是指向WMS Applet对象结构的指针。

返回值:
    none

备注:

==============================================================================*/
static void DialogTimeoutCallback(void *pUser)
{
    WmsApp *pMe = (WmsApp*)pUser;

    if (NULL == pMe)
    {
        return;
    }
    
    if (ISHELL_ActiveApplet(pMe->m_pShell) != AEECLSID_WMSAPP)
    {
        return;
    }

    // Post a message to the dialog telling it to close
    (void) ISHELL_PostEventEx(pMe->m_pShell,
                              EVTFLG_ASYNC,
                              AEECLSID_WMSAPP,
                              EVT_DISPLAYDIALOGTIMEOUT, 
                              0, 
                              0);
}

/*==============================================================================
函数:
    IDD_MAIN_Handler

说明:
    WMS Applet主界面对话框 IDD_MAIN 事件处理函数。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:
    New

==============================================================================*/
static boolean IDD_MAIN_Handler(void        *pUser, 
                                AEEEvent    eCode, 
                                uint16      wParam, 
                                uint32      dwParam)
{
    WmsApp *pMe = (WmsApp *)pUser;
    IMenuCtl *pMenu = NULL;
    MSG_FATAL("IDD_MAIN_Handler Start, eCode=0X%x", eCode,0,0);
    pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_MAIN_MENU);

    if (pMenu == NULL)
    {
        return FALSE;
    }
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            MSG_FATAL("IDD_MAIN_Handler EVT_DIALOG_INIT 1",0,0,0);
            // 设置设置菜单控件矩形和属性
            //SetControlRect(pMe,pMenu);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
            MSG_FATAL("IDD_MAIN_Handler EVT_DIALOG_INIT 2",0,0,0);
            
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
#endif
            {
				AECHAR WTitle[10] = {0};
                int result = 0;
				result = ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,                                
                        IDS_APP_WMS,
                        WTitle,
                        sizeof(WTitle));
                MSG_FATAL("IDD_MAIN_Handler EVT_DIALOG_INIT result=%d",result,0,0);
                if(pMe->m_pIAnn != NULL)
                {
				    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
            }
            MSG_FATAL("IDD_MAIN_Handler EVT_DIALOG_INIT 3",0,0,0);
            // 菜单项初始化
            #ifdef FEATURE_VERSION_W317A
            MENU_ADDITEM(pMenu, IDS_WRITEMESSAGEW317);
            #else
            MENU_ADDITEM(pMenu, IDS_WRITEMESSAGE);
            #endif
            MENU_ADDITEM(pMenu, IDS_INBOX);
            MENU_ADDITEM(pMenu, IDS_OUTBOX);
            MSG_FATAL("IDD_MAIN_Handler EVT_DIALOG_INIT 4",0,0,0);
#ifdef FEATURE_USES_MMS  
            MENU_ADDITEM(pMenu, IDS_INBOX_MMS);
            MENU_ADDITEM(pMenu, IDS_OUTBOX_MMS);
            MENU_ADDITEM(pMenu, IDS_DRAFT_MMS);
#endif
            MSG_FATAL("IDD_MAIN_Handler EVT_DIALOG_INIT 5",0,0,0);
            MENU_ADDITEM(pMenu, IDS_DRAFT);
#if (!defined FEATURE_CARRIER_THAILAND_HUTCH)
#if( !defined FEATURE_VERSION_C337)
            MENU_ADDITEM(pMenu, IDS_VOICEMAIL);
#endif
#endif //!defined FEATURE_CARRIER_THAILAND_HUTCH
#ifdef FEATURE_RESERVEDMSG
            MENU_ADDITEM(pMenu, IDS_RESERVEDMSG);
#endif  
            MENU_ADDITEM(pMenu, IDS_MSGMANAGEMENT);
            MENU_ADDITEM(pMenu, IDS_MSGSETTING);
            MENU_ADDITEM(pMenu, IDS_TEMPLATES);   
            AEE_CancelTimer(WMSDialog_keypadtimer,pMe);
			AEE_SetTimer(5*1000,WMSDialog_keypadtimer,pMe);            
            MSG_FATAL("IDD_MAIN_Handler EVT_DIALOG_INIT 6",0,0,0);
            return TRUE;

        case EVT_DIALOG_START:
        // 有新消息
        case EVT_WMS_MSG_RECEIVED_MESSAGE:
            {
                AECHAR *pwsz = NULL;
                uint32 dwSize;
                int nLen;
                AECHAR wstrFmt[20] = {0};
                uint16 nNews=0, nMsgs=0;
				wms_cache_info_list   *pList = NULL;
			    uint16 i = 0;
				uint16 temp = 0;
                
                dwSize = 64 * sizeof(AECHAR);
                pwsz = (AECHAR *)MALLOC(dwSize);
                DBGPRINTF("WMS Address = 0x%x", pwsz);
                (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                        EVTFLG_ASYNC,
                                        AEECLSID_WMSAPP,
                                        EVT_USER_REDRAW,
                                        0, 
                                        0);
                
                if (NULL == pwsz)
                {
                    return FALSE;
                }
#ifdef FEATURE_USES_MMS
                STRTOWSTR("(%d)",wstrFmt,sizeof(wstrFmt));
                ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                            IDS_DRAFT_MMS,
                            pwsz, dwSize);

                ICONFIG_GetItem(pMe->m_pConfig,CFGI_MMS_DRAFTCOUNT,&g_mmsDataInfoMax,sizeof(g_mmsDataInfoMax));
                nLen = WSTRLEN(pwsz);
                if (nLen<30)
                {
                    WSPRINTF(&pwsz[nLen], (64-nLen)*sizeof(AECHAR), 
                        wstrFmt, g_mmsDataInfoMax); 
                }
                IMENUCTL_SetItemText(pMenu, IDS_DRAFT_MMS, NULL, NULL, pwsz);
                DBGPRINTF("IDS_DRAFT_MMS pwsz=%S, g_mmsDataInfoMax=%d",pwsz, g_mmsDataInfoMax);
                
                STRTOWSTR("(%d)",wstrFmt,sizeof(wstrFmt));
                ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                            IDS_INBOX_MMS,
                            pwsz, dwSize);

                ICONFIG_GetItem(pMe->m_pConfig,CFGI_MMS_INCOUNT,&g_mmsDataInfoMax,sizeof(g_mmsDataInfoMax));
                nLen = WSTRLEN(pwsz);
                if (nLen<30)
                {
                    WSPRINTF(&pwsz[nLen], (64-nLen)*sizeof(AECHAR), 
                        wstrFmt, g_mmsDataInfoMax); 
                }
                IMENUCTL_SetItemText(pMenu, IDS_INBOX_MMS, NULL, NULL, pwsz);
				
                STRTOWSTR("(%d)",wstrFmt,sizeof(wstrFmt));
                ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                            IDS_OUTBOX_MMS,
                            pwsz, dwSize);
                            
                ICONFIG_GetItem(pMe->m_pConfig,CFGI_MMS_OUTCOUNT,&g_mmsDataInfoMax,sizeof(g_mmsDataInfoMax));
                nLen = WSTRLEN(pwsz);
                if (nLen<30)
                {
                    WSPRINTF(&pwsz[nLen], (64-nLen)*sizeof(AECHAR), 
                        wstrFmt, g_mmsDataInfoMax); 
                }
                IMENUCTL_SetItemText(pMenu, IDS_OUTBOX_MMS, NULL, NULL, pwsz);				
#endif
                // 格式化收件箱菜单项文本
                (void)STRTOWSTR("(%d/%d)",wstrFmt,sizeof(wstrFmt));
                (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                            IDS_INBOX,
                            pwsz, dwSize);
                #if 1//def FEATURE_SUPPORT_ID
                wms_cacheinfolist_getcounts(WMS_MB_INBOX, &nNews, NULL, NULL);
				//add by yangdecai 
				// 更新链表序号
    			wms_cacheinfolist_updatexuhao(WMS_MB_INBOX);
				pList = wms_get_cacheinfolist(WMS_MB_INBOX);
                if (NULL != pList)
                {
                     nMsgs = pList->nBranches;
					 
                }
				//add end
				#else
                wms_cacheinfolist_getcounts(WMS_MB_INBOX, &nNews, NULL, &nMsgs);
				#endif
                nLen = WSTRLEN(pwsz);
                if (nLen<30)
                {
                    WSPRINTF(&pwsz[nLen], (64-nLen)*sizeof(AECHAR), 
                        wstrFmt, nNews, nMsgs); 
                }
                IMENUCTL_SetItemText(pMenu, IDS_INBOX, NULL, NULL, pwsz);
                
                (void)STRTOWSTR("(%d)",wstrFmt,sizeof(wstrFmt));
                // 格式化发件箱菜单项文本
                (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                            IDS_OUTBOX,
                            pwsz, dwSize);
                nLen = WSTRLEN(pwsz);
                nMsgs = 0;
				#if 1//def FEATURE_SUPPORT_ID
				// 更新链表序号
    			wms_cacheinfolist_updatexuhao(WMS_MB_OUTBOX);
				pList = wms_get_cacheinfolist(WMS_MB_OUTBOX);
                if (NULL != pList)
                {
                     nMsgs = pList->nBranches;
                }
				#else
				wms_cacheinfolist_getcounts(WMS_MB_OUTBOX, NULL, NULL, &nMsgs); 
				#endif
				//add end
                if (nLen<30)
                {
                    WSPRINTF(&pwsz[nLen], (64-nLen)*sizeof(AECHAR), 
                        wstrFmt, nMsgs); 
                }
                IMENUCTL_SetItemText(pMenu, IDS_OUTBOX, NULL, NULL, pwsz);
                
                // 格式化草稿箱菜单项文本
                (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                            IDS_DRAFT,
                            pwsz, dwSize);
                nLen = WSTRLEN(pwsz);
                nMsgs = 0;
                //wms_cacheinfolist_getcounts(WMS_MB_DRAFT, NULL, NULL, &nMsgs);    //DELETE by yangdecai
				//add by yangdecai 
				#if 1 //def FEATURE_SUPPORT_ID
				wms_cacheinfolist_updatexuhao(WMS_MB_DRAFT);
				pList = wms_get_cacheinfolist(WMS_MB_DRAFT);
                if (NULL != pList)
                {
                     nMsgs = pList->nBranches;
                }
				#else
                wms_cacheinfolist_getcounts(WMS_MB_DRAFT, NULL, NULL, &nMsgs);
				#endif
                if (nLen<30)
                {
                    WSPRINTF(&pwsz[nLen], (64-nLen)*sizeof(AECHAR), wstrFmt, nMsgs); 
                }
                IMENUCTL_SetItemText(pMenu, IDS_DRAFT, NULL, NULL, pwsz);

                FREE(pwsz);
            }
            IMENUCTL_SetSel(pMenu, pMe->m_wMainMenuSel);
            return TRUE;
            
        case EVT_USER_REDRAW:
            // 绘制底条提示
            // Select       Back
            //DRAW_BOTTOMBAR(BTBAR_SELECT_BACK)
            
            // 更新界面
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);  
            //(void)IMENUCTL_Redraw(pMenu);   //dele by yangdecai
            return TRUE;
			
		case EVT_KEY_HELD:
            
        #if defined(FEATURE_VERSION_K202)||defined(FEATURE_VERSION_K212)
           if((AVKType)wParam == AVK_SELECT)
            {
                boolean TorchOn = FALSE;
                OEM_GetConfig(CFGI_FLSHLITHG_STATUS,&TorchOn, sizeof(TorchOn));
                if (TorchOn == FALSE )
                {
                    TorchOn = TRUE;
                    if (pMe->m_pBacklight)
                    {
                        IBACKLIGHT_TurnOnTorch(pMe->m_pBacklight);
                    }
                }
                else
                {
                    TorchOn = FALSE;
                    if (pMe->m_pBacklight)
                    {                           
                        IBACKLIGHT_TurnOffTorch(pMe->m_pBacklight);
                    }
                }
                OEM_SetConfig(CFGI_FLSHLITHG_STATUS,&TorchOn, sizeof(TorchOn));
                ISHELL_CloseApplet(pMe->m_pShell, TRUE); 
                return TRUE;
            }
        #endif
            return TRUE;

        case EVT_CTL_SEL_CHANGED:
            pMe->m_wMainMenuSel = wParam;
            return TRUE;
            
        case EVT_DIALOG_END:
            //IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,NULL);
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                case AVK_STAR:
                    if(gbWMSDialogLock)
                    {
                        OEMKeyguard_SetState(TRUE);	
#ifdef FEATURE_VERSION_C337
						MSLEEP(500);
#endif
                        ISHELL_CloseApplet(pMe->m_pShell, TRUE); 
                    }
                    return TRUE;                    
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            pMe->m_wMainMenuSel = wParam;
            switch(wParam)
            {
                 // 收件箱
                case IDS_INBOX:                	
                    CLOSE_DIALOG(DLGRET_INBOXES)
                    return TRUE;
                    
                // 写消息
                #ifdef FEATURE_VERSION_W317A
                case IDS_WRITEMESSAGEW317:
                #endif
                case IDS_WRITEMESSAGE:
                    CLOSE_DIALOG(DLGRET_WRITEMSG)
                    return TRUE;              
                    
                // 发件箱
                case IDS_OUTBOX:
                    CLOSE_DIALOG(DLGRET_OUTBOX)
                    return TRUE;              

#ifdef FEATURE_USES_MMS
                case IDS_INBOX_MMS:
                {
                    MSG_FATAL("IDD_MAIN_Handler IDS_OUTBOX_MMS",0,0,0);
                    ICONFIG_GetItem(pMe->m_pConfig,CFGI_MMS_INCOUNT,&g_mmsDataInfoMax,sizeof(g_mmsDataInfoMax));

                    MSG_FATAL("IDD_MAIN_Handler g_mmsDataInfoMax=%d", g_mmsDataInfoMax, 0, 0);
                                     
                    CLOSE_DIALOG(DLGRET_INBOX_MMS)
                }
                return TRUE; 
                case IDS_OUTBOX_MMS:
                    MSG_FATAL("IDD_MAIN_Handler IDS_OUTBOX_MMS",0,0,0);
                    (void) ICONFIG_GetItem(pMe->m_pConfig,CFGI_MMS_OUTCOUNT,&g_mmsDataInfoMax,sizeof(g_mmsDataInfoMax));
                    {
                        MSG_FATAL("IDD_MAIN_Handler g_mmsDataInfoMax=%d", g_mmsDataInfoMax, 0, 0);
                    }                      
                    CLOSE_DIALOG(DLGRET_OUTBOX_MMS)
                    return TRUE;   

                // 草稿箱
                case IDS_DRAFT_MMS:
                    MSG_FATAL("IDD_MAIN_Handler IDS_OUTBOX_MMS",0,0,0);
                    (void) ICONFIG_GetItem(pMe->m_pConfig,CFGI_MMS_DRAFTCOUNT,&g_mmsDataInfoMax,sizeof(g_mmsDataInfoMax));
                    {
                        MSG_FATAL("IDD_MAIN_Handler g_mmsDataInfoMax=%d", g_mmsDataInfoMax, 0, 0);
                    }                     
                    CLOSE_DIALOG(DLGRET_DRAFTBOX_MMS)
                    return TRUE;    

                // 存储器状态
                case IDS_MMS_MENSTATUS:
                    CLOSE_DIALOG(DLGRET_MMS_MEMSTATUS)
                    return TRUE;                       
#endif

                // 查看语音邮件通知消息
                case IDS_VOICEMAIL:
                    CLOSE_DIALOG(DLGRET_VIEWVOICEMSG)
                    return TRUE;              
                
                // 草稿箱
                case IDS_DRAFT:
                    CLOSE_DIALOG(DLGRET_DRAFT)
                    return TRUE;  

                // 消息模板
                case IDS_TEMPLATES:
                    CLOSE_DIALOG(DLGRET_TEMPLATES)
                    return TRUE;       
                    
                // 存储器状态
                case IDS_MENSTATUS:
                    CLOSE_DIALOG(DLGRET_MEMSTATUS)
                    return TRUE;              
                
                // 删除消息
                case IDS_DELETEMSGS:
                    CLOSE_DIALOG(DLGRET_DELETEMSGS)
                    return TRUE;   
                    
                case IDS_MSGMANAGEMENT:
                    CLOSE_DIALOG(DLGRET_MANAGEMENT)
                    return TRUE;
                    
                // 消息设置
                case IDS_MSGSETTING:
                    CLOSE_DIALOG(DLGRET_SETTINGS)
                    return TRUE;              
                
                // 预约信息
                case IDS_RESERVEDMSG:
                    CLOSE_DIALOG(DLGRET_RESERVEDMSG)
                    return TRUE;              
                    
                default:
                    break;
            }
            break;

        default:
            break;
    }

    return FALSE;
} // IDD_MAIN_Handler()

void WMSDialog_KeypadLock(boolean block)
{
   gbWMSDialogLock = block;
}
static void WMSDialog_keypadtimer(void *pUser)
{
    gbWMSDialogLock =FALSE;
}
/*==============================================================================
函数:
    IDD_PWD_Handler

说明:
    WMS Applet主界面对话框 IDD_PWD 事件处理函数。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:
    New

==============================================================================*/
static boolean  IDD_PWD_Handler(void       *pUser,
                                AEEEvent   eCode,
                                uint16     wParam,
                                uint32     dwParam)
#if 0                                
{
    static char   *m_strPhonePWD = NULL;
    WmsApp *pMe = (WmsApp *)pUser;
    
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            m_strPhonePWD = (char *)MALLOC((OEMNV_LOCKCODE_MAXLEN + 1)* sizeof(char));
            return TRUE;
            
        case EVT_DIALOG_START:
            (void) ISHELL_PostEvent(pMe->m_pShell,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);

            return TRUE;
            
        case EVT_USER_REDRAW:
            // 绘制相关信息
            {
                AECHAR  wstrDisplay[OEMNV_LOCKCODE_MAXLEN+1] = {0};
                int     y, nLen = 0, nBottomBarH;
                char    strDisplay[OEMNV_LOCKCODE_MAXLEN+1] = {0};
                AEERect rc;
                AECHAR  wstrLeft[32] = {0};
                AECHAR  wstrRight[32] = {0};
                
                y = GetTitleBarHeight(pMe->m_pDisplay);
                nBottomBarH = GetBottomBarHeight(pMe->m_pDisplay);
                if (dwParam == 0)
                {
                    TitleBar_Param_type  TitleBar_Param = {0};
                    
                    // 画标题条
                    TitleBar_Param.pwszTitle = wstrLeft;
                    TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER;
                    (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                                IDS_PWDTIPS, wstrLeft,sizeof(wstrLeft));
                    DrawTitleBar(pMe->m_pDisplay, &TitleBar_Param);
                    
                    wstrLeft[0] = 0;
                    y = GetTitleBarHeight(pMe->m_pDisplay);
                    
                    SETAEERECT(&rc,0, y,pMe->m_rc.dx, pMe->m_rc.dy - nBottomBarH - y);
                    // 先清屏
                    IDISPLAY_EraseRect(pMe->m_pDisplay,&rc);
                
                    y = 50;
                }
                else
                {
                    y = 50;
                    
                    SETAEERECT(&rc,0, y,pMe->m_rc.dx, 20);
                    
                    // 清除输入区
                    IDISPLAY_EraseRect(pMe->m_pDisplay,&rc);
                }
                
                // 绘制输入矩形框
                (void) STRTOWSTR("    [", wstrLeft, sizeof(wstrLeft));
                (void) STRTOWSTR("]    ", wstrRight, sizeof(wstrRight));
                IDISPLAY_DrawText( pMe->m_pDisplay, 
                            AEE_FONT_BOLD, wstrLeft,
                            -1, 0, y, NULL, 
                            IDF_TEXT_TRANSPARENT|IDF_ALIGN_LEFT);
                IDISPLAY_DrawText( pMe->m_pDisplay, 
                            AEE_FONT_BOLD, wstrRight,
                            -1, 0, y, NULL, 
                            IDF_TEXT_TRANSPARENT|IDF_ALIGN_RIGHT);
                
                // 绘制输入
                nLen = STRLEN(m_strPhonePWD);
                MEMSET(strDisplay, '*', nLen);
                (void) STRTOWSTR(strDisplay, wstrDisplay, sizeof(wstrDisplay));
                IDISPLAY_DrawText( pMe->m_pDisplay, 
                            AEE_FONT_BOLD, wstrDisplay,
                            -1, 0, y, NULL, 
                            IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER);
                
                // 绘制底条提示
                if ((nLen == 0) || (nLen == 1))
                {
                    if (nLen > 0)
                    {
                        // 确定-----删除
                        DRAW_BOTTOMBAR(BTBAR_OK_DELETE)
                    }
                    else
                    {
                        // 确定-----取消
                        DRAW_BOTTOMBAR(BTBAR_OK_CANCEL)
                    }
                }
            }
            
            // 更新显示
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);  
            return TRUE;
            
        case EVT_DIALOG_END:
            if (NULL != m_strPhonePWD)
            {
                FREE(m_strPhonePWD);
                m_strPhonePWD = NULL;
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
                        if (STRLEN(m_strPhonePWD) == 0)
                        {
                            CLOSE_DIALOG(DLGRET_CANCELED)
                            return TRUE;
                        }
                        break;
                        
                    case AVK_SELECT:
        		    case AVK_INFO:			
                        if (STRLEN(m_strPhonePWD) == 0)
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
                            
                            if (wPWD == EncodePWDToUint16(m_strPhonePWD))
                            {// 密码符合
                                CLOSE_DIALOG(DLGRET_PWDOK)
                            }
                            else
                            {// 密码错误
                                CLOSE_DIALOG(DLGRET_PWDERR)
                            }
                        }
                        return TRUE;
                        
                    default:
                        return TRUE;
                }
                
                nLen = STRLEN(m_strPhonePWD);
                if (chEnter == 0)
                {// 删除字符
                    if (nLen > 0)
                    {
                        bRedraw = TRUE;
                        m_strPhonePWD[nLen-1] = chEnter;
                    }
                }
                else if (nLen < OEMNV_LOCKCODE_MAXLEN)
                {
                    m_strPhonePWD[nLen] = chEnter;
                    nLen++;
                    m_strPhonePWD[nLen] = 0;
                    bRedraw = TRUE;
                }
                
                if (bRedraw)
                {
                    (void) ISHELL_PostEvent(pMe->m_pShell,
                                            AEECLSID_WMSAPP,
                                            EVT_USER_REDRAW,
                                            0,
                                            EVT_USER_REDRAW);
                }
            }
            return TRUE;
            
        default:
            break;
    }
    
    return FALSE;
} // IDD_PWD_Handler
#else
{
    PARAM_NOT_REF(dwParam)
    WmsApp *pMe = (WmsApp *)pUser;
    //static char   *m_strPhonePWD = NULL;
    AECHAR      wstrDisplay[OEMNV_LOCKCODE_MAXLEN+2] = {0};
    int             nLen = 0;
    char        strDisplay[OEMNV_LOCKCODE_MAXLEN+2] = {0};
    
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			{
    			 AECHAR  text[32] = {0};		
                if(NULL == pMe->m_strPhonePWD)
                {
                    pMe->m_strPhonePWD = (char *)MALLOC((OEMNV_LOCKCODE_MAXLEN + 1)* sizeof(char));
                }
                 (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                  AEE_WMSAPPRES_LANGFILE,
                                                  IDS_APP_WMS, 
                                                  text,
                                                  sizeof(text));                  
                  // 画标题条
    			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,text);
                return TRUE;
        	}
            
        case EVT_DIALOG_START:  
            (void) ISHELL_PostEvent(pMe->m_pShell,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    NULL,
                                    NULL);

            return TRUE;
            
        case EVT_USER_REDRAW:
            // 绘制相关信息
            {
                AECHAR  text[32] = {0};
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
				  MSG_FATAL("IDD_PWD_Handler pMe->m_rc.x=%d, pMe->m_rc.y=%d", pMe->m_rc.x, pMe->m_rc.y, 0);
				  pMe->m_rc.y = 0;	
				  Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
#endif
                //IDISPLAY_FillRect  (pMe->m_pDisplay,&pMe->m_rc,RGB_BLACK);
               (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                AEE_WMSAPPRES_LANGFILE,
                                                IDS_APP_WMS, 
                                                text,
                                                sizeof(text));                  
                // 画标题条
                TitleBar_Param.pwszTitle = text;
				#if 0
                TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
                
                DrawTitleBar(pMe->m_pDisplay, &TitleBar_Param);
				#else
			//	IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,text);
				#endif

               (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                AEE_WMSAPPRES_LANGFILE,
                                                IDS_PWDTIPS, 
                                                text,
                                                sizeof(text));
                nOldFontColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
                
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                    AEE_FONT_BOLD, 
                                    text,
                                    -1, 
                                    5, 
                                    TITLEBAR_HEIGHT + MENUITEM_HEIGHT*1/2,
                                    NULL, 
                                    IDF_TEXT_TRANSPARENT);
                   
                nLen = (pMe->m_strPhonePWD == NULL)?(0):(STRLEN(pMe->m_strPhonePWD));
                MEMSET(strDisplay, '*', nLen);
                strDisplay[nLen] = '|';
                strDisplay[nLen + 1] = '\0';
                (void) STRTOWSTR(strDisplay, wstrDisplay, sizeof(wstrDisplay));
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                                AEE_FONT_BOLD, 
                                wstrDisplay,
                                -1, 
                                10, 
                                TITLEBAR_HEIGHT + MENUITEM_HEIGHT*3/2,
                                NULL, 
                                IDF_TEXT_TRANSPARENT);
                (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, nOldFontColor);
        		#ifndef FEATURE_ALL_KEY_PAD    //add by yangdecai 
                // 绘制底条提示
                if (nLen > 3)
                {// 确定-----删除
                    DRAW_BOTTOMBAR(BTBAR_OK_DELETE)
                }
                else if(nLen > 0)
                {
                    DRAW_BOTTOMBAR(BTBAR_DELETE)
                }
                else
                #else
                // 绘制底条提示
                if (nLen > 3)
                {// 确定-----删除
                    DRAW_BOTTOMBAR(BTBAR_OK_BACK)
                }
                else if(nLen > 0)
                {
                    DRAW_BOTTOMBAR(BTBAR_BACK)
                }
                else
                #endif
                {// 确定-----取消
                    DRAW_BOTTOMBAR(BTBAR_CANCEL)
                }

                // 更新显示
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE); 
        
                return TRUE;
            }
            
        case EVT_DIALOG_END:
            if(pMe->m_eAppStatus != WMSAPP_SUSPEND)
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

					//Add By zzg 2012_02_27					
					case AVK_DEL:	 
					{
						chEnter = 0;
						break;
					}
					//Add End
                        
                    case AVK_CLR:
                        chEnter = 0;    
						MSG_FATAL("***zzg WMSDialog IDD_PWD_Handler dwParam=%x, m_strPhonePWD=%x, STRLEN(pMe->m_strPhonePWD)=%d***", dwParam, pMe->m_strPhonePWD, STRLEN(pMe->m_strPhonePWD));
                        #ifndef FEATURE_ALL_KEY_PAD    //add by yangdecai 
                        if (pMe->m_strPhonePWD == NULL || STRLEN(pMe->m_strPhonePWD) == 0)
                        {
                            CLOSE_DIALOG(DLGRET_CANCELED)
                            return TRUE;
                        }
                        #else
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
                        #endif
                        break;
                        
                    case AVK_SELECT:
                    case AVK_INFO:
                        if (pMe->m_strPhonePWD == NULL || STRLEN(pMe->m_strPhonePWD) < 4)
                        {
                            return TRUE;
                        }
                        else
                        {
                            uint16 wPWD=0;

                            OEM_GetConfig(CFGI_PHONE_PASSWORD, &wPWD, sizeof(wPWD));
                        
                            if (wPWD == EncodePWDToUint16(pMe->m_strPhonePWD))
                            {// 密码符合
                                CLOSE_DIALOG(DLGRET_PWDOK)
                            }
                            else
                            {// 密码错误
                                CLOSE_DIALOG(DLGRET_PWDERR)
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
                                            AEECLSID_WMSAPP,
                                            EVT_USER_REDRAW,
                                            NULL,
                                            NULL);
                }
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE
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

                if(WMSAPP_PT_IN_RECT(wXPos,wYPos,rc))
                {
                    if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
                    {
                        boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_SELECT,0);
                        return rt;
                    }
                    else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
                    {
                         boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_INFO,0);
                         return rt;
                    }
                    else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
                    {                       
                         boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_CLR,0);
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

}
#endif

/*==============================================================================
函数:
    IDD_CONFIRM_Handler

说明:
    WMS Applet对话框 IDD_CONFIRM 事件处理函数。

参数:
    pMe [in]:       指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]:     事件代码。
    wParam:         事件参数
    dwParam [in]:   与wParam关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_CONFIRM_Handler(void        *pUser, 
                                   AEEEvent    eCode, 
                                   uint16      wParam, 
                                   uint32      dwParam)
{
    WmsApp *pMe = (WmsApp *)pUser;
    
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    if (NULL == pMe->m_pStatic)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;

        case EVT_USER_REDRAW:
            {
                AECHAR  wstrText[MSGBOX_MAXTEXTLEN];
                PromptMsg_Param_type  Msg_Param={0};
 
                (void) ISHELL_LoadResString(pMe->m_pShell,
                                AEE_WMSAPPRES_LANGFILE,
                                pMe->m_wMsgResID,
                                wstrText,
                                sizeof(wstrText));
  
                // 调用公共函数绘制界面
                Msg_Param.ePMsgType = MESSAGE_CONFIRM;
                Msg_Param.pwszMsg = wstrText;
#ifdef FEATURE_CARRIER_THAILAND_HUTCH                  
                Msg_Param.eBBarType = BTBAR_OK_BACK;
#else
                Msg_Param.eBBarType = BTBAR_OK_CANCEL;
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH		
                DrawPromptMessage(pMe->m_pDisplay, pMe->m_pStatic, &Msg_Param);
            }
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            
            return TRUE;

        case EVT_DIALOG_END:
            ISTATIC_Release(pMe->m_pStatic);
            pMe->m_pStatic = NULL;
            return TRUE;
  
        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                case AVK_SELECT:
                    CLOSE_DIALOG(DLGRET_OK)
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

                if(WMSAPP_PT_IN_RECT(wXPos,wYPos,rc))
                {
                    if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
                    {
                        boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_SELECT,0);
                        return rt;
                    }
                    else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
                    {
                         boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_INFO,0);
                         return rt;
                    }
                    else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
                    {                       
                         boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_CLR,0);
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
} // IDD_CONFIRM_Handler

/*==============================================================================
函数:
    IDD_MESSAGELIST_Handler

说明:
    WMS Applet对话框 IDD_MESSAGELIST 事件处理函数。
    (显示收件箱、发件箱、草稿箱、UIM 消息列表，并处理用户做出的操作）

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam: 事件参数
    dwParam [in]: 与wParam关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_MESSAGELIST_Handler(void        *pUser,
                                       AEEEvent    eCode, 
                                       uint16      wParam, 
                                       uint32      dwParam)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;
    MSG_FATAL("IDD_MESSAGELIST_Handler Start eCode=0x%x, wParam=0x%x",eCode,wParam,0);
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_MESSAGELIST_MENU);
    if (NULL == pMenu)
    {
        return FALSE;
    }

    switch(eCode)
    {
        case EVT_DIALOG_INIT:
            MSG_FATAL("IDD_MESSAGELIST_Handler pMe->m_currState=%d", pMe->m_currState, 0, 0);
            //设置控件区域
            //SetControlRect(pMe,pMenu);
            
            return TRUE;

        case EVT_DIALOG_START:
            {
                uint16 nTitleID=0;
                AECHAR wszFmt[20] = {0};
                AECHAR wszTitle[32] = {0};
                uint16 nCount = 0;
                int nLen = 0;
                uint32 dwMask = IMENUCTL_GetProperties(pMenu);
                MSG_FATAL("IDD_MESSAGELIST_Handler pMe->m_currState=%d", pMe->m_currState, 0, 0);
                if (pMe == NULL)
                {
                    return TRUE;
                }
                IMENUCTL_SetRect(pMenu, &pMe->m_rc);
               
                dwMask = (dwMask & (~MP_WRAPSCROLL)) | MP_UNDERLINE_TITLE|MP_ACTIVE_NO_REDRAW;                
                IMENUCTL_SetProperties(pMenu, dwMask);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
#endif
                MENU_SETBOTTOMBAR(pMenu, BTBAR_OPTION_BACK);
#ifdef FEATURE_USES_MMS
                if((pMe->m_eMBoxType != WMS_MB_OUTBOX_MMS)
                    && (pMe->m_eMBoxType != WMS_MB_INBOX_MMS)
                    && (pMe->m_eMBoxType != WMS_MB_DRAFTBOX_MMS)
                    )
#endif                    
                {
                    wms_cacheinfolist_getcounts(pMe->m_eMBoxType, NULL, NULL, &nCount);
                }
            
                MSG_FATAL("pMe->m_eMBoxType = %d", pMe->m_eMBoxType, 0, 0);
                switch (pMe->m_eMBoxType)
                {
                    case WMS_MB_INBOX:
                        nTitleID = IDS_INBOX;
                        break;

                    case WMS_MB_OUTBOX:
                        nTitleID = IDS_OUTBOX;
                        break;

                    case WMS_MB_DRAFT:
                        nTitleID = IDS_DRAFT;
                        break;

#ifdef FEATURE_USES_MMS
                    case WMS_MB_INBOX_MMS:
                        nTitleID = IDS_INBOX_MMS;
                        break;

                    case WMS_MB_OUTBOX_MMS:
                        nTitleID = IDS_OUTBOX_MMS;
                        break;

                    case WMS_MB_DRAFTBOX_MMS:
                        nTitleID = IDS_DRAFT_MMS;
                        break;                        
#endif

                    default:
                        break;
                }
                
                if (nTitleID != 0)
                {
                    (void)STRTOWSTR(" (%d)", wszFmt, sizeof(wszFmt));
                    
                    (void) ISHELL_LoadResString(pMe->m_pShell,
                            AEE_WMSAPPRES_LANGFILE,
                            nTitleID,
                            wszTitle,
                            sizeof(wszTitle));
                    nLen = WSTRLEN(wszTitle);
                    WSPRINTF(&wszTitle[nLen], (32-nLen)*sizeof(AECHAR), wszFmt, nCount);
					#if 0
                    (void)IMENUCTL_SetTitle(pMenu, NULL, 0, wszTitle);
					#else
					{
					
						IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,wszTitle);
					}
					#endif
                }
            }
#ifdef FEATURE_USES_MMS
            if((pMe->m_eMBoxType == WMS_MB_OUTBOX_MMS) 
                || (pMe->m_eMBoxType == WMS_MB_INBOX_MMS)
                || (pMe->m_eMBoxType == WMS_MB_DRAFTBOX_MMS)
                )
            {
                WmsApp_UpdateMenuList_MMS(pMe, pMenu);
            }
            else
#endif                
            {
                // 创建消息菜单列表
                WmsApp_UpdateMenuList(pMe, pMenu);
            }
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            MSG_FATAL("IDD_MESSAGELIST_Handler pMe->m_currState=%d", pMe->m_currState, 0, 0);
            return TRUE;
          
        case EVT_USER_REDRAW:
            // 绘制底条提示
            // Read       Back
            //DRAW_BOTTOMBAR(BTBAR_READ_BACK)
            {
                wms_cache_info_list   *pList = NULL;
                AEERect  rc = {0};
                AEERect  menuRc = {0};
                int      nCount = 0;
#ifdef FEATURE_USES_MMS
                if(pMe->m_eMBoxType == WMS_MB_OUTBOX_MMS)
                {
                    ICONFIG_GetItem(pMe->m_pConfig,CFGI_MMS_OUTCOUNT,&g_mmsDataInfoMax,sizeof(g_mmsDataInfoMax));
					nCount = g_mmsDataInfoMax;	//Add By zzg 2012_02_25
                }
                else if(pMe->m_eMBoxType == WMS_MB_INBOX_MMS)
                {
                    ICONFIG_GetItem(pMe->m_pConfig,CFGI_MMS_INCOUNT,&g_mmsDataInfoMax,sizeof(g_mmsDataInfoMax));
					nCount = g_mmsDataInfoMax;	//Add By zzg 2012_02_25
                }
                else if(pMe->m_eMBoxType == WMS_MB_DRAFTBOX_MMS)
                {
                    ICONFIG_GetItem(pMe->m_pConfig,CFGI_MMS_DRAFTCOUNT,&g_mmsDataInfoMax,sizeof(g_mmsDataInfoMax));
					nCount = g_mmsDataInfoMax;	//Add By zzg 2012_02_25
                }                
                else
#endif 
                {
                    pList = wms_get_cacheinfolist(pMe->m_eMBoxType);

                    if (NULL != pList)
                    {
                        if(pMe->m_eMBoxType == WMS_MB_RSVFAILED)
                        {
                            nCount = pList->nOnUIMS;
                        }
                        else
                        {
                            nCount = pList->nBranches;
                        }
                    }
                }
                if(nCount > MAXITEMS_ONEPAGE)
                {
                    rc = pMe->m_rc;
                    IMENUCTL_GetRect(pMenu, &menuRc);
                    menuRc.dx = rc.dx - SCROLLBAR_WIDTH;
                    IMENUCTL_SetRect(pMenu, &menuRc);
            
                    rc.x = pMe->m_rc.dx - SCROLLBAR_WIDTH;
                    rc.dx = SCROLLBAR_WIDTH;
                    rc.y = 0;  //modify by miaoxiaoming indonesian
                    rc.dy = pMe->m_rc.dy  - BOTTOMBAR_HEIGHT; //modify by miaoxiaoming indonesian
                
                    Appscommon_DrawScrollBar(pMe->m_pDisplay, pMe->m_wCurPageStarxuhao + MAXITEMS_ONEPAGE - 1, nCount, MAXITEMS_ONEPAGE, &rc);
                    IDISPLAY_Update(pMe->m_pDisplay);
                }
            }            
            (void)IMENUCTL_Redraw(pMenu);
            return TRUE;
            
        case EVT_UPDATAMENU:
#ifdef FEATURE_USES_MMS            
            if((pMe->m_eMBoxType == WMS_MB_OUTBOX_MMS)
                || (pMe->m_eMBoxType == WMS_MB_INBOX_MMS)
                || (pMe->m_eMBoxType == WMS_MB_DRAFTBOX_MMS)
                )
            {
                WmsApp_UpdateMenuList_MMS(pMe, pMenu);
            }
            else
#endif                     
            // 创建消息菜单列表
            WmsApp_UpdateMenuList(pMe, pMenu);
            if (pMe->m_eMakeListMode == MAKEMSGLIST_RESUMECURPAGE)
            {
                (void)IMENUCTL_Redraw(pMenu);
            }
            return TRUE;

        // 有新消息
        case EVT_WMS_MSG_RECEIVED_MESSAGE:
            if (pMe->m_eMBoxType == WMS_MB_INBOX)
            {
                pMe->m_eMakeListMode = MAKEMSGLIST_RESUMECURPAGE;
                
                // 发送更新菜单列表事件
                (void) ISHELL_PostEventEx(pMe->m_pShell,
                                EVTFLG_ASYNC,
                                AEECLSID_WMSAPP,
                                EVT_UPDATAMENU,
                                0,
                                0);
            }
            return TRUE;
        
        case EVT_CTL_SEL_CHANGED:
            {
                uint16 nTitleID=0;
                AECHAR wszFmt[20] = {0};
                AECHAR wszTitle[32] = {0};
                wms_cache_info_list   *pList = NULL;
                uint32      dwcurxuhao=0;
                int         nLen = 0;
                int         nCount = 0;
                
                (void)IMENUCTL_GetItemData(pMenu, wParam, &dwcurxuhao);

				MSG_FATAL("***zzg IDD_MESSAGELIST dwcurxuhao=%d***", dwcurxuhao, 0, 0);
				
                pMe->m_wSelItemxuhao = dwcurxuhao;
                pList = wms_get_cacheinfolist(pMe->m_eMBoxType);
#ifdef FEATURE_USES_MMS
                if(pMe->m_eMBoxType == WMS_MB_OUTBOX_MMS)
                {
                    ICONFIG_GetItem(pMe->m_pConfig,CFGI_MMS_OUTCOUNT,&g_mmsDataInfoMax,sizeof(g_mmsDataInfoMax));
                    pMe->m_wSelItemxuhao = IMENUCTL_GetSel(pMenu);

					nCount = g_mmsDataInfoMax;	//Add By zzg 2012_02_25
                }
                else if(pMe->m_eMBoxType == WMS_MB_INBOX_MMS)
                {
                    ICONFIG_GetItem(pMe->m_pConfig,CFGI_MMS_INCOUNT,&g_mmsDataInfoMax,sizeof(g_mmsDataInfoMax));
                    pMe->m_wSelItemxuhao = IMENUCTL_GetSel(pMenu);

					MSG_FATAL("***zzg IDD_MESSAGELIST g_mmsDataInfoMax=%d, m_wSelItemxuhao=%d***", g_mmsDataInfoMax, pMe->m_wSelItemxuhao, 0);

					nCount = g_mmsDataInfoMax;	//Add By zzg 2012_02_25
                }
                else if(pMe->m_eMBoxType == WMS_MB_DRAFTBOX_MMS)
                {
                    ICONFIG_GetItem(pMe->m_pConfig,CFGI_MMS_DRAFTCOUNT,&g_mmsDataInfoMax,sizeof(g_mmsDataInfoMax));
                    pMe->m_wSelItemxuhao = IMENUCTL_GetSel(pMenu);

					nCount = g_mmsDataInfoMax;	//Add By zzg 2012_02_25
                }                
                else
#endif
                {
                    if (NULL != pList)
                    {
                        if(pMe->m_eMBoxType == WMS_MB_RSVFAILED)
                        {
                            nCount = pList->nOnUIMS;
                        }
                        else
                        {
                            nCount = pList->nBranches;
                        }
                    }
                }
                switch (pMe->m_eMBoxType)
                {
                    case WMS_MB_INBOX:
                        nTitleID = IDS_INBOX;
                        break;

                    case WMS_MB_OUTBOX:
                        nTitleID = IDS_OUTBOX;
                        break;

                    case WMS_MB_DRAFT:
                        nTitleID = IDS_DRAFT;
                        break;
                        
                    case WMS_MB_RESERVE:
                        nTitleID = IDS_RESERVEDMSGS;
                        break;
        
                    case WMS_MB_RSVFAILED:
                        nTitleID = IDS_FAILEDRSVDMSGS;
                        break;

#ifdef FEATURE_USES_MMS
                    case WMS_MB_INBOX_MMS:
                        nTitleID = IDS_INBOX_MMS;
                        break;
                    case WMS_MB_OUTBOX_MMS:
                        nTitleID = IDS_OUTBOX_MMS;
                        break;
                    case WMS_MB_DRAFTBOX_MMS:
                        nTitleID = IDS_DRAFT_MMS;
                        break;                        
#endif
                    default:
                        break;
                }
                
                if (nTitleID != 0)
                {
                    (void)STRTOWSTR(" (%d/%d)", wszFmt, sizeof(wszFmt));
                    
                    (void) ISHELL_LoadResString(pMe->m_pShell,
                            AEE_WMSAPPRES_LANGFILE,
                            nTitleID,
                            wszTitle,
                            sizeof(wszTitle));
                    nLen = WSTRLEN(wszTitle);
#ifdef FEATURE_USES_MMS    
                    if((pMe->m_eMBoxType == WMS_MB_OUTBOX_MMS)
                        || (pMe->m_eMBoxType == WMS_MB_INBOX_MMS)
                        || (pMe->m_eMBoxType == WMS_MB_DRAFTBOX_MMS)
                       )
                    {
                        dwcurxuhao = IMENUCTL_GetSel(pMenu);
						
						MSG_FATAL("***zzg IMENUCTL_GetSel dwcurxuhao=%d***", dwcurxuhao, 0, 0);
                    }
#endif
                    WSPRINTF(&wszTitle[nLen], (32-nLen)*sizeof(AECHAR), wszFmt, dwcurxuhao, nCount);
                    DBGPRINTF("EVT_CTL_SEL_CHANGED wszTitle=%S, dwcurxuhao=%d, nCount=%d",wszTitle, dwcurxuhao, nCount);
					#if 0
                    (void)IMENUCTL_SetTitle(pMenu, NULL, 0, wszTitle);
					#else
					{
						IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,wszTitle);
					}
					#endif
                }
            }
            (void)IMENUCTL_Redraw(pMenu);
            return TRUE;
        
        case EVT_DIALOG_END:
            pMe->m_eMakeListMode = MAKEMSGLIST_RESUMECURPAGE;
            if (pMe->m_eAppStatus == WMSAPP_SUSPEND)
            {
                pMe->m_wPrevMenuSel = IMENUCTL_GetSel(pMenu);
            }
            {
                uint32      dwcurxuhao=0;
                
                (void)IMENUCTL_GetItemData(pMenu, pMe->m_wPrevMenuSel, &dwcurxuhao);
                pMe->m_wSelItemxuhao = dwcurxuhao;
            }
            return TRUE;

        case EVT_KEY:
            {
                int nCount = IMENUCTL_GetItemCount(pMenu);
				
                switch(wParam)
                {
                    case AVK_DOWN:
                        if (nCount < 1)
                        {
                            return TRUE;
                        }
                        if (IMENUCTL_GetItemID(pMenu, nCount -1) == 
                            IMENUCTL_GetSel(pMenu))
                        {
                            // 菜单到底，需列举下一页菜单
                            pMe->m_eMakeListMode = MAKEMSGLIST_NEXTONE;
                            
                            // 发送更新菜单列表事件
                            (void) ISHELL_PostEventEx(pMe->m_pShell,
                                            EVTFLG_ASYNC,
                                            AEECLSID_WMSAPP,
                                            EVT_UPDATAMENU,
                                            0,
                                            0);
                        }
                        return TRUE;
                        
                    case AVK_UP:
                        if (IMENUCTL_GetItemID(pMenu, 0) == 
                            IMENUCTL_GetSel(pMenu))
                        {
                            // 菜单到顶，需列举上一页菜单
                            pMe->m_eMakeListMode = MAKEMSGLIST_BACKONE;
                            
                            // 发送更新菜单列表事件
                            (void) ISHELL_PostEventEx(pMe->m_pShell,
                                            EVTFLG_ASYNC,
                                            AEECLSID_WMSAPP,
                                            EVT_UPDATAMENU,
                                            0,
                                            0);
                        }
                        return TRUE;
                        
                    case AVK_LEFT:
                        // 菜单到顶，需列举上一页菜单
                        pMe->m_eMakeListMode = MAKEMSGLIST_PREPAGE;
                        
                        // 发送更新菜单列表事件
                        (void) ISHELL_PostEventEx(pMe->m_pShell,
                                        EVTFLG_ASYNC,
                                        AEECLSID_WMSAPP,
                                        EVT_UPDATAMENU,
                                        0,
                                        0);
                        return TRUE;
                        
                    case AVK_RIGHT:
                        // 菜单到顶，需列举上一页菜单
                        pMe->m_eMakeListMode = MAKEMSGLIST_NEXTPAGE;
                        
                        // 发送更新菜单列表事件
                        (void) ISHELL_PostEventEx(pMe->m_pShell,
                                        EVTFLG_ASYNC,
                                        AEECLSID_WMSAPP,
                                        EVT_UPDATAMENU,
                                        0,
                                        0);
                        return TRUE;
                        
                    case AVK_INFO:
                        pMe->m_wPrevMenuSel = IMENUCTL_GetSel(pMenu);
                        MSG_FATAL("IDD_MESSAGELIST_Handler Start  pMe->m_wPrevMenuSel=%d, pMe->m_eMBoxType=%d",pMe->m_wPrevMenuSel,pMe->m_eMBoxType,0);
#ifdef FEATURE_USES_MMS
                        if((pMe->m_eMBoxType == WMS_MB_OUTBOX_MMS)
                            || (pMe->m_eMBoxType == WMS_MB_INBOX_MMS)
                            || (pMe->m_eMBoxType == WMS_MB_DRAFTBOX_MMS)
                           )
                        {
                            MMSData mmsDataInfoList[MAX_MMS_STORED];
                            MMSData* pMmsDataInfoCur = NULL;
                            int nMmsDataInfo = 0;
                            IFile* pIFile = NULL;
                            IFileMgr *pIFileMgr = NULL;           
                            int result = 0;
                            pMe->m_wCurindex = pMe->m_wPrevMenuSel;
                            switch(pMe->m_eMBoxType)
                            {
                                case WMS_MB_OUTBOX_MMS:
                                {
                                    nMmsDataInfo = CFGI_MMSOUTDATA_INFO;
                                }
                                break;
                                case WMS_MB_INBOX_MMS:
                                {
                                    nMmsDataInfo = CFGI_MMSINDATA_INFO;
                                }
                                break;
                                case WMS_MB_DRAFTBOX_MMS:
                                {
                                    nMmsDataInfo = CFGI_MMSDRAFTDATA_INFO;
                                }
                                break;                    
                            }; 
                            (void) ICONFIG_GetItem(pMe->m_pConfig,
                                               nMmsDataInfo,
                                               (void*)mmsDataInfoList,
                                               sizeof(mmsDataInfoList));   
                            pMmsDataInfoCur  = &mmsDataInfoList[pMe->m_wCurindex-1]; 
                            DBGPRINTF("nInfoIndex=%d,MMSDataFileName=%s",pMe->m_wCurindex-1,pMmsDataInfoCur->MMSDataFileName);
                            result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);
                            if(SUCCESS == result)
                            {
                                pIFile = IFILEMGR_OpenFile( pIFileMgr, pMmsDataInfoCur->MMSDataFileName, _OFM_READ);     
                                if((pMmsDataInfoCur != NULL) && (pIFile != NULL))
                                {
                                    MSG_FATAL("IDD_MESSAGELIST_Handler AVK_INFO 1", 0,0,0);
                                    MSG_FATAL("IDD_MESSAGELIST_Handler pMe->m_currState=%d", pMe->m_currState, 0, 0);
                                    CLOSE_DIALOG(DLGRET_LOADOK)
                                }
                                else
                                {
                                    MSG_FATAL("IDD_MESSAGELIST_Handler AVK_INFO 2", 0,0,0);
                                    CLOSE_DIALOG(DLGRET_CANCELED)
                                }
                            }
                            else
                            {
                                MSG_FATAL("IDD_MESSAGELIST_Handler AVK_INFO 3", 0,0,0);
                                CLOSE_DIALOG(DLGRET_CANCELED)
                            }    
                            RELEASEIF(pIFile);
                            RELEASEIF(pIFileMgr);                            
                        }
                        else
#endif
                        {
                            pMe->m_wCurindex = pMe->m_wPrevMenuSel - MSG_CMD_BASE;
                            MSG_FATAL("IDD_MESSAGELIST_Handler pMe->m_wCurindex:%d, m_wPrevMenuSel:%d", pMe->m_wCurindex,pMe->m_wPrevMenuSel,0);
                            CLOSE_DIALOG(DLGRET_LOAD)
                        }
                        return TRUE;
                        
                    case AVK_CLR:
						if(dwParam != 1)
						{
                        	CLOSE_DIALOG(DLGRET_CANCELED)
                        }
                        else
                        {
                        	{
                        		// 读取消息
                        		pMe->m_wPrevMenuSel = IMENUCTL_GetSel(pMenu);
                        		pMe->m_wCurindex = pMe->m_wPrevMenuSel - MSG_CMD_BASE;
                        		WmsApp_ReadMsg(pMe);
                        		MSG_FATAL("pMe->m_wCurindex=======%d",pMe->m_wCurindex,0,0);
                    			CLOSE_DIALOG(DLGRET_DELETE)
                    		}
                    
                        }
                        return TRUE;
//miaoxiaoming add , press send key at messagelist page,then dial out  						
#ifdef CUST_EDITION
#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)
					case AVK_CAMERA:
			 		{
						nv_item_type	  SimChoice;
						OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
						if(SimChoice.sim_select ==AVK_SEND_TWO)
						{
							wms_cache_info_node *pNode;
							AECHAR  wstrNum[MAX_PH_DIGITS+1];
							wms_message_index_type wIndex = 0;
							
							if (pMe->m_eMBoxType == WMS_MB_INBOX)
							{				
							
								pMe->m_wPrevMenuSel = IMENUCTL_GetSel(pMenu);
								pMe->m_wCurindex = pMe->m_wPrevMenuSel - MSG_CMD_BASE;
								wIndex = pMe->m_wCurindex;
	                
				                // 取消息 cache info 节点
				                if (wIndex>=RUIM_MSGINDEX_BASE)
				                {
				                    wIndex = wIndex - RUIM_MSGINDEX_BASE;
				                    pNode = wms_cacheinfolist_getnode(pMe->m_eMBoxType, WMS_MEMORY_STORE_RUIM, wIndex);
				                }
				                else
				                {
				                    pNode = wms_cacheinfolist_getnode(pMe->m_eMBoxType, WMS_MEMORY_STORE_NV_CDMA, wIndex);
				                }
														
								(void)STRTOWSTR(pNode->pszNum, wstrNum, sizeof(wstrNum));
								
								if (WSTRLEN(wstrNum) > 0)
			                    {
			                        // 调用呼叫接口，本 Applet 会被挂起，返回时回到当前状态
			                        WMSExtApp_CallPhoneNumber(pMe,wstrNum, FALSE);
			                    }
								return TRUE;
							}
							return TRUE;
						}
					}
					break;
#endif
					case AVK_SEND:
					{
						wms_cache_info_node *pNode;
						AECHAR  wstrNum[MAX_PH_DIGITS+1];
						wms_message_index_type wIndex = 0;
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

						MSG_FATAL("***zzg AVK_SEND CallPhoneNumber 111 m_eMBoxType=%x", pMe->m_eMBoxType, 0, 0);

						#ifdef FEATURE_VERSION_C337
						if ((pMe->m_eMBoxType == WMS_MB_INBOX) || (pMe->m_eMBoxType == WMS_MB_OUTBOX))						
						#else
						if (pMe->m_eMBoxType == WMS_MB_INBOX)
						#endif							
						{				
						
							pMe->m_wPrevMenuSel = IMENUCTL_GetSel(pMenu);
							pMe->m_wCurindex = pMe->m_wPrevMenuSel - MSG_CMD_BASE;
							wIndex = pMe->m_wCurindex;
                
			                // 取消息 cache info 节点
			                if (wIndex>=RUIM_MSGINDEX_BASE)
			                {
			                    wIndex = wIndex - RUIM_MSGINDEX_BASE;
			                    pNode = wms_cacheinfolist_getnode(pMe->m_eMBoxType, WMS_MEMORY_STORE_RUIM, wIndex);
			                }
			                else
			                {
			                    pNode = wms_cacheinfolist_getnode(pMe->m_eMBoxType, WMS_MEMORY_STORE_NV_CDMA, wIndex);
			                }
													
							(void)STRTOWSTR(pNode->pszNum, wstrNum, sizeof(wstrNum));
							
							if (WSTRLEN(wstrNum) > 0)
		                    {
		                        // 调用呼叫接口，本 Applet 会被挂起，返回时回到当前状态
		                        WMSExtApp_CallPhoneNumber(pMe,wstrNum, FALSE);
		                    }
							return TRUE;
						}
						return TRUE;
					}
#endif                        
                    default: 
                        break;
                }
            }
            
            return TRUE;
            
        case EVT_COMMAND:
            pMe->m_wPrevMenuSel = wParam;
            pMe->m_wCurindex = pMe->m_wPrevMenuSel - MSG_CMD_BASE;
            MSG_FATAL("pMe->m_wCurindex---%d", pMe->m_wCurindex, 0, 0);
            
            CLOSE_DIALOG(DLGRET_OPT)
            return TRUE;
            
        default:
            break;
    }

    return FALSE;
} // IDD_MESSAGELIST_Handler


/*==============================================================================
函数:
    IDD_VIEWMSG_Handler

说明:
    WMS Applet 对话框 IDD_VIEWMSG 事件处理函数。(查看一条消息，并对用户
    的操作作处理）

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam: 事件参数
    dwParam [in]: 与wParam关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:
    New
    
==============================================================================*/
static boolean IDD_VIEWMSG_Handler(void         *pUser, 
                                   AEEEvent     eCode, 
                                   uint16       wParam, 
                                   uint32       dwParam)
{
    IStatic * pStatic = NULL;
    WmsApp *pMe = (WmsApp *)pUser;
    MSG_FATAL("IDD_VIEWMSG_Handler eCode=0x%x, wParam=0x%x, dwParam=0x%x", eCode, wParam, dwParam);
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    pStatic = (IStatic*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_VIEWMSG_STATIC);

    if (NULL == pStatic)
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
                #if 0
                SETAEERECT(&rc,  0, pMe->m_rc.y + TITLEBAR_HEIGHT,
                            pMe->m_rc.dx,
                            pMe->m_rc.dy - BOTTOMBAR_HEIGHT - TITLEBAR_HEIGHT);
				#else
				SETAEERECT(&rc,  0, pMe->m_rc.y ,
                            pMe->m_rc.dx,
                            pMe->m_rc.dy - BOTTOMBAR_HEIGHT);
				#endif
                ISTATIC_SetRect(pStatic, &rc);
            }
            IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
            // 设置静态文本控件属性
            #ifdef FEATURE_VERSION_C316
            ISTATIC_SetProperties(pStatic, ST_CENTERTITLE | ST_NOSCROLL | ST_DISPLATSMS | ST_GRAPHIC_BG |ST_GRAPHIC_BLUE);
			#else
			ISTATIC_SetProperties(pStatic, ST_CENTERTITLE | ST_NOSCROLL | ST_DISPLATSMS | ST_GRAPHIC_BG);
			#endif
            return TRUE;

        case EVT_DIALOG_START:
            WmsApp_UpdateAnnunciators(pMe);
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;

        case EVT_USER_REDRAW:
            {
                AECHAR *pFormatedText = NULL;
                AECHAR wszTitle[32] = {0};
                uint16 nTitleID=0;
                TitleBar_Param_type     TBarParam = {0};                
                MSG_FATAL("IDD_VIEWMSG_Handler EVT_USER_REDRAW m_currState=%d",pMe->m_currState,0,0);
             
                switch (pMe->m_currState)
                {
                    // 查看收件箱消息
                    case WMSST_VIEWINBOXMSG:
                        nTitleID = IDS_INBOX;
                        break;
                        
                    // 查看发件箱消息
                    case WMSST_VIEWOUTBOXMSG:
                        nTitleID = IDS_OUTBOX;
                        break;
                        
                    // 查看草稿箱消息
                    case WMSST_VIEWDRAFTMSG:
                        nTitleID = IDS_DRAFT;
                        break;
                        
                    // 查看常用语
                    case WMSST_VIEWTEMPLATE:
                        nTitleID = IDS_TEMPLATES;
                        break;

                    //查看预约短信
                    case WMSST_VIEWRESERVEDMSG:
                        nTitleID = IDS_RESERVEDMSG;
                        break;

#ifdef FEATURE_USES_MMS
                    case WMSST_VIEWINBOXMSG_MMS:
                        nTitleID = IDS_INBOX_MMS;
                        break;
                    case WMSST_VIEWOUTBOXMSG_MMS:
                        nTitleID = IDS_OUTBOX_MMS;
                        break;
                    case WMSST_VIEWDRAFTBOXMSG_MMS:
                        nTitleID = IDS_DRAFT_MMS;
                        break;                        
#endif
                    default :
                        break;
                }
                
                if (nTitleID != 0)
                {
                    (void) ISHELL_LoadResString(pMe->m_pShell,
                            AEE_WMSAPPRES_LANGFILE,
                            nTitleID,
                            wszTitle,
                            sizeof(wszTitle));
                }

                TBarParam.pwszTitle = wszTitle;
				#if 0
                TBarParam.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
                DrawTitleBar(pMe->m_pDisplay, &TBarParam);
				#else
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,wszTitle);
				#endif

                MEMSET(wszTitle, 0, sizeof(wszTitle));
                
                if ((pMe->m_currState == WMSST_VIEWINBOXMSG) && 
                    ((pMe->m_msCur.m_epriority == WMS_PRIORITY_URGENT) || 
                     (pMe->m_msCur.m_epriority == WMS_PRIORITY_EMERGENCY)))
                {
                    AECHAR wstrFmt[32] = {0};
                    AECHAR wstrTep[32] = {0};
                    wszTitle[0] = 0;

                    (void)STRTOWSTR("%s !", wstrFmt, sizeof(wstrFmt));
                    (void) ISHELL_LoadResString(pMe->m_pShell,
                                                AEE_WMSAPPRES_LANGFILE,
                                                ((pMe->m_msCur.m_epriority == WMS_PRIORITY_URGENT) ? IDS_URGENT : IDS_EMERGENCY),
                                                wstrTep,
                                                sizeof(wstrTep));
                                                
                    WSPRINTF(wszTitle, sizeof(wszTitle), wstrFmt, wstrTep);
                }
                
                if ((pMe->m_currState == WMSST_VIEWTEMPLATE) &&
                    (pMe->m_msSend.m_szMessage != NULL))
                {
                    DBGPRINTF("Draft m_szMessage=%s", pMe->m_msSend.m_szMessage);
                    // 设置静态控件文本
                    (void)ISTATIC_SetText(pStatic,
                            NULL,
                            pMe->m_msSend.m_szMessage,
                            AEE_FONT_NORMAL,
                            AEE_FONT_NORMAL);
                }
                else
                {
                    // 格式化消息便于阅读
                    if ((pMe->m_currState == WMSST_VIEWTEMPLATE) &&
                        (pMe->m_msSend.m_szMessage != NULL))
                    {
                        pFormatedText = FormatMessageForDisplay(pMe, &pMe->m_msSend);
                    }
                    else
                    {
                        pFormatedText = FormatMessageForDisplay(pMe, &pMe->m_msCur);
                        DBGPRINTF("Draft pFormatedText=%S", pFormatedText);
                    }
#ifdef FEATURE_USES_MMS                    
                    {
                        char chrStr[100] = {0};
                        pMe->m_isMMSNotify = FALSE;
                        WSTRTOSTR(pFormatedText,(char*)&chrStr,sizeof(chrStr));
                        if(STRISTR((char*)&chrStr,"mms:http") != NULL)
                        {
                            pMe->m_isMMSNotify = TRUE;
                        }
                        else
                        {
                            pMe->m_isMMSNotify = FALSE;
                        }
                    }
#endif                    
                    if (NULL != pFormatedText)
                    {
                        DBGPRINTF("Draft pFormatedText=%S", pFormatedText);
                        // 设置静态控件文本
                        (void)ISTATIC_SetText(pStatic,
                                WSTRLEN(wszTitle)>0 ? wszTitle : NULL,
                                pFormatedText,
                                AEE_FONT_BOLD,
                                AEE_FONT_NORMAL);
                        
                        // 释放格式化消息时动态分配的空间
                        FREE(pFormatedText);
                    }
                }
                ISTATIC_SetActive(pStatic, TRUE);
                ISTATIC_Redraw(pStatic);
                
                // 绘制底条提示
                // Option       Back
#ifdef FEATURE_USES_MMS
                if(pMe->m_isMMSNotify)
                {
                    DRAW_BOTTOMBAR(BTBAR_OPTION_OK_BACK)
                }
                else
                {
                    DRAW_BOTTOMBAR(BTBAR_OPTION_BACK)
                }    
#else
                DRAW_BOTTOMBAR(BTBAR_OPTION_BACK)
#endif
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            }         
            return TRUE;
            
		case EVT_WMS_MSG_RECEIVED_MESSAGE:
			{
				switch (pMe->m_currState)
                {
                	case WMSST_VIEWINBOXMSG:
                        {
							wms_cache_info_node  *pnode = NULL;
				            uint16 wIndex=pMe->m_wCurindex;
				            MSG_FATAL("wIndex=============EVT_WMS_MSG_RECEIVED_MESSAGE===========%d",wIndex,0,0);
				            //释放查看的消息内存
							WMSMessageStruct_Free(pMe);
							//ADD BY YANGDECAI 2010-08-16
				            // 取消息 cache info 节点
				            if (wIndex>=RUIM_MSGINDEX_BASE)
				            {
				                wIndex = wIndex - RUIM_MSGINDEX_BASE;
				                pnode = wms_cacheinfolist_getnode(WMS_MB_INBOX, WMS_MEMORY_STORE_RUIM, wIndex);
				            }
				            else
				            {
				                pnode = wms_cacheinfolist_getnode(WMS_MB_INBOX, WMS_MEMORY_STORE_NV_CDMA, wIndex);
				            }
				            
				            if (NULL != pnode)
				            {
#ifdef FEATURE_SMS_UDH
				                if (pnode->pItems != NULL)
				                {
				                    int i;
				                    
				                    for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
				                    {
				                        pnode = pMe->m_CurMsgNodes[i];
				                        
				                        if ((NULL != pnode) &&
				                            (pnode->msg_tag == WMS_TAG_MT_READ))
				                        {
				                            (void)IWMS_MsgModifyTag(pMe->m_pwms,
				                                                    pMe->m_clientId,
				                                                    &pMe->m_callback,
				                                                    (void *)pMe,
				                                                    pnode->mem_store,
				                                                    pnode->index,
				                                                    WMS_TAG_MT_NOT_READ);
				                        }
				                    }
				                }
#else
							 if ((NULL != pnode) &&
                            (pnode->msg_tag == WMS_TAG_MT_READ))
				             {
		                            (void)IWMS_MsgModifyTag(pMe->m_pwms,
		                                                    pMe->m_clientId,
		                                                    &pMe->m_callback,
		                                                    (void *)pMe,
		                                                    pnode->mem_store,
		                                                    pnode->index,
		                                                    WMS_TAG_MT_NOT_READ);
	                        }
#endif
				           }
		
						CLOSE_DIALOG(DLGRET_CANCELED)
						}
                        break;
					default:
						break;
				}
			}
			return TRUE;

        case EVT_DIALOG_END:
			
            return TRUE;
            
        case EVT_KEY_PRESS:
            switch(wParam)
            {
                case AVK_1:
                    if (pMe->m_currState == WMSST_VIEWVOICEMAIL)
                    {// 呼叫语音信箱
                        CLOSE_DIALOG(DLGRET_CALLVMNUM)
                    }
                    return TRUE;
                    
#if !defined(FEATURE_CARRIER_VENEZUELA_MOVILNET)
                case AVK_2:
                    if (pMe->m_currState == WMSST_VIEWVOICEMAIL)
                    {// 删除语音邮件
                        CLOSE_DIALOG(DLGRET_DELETE)
                    }
                    return TRUE;
#endif                    
                
                default:
                    break;
            }
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
					{
						WMSMessageStruct *pTep;
						int i;
						int max  = 1;
						int nlen = 0;
						int         nBranches=0;
    					int         nCurBranchNum=0;
						#ifdef FEATURE_SMS_UDH
						uint8       total_sm;   // 消息数据包总数
						uint8       seq_num;    // 消息序号
						#endif
						 for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
    					{
					        pTep = pMe->m_CurMsgNodesMS[i];
					        if (pTep != NULL)
					        {
					            break;
					        }
					    }
						if (NULL == pTep)
    					{// 任一非空节点没找到
    					    CLOSE_DIALOG(DLGRET_CANCELED)
        					return TRUE;
    					}
    
#ifdef FEATURE_SMS_UDH
					    total_sm = pTep->total_sm;
					    seq_num = pTep->seq_num;
					    if (total_sm > 1)
					    {
					        // 计算长短信分支总数(每个分支最大含 LONGSMS_MAX_PACKAGES 个数据包)
					        nBranches = total_sm / (LONGSMS_MAX_PACKAGES);
					        if ((total_sm % (LONGSMS_MAX_PACKAGES)) != 0)
					        {
					            nBranches++;
					        }
					        
					        // 确定当前处理的分支号(0,...,nBranches-1)
					        nCurBranchNum = (seq_num - 1) / (LONGSMS_MAX_PACKAGES);
					        if (nBranches>1)
					        {
					            nlen += 20;
					            
					            if (nCurBranchNum < (nBranches-1))
					            {
					                max = LONGSMS_MAX_PACKAGES;
					            }
					            else
					            {
					                max = total_sm - nCurBranchNum*LONGSMS_MAX_PACKAGES;
					            }
					        }
					        else
					        {
					            max = total_sm;
					        }
					    }
#endif   
						for(i = 0;i<max;i++)
						{
							WMSMessageStruct_Reset(pMe->m_CurMsgNodesMS[i]);
						}
					}  //ADD BY YANGDECAI 2010-08-16
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

				//Add By zzg 2010_09_09
				case AVK_SEND:	
				{
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

					MSG_FATAL("***zzg AVK_SEND CallPhoneNumber 222 m_currState=%x", pMe->m_currState, 0, 0);

					#ifdef FEATURE_VERSION_C337
					if ((pMe->m_currState == WMSST_VIEWINBOXMSG) || (pMe->m_currState == WMSST_VIEWOUTBOXMSG))	
					#else
					if (pMe->m_currState == WMSST_VIEWINBOXMSG)
					#endif	
                	{                	
#if 1//defined(FEATURE_CARRIER_VENEZUELA_MOVILNET) || defined(FEATURE_CARRIER_THAILAND_HUTCH)
	                    // 该运营商要求 CBN 优先
	                    
	                    MSG_FATAL("***zzg m_szCallBkNum=%s", (char *)pMe->m_msCur.m_szCallBkNum, 0, 0);

	                    if (WSTRLEN(pMe->m_msCur.m_szCallBkNum) > 0)
	                    {
	                        // 调用呼叫接口，本 Applet 会被挂起，返回时回到当前状态
	                        WMSExtApp_CallPhoneNumber(pMe, pMe->m_msCur.m_szCallBkNum, FALSE);
	                    }
	                    else 
#endif                    
	                    if (WSTRLEN(pMe->m_msCur.m_szNum) > 0)
	                    {
	                        // 调用呼叫接口，本 Applet 会被挂起，返回时回到当前状态
	                        WMSExtApp_CallPhoneNumber(pMe, pMe->m_msCur.m_szNum, FALSE);
	                    }
					}
					return TRUE;
				}
				//Add End
  #if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)
  				case AVK_CAMERA:
  				{
  					nv_item_type	  SimChoice;
					OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
					if(SimChoice.sim_select ==AVK_SEND_TWO)
					{
						if (pMe->m_currState == WMSST_VIEWINBOXMSG)
	                	{                	
#if 1//defined(FEATURE_CARRIER_VENEZUELA_MOVILNET) || defined(FEATURE_CARRIER_THAILAND_HUTCH)
		                    // 该运营商要求 CBN 优先
		                    if (WSTRLEN(pMe->m_msCur.m_szCallBkNum) > 0)
		                    {
		                        // 调用呼叫接口，本 Applet 会被挂起，返回时回到当前状态
		                        WMSExtApp_CallPhoneNumber(pMe, pMe->m_msCur.m_szCallBkNum, FALSE);
		                    }
		                    else 
#endif                    
		                    if (WSTRLEN(pMe->m_msCur.m_szNum) > 0)
		                    {
		                        // 调用呼叫接口，本 Applet 会被挂起，返回时回到当前状态
		                        WMSExtApp_CallPhoneNumber(pMe, pMe->m_msCur.m_szNum, FALSE);
		                    }
						}
						return TRUE;
					}
  				}
  				break;
  #endif
                case AVK_SELECT:
                    CLOSE_DIALOG(DLGRET_OK)
                    return TRUE;
  
                case AVK_INFO:
                    CLOSE_DIALOG(DLGRET_INFO)
                    return TRUE;
                    
                default:
                    break;
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
			
							if(WMSAPP_PT_IN_RECT(wXPos,wYPos,rc))
							{
								if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
								{
									boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_SELECT,0);
									return rt;
								}
								else if(wXPos >= rc.x + (rc.dx/3)	&& wXPos < rc.x + (rc.dx/3)*2 )//左
								{
									 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_INFO,0);
									 return rt;
								}
								else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
								{						
									 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_CLR,0);
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
} // IDD_VIEWMSG_Handler

/*==============================================================================
函数:
    IDD_DELETEMSGS_Handler

说明:
    WMS Applet对话框 IDD_DELETEMSGS 事件处理函数。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_DELETEMSGS_Handler(void *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;
  
    if (NULL == pMe)
    {
        return FALSE;
    }

    pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,
                                          IDC_DELETEMSGS_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
#endif
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,                                
                        IDS_DELETEMSGS,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);
            //SetControlRect(pMe,pMenu);
            return TRUE;

        case EVT_DIALOG_START:
            MENU_ADDITEM(pMenu, IDS_INBOX); 
#ifdef FEATURE_CDSMS_RUIM
            if (IsRunAsUIMVersion())
            {
                MENU_ADDITEM(pMenu, IDS_RUIM); 
                MENU_ADDITEM(pMenu, IDS_PHONE); 
            }
#endif         
            //MENU_ADDITEM(pMenu, IDS_DELETE_READMSG);  
            MENU_ADDITEM(pMenu, IDS_OUTBOX); 
#if 0//def FEATURE_USES_MMS
            MENU_ADDITEM(pMenu, IDS_INBOX_MMS); 
            MENU_ADDITEM(pMenu, IDS_OUTBOX_MMS);
            MENU_ADDITEM(pMenu, IDS_DRAFT_MMS);
#endif
            MENU_ADDITEM(pMenu, IDS_DRAFT); 
            
#if (defined FEATURE_RESERVEDMSG) && (!defined FEATURE_CARRIER_THAILAND_HUTCH)
            MENU_ADDITEM(pMenu, IDS_RESERVEDMSG);
#endif
            MENU_ADDITEM(pMenu, IDS_ALL); 
            IMENUCTL_SetSel(pMenu, pMe->m_wPrevMenuSel);
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);

            return TRUE;
  
        case EVT_USER_REDRAW:
            // 绘制底条提示
            // Select       Back
            //DRAW_BOTTOMBAR(BTBAR_SELECT_BACK)

            // 更新界面
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            //(void)IMENUCTL_Redraw(pMenu);  //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            pMe->m_wPrevMenuSel = wParam;
            switch (wParam)
            {
                // 删除全部收件箱消息(话机+UIM卡)
                case IDS_INBOX:
                    pMe->m_eEraseWMSType = CLEAR_INBOXES;
                    CLOSE_DIALOG(DLGRET_CLEARINBOXES)
                    return TRUE;
                    
#ifdef FEATURE_CDSMS_RUIM
                // 删除全部UIM卡消息
                case IDS_RUIM:
                    pMe->m_eEraseWMSType = CLEAR_RUIM;
                    CLOSE_DIALOG(DLGRET_CLEARRUIM)
                    return TRUE;                
#endif                
                    
                // 删除全部收件箱话机上消息
                case IDS_PHONE:
                    pMe->m_eEraseWMSType = CLEAR_PHONE;
                    CLOSE_DIALOG(DLGRET_CLEARPHONE)
                    return TRUE;
                    
                // 删除全部发件箱内消息
                case IDS_OUTBOX:
                    pMe->m_eEraseWMSType = CLEAR_OUTBOX;
                    CLOSE_DIALOG(DLGRET_CLEAROUTBOX)
                    return TRUE;
                    
                // 删除全部草稿消息
                case IDS_DRAFT:
                    pMe->m_eEraseWMSType = CLEAR_DRAFT;
                    CLOSE_DIALOG(DLGRET_CLEARDRAFT)
                    return TRUE;                

                //删除全部预约信息
                case IDS_RESERVEDMSG:
                    pMe->m_eEraseWMSType = CLEAR_ALLRESERVED;
                    CLOSE_DIALOG(DLGRET_CLEARRESERVED)
                    return TRUE;

                /*
                // 删除已读消息
                case IDS_DELETE_READMSG:
                    pMe->m_eEraseWMSType = ERASE_READMSG;
                    CLOSE_DIALOG(DLGRET_DELETEREADMSG)
                    return TRUE;
                */
                    
                // 删除全部消息
                case IDS_ALL:
                    pMe->m_eEraseWMSType = CLEAR_ALL;
                    CLOSE_DIALOG(DLGRET_CLEARALL)
                    return TRUE;
                    
#ifdef FEATURE_USES_MMS
                case IDS_INBOX_MMS:
                    pMe->m_eEraseWMSType = CLEAR_INBOX_MMS;
                    CLOSE_DIALOG(DLGRET_CLEARINBOX_MMS)
                    return TRUE;
                case IDS_OUTBOX_MMS:
                    MSG_FATAL("IDD_DELETEMSGS_Handler IDS_OUTBOX_MMS",0,0,0);
                    pMe->m_eEraseWMSType = CLEAR_OUTBOX_MMS;
                    CLOSE_DIALOG(DLGRET_CLEAROUTBOX_MMS)
                    return TRUE;

                // 删除全部草稿消息
                case IDS_DRAFT_MMS:
                    pMe->m_eEraseWMSType = CLEAR_DRAFTBOX_MMS;
                    CLOSE_DIALOG(DLGRET_DRAFTBOX_MMS)
                    return TRUE;                          
#endif
                    
                default:
                    break;
            }
            break;

        default:
            break;
    }

    return FALSE;
} // IDD_DELETEMSGS_Handler()

/*==============================================================================
函数:
    IDD_SETTING_Handler

说明:
    WMS Applet对话框 IDD_SETTING 事件处理函数。显示关于短消息设置界面给用户作选择。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_SETTING_Handler(void   *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;
    MSG_FATAL("IDD_SETTING_Handler Start eCode=0x%x", eCode, 0, 0);
    if (NULL == pMe)
    {
        return FALSE;
    }

    pMenu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_SETTING_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
#endif
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,                                
                        IDS_MSGSETTING,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);
                     

            // 菜单项初始化
            MENU_ADDITEM(pMenu, IDS_MSGVALIDITY);
			//MENU_ADDITEM(pMenu, IDS_TIME_STAMP);	//Add By zzg 2012_02_04
#if (!defined FEATURE_CARRIER_TAIWAN_APBW) && (!defined FEATURE_CARRIER_THAILAND_HUTCH)&&!defined(FEATURE_VERSION_W021_CT100)
            MENU_ADDITEM(pMenu, IDS_REPORTTITLE); //IDS_DELIVERYREPORTS);
#endif            
#ifndef FEATURE_CARRIER_THAILAND_HUTCH              
            MENU_ADDITEM(pMenu, IDS_PRIORITY);
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH
            MENU_ADDITEM(pMenu, IDS_SENDMODE);
#ifdef FEATURE_RESERVEDMSG  
#endif                   
#if 1//def FEATURE_CARRIER_TAIWAN_APBW        //add by yangdecai   2010-08-23 
            MSG_FATAL("IDD_SETTING_Handler EVT_DIALOG_INIT IDS_CALLBACKNUM", 0, 0, 0);
#if !defined(FEATURE_VERSION_C316)&&!defined(FEATURE_VERSION_K202_LM129C)&&!defined(FEATURE_VERSION_W021_CT100)
            MENU_ADDITEM(pMenu, IDS_CALLBACKNUM);
#endif
#endif 
#ifdef FEATURE_USES_MMS  
            MENU_ADDITEM(pMenu,IDS_MMSNOTIFY);
            MENU_ADDITEM(pMenu,IDS_MMSDELIVERYREPORT);
            MENU_ADDITEM(pMenu,IDS_READREPLY);
            MENU_ADDITEM(pMenu,IDS_REPORTALLOWED);
            MENU_ADDITEM(pMenu,IDS_SENDERVISIBILITY);
            MENU_ADDITEM(pMenu,IDS_MMS_SERVER_ADDRESS);
            MENU_ADDITEM(pMenu,IDS_MMS_PROXY);
            MENU_ADDITEM(pMenu,IDS_MMS_PORT);
#endif
            IMENUCTL_SetSel(pMenu, pMe->m_wPrevMenuSel);
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);

            return TRUE;

        case EVT_USER_REDRAW:
            // 绘制底条提示
            // Select       Back
            //DRAW_BOTTOMBAR(BTBAR_SELECT_BACK)
            
            // 更新界面
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            //(void)IMENUCTL_Redraw(pMenu);   //dele by yangdecai
            return TRUE;

        case EVT_CTL_SEL_CHANGED:
            pMe->m_wPrevMenuSel = wParam;
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;					
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            pMe->m_wPrevMenuSel = wParam;
            switch (wParam)
            {
                // 发出短信存储选项
                case IDS_AUTOSAVE:
                    CLOSE_DIALOG(DLGRET_SETTINGAUTOSAVE)
                    return TRUE;

                // 发出短信的优先级设置
                case IDS_PRIORITY:
                    CLOSE_DIALOG(DLGRET_PRIORITY)
                    return TRUE;
                
                // 发出短信是否需要递交报告
                case IDS_REPORTTITLE: //IDS_DELIVERYREPORTS:
                    CLOSE_DIALOG(DLGRET_REPORTS)
                    return TRUE;
                    
#ifdef FEATURE_AUTOREPLACE
                // 收件箱满时，是否允许用新消息自动替换最旧消息开关设置
                case IDS_AUTOREPLACE:
                    CLOSE_DIALOG(DLGRET_AUTOREPLACE)
                    return TRUE;
#endif                    
#if 1//def FEATURE_CARRIER_TAIWAN_APBW        //add by yangdecai   2010-08-23 
                // 发出短信是否带回叫号码
                MSG_FATAL("IDD_SETTING_Handler EVT_COMMAND IDS_CALLBACKNUM", 0, 0, 0);
#if !defined(FEATURE_VERSION_C316)&&!defined(FEATURE_VERSION_K202_LM129C)
                case IDS_CALLBACKNUM:
                    CLOSE_DIALOG(DLGRET_CALLBACKNUM)
                    return TRUE;
#endif										
#endif
                //发送模式
                case IDS_SENDMODE:
                    CLOSE_DIALOG(DLGRET_SENDMODE)
                    return TRUE; 
#ifdef FEATURE_USES_MMS   
                case IDS_MMSNOTIFY:
                    CLOSE_DIALOG(DLGRET_MMSNOTIFY)
                     return TRUE;
                     
                case IDS_MMSDELIVERYREPORT:
                    CLOSE_DIALOG(DLGRET_MMSDELIVERYREPORT)
                     return TRUE;
                     
                case IDS_READREPLY:
                    CLOSE_DIALOG(DLGRET_READREPLY)
                     return TRUE;
                     
                case IDS_REPORTALLOWED:
                    CLOSE_DIALOG(DLGRET_REPORTALLOWED)
                     return TRUE;
                     
                case IDS_SENDERVISIBILITY:
                    CLOSE_DIALOG(DLGRET_SENDERVISIBILITY)
                     return TRUE;

                case IDS_MMS_SERVER_ADDRESS:
                    CLOSE_DIALOG(DLGRET_MMS_SERVER_ADDRESS)
                    return TRUE;         

                case IDS_MMS_PROXY:
                    CLOSE_DIALOG(DLGRET_MMS_PROXY)
                    return TRUE;    

                case IDS_MMS_PORT:
                    CLOSE_DIALOG(DLGRET_MMS_PORT)
                    return TRUE;                        
#endif                     
                // 发出短信相对有效期设置
                case IDS_MSGVALIDITY:
                    CLOSE_DIALOG(DLGRET_MSGVALIDITY)
                    return TRUE;

				//Add By zzg 2012_02_04
				case IDS_TIME_STAMP:
                    CLOSE_DIALOG(DLGRET_TIME_STAMP)
                    return TRUE;
				//Add End
                    
                // 收到短信时，若设定空间满是否自动删除已读消息腾出空间
                case IDS_AUTODELETE:
                    CLOSE_DIALOG(DLGRET_AUTODELETE)
                    return TRUE;
                    
                // 预约短信提醒超时缺省发送设置
                case IDS_RESERVEDMSG:
                    CLOSE_DIALOG(DLGRET_RESERVEDMSGALERTTIMEOUT)
                    return TRUE;
                                           
                default:
                    break;
            }
            
            return FALSE;

        default:
            break;
    }

    return FALSE;
} // IDD_SETTING_Handler

/*==============================================================================
函数:
    IDD_RESERVEDMSG_Handler

说明:
    WMS Applet对话框 IDD_RESERVEDMSG 事件处理函数。显示关于短消息设置界面给用户作选择。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_RESERVEDMSG_Handler(void   *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;

    if (NULL == pMe)
    {
        return FALSE;
    }

    pMenu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_RESERVEDMSG_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
#endif
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,                                
                        IDS_RESERVEDMSG,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);
            
            // 菜单项初始化
            MENU_ADDITEM(pMenu, IDS_NEWRESERVEDMSG);
            MENU_ADDITEM(pMenu, IDS_RESERVEDMSGS);
            MENU_ADDITEM(pMenu, IDS_FAILEDRSVDMSGS);
            IMENUCTL_SetSel(pMenu, pMe->m_wPrevMenuSel);
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            {
                AECHAR *pwsz = NULL;
                uint32 dwSize;
                int nLen;
                AECHAR wstrFmt[20] = {0};
                uint16 nNews=0, nMsgs=0;
                
                dwSize = 64 * sizeof(AECHAR);
                pwsz = (AECHAR *)MALLOC(dwSize);
                
                if (NULL == pwsz)
                {
                    return FALSE;
                }
                
                // 格式化预约短信菜单项文本
                (void)STRTOWSTR("(%d)",wstrFmt,sizeof(wstrFmt));
                (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                            IDS_RESERVEDMSGS,
                            pwsz, dwSize);
                            
                wms_cacheinfolist_getcounts(WMS_MB_RESERVE, &nNews, NULL, &nMsgs);
                nLen = WSTRLEN(pwsz);
                if (nLen<30)
                {
                    WSPRINTF(&pwsz[nLen], (64-nLen)*sizeof(AECHAR), wstrFmt, nNews); 
                }
                IMENUCTL_SetItemText(pMenu, IDS_RESERVEDMSGS, NULL, NULL, pwsz);
                
                // 格式化失败预约短信菜单项文本
                (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                            IDS_FAILEDRSVDMSGS,
                            pwsz, dwSize);
                nLen = WSTRLEN(pwsz);
                if (nLen<30)
                {
                    WSPRINTF(&pwsz[nLen], (64-nLen)*sizeof(AECHAR), wstrFmt, (nMsgs-nNews)); 
                }
                IMENUCTL_SetItemText(pMenu, IDS_FAILEDRSVDMSGS, NULL, NULL, pwsz);

                FREE(pwsz);
            }

            return TRUE;

        case EVT_USER_REDRAW:
            // 更新界面
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            //(void)IMENUCTL_Redraw(pMenu);   //dele by yangdecai
            return TRUE;

        case EVT_CTL_SEL_CHANGED:
            pMe->m_wPrevMenuSel = wParam;
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            pMe->m_wPrevMenuSel = wParam;
            switch (wParam)
            {
                // 预约信息
                case IDS_NEWRESERVEDMSG:
                    CLOSE_DIALOG(DLGRET_NEWRESERVEDMSG)
                    return TRUE;

                // 预约信息列表
                case IDS_RESERVEDMSGS:
                    CLOSE_DIALOG(DLGRET_RESERVEDMSGLST)
                    return TRUE;
                
                // 失败预约信息
                case IDS_FAILEDRSVDMSGS:
                    CLOSE_DIALOG(DLGRET_FAILEDRSVDMSGS)
                    return TRUE;
                
                default:
                    break;
            }
            
            return FALSE;

        default:
            break;
    }

    return FALSE;
} // IDD_RESERVEDMSG_Handler

/*==============================================================================
函数:
    GetMonthLength

说明:
    函数用于计算特定年特定月的天数。

参数:
    nMonth [in]: 特定月。
    nYear [in]: 特定年。

返回值:
    特定年特定月的天数。

备注:

==============================================================================*/
static int GetMonthLength(int nMonth, int nYear)
{
    // handle february special case
    if (nMonth == 2)
    {
        return (((((nYear)%4)==0) && (((nYear)%100) || (((nYear)%400)==0))) ? 29 : 28);
    }

    // If we shift the year to start on Mar1, then
    // there is a repeating 5-month pattern of
    // {31,30,31,30,31} {31,30,31,30,31} {31,<sp>
    
    nMonth = (nMonth > 2) ? nMonth-3 : nMonth+9;
    
    return ((nMonth % 5) & 1) ? 30 : 31;
}

/*==============================================================================
函数:
    IDD_GETDT_Handler

说明:
    WMS Applet对话框 IDD_GETDT 事件处理函数。显示关于短消息设置界面给用户作选择。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_GETDT_Handler(void   *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    WmsApp *pMe = (WmsApp *)pUser;
    
    if (NULL == pMe)
    {
        return FALSE;
    }
        
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
#ifdef FEATURE_TIME_DATA_SETTING
            (void) ICONFIG_GetItem(pMe->m_pConfig,
                                   CFGI_DATE_FORMAT,
                                   &(pMe->m_rsvDateTime.btDateFmt),
                                   sizeof(pMe->m_rsvDateTime.btDateFmt));
            switch (pMe->m_rsvDateTime.btDateFmt)
            {
                case OEMNV_DATEFORM_MDY:
                    pMe->m_rsvDateTime.eSel[0] = DT_SEL_MONTH;
                    pMe->m_rsvDateTime.eSel[1] = DT_SEL_DAY;
                    pMe->m_rsvDateTime.eSel[2] = DT_SEL_YEAR;
                    break;
                    
                case OEMNV_DATEFORM_DMY:
                    pMe->m_rsvDateTime.eSel[0] = DT_SEL_DAY;
                    pMe->m_rsvDateTime.eSel[1] = DT_SEL_MONTH;
                    pMe->m_rsvDateTime.eSel[2] = DT_SEL_YEAR;
                    break;
                    
                case OEMNV_DATEFORM_YMD:
                default:
                    pMe->m_rsvDateTime.btDateFmt = OEMNV_DATEFORM_YMD;
                    pMe->m_rsvDateTime.eSel[0] = DT_SEL_YEAR;
                    pMe->m_rsvDateTime.eSel[1] = DT_SEL_MONTH;
                    pMe->m_rsvDateTime.eSel[2] = DT_SEL_DAY;
                    break;
            }
#else
            pMe->m_rsvDateTime.eSel[] = {DT_SEL_YEAR, DT_SEL_MONTH, DT_SEL_DAY, DT_SEL_NONE};
#endif

            (void) ICONFIG_GetItem(pMe->m_pConfig,
                                   CFGI_TIME_FORMAT,
                                   &(pMe->m_rsvDateTime.btTimeFmt),
                                   sizeof(pMe->m_rsvDateTime.btTimeFmt));
            
            // 获取日期间
            if (pMe->m_rsvDateTime.dwSecs == 0)
            {
            	uint32  dwSecs;

				/*
				byte	btTimeStamp = 0;
			    
				(void) ICONFIG_GetItem(pMe->m_pConfig,
			                           CFGI_SMS_TIMESTAMP,
			                           &btTimeStamp,
			                           sizeof(btTimeStamp));

				if (btTimeStamp == OEMNV_SMS_TIMESTAMP_ADJUST)
				{
					dwSecs = GETUTCSECONDS();
					MSG_FATAL("***zzg GETUTCSECONDS 4 dwSecs=%d***", dwSecs, 0, 0);
				}
				else
				{
					dwSecs = GETTIMESECONDS();
					MSG_FATAL("***zzg GETTIMESECONDS 4 dwSecs=%d***", dwSecs, 0, 0);
				}   
				*/

				dwSecs = GETTIMESECONDS();
				
                pMe->m_rsvDateTime.dwSecs = dwSecs;	
				
            }
            {
                JulianType jtdate;
                AECHAR  wstrFMT[10]={0};
                
                GETJULIANDATE(pMe->m_rsvDateTime.dwSecs, &jtdate);

                pMe->m_rsvDateTime.bAm = (jtdate.wHour < 12) ? TRUE : FALSE;
                
                (void)STRTOWSTR("%d", wstrFMT, sizeof(wstrFMT));
                WSPRINTF(pMe->m_rsvDateTime.wstrYear, sizeof(pMe->m_rsvDateTime.wstrYear), wstrFMT, jtdate.wYear);
                WSPRINTF(pMe->m_rsvDateTime.wstrMonth, sizeof(pMe->m_rsvDateTime.wstrMonth), wstrFMT, jtdate.wMonth);
                WSPRINTF(pMe->m_rsvDateTime.wstrDay, sizeof(pMe->m_rsvDateTime.wstrDay), wstrFMT, jtdate.wDay);
                
                if (pMe->m_rsvDateTime.btTimeFmt == OEMNV_TIMEFORM_AMPM)
                {
                    if (jtdate.wHour == 0)
                    {
                        jtdate.wHour = 12;
                    }
                    else if (jtdate.wHour > 12)
                    {
                        jtdate.wHour -= 12;
                    }
                }
                WSPRINTF(pMe->m_rsvDateTime.wstrHour, sizeof(pMe->m_rsvDateTime.wstrHour), wstrFMT, jtdate.wHour);
                
                (void)STRTOWSTR("%02d", wstrFMT, sizeof(wstrFMT));
                WSPRINTF(pMe->m_rsvDateTime.wstrMin, sizeof(pMe->m_rsvDateTime.wstrMin), wstrFMT, jtdate.wMinute);
            }
            
            pMe->m_rsvDateTime.eSel[5] = ((pMe->m_rsvDateTime.btTimeFmt == OEMNV_TIMEFORM_AMPM) ? DT_SEL_TIMEFMT : DT_SEL_NONE);
            pMe->m_rsvDateTime.eSel[3] = DT_SEL_HOUR;
            pMe->m_rsvDateTime.eSel[4] = DT_SEL_MIN;
            pMe->m_rsvDateTime.nSelIdx = 3;
            if (pMe->m_rsvDateTime.eSel[5] == DT_SEL_TIMEFMT)
            {
                pMe->m_rsvDateTime.ePreSel = 5;
            }
            else
            {
                pMe->m_rsvDateTime.ePreSel = 2;
            }
            pMe->m_rsvDateTime.bFirst = TRUE;

            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
                                    
            return TRUE;

        case EVT_USER_REDRAW:
        case EVT_UPDATE:
            {
                TitleBar_Param_type  TitleBar_Param = {0};
                AECHAR  wstrTitle[32]={0};
                AEERect rcY, rcMo, rcD, rcH, rcMi, rc, rcDiv1, rcDiv2;
                int nLineHeight, x, y, nLen;
                RGBVAL  clrFrame = FRAME_COLOR; 
                RGBVAL  clrFill = RGB_WHITE;
                RGBVAL  clrNosel = BK_COLOR;
                uint32  wflg = IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE;
                
                //获得系统字体高度
                nLineHeight = IDISPLAY_GetFontMetrics(pMe->m_pDisplay,
                                AEE_FONT_NORMAL,
                                NULL,
                                NULL) + 6;
                                
                if (EVT_USER_REDRAW == eCode)
                {
                    TitleBar_Param.pwszTitle = wstrTitle;
                    TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER;
                    (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                                IDS_RESERVEDMSG, wstrTitle, sizeof(wstrTitle));
                    #if 0
                    // 画标题条
                    DrawTitleBar(pMe->m_pDisplay, &TitleBar_Param);
					#else
					IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,wstrTitle);
					#endif
                    
                    // 填充大背景色
                    MEMCPY(&rc, &pMe->m_rc, sizeof(rc));
                    rc.y = GetTitleBarHeight(pMe->m_pDisplay);
                    rc.dy -= (GetTitleBarHeight(pMe->m_pDisplay) + GetBottomBarHeight(pMe->m_pDisplay));
                    IDISPLAY_FillRect(pMe->m_pDisplay, &rc, BK_COLOR);
                    
                    // 绘制底条提示
                    // 确定-----返回
                    DRAW_BOTTOMBAR(BTBAR_OK_BACK)
                }
                else if (wParam)
                {// 要求对日期进行检查
                    int         nMaxDays;
                    JulianType  jtdate;
                    uint32      nRet = 0;
                    char        strTep[5] = {0};

					uint32  dwSecs;
					
					/*
					byte	btTimeStamp = 0;
				    
					(void) ICONFIG_GetItem(pMe->m_pConfig,
				                           CFGI_SMS_TIMESTAMP,
				                           &btTimeStamp,
				                           sizeof(btTimeStamp));

					if (btTimeStamp == OEMNV_SMS_TIMESTAMP_ADJUST)
					{
						dwSecs = GETUTCSECONDS();
						MSG_FATAL("***zzg GETUTCSECONDS 5 dwSecs=%d***", dwSecs, 0, 0);
					}
					else
					{
						dwSecs = GETTIMESECONDS();
						MSG_FATAL("***zzg GETTIMESECONDS 5 dwSecs=%d***", dwSecs, 0, 0);
					}   
					*/
                    
                    GETJULIANDATE(GETTIMESECONDS(), &jtdate);
                    //GETJULIANDATE(dwSecs, &jtdate);
                    
                    (void)WSTRTOSTR(pMe->m_rsvDateTime.wstrYear, strTep, sizeof(strTep));
                    nRet = STRTOUL(strTep, NULL, 10);
                    if ((nRet<jtdate.wYear) || (nRet>2099))
                    {
                        MEMSET(pMe->m_rsvDateTime.wstrYear, 0, sizeof(pMe->m_rsvDateTime.wstrYear));
                        SPRINTF(strTep, "%d", jtdate.wYear);
                        (void)STRTOWSTR(strTep, pMe->m_rsvDateTime.wstrYear, sizeof(pMe->m_rsvDateTime.wstrYear));
                    }
                    else
                    {
                        jtdate.wYear = nRet;
                    }
                    
                    (void)WSTRTOSTR(pMe->m_rsvDateTime.wstrMonth, strTep, sizeof(strTep));
                    nRet = STRTOUL(strTep, NULL, 10);
                    nMaxDays = GetMonthLength(nRet, jtdate.wYear);
                    (void)WSTRTOSTR(pMe->m_rsvDateTime.wstrDay, strTep, sizeof(strTep));
                    nRet = STRTOUL(strTep, NULL, 10);
                    
                    if (nRet>nMaxDays)
                    {
                        MEMSET(pMe->m_rsvDateTime.wstrDay, 0, sizeof(pMe->m_rsvDateTime.wstrDay));
                        SPRINTF(strTep, "%d", nMaxDays);
                        (void)STRTOWSTR(strTep, pMe->m_rsvDateTime.wstrDay, sizeof(pMe->m_rsvDateTime.wstrDay));
                    }
                }
                
                // 绘制日期字符串
                //y = nLineHeight;
                y = GetTitleBarHeight(pMe->m_pDisplay);
                
                if (EVT_USER_REDRAW == eCode)
                {// 绘制日期提示字符串
                    (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                                IDS_DATE, wstrTitle, sizeof(wstrTitle));
                    nLen = WSTRLEN(wstrTitle);
                    wstrTitle[nLen] = (AECHAR)':';
                    wstrTitle[nLen+1] = 0;
                    SETAEERECT(&rc,2, y, 38, nLineHeight);
                    IDISPLAY_DrawText( pMe->m_pDisplay, AEE_FONT_BOLD, wstrTitle, -1, 1, y, &rc, wflg);
                }
                            
                x = pMe->m_rc.x + (pMe->m_rc.dx - 38 - 20 - 20 - 10 -10) / 2;
                y += nLineHeight;
#ifdef FEATURE_TIME_DATA_SETTING
                switch (pMe->m_rsvDateTime.btDateFmt)
                {
                    case OEMNV_DATEFORM_MDY:
                        SETAEERECT(&rcMo, x, y, 20, nLineHeight);
                        SETAEERECT(&rcD,(x+30), y, 20, nLineHeight);
                        SETAEERECT(&rcY,(x+60), y, 38, nLineHeight);
                        SETAEERECT(&rcDiv1,(x+20), y, 10, nLineHeight);
                        SETAEERECT(&rcDiv2,(x+50), y, 10, nLineHeight);
                        break;
                        
                    case OEMNV_DATEFORM_DMY:
                        SETAEERECT(&rcD, x, y, 20, nLineHeight);
                        SETAEERECT(&rcMo,(x+30), y, 20, nLineHeight);
                        SETAEERECT(&rcY,(x+60), y, 38, nLineHeight);
                        SETAEERECT(&rcDiv1,(x+20), y, 10, nLineHeight);
                        SETAEERECT(&rcDiv2,(x+50), y, 10, nLineHeight);
                        break;
                        
                    case OEMNV_DATEFORM_YMD:
                    default:
                        SETAEERECT(&rcY, x, y, 38, nLineHeight);
                        SETAEERECT(&rcMo,(x+48), y, 20, nLineHeight);
                        SETAEERECT(&rcD,(x+78), y, 20, nLineHeight);
                        SETAEERECT(&rcDiv1,(x+38), y, 10, nLineHeight);
                        SETAEERECT(&rcDiv2,(x+68), y, 10, nLineHeight);
                        break;
                }
#else
                SETAEERECT(&rcY, x, y, 38, nLineHeight);
                SETAEERECT(&rcMo,(x+48), y, 20, nLineHeight);
                SETAEERECT(&rcD,(x+78), y, 20, nLineHeight);
                SETAEERECT(&rcDiv1,(x+38), y, 10, nLineHeight);
                SETAEERECT(&rcDiv2,(x+68), y, 10, nLineHeight);
#endif 
                if (pMe->m_rsvDateTime.eSel[pMe->m_rsvDateTime.nSelIdx] == DT_SEL_YEAR)
                {
                    clrFrame = FRAME_COLOR; 
                    clrFill =  RGB_WHITE;
                }
                else
                {
                    clrFrame = clrNosel; 
                    clrFill =  clrNosel;
                }
                IDISPLAY_DrawRect(pMe->m_pDisplay, &rcY, clrFrame, clrFill, IDF_RECT_FRAME | IDF_RECT_FILL);
                IDISPLAY_DrawText( pMe->m_pDisplay, AEE_FONT_BOLD, pMe->m_rsvDateTime.wstrYear, -1, 1, y, &rcY, wflg);
                                
                if (pMe->m_rsvDateTime.eSel[pMe->m_rsvDateTime.nSelIdx] == DT_SEL_MONTH)
                {
                    clrFrame = FRAME_COLOR; 
                    clrFill =  RGB_WHITE;
                }
                else
                {
                    clrFrame = clrNosel; 
                    clrFill =  clrNosel;
                }
                IDISPLAY_DrawRect(pMe->m_pDisplay, &rcMo, clrFrame, clrFill, IDF_RECT_FRAME | IDF_RECT_FILL);
                IDISPLAY_DrawText( pMe->m_pDisplay, AEE_FONT_BOLD, pMe->m_rsvDateTime.wstrMonth, -1, 1, y, &rcMo, wflg);
                                
                if (pMe->m_rsvDateTime.eSel[pMe->m_rsvDateTime.nSelIdx] == DT_SEL_DAY)
                {
                    clrFrame = FRAME_COLOR; 
                    clrFill =  RGB_WHITE;
                }
                else
                {
                    clrFrame = clrNosel; 
                    clrFill =  clrNosel;
                }
                
                IDISPLAY_DrawRect(pMe->m_pDisplay, &rcD, clrFrame, clrFill, IDF_RECT_FRAME | IDF_RECT_FILL);
                IDISPLAY_DrawText( pMe->m_pDisplay, AEE_FONT_BOLD, pMe->m_rsvDateTime.wstrDay, -1, 1, y, &rcD, wflg);
                            
                if (EVT_USER_REDRAW == eCode)
                {
                    (void)STRTOWSTR("/", wstrTitle, sizeof(wstrTitle));
                    IDISPLAY_DrawText( pMe->m_pDisplay, AEE_FONT_BOLD, wstrTitle, -1, 1, y, &rcDiv1, wflg);
                    IDISPLAY_DrawText( pMe->m_pDisplay, AEE_FONT_BOLD, wstrTitle, -1, 1, y, &rcDiv2, wflg);
                }
                
                // 绘制时间提示及上下午选择字符串
                y += nLineHeight;
                if (EVT_USER_REDRAW == eCode)
                {
                    (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                                IDS_TIME, wstrTitle, sizeof(wstrTitle));
                    SETAEERECT(&rc,2, y, 38, nLineHeight);
                    
                    nLen = WSTRLEN(wstrTitle);
                    wstrTitle[nLen] = (AECHAR)':';
                    wstrTitle[nLen+1] = 0;
                    IDISPLAY_DrawText( pMe->m_pDisplay, AEE_FONT_BOLD, wstrTitle, -1, 1, y, &rc, wflg);
                }
                if (pMe->m_rsvDateTime.btTimeFmt == OEMNV_TIMEFORM_AMPM)
                {
                    if (pMe->m_rsvDateTime.eSel[pMe->m_rsvDateTime.nSelIdx] == DT_SEL_TIMEFMT)
                    {
                        clrFrame = FRAME_COLOR; 
                        clrFill =  RGB_WHITE;
                    }
                    else
                    {
                        clrFrame = clrNosel; 
                        clrFill =  clrNosel;
                    }
                    
                    // 绘制左右箭头
                    {
                        IImage	*pLeftImg = NULL;
                        IImage	*pRightImg = NULL;
                        AEEImageInfo imginfo={0};
                        
                        SETAEERECT(&rc, 40, y, 30, nLineHeight);
                        
                        pLeftImg = ISHELL_LoadResImage(pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDB_LEFTARROW);
                        pRightImg = ISHELL_LoadResImage(pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDB_RIGHTARROW);
                        
                        if (pLeftImg != NULL)
                        {
                            IIMAGE_GetInfo(pLeftImg, &imginfo);
                            IIMAGE_Draw(pLeftImg, rc.x, y+(nLineHeight-imginfo.cy)/2);
                            rc.x += imginfo.cx;
                            IIMAGE_Release(pLeftImg);
                        }
                        
                        if (pRightImg != NULL)
                        {
                            IIMAGE_GetInfo(pRightImg, &imginfo);
                            IIMAGE_Draw(pRightImg, rc.x+rc.dx, y+(nLineHeight-imginfo.cy)/2);
                            IIMAGE_Release(pRightImg);
                        }
                    }

                    IDISPLAY_DrawRect(pMe->m_pDisplay, &rc, clrFrame, clrFill, IDF_RECT_FRAME | IDF_RECT_FILL);
                    if (pMe->m_rsvDateTime.bAm)
                    {
                        (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                                    IDS_AM, wstrTitle, sizeof(wstrTitle));
                    }
                    else
                    {
                        (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                                    IDS_PM, wstrTitle, sizeof(wstrTitle));
                    }
                    
                    IDISPLAY_DrawText( pMe->m_pDisplay, AEE_FONT_BOLD, wstrTitle, -1, 1, y, &rc, wflg);
                }
                
                // 绘制时间字符串
                y += nLineHeight;
                SETAEERECT(&rcH,40, y, 20, nLineHeight);
                SETAEERECT(&rcMi,70, y, 20, nLineHeight);
                if (pMe->m_rsvDateTime.eSel[pMe->m_rsvDateTime.nSelIdx] == DT_SEL_HOUR)
                {
                    clrFrame = FRAME_COLOR; 
                    clrFill =  RGB_WHITE;
                }
                else
                {
                    clrFrame = clrNosel; 
                    clrFill =  clrNosel;
                }
                IDISPLAY_DrawRect(pMe->m_pDisplay, &rcH, clrFrame, clrFill, IDF_RECT_FRAME | IDF_RECT_FILL);
                IDISPLAY_DrawText( pMe->m_pDisplay, AEE_FONT_BOLD, pMe->m_rsvDateTime.wstrHour, -1, 1, y, &rcH, wflg);
                                
                if (pMe->m_rsvDateTime.eSel[pMe->m_rsvDateTime.nSelIdx] == DT_SEL_MIN)
                {
                    clrFrame = FRAME_COLOR; 
                    clrFill =  RGB_WHITE;
                }
                else
                {
                    clrFrame = clrNosel; 
                    clrFill =  clrNosel;
                }
                IDISPLAY_DrawRect(pMe->m_pDisplay, &rcMi, clrFrame, clrFill, IDF_RECT_FRAME | IDF_RECT_FILL);
                IDISPLAY_DrawText( pMe->m_pDisplay, AEE_FONT_BOLD, pMe->m_rsvDateTime.wstrMin, -1, 1, y, &rcMi, wflg);
                                
                if (EVT_USER_REDRAW == eCode)
                {
                    (void)STRTOWSTR(":", wstrTitle, sizeof(wstrTitle));
                    SETAEERECT(&rc,60, y, 10, nLineHeight);
                    IDISPLAY_DrawText( pMe->m_pDisplay, AEE_FONT_BOLD, wstrTitle, -1, 1, y, &rc, wflg);
                }
            }
            
            // 更新界面
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            if ((wParam == AVK_LEFT || wParam == AVK_RIGHT || wParam == AVK_UP || wParam == AVK_DOWN) &&
                (pMe->m_rsvDateTime.eSel[pMe->m_rsvDateTime.nSelIdx] == DT_SEL_MIN) &&
                (WSTRLEN(pMe->m_rsvDateTime.wstrMin) == 1))
            {
                pMe->m_rsvDateTime.wstrMin[1] = pMe->m_rsvDateTime.wstrMin[0];
                pMe->m_rsvDateTime.wstrMin[0] = (AECHAR)'0';
                pMe->m_rsvDateTime.wstrMin[2] = 0;
            }
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                    
                case AVK_INFO:
                case AVK_SELECT:
                    if (WSTRLEN(pMe->m_rsvDateTime.wstrYear) < 4)
                    {
                        CLOSE_DIALOG(DLGRET_YEARERR)
                    }
                    else
                    {
                        JulianType  jtdate = {0};
                        char        strTep[5] = {0};
                        
                        (void)WSTRTOSTR(pMe->m_rsvDateTime.wstrYear, strTep, sizeof(strTep));
                        jtdate.wYear = STRTOUL(strTep, NULL, 10);
                        
                        (void)WSTRTOSTR(pMe->m_rsvDateTime.wstrMonth, strTep, sizeof(strTep));
                        jtdate.wMonth = STRTOUL(strTep, NULL, 10);
                        
                        (void)WSTRTOSTR(pMe->m_rsvDateTime.wstrDay, strTep, sizeof(strTep));
                        jtdate.wDay = STRTOUL(strTep, NULL, 10);
                        
                        (void)WSTRTOSTR(pMe->m_rsvDateTime.wstrHour, strTep, sizeof(strTep));
                        jtdate.wHour = STRTOUL(strTep, NULL, 10);
                        if ((pMe->m_rsvDateTime.btTimeFmt == OEMNV_TIMEFORM_AMPM) &&
                            (!pMe->m_rsvDateTime.bAm) &&
                            (jtdate.wHour != 12))
                        {
                            jtdate.wHour += 12;
                        }
                        
                        (void)WSTRTOSTR(pMe->m_rsvDateTime.wstrMin, strTep, sizeof(strTep));
                        jtdate.wMinute = STRTOUL(strTep, NULL, 10);
                        
                        pMe->m_rsvDateTime.dwSecs = JULIANTOSECONDS(&jtdate);

						{
							uint32  dwSecs;
							
							/*
							byte	btTimeStamp = 0;
						    
							(void) ICONFIG_GetItem(pMe->m_pConfig,
						                           CFGI_SMS_TIMESTAMP,
						                           &btTimeStamp,
						                           sizeof(btTimeStamp));

							if (btTimeStamp == OEMNV_SMS_TIMESTAMP_ADJUST)
							{
								dwSecs = GETUTCSECONDS();
								MSG_FATAL("***zzg GETUTCSECONDS 6 dwSecs=%d***", dwSecs, 0, 0);
							}
							else
							{
								dwSecs = GETTIMESECONDS();
								MSG_FATAL("***zzg GETTIMESECONDS 6 dwSecs=%d***", dwSecs, 0, 0);
							}   
							*/
						
						
	                        if (pMe->m_rsvDateTime.dwSecs < GETTIMESECONDS() + 5)
	                        //if (pMe->m_rsvDateTime.dwSecs < dwSecs + 5)
	                        {
	                            CLOSE_DIALOG(DLGRET_DATEERR)
	                        }
	                        else
	                        {
	                            CLOSE_DIALOG(DLGRET_OK)
	                        }
						}
                    }
                    return TRUE;
  
                case AVK_LEFT:
                    pMe->m_rsvDateTime.bFirst = TRUE;
                    pMe->m_rsvDateTime.ePreSel = pMe->m_rsvDateTime.eSel[pMe->m_rsvDateTime.nSelIdx];
                    
                    if (pMe->m_rsvDateTime.nSelIdx == 0)
                    {
                        pMe->m_rsvDateTime.nSelIdx = 2;
                    }
                    else if (pMe->m_rsvDateTime.nSelIdx == 3)
                    {
                        pMe->m_rsvDateTime.nSelIdx = 4;
                    }
                    else if (pMe->m_rsvDateTime.nSelIdx == 5)
                    {
                        pMe->m_rsvDateTime.ePreSel = 4;
                        
                        if (pMe->m_rsvDateTime.bAm)
                        {// 从上午到下午时, 0时换成12时
                            if ((pMe->m_rsvDateTime.wstrHour[0] == (AECHAR)'0') &&
                                (pMe->m_rsvDateTime.wstrHour[1] == 0))
                            {
                                (void)STRTOWSTR("12", pMe->m_rsvDateTime.wstrHour, sizeof(pMe->m_rsvDateTime.wstrHour));
                            }
                        }
                        else
                        {// 从下午到上午时, 12时换成0时
                            if ((pMe->m_rsvDateTime.wstrHour[0] == (AECHAR)'1') &&
                                (pMe->m_rsvDateTime.wstrHour[1] == (AECHAR)'2'))
                            {
                                (void)STRTOWSTR("0", pMe->m_rsvDateTime.wstrHour, sizeof(pMe->m_rsvDateTime.wstrHour));
                            }
                        }
                        
                        pMe->m_rsvDateTime.bAm = !pMe->m_rsvDateTime.bAm;
                    }
                    else
                    {
                        pMe->m_rsvDateTime.nSelIdx--;
                    }
                    (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                            EVTFLG_ASYNC,
                                            AEECLSID_WMSAPP,
                                            EVT_UPDATE,
                                            TRUE, 
                                            0);
                    return TRUE;
                    
                case AVK_RIGHT:
                    pMe->m_rsvDateTime.bFirst = TRUE;
                    pMe->m_rsvDateTime.ePreSel = pMe->m_rsvDateTime.eSel[pMe->m_rsvDateTime.nSelIdx];
                    
                    if (pMe->m_rsvDateTime.nSelIdx == 2)
                    {
                        pMe->m_rsvDateTime.nSelIdx = 0;
                    }
                    else if (pMe->m_rsvDateTime.nSelIdx == 4)
                    {
                        pMe->m_rsvDateTime.nSelIdx = 3;
                    }
                    else if (pMe->m_rsvDateTime.nSelIdx == 5)
                    {
                        pMe->m_rsvDateTime.ePreSel = 4;
                        if (pMe->m_rsvDateTime.bAm)
                        {// 从上午到下午时, 0时换成12时
                            if ((pMe->m_rsvDateTime.wstrHour[0] == (AECHAR)'0') &&
                                (pMe->m_rsvDateTime.wstrHour[1] == 0))
                            {
                                (void)STRTOWSTR("12", pMe->m_rsvDateTime.wstrHour, sizeof(pMe->m_rsvDateTime.wstrHour));
                            }
                        }
                        else
                        {// 从下午到上午时, 12时换成0时
                            if ((pMe->m_rsvDateTime.wstrHour[0] == (AECHAR)'1') &&
                                (pMe->m_rsvDateTime.wstrHour[1] == (AECHAR)'2'))
                            {
                                (void)STRTOWSTR("0", pMe->m_rsvDateTime.wstrHour, sizeof(pMe->m_rsvDateTime.wstrHour));
                            }
                        }
                        
                        pMe->m_rsvDateTime.bAm = !pMe->m_rsvDateTime.bAm;
                    }
                    else
                    {
                        pMe->m_rsvDateTime.nSelIdx++;
                    }
                    
                    (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                            EVTFLG_ASYNC,
                                            AEECLSID_WMSAPP,
                                            EVT_UPDATE,
                                            TRUE, 
                                            0);
                    return TRUE;
                    
                case AVK_UP:
                    pMe->m_rsvDateTime.bFirst = TRUE;
                    pMe->m_rsvDateTime.ePreSel = pMe->m_rsvDateTime.eSel[pMe->m_rsvDateTime.nSelIdx];

                    if ((pMe->m_rsvDateTime.nSelIdx >= 0) &&
                        (pMe->m_rsvDateTime.nSelIdx <= 2))
                    {
                        pMe->m_rsvDateTime.nSelIdx = 3;
                    }
                    else if (pMe->m_rsvDateTime.nSelIdx == 5)
                    {
                        pMe->m_rsvDateTime.nSelIdx = 0;
                    }
                    else
                    {
                        pMe->m_rsvDateTime.nSelIdx = ((pMe->m_rsvDateTime.eSel[5] == DT_SEL_TIMEFMT) ? 5 : 0);
                    }
                    (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                            EVTFLG_ASYNC,
                                            AEECLSID_WMSAPP,
                                            EVT_UPDATE,
                                            TRUE, 
                                            0);
                    return TRUE;
                    
                case AVK_DOWN:
                    pMe->m_rsvDateTime.bFirst = TRUE;
                    pMe->m_rsvDateTime.ePreSel = pMe->m_rsvDateTime.eSel[pMe->m_rsvDateTime.nSelIdx];
                    
                    if ((pMe->m_rsvDateTime.nSelIdx >= 0) &&
                        (pMe->m_rsvDateTime.nSelIdx <= 2))
                    {
                        pMe->m_rsvDateTime.nSelIdx = ((pMe->m_rsvDateTime.eSel[5] == DT_SEL_TIMEFMT) ? 5 : 3);
                    }
                    else if (pMe->m_rsvDateTime.nSelIdx == 5)
                    {
                        pMe->m_rsvDateTime.nSelIdx = 3;
                    }
                    else
                    {
                        pMe->m_rsvDateTime.nSelIdx = 0;
                    }
                    
                    (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                            EVTFLG_ASYNC,
                                            AEECLSID_WMSAPP,
                                            EVT_UPDATE,
                                            TRUE, 
                                            0);
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
                    {
                        boolean bChkDate = FALSE;
                        
                        switch (pMe->m_rsvDateTime.eSel[pMe->m_rsvDateTime.nSelIdx])
                        {
                            // 当前输入---年
                            case DT_SEL_YEAR:
                                
                                {
                                    int nLen = WSTRLEN(pMe->m_rsvDateTime.wstrYear);
                                    
                                    if (nLen >= 4)
                                    {
                                        MEMSET(pMe->m_rsvDateTime.wstrYear, 0, sizeof(pMe->m_rsvDateTime.wstrYear));
                                        nLen = 0;
                                    }
                                    pMe->m_rsvDateTime.wstrYear[nLen] = (AECHAR)('0'+wParam-AVK_0);
                                    nLen++;
                                    pMe->m_rsvDateTime.wstrYear[nLen] = 0;
                                    if (nLen == 4)
                                    {
                                        bChkDate = TRUE;
                                    }
                                }
                                break;
                                
                            // 当前输入---月
                            case DT_SEL_MONTH:
                                {
                                    int nLen = WSTRLEN(pMe->m_rsvDateTime.wstrMonth);
                                    
                                    if (nLen != 1)
                                    {
                                        if (wParam == AVK_0)
                                        {
                                            return TRUE;
                                        }
                                        MEMSET(pMe->m_rsvDateTime.wstrMonth, 0, sizeof(pMe->m_rsvDateTime.wstrMonth));
                                        nLen = 0;
                                    }
                                    else
                                    {
                                        if ((pMe->m_rsvDateTime.wstrMonth[0] != (AECHAR)'1') ||
                                            (wParam>AVK_2))
                                        {
                                            nLen = 0;
                                        }
                                    }
                                    pMe->m_rsvDateTime.wstrMonth[nLen] = (AECHAR)('0'+wParam-AVK_0);
                                    pMe->m_rsvDateTime.wstrMonth[nLen+1] = 0;
                                }
                                bChkDate = TRUE;
                                break;
                                
                            // 当前输入---日
                            case DT_SEL_DAY:
                                {
                                    int     nLen, wYear=0, wNonth=0, nMaxDays=0;
                                    char    strTep[5] = {0};
                                    
                                    (void)WSTRTOSTR(pMe->m_rsvDateTime.wstrYear, strTep, sizeof(strTep));
                                    wYear = STRTOUL(strTep, NULL, 10);
                                    (void)WSTRTOSTR(pMe->m_rsvDateTime.wstrMonth, strTep, sizeof(strTep));
                                    wNonth = STRTOUL(strTep, NULL, 10);
                                    nMaxDays = GetMonthLength(wNonth, wYear);
                                    
                                    nLen = WSTRLEN(pMe->m_rsvDateTime.wstrDay);
                                    if (nLen < 2)
                                    {
                                        pMe->m_rsvDateTime.wstrDay[nLen] = (AECHAR)('0'+wParam-AVK_0);
                                        pMe->m_rsvDateTime.wstrDay[nLen+1] = 0;
                                        
                                        (void)WSTRTOSTR(pMe->m_rsvDateTime.wstrDay, strTep, sizeof(strTep));
                                        nLen = STRTOUL(strTep, NULL, 10);
                                        if (nLen>nMaxDays)
                                        {
                                            pMe->m_rsvDateTime.wstrDay[0] = (AECHAR)('0'+wParam-AVK_0);
                                            pMe->m_rsvDateTime.wstrDay[1] = 0;
                                        }
                                    }
                                    else if (wParam != AVK_0)
                                    {
                                        pMe->m_rsvDateTime.wstrDay[0] = (AECHAR)('0'+wParam-AVK_0);
                                        pMe->m_rsvDateTime.wstrDay[1] = 0;
                                    }
                                }
                                break;
                                
                            // 当前输入---时
                            case DT_SEL_HOUR:
                                pMe->m_rsvDateTime.ePreSel = 5;
                                if ((pMe->m_rsvDateTime.bFirst) ||
                                    (pMe->m_rsvDateTime.wstrHour[1] != 0) ||
                                    ((pMe->m_rsvDateTime.wstrHour[0] != (AECHAR)'1') &&
                                     (pMe->m_rsvDateTime.wstrHour[0] != (AECHAR)'2')))
                                {
                                    pMe->m_rsvDateTime.bFirst = FALSE;
                                    pMe->m_rsvDateTime.wstrHour[0] = (AECHAR)('0'+wParam-AVK_0);
                                    
                                    if ((pMe->m_rsvDateTime.eSel[5] == DT_SEL_TIMEFMT) &&
                                        (pMe->m_rsvDateTime.wstrHour[0] == (AECHAR)'0'))
                                    {
                                        pMe->m_rsvDateTime.bAm = TRUE;
                                    }
                                    pMe->m_rsvDateTime.wstrHour[1] = 0;
                                }
                                else
                                {
                                    if (pMe->m_rsvDateTime.wstrHour[0] == (AECHAR)'1')
                                    {
                                        if (pMe->m_rsvDateTime.eSel[5] == DT_SEL_TIMEFMT)
                                        {
                                            if (wParam < AVK_2)
                                            {
                                                pMe->m_rsvDateTime.wstrHour[1] = (AECHAR)('0'+wParam-AVK_0);
                                            }
                                            else if (wParam == AVK_2)
                                            {
                                                pMe->m_rsvDateTime.wstrHour[1] = (AECHAR)('0'+wParam-AVK_0);
                                                pMe->m_rsvDateTime.bAm = FALSE;
                                            }
                                            else
                                            {
                                                pMe->m_rsvDateTime.wstrHour[0] = (AECHAR)('0'+wParam-AVK_0);
                                                pMe->m_rsvDateTime.wstrHour[1] = 0;
                                            }
                                        }
                                        else
                                        {
                                            pMe->m_rsvDateTime.wstrHour[1] = (AECHAR)('0'+wParam-AVK_0);
                                        }
                                    }
                                    else
                                    {
                                        if (pMe->m_rsvDateTime.eSel[5] == DT_SEL_NONE)
                                        {
                                            if (wParam < AVK_4)
                                            {
                                                pMe->m_rsvDateTime.wstrHour[1] = (AECHAR)('0'+wParam-AVK_0);
                                            }
                                            else if (wParam == AVK_4)
                                            {
                                                pMe->m_rsvDateTime.wstrHour[0] = (AECHAR)'0';
                                                pMe->m_rsvDateTime.wstrHour[1] = 0;
                                            }
                                            else
                                            {
                                                pMe->m_rsvDateTime.wstrHour[0] = (AECHAR)('0'+wParam-AVK_0);
                                                pMe->m_rsvDateTime.wstrHour[1] = 0;
                                            }
                                        }
                                        else
                                        {
                                            pMe->m_rsvDateTime.wstrHour[0] = (AECHAR)('0'+wParam-AVK_0);
                                            pMe->m_rsvDateTime.wstrHour[1] = 0;
                                            if (pMe->m_rsvDateTime.wstrHour[0] == (AECHAR)'0')
                                            {
                                                pMe->m_rsvDateTime.bAm = TRUE;
                                            }
                                        }
                                    }
                                }
                                break;
                                
                            // 当前输入---分
                            case DT_SEL_MIN:
                                {
                                    int     nLen;
                                    char    strTep[5] = {0};
                                    
                                    nLen = WSTRLEN(pMe->m_rsvDateTime.wstrMin);
                                    if (nLen < 2)
                                    {
                                        pMe->m_rsvDateTime.wstrMin[nLen] = (AECHAR)('0'+wParam-AVK_0);
                                        pMe->m_rsvDateTime.wstrMin[nLen+1] = 0;
                                        
                                        (void)WSTRTOSTR(pMe->m_rsvDateTime.wstrMin, strTep, sizeof(strTep));
                                        nLen = STRTOUL(strTep, NULL, 10);
                                        if (nLen>59)
                                        {
                                            pMe->m_rsvDateTime.wstrMin[0] = (AECHAR)('0'+wParam-AVK_0);
                                            pMe->m_rsvDateTime.wstrMin[1] = 0;
                                        }
                                    }
                                    else
                                    {
                                        pMe->m_rsvDateTime.wstrMin[0] = (AECHAR)('0'+wParam-AVK_0);
                                        pMe->m_rsvDateTime.wstrMin[1] = 0;
                                    }
                                }
                                break;
                                
                            default:
                                break;
                        }
                        
                        (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                                EVTFLG_ASYNC,
                                                AEECLSID_WMSAPP,
                                                EVT_UPDATE,
                                                bChkDate, 
                                                0);
                    }
                    break;
                    
                default:
                    break;
            }
            return TRUE;

        default:
            break;
    }

    return FALSE;
} // IDD_GETDT_Handler

/*==============================================================================
函数:
    IDD_RESERVEDMSGALERT_Handler

说明:
    IDD_RESERVEDMSGALERT 事件处理函数。 删除信息。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
//extern boolean is_g_bMp3PlayerRun();

static boolean IDD_RESERVEDMSGALERT_Handler(void        *pUser,
                                            AEEEvent    eCode,
                                            uint16      wParam,
                                            uint32      dwParam)
{
    WmsApp *pMe = (WmsApp *)pUser;

    if (NULL == pMe)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;
            
        case EVT_USER_REDRAW:
            {
                uint16  ring_id = 1;   //闹钟铃声ID
                int    y = 0, nLineHeight = 0;
                AECHAR wstrText[MSGBOX_MAXTEXTLEN]={0};
                AECHAR wstrTep[MSGBOX_MAXTEXTLEN]={0};
                AECHAR wstrFmt[10]={0};
                wms_cache_info_node  *pnode = NULL;
                
                
                //获得系统字体高度
                nLineHeight = IDISPLAY_GetFontMetrics(pMe->m_pDisplay,
                                AEE_FONT_NORMAL,
                                NULL,
                                NULL)+5;
				// 画标题条
                {
                    TitleBar_Param_type  TitleBar_Param = {0};
                    
                    TitleBar_Param.pwszTitle = wstrText;
                    TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER;
                    (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                                IDS_RESERVEDMSG, wstrText,sizeof(wstrText));
		            #if 0
		            // 画标题条
		            {
		                TitleBar_Param_type  TitleBar_Param = {0};
		                
		                TitleBar_Param.pwszTitle = wstrText;
		                TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER;
		                (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
		                            IDS_RESERVEDMSG, wstrText,sizeof(wstrText));
		                
		                DrawTitleBar(pMe->m_pDisplay, &TitleBar_Param);
		                MEMSET(wstrText, 0, sizeof(wstrText));
		            }
					#else
					IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,wstrText);
					#endif
                }
                
                // 绘制"发送 ?"提示字符
                y += (10+GetTitleBarHeight(pMe->m_pDisplay));
                (void)STRTOWSTR("%s ?", wstrFmt, sizeof(wstrFmt));
                (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                            IDS_SEND, wstrTep,sizeof(wstrTep));
                WSPRINTF(wstrText, sizeof(wstrText), wstrFmt, wstrTep);
                IDISPLAY_DrawText( pMe->m_pDisplay, 
                            AEE_FONT_BOLD, wstrText,
                            -1, 0, y, &pMe->m_rc, 
                            IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER);
                
                y = pMe->m_rc.dy - GetBottomBarHeight(pMe->m_pDisplay) - 2 * nLineHeight;
                // 绘制送至号码或人名及预约时间
                wms_cacheinfolist_enumbegin(WMS_MB_RESERVE);
                pnode = wms_cacheinfolist_enumnext(WMS_MB_RESERVE);
                if (NULL != pnode)
                {
                    if (NULL != pnode->pszNum)
                    {
                        AECHAR *pTitle = NULL;
                        
                        (void)STRTOWSTR(pnode->pszNum, wstrTep, sizeof(wstrTep));
                        wstrText[0] = 0;
                        WMSUtil_GetContactName(pMe, wstrTep, wstrText, MSGBOX_MAXTEXTLEN);
                        pTitle = ((WSTRLEN(wstrText) > 0) ? wstrText : wstrTep);
                            
                        IDISPLAY_DrawText( pMe->m_pDisplay, 
                                    AEE_FONT_BOLD, pTitle,
                                    -1, 0, y, &pMe->m_rc, 
                                    IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER);
                                    
                        wstrText[0] = 0;
                    }
                    
                    y += nLineHeight;
                    WMSUtil_SecsToDateString(pMe, pnode->dwTime, wstrText, sizeof(wstrText));
                    IDISPLAY_DrawText( pMe->m_pDisplay, 
                                AEE_FONT_BOLD, wstrText,
                                -1, 0, y, &pMe->m_rc, 
                                IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER);
                }
                
                // 绘制底条提示
                // 确定-----取消
                DRAW_BOTTOMBAR(BTBAR_OK_CANCEL)
                
                // 更新界面
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                
                // 设置定时器函数以关闭该对话框
                (void) ISHELL_SetTimer(pMe->m_pShell,
                        RESERVEDMSGALERT_TIMEOUT,
                        DialogTimeoutCallback, pMe);
                        
                // 放提示音
                ICONFIG_GetItem(pMe->m_pConfig,CFGI_ALARM_RINGER,&ring_id, sizeof(ring_id));
#ifdef FEATURE_APP_MUSICPLAYER
                if(GetMp3PlayerStatus() != MP3STATUS_NONE)
                {
                    (void)ISHELL_SendEvent(pMe->m_pShell,
                                           AEECLSID_APP_MUSICPLAYER,
                                           EVT_ALARM,
                                           TRUE,
                                           0);
                }
#endif
                IALERT_StartRingerAlert(pMe->m_pAlert, ring_id);
            }
            return TRUE;
            
        case EVT_DIALOG_END:
            (void) ISHELL_CancelTimer(pMe->m_pShell,DialogTimeoutCallback,pMe);
            IALERT_StopRingerAlert(pMe->m_pAlert);
#ifdef FEATURE_APP_MUSICPLAYER
                if(GetMp3PlayerStatus() != MP3STATUS_NONE)
                {
                    (void)ISHELL_SendEvent(pMe->m_pShell,
                                           AEECLSID_APP_MUSICPLAYER,
                                           EVT_ALARM,
                                           FALSE,
                                           0);
                }
#endif
            if (pMe->m_eDlgReturn == DLGRET_CREATE)
            {
                WmsApp_ReservedMsgSetTimer(pMe);
                if (pMe->m_bNaturalStart)
                {
                    MOVE_TO_STATE(WMSST_MAIN)
                }
                else
                {
                    MOVE_TO_STATE(WMSST_EXIT)
                }
            }
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    (void) ISHELL_CancelTimer(pMe->m_pShell,DialogTimeoutCallback,pMe);
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                    
                case AVK_INFO:
                case AVK_SELECT:
                    (void) ISHELL_CancelTimer(pMe->m_pShell,DialogTimeoutCallback,pMe);
                    CLOSE_DIALOG(DLGRET_OK)
                    return TRUE;
                    
                default:
                    break;
            }
            
            return TRUE;

        case EVT_DISPLAYDIALOGTIMEOUT:
            {
                boolean bVal = FALSE;
                
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_WMS_RESVDMSGALERT_TIMEIOUT,
                                       &bVal,
                                       sizeof(bVal));
                
                if (bVal)
                {
                    CLOSE_DIALOG(DLGRET_OK)
                }
                else
                {
                    CLOSE_DIALOG(DLGRET_CANCELED)
                }
            }
            
            return TRUE;

        default:
            break;
    }

    return FALSE;
} // IDD_RESERVEDMSGALERT_Handler()

/*==============================================================================
函数:
    IDD_OUTMSGSAVEOPT_Handler

说明:
    WMS Applet对话框 IDD_OUTMSGSAVEOPT 事件处理函数。
    
参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_OUTMSGSAVEOPT_Handler(void *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;

    if (NULL == pMe)
    {
        return FALSE;
    }

    pMenu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_OUTMSGSAVEOPT_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            //SetControlRect(pMe,pMenu);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,                                
                        IDS_AUTOSAVE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);

            {
                uint16 nSelID;
                
                if (pMe->m_cfg.m_bAutoSave)
                {
                    nSelID = IDL_AUTOSAVE;
                }
                else
                {
                    nSelID = IDL_NOTSAVE;
                }
            
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, nSelID, TRUE);
                IMENUCTL_SetSel(pMenu, nSelID);
            }
            return TRUE;

        case EVT_USER_REDRAW:
            // 绘制底条提示
            // Select       Back
            //DRAW_BOTTOMBAR(BTBAR_SELECT_BACK)
            
            // 更新界面
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            //(void)IMENUCTL_Redraw(pMenu);     //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            switch (wParam)
            {
                // 发出短信自动存储
                case IDL_AUTOSAVE:
                    pMe->m_cfg.m_bAutoSave = TRUE;
                    break;

                // 发出短信不予保存
                case IDL_NOTSAVE:
                    pMe->m_cfg.m_bAutoSave = FALSE;
                    break;
                
                default:
                    return FALSE;
            }
            
            CLOSE_DIALOG(DLGRET_OK)
            return TRUE;

        default:
            break;
    }

    return FALSE;
} // IDD_OUTMSGSAVEOPT_Handler

/*==============================================================================
函数:
    IDD_PRIORITY_Handler

说明:
    WMS Applet对话框 IDD_PRIORITY 事件处理函数。
    
参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_PRIORITY_Handler(void *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;

    if (NULL == pMe)
    {
        return FALSE;
    }

    pMenu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_PRIORITY_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            //SetControlRect(pMe,pMenu);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
#endif
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,                                
                        IDS_PRIORITY,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);
            
            // 菜单项初始化
            MENU_ADDITEM(pMenu, IDS_NORMAL);
            MENU_ADDITEM(pMenu, IDS_URGENT);
            //MENU_ADDITEM(pMenu, IDS_EMERGENCY);
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            {// 需根据具体配置选择相应菜单项
                uint16 nSelID = IDS_NORMAL;
                byte   btPriority;
                
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_WMS_PRIORITY,
                                       &btPriority,
                                       sizeof(btPriority));
                
                switch (btPriority)
                {
                    case WMS_PRIORITY_URGENT://紧急
                        nSelID = IDS_URGENT;
                        break;
                        
                    case WMS_PRIORITY_EMERGENCY://特急
                        nSelID = IDS_EMERGENCY;
                        break;
                        
                    default:
                        break;
                }
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, nSelID, TRUE);
                IMENUCTL_SetSel(pMenu, nSelID);
            }
            return TRUE;

        case EVT_USER_REDRAW:
            // 绘制底条提示
            // Select       Back
            //DRAW_BOTTOMBAR(BTBAR_SELECT_BACK)

            // 更新界面
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            //(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            {
                byte   btPriority;
                
                switch (wParam)
                {
                    // 发出短信优先级为普通
                    case IDS_NORMAL:
                        btPriority = WMS_PRIORITY_NORMAL;
                        break;

                    // 发出短信优先级为紧急
                    case IDS_URGENT:
                        btPriority = WMS_PRIORITY_URGENT;
                        break;
                    
                    // 发出短信优先级为特急
                    case IDS_EMERGENCY:
                        btPriority = WMS_PRIORITY_EMERGENCY;
                        break;
                    
                    default:
                        return FALSE;
                }

                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wParam, TRUE);
                (void) ICONFIG_SetItem(pMe->m_pConfig,
                                       CFGI_WMS_PRIORITY,
                                       &btPriority,
                                       sizeof(btPriority));
            }
            CLOSE_DIALOG(DLGRET_OK)
            return TRUE;

        default:
            break;
    }

    return FALSE;
} // IDD_PRIORITY_Handler


/*==============================================================================
函数:
    IDD_SENDMODE_Handler

说明:
    WMS Applet对话框 IDD_SENDMODE 事件处理函数。
    
参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_SENDMODE_Handler(void *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;

    if (NULL == pMe)
    {
        return FALSE;
    }

    pMenu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_SENDMODE_MENU);
      
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
#endif
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);
            
            // 菜单项初始化
            MENU_ADDITEM(pMenu, IDS_SENDANDSAVE);
            MENU_ADDITEM(pMenu, IDS_SENDONLY);
            MENU_ADDITEM(pMenu, IDS_SAVEONLY);
            {
                AECHAR WTitle[40] = {0};
                (void)ISHELL_LoadResString(pMe->m_pShell,
                AEE_WMSAPPRES_LANGFILE,                                
                IDS_SENDMODE,
                WTitle,
                sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
            }               
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            {// 需根据具体配置选择相应菜单项
                uint16 nSelID = IDS_SENDANDSAVE;
                byte   btSendmode;
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_WMS_SENDMODE,
                                       &btSendmode,
                                       sizeof(btSendmode));                
                switch (btSendmode)
                {
                    case SENDOPT_SAVEANDSEND://发送并保存
                        nSelID = IDS_SENDANDSAVE;
                        break;
                        
                    case SENDOPT_SEND://只发送
                        nSelID = IDS_SENDONLY;
                        break;

                    case SENDOPT_SAVE:
                        nSelID = IDS_SAVEONLY;
                        break;
                        
                    default:
                        break;
                }

                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, nSelID, TRUE);
                IMENUCTL_SetSel(pMenu, nSelID);
            }
            return TRUE;

        case EVT_USER_REDRAW:

            //(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            {
                byte   btSendmode;
                
                switch (wParam)
                {
                    // 发出短信模式 为发送并保存
                    case IDS_SENDANDSAVE:
                        btSendmode = SENDOPT_SAVEANDSEND;
                        break;

                    // 发出短信模式为 只发送
                    case IDS_SENDONLY:
                        btSendmode = SENDOPT_SEND;
                        break;
                    
                    // 发出短信模式为 只保存
                    case IDS_SAVEONLY:
                        btSendmode = SENDOPT_SAVE;
                        break;
                    
                    default:
                        return FALSE;
                }

                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wParam, TRUE);
                (void) ICONFIG_SetItem(pMe->m_pConfig,
                                       CFGI_WMS_SENDMODE,
                                       &btSendmode,
                                       sizeof(btSendmode));                
            }
            CLOSE_DIALOG(DLGRET_OK)
            return TRUE;

        default:
            break;
    }

    return FALSE;
} // IDD_SENDMODE_Handler
/*==============================================================================
函数:
    IDD_REPORTS_Handler

说明:
    WMS Applet对话框 IDD_REPORTS 事件处理函数。
    
参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_REPORTS_Handler(void *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;

    if (NULL == pMe)
    {
        return FALSE;
    }

    pMenu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_REPORTS_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            //SetControlRect(pMe,pMenu);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
#endif
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,                                
                        IDS_REPORTTITLE, //IDS_DELIVERYREPORTS,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);
            
            // 菜单项初始化
            MENU_ADDITEM(pMenu, IDS_ENABLE);
            MENU_ADDITEM(pMenu, IDS_DISABLE);
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);

            {// 需根据具体配置选择相应菜单项
                uint16 nSelID;
                boolean bDeliveryReports;
                
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_WMS_DELIVERYREPORTS,
                                       &bDeliveryReports,
                                       sizeof(bDeliveryReports));
                
                if (bDeliveryReports)
                {
                    nSelID = IDS_ENABLE;
                }
                else
                {
                    nSelID = IDS_DISABLE;
                }
                
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, nSelID, TRUE);
                IMENUCTL_SetSel(pMenu, nSelID);
            }
            return TRUE;

        case EVT_USER_REDRAW:
            // 绘制底条提示
            // Select       Back
            //DRAW_BOTTOMBAR(BTBAR_SELECT_BACK)
            
            // 更新界面
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            //(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            switch (wParam)
            {
                // 发短信要求有投递报告
                case IDS_ENABLE:
                // 发短信不要求投递报告
                case IDS_DISABLE:
                    {
                        boolean bDeliveryReports = TRUE;
                        
                        if (wParam == IDS_DISABLE)
                        {
                            bDeliveryReports = FALSE;
                        }

                        InitMenuIcons(pMenu);
                        SetMenuIcon(pMenu, wParam, TRUE);
                        (void) ICONFIG_SetItem( pMe->m_pConfig,
                                               CFGI_WMS_DELIVERYREPORTS,
                                               &bDeliveryReports,
                                               sizeof(bDeliveryReports));
                    }
                    break;
                
                default:
                    return FALSE;
            }
            
            CLOSE_DIALOG(DLGRET_OK)
            return TRUE;

        default:
            break;
    }

    return FALSE;
} // IDD_REPORTS_Handler
#ifdef FEATURE_USES_MMS  
/*==============================================================================
函数:
    IDD_MMSNOTIFY_Handler

说明:
    WMS Applet对话框 IDD_MMSNOTIFY事件处理函数。
    
参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_OPTION_YESNO_Handle(void *pUser,
    int nCFIG_Id,
    int8* pItem,
    int nTitleTextId,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;

    if (NULL == pMe)
    {
        return FALSE;
    }

    pMenu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_MMSOPTION_YESNO);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            //SetControlRect(pMe,pMenu);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
#endif
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,                                
                        nTitleTextId, //IDS_DELIVERYREPORTS,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);
            
            // 菜单项初始化
            MENU_ADDITEM(pMenu, IDS_ENABLE);
            MENU_ADDITEM(pMenu, IDS_DISABLE);
            MENU_ADDITEM(pMenu, IDS_NORMAL);
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);

            {// 需根据具体配置选择相应菜单项
                uint16 nSelID;
                
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       nCFIG_Id,
                                       pItem,
                                       sizeof(*pItem));
                if(*pItem == -1)
                {
                    nSelID = IDS_NORMAL;
                }
                else if (*pItem)
                {
                    nSelID = IDS_ENABLE;
                }
                else
                {
                    nSelID = IDS_DISABLE;
                }
                
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, nSelID, TRUE);
                IMENUCTL_SetSel(pMenu, nSelID);
            }
            return TRUE;

        case EVT_USER_REDRAW:
            // 绘制底条提示
            // Select       Back
            //DRAW_BOTTOMBAR(BTBAR_SELECT_BACK)
            
            // 更新界面
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            //(void)IMENUCTL_Redraw(pMenu);   //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            switch (wParam)
            {
                // 发短信要求有投递报告
                case IDS_ENABLE:
                    *pItem = TRUE;
                    break;
                // 发短信不要求投递报告
                case IDS_DISABLE:
                    *pItem = FALSE;
                    break;
                case IDS_NORMAL:
                    *pItem = -1;
                    break;
                
                default:
                    return FALSE;
            }
            InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, wParam, TRUE);
            (void) ICONFIG_SetItem( pMe->m_pConfig,
                                   nCFIG_Id,
                                   pItem,
                                   sizeof(*pItem));
            
            CLOSE_DIALOG(DLGRET_OK)
            return TRUE;

        default:
            break;
    }

    return FALSE;
}
/*==============================================================================
函数:
    IDD_MMSNOTIFY_Handler

说明:
    WMS Applet对话框 IDD_MMSNOTIFY事件处理函数。
    
参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/

static boolean IDD_MMSNOTIFY_Handler(void *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    return IDD_OPTION_YESNO_Handle(pUser,
        CFGI_WMS_MMSNOTIFY,
        (int8*)&((WmsApp*)pUser)->m_isCheckMMSNotify,
        IDS_MMSNOTIFY,
        eCode,
        wParam,
        dwParam);
        
} // IDD_REPORTS_Handler
static boolean IDD_MMSDELIVERYREPORT_Handler(void *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{    
    return IDD_OPTION_YESNO_Handle(pUser,
        CFGI_WMS_MMSDELIVERYREPORT,
        &((WmsApp*)pUser)->m_isMMSDeliveryReport,
        IDS_MMSDELIVERYREPORT,
        eCode,
        wParam,
        dwParam);
}
static boolean IDD_READREPLY_Handler(void *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    return IDD_OPTION_YESNO_Handle(pUser,
        CFGI_WMS_READREPLY,
        &((WmsApp*)pUser)->m_isMMSReadReply,
        IDS_READREPLY,
        eCode,
        wParam,
        dwParam);
}       
static boolean IDD_REPORTALLOWED_Handler(void *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    return IDD_OPTION_YESNO_Handle(pUser,
        CFGI_WMS_REPORTALLOWED,
        &((WmsApp*)pUser)->m_isMMSReporyAllowed,
        IDS_REPORTALLOWED,
        eCode,
        wParam,
        dwParam);
}       
static boolean IDD_SENDERVISIBILITY_Handler(void *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    return IDD_OPTION_YESNO_Handle(pUser,
        CFGI_WMS_SENDERVISIBILITY,
        &((WmsApp*)pUser)->m_isMMSSenderVisibility,
        IDS_SENDERVISIBILITY,
        eCode,
        wParam,
        dwParam);
}   

#endif
#ifdef FEATURE_AUTOREPLACE
/*==============================================================================
函数:
    IDD_AUTOREPLACE_Handler

说明:
    WMS Applet对话框 IDD_AUTOREPLACE 事件处理函数。
    
参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_AUTOREPLACE_Handler(void *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;

    if (NULL == pMe)
    {
        return FALSE;
    }

    pMenu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_AUTOREPLACE_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            //SetControlRect(pMe,pMenu);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);
            
            // 菜单项初始化
            MENU_ADDITEM(pMenu, IDS_ENABLE);
            MENU_ADDITEM(pMenu, IDS_DISABLE);
            {
                AECHAR WTitle[40] = {0};
                (void)ISHELL_LoadResString(pMe->m_pShell,
                AEE_WMSAPPRES_LANGFILE,                                
                IDS_AUTOREPLACE,
                WTitle,
                sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
            }               
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);

            {// 需根据具体配置选择相应菜单项
                uint16 nSelID;
                boolean bAutoreplace;
                
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_WMS_AUTOREPLACE,
                                       &bAutoreplace,
                                       sizeof(bAutoreplace));
                
                if (bAutoreplace)
                {
                    nSelID = IDS_ENABLE;
                }
                else
                {
                    nSelID = IDS_DISABLE;
                }
                
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, nSelID, TRUE);
                IMENUCTL_SetSel(pMenu, nSelID);
            }
            return TRUE;

        case EVT_USER_REDRAW:
            // 更新界面
            //(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            switch (wParam)
            {
                // 允许收件箱满时，用新消息自动替换最旧消息
                case IDS_ENABLE:
                // 不允许收件箱满时，用新消息自动替换最旧消息
                case IDS_DISABLE:
                    {
                        boolean bAutoreplace = TRUE;
                        
                        if (wParam == IDS_DISABLE)
                        {
                            bAutoreplace = FALSE;
                        }
                        (void) ICONFIG_SetItem(pMe->m_pConfig,
                                               CFGI_WMS_AUTOREPLACE,
                                               &bAutoreplace,
                                               sizeof(bAutoreplace));
                                               
                        (void)IWMS_SetAutoreplace(pMe->m_pwms, 
                                                  pMe->m_clientId, 
                                                  &pMe->m_callback,
                                                  (void*)pMe,
                                                  bAutoreplace);
                    }
                    break;
                
                default:
                    return FALSE;
            }
            
            CLOSE_DIALOG(DLGRET_OK)
            return TRUE;

        default:
            break;
    }

    return FALSE;
} // IDD_AUTOREPLACE_Handler
#endif /* # FEATURE_AUTOREPLACE */

/*==============================================================================
函数:
    IDD_CALLBACKNUMSWITCH_Handler

说明:
    WMS Applet对话框 IDD_CALLBACKNUMSWITCH 事件处理函数。
    
参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_CALLBACKNUMSWITCH_Handler(void *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;

    if (NULL == pMe)
    {
        return FALSE;
    }

    pMenu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_CALLBACKNUMSWITCH_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            //SetControlRect(pMe,pMenu);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
#endif
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,                                
                        IDS_CALLBACKNUM,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);
            
            // 菜单项初始化
            MENU_ADDITEM(pMenu, IDS_ENABLE);
            MENU_ADDITEM(pMenu, IDS_DISABLE);
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);

            {// 需根据具体配置选择相应菜单项
                uint16 nSelID;
                boolean bVal;
                
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_WMS_CALLBACKNUMSWITCH,
                                       &bVal,
                                       sizeof(bVal));
                
                if (bVal)
                {
                    nSelID = IDS_ENABLE;
                }
                else
                {
                    nSelID = IDS_DISABLE;
                }
                
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, nSelID, TRUE);
                IMENUCTL_SetSel(pMenu, nSelID);
            }
            return TRUE;

        case EVT_USER_REDRAW:
            // 绘制底条提示
            // Select       Back
            //DRAW_BOTTOMBAR(BTBAR_SELECT_BACK)
            
            // 更新界面
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            //(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            switch (wParam)
            {
#if !defined(FEATURE_VERSION_C316)&&!defined(FEATURE_VERSION_K202_LM129C)

                // 发短信要求带 Callback number
                case IDS_ENABLE:
                    /*{
                        boolean bVal = TRUE;
                        
                        (void) ICONFIG_SetItem(pMe->m_pConfig,
                                               CFGI_WMS_CALLBACKNUMSWITCH,
                                               &bVal,
                                               sizeof(bVal));
                    }*/
                    CLOSE_DIALOG(DLGRET_CALLBACKNUM)
                    break;
#endif                    
                    
                // 发短信不要求带 Callback number
                case IDS_DISABLE:
                    {
                        boolean bVal = FALSE;
                        AECHAR    wstrNum[WMS_ADDRESS_MAX]={0};
                        
                        (void) ICONFIG_SetItem(pMe->m_pConfig,
                                               CFGI_WMS_CALLBACKNUMSWITCH,
                                               &bVal,
                                               sizeof(bVal));
                                               
                        (void) ICONFIG_SetItem(pMe->m_pConfig,
                                    CFGI_CALLBACKNUM,
                                    wstrNum,
                                    sizeof(wstrNum));
                    }
                    InitMenuIcons(pMenu);
                    SetMenuIcon(pMenu, wParam, TRUE);
                    CLOSE_DIALOG(DLGRET_OK)
                    break;
                
                default:
                    return FALSE;
            }

            return TRUE;

        default:
            break;
    }

    return FALSE;
} // IDD_CALLBACKNUMSWITCH_Handler

/*==============================================================================
函数:
    IDD_CALLBACKNUM_Handler

说明:
    WMS Applet对话框 IDD_CALLBACKNUM 事件处理函数。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_CALLBACKNUM_Handler(void   *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    ITextCtl *pTextCtl = NULL;
    WmsApp *pMe = (WmsApp *)pUser;
    MSG_FATAL("IDD_CALLBACKNUM_Handler eCode=0x%x, wParam=0x%x",eCode,wParam,0);
    if (NULL == pMe)
    {
        return FALSE;
    }

    pTextCtl = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_CALLBACKNUM_TEXT);
    if (NULL == pTextCtl)
    {
        return FALSE;
    }

    switch(eCode)
    {
        case EVT_DIALOG_INIT:
            ITEXTCTL_SetProperties(pTextCtl, TP_FRAME | TP_NOUPDATE|TP_FOCUS_NOSEL);
            return TRUE;

        case EVT_DIALOG_START:
            SetControlRect(pMe, pTextCtl);
            if (MAX_PH_DIGITS<WMS_ADDRESS_MAX)
            {
                ITEXTCTL_SetMaxSize(pTextCtl, MAX_PH_DIGITS);
            }
            else
            {
                ITEXTCTL_SetMaxSize(pTextCtl, WMS_ADDRESS_MAX-1);
            }

            if (WSTRLEN(pMe->m_msSend.m_szCallBkNum)>0)
            {// 这是从挂起状态恢复
                DBGPRINTF("IDD_CALLBACKNUM_Handler m_szCallBkNum=%S", pMe->m_msSend.m_szCallBkNum);
                (void) ITEXTCTL_SetText(pTextCtl, pMe->m_msSend.m_szCallBkNum, -1);
            }
            else         
            {// 检查回叫号码是否设置
                AECHAR    wstrNum[WMS_ADDRESS_MAX]={0};
                
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_CALLBACKNUM,
                                       wstrNum,
                                       sizeof(wstrNum));
                DBGPRINTF("IDD_CALLBACKNUM_Handler wstrNum=%S",wstrNum);
                if (WSTRLEN(wstrNum) == 0)
                {// 取默认值: 转化后的 IMSI_S
#ifdef FEATURE_CARRIER_VENEZUELA_MOVILNET
                    AECHAR  wstrPrefix1[]={(AECHAR)'1',(AECHAR)'5', (AECHAR)'8', 0};
                    AECHAR  wstrPrefix2[]={(AECHAR)'1',(AECHAR)'9', (AECHAR)'9', 0};
                    AECHAR  wstrPrefix3[5]={0};
                    int     nLen = 0, i;
                    
                    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                           CFGI_IMSI_S,
                                           wstrNum,
                                           sizeof(AECHAR) * OEMNV_IMSI_S_LENGTH);
                    nLen = WSTRLEN(wstrNum);                       
                    // 将"158"变成"0416"
                    // 将"199"变成"0419"
                    if (nLen>3 && nLen<MAX_PH_DIGITS)
                    {
                        if (WSTRNCMP(wstrNum, wstrPrefix1, 3) == 0)
                        {
                            (void)STRTOWSTR("0416", wstrPrefix3, sizeof(wstrPrefix3));
                        }
                        else if (WSTRNCMP(wstrNum, wstrPrefix2, 3) == 0)
                        {
                            // 将"199"变成"0426" guoys modified for movilnet change 2008.10.28
                            //(void)STRTOWSTR("0419", wstrPrefix3, sizeof(wstrPrefix3));
                            (void)STRTOWSTR("0426", wstrPrefix3, sizeof(wstrPrefix3));
                        }
                    }
                    if (WSTRLEN(wstrPrefix3)>0)
                    {
                        for (i=nLen; i>3; i--)
                        {
                            wstrNum[i] = wstrNum[i-1];
                        }
                        wstrNum[0] = wstrPrefix3[0];
                        wstrNum[1] = wstrPrefix3[1];
                        wstrNum[2] = wstrPrefix3[2];
                        wstrNum[3] = wstrPrefix3[3];
                        wstrNum[nLen+1] = 0;
                    }
#endif                                                   
                }
                (void) ITEXTCTL_SetText(pTextCtl, wstrNum, -1);
            }

            (void) ITEXTCTL_SetInputMode(pTextCtl, AEE_TM_NUMBERS);    
              {
                AECHAR WTitle[40] = {0};
                (void)ISHELL_LoadResString(pMe->m_pShell,
                AEE_WMSAPPRES_LANGFILE,                                
                IDS_CALLBACKNUM,
                WTitle,
                sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
              }               
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;

        case EVT_USER_REDRAW:
        case EVT_KEY_RELEASE:
            // 绘制底条提示
            {
                AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pTextCtl);
                int nLen = 0;
                
                if (NULL != pwstrText)
                {
                    nLen = WSTRLEN(pwstrText);
                }
                #ifndef FEATURE_ALL_KEY_PAD   //add by yangdecai
                if (nLen > 0)
                {
                    if (ITEXTCTL_GetCursorPos(pTextCtl) != TC_CURSORSTART)
                    {
                        // Save       Delete
                        DRAW_BOTTOMBAR(BTBAR_SAVE_DELETE)
                    }
                    else
                    {
                        // Save       Back
                        DRAW_BOTTOMBAR(BTBAR_SAVE_BACK)
                    }
                }
                else
                #endif
                {
                    //            Back
                    DRAW_BOTTOMBAR(BTBAR_SAVE_BACK)
                }
            }
            #ifdef FEATURE_LCD_TOUCH_ENABLE
            TSIM_NumberKeypad(TRUE);
            #endif
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            
            (void)ITEXTCTL_Redraw(pTextCtl);
            
            return TRUE;
            
        case EVT_DIALOG_END:
            if (pMe->m_eAppStatus == WMSAPP_SUSPEND)
            {
                // Applet 被挂起时关闭本对话框，保存输入，待返回时恢复
                MEMSET(pMe->m_msSend.m_szCallBkNum, 0, sizeof(pMe->m_msSend.m_szCallBkNum));
                        
                (void) ITEXTCTL_GetText(pTextCtl, pMe->m_msSend.m_szCallBkNum, MAX_PH_DIGITS);
            }
            else
            {
                pMe->m_msSend.m_szCallBkNum[0] = 0;
            }
            #ifdef FEATURE_LCD_TOUCH_ENABLE
            TSIM_NumberKeypad(FALSE);
            #endif
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                case AVK_INFO:
                case AVK_SELECT:
                    {
                        AECHAR    wstrNum[WMS_ADDRESS_MAX]={0};
                        int       nLen;
                        
                        (void) ITEXTCTL_GetText(pTextCtl,
                                        wstrNum,
                                        WMS_ADDRESS_MAX);
      
                        (void) ICONFIG_SetItem(pMe->m_pConfig,
                                    CFGI_CALLBACKNUM,
                                    wstrNum,
                                    sizeof(wstrNum));

                        if (NULL != wstrNum)
                        {
                            nLen = WSTRLEN(wstrNum);
                        }
                        if(nLen > 0)
                        {
                            boolean bVal = TRUE;        
                            (void) ICONFIG_SetItem(pMe->m_pConfig,
                                                   CFGI_WMS_CALLBACKNUMSWITCH,
                                                   &bVal,
                                                   sizeof(bVal)); 
                            CLOSE_DIALOG(DLGRET_OK)
                        }
                    }
                    //CLOSE_DIALOG(DLGRET_OK)
                    return TRUE;
  
                default:
                    break;
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

                if(WMSAPP_PT_IN_RECT(wXPos,wYPos,rc))
                {
                    if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
                    {
                        boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_SELECT,0);
                        return rt;
                    }
                    else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
                    {
                         boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_INFO,0);
                         return rt;
                    }
                    else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
                    {                       
                         boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_CLR,0);
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
} // IDD_CALLBACKNUM_Handler()

/*==============================================================================
函数:
    IDD_AUTODELETE_Handler

说明:
    WMS Applet对话框 IDD_AUTODELETE 事件处理函数。
    
参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_AUTODELETE_Handler(void *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;

    if (NULL == pMe)
    {
        return FALSE;
    }

    pMenu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_AUTODELETE_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);

            //SetControlRect(pMe,pMenu);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,                                
                        IDS_AUTODELETE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);
            {// 需根据具体配置选择相应菜单项
                uint16 nSelID;
                
                if (pMe->m_cfg.m_bAutoDelete)
                {
                    nSelID = IDL_AUTODELETE_ON;
                }
                else
                {
                    nSelID = IDL_AUTODELETE_OFF;
                }
                
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, nSelID, TRUE);
                IMENUCTL_SetSel(pMenu, nSelID);
            }
            return TRUE;

        case EVT_USER_REDRAW:
            // 绘制底条提示
            // Select       Back
            //DRAW_BOTTOMBAR(BTBAR_SELECT_BACK)
            
            // 更新界面
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            //(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            switch (wParam)
            {
                // 发短信要求有投递报告
                case IDL_AUTODELETE_ON:
                    pMe->m_cfg.m_bAutoDelete = TRUE;
                    break;

                // 发短信不要求投递报告
                case IDL_AUTODELETE_OFF:
                    pMe->m_cfg.m_bAutoDelete = FALSE;
                    break;
                
                default:
                    return FALSE;
            }
            
            CLOSE_DIALOG(DLGRET_OK)
            return TRUE;

        default:
            break;
    }

    return FALSE;
} // IDD_AUTODELETE_Handler

/*==============================================================================
函数:
    IDD_RSVDMSGALERTTIMEOUT_SETTING_Handler

说明:
    WMS Applet对话框 IDD_RSVDMSGALERTTIMEOUT_SETTING 事件处理函数。
    
参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_RSVDMSGALERTTIMEOUT_SETTING_Handler(void *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;

    if (NULL == pMe)
    {
        return FALSE;
    }

    pMenu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_RSVDMSGALERTTIMEOUT_SETTING);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            //SetControlRect(pMe,pMenu);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,                                
                        IDS_RESERVEDMSGS,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);
            
            // 菜单项初始化
            MENU_ADDITEM(pMenu, IDS_ALERTTIMEOUT);
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);

            {// 需根据具体配置选择相应菜单项
                boolean bVal = FALSE;
                
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_WMS_RESVDMSGALERT_TIMEIOUT,
                                       &bVal,
                                       sizeof(bVal));
                
                if (bVal)
                {
                    SetMenuIcon(pMenu, IDS_ALERTTIMEOUT, TRUE);
                }
                else
                {
                    SetMenuIcon(pMenu, IDS_ALERTTIMEOUT, FALSE);
                }
            }
            return TRUE;

        case EVT_USER_REDRAW:
            // 绘制底条提示
            // Select       Back
            //DRAW_BOTTOMBAR(BTBAR_SELECT_BACK)
            
            // 更新界面
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            //(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            switch (wParam)
            {
                case IDS_ALERTTIMEOUT:
                    {// 需根据具体配置选择相应菜单项
                        boolean bVal = FALSE;
                        
                        (void) ICONFIG_GetItem(pMe->m_pConfig,
                                               CFGI_WMS_RESVDMSGALERT_TIMEIOUT,
                                               &bVal,
                                               sizeof(bVal));
                        
                        bVal = !bVal;
                        
                        (void) ICONFIG_SetItem(pMe->m_pConfig,
                                               CFGI_WMS_RESVDMSGALERT_TIMEIOUT,
                                               &bVal,
                                               sizeof(bVal));
                    }
                    break;

                default:
                    return FALSE;
            }
            
            CLOSE_DIALOG(DLGRET_OK)
            return TRUE;

        default:
            break;
    }

    return FALSE;
} // IDD_RSVDMSGALERTTIMEOUT_SETTING_Handler

#ifdef FEATURE_CDSMS_RUIM
/*==============================================================================
函数:
    IDD_STORAGE_Handler

说明:
    WMS Applet对话框 IDD_STORAGE 事件处理函数。
    
参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_STORAGE_Handler(void   *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;

    if (NULL == pMe)
    {
        return FALSE;
    }

    pMenu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_STORAGE_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            //SetControlRect(pMe,pMenu);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
#endif
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,                                
                        IDS_STORAGE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);
            
            // 菜单项初始化
            MENU_ADDITEM(pMenu, IDS_RUIM);
            MENU_ADDITEM(pMenu, IDS_PHONE);
            
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
                                    
            InitMenuIcons(pMenu);
            {
                uint16 nSelID = IDS_RUIM;
                wms_memory_store_e_type   mem_store = WMS_MEMORY_STORE_RUIM;
                
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_WMS_MEMSTORE,
                                       &mem_store,
                                       sizeof(mem_store));
                                       
                if (mem_store == WMS_MEMORY_STORE_RUIM)
                {
                    nSelID = IDS_RUIM;
                }
                else
                {
                    nSelID = IDS_PHONE;
                }
                SetMenuIcon(pMenu, nSelID, TRUE);
                IMENUCTL_SetSel(pMenu, nSelID);
            }
            
            return TRUE;

        case EVT_USER_REDRAW:
            // 绘制底条提示
            // Select       Back
            //DRAW_BOTTOMBAR(BTBAR_SELECT_BACK)
            
            // 更新界面
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            //(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            {
                wms_memory_store_e_type   mem_store = WMS_MEMORY_STORE_RUIM;
                
                switch (wParam)
                {
                    // 存储在 RUIM 上
                    case IDS_RUIM:
                        break;
    
                    // 存储在话机内存上
                    case IDS_PHONE:
                        mem_store = WMS_MEMORY_STORE_NV_CDMA;
                        break;
                    
                    default:
                        return FALSE;
                }
                
                // 保存配置
                (void) ICONFIG_SetItem(pMe->m_pConfig,
                                       CFGI_WMS_MEMSTORE,
                                       &mem_store,
                                       sizeof(mem_store));
                
                // 配置短信存储空间
                WmsApp_UpdateAnnunciators(pMe);
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wParam, TRUE);
                CLOSE_DIALOG(DLGRET_OK)
            }
            return TRUE;

        default:
            break;
    }

    return FALSE;
} // IDD_STORAGE_Handler
#endif

/*==============================================================================
函数:
    IDD_MESSAGEVALIDITY_Handler

说明:
    WMS Applet对话框 IDD_MESSAGEVALIDITY 事件处理函数。
    
参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_MESSAGEVALIDITY_Handler(void   *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;

    if (NULL == pMe)
    {
        return FALSE;
    }

    pMenu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_MESSAGEVALIDITY_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            //SetControlRect(pMe,pMenu);
            //add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,                                
                        IDS_MSGVALIDITY,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);
            
            // 菜单项初始化
            MENU_ADDITEM(pMenu, IDS_MSGVALIDITY_MAX);
            MENU_ADDITEM(pMenu, IDS_MSGVALIDITY_1HOUR);
            MENU_ADDITEM(pMenu, IDS_MSGVALIDITY_6HOURS);
            MENU_ADDITEM(pMenu, IDS_MSGVALIDITY_24HOURS);
            MENU_ADDITEM(pMenu, IDS_MSGVALIDITY_3DAYS);
            MENU_ADDITEM(pMenu, IDS_MSGVALIDITY_1WEEK);
#ifdef FEATURE_FLASH_SMS
            MENU_ADDITEM(pMenu, IDS_MSGVALIDITY_IMMEDIATE);
#endif
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
                                    
            InitMenuIcons(pMenu);
            {// 需根据具体配置选择相应菜单项
                uint16 nSelID = IDS_NORMAL;
                byte   btValidity = OEMNV_SMS_VALIDITYPERIOD_MAX;
                
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_WMS_VALIDITYPERIOD,
                                       &btValidity,
                                       sizeof(btValidity));
                
                switch (btValidity)
                {
                    case OEMNV_SMS_VALIDITYPERIOD_MAX://无限
                        nSelID = IDS_MSGVALIDITY_MAX;
                        break;
                        
                    case OEMNV_SMS_VALIDITYPERIOD_1OUR://1 hour
                        nSelID = IDS_MSGVALIDITY_1HOUR;
                        break;
                        
                    case OEMNV_SMS_VALIDITYPERIOD_6OURS://6 hours
                        nSelID = IDS_MSGVALIDITY_6HOURS;
                        break;
                        
                    case OEMNV_SMS_VALIDITYPERIOD_24OURS://24 hours
                        nSelID = IDS_MSGVALIDITY_24HOURS;
                        break;
                        
                    case OEMNV_SMS_VALIDITYPERIOD_3DAYS://3 days
                        nSelID = IDS_MSGVALIDITY_3DAYS;
                        break;
                        
                    case OEMNV_SMS_VALIDITYPERIOD_1WEEK://1 week
                        nSelID = IDS_MSGVALIDITY_1WEEK;
                        break;
                    #ifdef FEATURE_FLASH_SMS
					case OEMNV_SMS_VALIDITYPERIOD_IMMEDIATE:
						nSelID = IDS_MSGVALIDITY_IMMEDIATE;
						break;
					#endif
                    default:
                        nSelID = IDS_MSGVALIDITY_MAX;
                        break;
                }
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
#endif
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, nSelID, TRUE);
                IMENUCTL_SetSel(pMenu, nSelID);
            }
            return TRUE;

        case EVT_USER_REDRAW:
            // 绘制底条提示
            // Select       Back
            //DRAW_BOTTOMBAR(BTBAR_SELECT_BACK)
            
            // 更新界面
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            //(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            {
                byte   btValidity = OEMNV_SMS_VALIDITYPERIOD_MAX;
                
                switch (wParam)
                {
                    case IDS_MSGVALIDITY_MAX://无限
                        btValidity = OEMNV_SMS_VALIDITYPERIOD_MAX;
                        break;
                        
                    case IDS_MSGVALIDITY_1HOUR://1 hour
                        btValidity = OEMNV_SMS_VALIDITYPERIOD_1OUR;
                        break;
                        
                    case IDS_MSGVALIDITY_6HOURS://6 hours
                        btValidity = OEMNV_SMS_VALIDITYPERIOD_6OURS;
                        break;
                        
                    case IDS_MSGVALIDITY_24HOURS://24 hours
                        btValidity = OEMNV_SMS_VALIDITYPERIOD_24OURS;
                        break;
                        
                    case IDS_MSGVALIDITY_3DAYS://3 days
                        btValidity = OEMNV_SMS_VALIDITYPERIOD_3DAYS;
                        break;
                        
                    case IDS_MSGVALIDITY_1WEEK://1 week
                        btValidity = OEMNV_SMS_VALIDITYPERIOD_1WEEK;
                        break;
                    #ifdef FEATURE_FLASH_SMS
					case IDS_MSGVALIDITY_IMMEDIATE:
						btValidity = OEMNV_SMS_VALIDITYPERIOD_IMMEDIATE;
						break;
					#endif
                    default:
                        return TRUE;
                }
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wParam, TRUE);
                (void) ICONFIG_SetItem(pMe->m_pConfig,
                                       CFGI_WMS_VALIDITYPERIOD,
                                       &btValidity,
                                       sizeof(btValidity));
            }
            
            CLOSE_DIALOG(DLGRET_OK)
            return TRUE;

        default:
            break;
    }

    return FALSE;
} // IDD_MESSAGEVALIDITY_Handler


//Add By zzg 2012_02_04
/*==============================================================================
函数:
    IDD_TIME_STAMP_Handler

说明:
    WMS Applet对话框 IDD_TIME_STAMP 事件处理函数。
    
参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_TIME_STAMP_Handler(void   *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;

    if (NULL == pMe)
    {
        return FALSE;
    }

    pMenu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_TIME_STAMP_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            //SetControlRect(pMe,pMenu);
            //add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,                                
                        IDS_TIME_STAMP,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);
            
            // 菜单项初始化
            MENU_ADDITEM(pMenu, IDS_TIME_STAMP_ADJUST);
            MENU_ADDITEM(pMenu, IDS_TIME_STAMP_ASRECEIVED);

            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
                                    
            InitMenuIcons(pMenu);
            {// 需根据具体配置选择相应菜单项
                uint16 nSelID = IDS_TIME_STAMP_ADJUST;
                byte   btTimeStamp = OEMNV_SMS_TIMESTAMP_ADJUST;
                
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_SMS_TIMESTAMP,
                                       &btTimeStamp,
                                       sizeof(btTimeStamp));
                
                switch (btTimeStamp)
                {
                    case OEMNV_SMS_TIMESTAMP_ADJUST:
                        nSelID = IDS_TIME_STAMP_ADJUST;
                        break;
                        
                    case OEMNV_SMS_TIMESTAMP_ASRECEIVED:
                        nSelID = IDS_TIME_STAMP_ASRECEIVED;
                        break;                       
                    
                    default:
                        break;
                }
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);
                IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
#endif
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, nSelID, TRUE);
                IMENUCTL_SetSel(pMenu, nSelID);
            }
            return TRUE;

        case EVT_USER_REDRAW:
            // 绘制底条提示
            // Select       Back
            //DRAW_BOTTOMBAR(BTBAR_SELECT_BACK)
            
            // 更新界面
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            //(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            {
                byte   btTimeStamp = OEMNV_SMS_TIMESTAMP_ASRECEIVED;
                
                switch (wParam)
                {
                    case IDS_TIME_STAMP_ADJUST:
                        btTimeStamp = OEMNV_SMS_TIMESTAMP_ADJUST;
                        break;
                        
                    case IDS_TIME_STAMP_ASRECEIVED:
                        btTimeStamp = OEMNV_SMS_TIMESTAMP_ASRECEIVED;
                        break;
                                            
                    default:
                        return TRUE;
                }
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wParam, TRUE);
                (void) ICONFIG_SetItem(pMe->m_pConfig,
                                       CFGI_SMS_TIMESTAMP,
                                       &btTimeStamp,
                                       sizeof(btTimeStamp));
            }
            
            CLOSE_DIALOG(DLGRET_OK)
            return TRUE;

        default:
            break;
    }

    return FALSE;
} 
//Add End


#ifdef FEATURE_USES_MMS
    /*==============================================================================
    函数:
        IDD_GETTING_Handler
    
    说明:
        WMS Applet对话框IDD_GETTING事件处理函数。显示群发送消息的状态。
    
    参数:
        pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
        eCode [in]: 事件代码。
        wParam[in]: 事件参数
        dwParam [in]: 与事件关联的数据。
    
    返回值:
        TRUE:  传入事件得到处理。
        FALSE: 传入事件没被处理。
    
    备注:
    发送消息过程：
    1、重发
    (1) m_pSendList 中仅一项，为消息发送地址。m_CurMsgNodes 中保存了当前整条消息的
        全部数据包的 Cache 节点。m_idxCur 用于检索节点。WmsApp_ReSendMsgTimer 负责
        发出读取当前节点消息的命令。
    (2) IDD_GETTING_Handler 处理读取消息的结果。
    
    ==============================================================================*/
    static boolean IDD_GETTING_Handler(void *pUser,
        AEEEvent eCode,
        uint16   wParam,
        uint32   dwParam
    )
    {
        WmsApp *pMe = (WmsApp *)pUser;
        //static boolean bAniStart = FALSE;
        
        if (NULL == pMe)
        {
            return FALSE;
        }
        
        switch (eCode)
        {
            case EVT_DIALOG_INIT:
                if (pMe->m_pImage == NULL)
                {
                    pMe->m_pImage = ISHELL_LoadImage(pMe->m_pShell, SENDINGSMS_ANI);
                }
                return TRUE;
    
            case EVT_DIALOG_START:
                (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                        EVTFLG_ASYNC,
                                        AEECLSID_WMSAPP,
                                        EVT_USER_REDRAW,
                                        0, 
                                        0);
                return TRUE;
    
            // 发送提示
            case EVT_USER_REDRAW:
                MSG_FATAL("IDD_SENDING_Handler EVT_USER_REDRAW",0,0,0);
                {// 发送消息提示界面
                    AECHAR wszTitle[64] = {0};
                    CMultiSendItemInfo *pItem = NULL;
                    AEERect rc;
                    int x, y, nLineHeight;
                    RGBVAL oldColor = 0;
                    
#ifdef FEATURE_CARRIER_CHINA_VERTU
                    {
                        IImage *pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
                        
                        Appscommon_ResetBackground(pMe->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &pMe->m_rc, 0, 0);
                        if(pImageBg != NULL)
                        {
                            IImage_Release(pImageBg);
                        }
                    }
#else
                    Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
#endif
                    
                    //获得系统字体高度
                    nLineHeight = IDISPLAY_GetFontMetrics(pMe->m_pDisplay,
                                    AEE_FONT_NORMAL,
                                    NULL,
                                    NULL);
                                    
                    x=0;
                #ifdef FEATURE_VERSION_FLEXI203P
                    y=SENDINGSMS_ANI_Y*2;
                #else
                    y=SENDINGSMS_ANI_Y;
                #endif
                    if (pMe->m_pImage != NULL)
                    {
                        AEEImageInfo info;
                        
                        IIMAGE_GetInfo(pMe->m_pImage, &info);
                        y += info.cy;
                    }
                    
                    SETAEERECT(&rc,  0, y, pMe->m_rc.dx, pMe->m_rc.dy-y);
                    
                    (void) ISHELL_LoadResString(pMe->m_pShell,
                            AEE_WMSAPPRES_LANGFILE,
                            IDS_GETTINGFROM,
                            wszTitle,
                            sizeof(wszTitle));
                    
                    //IDISPLAY_FillRect(pMe->m_pDisplay, &rc, SENDINGSMS_ANI_G);
    
                    y += WMS_PIXELS_BETWEEN_LINE;
                    oldColor = IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, SENDINGSMS_TEXT_COLOR);
                    // 绘制提示信息-- Sending To
                    IDISPLAY_DrawText(pMe->m_pDisplay, 
                                AEE_FONT_NORMAL, wszTitle,
                                -1, 0, y, NULL, 
                                IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER);
                    
                    wszTitle[0] = 0;
                    y += (nLineHeight + WMS_PIXELS_BETWEEN_LINE);
                    
                    pItem = (CMultiSendItemInfo *)IVector_ElementAt(pMe->m_pSendList, 0);
                    
                    if (NULL != pItem)
                    {
                        if (WSTRLEN(pItem->m_szName) > 0)
                        {
                            (void)WSTRCPY(wszTitle, pItem->m_szName);
                        }
                        else
                        {
                            (void)WSTRCPY(wszTitle, pItem->m_szTo);
                        }
                        // 绘制提示信息-- 目的地或人名
                        IDISPLAY_DrawText( pMe->m_pDisplay, 
                                    AEE_FONT_NORMAL, wszTitle,
                                    -1, 0, y, NULL, 
                                    IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER);
                        DBGPRINTF("IDD_SENDING_Handler wszTitle=%S", wszTitle);
                    }
                    IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, oldColor);
                    
                    WmsApp_PlaySendingAni(pMe);
                }
                return TRUE;
                
            // 发送结果提示
            case EVT_UPDATE:
                MSG_FATAL("EVT_UPDATE",0,0,0);
                if (!WmsApp_CurmessageIsFullSendout(pMe))
                {// 发送完毕才提示
                    PFNNOTIFY pfn = WmsApp_MultSendMsgTimer;
                    MSG_FATAL("EVT_UPDATE 2 m_eCreateWMSType=%d",pMe->m_eCreateWMSType,0,0);
                    switch (pMe->m_eCreateWMSType)
                    {
                        case SEND_MSG_RESEND:
                            pfn = WmsApp_ReSendMsgTimer;
                            break;
                            
                        case SEND_MSG_RESERVE:
                            pfn = WmsApp_SendReservedMsgTimer;
                            break;
                            
                        default:
                            break;
                    }
                    
                    (void)ISHELL_SetTimer(pMe->m_pShell, 300, pfn, pMe);
    
                    return TRUE;
                }
                // 保存发送失败的短信到 OUTBOX
                // 对于 FEATURE_CARRIER_VENEZUELA_MOVILNET 要求为群发的每个目标保存短信
                // ，短信发送前已保存，这里不再保存失败的短信

                {// 消息发送结果提示界面
                    AECHAR wszText[32] = {0};
                    uint16 nResID = IDS_FAILED;
                    AEERect rc;
                    int x, y;
                    RGBVAL oldColor = 0;
                    MSG_FATAL("消息发送结果提示界面",0,0,0);
                    x=0;
                    y=SENDINGSMS_ANI_Y;
                    if (pMe->m_pImage != NULL)
                    {
                        AEEImageInfo info;
                        
                        IIMAGE_GetInfo(pMe->m_pImage, &info);
                        y += info.cy;
                    }
                
                    SETAEERECT(&rc,  0, y, pMe->m_rc.dx, pMe->m_rc.dy-y);
                        //MSG_FATAL("MMS_GetSocketReadStatus()=%d",MMS_GetSocketReadStatus(),0,0);
                        //if(MMS_GetSocketReadStatus() == HTTP_CODE_OK)
                    if(pMe->m_GetStatus == HTTP_CODE_OK)
                    {
                        nResID = IDS_SAVED;
                    }
                    else
                    {
                        nResID = IDS_FAILED;
                    }
                    
                    (void) ISHELL_LoadResString(pMe->m_pShell,
                            AEE_WMSAPPRES_LANGFILE,
                            nResID,
                            wszText,
                            sizeof(wszText));
                            
                    //IDISPLAY_FillRect(pMe->m_pDisplay, &rc, SENDINGSMS_ANI_G);                
#ifdef FEATURE_CARRIER_CHINA_VERTU
                    {
                        IImage *pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
                        
                        Appscommon_ResetBackground(pMe->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &rc, 0, 0);
                        if(pImageBg != NULL)
                        {
                            IImage_Release(pImageBg);
                        }
                    }
#else
                    Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &rc, TRUE);
#endif
                    
                    oldColor = IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, SENDINGSMS_TEXT_COLOR);
                    IDISPLAY_DrawText( pMe->m_pDisplay, 
                                AEE_FONT_NORMAL, wszText,
                                -1, 0, 0, &rc, 
                                IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE);
                    
                    IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, oldColor);
                    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
#ifdef FEATURE_USES_MMS
                    if(pMe->m_isMMS)    
                    {
                        (void)ISHELL_PostEventEx(pMe->m_pShell,
                                                 EVTFLG_ASYNC, 
                                                 AEECLSID_WMSAPP, 
                                                 EVT_SENDSMSEND,
                                                 0, 
                                                 0);
                    }
#endif                
                }
                return TRUE;
                    
            case EVT_DIALOG_END:
                MSG_FATAL("IDD_SENDING_Handler EVT_DIALOG_END",0,0,0);                   
                (void) ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY)WmsApp_PlaySendingAni, pMe);
                if (pMe->m_pImage != NULL)
                {
                    //WmsApp_StopSendingAni(pMe);
                    IIMAGE_Release(pMe->m_pImage);
                    pMe->m_pImage = NULL;
                }
                return TRUE;
    
            case EVT_KEY:
                return TRUE;
                
            case EVT_SENDSMSEND:
                MSG_FATAL("IDD_SENDING_Handler EVT_SENDSMSEND",0,0,0);
                CLOSE_DIALOG(DLGRET_END)
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
    
                    if(WMSAPP_PT_IN_RECT(wXPos,wYPos,rc))
                    {
                        if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
                        {
                            boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_SELECT,0);
                            return rt;
                        }
                        else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
                        {
                             boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_INFO,0);
                             return rt;
                        }
                        else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
                        {                       
                             boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_CLR,0);
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
    } // IDD_SENDING_Handler
#endif
/*==============================================================================
函数:
    IDD_SENDING_Handler

说明:
    WMS Applet对话框IDD_SENDING事件处理函数。显示群发送消息的状态。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:
发送消息过程：
1、重发
(1) m_pSendList 中仅一项，为消息发送地址。m_CurMsgNodes 中保存了当前整条消息的
    全部数据包的 Cache 节点。m_idxCur 用于检索节点。WmsApp_ReSendMsgTimer 负责
    发出读取当前节点消息的命令。
(2) IDD_SENDING_Handler 处理读取消息的结果。

==============================================================================*/
static boolean IDD_SENDING_Handler(void *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    WmsApp *pMe = (WmsApp *)pUser;
    //static boolean bAniStart = FALSE;
    
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
#ifdef FEATURE_USES_MMS             
            if(!pMe->m_isMMS)
#endif                
            {
                pMe->m_bSaveFailedMsg = FALSE;
                
                if (pMe->m_ContinueSendType == NONE_CONTINUE)
                {// 非继续发送
                    pMe->m_bMuitiSend = FALSE;
                    
                    if (IVector_Size(pMe->m_pSendList)>1)
                    {
                        pMe->m_bMuitiSend = TRUE;
                    }
                }
                if (pMe->m_bMuitiSend) 
                {
                    (void)IWMS_DcDisableAutoDisconnect(pMe->m_pwms, 
                                                       pMe->m_clientId, 
                                                       &pMe->m_callback,
                                                       (void*)pMe);
                                                       
                    (void)IWMS_SetMultiSend(pMe->m_pwms, 
                                            pMe->m_clientId, 
                                            &pMe->m_callback,
                                            (void*)pMe,
                                            TRUE);
                }
            }
            if (pMe->m_pImage == NULL)
            {
                pMe->m_pImage = ISHELL_LoadImage(pMe->m_pShell, SENDINGSMS_ANI);
            }
            return TRUE;

        case EVT_DIALOG_START:
#ifdef FEATURE_USES_MMS   
            if(!pMe->m_isMMS)
#endif
            {
                if (pMe->m_eCreateWMSType == SEND_MSG_RESEND)
                {
                    (void)ISHELL_SetTimer(pMe->m_pShell,
                                    150,
                                    (PFNNOTIFY)WmsApp_ReSendMsgTimer,
                                    pMe);
                }
                else if (pMe->m_eCreateWMSType == SEND_MSG_RESERVE)
                {
                    (void)ISHELL_SetTimer(pMe->m_pShell,
                                    150,
                                    (PFNNOTIFY)WmsApp_SendReservedMsgTimer,
                                    pMe);
                }
                else if (pMe->m_ContinueSendType == NONE_CONTINUE)
                {
                    boolean bSetTimer = TRUE;
                    
                    // 建立客户消息列表
                    WmsApp_BuildSendClentMsgList(pMe);
                    
                    if (NULL == pMe->m_pCurSendCltMsg)
                    {
                        bSetTimer = FALSE;
                    }
                    else if (pMe->m_SendOPT != SENDOPT_SEND)
                    {// 消息需保存
                        int nRet;
                      
                        // 保存消息
                        nRet = IWMS_MsgWrite(pMe->m_pwms, 
                                            pMe->m_clientId, 
                                            &pMe->m_callback,
                                            (void*)pMe,
                                            WMS_WRITE_MODE_INSERT,
                                            pMe->m_pCurSendCltMsg[pMe->m_idxCurSend]);
                                            
                        if (nRet == SUCCESS)
                        {
                            bSetTimer = FALSE;
                        }
    					
                    }
                    
                    if (bSetTimer)
                    {
                        (void)ISHELL_SetTimer(pMe->m_pShell,
                                        150,
                                        (PFNNOTIFY)WmsApp_MultSendMsgTimer,
                                        pMe);
                    }
                }
                else 
                {
                    WmsApp_MultSendMsgTimer((void *)pMe);
                }
            }
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;

        // 发送提示
        case EVT_USER_REDRAW:
            MSG_FATAL("IDD_SENDING_Handler EVT_USER_REDRAW",0,0,0);
            if ((pMe->m_eCreateWMSType != SEND_MSG_RESEND) &&
                (pMe->m_eCreateWMSType != SEND_MSG_RESERVE) &&
                (NULL == pMe->m_pCurSendCltMsg) 
#ifdef FEATURE_USES_MMS   
                && !pMe->m_isMMS
#endif                
                )
            {
                CLOSE_DIALOG(DLGRET_END)
            }
            else
            {// 发送消息提示界面
                AECHAR wszTitle[64] = {0};
                CMultiSendItemInfo *pItem = NULL;
                AEERect rc;
                int x, y, nLineHeight;
                RGBVAL oldColor = 0;
                
#ifdef FEATURE_CARRIER_CHINA_VERTU
                {
                    IImage *pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
                    
                    Appscommon_ResetBackground(pMe->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &pMe->m_rc, 0, 0);
                    if(pImageBg != NULL)
                    {
                        IImage_Release(pImageBg);
                    }
                }
#else
                Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
#endif
                //Add by pyuangui 2013-01-09
                { 
	                AECHAR wstrTitle[MAX_TITLE_LEN] = {0};
	                int nLen = 0;
	                
	                (void)ISHELL_LoadResString(pMe->m_pShell, 
	                            AEE_WMSAPPRES_LANGFILE,
	                            IDS_RECEIVER,
	                            wstrTitle,
	                            sizeof(wstrTitle));
	                            
	                nLen = WSTRLEN(wstrTitle);
					WSPRINTF(&wstrTitle[nLen], (MAX_TITLE_LEN-nLen)*sizeof(AECHAR), 
	                    L" (%d/%d)", IVector_Size(pMe->m_pSendList), MAXNUM_MULTISEND); 
					#if 0
	                (void)IMENUCTL_SetTitle(pMenu, NULL, 0, wstrTitle);
					#else
					{
					
					IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,wstrTitle);
					}
					#endif
            	}	
                //Add End
                
                //获得系统字体高度
                nLineHeight = IDISPLAY_GetFontMetrics(pMe->m_pDisplay,
                                AEE_FONT_NORMAL,
                                NULL,
                                NULL);
                                
                x=0;
                #ifdef FEATURE_VERSION_FLEXI203P
                y=SENDINGSMS_ANI_Y*2;
                #else
                y=SENDINGSMS_ANI_Y;
                #endif
                if (pMe->m_pImage != NULL)
                {
                    AEEImageInfo info;
                    
                    IIMAGE_GetInfo(pMe->m_pImage, &info);
                    y += info.cy;
                }
                
                SETAEERECT(&rc,  0, y, pMe->m_rc.dx, pMe->m_rc.dy-y);
                
                (void) ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,
                        IDS_SENDINGTO,
                        wszTitle,
                        sizeof(wszTitle));
                
                //IDISPLAY_FillRect(pMe->m_pDisplay, &rc, SENDINGSMS_ANI_G);

                y += WMS_PIXELS_BETWEEN_LINE;
                oldColor = IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, SENDINGSMS_TEXT_COLOR);
                // 绘制提示信息-- Sending To
                IDISPLAY_DrawText(pMe->m_pDisplay, 
                            AEE_FONT_NORMAL, wszTitle,
                            -1, 0, y, NULL, 
                            IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER);
                
                wszTitle[0] = 0;
                y += (nLineHeight + WMS_PIXELS_BETWEEN_LINE);
                
                pItem = (CMultiSendItemInfo *)IVector_ElementAt(pMe->m_pSendList, 0);
                
                if (NULL != pItem)
                {
                    if (WSTRLEN(pItem->m_szName) > 0)
                    {
                        (void)WSTRCPY(wszTitle, pItem->m_szName);
                    }
                    else
                    {
                        (void)WSTRCPY(wszTitle, pItem->m_szTo);
                    }
                    // 绘制提示信息-- 目的地或人名
                    IDISPLAY_DrawText( pMe->m_pDisplay, 
                                AEE_FONT_NORMAL, wszTitle,
                                -1, 0, y, NULL, 
                                IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER);
                    DBGPRINTF("IDD_SENDING_Handler wszTitle=%S", wszTitle);
                }
                IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, oldColor);
                
                WmsApp_PlaySendingAni(pMe);
                /*if (bAniStart == FALSE)
                {
                    WmsApp_PlaySendingAni(pMe);
                    bAniStart = TRUE;
                }*/
                
                //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            }
            return TRUE;
            
        // 发送结果提示
        case EVT_UPDATE:
            MSG_FATAL("EVT_UPDATE",0,0,0);
            if (!WmsApp_CurmessageIsFullSendout(pMe))
            {// 发送完毕才提示
                PFNNOTIFY pfn = WmsApp_MultSendMsgTimer;
                MSG_FATAL("EVT_UPDATE 2 m_eCreateWMSType=%d",pMe->m_eCreateWMSType,0,0);
                switch (pMe->m_eCreateWMSType)
                {
                    case SEND_MSG_RESEND:
                        pfn = WmsApp_ReSendMsgTimer;
                        break;
                        
                    case SEND_MSG_RESERVE:
                        pfn = WmsApp_SendReservedMsgTimer;
                        break;
                        
                    default:
                        break;
                }
                
                (void)ISHELL_SetTimer(pMe->m_pShell, 300, pfn, pMe);

                return TRUE;
            }
            // 保存发送失败的短信到 OUTBOX
            // 对于 FEATURE_CARRIER_VENEZUELA_MOVILNET 要求为群发的每个目标保存短信
            // ，短信发送前已保存，这里不再保存失败的短信
#if !defined(FEATURE_CARRIER_VENEZUELA_MOVILNET)
            if ((pMe->m_SendStatus != WMS_RPT_OK) && 
                ((pMe->m_SendOPT == SENDOPT_SEND) || pMe->m_bMuitiSend))
            {// 之前没保存
                int i, nRet;
                
                pMe->m_bSaveFailedMsg = TRUE;
                if (pMe->m_bMuitiSend)
                {
                    if (pMe->m_SucNum == 0 && 
                        pMe->m_FailNum == 0 &&
                        pMe->m_SendOPT != SENDOPT_SEND)
                    {// 群发第一条已保存
                        pMe->m_bSaveFailedMsg = FALSE;
                    }
                }

                if (pMe->m_bSaveFailedMsg && NULL != pMe->m_pCurSendCltMsg)
                {
                    for(i=0; i<pMe->m_nSendItems; i++)
                    { 
                        // 修改标记
                        pMe->m_pCurSendCltMsg[i]->msg_hdr.tag = WMS_TAG_MO_NOT_SENT;
                        
                        // 保存消息
                        nRet = IWMS_MsgWrite(pMe->m_pwms, 
                                            pMe->m_clientId, 
                                            &pMe->m_callback,
                                            (void*)pMe,
                                            WMS_WRITE_MODE_INSERT,
                                            pMe->m_pCurSendCltMsg[i]);
                                            
                        if (nRet != SUCCESS)
                        {
                            break;
                        }
                        else
                        {// 休眠10毫秒以确保有时间执行保存消息的操作
                            MSLEEP(10);
                        }
                    }
                }
            }
#endif            
            {// 消息发送结果提示界面
                AECHAR wszText[32] = {0};
                uint16 nResID = IDS_FAILED;
                AEERect rc;
                int x, y;
                RGBVAL oldColor = 0;
                MSG_FATAL("消息发送结果提示界面",0,0,0);
                x=0;
                y=SENDINGSMS_ANI_Y;
                if (pMe->m_pImage != NULL)
                {
                    AEEImageInfo info;
                    
                    IIMAGE_GetInfo(pMe->m_pImage, &info);
                    y += info.cy;
                }
            
                SETAEERECT(&rc,  0, y, pMe->m_rc.dx, pMe->m_rc.dy-y);
#ifdef FEATURE_USES_MMS
                if(pMe->m_isMMS)    
                {
                    char pszPath[AEE_MAX_FILE_NAME]={'\0'};
                    if(pMe->m_isForward)//如果是转发进来的，必须把转发的临时文件给删掉
                    {
            		    IFileMgr *pIFileMgr = NULL;                             
                        AEEFileInfo myInfo;
                        int result = 0;
                        result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);
                        if( IFILEMGR_EnumInit(pIFileMgr, "fs:/hsmm/MMSTemp", FALSE ) == SUCCESS )
                        {
                            while( IFILEMGR_EnumNext(pIFileMgr, &myInfo ) )
                            {
                                result = IFILEMGR_Remove(pIFileMgr, myInfo.szName);
                                if( result != SUCCESS )
                                {
                                    DBGPRINTF( "File %s could not be removed result=%d", myInfo.szName, result );
                                }
                                DBGPRINTF( "File name %s", myInfo.szName );
                            }
                            IFILEMGR_RmDir(pIFileMgr, "fs:/hsmm/MMSTemp");
                        }
                        pMe->m_isForward = FALSE;
                        RELEASEIF(pIFileMgr);                        
                    }                    
                    //MSG_FATAL("MMS_GetSocketReadStatus()=%d",MMS_GetSocketReadStatus(),0,0);
                    //if(MMS_GetSocketReadStatus() == HTTP_CODE_OK)
                    if(pMe->m_SendStatus == HTTP_CODE_OK)
                    {
                        nResID = IDS_MSGSENT;
                    }
                    else
                    {
                        nResID = IDS_FAILED;
                    }
                    DBGPRINTF("MMSImageName=%s len=%d", pszPath, STRLEN(pszPath));
                    ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSIMAGE,pszPath, sizeof(pszPath));      
                    ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSSOUND,pszPath, sizeof(pszPath)); 
                    ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSVIDEO,pszPath, sizeof(pszPath)); 
                    FREEIF(pMe->m_EncData.pMessage);
                    pMe->m_hasImage = FALSE;
                    pMe->m_hasSound = FALSE;
                    pMe->m_hasVideo = FALSE;
                    pMe->m_isMMS = FALSE;
                    pMe->m_isSendToAlbumOrEmain = FALSE; 
                }
                else
#endif            
                {
                    MSG_FATAL("pMe->m_SendStatus==================%d",pMe->m_SendStatus,0,0);
                    if (pMe->m_SendStatus == WMS_RPT_OK)
                    {
                        nResID = IDS_MSGSENT;
                        pMe->m_SucNum += 1;
                    }
                    else
                    {
                        // 为满足CDG2测试要求，针对几个特殊状态予以提示
                        switch(pMe->m_SendtlStatus)
                        {
                            // Unknown destination address.
                            case WMS_TL_ADDRESS_TRANSLATION_FAILURE_S: 
                                nResID = IDS_CDG2_FAIL1;
                                break;
           
                            // The mobile station originated WMS feature is not
                            // activated in the network.
                            case WMS_TL_SMS_ORIGINATION_DENIED_S: 
                                nResID = IDS_CDG2_FAIL97;
                                break;
           
                            // The WMS feature is not supported by the base station.
                            case WMS_TL_SMS_NOT_SUPPORTED_S: 
                                nResID = IDS_CDG2_FAIL100;
                                break;
           
                            default:
                                break;
                        }
                        pMe->m_FailNum += 1;
                    }
                }
                (void) ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,
                        nResID,
                        wszText,
                        sizeof(wszText));
                        
                //IDISPLAY_FillRect(pMe->m_pDisplay, &rc, SENDINGSMS_ANI_G);                
#ifdef FEATURE_CARRIER_CHINA_VERTU
                {
                    IImage *pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
                    
                    Appscommon_ResetBackground(pMe->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &rc, 0, 0);
                    if(pImageBg != NULL)
                    {
                        IImage_Release(pImageBg);
                    }
                }
#else
                Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &rc, TRUE);
#endif
                
                oldColor = IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, SENDINGSMS_TEXT_COLOR);
                IDISPLAY_DrawText( pMe->m_pDisplay, 
                            AEE_FONT_NORMAL, wszText,
                            -1, 0, 0, &rc, 
                            IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER|IDF_ALIGN_MIDDLE);
                
                IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, oldColor);
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
#ifdef FEATURE_USES_MMS
                if(pMe->m_isMMS)    
                {
                    (void)ISHELL_PostEventEx(pMe->m_pShell,
                                             EVTFLG_ASYNC, 
                                             AEECLSID_WMSAPP, 
                                             EVT_SENDSMSEND,
                                             0, 
                                             0);
                }
                else
#endif                
                // 设置定时器函数启动下一次发送
                {
                    PFNNOTIFY pfn = WmsApp_MultSendMsgTimer;
                    
                    switch (pMe->m_eCreateWMSType)
                    {
                        case SEND_MSG_RESEND:
                            pfn = WmsApp_ReSendMsgTimer;
                            break;
                            
                        case SEND_MSG_RESERVE:
                            pfn = WmsApp_SendReservedMsgTimer;
                            break;
                            
                        default:
                            break;
                    }
                    
                    (void)ISHELL_SetTimer(pMe->m_pShell, 300, pfn, pMe);
                }
            }
            return TRUE;
                
        case EVT_DIALOG_END:
            MSG_FATAL("IDD_SENDING_Handler EVT_DIALOG_END",0,0,0);
#ifdef FEATURE_USES_MMS
            if(!pMe->m_isMMS)   
#endif                    
            {
                (void)IWMS_SetMultiSend(pMe->m_pwms, 
                                        pMe->m_clientId, 
                                        &pMe->m_callback,
                                        (void*)pMe,
                                        FALSE);
            }
            (void) ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY)WmsApp_PlaySendingAni, pMe);
            if (pMe->m_pImage != NULL)
            {
                //WmsApp_StopSendingAni(pMe);
                IIMAGE_Release(pMe->m_pImage);
                pMe->m_pImage = NULL;
            }
#ifdef FEATURE_USES_MMS
            if(!pMe->m_isMMS)   
#endif   
            {
                //bAniStart = FALSE;
                (void)IWMS_DcEnableAutoDisconnect(pMe->m_pwms, 
                                                  pMe->m_clientId, 
                                                  &pMe->m_callback,
                                                  (void*)pMe,
                                                  WMSAPP_AUTO_DISCONNECT_TIME);
                
                if ((pMe->m_eDlgReturn == DLGRET_CREATE) &&
                    (pMe->m_eAppStatus != WMSAPP_STOP))
                {
                    pMe->m_bNeedContinueSend = WmsApp_IsNeedContinueSendTask(pMe);
                }
                else
                {
                    pMe->m_bNeedContinueSend = FALSE;
                    pMe->m_ContinueSendType = NONE_CONTINUE;
                }
            }
            return TRUE;

        case EVT_KEY:
            return TRUE;
            
        // 保存消息时返回
        case EVT_WMS_MSG_WRITE:
            {
                wms_memory_store_e_type mem_store;
                wms_message_index_type  wIndex;
                wms_message_tag_e_type  tag;
                wms_msg_event_info_s_type *Info = (wms_msg_event_info_s_type*)dwParam;
                MSG_FATAL("IDD_SENDING_Handler EVT_WMS_MSG_WRITE",0,0,0);
                mem_store = Info->status_info.message.msg_hdr.mem_store;
                wIndex = Info->status_info.message.msg_hdr.index;
                tag = Info->status_info.message.msg_hdr.tag;
                
                // 为避免发送短信途中收到短信时的写操作，先判断此事件是否为写发出短信的操作
                if ((tag != WMS_TAG_MO_NOT_SENT) || pMe->m_bSaveFailedMsg)
                {
                    return TRUE;
                }
                
                // 对于发送中保存发送失败短信(特别是群发)时不应修改 pMe->m_idxCurSend
                // 否则回导致发送无法结束的情况
                if (pMe->m_bSaveFailedMsg)
                {
                    return TRUE;
                }
                if (mem_store == pMe->m_pCurSendCltMsg[pMe->m_idxCurSend]->msg_hdr.mem_store)
                {
                    int nRet;
                    boolean bSetTimer = TRUE;
                    
                    pMe->m_pCurSendCltMsg[pMe->m_idxCurSend]->msg_hdr.index = wIndex;
                    
                    pMe->m_idxCurSend++;
                    if (pMe->m_idxCurSend<pMe->m_nSendItems)
                    {
                        // 保存消息
                        nRet = IWMS_MsgWrite(pMe->m_pwms, 
                                            pMe->m_clientId, 
                                            &pMe->m_callback,
                                            (void*)pMe,
                                            WMS_WRITE_MODE_INSERT,
                                            pMe->m_pCurSendCltMsg[pMe->m_idxCurSend]);
                                            
                        if (nRet == SUCCESS)
                        {
                            bSetTimer = FALSE;
                        }
                    }
                    
                    if (bSetTimer)
                    {// 开始发送过程
                        pMe->m_idxCurSend = 0;
                        
                        (void)ISHELL_SetTimer(pMe->m_pShell,
                                        150,
                                        (PFNNOTIFY)WmsApp_MultSendMsgTimer,
                                        pMe);
                    }
                }
            }
            
            return TRUE;
            
        // 读消息命令返回
        case EVT_WMS_MSG_READ:
            MSG_FATAL("IDD_SENDING_Handler EVT_WMS_MSG_READ",0,0,0);
            if (((pMe->m_eCreateWMSType == SEND_MSG_RESEND) ||
                 (pMe->m_eCreateWMSType == SEND_MSG_RESERVE)) && 
                (pMe->m_idxCur < LONGSMS_MAX_PACKAGES))
            {
                int                     nRet;
                wms_cache_info_node     *pnode = NULL;
                wms_memory_store_e_type mem_store;
                wms_message_index_type  wIdx;
                wms_msg_event_info_s_type *Info = (wms_msg_event_info_s_type*)dwParam;
                wms_client_message_s_type *pClientMsg = NULL;
                
                mem_store = Info->status_info.message.msg_hdr.mem_store;
                wIdx = Info->status_info.message.msg_hdr.index;
                pnode = pMe->m_CurMsgNodes[pMe->m_idxCur];
                
                if ((mem_store == pnode->mem_store) && (pnode->index == wIdx))
                {
                    WmsApp_FreeSendClentMsgList(pMe);
                    pClientMsg = (wms_client_message_s_type *)MALLOC(sizeof(wms_client_message_s_type));
                    DBGPRINTF("WMS Address = 0x%x", pClientMsg);
                    pMe->m_pCurSendCltMsg = (wms_client_message_s_type **)MALLOC(sizeof(wms_client_message_s_type *));
                    DBGPRINTF("WMS Address = 0x%x", pMe->m_pCurSendCltMsg);
                    if ((NULL == pnode) || 
                        (pMe->m_pCurSendCltMsg == NULL) ||
                        (NULL == pClientMsg))
                    {
                        if (NULL != pClientMsg)
                        {
                            FREE(pClientMsg);
                        }
                        if (NULL != pMe->m_pCurSendCltMsg)
                        {
                            FREE(pMe->m_pCurSendCltMsg);
                        }
                        
                        CLOSE_DIALOG(DLGRET_END)
                        
                        ERR("MALLOC Failed!",0,0,0);
                        return TRUE;
                    }
                    
                    MEMCPY(pClientMsg, &(Info->status_info.message), sizeof(wms_client_message_s_type));
                    pMe->m_pCurSendCltMsg[0] = pClientMsg;
                    pMe->m_nSendItems = 1;
                    pMe->m_idxCurSend = 0;
                    
                    // 对于预约短信，这里将标签预设为发送成功状态，若成功发送此消息，则将此消息写入发件箱
                    // ，同时删除此消息；否则消息将被标记为预约失败消息
                    if (pMe->m_eCreateWMSType == SEND_MSG_RESERVE)
                    {
                        pClientMsg->msg_hdr.tag = WMS_TAG_MO_SENT;
                    }
                    
                    pMe->m_bSending = TRUE;
                    pMe->m_bDCDisconnectedInSending = FALSE;
					if((int)WMS_TAG_MO_NOT_SENT == pMe->m_pCurSendCltMsg[pMe->m_idxCurSend]->msg_hdr.tag)
					{
                    nRet = IWMS_MsgSend(pMe->m_pwms, 
                                        pMe->m_clientId, 
                                        &pMe->m_callback,
                                        (void*)pMe,
                                        WMS_SEND_MODE_CLIENT_MESSAGE,
                                        pMe->m_pCurSendCltMsg[pMe->m_idxCurSend]);
					}
                                        
                    if (nRet != SUCCESS)
                    {
                        pMe->m_bSending = FALSE;
                        WmsApp_FreeSendClentMsgList(pMe);
                        CLOSE_DIALOG(DLGRET_END)
                    }
                }
            }
            
            return TRUE;
            
        case EVT_WMS_CMD_STATUS:
            {
                wmsapp_cmd_status_type  *pStatus = (wmsapp_cmd_status_type *)dwParam;
                MSG_FATAL("IDD_SENDING_Handler EVT_WMS_CMD_STATUS",0,0,0);
                if (NULL == pStatus)
                {
                    return TRUE;
                }
                
                switch (pStatus->cmd)
                {
                    case WMS_CMD_MSG_WRITE:
                        // 对于发送中保存发送失败短信(特别是群发)时不应修改 pMe->m_idxCurSend
                        // 否则回导致发送无法结束的情况
                        if (pMe->m_bSaveFailedMsg)
                        {
                            return TRUE;
                        }
                        
                        if ((WMS_CMD_ERR_NONE != pStatus->cmd_err) &&
                            (pMe->m_eCreateWMSType != SEND_MSG_RESERVE))
                        {// 保存发送消息失败, 不再进行后续保存操作，变更发送选项，启动发送进程
                            pMe->m_idxCurSend = 0;
                            
                            (void)ISHELL_SetTimer(pMe->m_pShell,
                                            150,
                                            (PFNNOTIFY)WmsApp_MultSendMsgTimer,
                                            pMe);
                        }
                        
                        break;
                    
                    case WMS_CMD_MSG_READ:
                        if (WMS_CMD_ERR_NONE != pStatus->cmd_err)
                        {// 读取待发送消息失败
                            CLOSE_DIALOG(DLGRET_END)
                        }
                        break;
                        
                    default:
                        break;
                }
            }
            
            return TRUE;
            
        case EVT_SENDSMSEND:
            MSG_FATAL("IDD_SENDING_Handler EVT_SENDSMSEND",0,0,0);
            CLOSE_DIALOG(DLGRET_END)
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

				if(WMSAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_CLR,0);
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
} // IDD_SENDING_Handler

/*==============================================================================
函数:
    WmsApp_UpdateAddListMenu

说明:
    更新当前地址列表菜单。

参数:
    pMe [in]: 指向 WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    pMenu [in/out]: IMenuCtl接口指针。

返回值:
    uint16 : 菜单应设置的选中项。

备注:
    本函数只负责创建菜单列表，不显示设置选中项，不做菜单刷新操作。
        
==============================================================================*/
static uint16 WmsApp_UpdateAddListMenu(WmsApp *pMe, IMenuCtl *pMenu)
{
    int32   i;
    int32   wItemCount;
    AECHAR  wstrBlank[10]={0};
    CtlAddItem  mai;
    uint16      nSelID = 0;
    int         nStart = 0, nEnd = 0;
    boolean     bAddBlankItem = FALSE;
    CMultiSendItemInfo *pItem = NULL;
    
    if ((NULL == pMe) || (NULL == pMenu) || (NULL == pMe->m_pSendList))
    {
        return (nSelID+MSG_CMD_BASE);
    }
    
    (void)STRTOWSTR("   ", wstrBlank, sizeof(wstrBlank));
    
    // 取链表项数
    wItemCount = IVector_Size(pMe->m_pSendList);
    ERR("wItemCount = %d",wItemCount,0,0);
    
    // 确定建立菜单列表时，对应消息列表的起点位置
    switch (pMe->m_eMakeAddListMode)
    {
        // 非群发时适用
        case MAKEADDLIST_NONE:
            nStart = 0;
            nEnd = 0;
            pMe->m_nCurAddPage = 0;
            bAddBlankItem = TRUE;
            break;
            
        // 群发-地址列表菜单输入界面初始化时适用
        case MAKEADDLIST_INIT:
            pMe->m_nCurAddPage = 0;
            nStart = 0;
            if (wItemCount == 0)
            {
                bAddBlankItem = TRUE;
                nEnd = 0;
            }
            else if (wItemCount <= MAXITEMS_ONEPAGE)
            {
                nEnd = wItemCount-1;
            }
            else
            {
                nEnd = MAXITEMS_ONEPAGE-1;
            }
            break;
            
        case MAKEADDLIST_NEXTONE:
            if (wItemCount == 0)
            {
                pMe->m_nCurAddPage = 0;
                nStart = 0;
                nEnd = wItemCount;
                bAddBlankItem = TRUE;
            }
            else if (wItemCount < MAXITEMS_ONEPAGE)
            {
                pMe->m_nCurAddPage = 0;
                bAddBlankItem = TRUE;
                nStart = 0;
                nEnd = wItemCount;
                nSelID = nEnd;
            }
            else if (pMe->m_nCurAddPage + MAXITEMS_ONEPAGE < wItemCount)
            {
                pMe->m_nCurAddPage++;
                nStart = pMe->m_nCurAddPage;
                nEnd = pMe->m_nCurAddPage+MAXITEMS_ONEPAGE-1;
                nSelID = nEnd;
            }
            else if (wItemCount < MAXNUM_MULTISEND)
            {
                pMe->m_nCurAddPage++;
                bAddBlankItem = TRUE;
                nStart = pMe->m_nCurAddPage;
                nEnd = pMe->m_nCurAddPage+MAXITEMS_ONEPAGE-1;
                nSelID = nEnd;
            }
            else
            {
                pMe->m_nCurAddPage = 0;
                nStart = 0;
                nEnd = MAXITEMS_ONEPAGE-1;
                nSelID = nStart;
            }
            break;
            
        case MAKEADDLIST_BACKONE:
            if (wItemCount == 0)
            {
                pMe->m_nCurAddPage = 0;
                nStart = 0;
                nEnd = wItemCount;
                bAddBlankItem = TRUE;
            }
            else if (wItemCount < MAXITEMS_ONEPAGE)
            {
                pMe->m_nCurAddPage = 0;
                nStart = 0;
                nEnd = wItemCount-1;
                nSelID = nEnd;
            }
            else if (pMe->m_nCurAddPage > 0)
            {
                pMe->m_nCurAddPage--;
                nStart = pMe->m_nCurAddPage;
                nEnd = pMe->m_nCurAddPage+MAXITEMS_ONEPAGE-1;
                nSelID = nStart;
            }
            else
            {
                pMe->m_nCurAddPage = wItemCount-MAXITEMS_ONEPAGE;
                nStart = pMe->m_nCurAddPage;
                nEnd = wItemCount-1;
                nSelID = nEnd;
            }
            break;
            
        case MAKEADDLIST_RESUMECURPAGE:
            if (wItemCount == 0)
            {
                pMe->m_nCurAddPage = 0;
                nStart = 0;
                nEnd = wItemCount;
                bAddBlankItem = TRUE;
            }
            else if (wItemCount < MAXITEMS_ONEPAGE)
            {
                pMe->m_nCurAddPage = 0;
                nStart = 0;
                nEnd = wItemCount-1;
            }
            else if (pMe->m_nCurAddPage+MAXITEMS_ONEPAGE > wItemCount)
            {
                pMe->m_nCurAddPage = wItemCount-MAXITEMS_ONEPAGE;
                nStart = pMe->m_nCurAddPage;
                nEnd = pMe->m_nCurAddPage+MAXITEMS_ONEPAGE-1;
            }
            else
            {
                nStart = pMe->m_nCurAddPage;
                nEnd = pMe->m_nCurAddPage+MAXITEMS_ONEPAGE-1;
            }
            
            if ((pMe->m_CurAddID >= nStart+MSG_CMD_BASE) &&
                (pMe->m_CurAddID <= nEnd+MSG_CMD_BASE))
            {
                nSelID = pMe->m_CurAddID - MSG_CMD_BASE;
            }
            else
            {
                nSelID = nStart;
            }
            break;
            
        default:
            return (nSelID+MSG_CMD_BASE);
    }
    
    // 建立菜单项列表
    MEMSET(&mai, 0, sizeof(mai));
    mai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
    mai.wImage = IDB_NAME;
    mai.pszResText = NULL;
    mai.pImage = NULL;
    mai.wFont = AEE_FONT_NORMAL;
    mai.dwData = 0;
    
    // 先清除旧有消息列表
    (void)IMENUCTL_DeleteAll(pMenu);

    for (i=nStart; i<=nEnd; i++)
    {
        if (i<wItemCount)
        {
            pItem = (CMultiSendItemInfo *)IVector_ElementAt(pMe->m_pSendList, i);
            
            if (NULL == pItem)
            {
                continue;
            }
            
            if ((i != nSelID) && (WSTRLEN(pItem->m_szName) > 0))
            {
                mai.pText = pItem->m_szName;
            }
#ifdef FEATURE_USES_MMS                  
            else if(pMe->m_isSendToAlbumOrEmain && (WSTRLEN(pItem->m_szEmail) > 0))
            {
                mai.pText = pItem->m_szEmail;
            }
#endif               
            else
            {
                mai.pText = pItem->m_szTo;
            }
        }
        else if (bAddBlankItem)
        {
            mai.pText = wstrBlank;
        }
        else
        {
            continue;
        }

        mai.wItemID = MSG_CMD_BASE + i;
            
        // 添加带图标菜单项到菜单
        (void)IMENUCTL_AddItemEx(pMenu, &mai);
    }
    
    return (nSelID+MSG_CMD_BASE);
} //WmsApp_UpdateAddListMenu()

/*==============================================================================
函数:
    IDD_TONUMLIST_Handler

说明:
    WMS Applet对话框 IDD_TONUMLIST 事件处理函数。处理消息发往目标地址输入。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_TONUMLIST_Handler(void   *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    IMenuCtl  *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;
    ITextCtl *pIText = NULL;
   
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_TONUMLIST_MENU);
    pIText = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_NUMTEXT);

    if ((NULL == pMenu) || (NULL == pIText))
    {
        return FALSE;
    }
    MSG_FATAL("IDD_TONUMLIST_Handler---------eCode=%x-----wParam=%x",eCode,wParam,0);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            SetControlRect(pMe, pMenu);
            //IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_NO_REDRAW);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_NODRAWSELECTBAR | OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
#endif
            {
                AECHAR WTitle[20] = {0};
                (void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,                                
                        IDS_RECEIVER,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            ITEXTCTL_SetProperties(pIText, TP_NOUPDATE|TP_FOCUS_NOSEL);
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
                                    
            {
                uint32 dwMask = IDIALOG_GetProperties(pMe->m_pActiveIDlg);
                
                dwMask |= DLG_NOT_SET_FOCUS_AUTO;
                IDIALOG_SetProperties(pMe->m_pActiveIDlg, dwMask);
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
                ITEXTCTL_SetProperties(pIText, TP_FIXSETRECT|TP_FOCUS_NOSEL);//show cursor
#else
#ifdef FEATURE_USES_MMS                         
                if(pMe->m_isSendToAlbumOrEmain)
                {
                    ITEXTCTL_SetProperties( pIText, (TP_FIXSETRECT |TP_EDITNUMBER_PTSTRING |TP_FIXOEM | TP_USELESS_UPDOWN | TP_FOCUS_NOSEL| TP_STARKEY_SWITCH));
                    ITEXTCTL_SetMaxSize(pIText, MAX_EMAILADD_LEN);    
                }
                else
#endif                    
                {
                    ITEXTCTL_SetProperties(pIText, TP_FIXSETRECT|TP_FIXOEM|TP_USELESS_UPDOWN|TP_FOCUS_NOSEL);
                    ITEXTCTL_SetMaxSize(pIText, MAX_PH_DIGITS);
                }
#endif
                
                (void)ITEXTCTL_SetInputMode(pIText, AEE_TM_NUMBERS);
                ITEXTCTL_SetActive(pIText, FALSE);
            }
            return TRUE;

        case  EVT_USER_REDRAW:
            {
                AEERect rc;
                AECHAR wstrText[2] = {0};
                CMultiSendItemInfo *pItem = NULL;
				int32 nCount = IVector_Size(pMe->m_pSendList);
                uint16 nSelID = WmsApp_UpdateAddListMenu(pMe, pMenu);

				int nLen=0;
                if (pMe->m_eMakeAddListMode != MAKEADDLIST_NONE)
                {// 群发: 提示当前已输入地址数和最大允许输入地址数
                    AECHAR wstrTitle[MAX_TITLE_LEN] = {0};
                    int nLen = 0;
                    
                    (void)ISHELL_LoadResString(pMe->m_pShell, 
                                AEE_WMSAPPRES_LANGFILE,
                                IDS_RECEIVER,
                                wstrTitle,
                                sizeof(wstrTitle));
                                
                    nLen = WSTRLEN(wstrTitle);
                    WSPRINTF(&wstrTitle[nLen], (MAX_TITLE_LEN-nLen)*sizeof(AECHAR), 
                        L" (%d/%d)", nCount, MAXNUM_MULTISEND); 
					#if 0
                    (void)IMENUCTL_SetTitle(pMenu, NULL, 0, wstrTitle);
					#else
					{
					
					IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,wstrTitle);
					}
					#endif
                    
                    // 画底条提示方向键
                    SetArrowFlagonIM(TRUE);
                }
                
                IMENUCTL_SetSel(pMenu, nSelID);
                pItem = WmsApp_GetAddItem(pMe->m_pSendList, (nSelID-MSG_CMD_BASE));
                DBGPRINTF("nCount = %d, pwstrText=%S",nCount, ITEXTCTL_GetTextPtr(pIText));
                if (nCount<=0)
                { 
                    #ifdef FEATURE_LCD_TOUCH_ENABLE    
                        {
                         AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pIText);
                         ITEXTCTL_SetText(pIText, pwstrText, -1);   
                        }
                    #else
                      #ifdef FEATURE_USES_MMS
                        if(pMe->m_isSendToAlbumOrEmain)
                         {
                             AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pIText);
                             if(pwstrText == NULL)
                             {
                                 DBGPRINTF("IDD_TONUMLIST_Handler pwstrText==NULL");   
                             }
                             else
                             {
                                 DBGPRINTF("IDD_TONUMLIST_Handler pwstrText=%S", pwstrText);   
                             }
                             ITEXTCTL_SetText(pIText, pwstrText, -1);   
                         }
                        else
                      #else
                         {
                          ITEXTCTL_SetText(pIText, wstrText, -1); 
                         }
                      #endif
                    #endif
                    // 绘制底条提示
                    // Search      Back
                    //MENU_SETBOTTOMBAR(pMenu, BTBAR_FIND_BACK);
                    DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
                }
                else
                {
#ifdef FEATURE_USES_MMS    
                    if(pMe->m_isSendToAlbumOrEmain)
                    {
                        if(pItem != NULL)
                        {
                            (void)ITEXTCTL_SetText(pIText, pItem->m_szEmail, -1);  
                        }
                        else
                        {
                            (void)ITEXTCTL_SetText(pIText, L"", -1);  
                        }
                    }
                    else
#endif                        
                    {
                        (void)ITEXTCTL_SetText(pIText, pItem->m_szTo, -1);  
                    }
                    // 绘制底条提示
                    
                    if ((pMe->m_eCreateWMSType == SEND_MSG_RESERVE) ||
                        (pMe->m_eCreateWMSType == SEND_MSG_EDITRESERVE))
                    {
                    	#ifndef FEATURE_ALL_KEY_PAD   //add by yangdecai
                        DRAW_BOTTOMBAR(BTBAR_SAVE_DELETE)
                        #else
                        DRAW_BOTTOMBAR(BTBAR_SEND_BACK);
                        #endif
                    }
                    else
                    {
                    	#ifndef FEATURE_ALL_KEY_PAD   //add by yangdecai
                        DRAW_BOTTOMBAR(BTBAR_SEND_EARSE);
                        #else
                        DRAW_BOTTOMBAR(BTBAR_SEND_BACK);
                        #endif
                    }
                }
                pMe->m_CurAddID = nSelID;             
                (void)IMENUCTL_Redraw(pMenu);
                IMENUCTL_GetSelItemRect(pMenu, &rc);
                ITEXTCTL_SetRect(pIText, &rc);
                IMENUCTL_SetActive(pMenu, FALSE);
                ITEXTCTL_SetActive(pIText, TRUE);
                (void)IDIALOG_SetFocus(pMe->m_pActiveIDlg, IDC_NUMTEXT);
                
                (void)ITEXTCTL_SetInputMode(pIText, AEE_TM_NUMBERS);
                #ifdef FEATURE_LCD_TOUCH_ENABLE
                TSIM_NumberKeypad(TRUE);
                #endif
                ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
                (void)ITEXTCTL_Redraw(pIText);
            }
            IDISPLAY_Update(pMe->m_pDisplay);
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            return TRUE;
            
        case EVT_DIALOG_END:
            if (pMe->m_eMakeAddListMode != MAKEADDLIST_NONE)
            {
                pMe->m_eMakeAddListMode = MAKEADDLIST_RESUMECURPAGE;
                
                // 画底条提示方向键
                SetArrowFlagonIM(FALSE);
            }
            if(pMe->m_eAppStatus == WMSAPP_SUSPEND)
            {
                CMultiSendItemInfo *pItem = NULL;
                AECHAR *pwsText = ITEXTCTL_GetTextPtr(pIText);
                
                if (pwsText == NULL)
                {
                    return TRUE;
                }
                
                pItem = WmsApp_GetAddItem(pMe->m_pSendList, (pMe->m_CurAddID-MSG_CMD_BASE));
                #ifdef FEATURE_LCD_TOUCH_ENABLE
                TSIM_NumberKeypad(FALSE);
                #endif
                if (WSTRLEN(pwsText)>0)
                {
                    if (NULL == pItem)
                    {// 保存当前地址的缓冲区尚未分配
                        pItem = (CMultiSendItemInfo *)sys_malloc(sizeof(CMultiSendItemInfo));
                        DBGPRINTF("WMS Address = 0x%x", pItem);
                        if (NULL == pItem)
                        {// 缓冲区分配不成功
                            return TRUE;
                        }
                        
                        if (SUCCESS != IVector_AddElement(pMe->m_pSendList, pItem))
                        {
                            sys_free(pItem);
                            return FALSE;
                        }
                    }
                    else
                    {
                        MEMSET(pItem->m_szName, 0, sizeof(pItem->m_szName));
                    }
#ifdef FEATURE_USES_MMS                  
                    if(pMe->m_isSendToAlbumOrEmain)
                    {
                        (void)ITEXTCTL_GetText(pIText, pItem->m_szEmail, MAX_EMAILADD_LEN + 1);
                    }
                    else
#endif                          
                    (void)ITEXTCTL_GetText(pIText, pItem->m_szTo, MAX_PH_DIGITS);
                    
                    // 调用电话本接口获取人名
                    WMSUtil_GetContactName(pMe, pItem->m_szTo, pItem->m_szName, MAX_TITLE_LEN);
                    
                }
            }
            return TRUE;

        case EVT_KEY_PRESS:
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
                case AVK_INFO:    
                    {
#ifdef FEATURE_USES_MMS             
                        DBGPRINTF("IDD_TONUMLIST_Handler EVT_KEY_PRESS");
                        if(pMe->m_isSendToAlbumOrEmain)
                        {
                            AECHAR *pwsText = ITEXTCTL_GetTextPtr(pIText);
                            DBGPRINTF("IDD_TONUMLIST_Handler AVK_INFO,pwsText=%S", pwsText);
                            MEMSET(pMe->m_szEmail,0,sizeof(pMe->m_szEmail));
                            MEMCPY(pMe->m_szEmail, pwsText, sizeof(pMe->m_szEmail));
                            CLOSE_DIALOG(DLGRET_EDIT_ALBUMOREMAIN);
                        }
#endif                            
                    }
                    return TRUE;                    
            }
        
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_INFO:
                case AVK_SELECT:
                    {
                        CMultiSendItemInfo *pItem = NULL;
                        AECHAR *pwsText = ITEXTCTL_GetTextPtr(pIText);
                        
                        if (pwsText == NULL)
                        {
                            return TRUE;
                        }
                        
                        pItem = WmsApp_GetAddItem(pMe->m_pSendList, (pMe->m_CurAddID-MSG_CMD_BASE));
                        
                        if (WSTRLEN(pwsText)>0)
                        {
                            if (NULL == pItem)
                            {// 保存当前地址的缓冲区尚未分配
                                pItem = (CMultiSendItemInfo *)sys_malloc(sizeof(CMultiSendItemInfo));
                                DBGPRINTF("WMS Address = 0x%x", pItem);
                                if (NULL == pItem)
                                {// 缓冲区分配不成功
                                    return TRUE;
                                }
                                
                                if (SUCCESS != IVector_AddElement(pMe->m_pSendList, pItem))
                                {
                                    sys_free(pItem);
                                    return FALSE;
                                }
                            }
                            else
                            {
#ifdef FEATURE_USES_MMS                  
                                if(pMe->m_isSendToAlbumOrEmain)
                                {
                                    MEMSET(pItem->m_szEmail, 0, sizeof(pItem->m_szEmail));
                                }
                                else
#endif                                 
                                MEMSET(pItem->m_szName, 0, sizeof(pItem->m_szName));
                            }
#ifdef FEATURE_USES_MMS                  
                            if(pMe->m_isSendToAlbumOrEmain)
                            {
                                (void)ITEXTCTL_GetText(pIText, pItem->m_szEmail, MAX_EMAILADD_LEN + 1);
                            }
                            else
#endif                
                            (void)ITEXTCTL_GetText(pIText, pItem->m_szTo, MAX_PH_DIGITS + 1);
                            
                            // 调用电话本接口获取人名
                            WMSUtil_GetContactName(pMe, pItem->m_szTo, pItem->m_szName, MAX_TITLE_LEN);
#ifdef FEATURE_USES_MMS
                            if(pMe->m_isMMS)
                            {
                                uint8 len = 0;
                                uint8 count = IVector_Size(pMe->m_pSendList);
                                IFileMgr *pIFileMgr = NULL;
                                int result = 0;
                                uint32 pdwTotal = 0;
                                uint32 pdwFree = 0;
                                MSG_FATAL("IDD_WRITEMSG_Handler count=%d", count, 0, 0);
                                result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);
                                if (SUCCESS != result)
                                {
                                    MSG_FATAL("IDD_WRITEMSG_Handler: Open file error %x", result,0,0);
                                    return FALSE;
                                }            
                                pdwFree = IFILEMGR_GetFreeSpace(pIFileMgr, &pdwTotal); 
                                IFILEMGR_Release(pIFileMgr);    
                                MSG_FATAL("IDD_WRITEMSG_Handler pdwFree=%d, pdwTotal=%d",pdwFree, pdwTotal, 0);
                                if((pdwFree > MSG_MAX_PACKET_SIZE+50))
                                {
                                    if(NULL == pMe->m_EncData.pMessage)
                                    {
                                        pMe->m_EncData.pMessage = MALLOC(sizeof(MMS_WSP_MESSAGE_SEND));
                                        DBGPRINTF("WMS Address = 0x%x", pMe->m_EncData.pMessage);
                                        MEMSET(pMe->m_EncData.pMessage,NULL,sizeof(MMS_WSP_MESSAGE_SEND));
                                    }
                                    if(!pMe->m_isSendToAlbumOrEmain)
                                    {
                                        STRCAT((char*)pMe->m_EncData.pMessage->hTo, "PLMN");
                                    }
                                    if (count > 0)
                                    {
                                        int index = 0;
                                        char TempTo[MAX_EMAILADD_LEN];
										char TempToStr[MAX_EMAILADD_LEN];
                                        for(; index < count; ++index)
                                        {
                                            pItem = (CMultiSendItemInfo *)IVector_ElementAt(pMe->m_pSendList, index);
                                            if (NULL != pItem)
                                            {
                                                if(pMe->m_isSendToAlbumOrEmain)
                                                {
                                                    DBGPRINTF("pItem->m_szEmail=%S", pItem->m_szEmail);
                                                    WSTRTOSTR(pItem->m_szEmail,(char*)TempTo,sizeof(TempTo));
                                                }
                                                else
                                                {
                                                    DBGPRINTF("pItem->m_szTo=%S", pItem->m_szTo);
                                                    WSTRTOSTR(pItem->m_szTo,(char*)TempTo,sizeof(TempTo));
                                                } 
                                                STRCAT((char*)pMe->m_EncData.pMessage->hTo,TempTo);
                                                STRCAT((char*)pMe->m_EncData.pMessage->hTo, ",");
                                                MEMSET(TempTo, 0, sizeof(TempTo));
                                            }                                        
                                        }
                                    }
                                    DBGPRINTF("pMessage->hTo:%s",pMe->m_EncData.pMessage->hTo,0,0);
                                    pMe->m_EncData.pMessage->iRetrieveStatus = MMS_VALUE_USELESSNESS;
                                    pMe->m_EncData.pMessage->iDate = MMS_VALUE_USELESSNESS;
                                    pMe->m_EncData.pMessage->iPriority = MMS_VALUE_USELESSNESS;
                                    pMe->m_EncData.pMessage->bSenderVisibility = pMe->m_isMMSSenderVisibility;
                                    pMe->m_EncData.pMessage->iRetrieveStatus = MMS_VALUE_USELESSNESS;
                                    pMe->m_EncData.pMessage->iExpiry = MMS_VALUE_USELESSNESS;
                                    pMe->m_EncData.pMessage->iDeliveryTime = MMS_VALUE_USELESSNESS;
                                    pMe->m_EncData.pMessage->bReadRep = pMe->m_isMMSReadReply;
                                    pMe->m_EncData.pMessage->bDelRep = pMe->m_isMMSDeliveryReport;

                                    
                                    if(!WMS_MMSState(WMS_MMS_PDU_MSendReq,0,(uint32)&pMe->m_EncData))
                                    {
                                        pMe->m_SendStatus = HTTP_CODE_Bad_Request;
    		                            ISHELL_SetTimer(pMe->m_pShell,1000,(PFNNOTIFY)&WmsApp_ProcessMMSStatus,pMe);
                                    }
                                }
                            }
                            //return TRUE;
#endif
                            CLOSE_DIALOG(DLGRET_SENDOPT);
                        }
                        else
                        {
                            if (NULL != pItem)
                            {// 用户放弃当前项
                                (void)IVector_RemoveElementAt(pMe->m_pSendList, (pMe->m_CurAddID-MSG_CMD_BASE));
                                FREE(pItem);
                                pItem = NULL;
                            }
                            // 调用通信簿群发取号码接口，本Applet 会被挂起，返回时回到当前状态
                            if(wParam == AVK_SELECT)
                            {
                                CLOSE_DIALOG(DLGRET_SELECTFROMOPT);
                            }
                            else if(wParam == AVK_INFO)
                            {
                                if(pMe->m_eMakeAddListMode == MAKEADDLIST_NONE)
                                {
                                    WMSExtApp_GetAddresseeFromContactsApp(pMe, GETADDR_ONEADD);
                                }
                                else
                                {
                                    WMSExtApp_GetAddresseeFromContactsApp(pMe, GETADDR_MULTIADD);
                                }

                            }
                            //}
                        }
                    }
                    return TRUE;
                    
                // 向上滚动一屏
                case AVK_LEFT:
                	MSG_FATAL("AVK_LEFT..........................",0,0,0);
                    break;
                    
                // 向下滚动一屏
                case AVK_RIGHT:
                	MSG_FATAL("AVK_RIGHT..........................",0,0,0);
                    break;
                
                case AVK_CLR:
                	#ifdef FEATURE_ALL_KEY_PAD   //add by yangdecai
                	if(dwParam == 1)
                    {
                        AECHAR *pwsText = ITEXTCTL_GetTextPtr(pIText);
                        
                        if ((pwsText == NULL) || (WSTRLEN(pwsText)>0))
                        {
                            return TRUE;
                        }
                    }
                    #else
                    {
                        AECHAR *pwsText = ITEXTCTL_GetTextPtr(pIText);
                        
                        if ((pwsText == NULL) || (WSTRLEN(pwsText)>0))
                        {
                            return TRUE;
                        }
                    }
                    #endif
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;
                   
                default:
                    break;
            }
            return TRUE;
#ifdef FEATURE_USES_MMS
        case EVT_CTL_TEXT_MODECHANGED:  //切换输入法    
            {
                if(pMe->m_isSendToAlbumOrEmain)
                {
                    AEETextInputMode nInputMode;
                    nInputMode = ITEXTCTL_GetInputMode(pIText,NULL);
                    if (nInputMode != AEE_TM_SYMBOLS && nInputMode != AEE_TM_FACE_SYMBOL)
                    {
                        IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    }
                }
            }
            return TRUE;    
#endif
   
        case EVT_CTL_SEL_CHANGED:
            pMe->m_CurAddID = wParam;
            return TRUE;
            
        case EVT_KEY_RELEASE:
            {
                AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pIText);
                int nLen=0;
                
                if (NULL != pwstrText)
                {
                    nLen = WSTRLEN(pwstrText);
                }
                
                if (nLen >= 1)
                {
                    // Send       Delete
                    if ((pMe->m_eCreateWMSType == SEND_MSG_RESERVE) ||
                        (pMe->m_eCreateWMSType == SEND_MSG_EDITRESERVE))
                    {
                    	#ifndef FEATURE_ALL_KEY_PAD   //add by yangdecai
                        DRAW_BOTTOMBAR(BTBAR_SAVE_DELETE)
                        #else
                        DRAW_BOTTOMBAR(BTBAR_SEND_BACK)
                        #endif
                    }
                    else
                    {
                    	#ifndef FEATURE_ALL_KEY_PAD   //add by yangdecai
                        DRAW_BOTTOMBAR(BTBAR_SEND_EARSE)
                        #else
                        DRAW_BOTTOMBAR(BTBAR_SEND_BACK)
                        #endif
                    }
                    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                }
                else if (nLen <= 0)
                {
                    // Search      Back
                    DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
                    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                }
            }
            
            return TRUE;
            
        case EVT_COMMAND:
            pMe->m_CurAddID = wParam;
            CLOSE_DIALOG(DLGRET_SENDOPT);
            
            return TRUE;
        
        case EVT_CTL_TAB:
        	MSG_FATAL("EVT_CTL_TAB...............dwParam=%d",dwParam,0,0);
            if (pMe->m_eMakeAddListMode == MAKEADDLIST_NONE)
            {// 非群发
                return TRUE;
            }
            if ((IControl *)dwParam == (IControl *)pIText)
            {// 文本控件发出
                CMultiSendItemInfo *pItem = NULL;
                int nMenuItems = IMENUCTL_GetItemCount(pMenu); 
                AECHAR *pwsText = ITEXTCTL_GetTextPtr(pIText);
                int  nLen = 0;
                int32   nCount = 0;
                
                if (NULL == pwsText)
                {
                    return TRUE;
                }
                
                nLen = WSTRLEN(pwsText);
                pItem = WmsApp_GetAddItem(pMe->m_pSendList, (pMe->m_CurAddID-MSG_CMD_BASE));
                nCount = IVector_Size(pMe->m_pSendList);
                
                if (pItem == NULL)
                {// 当前为添加号码
                    if (nLen == 0)
                    {// 输入为空
                        if (nCount == 0)
                        {
                            return TRUE;
                        }
                        
                        // 用户放弃输入，但此时菜单列表只允许往上移动,但此时若为添加最后一个地址时例外
                        if (wParam > 0)
                        {
                            if (pMe->m_CurAddID < (MAXNUM_MULTISEND+MSG_CMD_BASE-1))
                            {// 禁止移动菜单选项
                                return TRUE;
                            }
                            else
                            {// 返回地址列表首页
                                pMe->m_eMakeAddListMode = MAKEADDLIST_INIT;
                            }
                        }
                        else
                        {// 往上移动--当前项在添加输入时始终未当前页最末项
                            pMe->m_CurAddID--;
                            pMe->m_eMakeAddListMode = MAKEADDLIST_RESUMECURPAGE;
                        }
                    }
                    else
                    {
                        if ((nCount == 0) && (wParam<1))
                        {
                            return TRUE;
                        }
                        
                        // 添加当前输入
                        pItem = (CMultiSendItemInfo *)sys_malloc(sizeof(CMultiSendItemInfo));
                        DBGPRINTF("WMS Address = 0x%x", pItem);
                        if (NULL != pItem)
                        {// 缓冲区分配成功
                            if (SUCCESS != IVector_AddElement(pMe->m_pSendList, pItem))
                            {
                                sys_free(pItem);
                                return TRUE;
                            }
                            else
                            {
                                MSG_FATAL("IDD_TONUMLIST_Handler IVector_AddElement SUCCESS",0,0,0);
#ifdef FEATURE_USES_MMS                  
                                if(pMe->m_isSendToAlbumOrEmain)
                                {
                                    (void)ITEXTCTL_GetText(pIText, pItem->m_szEmail, MAX_EMAILADD_LEN + 1);
                                }
                                else
#endif                                      
                                (void)ITEXTCTL_GetText(pIText, pItem->m_szTo, MAX_PH_DIGITS);
                            
                                // 调用电话本接口重新获取人名
                                WMSUtil_GetContactName(pMe, pItem->m_szTo, pItem->m_szName, MAX_TITLE_LEN);
                            }
                        }
                        else
                        {// 缓冲区分配不成功
                            return TRUE;
                        }
                        
                        // 移动菜单光标：wParam<1向上移动，否则向下移动
                        if (wParam<1)
                        {
                            if (IMENUCTL_GetItemID(pMenu,0) == pMe->m_CurAddID)
                            {
                                pMe->m_eMakeAddListMode = MAKEADDLIST_BACKONE;
                            }
                            else
                            {
                                pMe->m_CurAddID--;
                                pMe->m_eMakeAddListMode = MAKEADDLIST_RESUMECURPAGE;
                            }
                        }
                        else
                        {
                            if (IMENUCTL_GetItemID(pMenu, (nMenuItems-1)) == pMe->m_CurAddID)
                            {
                                if (pMe->m_CurAddID == MSG_CMD_BASE+MAXNUM_MULTISEND-1)
                                {
                                    pMe->m_eMakeAddListMode = MAKEADDLIST_INIT;
                                }
                                else
                                {
                                    pMe->m_eMakeAddListMode = MAKEADDLIST_NEXTONE;
                                }
                            }
                            else
                            {
                                pMe->m_CurAddID++;
                                pMe->m_eMakeAddListMode = MAKEADDLIST_RESUMECURPAGE;
                            }
                        }
                    }
                }
                else
                {// 当前为编辑号码
                    if (nLen == 0)
                    {// 输入为空
                        if (nCount == 0)
                        {
                            return TRUE;
                        }
                        
                        // 此时移动光标，视为删除当前记录
                        (void)IVector_RemoveElementAt(pMe->m_pSendList, (pMe->m_CurAddID-MSG_CMD_BASE));
                        FREE(pItem);
                        pItem = NULL;
                        
                        // 移动菜单光标：wParam<1向上移动，否则向下移动
                        if (wParam<1)
                        {// 向上
                            if (IMENUCTL_GetItemID(pMenu,0) == pMe->m_CurAddID)
                            {
                                pMe->m_eMakeAddListMode = MAKEADDLIST_BACKONE;
                            }
                            else
                            {
                                pMe->m_CurAddID--;
                                pMe->m_eMakeAddListMode = MAKEADDLIST_RESUMECURPAGE;
                            }
                        }
                        else
                        {// 向下
                            if (IMENUCTL_GetItemID(pMenu, (nMenuItems-1)) == pMe->m_CurAddID)
                            {
                                if (pMe->m_CurAddID+1-MSG_CMD_BASE<nCount)
                                {
                                    pMe->m_eMakeAddListMode = MAKEADDLIST_RESUMECURPAGE;
                                }
                                else if ((MAXITEMS_ONEPAGE == nMenuItems) &&
                                         (pMe->m_CurAddID+1-MSG_CMD_BASE== nCount))
                                {
                                    pMe->m_eMakeAddListMode = MAKEADDLIST_RESUMECURPAGE;
                                }
                                else
                                {
                                    pMe->m_eMakeAddListMode = MAKEADDLIST_NEXTONE;
                                }
                            }
                            else
                            {
                                pMe->m_eMakeAddListMode = MAKEADDLIST_RESUMECURPAGE;
                            }
                        }
                    }
                    else
                    {
                        if ((nCount == 0) && (wParam<1))
                        {
                            return TRUE;
                        }                       
#ifdef FEATURE_USES_MMS    
						DBGPRINTF("m_szEmail=%S, pwsText=%S,m_isSendToAlbumOrEmain=%d",pItem->m_szEmail, pwsText, pMe->m_isSendToAlbumOrEmain);

                        if(pMe->m_isSendToAlbumOrEmain)       
                        {
                            // 检查内容是否有变更，若有则更新记录
                            if (WSTRCMP(pItem->m_szEmail, pwsText) != 0)
                            {
                                //这里更新的记录，应该保存到m_szTo里去，因为发送时都是从m_szTo取地址的
                                (void)ITEXTCTL_GetText(pIText, pItem->m_szEmail, MAX_EMAILADD_LEN);
                                DBGPRINTF("pItem->m_szEmail=%S",pItem->m_szEmail);
                                // 清除以前可能存在的数据
                                MEMSET(pItem->m_szEmail, 0, sizeof(pItem->m_szEmail));                               
                                // 调用电话本接口重新获取人名
                                WMSUtil_GetContactName(pMe, pItem->m_szTo, pItem->m_szName, MAX_TITLE_LEN);
                            }                            
                        }
                        else
#endif                      
                        {
                            // 检查内容是否有变更，若有则更新记录
                            if (WSTRCMP(pItem->m_szTo, pwsText) != 0)
                            {
                                (void)ITEXTCTL_GetText(pIText, pItem->m_szTo, MAX_PH_DIGITS);
                                DBGPRINTF("pItem->m_szTo=%S",pItem->m_szTo);
                                // 清除以前可能存在的数据
                                MEMSET(pItem->m_szName, 0, sizeof(pItem->m_szName));
                                
                                // 调用电话本接口重新获取人名
                                WMSUtil_GetContactName(pMe, pItem->m_szTo, pItem->m_szName, MAX_TITLE_LEN);
                            }
                        }
                        
                        // 移动光标
                        if (wParam<1)
                        {// 向上
                            if (IMENUCTL_GetItemID(pMenu,0) == pMe->m_CurAddID)
                            {
                                pMe->m_eMakeAddListMode = MAKEADDLIST_BACKONE;
                            }
                            else
                            {
                                pMe->m_CurAddID--;
                                pMe->m_eMakeAddListMode = MAKEADDLIST_RESUMECURPAGE;
                            }
                        }
                        else
                        {// 向下
                            if (IMENUCTL_GetItemID(pMenu, (nMenuItems-1)) == pMe->m_CurAddID)
                            {
                                pMe->m_eMakeAddListMode = MAKEADDLIST_NEXTONE;
                            }
                            else
                            {
                                pMe->m_eMakeAddListMode = MAKEADDLIST_RESUMECURPAGE;
                                pMe->m_CurAddID++;
                            }
                        }
                    }
                }
                
                ITEXTCTL_SetActive(pIText, FALSE);
                IMENUCTL_SetActive(pMenu, TRUE);
                (void)IDIALOG_SetFocus(pMe->m_pActiveIDlg, IDC_TONUMLIST_MENU);
            }
            
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            
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

				if(WMSAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{					
						boolean rt;
						int len = WSTRLEN(ITEXTCTL_GetTextPtr(pIText));

						if((ITEXTCTL_IsActive(pIText)) && (len > 0))
							rt = ITEXTCTL_HandleEvent(pIText,EVT_KEY,AVK_CLR,0);
						else
							rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_CLR,0);
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
} // IDD_TONUMLIST_Handler

/*==============================================================================
函数:
    IDD_SENDOPTS_Handler

说明:
    WMS Applet对话框 IDD_SENDOPTS 事件处理函数。处理单发消息时相关选项。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_SENDOPTS_Handler(void   *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    static uint16 wControls[5]={0}; // 用到的控件列表
    //static AECHAR wstrText[4][20] = {0};
    static AECHAR wstrText[4][80] = {0};
    static int    nSelIdx = 0;      // 当前聚焦的控件在 wControls 的索引
    static int    nControls = 0;    // 用到的控件数
    static Send_OPT_e_Type      sendmode;
    static wms_priority_e_type  priority;
    static boolean              report;
    static boolean              cbval;    
    WmsApp *pMe = (WmsApp *)pUser;

    if (NULL == pMe)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			{

            MEMSET(wControls, 0, sizeof(wControls));
            nSelIdx = 0;
            nControls = 0;
            sendmode = pMe->m_SendOPT;
            priority = pMe->m_msSend.m_epriority;
            report   = pMe->m_msSend.m_bDeliveryReport;
            cbval    = pMe->m_CbVal;
			            
            // 初始化用到的控件列表
            {
                // 掩码 00001111 这里需要5个控件，所以掩码 改为 1111 1111
                byte mask = 0xFF;
                
                if ((pMe->m_eCreateWMSType == SEND_MSG_RESERVE) ||
                    (pMe->m_eCreateWMSType == SEND_MSG_EDITRESERVE))
                {// 不使用 IDC_MENU_SENDOPT (00001000),去掉相应掩码
                    //mask = mask & (~0x08);
                }
                                   
#ifdef FEATURE_CARRIER_TAIWAN_APBW
                // 亚太要求隐藏项 IDC_MENU_RPT (00000010),去掉相应掩码
                mask = mask & (~0x02);
#endif                    
                // 亚太要求隐藏项 IDC_TEXT_CBNUM (00000001),去掉相应掩码
                // 现改为根据设置, 亚太屏蔽掉设置即可，默认为关   
                if (!pMe->m_CbVal)
                {
                    mask = mask & (~0x01);
                }

                if (mask & 0x08)
                {
                    wControls[nControls] = IDC_MENU_SENDOPT;
                    ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE, IDS_SENDMODE, wstrText[nControls], sizeof(wstrText));
                    nControls++;
                }
                if (mask & 0x04)
                {
                    wControls[nControls] = IDC_MENU_PRI;
                    ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE, IDS_PRIORITY, wstrText[nControls], sizeof(wstrText));
                    nControls++;
                }
                if (mask & 0x02)
                {
                    wControls[nControls] = IDC_MENU_RPT;
                    ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE, IDS_DELIVERYREPORTS, wstrText[nControls], sizeof(wstrText));
                    nControls++;
                }
#if !defined(FEATURE_VERSION_C316)&&!defined(FEATURE_VERSION_K202_LM129C) //xxzhen
                if (mask & 0x10)
                {
                    wControls[nControls] = IDC_MENU_CBNUM;
                    ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE, IDS_CALLBACKNUM, wstrText[nControls], sizeof(wstrText));
                    nControls++;
                }
               
                if (mask & 0x01)
                {
                    wControls[nControls] = IDC_TEXT_CBNUM;
                    nControls++;
                }
#endif 
            }			
				
            // 解决闪屏问题，从start移动到此处 
            {
                uint32 dwMask = IDIALOG_GetProperties(pMe->m_pActiveIDlg);
                ITextCtl *pIText = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_TEXT_CBNUM);
                
                dwMask |= DLG_NOT_SET_FOCUS_AUTO;
                IDIALOG_SetProperties(pMe->m_pActiveIDlg, dwMask);
                ITEXTCTL_SetProperties(pIText, TP_NOUPDATE|TP_FOCUS_NOSEL);
                
                if (NULL != pIText)
                {
                    (void)ITEXTCTL_SetInputMode(pIText, AEE_TM_NUMBERS);
                }
            }
        	}
            
            return TRUE;

        case EVT_DIALOG_START:
			{
			ITextCtl *pIText = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_TEXT_CBNUM);
			(void)ITEXTCTL_SetText(pIText, pMe->m_msSend.m_szCallBkNum,MAX_PH_DIGITS);
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
        	}
            return TRUE;

        case EVT_USER_REDRAW:
            {
                
#if defined(FEATURE_DISP_160X128)
                int  i,y = 0,dy=0,ygap = 1;	//Add By zzg 2010_07_10
#elif defined(FEATURE_DISP_220X176)
                int  i,y = 0,dy=0,ygap = 1;	//Add By zzg 2010_07_10                     
#elif defined(FEATURE_DISP_128X160)
                int  i,y = 0,dy=0,ygap = 1;	//Add By zzg 2010_07_10      
#elif defined(FEATURE_DISP_176X220)
                int  i,y = 0,dy=0,ygap = 1;	//Add By zzg 2010_07_10       
#elif defined(FEATURE_DISP_240X320)
                int  i,y = 0,dy=0,ygap = 1;	//Add By zzg 2010_07_10                 
#elif defined(FEATURE_DISP_320X240)
                int  i,y = 0,dy=0,ygap = 1;	//Add By zzg 2010_07_10                                
#else
				int  i,y = 0,dy=0,ygap = 10;
#endif
                AEERect rc;
                IImage  *pLeftImg = NULL;
                IImage  *pRightImg = NULL;
                AEEImageInfo imginfo={0};
                AEEMenuColors color; 
                IControl *pControl = NULL;
                AECHAR wstrTitle[20] = {0};
                BottomBar_e_Type    eBBarType = BTBAR_SELECT_BACK;
                int16  nLineHeight = IDISPLAY_GetFontMetrics(pMe->m_pDisplay,
                                                             AEE_FONT_NORMAL,
                                                             NULL,
                                                             NULL);				

                if(cbval == TRUE && wControls[nControls - 1] != IDC_TEXT_CBNUM)
                {
                    wControls[nControls] = IDC_TEXT_CBNUM;
                    nControls++;
                }
                else if(cbval == FALSE && wControls[nControls - 1] == IDC_TEXT_CBNUM)
                {
                    wControls[nControls - 1] = NULL;
                    nControls--;
                }
			
                //初始化List Menu的颜色及清屏
                #ifdef FEATURE_VERSION_W317A
                color.wMask = MC_SEL_TEXT | MC_SEL_BACK | MC_TEXT | MC_BACK;
                color.cBack = RGB_BLACK;
                color.cText =  0x84848400;	//RGB_BLACK;				
                color.cSelBack = RGB_BLACK; //MAKE_RGB(255,128,64);        	        			
                color.cSelText = RGB_WHITE; //RGB_WHITE;
                #else
                color.wMask = MC_SEL_TEXT | MC_SEL_BACK | MC_TEXT | MC_BACK;
                color.cBack = RGB_WHITE;
                color.cText =  0x84848400;	//RGB_BLACK;				//Add By zzg 2010_07_10	
                color.cSelBack = RGB_WHITE; //MAKE_RGB(255,128,64);      //Add By zzg 2010_07_10  	        			
                color.cSelText = RGB_BLACK; //RGB_WHITE;				//Add By zzg 2010_07_10
                #endif
                //IDISPLAY_ClearScreen(pMe->m_pDisplay);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                {
                    IImage *pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
                    
                    Appscommon_ResetBackground(pMe->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &pMe->m_rc, 0, 0);
                    if(pImageBg != NULL)
                    {
                        IImage_Release(pImageBg);
                    }
                }
#else
                Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &pMe->m_rc, TRUE);
#endif
				// 画标题条
			    {
				TitleBar_Param_type  TitleBar_Param = {0};
                    
                TitleBar_Param.pwszTitle = wstrTitle;
                TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER;
                (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                                IDS_OPTION, wstrTitle,sizeof(wstrTitle));
                #if 0
                // 画标题条
                {
                    TitleBar_Param_type  TitleBar_Param = {0};
                    
                    TitleBar_Param.pwszTitle = wstrTitle;
                    TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER;
                    (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                                IDS_OPTION, wstrTitle,sizeof(wstrTitle));
                    
                    DrawTitleBar(pMe->m_pDisplay, &TitleBar_Param);
                }
				#else
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,wstrTitle);
				#endif
				}
                
                y = y+ 3;
				
#if defined(FEATURE_DISP_160X128)
                dy = nLineHeight; // + 2; 
#elif defined(FEATURE_DISP_220X176)
                dy = nLineHeight; // + 2;                    
#elif defined(FEATURE_DISP_128X160)
                dy = nLineHeight; // + 2;                 
#elif defined(FEATURE_DISP_176X220)
                dy = nLineHeight; // + 2;    
#elif defined(FEATURE_DISP_240X320)
                dy = nLineHeight; // + 2;          
#elif defined(FEATURE_DISP_320X240)
                dy = nLineHeight; // + 2;                   
#else
				dy = nLineHeight + 2; 
#endif
                
                pLeftImg = ISHELL_LoadResImage(pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDB_LEFTARROW);
                pRightImg = ISHELL_LoadResImage(pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDB_RIGHTARROW);
                
                for (i=0; i<nControls; i++, y+=dy+ygap)
                {
                    AEERect rect;
                    int ry=0;
                    #if defined(FEATURE_VERSION_C01) || defined(FEATURE_VERSION_1110W516)||defined(FEATURE_VERSION_C11)||defined(FEATURE_VERSION_C180)||defined(FEATURE_VERSION_C100)|| defined(FEATURE_VERSION_W027)||defined(FEATURE_VERSION_W021_CT100)
                    if(i<4 && i != 2)
                    #else
                    if(i<4)
                    #endif    
                    {
                        y=y+dy+ygap;
                        ry=y-dy-ygap;
                    }
                    else
                    {
                        ry=y;
                    }
                    
                    SETAEERECT(&rect, 2, ry, pMe->m_rc.dx- 2, dy);
                    pControl = IDIALOG_GetControl(pMe->m_pActiveIDlg, wControls[i]);
                    
                    if (NULL == pControl)
                    {
                        continue;
                    }
                     //-SENDOPT_TITLELENGTH 
                    SETAEERECT(&rc, 2, y, pMe->m_rc.dx- 2, dy);  //SETAEERECT(&rc, 2, y, pMe->m_rc.dx-4, dy);

                    if((wControls[i] == IDC_MENU_CBNUM || wControls[i] == IDC_MENU_RPT))
                    {
#ifndef WIN32
                        nv_language_enum_type language;

                        (void) ICONFIG_GetItem(pMe->m_pConfig,
                                               CFGI_LANGUAGE_SELECTION,
                                               &language,
                                               sizeof(language));
                    
                        if((language != NV_LANGUAGE_CHINESE) && (language != NV_LANGUAGE_TCHINESE))
                        {
                            #ifdef FEATURE_VERSION_C01
                            rect.dx = SENDOPT_TITLELONGER+40;
                            #else
                            rect.dx = SENDOPT_TITLELONGER;
                            #endif
                            rc.x    = SENDOPT_TITLELONGER + 2;
                            rc.dx   = pMe->m_rc.dx - SENDOPT_TITLELONGER - 4;
                            
                        }
#endif
                    }
                    if (wControls[i] != IDC_TEXT_CBNUM)
                    {
                        if (pLeftImg != NULL)
                        {
                            IIMAGE_GetInfo(pLeftImg, &imginfo);
							pMe->m_lContrlRectImage[i].x = 0;
							pMe->m_lContrlRectImage[i].y = y+(dy-imginfo.cy)/2-5;
							pMe->m_lContrlRectImage[i].dx = rc.x+imginfo.cx;
							pMe->m_lContrlRectImage[i].dy = imginfo.cy+10;
                            IIMAGE_Draw(pLeftImg, rc.x, y+(dy-imginfo.cy)/2);
                            rc.x += imginfo.cx;
                            rc.dx -= imginfo.cx;
                        }
                        
                        if (pRightImg != NULL)
                        {
                            IIMAGE_GetInfo(pLeftImg, &imginfo);
							pMe->m_rContrlRectImage[i].x = rc.x+rc.dx-imginfo.cx-5;
							pMe->m_rContrlRectImage[i].y = y+(dy-imginfo.cy)/2-5;
							pMe->m_rContrlRectImage[i].dx = imginfo.cx+5;
							pMe->m_rContrlRectImage[i].dy = imginfo.cy+10;
                            IIMAGE_Draw(pRightImg, rc.x+rc.dx-imginfo.cx, y+(dy-imginfo.cy)/2);
                            rc.dx -= imginfo.cx;
                        }

                        IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
                        IDISPLAY_DrawText(pMe->m_pDisplay, 
                                        AEE_FONT_NORMAL, 
                                        wstrText[i], 
                                        -1, 2, ry, 
                                        &rect, 
                                        IDF_TEXT_TRANSPARENT);

                        IMENUCTL_SetOemProperties((IMenuCtl *)pControl, OEMMP_SWITCHNAVIGATIONKEY|
                                                                        OEMMP_DISTINGUISH_INFOKEY_SELECTKEY|
                                                                        OEMMP_IDF_ALIGN_CENTER);
                    }
                    else if(wControls[i] == IDC_TEXT_CBNUM)
                    {
                    	ITextCtl *pIText = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_TEXT_CBNUM);
                        rc.x  = 2;
                        rc.dx = pMe->m_rc.dx - rc.x - 2;
#if defined(FEATURE_DISP_160X128)						
						rc.dy += 2;	//Add By zzg 2010_07_13
#elif defined(FEATURE_DISP_220X176)						
						rc.dy += 2;	//Add By zzg 2010_07_13							
#elif defined(FEATURE_DISP_128X160)						
						rc.dy += 2;	//Add By zzg 2010_07_13		
#elif defined(FEATURE_DISP_176X220)						
						rc.dy += 2;	//Add By zzg 2010_07_13	
#elif defined(FEATURE_DISP_240X320)						
						rc.dy += 2;	//Add By zzg 2010_07_13			
#elif defined(FEATURE_DISP_320X240)						
						rc.dy += 2;	//Add By zzg 2010_07_13								
#endif
                        IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);
						MSG_FATAL("IDC_TEXT_CBNUM.........",0,0,0);
						if (NULL != pIText)
						{
							AECHAR *txtPtr = ITEXTCTL_GetTextPtr(pIText);
							
							if (NULL != txtPtr)
							{
								AECHAR	wszText[MAX_PH_DIGITS] = {0};
								DBGPRINTF("IDD_SENDOPTS_Handler txtPtr=%s", txtPtr);
								(void)ISHELL_LoadResString(pMe->m_pShell, 
											AEE_WMSAPPRES_LANGFILE,
											IDS_CALLBACKNUM,
											wszText, 
											sizeof(wszText));
								
								if (WSTRCMP(pMe->m_msSend.m_szCallBkNum, txtPtr) != 0)
								{// 用户输入有输入, 为确保获得最新输入,这里重新获取用户的输入。
									MSG_FATAL("MAX_PH_DIGITS..........",0,0,0);
									(void)ITEXTCTL_GetText(pIText, pMe->m_msSend.m_szCallBkNum, MAX_PH_DIGITS);
								}
							}
						}

                    }
                    
                    switch(wControls[i])
                    {
                        case IDC_MENU_SENDOPT:
                            //设置LIST menu控件的颜色
                            IMENUCTL_SetColors((IMenuCtl *)pControl, &color);
                            
                            MENU_SETBOTTOMBAR((IMenuCtl *)pControl, BTBAR_NONE);

                            MENU_ADDITEM((IMenuCtl *)pControl, IDS_SENDONLY);
                            MENU_ADDITEM((IMenuCtl *)pControl, IDS_SAVEONLY);
                            MENU_ADDITEM((IMenuCtl *)pControl, IDS_SENDOPT_SAVEANDSEND_SHORT);
                            
                            {// 需根据具体配置选择相应菜单项
                                uint16 nSelID = IDS_SENDOPT_SAVEANDSEND_SHORT;
                                
                                switch (sendmode)
                                {
                                    case SENDOPT_SEND://只发送
                                        nSelID = IDS_SENDONLY;
                                        break;
                                        
                                    case SENDOPT_SAVE://只保存
                                        nSelID = IDS_SAVEONLY;
                                        break;
                                        
                                    default://发送并保存
                                        sendmode = IDS_SENDOPT_SAVEANDSEND_SHORT;
                                        break;
                                }
                                IMENUCTL_SetSel((IMenuCtl *)pControl, nSelID);
                            }
                            
                            break;
                            
                        case IDC_MENU_PRI:
                            //设置LIST menu控件的颜色
                            IMENUCTL_SetColors((IMenuCtl *)pControl, &color);
                            
                            MENU_SETBOTTOMBAR((IMenuCtl *)pControl, BTBAR_NONE);
                            MENU_ADDITEM((IMenuCtl *)pControl, IDS_NORMAL);
                            MENU_ADDITEM((IMenuCtl *)pControl, IDS_URGENT);
                            
                            {// 需根据具体配置选择相应菜单项
                                uint16 nSelID = IDS_NORMAL;
                                
                                switch (priority)
                                {
                                    case WMS_PRIORITY_URGENT://紧急
                                        nSelID = IDS_URGENT;
                                        break;
                                        
                                    case WMS_PRIORITY_EMERGENCY://特急
                                        nSelID = IDS_EMERGENCY;
                                        break;
                                        
                                    default:
                                        break;
                                }
                                IMENUCTL_SetSel((IMenuCtl *)pControl, nSelID);
                            }
                            break;
                            
                        case IDC_MENU_RPT:
                            //设置LIST menu控件的颜色
                            IMENUCTL_SetColors((IMenuCtl *)pControl, &color);
                            
                            {
                                uint16 nSelID;
                                                                
                                MENU_SETBOTTOMBAR((IMenuCtl *)pControl, BTBAR_NONE);
                                
                                MENU_ADDITEM((IMenuCtl *)pControl, IDS_ENABLEREPORT_SHORT);
                                MENU_ADDITEM((IMenuCtl *)pControl, IDS_DISABLEREPORT);
                                
                                // 需根据具体配置选择相应菜单项
                                if (report)
                                {
                                    nSelID = IDS_ENABLEREPORT_SHORT;
                                }
                                else
                                {
                                    nSelID = IDS_DISABLEREPORT;
                                }
                                
                                IMENUCTL_SetSel((IMenuCtl *)pControl, nSelID);
                            }
                            break;

                        case IDC_MENU_CBNUM:
                            //设置LIST MENU的颜色
                            IMENUCTL_SetColors((IMenuCtl *)pControl, &color);

                            {
                                uint16 nSelID = IDS_ENABLE_SHORT;

                                MENU_SETBOTTOMBAR((IMenuCtl *)pControl, BTBAR_NONE);

                                MENU_ADDITEM((IMenuCtl *)pControl, IDS_ENABLE_SHORT);
                                MENU_ADDITEM((IMenuCtl *)pControl, IDS_DISABLE);

                                if(cbval)
                                {
                                    nSelID = IDS_ENABLE_SHORT;
                                }
                                else
                                {
                                    nSelID = IDS_DISABLE;
                                }

                                IMENUCTL_SetSel((IMenuCtl *)pControl, nSelID);
                            }
                            break;
                            
                        case IDC_TEXT_CBNUM:
                            ITEXTCTL_SetProperties((ITextCtl *)pControl, TP_FIXSETRECT|TP_FIXOEM|TP_FOCUS_NOSEL);
                            ITEXTCTL_SetMaxSize((ITextCtl *)pControl, MAX_PH_DIGITS - 1);
                            
                            if (WSTRLEN(pMe->m_msSend.m_szCallBkNum) == 0)
                            {
                                AECHAR  wszText[MAX_PH_DIGITS] = {0};
                                
                                if (pMe->m_wPrevMenuSel != IDC_TEXT_CBNUM)
                                {
                                    (void)ISHELL_LoadResString(pMe->m_pShell, 
                                                AEE_WMSAPPRES_LANGFILE,
                                                IDS_CALLBACKNUM,
                                                wszText, 
                                                sizeof(wszText));
                                }
                                else
                                {
                                    eBBarType = BTBAR_BACK;
                                }
                                            
                                (void)ITEXTCTL_SetText((ITextCtl *)pControl, wszText, -1);
                            }
                            else
                            {
                                (void)ITEXTCTL_SetText((ITextCtl *)pControl, pMe->m_msSend.m_szCallBkNum, -1);
                                
                                if (pMe->m_wPrevMenuSel == IDC_TEXT_CBNUM)
                                {
                                    eBBarType = BTBAR_SELECT_BACK;
                                }
                            }
                            ITEXTCTL_SetCursorPos((ITextCtl *)pControl, TC_CURSOREND);
                            break;
                            
                        default:
                            break;
                    }
                    ICONTROL_SetRect(pControl, &rc);
                    ICONTROL_SetActive(pControl,TRUE);
                    if (wControls[i])
                    {
                        IDIALOG_SetFocus(pMe->m_pActiveIDlg, wControls[i]);
                    }
                    ICONTROL_Redraw(pControl);
                    ICONTROL_SetActive(pControl,FALSE);
                }

				/*
				//Add By zzg 2010_07_13
#if defined(FEATURE_DISP_160X128)
				{
					int         titleBarHeight      = 0;
				    int         bottomBarHeight     = 0;
				    int         itemNumberPerPage   = 0;				   
					AEERect     rectWindow       	= {0};
					int         fontHeight          = 0;
					int         itemHeight          = 0;
					int         currentItem         = 0;	
					int     	y              	 	= 0;	
		            int     	height          	= 0;
					fontHeight          = IDISPLAY_GetFontMetrics( pMe->m_pDisplay, AEE_FONT_NORMAL, 0, 0);
					itemHeight          = fontHeight + 5;
					titleBarHeight      = GetTitleBarHeight( pMe->m_pDisplay);
			        bottomBarHeight     = GetBottomBarHeight( pMe->m_pDisplay);
					SETAEERECT( &rectWindow, 0, titleBarHeight, pMe->m_rc.dx, pMe->m_rc.dy - titleBarHeight - bottomBarHeight);
					itemNumberPerPage   = (pMe->m_rc.dy - titleBarHeight - bottomBarHeight) / itemHeight;

					switch (pMe->m_wPrevMenuSel)
					{
						case IDC_MENU_SENDOPT:
						{
							currentItem = 0;
							break;
						}
						case IDC_MENU_PRI:
						{
							currentItem = 1;
							break;
						}
						case IDC_MENU_RPT:
						{
							currentItem = 2;
							break;
						}
						case IDC_MENU_CBNUM:
						{
							currentItem = 3;
							break;
						}
						case IDC_TEXT_CBNUM:
						{
							currentItem = 4;
							break;
						}
						default:
						{
							break;
						}
					}

						
					// draw scroll bar	
					if( itemNumberPerPage < nControls)
					{
#ifdef FEATURE_SCROLLBAR_USE_STYLE
												
						static Theme_Param_type    themeParms       = {0};

		                RGBVAL  ScrollbarClr = MAKE_RGB(0xDE, 0xDE, 0xDE),
		                        ScrollbarFillClr = MAKE_RGB(0xFF, 0x70, 0x00);
#endif
						
		                height  = (rectWindow.dy / nControls) * nControls;
		                y       = rectWindow.y + ( rectWindow.dy - height) / 2;		

		                
#ifdef FEATURE_SCROLLBAR_USE_STYLE

						Appscom_GetThemeParameters( &themeParms);

		                SETAEERECT(&rc, pMe->m_rc.dx - (SCROLLBAR_WIDTH - 2), y, (SCROLLBAR_WIDTH - 2), height);
		                IDISPLAY_FillRect(pMe->m_pDisplay, &rc, ScrollbarClr);
		                rc.dy  = rc.dy / nControls;
		                rc.y  += rc.dy * currentItem;
		                IDISPLAY_FillRect(pMe->m_pDisplay, &rc, ScrollbarFillClr);
#else
		                SETAEERECT(&rc, pMe->m_rc.dx - 1, y, 1, height);
		                IDISPLAY_FillRect( pMe->m_pDisplay, &rc, themeParms.textColor);
		                rc.dy  = rc.dy / nControls;
		                rc.y  += rc.dy * currentItem;
		                IDISPLAY_FillRect(pMe->m_pDisplay, &rc, themeParms.themeColor);
#endif                
					}
				}
#endif
				//Add End
				*/
                
                if (pLeftImg != NULL)
                {
                    IIMAGE_Release(pLeftImg);
                }
                
                if (pRightImg != NULL)
                {
                    IIMAGE_Release(pRightImg);
                }
                
                IDIALOG_SetFocus(pMe->m_pActiveIDlg, pMe->m_wPrevMenuSel);
                pControl = IDIALOG_GetControl(pMe->m_pActiveIDlg, pMe->m_wPrevMenuSel);
                if (NULL != pControl)
                {
                    ICONTROL_SetActive(pControl,TRUE);
                }
                
                // 绘制底条提示
                DRAW_BOTTOMBAR(eBBarType)
                
                // 更新界面
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            }

            return TRUE;

        case EVT_DIALOG_END:
            {
                ITextCtl *pIText = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_TEXT_CBNUM);
                
                if (NULL != pIText)
                {
                    AECHAR *txtPtr = ITEXTCTL_GetTextPtr(pIText);
                    
                    if (NULL != txtPtr)
                    {
                        AECHAR  wszText[MAX_PH_DIGITS] = {0};
                        MSG_FATAL("IDD_SENDOPTS_Handler txtPtr=%s", txtPtr,0,0);
                        (void)ISHELL_LoadResString(pMe->m_pShell, 
                                    AEE_WMSAPPRES_LANGFILE,
                                    IDS_CALLBACKNUM,
                                    wszText, 
                                    sizeof(wszText));
                        
                        if (WSTRCMP(wszText, txtPtr) != 0)
                        {// 用户输入有输入, 为确保获得最新输入,这里重新获取用户的输入。
                        MSG_FATAL("MAX_PH_DIGITS..........",0,0,0);
						(void)ITEXTCTL_GetText(pIText, pMe->m_msSend.m_szCallBkNum, MAX_PH_DIGITS);
                        }
                    }
                }
            }
            return TRUE;
            
        case EVT_KEY_RELEASE:
            {
                ITextCtl * pIText = (ITextCtl *)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_TEXT_CBNUM);
                
                if ((NULL != pIText) &&
                    (ITEXTCTL_IsActive(pIText)))
                {
                    AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pIText);
                    int nLen=0;
                    
                    if (NULL != pwstrText)
                    {
                        nLen = WSTRLEN(pwstrText);
                    }
                    
                    if (nLen == 1)
                    {
                    	#ifndef FEATURE_ALL_KEY_PAD
                        // Send       Earse
                        DRAW_BOTTOMBAR(BTBAR_OK_DELETE)
                        #else
                        // Send       Back
                        DRAW_BOTTOMBAR(BTBAR_OK_BACK)
                        #endif
                        IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    }
                    else if (nLen <= 0)
                    {
                        // Search      Back
                        DRAW_BOTTOMBAR(BTBAR_BACK)
                        IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    }
                }
            }
            
            return TRUE;

		/*	
		//Add By zzg 2010_07_13
		case EVT_KEY_PRESS:
        {    
			switch (wParam)
			{
				case AVK_UP:
				case AVK_DOWN:
				{
					(void) ISHELL_PostEventEx(pMe->m_pShell, 
		                                    EVTFLG_ASYNC,
		                                    AEECLSID_WMSAPP,
		                                    EVT_USER_REDRAW,
		                                    0, 
		                                    0);
	            	return TRUE;
				}
				default:
					return TRUE;
			}
		}
		//Add End	
		*/

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:                 
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                    
                case AVK_INFO:
                    {
                        pMe->m_SendOPT = sendmode;
                        pMe->m_msSend.m_epriority = priority;
                        pMe->m_msSend.m_bDeliveryReport  = report;
                        pMe->m_CbVal  = cbval;                               
                    }
                    CLOSE_DIALOG(DLGRET_OK)
                    break;
  
                case AVK_SELECT:                   
                    {// 此时焦点必定落在文本控件上
                        AECHAR *txtPtr;
                        ITextCtl *pIText = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_TEXT_CBNUM);
                        
                        if (NULL == pIText)
                        {
                            return TRUE;
                        }
                        
                        txtPtr = ITEXTCTL_GetTextPtr(pIText);
                        if ((NULL == txtPtr) || (WSTRLEN(txtPtr) == 0))
                        {// 用户意欲查询电话本输入号码
                            // 调用通信簿群发取号码接口，本Applet 会被挂起，返回时回到当前状态
                            //WMSExtApp_GetAddresseeFromContactsApp(pMe, GETADDR_ONEADD);
                            return TRUE;
                        }
                        else
                        {
                            CLOSE_DIALOG(DLGRET_OK)
                        }
                    }                 
                    break;	
				case AVK_LEFT:
				case AVK_RIGHT:
						{
							IControl *pControl = NULL;
							
						}
                default:
                    break;
            }
            return TRUE;

        case EVT_CTL_SEL_CHANGED:
            switch (wParam)
            {
                case IDS_SENDONLY:
                    sendmode = SENDOPT_SEND;
                    break;
                    
                case IDS_SAVEONLY:
                    sendmode = SENDOPT_SAVE;
                    break;

                case IDS_SENDOPT_SAVEANDSEND_SHORT:
                	sendmode = SENDOPT_SAVEANDSEND;
                    break;
                    
                case IDS_NORMAL:
                    priority = WMS_PRIORITY_NORMAL;
                    break;
                    
                case IDS_URGENT:
                    priority = WMS_PRIORITY_URGENT;
                    break;
                    
                case IDS_EMERGENCY:
                    priority = WMS_PRIORITY_EMERGENCY;
                    break;
                    
                case IDS_ENABLEREPORT_SHORT:
                    report = TRUE;
                    break;
                    
                case IDS_DISABLEREPORT:
                    report = FALSE;
                    break;

                case IDS_ENABLE_SHORT:
                    cbval = TRUE;
                    (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                            EVTFLG_ASYNC,
                                            AEECLSID_WMSAPP,
                                            EVT_USER_REDRAW,
                                            0, 
                                            0);

                    break;

                case IDS_DISABLE:
                    cbval = FALSE;
                    (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                            EVTFLG_ASYNC,
                                            AEECLSID_WMSAPP,
                                            EVT_USER_REDRAW,
                                            0, 
                                            0);
                    break;
                    
                default:
                    break;
            }
            return TRUE;
            
        case EVT_CTL_TAB:
            {
                int i;
                BottomBar_e_Type    eBBarType = BTBAR_SELECT_BACK;
                IControl *pControl = NULL;
                uint16    wNextFocus = 0;
                
                if (nControls < 2)
                {
                    return TRUE;
                }
                
                for (i=0; i<nControls; i++)
                {
                    pControl = IDIALOG_GetControl(pMe->m_pActiveIDlg, wControls[i]);
                    
                    if ((NULL == pControl) || ((IControl *)dwParam != pControl))
                    {
                        continue;
                    }
                    
                    if (wParam<1)
                    {
                        if (i == 0)
                        {
                            wNextFocus = wControls[nControls-1];
                        }
                        else
                        {
                            wNextFocus = wControls[i-1];
                        }
                    }
                    else
                    {
                        if (i == (nControls-1))
                        {
                            wNextFocus = wControls[0];
                        }
                        else
                        {
                            wNextFocus = wControls[i+1];
                        }
                    }
                    if (wNextFocus != 0)
                    {
                        pMe->m_wPrevMenuSel = wNextFocus;
                    }
                    
                    switch (wControls[i])
                    {
                        case IDC_TEXT_CBNUM:
                            // 文本控件失去焦点
                            (void)ITEXTCTL_GetText((ITextCtl *)pControl, pMe->m_msSend.m_szCallBkNum, MAX_PH_DIGITS);
                            if (WSTRLEN(pMe->m_msSend.m_szCallBkNum) == 0)
                            {
                                AECHAR  wszText[MAX_PH_DIGITS] = {0};
                                
                                (void)ISHELL_LoadResString(pMe->m_pShell, 
                                            AEE_WMSAPPRES_LANGFILE,
                                            IDS_CALLBACKNUM,
                                            wszText, 
                                            sizeof(wszText));
                                            
                                (void)ITEXTCTL_SetText((ITextCtl *)pControl, wszText, -1);
                            }
                            break;
                            
                        default:                            
                            break;
                    }
                    
                    ICONTROL_SetActive(pControl,FALSE);
                    IDIALOG_SetFocus(pMe->m_pActiveIDlg, pMe->m_wPrevMenuSel);
                            
                    pControl = IDIALOG_GetControl(pMe->m_pActiveIDlg, pMe->m_wPrevMenuSel);
                    if (NULL != pControl)
                    {
                        ICONTROL_SetActive(pControl,TRUE);
                        
                        if (pMe->m_wPrevMenuSel == IDC_TEXT_CBNUM)
                        {// 文本控件获得焦点
                            if (WSTRLEN(pMe->m_msSend.m_szCallBkNum) == 0)
                            {
                                ITEXTCTL_Reset((ITextCtl *)pControl);
                                ICONTROL_SetActive(pControl,TRUE);
                                IDIALOG_SetFocus(pMe->m_pActiveIDlg, pMe->m_wPrevMenuSel);
                                
                                // 绘制底条提示
                                // Search       Back
                                eBBarType = BTBAR_BACK;
                            }
                            else
                            {
                                (void)ITEXTCTL_SetText((ITextCtl *)pControl, pMe->m_msSend.m_szCallBkNum, -1);
                                #ifndef FEATURE_ALL_KEY_PAD
                                // 绘制底条提示
                                // Send       Earse
                                eBBarType = BTBAR_OK_DELETE;
                                #else
                                // Send       Back
                                eBBarType = BTBAR_OK_BACK;
                                #endif
                            }
                            
                            ITEXTCTL_SetCursorPos((ITextCtl *)pControl, TC_CURSOREND);
                            (void)ITEXTCTL_SetInputMode((ITextCtl *)pControl, AEE_TM_NUMBERS);
                        }
                        ICONTROL_Redraw(pControl);
                        
                        // 绘制底条提示
                        DRAW_BOTTOMBAR(eBBarType)
                        
                        // 更新界面
                        IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    }
                    
                    break;
                }
            }
            return TRUE;
        
        case EVT_COMMAND:
            {
                pMe->m_SendOPT = sendmode;
                pMe->m_msSend.m_epriority = priority;
                pMe->m_msSend.m_bDeliveryReport  = report;
                pMe->m_CbVal  = cbval;                               
            }
#ifndef FEATURE_LCD_TOUCH_ENABLE        
            CLOSE_DIALOG(DLGRET_OK)
#endif           
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch
        case EVT_PEN_UP:
            {
                AEEDeviceInfo devinfo;
                int nBarH ;
                AEERect rc;
                int16 wXPos = (int16)AEE_GET_X(dwParam);
                int16 wYPos = (int16)AEE_GET_Y(dwParam);
				int i = 0;
                IControl *pControl = NULL;
                nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
                MEMSET(&devinfo, 0, sizeof(devinfo));
                ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
                SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

                if(WMSAPP_PT_IN_RECT(wXPos,wYPos,rc))
                {
                    if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
                    {
                        boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_INFO,0);
                        return rt;
                    }
                    else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
                    {
                         boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_SELECT,0);
                         return rt;
                    }
                    else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
                    {                   
                        boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_CLR,0);
                        return rt;
                    }
                }
				MSG_FATAL("wXPos===%d,wYPos====%d",wXPos,wYPos,0);
				MSG_FATAL("wXPos===%d,wYPos====%d",pMe->m_lContrlRectImage[0].x,pMe->m_lContrlRectImage[0].y,0);
				MSG_FATAL("wXPos===%d,wYPos====%d",pMe->m_lContrlRectImage[0].dx,pMe->m_lContrlRectImage[0].dy,0);
                MSG_FATAL("wXPos===%d,wYPos====%d",pMe->m_rContrlRectImage[0].x,pMe->m_rContrlRectImage[0].y,0);
				MSG_FATAL("wXPos===%d,wYPos====%d",pMe->m_rContrlRectImage[0].dx,pMe->m_rContrlRectImage[0].dy,0);
				MSG_FATAL("wXPos===%d,wYPos====%d",pMe->m_lContrlRectImage[1].x,pMe->m_lContrlRectImage[1].y,0);
				MSG_FATAL("wXPos===%d,wYPos====%d",pMe->m_lContrlRectImage[1].dx,pMe->m_lContrlRectImage[1].dy,0);
				MSG_FATAL("wXPos===%d,wYPos====%d",pMe->m_lContrlRectImage[2].x,pMe->m_lContrlRectImage[2].y,0);
				MSG_FATAL("wXPos===%d,wYPos====%d",pMe->m_lContrlRectImage[2].dx,pMe->m_lContrlRectImage[2].dy,0);
				for(i=0;i<4;i++)
				{
					switch(i)
					{
						case 0:
							if(WMSAPP_PT_IN_RECT(wXPos,wYPos,pMe->m_lContrlRectImage[0]))
							{
                                pControl = IDIALOG_GetControl(pMe->m_pActiveIDlg, wControls[i]);
                                IMENUCTL_HandleEvent((IMenuCtl *)pControl,EVT_KEY,AVK_LEFT,0);
                                return TRUE;
							}
							else if(WMSAPP_PT_IN_RECT(wXPos,wYPos,pMe->m_rContrlRectImage[0]))
							{
                                pControl = IDIALOG_GetControl(pMe->m_pActiveIDlg, wControls[i]);
                                IMENUCTL_HandleEvent((IMenuCtl *)pControl,EVT_KEY,AVK_RIGHT,0);
								return TRUE;
							}
							break;
						case 1:
							if(WMSAPP_PT_IN_RECT(wXPos,wYPos,pMe->m_lContrlRectImage[1]))
							{
                                pControl = IDIALOG_GetControl(pMe->m_pActiveIDlg, wControls[i]);
                                IMENUCTL_HandleEvent((IMenuCtl *)pControl,EVT_KEY,AVK_LEFT,0);
								return TRUE;
							}
							else if(WMSAPP_PT_IN_RECT(wXPos,wYPos,pMe->m_rContrlRectImage[1]))
							{
                                pControl = IDIALOG_GetControl(pMe->m_pActiveIDlg, wControls[i]);
                                IMENUCTL_HandleEvent((IMenuCtl *)pControl,EVT_KEY,AVK_RIGHT,0);
								return TRUE;
							}
							break;
						case 2:
							if(WMSAPP_PT_IN_RECT(wXPos,wYPos,pMe->m_lContrlRectImage[2]))
							{
								pControl = IDIALOG_GetControl(pMe->m_pActiveIDlg, wControls[i]);
                                IMENUCTL_HandleEvent((IMenuCtl *)pControl,EVT_KEY,AVK_LEFT,0);
								return TRUE;
							}
							else if(WMSAPP_PT_IN_RECT(wXPos,wYPos,pMe->m_rContrlRectImage[2]))
							{
								pControl = IDIALOG_GetControl(pMe->m_pActiveIDlg, wControls[i]);
                                IMENUCTL_HandleEvent((IMenuCtl *)pControl,EVT_KEY,AVK_RIGHT,0);
								return TRUE;
							}
							break;
						case 3:
							if(WMSAPP_PT_IN_RECT(wXPos,wYPos,pMe->m_lContrlRectImage[3]))
							{
								pControl = IDIALOG_GetControl(pMe->m_pActiveIDlg, wControls[i]);
                                IMENUCTL_HandleEvent((IMenuCtl *)pControl,EVT_KEY,AVK_LEFT,0);
								return TRUE;
							}
							else if(WMSAPP_PT_IN_RECT(wXPos,wYPos,pMe->m_rContrlRectImage[3]))
							{
								pControl = IDIALOG_GetControl(pMe->m_pActiveIDlg, wControls[i]);
                                IMENUCTL_HandleEvent((IMenuCtl *)pControl,EVT_KEY,AVK_RIGHT,0);
								return TRUE;
							}
							break;
						default:
							break;
					}
				}

            }
            break;
#endif                
    
        default:
            break;
    }

    return FALSE;
} // IDD_SENDOPTS_Handler


/*==============================================================================
函数:
    IDD_WRITEMSG_Handler

说明:
    WMS Applet对话框 IDD_WRITEMSG 事件处理函数。用于短消息文本输处理。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
extern char charsvc_p_name [UIM_CDMA_HOME_SERVICE_SIZE+1];
static boolean IDD_WRITEMSG_Handler(void *pUser, 
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
)
{
    ITextCtl *pIText = NULL;   
    AEETextInputMode nInputMode;
    static AEETextInputMode nMode;
#ifdef FEATURE_USES_MMS    
    static boolean mmsTotalSizeBig = FALSE;
#endif
	AECHAR Annstr[20] = {0};
    WmsApp *pMe = (WmsApp *)pUser;
    boolean m_Issetmod = FALSE;
    IMenuCtl *pMenuCtl = NULL;
    uint32 dwMask;
    int m_WmsMaxSize = WMS_MSG_MAXCHARS;
    
    DBGPRINTF("IDD_WRITEMSG_Handler charsvc_p_name=%s\n",charsvc_p_name);

#ifdef FEATURE_VERSION_W021_CT100_RELIANCE
  //  if(STRISTR (charsvc_p_name,"Reliance"))
  //  {        
  //     DBGPRINTF("IDD_WRITEMSG_Handler m_WmsMaxSize=%d\n",m_WmsMaxSize);
  //      m_WmsMaxSize = 160;
  //  }
#endif    
    
    MSG_FATAL("IDD_WRITEMSG_Handler Start eCode=0x%x, wParam=0x%x",eCode,wParam,0);
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    if (pMe->m_pMenu != NULL)
    {
        if (IMENUCTL_HandleEvent(pMe->m_pMenu, eCode, wParam, dwParam))
        {
            return TRUE;
        }
    }
     
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveIDlg, IDC_WRITEMSG_MENU);
    
#ifdef FEATURE_USES_MMS 
    if(NULL == pMenuCtl)
    {
        MSG_FATAL("IDD_WRITEMSG_Handler Start NULL == pMenuCtl",0,0,0);
        return FALSE;
    }
#endif

#ifndef FEATURE_USES_MMS
    {
        //如果不给这个菜单控件设置坐标，那么在触摸事件中，事件会乱传，有时会传到MENU菜单中去
        //只有把这控件的坐标设到屏幕外面去，才可以避免这种情况
        AEERect rc={0};
        rc.y = -1; 
        rc.dy = 1;
        rc.x = -1;
        rc.dx = 1;
        MSG_FATAL("IDD_WRITEMSG_Handler rc.x=%d, rc.y=%d,rc.dy=%d", rc.x, rc.y, rc.dy);
        if(pMenuCtl != NULL)
        {
            IMENUCTL_SetRect(pMenuCtl, &rc);
        }
    }
#endif    
    MSG_FATAL("IDD_WRITEMSG_Handler Start 2",0,0,0);
    pIText = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_WRITEMSG_TEXT);

    if (NULL == pIText)
    {
        return FALSE;
    }
    MSG_FATAL("IDD_WRITEMSG_Handler Start 3",0,0,0);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        	{
        		boolean Is_notend = TRUE;
#ifndef FEATURE_USES_MMS                  
                IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
#if defined FEATURE_CARRIER_THAILAND_HUTCH || defined FEATURE_CARRIER_THAILAND_CAT
                ITEXTCTL_SetProperties(pIText, TP_GRAPHIC_BG|TP_FRAME | TP_MULTILINE | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT|TP_FOCUS_NOSEL);
#else
#ifdef FEATURE_VERSION_C316
				ITEXTCTL_SetProperties(pIText, TP_GRAPHIC_BGBLUE|TP_GRAPHIC_BG|TP_FRAME | TP_MULTILINE | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT | TP_DISPLAY_SMSCOUNT | TP_NOUPDATE|TP_FOCUS_NOSEL);
#else
                ITEXTCTL_SetProperties(pIText, TP_GRAPHIC_BG|TP_FRAME | TP_MULTILINE | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT | TP_DISPLAY_SMSCOUNT | TP_NOUPDATE|TP_FOCUS_NOSEL);
#endif
#endif
                SetControlRect(pMe, pIText);
#endif
                dwMask = IDIALOG_GetProperties(pMe->m_pActiveIDlg); 
                dwMask |= DLG_NOT_SET_FOCUS_AUTO;
                IDIALOG_SetProperties(pMe->m_pActiveIDlg, dwMask);  

#ifdef FEATURE_USES_MMS    
                {
                    char MMSImagepszPath[70];
                    char MMSSoundpszPath[70];
                    char MMSVideopszPath[70];
                    CtlAddItem ai;
                    AEERect rc={0};
                    AEEDeviceInfo devinfo={0};
                    MSG_FATAL("IDD_WRITEMSG_Handler EVT_DIALOG_INIT m_insertMMSType=%d",pMe->m_insertMMSType,0,0);
                    pMe->m_isSendToAlbumOrEmain = FALSE;             
                    pMe->m_pMMSMenuHasFocus = FALSE;
                    ICONFIG_GetItem(pMe->m_pConfig, CFGI_MMSIMAGE,MMSImagepszPath, sizeof(MMSImagepszPath));
                    ICONFIG_GetItem(pMe->m_pConfig, CFGI_MMSSOUND,MMSSoundpszPath, sizeof(MMSSoundpszPath));
                    ICONFIG_GetItem(pMe->m_pConfig, CFGI_MMSVIDEO,MMSVideopszPath, sizeof(MMSVideopszPath));
                    DBGPRINTF("MMSImagepszPath=%s len=%d", MMSImagepszPath, STRLEN(MMSImagepszPath));
                    DBGPRINTF("MMSSoundpszPath=%s len=%d", MMSSoundpszPath, STRLEN(MMSSoundpszPath));
                    DBGPRINTF("MMSVideopszPath=%s len=%d", MMSVideopszPath, STRLEN(MMSVideopszPath));
                    pMe->m_wSelectStore = 1;
                    if( (STRLEN(MMSImagepszPath) != 0) || (STRLEN(MMSSoundpszPath) != 0) || (STRLEN(MMSVideopszPath) != 0))
                    {
                        uint8 ImageIndex = IMAGE_MENU_INDEX;
                        uint8 SoundIndex = SOUND_MENU_INDEX;
                        uint8 VideoIndex = VIDEO_MENU_INDEX;
                        char* pszBasename = NULL;
                        AECHAR FileName[30];
                        
                        IFile  *pFile;
                        FileInfo fileInfo;
                        IFileMgr* pFileMgr = NULL;
                        int mmsTotalSize = 0;
                        int result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pFileMgr);
                        if(result != SUCCESS)
                        {
                            return FALSE;
                        }
                        if(STRLEN(MMSImagepszPath) != 0)
                        {
                            MSG_FATAL("IDD_WRITEMSG_Handler Image too big",0,0,0);
                            pFile = IFILEMGR_OpenFile(pFileMgr, MMSImagepszPath, _OFM_READ);
                            if(pFile != NULL)
                            {
                                IFILE_GetInfo(pFile, &fileInfo);
                                mmsTotalSize += fileInfo.dwSize;
                                MSG_FATAL("IDD_WRITEMSG_Handler Image too big %d",mmsTotalSize,0,0);
                                if((pMe->m_insertMMSType == 0) && (mmsTotalSize > 380*1024))//最大可以为400K,留20K给文本
                                {
                                    char pszPath[50]={'\0'};
                                    DBGPRINTF("MMSImageName=%s len=%d", pszPath, STRLEN(pszPath));
                                    ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSIMAGE,pszPath, sizeof(pszPath));   
                                    RELEASEIF(pFile);                        
                                    MEMSET(MMSImagepszPath, 0, sizeof(MMSImagepszPath));
                                    mmsTotalSizeBig = TRUE;    
                                }
                                RELEASEIF(pFile);
                            }
                        }
                        if(STRLEN(MMSSoundpszPath) != 0)
                        {
                            MSG_FATAL("IDD_WRITEMSG_Handler Sound too big",0,0,0);
                            pFile = IFILEMGR_OpenFile(pFileMgr, MMSSoundpszPath, _OFM_READ);
                            if(pFile != NULL)
                            {
                                IFILE_GetInfo(pFile, &fileInfo);
                                mmsTotalSize += fileInfo.dwSize;
                                MSG_FATAL("IDD_WRITEMSG_Handler Sound too big mmsTotalSize=%d",mmsTotalSize,0,0);
                                if((pMe->m_insertMMSType == 1) && (mmsTotalSize > 380*1024))
                                {
                                    char pszPath[50]={'\0'};
                                    DBGPRINTF("MMSSoundpszPath=%s len=%d", MMSSoundpszPath, STRLEN(MMSSoundpszPath));
                                    ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSSOUND,pszPath, sizeof(pszPath)); 
                                    MSG_FATAL("IDD_WRITEMSG_Handler mms size too big",0,0,0);
                                    RELEASEIF(pFile);                        
                                    MSG_FATAL("IDD_WRITEMSG_Handler mms size too big 1",0,0,0);
                                    MEMSET(MMSSoundpszPath, 0, sizeof(MMSSoundpszPath));
                                    mmsTotalSizeBig = TRUE;
                                }
                                RELEASEIF(pFile);
                            }
                        }
                        
                        if(STRLEN(MMSVideopszPath) != 0)
                        {
                            pFile = IFILEMGR_OpenFile(pFileMgr, MMSVideopszPath, _OFM_READ);
                            if(pFile != NULL)
                            {
                                IFILE_GetInfo(pFile, &fileInfo);
                                mmsTotalSize += fileInfo.dwSize;
                                if((pMe->m_insertMMSType == 2) && (mmsTotalSize > 380*1024))
                                {
                                    char pszPath[50]={'\0'};
                                    DBGPRINTF("MMSImageName=%s len=%d", pszPath, STRLEN(pszPath));
                                    ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSVIDEO,pszPath, sizeof(pszPath));   
                                    MSG_FATAL("IDD_WRITEMSG_Handler mms size too big",0,0,0);
                                    RELEASEIF(pFile);                        
                                    RELEASEIF(pFileMgr);             
                                    MSG_FATAL("IDD_WRITEMSG_Handler mms size too big 1",0,0,0);
                                    MEMSET(MMSVideopszPath, 0, sizeof(MMSVideopszPath));
                                    mmsTotalSizeBig = TRUE;
                                }
                            }
                        }
                        RELEASEIF(pFile);                        
                        RELEASEIF(pFileMgr);

                        rc = pMe->m_rc;
                        pMe->m_isMMS = TRUE;
                        ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
						
                        //rc.y = 0; 
                        //rc.dy = devinfo.cyScreen;
                        //rc.dy -= GetBottomBarHeight(pMe->m_pDisplay); 

						//Modify by zzg 2012_01_12
                        rc.y = GetBottomBarHeight(pMe->m_pDisplay)/2 - 2;	
						rc.dy -= rc.y;
						//Modify end
                          
                        MSG_FATAL("IDD_WRITEMSG_Handler rc.x=%d, rc.y=%d,rc.dy=%d", rc.x, rc.y, rc.dy);
                        IMENUCTL_SetRect(pMenuCtl, &rc);
                        IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL| OEMMP_USE_MENU_INFO_SELECT);
                        IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY|OEMMP_USE_MENU_STYLE);

						//Add by zzg 2012_01_12
						rc.y = 0; 			
						rc.dy = devinfo.cyScreen;
                        rc.dy -= GetBottomBarHeight(pMe->m_pDisplay); 
                        //Add End
                		
                        // Clear items
                        (void)IMENUCTL_DeleteAll(pMenuCtl);                
                        // Init items
                        MEMSET(&ai, 0, sizeof(ai));

                        ai.pszResText = CONTAPP_RES_FILE_LANG;
                        ai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;

                        ai.wItemID   = pMe->m_wSelectStore;
                        ai.wImage    = IDB_HOMENUM;
                              
                        if(FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
                        {
                            MSG_FATAL("Failed to Add Opts item %d", ai.wItemID,0,0);
                            return EFAILED;
                        }  
						
                        IMENUCTL_SetItemText(pMenuCtl, pMe->m_wSelectStore, AEE_WMSAPPRES_LANGFILE, IDS_PHONE, NULL);
                        MSG_FATAL("pMe->m_wSelectStore=%d", pMe->m_wSelectStore,0,0);
                        //IMENUCTL_SetItemText(pMenuCtl, pMe->m_wSelectStore++, NULL, 0, L"homenum");
                          
                        if(STRLEN(MMSImagepszPath) != 0)
                        {
                            pMe->m_pMMSImage = ISHELL_LoadImage(pMe->m_pShell,MMSImagepszPath);                            
                            if(pMe->m_pMMSImage != NULL)
                            {
                                pszBasename = BASENAME(MMSImagepszPath);
                                STRTOWSTR(pszBasename, FileName, sizeof(FileName));
                                //IIMAGE_SetParm(pMe->m_pMMSImage,IPARM_SCALE, pMe->m_rc.dx/2, (devinfo.cyScreen-GetBottomBarHeight(pMe->m_pDisplay))/2);
                                IIMAGE_SetParm(pMe->m_pMMSImage,IPARM_SCALE, 30, 30);
                                MEMSET(&ai, 0, sizeof(ai));
                               // ai.pText = L"Image";
                                ai.wItemID   = ImageIndex;
                                ai.pImage = pMe->m_pMMSImage;
                                if(FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
                                {
                                   MSG_FATAL("Failed to Add Opts item %d", ai.wItemID,0,0);
                                   return EFAILED;
                                }
                                MSG_FATAL("ImageIndex=%d", ImageIndex,0,0);
                               // IMENUCTL_SetItemText(pMenuCtl, ImageIndex++, NULL, 0, pMe->m_msSend.m_szMessage);
                                IMENUCTL_SetItemText(pMenuCtl, ImageIndex++, NULL, 0, FileName);
                                RELEASEIF(pMe->m_pMMSImage);
                                pMe->m_hasImage = TRUE;
                                pszBasename = NULL;
                                MEMSET(FileName, 0, sizeof(FileName));
                                AddMimeResIntoMms(pMe,MMSImagepszPath);
                                MSG_FATAL("m_pMMSImage != NULL,dy=%d, pMe->m_rc.dy=%d",pMe->m_rc.dy/2,pMe->m_rc.dy,0);
                            }                          
                        }
                        if(STRLEN(MMSSoundpszPath) != 0)
                        {
                            pszBasename = BASENAME(MMSSoundpszPath);
                            STRTOWSTR(pszBasename, FileName, sizeof(FileName));                        
                            pMe->m_pMMSSOUND = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MUSIC);           
                            //IIMAGE_SetParm(pMe->m_pMMSSOUND,IPARM_SCALE, pMe->m_rc.dx/2, (devinfo.cyScreen-GetBottomBarHeight(pMe->m_pDisplay))/2);      
                            IIMAGE_SetParm(pMe->m_pMMSSOUND,IPARM_SCALE, 30, 30);                        
                            MEMSET(&ai, 0, sizeof(ai));
                            MEMSET(&ai, 0, sizeof(ai));
                            //ai.pText = L"Sound";
                            ai.wItemID   = SoundIndex;
                            ai.pImage = pMe->m_pMMSSOUND;
                            if(FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
                            {
                               MSG_FATAL("Failed to Add Opts item %d", ai.wItemID,0,0);
                               return EFAILED;
                            }                   
                            MSG_FATAL("SoundIndex=%d", SoundIndex,0,0);
                            IMENUCTL_SetItemText(pMenuCtl, SoundIndex++, NULL, 0, FileName);
                            RELEASEIF(pMe->m_pMMSSOUND);
                            pMe->m_hasSound = TRUE;
                            pszBasename = NULL;
                            MEMSET(FileName, 0, sizeof(FileName));
                            AddMimeResIntoMms(pMe,MMSSoundpszPath);
                        }
                        if(STRLEN(MMSVideopszPath) != 0)
                        {
                            pszBasename = BASENAME(MMSVideopszPath);
                            STRTOWSTR(pszBasename, FileName, sizeof(FileName));                         
                            pMe->m_pMMSVIDEO = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_VIDEO);           
                            //IIMAGE_SetParm(pMe->m_pMMSVIDEO,IPARM_SCALE, pMe->m_rc.dx/2, (devinfo.cyScreen-GetBottomBarHeight(pMe->m_pDisplay))/2);                                                
                            IIMAGE_SetParm(pMe->m_pMMSVIDEO,IPARM_SCALE, 30, 30);                
                            MEMSET(&ai, 0, sizeof(ai));
                            ai.wItemID   = VideoIndex;
                            ai.pImage = pMe->m_pMMSVIDEO;
                            if(FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
                            {
                               MSG_FATAL("Failed to Add Opts item %d", ai.wItemID,0,0);
                               return EFAILED;
                            }            
                            IMENUCTL_SetItemText(pMenuCtl, VideoIndex++, NULL, 0, FileName);
                            RELEASEIF(pMe->m_pMMSVIDEO);
                            pMe->m_hasVideo = TRUE;
                            pszBasename = NULL;
                            MEMSET(FileName, 0, sizeof(FileName));

                            AddMimeResIntoMms(pMe,MMSVideopszPath);
                        }    
                        IMENUCTL_Redraw(pMenuCtl);
                        MEMSET(&rc, 0, sizeof(rc));
                        pMe->m_wSelectStore = 1;
                        IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
                        MSG_FATAL("IDD_WRITEMSG_Handler m_rc.x=%d, m_rc.y=%d, m_rc.dy=%d", pMe->m_rc.x, pMe->m_rc.y, pMe->m_rc.dy);
                        //IMENUCTL_GetSelItemRect( pMenuCtl, &rc);
                        rc.x = 0;//大概的一个数字
                        rc.y = 0;
                        rc.dy = GetBottomBarHeight(pMe->m_pDisplay);
                        rc.dx = pMe->m_rc.dx;//five pixels for right edge, 
                        ITEXTCTL_SetRect( pIText, &rc);
						#ifdef FEATURE_VERSION_C316
						ITEXTCTL_SetProperties(pIText, TP_GRAPHIC_BGBLUE|TP_FIXSETRECT |TP_EDITNUMBER_PTSTRING |TP_FIXOEM | TP_USELESS_UPDOWN | TP_GRAPHIC_BG | TP_FOCUS_NOSEL| TP_STARKEY_SWITCH);
						#else
                        ITEXTCTL_SetProperties( pIText, (TP_FIXSETRECT |TP_EDITNUMBER_PTSTRING |TP_FIXOEM | TP_USELESS_UPDOWN | TP_GRAPHIC_BG | TP_FOCUS_NOSEL| TP_STARKEY_SWITCH));
						#endif
                        if (NULL != pMe->m_msSend.m_szMessage)
                        {
                            ITEXTCTL_SetText( pIText, pMe->m_msSend.m_szMessage, WSTRLEN(pMe->m_msSend.m_szMessage));
                        }
                        IMENUCTL_SetActive(pMenuCtl, FALSE);
                        ITEXTCTL_SetActive(pIText, TRUE);
                        ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
                        IDIALOG_SetFocus(pMe->m_pActiveIDlg, IDC_WRITEMSG_TEXT);
                        IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);          
                    }
                    else
                    {
                        IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
#if defined FEATURE_CARRIER_THAILAND_HUTCH || defined FEATURE_CARRIER_THAILAND_CAT
                        ITEXTCTL_SetProperties(pIText, TP_GRAPHIC_BG|TP_FRAME | TP_MULTILINE | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT|TP_FOCUS_NOSEL);
#else
						#ifdef FEATURE_VERSION_C316
						ITEXTCTL_SetProperties(pIText, TP_GRAPHIC_BGBLUE|TP_GRAPHIC_BG|TP_FRAME | TP_MULTILINE | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT | TP_DISPLAY_SMSCOUNT | TP_NOUPDATE|TP_FOCUS_NOSEL);
						#else
                        ITEXTCTL_SetProperties(pIText, TP_GRAPHIC_BG|TP_FRAME | TP_MULTILINE | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT | TP_DISPLAY_SMSCOUNT | TP_NOUPDATE|TP_FOCUS_NOSEL);
						#endif
#endif
                        SetControlRect(pMe, pIText);                        
                    }
                    MSG_FATAL("Menu count=%d", IMENUCTL_GetItemCount(pMenuCtl),0,0);
                }                
#endif       
                MSG_FATAL("IDD_WRITEMSG_Handler EVT_DIALOG_INIT",0,0,0);
	            //IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
	            ICONFIG_GetItem(pMe->m_pConfig,CFGI_WMSWRITD_END_STATUS,&Is_notend,sizeof(Is_notend));
                MSG_FATAL("Is_notend=%d",Is_notend,0,0);
				if(NULL != pMe->m_msSend.m_szMessage)
				{
				    MSG_FATAL("IDD_WRITEMSG_Handler NULL != pMe->m_msSend.m_szMessage",0,0,0);
					ITEXTCTL_SetMaxSize ( pIText, m_WmsMaxSize);
                	(void)ITEXTCTL_SetText(pIText,pMe->m_msSend.m_szMessage,-1);
				}
				else
				{
	            if(!Is_notend)
	            {
	            	uint16 m_nCount = 0;
	            	// 读取消息
                    // 读取消息
    				uint16 wIndex=0;
    				wms_cache_info_node  *pnode = NULL;
    				int nRet,i,nCount=0;
    				boolean bUIMSMS = FALSE;
    				int ret = SUCCESS;
    				
                    pMe->m_eMBoxType = WMS_MB_DRAFT;
                    wms_cacheinfolist_getcounts(WMS_MB_DRAFT, NULL, NULL, &m_nCount);
                    
                    MSG_FATAL("***zzg ReadMessage  DRAFT_m_nCount=%d***",m_nCount,0,0);
                    
                    pMe->m_wPrevMenuSel = 5320+m_nCount;
                    pMe->m_wCurindex = pMe->m_wPrevMenuSel - MSG_CMD_BASE;
                    
                    wIndex = pMe->m_wCurindex;
    				MSG_FATAL("WmsApp_ReadMsg:::1:::::::::::::%d",wIndex,0,0);
    				// 取消息 cache info 节点
    				if (wIndex>=RUIM_MSGINDEX_BASE)
    				{
        				wIndex = wIndex - RUIM_MSGINDEX_BASE;
        				pnode = wms_cacheinfolist_getnode(pMe->m_eMBoxType, WMS_MEMORY_STORE_RUIM, wIndex);
    				}
   					else
    				{
        				pnode = wms_cacheinfolist_getnode(pMe->m_eMBoxType, WMS_MEMORY_STORE_NV_CDMA, wIndex);
    				}

                    //Add By zzg 2013_01_11
                    if (NULL == pnode)
                    {
                        pMe->m_wPrevMenuSel -= 1;
                        pMe->m_wCurindex = pMe->m_wPrevMenuSel - MSG_CMD_BASE;
                        
                        wIndex = pMe->m_wCurindex;
                        MSG_FATAL("WmsApp_ReadMsg:::111:::::::::::::%d",wIndex,0,0);
                        
                        if (wIndex>=RUIM_MSGINDEX_BASE)
        				{
            				wIndex = wIndex - RUIM_MSGINDEX_BASE;
            				pnode = wms_cacheinfolist_getnode(pMe->m_eMBoxType, WMS_MEMORY_STORE_RUIM, wIndex);
        				}
       					else
        				{
            				pnode = wms_cacheinfolist_getnode(pMe->m_eMBoxType, WMS_MEMORY_STORE_NV_CDMA, wIndex);
        				}                                       
                    }                	
                    //Add End
    
    				if (NULL != pnode)
    				{
	    				// 重置当前消息列表
	    				MSG_FATAL("pnode is not null",0,0,0);
	    				MEMSET(pMe->m_CurMsgNodes, 0, sizeof(pMe->m_CurMsgNodes));
	    				WmsApp_FreeMsgNodeMs(pMe);
	    
	   					 pMe->m_idxCur = 0;
#ifdef FEATURE_SMS_UDH
	    				if (pnode->pItems != NULL)
	    				{
	        				MEMCPY(pMe->m_CurMsgNodes, pnode->pItems, sizeof(pMe->m_CurMsgNodes));
					        for (; pMe->m_idxCur<LONGSMS_MAX_PACKAGES; pMe->m_idxCur++)
					        {
					            if (pMe->m_CurMsgNodes[pMe->m_idxCur] != NULL)
					            {
					                pnode = pMe->m_CurMsgNodes[pMe->m_idxCur];
					                break;
					            }
					        }
					    }
					    else
#endif
					    {
					        pMe->m_CurMsgNodes[0] = pnode;
					    }
					    pMe->m_eCurStore = pnode->mem_store;
	                	// 发送读消息命令
	               		ret = IWMS_MsgRead(pMe->m_pwms,
	                                   pMe->m_clientId,
	                                   &pMe->m_callback,
	                                   (void *)pMe,
	                                   pnode->mem_store,
	                                   pnode->index);
	                    if(ret)
	                    {
	                    	return TRUE;
	                    }
                    }
                    else
                    {
                    	MSG_FATAL("pnode is null",0,0,0);
                    	if (NULL != pMe->m_msSend.m_szMessage)
	            		{
	                		ITEXTCTL_SetMaxSize ( pIText, m_WmsMaxSize);
	                		(void)ITEXTCTL_SetText(pIText,pMe->m_msSend.m_szMessage,-1);
	            		}
                    }
                                   /*
                    WmsApp_CombinateMsg(pMe);
                    MSG_FATAL("Is_notend...............................%d",nCount,0,0);
                    if(NULL != pMe->m_msCur.m_szMessage)
	            	{
	            		MSG_FATAL("Is_notend...............................VIEW",0,0,0);
	            		ITEXTCTL_SetMaxSize ( pIText, m_WmsMaxSize);
	                	(void)ITEXTCTL_SetText(pIText,pMe->m_msCur.m_szMessage,-1);
	            	}
	            	Is_notend = TRUE;
	            	ICONFIG_SetItem(pMe->m_pConfig,CFGI_WMSWRITD_END_STATUS,&Is_notend,sizeof(Is_notend));*/
	            }
	            else
	            {
	            	if (NULL != pMe->m_msSend.m_szMessage)
	            	{
	                	ITEXTCTL_SetMaxSize ( pIText, m_WmsMaxSize);
	                	(void)ITEXTCTL_SetText(pIText,pMe->m_msSend.m_szMessage,-1);
                        DBGPRINTF("pMe->m_msSend.m_szMessage=%S", pMe->m_msSend.m_szMessage);
	            	}
	            }
					}
	            
	            //bw:Maximum character SMS in create SMS 1024 wrong.<<20080801
	            pMe->m_nMOmaxChars = WmsApp_SetMaxSize(pIText);
#if defined FEATURE_CARRIER_THAILAND_HUTCH || defined FEATURE_CARRIER_THAILAND_CAT
	            if (pMe->m_nMOmaxChars>MAX_UNICODMSG_PAYLOAD)
	            {
	                nInputMode = ITEXTCTL_GetInputMode (pIText, NULL);
	                
	                if (AEE_TM_THAI == nInputMode || AEE_TM_THAI_R == nInputMode)
	                {
	                    (void)ITEXTCTL_SetInputMode(pIText, AEE_TM_CAPLOWER);
	                }
	            }
#endif

	        #ifdef FEATURE_VERSION_M8P
	        	(void)ITEXTCTL_SetInputMode(pIText, AEE_TM_CAPLOWER);
	        #else
				(void)OEM_GetConfig(
		                          CFGI_LANGUAGE_MOD,
		                          &m_Issetmod,
		                          sizeof(boolean));
		         
		         if(m_Issetmod)
		         {
		         	(void)ITEXTCTL_SetInputMode(pIText, AEE_TM_THAI_R);
		         }
		         else
		         {
		         	#if defined(FEATURE_LANG_ARABIC) ||defined(FEATURE_LANG_THAI)
        	    	{
	        	    	nv_language_enum_type language;
	        	    	OEM_GetConfig( CFGI_LANGUAGE_SELECTION,&language,sizeof(language));
	                    if(NV_LANGUAGE_ARABIC == language)
	                    {
	        	    		return AEE_TM_ARABIC;
	        	    	}
	        	    	else if(NV_LANGUAGE_THAI== language)
	        	    	{
	        	    		return AEE_TM_THAI;
	        	    	}
	        	    	else
	        	    	{
	        	    		return AEE_TM_LETTERS;
	        	    	}
        	    	}
		         	#elif defined(FEATURE_VERSION_MYANMAR)
		         	{
		         	nv_language_enum_type language;
        	    	OEM_GetConfig( CFGI_LANGUAGE_SELECTION,&language,sizeof(language));
                    if(NV_LANGUAGE_MYANMAR == language)
                    {
		         		(void)ITEXTCTL_SetInputMode(pIText, AEE_TM_MYANMAR);
		         	}
		         	else
		         	{
		         		(void)ITEXTCTL_SetInputMode(pIText, AEE_TM_LETTERS);
		         	}
		         	}		         	
		         	#elif defined(FEATURE_VERSION_K202_LM129C)//xxzhen
					(void)ITEXTCTL_SetInputMode(pIText, AEE_TM_PINYIN);
		         	#else
					#ifdef FEATURE_ALL_KEY_PAD
        	    	(void)ITEXTCTL_SetInputMode(pIText, AEE_TM_RAPID);
        	    	#else
					#ifdef FEATURE_VERSION_C316
					(void)ITEXTCTL_SetInputMode(pIText, AEE_TM_CAPLOWER);
					#else
        	    	(void)ITEXTCTL_SetInputMode(pIText, AEE_TM_LETTERS);
					#endif
        	    	#endif
					#endif
				 }
			 #endif
#ifndef FEATURE_USES_MMS              
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0,
                                    0);		
#endif
	            return TRUE;
            }
        case EVT_WMS_MSG_READ:
        	{
        		// 读取消息
        		wms_cache_info_node  *ptnode = NULL;
                wms_cache_info_node  *pnode = NULL;
                wms_memory_store_e_type mem_store;
                wms_message_index_type  wIdx;
                int nRet;
                int i;
                boolean Is_notend;
                wms_msg_event_info_s_type *Info = (wms_msg_event_info_s_type*)dwParam;
                MSG_FATAL("IDD_WRITEMSG_Handler EVT_WMS_MSG_READ",0,0,0);
                mem_store = Info->status_info.message.msg_hdr.mem_store;
                wIdx = Info->status_info.message.msg_hdr.index;
                pnode = pMe->m_CurMsgNodes[pMe->m_idxCur];
                
                if (NULL == pnode)
                {
                    CLOSE_DIALOG(DLGRET_LOADFAILED)
                    return TRUE;
                }
                if ((mem_store == pnode->mem_store) && (pnode->index == wIdx))
                {
                    WMSMessageStruct *pTms = (WMSMessageStruct *)MALLOC(sizeof(WMSMessageStruct));
                    DBGPRINTF("WMS Address = 0x%x", pTms);
                    // 保存读取的数据
                    if (NULL != pTms)
                    {
                    	MSG_FATAL("...................................200 1",0,0,0);
                        WmsApp_ConvertClientMsgToMS(&(Info->status_info.message), pTms);
                        pMe->m_CurMsgNodesMS[pMe->m_idxCur] = pTms;
                    }
                    
                    if ((pnode->msg_tag == WMS_TAG_MT_NOT_READ) &&
                        (pMe->m_eOptType == OPT_VIA_VIEWMSG))
                    {// 只有用户明确选择查看时才修改消息标记
                        nRet = IWMS_MsgModifyTag(pMe->m_pwms,
                                                 pMe->m_clientId,
                                                 &pMe->m_callback,
                                                 (void *)pMe,
                                                 pnode->mem_store,
                                                 pnode->index,
                                                 WMS_TAG_MT_READ);
                                                 
                        if (nRet != SUCCESS)
                        {
                            CLOSE_DIALOG(DLGRET_LOADFAILED)
                        }
                        
                        return TRUE;
                    }
                    
                    pnode = NULL;
                    
                    pMe->m_idxCur++;
                    for (; pMe->m_idxCur<LONGSMS_MAX_PACKAGES; pMe->m_idxCur++)
                    {
                    	MSG_FATAL("...................................201",0,0,0);
                        if (pMe->m_CurMsgNodes[pMe->m_idxCur] != NULL)
                        {
                            pnode = pMe->m_CurMsgNodes[pMe->m_idxCur];
                            break;
                        }
                    }
                    
                    if (NULL != pnode)
                    {// 读下一数据包
                        // 发送读消息命令
                        MSG_FATAL("..................................read",0,0,0);
                        nRet = IWMS_MsgRead(pMe->m_pwms,
                                           pMe->m_clientId,
                                           &pMe->m_callback,
                                           (void *)pMe,
                                           pnode->mem_store,
                                           pnode->index);
                                           
                        if (nRet != SUCCESS)
                        {
                            CLOSE_DIALOG(DLGRET_LOADFAILED)
                        }
                                           
                        return TRUE;
                    }
                    else
                    {
                        MSG_FATAL("WmsApp_CombinateMsg sssssss",0,0,0);
                        WmsApp_CombinateMsg(pMe);
						 if(NULL != pMe->m_msCur.m_szMessage)
	            		{
	            			MSG_FATAL("Is_notend...............................VIEW",0,0,0);
	            			ITEXTCTL_SetMaxSize ( pIText, m_WmsMaxSize);
	                		(void)ITEXTCTL_SetText(pIText,pMe->m_msCur.m_szMessage,-1);
	            		}
	            	Is_notend = TRUE;
	            	ICONFIG_SetItem(pMe->m_pConfig,CFGI_WMSWRITD_END_STATUS,&Is_notend,sizeof(Is_notend));
                    }
                }
                 pMe->m_nMOmaxChars = WmsApp_SetMaxSize(pIText);
#if defined FEATURE_CARRIER_THAILAND_HUTCH || defined FEATURE_CARRIER_THAILAND_CAT
	            if (pMe->m_nMOmaxChars>MAX_UNICODMSG_PAYLOAD)
	            {
	                nInputMode = ITEXTCTL_GetInputMode (pIText, NULL);
	                
	                if (AEE_TM_THAI == nInputMode || AEE_TM_THAI_R == nInputMode)
	                {
	                    (void)ITEXTCTL_SetInputMode(pIText, AEE_TM_CAPLOWER);
	                }
	            }
#endif
	             
				(void)OEM_GetConfig(
		                          CFGI_LANGUAGE_MOD,
		                          &m_Issetmod,
		                          sizeof(boolean));
		         
		         if(m_Issetmod)
		         {
		         	(void)ITEXTCTL_SetInputMode(pIText, AEE_TM_THAI_R);
		         }
		         else
		         {
					#if defined(FEATURE_LANG_ARABIC) ||defined(FEATURE_LANG_THAI)
        	    	{
	        	    	nv_language_enum_type language;
	        	    	OEM_GetConfig( CFGI_LANGUAGE_SELECTION,&language,sizeof(language));
	                    if(NV_LANGUAGE_ARABIC == language)
	                    {
	        	    		return AEE_TM_ARABIC;
	        	    	}
	        	    	else if(NV_LANGUAGE_THAI== language)
	        	    	{
	        	    		return AEE_TM_THAI;
	        	    	}
	        	    	else
	        	    	{
	        	    		return AEE_TM_LETTERS;
	        	    	}
        	    	}
					#elif defined(FEATURE_LANG_CHINESE)
					(void)ITEXTCTL_SetInputMode(pIText, AEE_TM_PINYIN);
                    #else
                    (void)ITEXTCTL_SetInputMode(pIText, AEE_TM_LETTERS);
					#endif
				 }
				 
                
                
                for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
                {
                    if (pMe->m_CurMsgNodes[i] != NULL)
                    {
                        ptnode = pMe->m_CurMsgNodes[i];
                        
                        // 发布删除消息命令
                        nRet = ENOMEMORY;
                        do
                        {
                            nRet = IWMS_MsgDelete(pMe->m_pwms,
                                               pMe->m_clientId,
                                               &pMe->m_callback,
                                               (void *)pMe,
                                               ptnode->mem_store,
                                               ptnode->index);
                        } while(nRet != SUCCESS);
                        pMe->m_CurMsgNodes[i] = NULL;
                    }
                }
        		pMe->m_bwriteclr = FALSE;
            	(void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
        	}
        	return TRUE;
        case EVT_DIALOG_START:         
        	pMe->m_bwriteclr = FALSE;
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;

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
            }
            #endif
#ifdef FEATURE_USES_MMS  
            MSG_FATAL("IDD_WRITEMSG_Handler EVT_USER_REDRAW mmsTotalSizeBig=%d",mmsTotalSizeBig,0,0);
            if(mmsTotalSizeBig)
            {
                mmsTotalSizeBig = FALSE;
                CLOSE_DIALOG(DLGRET_SIZETOOBIG_MMS)
                return TRUE;    
            }

            MSG_FATAL("EVT_USER_REDRAW sel=%d, count=%d",IMENUCTL_GetSel(pMenuCtl),IMENUCTL_GetItemCount(pMenuCtl),0);
            if((pMenuCtl != NULL) && pMe->m_isMMS && ((NULL == pMe->m_pMenu)))
            {
                //IDISPLAY_ClearScreen(pMe->m_pDisplay);
                IMENUCTL_Redraw(pMenuCtl);
                IMENUCTL_SetSel(pMenuCtl, pMe->m_wSelectStore);
                if(pMe->m_wSelectStore == 1);
                {
                    AEERect rc={0};
                    MSG_FATAL("m_pMMSMenuHasFocus == FALSE m_wSelectStore=%d",pMe->m_wSelectStore,0,0);
                    rc.y = 0;
                    rc.dy = GetBottomBarHeight(pMe->m_pDisplay);
                    rc.x = 0;//大概的一个数字
                    rc.dx = pMe->m_rc.dx;//five pixels for right edge, 
                    ITEXTCTL_SetRect( pIText, &rc);
                    MSG_FATAL("EVT_USER_REDRAW rc.x=%d, rc.y=%d, rc.dy=%d", rc.x, rc.y, rc.dy);
					#ifdef FEATURE_VERSION_C316
					ITEXTCTL_SetProperties(pIText, TP_GRAPHIC_BGBLUE|TP_FIXSETRECT |TP_EDITNUMBER_PTSTRING |TP_FIXOEM | TP_USELESS_UPDOWN | TP_GRAPHIC_BG | TP_FOCUS_NOSEL| TP_STARKEY_SWITCH);
					#else
                    ITEXTCTL_SetProperties( pIText, (TP_FIXSETRECT |TP_EDITNUMBER_PTSTRING |TP_FIXOEM | TP_USELESS_UPDOWN | TP_GRAPHIC_BG | TP_FOCUS_NOSEL| TP_STARKEY_SWITCH));
					#endif
                    //IDISPLAY_ClearScreen(pMe->m_pDisplay);
                    if(NULL != pMe->m_msSend.m_szMessage)
                    {
                        ITEXTCTL_SetText( pIText, pMe->m_msSend.m_szMessage, WSTRLEN(pMe->m_msSend.m_szMessage));
                    }
                    IMENUCTL_SetActive(pMenuCtl, FALSE);
                    ITEXTCTL_SetActive(pIText, TRUE);
                    ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
                    IDIALOG_SetFocus(pMe->m_pActiveIDlg, IDC_WRITEMSG_TEXT);
                }
            }
#endif            

#ifdef FEATURE_VERSION_W317A
			(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,                                
                        IDS_MSG_BODY,
                        Annstr,
                        sizeof(Annstr));
#else
			(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,                                
                        IDS_EDIT,
                        Annstr,
                        sizeof(Annstr));
#endif
			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,Annstr);
            if (pMe->m_dwInsertPos == 0)
            {
                ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
            }
            else
            {
                ITEXTCTL_SetCursorPos(pIText, pMe->m_dwInsertPos);
            }
            ITEXTCTL_SetActive(pIText, TRUE);  
            ITEXTCTL_Redraw(pIText);
            // 中断退出时保存的输入法
            {
                if(nMode == AEE_TM_SYMBOLS || nMode == AEE_TM_FACE_SYMBOL)
                {
                    (void)ITEXTCTL_SetInputMode(pIText, nMode);
                    nMode = AEE_TM_NONE;
                 }                
            }
            // 绘制底部操作提示条
            #ifdef FEATURE_ALL_KEY_PAD
            // Option     Back
            DRAW_BOTTOMBAR(BTBAR_OPTION_BACK)
            #else
            {
                AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pIText);
                int nLen=0;
                
                if (NULL != pwstrText)
                {
                    nLen = WSTRLEN(pwstrText);
                }
                
                if (nLen>0)
                {
                    // Option     Delete
                    DRAW_BOTTOMBAR(BTBAR_OPTION_DELETE)
                }
                else
                {
                    // Option     Back
                    DRAW_BOTTOMBAR(BTBAR_OPTION_BACK)
                }
            }
            #endif 
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
#ifndef FEATURE_USES_MMS             
            IDISPLAY_Update(pMe->m_pDisplay);  
#endif
#ifdef FEATURE_LCD_TOUCH_ENABLE
            TSIM_NumberKeypad(FALSE);
#endif
            return TRUE; 
            
        case EVT_KEY_RELEASE:
        case EVT_UPDATE_ARROW_BUTTON:
            nInputMode = ITEXTCTL_GetInputMode(pIText,NULL);
            if ((nInputMode == AEE_TM_SYMBOLS) || nInputMode == AEE_TM_FACE_SYMBOL ||
                (NULL != pMe->m_pMenu))
            {
                return TRUE;
            }
#if defined FEATURE_CARRIER_THAILAND_HUTCH || defined FEATURE_CARRIER_THAILAND_CAT
            {
                AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pIText);
                if (NULL != pwstrText)
                {
                    static int nLen = 0 ;
                    if ( nLen != WSTRLEN(pwstrText) )
                    {
                        nLen = WSTRLEN(pwstrText);
                        pMe->m_nMOmaxChars = WmsApp_SetMaxSize(pIText);
                        ITEXTCTL_Redraw ( pIText );
                    }
                }
            }
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH || defined FEATURE_CARRIER_THAILAND_CAT
            // 绘制底部操作提示条
            
            #ifdef FEATURE_VERSION_SKY
            (void) ISHELL_PostEventEx(pMe->m_pShell,  EVTFLG_ASYNC,
                                      AEECLSID_WMSAPP,
                                      EVT_USER_REDRAW,
                                      0, 0);
            #else
            #ifdef FEATURE_ALL_KEY_PAD
             // Option     Back
             DRAW_BOTTOMBAR(BTBAR_OPTION_BACK)
            #else
            {
                AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pIText);
                int nLen=0;
                
                if (NULL != pwstrText)
                {
                    nLen = WSTRLEN(pwstrText);
                }
                if (nLen>0)
                {
                    // Option     Delete
                    DRAW_BOTTOMBAR(BTBAR_OPTION_DELETE)
                }
                else
                {
                    // Option     Back
                    DRAW_BOTTOMBAR(BTBAR_OPTION_BACK)
                }
            }
            #endif
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);    
            #endif
            return TRUE;

        case EVT_CTL_TEXT_MODECHANGED:  //切换输入法    这里增加画底条，否则从符号输入界面返回时无底条
#if defined FEATURE_CARRIER_THAILAND_HUTCH || defined FEATURE_CARRIER_THAILAND_CAT
            {
                pMe->m_nMOmaxChars = WmsApp_SetMaxSize(pIText);
                //ITEXTCTL_Redraw ( pIText );
                
            }
            
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH || defined FEATURE_CARRIER_THAILAND_CAT
            // 绘制底部操作提示条
            {
                AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pIText);
                int nLen=0;
                 
                nInputMode = ITEXTCTL_GetInputMode(pIText,NULL);
                if (NULL != pwstrText)
                {
                    nLen = WSTRLEN(pwstrText);
                }
                if (nInputMode != AEE_TM_SYMBOLS && nInputMode != AEE_TM_FACE_SYMBOL)
                {
                	#ifdef FEATURE_ALL_KEY_PAD
                		// Option     Back
                        DRAW_BOTTOMBAR(BTBAR_OPTION_BACK)
                	#else
                    if (nLen>0)
                    {
                        // Option     Delete
                        DRAW_BOTTOMBAR(BTBAR_OPTION_DELETE)
                    }
                    else
                    {
                        // Option     Back
                        DRAW_BOTTOMBAR(BTBAR_OPTION_BACK)
                    }
                    #endif
                    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                }
            }
            return TRUE; 
            
        case EVT_CTL_TEXT_TEXTFULL:
            CLOSE_DIALOG(DLGRET_TEXTFULL)
            return TRUE;

        case EVT_DIALOG_END:
            MSG_FATAL("IDD_WRITEMSG_Handler EVT_DIALOG_END",0,0,0);            
            if (NULL != pMe->m_pMenu)
            {
                IMENUCTL_Release(pMe->m_pMenu);
                pMe->m_pMenu = NULL;
            }

            if (NULL != pMe->m_msSend.m_szMessage)
            {
                FREE(pMe->m_msSend.m_szMessage);
                pMe->m_msSend.m_szMessage = NULL;
            }
			
            {
                AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pIText);
                int nLen=0;
                boolean Is_Notend = TRUE;
                pMe->m_CurInputMode = ITEXTCTL_GetInputMode(pIText,NULL);
                
                if (NULL != pwstrText)
                {
                    nLen = WSTRLEN(pwstrText);
                }
                
                if (nLen>0)
                {		                
                    {
                        pMe->m_msSend.m_szMessage = WSTRDUP(ITEXTCTL_GetTextPtr(pIText));
                        
                        if (pMe->m_eAppStatus == WMSAPP_STOP && pMe->m_eDlgReturn != DLGRET_EXIT_EDITOR)
                        {// 程序被中断退出，保存输入到草稿箱
                            int32  nItems = 0;
                            uint16 nMsgs = 0;
                            Is_Notend = FALSE;
                            MSG_FATAL("EVT_DIALOG_END....IDD_WRITEMSG_Handler........2",0,0,0);
                            // 释放用户数据列表
                            WmsApp_FreeUserDataMOList(pMe->m_pUserDataMOList);
                            ICONFIG_SetItem(pMe->m_pConfig,CFGI_WMSWRITD_END_STATUS,&Is_Notend,sizeof(Is_Notend));
                            // 打包消息
                            WmsApp_PrepareUserDataMOList(pMe);
                            pMe->m_idxUserdata = 0;
                            
                            nItems = IVector_Size(pMe->m_pUserDataMOList);
                            
                            // 获取草稿箱消息数
                            wms_cacheinfolist_getcounts(WMS_MB_DRAFT, NULL, NULL, &nMsgs);

                            MSG_FATAL("***zzg nMsgs=%d, nItems=%d***",nMsgs,nItems,0);
                            
                            if ((nMsgs+nItems) <= DRAFT_MAX)
                            {// 存储空间足够，保存中断的输入到草稿箱
                                wms_client_message_s_type *pClientMsg = NULL;
                                int nRet;

                                WmsApp_FreeMultiSendList(pMe->m_pSendList);
                                
                                pClientMsg = WmsApp_GetClientMsgMO(pMe, FALSE);
                                while (pClientMsg != NULL)
                                {
                                    // Must modify message tag!
                                    pClientMsg->msg_hdr.tag = WMS_TAG_MO_DRAFT;
                                   
                                    // 保存消息
                                    nRet = ENOMEMORY;
                                    do 
                                    {
                                        MSG_FATAL("***zzg IWMS_MsgWrite***",0,0,0);
                                        
                                        nRet = IWMS_MsgWrite(pMe->m_pwms, 
                                                             pMe->m_clientId, 
                                                             &pMe->m_callback,
                                                             (void*)pMe,
                                                             WMS_WRITE_MODE_INSERT,
                                                             pClientMsg);

										#ifdef FEATURE_VERSION_W317A		
										MSG_FATAL("***zzg IWMS_MsgWrite IDS_SAVED***", 0, 0, 0);

										(void) ISHELL_PostEvent(pMe->m_pShell,
							                                    AEECLSID_CORE_APP,
							                                    EVT_WMS_DRAFT_SAVED,
							                                    NULL,
							                                    NULL);
										
										#endif
															 
#ifndef WIN32
                                        if (nRet == SUCCESS)
                                        {// 休眠10毫秒以确保有时间执行保存消息的操作
                                            MSLEEP(10);
                                        }
#endif
                                    } while(nRet != SUCCESS);
                                                        
                                    FREE(pClientMsg);
                                    pClientMsg = WmsApp_GetClientMsgMO(pMe, FALSE);
                                }
                                
                                WmsApp_FreeMultiSendList(pMe->m_pSendList);
                                
                                // 释放用户数据列表
                                WmsApp_FreeUserDataMOList(pMe->m_pUserDataMOList);
                                
                                pMe->m_idxUserdata = 0;
                            }
                            else
                            {
                                // 释放用户数据列表
                                WmsApp_FreeUserDataMOList(pMe->m_pUserDataMOList);
                            }
                        }
                        else
                        {
                        	
                        	if(!(pMe->m_bwriteclr))
                        	{
                        		pMe->m_bincommend = TRUE;
                        	}
                        }
                    }
                }
            }			
				
            if(pMe->m_eAppStatus == WMSAPP_SUSPEND)
            {
                nMode = ITEXTCTL_GetInputMode(pIText,NULL);
            }

            pMe->m_dwInsertPos = ITEXTCTL_GetCursorPos(pIText);
			
            return TRUE;
#ifdef FEATURE_USES_MMS 
        case EVT_CTL_SEL_CHANGED:
            if((pMenuCtl != NULL) && pMe->m_isMMS && (NULL == pMe->m_pMenu))
            {
                pMe->m_wSelectStore = wParam;
                MSG_FATAL("m_wSelectStore=%d",pMe->m_wSelectStore,0,0);
                if(pMe->m_wSelectStore == 1)
                {
                    AEERect rc={0};
                    
                    //IMENUCTL_GetSelItemRect( pMenuCtl, &rc);
                    rc.x = 0;//大概的一个数字
                    rc.y = 0;
                    rc.dy = GetBottomBarHeight(pMe->m_pDisplay);
                    rc.dx = pMe->m_rc.dx;//five pixels for right edge, 
                    ITEXTCTL_SetRect( pIText, &rc);
                    MSG_FATAL("EVT_USER_REDRAW rc.x=%d, rc.y=%d, rc.dy=%d", rc.x, rc.y, rc.dy);
                    ITEXTCTL_SetProperties( pIText, (TP_FIXSETRECT |TP_EDITNUMBER_PTSTRING |TP_FIXOEM | TP_USELESS_UPDOWN | TP_GRAPHIC_BG | TP_FOCUS_NOSEL| TP_STARKEY_SWITCH));
                    //IDISPLAY_ClearScreen(pMe->m_pDisplay);
                    if(NULL != pMe->m_msSend.m_szMessage)
                    {
                        ITEXTCTL_SetText( pIText, pMe->m_msSend.m_szMessage, WSTRLEN(pMe->m_msSend.m_szMessage));
                    }
                    IMENUCTL_SetActive(pMenuCtl, FALSE);
                    ITEXTCTL_SetActive(pIText, TRUE);
                    ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
                    IDIALOG_SetFocus(pMe->m_pActiveIDlg, IDC_WRITEMSG_TEXT);
                }    
                else
                {
                    IMENUCTL_SetActive(pMenuCtl, TRUE);
                    ITEXTCTL_SetActive(pIText, FALSE);
                    IDIALOG_SetFocus(pMe->m_pActiveIDlg, IDC_WRITEMSG_MENU);                    
                }
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                MSG_FATAL("EVT_CTL_SEL_CHANGED pMe->m_wSelectStore=%d", pMe->m_wSelectStore,0,0);
            }
            return TRUE;
          
        case EVT_KEY_PRESS:             
            switch(wParam)
            {
                case AVK_UP:
                case AVK_DOWN:
                {                    
                    if((!pMe->m_isMMS) && (NULL == pMe->m_pMenu))
                    {
                        MSG_FATAL("EVT_KEY_PRESS AVK_DOWN",0,0,0);
                        if(pMenuCtl != NULL)
                        {
                            MSG_FATAL("EVT_KEY_PRESS AVK_DOWN 1",0,0,0);
                            IMENUCTL_SetActive(pMenuCtl, FALSE);
                        }
                        MSG_FATAL("EVT_KEY_PRESS AVK_DOWN 2",0,0,0);
                        ITEXTCTL_SetActive(pIText, TRUE);
                        ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);                
                        IDIALOG_SetFocus(pMe->m_pActiveIDlg, IDC_WRITEMSG_TEXT);
                        return TRUE;
                    }                          
                    if ((NULL == pMe->m_pMenu) && (IMENUCTL_GetItemCount(pMenuCtl) > 0))
                    {
                        MSG_FATAL("IMENUCTL_GetSel=%d, m_wSelectStore=%d, Count=%d", IMENUCTL_GetSel(pMenuCtl),pMe->m_wSelectStore,IMENUCTL_GetItemCount(pMenuCtl));                        
                        MSG_FATAL("EVT_KEY_PRESS SelectStore=%d, Sel=%d",pMe->m_wSelectStore,IMENUCTL_GetSel(pMenuCtl),0);
                        if(pMe->m_wSelectStore == 1)
                        {
                            if(NULL != pMe->m_msSend.m_szMessage)
                            {
                                ITEXTCTL_SetText( pIText, pMe->m_msSend.m_szMessage, WSTRLEN(pMe->m_msSend.m_szMessage));
                                IMENUCTL_SetItemText(pMenuCtl, 1, NULL, 0, pMe->m_msSend.m_szMessage);
                            }      
                            else
                            {
                                IMENUCTL_SetItemText(pMenuCtl, 1, NULL, 0, NULL);
                            }
                        }
                        ITEXTCTL_SetActive(pIText, FALSE);
                        IMENUCTL_SetActive(pMenuCtl, TRUE);
                        pMe->m_pMMSMenuHasFocus = TRUE;
                        IDIALOG_SetFocus(pMe->m_pActiveIDlg, IDC_WRITEMSG_MENU);                       
                    }
                    return TRUE;                   
                } 
#ifdef FEATURE_USES_MMS
                case AVK_END:
                    if(pMe->m_isMMS)
                    { 
                        char pszPath[AEE_MAX_FILE_NAME]={'\0'};
                        DBGPRINTF("MMSImageName=%s len=%d", pszPath, STRLEN(pszPath));
                        ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSIMAGE,pszPath, sizeof(pszPath));      
                        ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSSOUND,pszPath, sizeof(pszPath)); 
                        ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSVIDEO,pszPath, sizeof(pszPath)); 
                        FREEIF(pMe->m_EncData.pMessage);
                        pMe->m_hasImage = FALSE;
                        pMe->m_hasSound = FALSE;
                        pMe->m_hasVideo = FALSE;
                        pMe->m_isMMS = FALSE;
                        pMe->m_isSendToAlbumOrEmain = FALSE; 
                    }
                    return TRUE;
#endif                 
            }
            return TRUE;
#endif

        case EVT_KEY:
            MSG_FATAL("IDD_WRITEMSG_Handler EVT_KEY",0,0,0);
            switch (wParam)
            {
                case AVK_CLR:
#ifdef FEATURE_ALL_KEY_PAD
                    if(dwParam == 1)
                    {
                    	if (NULL != pMe->m_pMenu)
                    	{
                        	IMENUCTL_Release(pMe->m_pMenu);
                        	pMe->m_pMenu = NULL;
                        	(void) ISHELL_PostEventEx(pMe->m_pShell, 
                                                EVTFLG_ASYNC,
                                                AEECLSID_WMSAPP,
                                                EVT_USER_REDRAW,
                                                0, 
                                                0);
                    	}
                    }
                    else
                    {
#ifdef FEATURE_USES_MMS                        
                        MSG_FATAL("IDD_WRITEMSG_Handler AVK_CLR m_isMMS=%d, dwParam=%d",pMe->m_isMMS,dwParam,0);
                        if(pMe->m_isMMS)
                        { 
                            char pszPath[AEE_MAX_FILE_NAME]={'\0'};
                            DBGPRINTF("MMSImageName=%s len=%d", pszPath, STRLEN(pszPath));
                            ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSIMAGE,pszPath, sizeof(pszPath));      
                            ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSSOUND,pszPath, sizeof(pszPath)); 
                            ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSVIDEO,pszPath, sizeof(pszPath)); 
                            FREEIF(pMe->m_EncData.pMessage);
                            pMe->m_hasImage = FALSE;
                            pMe->m_hasSound = FALSE;
                            pMe->m_hasVideo = FALSE;
                            pMe->m_isMMS = FALSE;
                            pMe->m_isSendToAlbumOrEmain = FALSE; 
                        }
#endif                         
                    	pMe->m_bwriteclr = TRUE;
                    	CLOSE_DIALOG(DLGRET_CANCELED)
                    }
#else
                    MSG_FATAL("IDD_WRITEMSG_Handler AVK_CLR 1",0,0,0);
                    if (NULL == pMe->m_pMenu)
                    {
#ifdef FEATURE_USES_MMS
                        MSG_FATAL("IDD_WRITEMSG_Handler AVK_CLR 2",0,0,0);
                        if(pMe->m_isMMS)
                        { 
                            char pszPath[AEE_MAX_FILE_NAME]={'\0'};
                            DBGPRINTF("MMSImageName=%s len=%d", pszPath, STRLEN(pszPath));
                            ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSIMAGE,pszPath, sizeof(pszPath));      
                            ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSSOUND,pszPath, sizeof(pszPath)); 
                            ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSVIDEO,pszPath, sizeof(pszPath)); 
                            FREEIF(pMe->m_EncData.pMessage);
                            pMe->m_hasImage = FALSE;
                            pMe->m_hasSound = FALSE;
                            pMe->m_hasVideo = FALSE;
                            pMe->m_isMMS = FALSE;
                            pMe->m_isSendToAlbumOrEmain = FALSE; 
                        }
#endif                           
                        CLOSE_DIALOG(DLGRET_CANCELED)
                    }
                    else
                    {
                        MSG_FATAL("pMe->m_pMenu != NULL",0,0,0);
                    	IMENUCTL_Release(pMe->m_pMenu);
                       	pMe->m_pMenu = NULL;
                       	(void) ISHELL_PostEventEx(pMe->m_pShell, 
                                                EVTFLG_ASYNC,
                                                AEECLSID_WMSAPP,
                                                EVT_USER_REDRAW,
                                                0, 
                                                0);
                    }
#endif
                    return TRUE;
   
                case AVK_SELECT:
                    MSG_FATAL("AVK_SELECT 1",0,0,0);
                    pMe->m_dwInsertPos = ITEXTCTL_GetCursorPos(pIText);           
                    if (pMe->m_pMenu != NULL)
                    {
                        return TRUE;
                    }
                    MSG_FATAL("AVK_SELECT 2",0,0,0);
                    // 显示弹出菜单
                    if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_MENUCTL, 
                            (void **) &pMe->m_pMenu) == SUCCESS)
                    {
                    
                        AEERect rc={0};
                        AEERect Temprc={0};
                        // 将文本控件置于非激活状态
                        ITEXTCTL_SetActive(pIText, FALSE);				
                        
                        // 动态添加菜单项
                        MENU_ADDITEM(pMe->m_pMenu, IDS_SEND);
						#ifdef  EMOTIONSYMBOL
                        MENU_ADDITEM(pMe->m_pMenu, IDS_INSERTEMOTIONSYMBOL);
						#endif
                        MENU_ADDITEM(pMe->m_pMenu, IDS_INSERTCONTACT);
#ifdef FEATURE_USES_MMS       
                        IMENUCTL_SetActive(pMenuCtl, FALSE);
                        if(!pMe->m_hasImage && !pMe->m_hasSound && !pMe->m_hasVideo)
                        {
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_PICTURE);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_NEW_PICTURE);
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_VIDEO);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_SOUND);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_FILE);//add by xuhui 2011/08/01
                        } 
                        else if(pMe->m_hasImage && !pMe->m_hasSound && !pMe->m_hasVideo)
                        {
                            MENU_ADDITEM(pMe->m_pMenu, IDS_REMOVE_PICTURE);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_VIDEO);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_SOUND);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_FILE);//add by xuhui 2011/08/01
                        } 
                        else if(pMe->m_hasSound && !pMe->m_hasImage && !pMe->m_hasVideo)
                        {
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_PICTURE);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_NEW_PICTURE);
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_VIDEO);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_REMOVE_SOUND);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_FILE);//add by xuhui 2011/08/01
                        }   
                        else if(pMe->m_hasVideo && !pMe->m_hasImage && !pMe->m_hasSound)
                        {
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_PICTURE);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_NEW_PICTURE);
                            MENU_ADDITEM(pMe->m_pMenu, IDS_REMOVE_VIDEO);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_SOUND);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_FILE);//add by xuhui 2011/08/01
                        }           
                        else if(pMe->m_hasVideo && pMe->m_hasImage && !pMe->m_hasSound)
                        {
                            MENU_ADDITEM(pMe->m_pMenu, IDS_REMOVE_PICTURE);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_REMOVE_VIDEO);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_SOUND);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_FILE);//add by xuhui 2011/08/01
                        }    
                        else if(pMe->m_hasSound && pMe->m_hasImage && !pMe->m_hasVideo)
                        {
                            MENU_ADDITEM(pMe->m_pMenu, IDS_REMOVE_PICTURE);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_VIDEO);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_REMOVE_SOUND);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_FILE);//add by xuhui 2011/08/01
                        }           
                        else if(pMe->m_hasSound && pMe->m_hasVideo && !pMe->m_hasImage)
                        {
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_PICTURE);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_NEW_PICTURE);
                            MENU_ADDITEM(pMe->m_pMenu, IDS_REMOVE_VIDEO);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_REMOVE_SOUND);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_FILE);//add by xuhui 2011/08/01
                        }    
                        else if(pMe->m_hasSound && pMe->m_hasVideo && pMe->m_hasImage)
                        {
                            MENU_ADDITEM(pMe->m_pMenu, IDS_REMOVE_PICTURE);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_REMOVE_VIDEO);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_REMOVE_SOUND);//add by xuhui 2011/08/01
                            MENU_ADDITEM(pMe->m_pMenu, IDS_INSERT_FILE);//add by xuhui 2011/08/01
                        }  
#ifdef FEATURE_USES_MMS 
                        if(pMe->m_isMMS)
                        {
                            MENU_ADDITEM(pMe->m_pMenu, IDS_SEND_TO_ALBUM_OR_EMAIL);
                        }
#endif                        
#endif                       
                        MENU_ADDITEM(pMe->m_pMenu, IDS_INSERTTEMPLATES);
						MENU_ADDITEM(pMe->m_pMenu, IDS_SAVETODRAFT);	//Add By zzg 2010_09_11						
                        MENU_ADDITEM(pMe->m_pMenu, IDS_SAVEASPRESET);
                        MENU_ADDITEM(pMe->m_pMenu, IDS_EXIT_EDITOR);
                        MENU_ADDITEM(pMe->m_pMenu, IDS_SETTING);
#ifdef FEATURE_USES_MMS 
                        if(pMe->m_isMMS)
                        {
                            AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pIText);
                            byte    wmsContentText[MMS_MAX_TEXT_SIZE] = {0}; 
                            uint16 len = 0;
                            if (NULL != pwstrText)
                            {
                                len = WSTRLEN(pwstrText);
                            }
                            MSG_FATAL("IDD_WRITEMSG_Handler len=%d",len,0,0);
                            DBGPRINTF("pwstrText=%S", pwstrText);
                            if (len>0)
                            {
                                WSP_MMS_ENCODE_DATA *mms_data = NULL;
                                if(NULL == pMe->m_EncData.pMessage)
                                {
                                    MSG_FATAL("IDD_WRITEMSG_Handler pMe->m_EncData.pMessage MALLOC",0,0,0);
                                    pMe->m_EncData.pMessage = MALLOC(sizeof(MMS_WSP_MESSAGE_SEND));
                                    DBGPRINTF("WMS Address = 0x%x", pMe->m_EncData.pMessage);
                                    MEMSET(pMe->m_EncData.pMessage,NULL,sizeof(MMS_WSP_MESSAGE_SEND));
                                }
                                    
                            	mms_data = &pMe->m_EncData.pMessage->mms_data;
                                WSTRTOUTF8(pwstrText, len, wmsContentText, MMS_MAX_TEXT_SIZE);
                                DBGPRINTF("wmsContentText = %s", wmsContentText);
                                len = STRLEN((char*)wmsContentText);
                                STRNCPY((char*)mms_data->fragment[0].hContentText,(char*)wmsContentText,len);                                    
                                DBGPRINTF("mms_data->fragment[0].len=%s", len);  
                                DBGPRINTF("hContentText=%s", (char*)mms_data->fragment[0].hContentText);
                                MSG_FATAL("mms_data->frag_num=%d",mms_data->frag_num,0,0);
                                                    
                                len = STRLEN("text/plain");
                                STRNCPY((char*)mms_data->fragment[0].hContentType,"text/plain",len);
                                len = STRLEN("1.txt");
                                STRNCPY((char*)mms_data->fragment[0].hContentLocation,"1.txt",len);
                                len = STRLEN("1.txt");
                                STRNCPY((char*)mms_data->fragment[0].hContentID,"1.txt",len);
                                len = STRLEN("1.txt");
                                STRNCPY((char*)mms_data->fragment[0].hContentName,"1.txt",len);
                                mms_data->frag_num++;
                            }
                        }     
#endif                            
                        // 设置菜单属性
                        IMENUCTL_SetPopMenuRect(pMe->m_pMenu);

                        IMENUCTL_SetProperties(pMe->m_pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
						IMENUCTL_SetOemProperties(pMe->m_pMenu, OEMMP_USE_MENU_STYLE);
						
                        IMENUCTL_SetBottomBarType(pMe->m_pMenu,BTBAR_SELECT_BACK);
                        
                        IMENUCTL_SetActive(pMe->m_pMenu, TRUE);						
    					(void)IMENUCTL_Redraw(pMe->m_pMenu);     						
                        
                        IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                    }
                    return TRUE;
                
                case AVK_INFO:
#ifdef FEATURE_USES_MMS 
                        if(pMe->m_isMMS)
                        {
                            AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pIText);
                            byte    wmsContentText[MMS_MAX_TEXT_SIZE] = {0}; 
                            uint8 len = 0;
                            if (NULL != pwstrText)
                            {
                                len = WSTRLEN(pwstrText);
                            }
                            MSG_FATAL("IDD_WRITEMSG_Handler len=%d",len,0,0);
                            if (len>0)
                            {
                                WSP_MMS_ENCODE_DATA *mms_data = NULL;
                                if(NULL == pMe->m_EncData.pMessage)
                                {
                                    MSG_FATAL("IDD_WRITEMSG_Handler pMe->m_EncData.pMessage MALLOC",0,0,0);
                                    pMe->m_EncData.pMessage = MALLOC(sizeof(MMS_WSP_MESSAGE_SEND));
                                    DBGPRINTF("WMS Address = 0x%x", pMe->m_EncData.pMessage);
                                    MEMSET(pMe->m_EncData.pMessage,NULL,sizeof(MMS_WSP_MESSAGE_SEND));
                                }
                                    
                            	mms_data = &pMe->m_EncData.pMessage->mms_data;
                                WSTRTOUTF8(pwstrText, len, wmsContentText, MMS_MAX_TEXT_SIZE);
                                DBGPRINTF("wmsContentText = %s", (char*)wmsContentText);
                                len = STRLEN((char*)wmsContentText);
                                STRNCPY((char*)mms_data->fragment[0].hContentText,(char*)wmsContentText,len);                                    
                                DBGPRINTF("mms_data->fragment[0].len%s", len);  
                                DBGPRINTF("hContentText=%s", (char*)mms_data->fragment[0].hContentText);
                                MSG_FATAL("mms_data->frag_num=%d",mms_data->frag_num,0,0);
                                                    
                                len = STRLEN("text/plain");
                                STRNCPY((char*)mms_data->fragment[0].hContentType,"text/plain",len);
                                len = STRLEN("1.txt");
                                STRNCPY((char*)mms_data->fragment[0].hContentLocation,"1.txt",len);
                                len = STRLEN("1.txt");
                                STRNCPY((char*)mms_data->fragment[0].hContentID,"1.txt",len);
                                len = STRLEN("1.txt");
                                STRNCPY((char*)mms_data->fragment[0].hContentName,"1.txt",len);
                                mms_data->frag_num++;
                            }

                        }     
#endif                       
                    return IDD_WRITEMSG_Handler((void *)pMe, EVT_COMMAND, IDS_SEND, 0);
                      
                default:
                    break;
            }
            return TRUE;
            
        case EVT_COMMAND:
#ifdef FEATURE_USES_MMS
            MSG_FATAL("EVT_COMMAND wParam=0x%x", wParam,0,0);
            if((NULL == pMe->m_pMenu) && (IMENUCTL_GetItemCount(pMenuCtl) > 0))
            {
                pMe->m_wSelectStore = wParam;
            }
            else
#endif
            if (NULL != pMe->m_pMenu)
            {
                IMENUCTL_Release(pMe->m_pMenu);
                pMe->m_pMenu = NULL;
            }
            {
                AECHAR *pText = ITEXTCTL_GetTextPtr(pIText);
                
                if (NULL != pText &&
                    (wParam == IDS_INSERTCONTACT || 
                     wParam == IDS_INSERTTEMPLATES || 
                     wParam == IDS_INSERTEMOTIONSYMBOL || 
                     wParam == IDS_INSERTDOMAIM) &&
                    WSTRLEN(pText) >= pMe->m_nMOmaxChars)
                {
                    CLOSE_DIALOG(DLGRET_TEXTFULL)
                    return TRUE;
                }
            }
            switch(wParam)
            {
                // 发送
                case IDS_SEND:                       
                    if ((SEND_MSG_REPLY != pMe->m_eCreateWMSType) &&
                        (SEND_MSG_RESEND != pMe->m_eCreateWMSType) &&
                        (SEND_MSG_EDITRESERVE != pMe->m_eCreateWMSType) &&
                        (SEND_MSG_RESERVE != pMe->m_eCreateWMSType))
                    {
                        pMe->m_eMakeAddListMode = MAKEADDLIST_INIT;
                    }
                    else
                    {
                        pMe->m_eMakeAddListMode = MAKEADDLIST_NONE;
                    }
                    CLOSE_DIALOG(DLGRET_SEND)
                    return TRUE;

                // 插入联系方式
                case IDS_INSERTCONTACT:
                    pMe->m_eInsertType = INSERT_EDITWMS;
                    WMSExtApp_GetAddresseeFromContactsApp(pMe, GETADDR_INSERT);
                    return TRUE;
                    
                // 插入常用语
                case IDS_INSERTTEMPLATES:
                    CLOSE_DIALOG(DLGRET_INSERTTEMPLATES)
                    return TRUE;
#ifdef FEATURE_USES_MMS
                case IDS_INSERT_PICTURE:
                {
                    char MMSName[MG_MAX_FILE_NAME]={'\0'};
                    MSG_FATAL("IDS_INSERT_PICTURE",0,0,0);
                    pMe->m_insertMMSType = 0;
                    DBGPRINTF("MMSName=%s len=%d", MMSName, STRLEN(MMSName));                     
#ifdef FEATURE_APP_MEDIAGALLERY					
					CMediaGallery_FileExplorer_ForMMS(GALLERY_IMAGE_SETTING, NULL);
#endif                    
                    return TRUE;
                }

                case IDS_INSERT_NEW_PICTURE:
                {
                    char MMSName[MG_MAX_FILE_NAME]={'\0'};
                    MSG_FATAL("IDS_INSERT_NEW_PICTURE",0,0,0);
                    pMe->m_insertMMSType = 0;
                    return ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_APP_CAMERA, "MMS");
                }

                    
                case IDS_INSERT_VIDEO:
                {
                    char MMSName[MG_MAX_FILE_NAME]={'\0'};
                    MSG_FATAL("IDS_INSERT_VIDEO",0,0,0);
                    pMe->m_insertMMSType = 2;
                    DBGPRINTF("MMSName=%s len=%d", MMSName, STRLEN(MMSName));                  
#ifdef FEATURE_APP_MEDIAGALLERY					
					CMediaGallery_FileExplorer_ForMMS(GALLERY_VIDEO_BROWSE, NULL);
#endif                      
                    //CLOSE_DIALOG(DLGRET_INSERTVIDEO)
                    return TRUE;    
                }

                case IDS_INSERT_SOUND:
                {
                    char MMSName[MG_MAX_FILE_NAME]={'\0'};
                    MSG_FATAL("IDS_INSERT_VIDEO",0,0,0);
                    pMe->m_insertMMSType = 1;
                    DBGPRINTF("MMSName=%s len=%d", MMSName, STRLEN(MMSName));               
#ifdef FEATURE_APP_MEDIAGALLERY					
					CMediaGallery_FileExplorer_ForMMS(GALLERY_MUSIC_SETTING, NULL);
#endif                           
                    //CLOSE_DIALOG(DLGRET_INSERTSOUND)
                    return TRUE;
                }
                case IDS_INSERT_FILE:
                    pMe->m_insertMMSType = 3;
#ifdef FEATURE_APP_MEDIAGALLERY					
					CMediaGallery_FileExplorer_ForMMS(GALLERY_FILE_SELECT, NULL);
#endif  
                    return TRUE;

                case IDS_REMOVE_PICTURE:
                {
                    char MMSImageName[AEE_MAX_FILE_NAME]={'/0'};
                    char* pMMSImageName = NULL;
                    uint8* pBuf = NULL;
                    uint32 nBufLen = 0;
                    uint8 i = IMAGE_MENU_INDEX;
                    CtlAddItem ai;
                    MSG_FATAL("IDS_REMOVE_PICTURE pMe->m_pMMSImage != NULL",0,0,0);
                    ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSIMAGE,MMSImageName, sizeof(MMSImageName));       
                    if((pMMSImageName = MimeResCheckTypeExist(pMe,IMAGE_MIME_BASE,&pBuf,&nBufLen)) != NULL)
                    {
                        if(nBufLen > 0)
                        {
                            FREEIF(pBuf);
                        }
                        MEMSET(pMMSImageName,NULL,STRLEN(pMMSImageName));
                        pMe->m_EncData.pMessage->mms_data.frag_num --;
                    };
                    
                    RELEASEIF(pMe->m_pMMSImage);
                    pMe->m_hasImage = FALSE;
                    for(; i < SOUND_MENU_INDEX; i++)
                    {
                        if ( IMENUCTL_GetItem( pMenuCtl, i,&ai ) )
                        {
                            MSG_FATAL("menu item index=%d", i,0,0);
                            IMENUCTL_DeleteItem(pMenuCtl, i);
                        }
                        else
                        {
                            MSG_FATAL("menu item don't exit",0,0,0);
                        }
                    }
                    
                    MSG_FATAL("menu count = %d", IMENUCTL_GetItemCount(pMenuCtl),0,0);
                    if(IMENUCTL_GetItemCount(pMenuCtl) == 1)
                    {
                        (void)IMENUCTL_DeleteAll(pMenuCtl);
                        IMENUCTL_SetActive(pMenuCtl, FALSE);
                        ITEXTCTL_SetActive(pIText, TRUE);
                        ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
                        IDIALOG_SetFocus(pMe->m_pActiveIDlg, IDC_WRITEMSG_TEXT);
                        { 
                            char pszPath[AEE_MAX_FILE_NAME]={'\0'};
                            DBGPRINTF("MMSImageName=%s len=%d", pszPath, STRLEN(pszPath));
                            ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSIMAGE,pszPath, sizeof(pszPath));      
                            ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSSOUND,pszPath, sizeof(pszPath)); 
                            ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSVIDEO,pszPath, sizeof(pszPath)); 
                            FREEIF(pMe->m_EncData.pMessage);
                            pMe->m_hasImage = FALSE;
                            pMe->m_hasSound = FALSE;
                            pMe->m_hasVideo = FALSE;
                            pMe->m_isMMS = FALSE;
                            pMe->m_isSendToAlbumOrEmain = FALSE; 
                        }
                    }
                    else
                    {
                        char pszPath[AEE_MAX_FILE_NAME]={'\0'};
                        ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSIMAGE,pszPath, sizeof(pszPath));  
                    }
                	(void) ISHELL_PostEventEx(pMe->m_pShell, 
                                        EVTFLG_ASYNC,
                                        AEECLSID_WMSAPP,
                                        EVT_USER_REDRAW,
                                        0, 
                                        0);
                    return TRUE;
                }

                case IDS_REMOVE_SOUND:
                {
                    char MMSImageName[AEE_MAX_FILE_NAME]={'/0'};
                    char* pMMSSoundName = NULL;
                    uint8* pBuf = NULL;
                    uint32 nBufLen = 0;
                    CtlAddItem ai;
                    uint8 i = SOUND_MENU_INDEX;
                    MSG_FATAL("pMe->m_pMMSSOUND != NULL",0,0,0);
                    ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSSOUND,MMSImageName, sizeof(MMSImageName));       
                    if(((pMMSSoundName = MimeResCheckTypeExist(pMe,SOUND_MIME_BASE,&pBuf,&nBufLen)) != NULL)
                        || ((pMMSSoundName = MimeResCheckTypeExist(pMe,AUDIO_MIME_BASE,&pBuf,&nBufLen)) != NULL))
                    {
                        if(nBufLen > 0)
                        {
                            FREEIF(pBuf);
                        }
                        MEMSET(pMMSSoundName,NULL,STRLEN(pMMSSoundName));
                        pMe->m_EncData.pMessage->mms_data.frag_num --;
                    };   
                    
                    RELEASEIF(pMe->m_pMMSSOUND);
                    pMe->m_hasSound = FALSE;
                    MSG_FATAL("IMENUCTL_DeleteItem index=%d,frag_num=%d",IMENUCTL_GetSel(pMenuCtl),pMe->m_EncData.pMessage->mms_data.frag_num,0);
                    for(; i < VIDEO_MENU_INDEX; i++)
                    {
                        if ( IMENUCTL_GetItem( pMenuCtl, i ,&ai ) )
                        {
                            IMENUCTL_DeleteItem(pMenuCtl, i);
                        }
                    }                    
                    MSG_FATAL("menu count = %d", IMENUCTL_GetItemCount(pMenuCtl),0,0);
                    if(IMENUCTL_GetItemCount(pMenuCtl) == 1)
                    {
                        (void)IMENUCTL_DeleteAll(pMenuCtl);    
                        IMENUCTL_SetActive(pMenuCtl, FALSE);
                        ITEXTCTL_SetActive(pIText, TRUE);
                        ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
                        IDIALOG_SetFocus(pMe->m_pActiveIDlg, IDC_WRITEMSG_TEXT);
                        { 
                            char pszPath[AEE_MAX_FILE_NAME]={'\0'};
                            DBGPRINTF("MMSImageName=%s len=%d", pszPath, STRLEN(pszPath));
                            ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSIMAGE,pszPath, sizeof(pszPath));      
                            ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSSOUND,pszPath, sizeof(pszPath)); 
                            ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSVIDEO,pszPath, sizeof(pszPath)); 
                            FREEIF(pMe->m_EncData.pMessage);
                            pMe->m_hasImage = FALSE;
                            pMe->m_hasSound = FALSE;
                            pMe->m_hasVideo = FALSE;
                            pMe->m_isMMS = FALSE;
                            pMe->m_isSendToAlbumOrEmain = FALSE; 
                        }                        
                    }  
                    else
                    {
                        char pszPath[AEE_MAX_FILE_NAME]={'\0'};
                        ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSSOUND,pszPath, sizeof(pszPath)); 
                    }                    
                	(void) ISHELL_PostEventEx(pMe->m_pShell, 
                                        EVTFLG_ASYNC,
                                        AEECLSID_WMSAPP,
                                        EVT_USER_REDRAW,
                                        0, 
                                        0);
                    return TRUE;
                }          

                case IDS_REMOVE_VIDEO:
                {
                    char MMSImageName[AEE_MAX_FILE_NAME]={'/0'};
                    char* pMMSVideoName = NULL;
                    uint8* pBuf = NULL;
                    uint32 nBufLen = 0;
                    CtlAddItem ai;
                    uint8 i = VIDEO_MENU_INDEX;                    
                    MSG_FATAL("pMe->m_pMMSVIDEO != NULL",0,0,0);
                    ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSVIDEO,MMSImageName, sizeof(MMSImageName));       
                    if((pMMSVideoName = MimeResCheckTypeExist(pMe,VIDEO_MIME_BASE,&pBuf,&nBufLen)) != NULL)
                    {
                        if(nBufLen > 0)
                        {
                            FREEIF(pBuf);
                        }
                        MEMSET(pMMSVideoName,NULL,STRLEN(pMMSVideoName));
                        pMe->m_EncData.pMessage->mms_data.frag_num --;
                    };      
                    RELEASEIF(pMe->m_pMMSVIDEO);
                    pMe->m_hasVideo = FALSE;
                    MSG_FATAL("IMENUCTL_DeleteItem index=%d",IMENUCTL_GetSel(pMenuCtl),0,0);
                    for(; i < OTHER_MENU_INDEX; i++)
                    {
                        if ( IMENUCTL_GetItem( pMenuCtl, i,&ai ) )
                        {
                            IMENUCTL_DeleteItem(pMenuCtl, i);
                        }
                    }  

                    MSG_FATAL("menu count = %d", IMENUCTL_GetItemCount(pMenuCtl),0,0);
                    if(IMENUCTL_GetItemCount(pMenuCtl) == 1)
                    {
                        (void)IMENUCTL_DeleteAll(pMenuCtl);    
                        IMENUCTL_SetActive(pMenuCtl, FALSE);
                        ITEXTCTL_SetActive(pIText, TRUE);
                        ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
                        IDIALOG_SetFocus(pMe->m_pActiveIDlg, IDC_WRITEMSG_TEXT);
                        { 
                            char pszPath[AEE_MAX_FILE_NAME]={'\0'};
                            DBGPRINTF("MMSImageName=%s len=%d", pszPath, STRLEN(pszPath));
                            ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSIMAGE,pszPath, sizeof(pszPath));      
                            ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSSOUND,pszPath, sizeof(pszPath)); 
                            ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSVIDEO,pszPath, sizeof(pszPath)); 
                            FREEIF(pMe->m_EncData.pMessage);
                            pMe->m_hasImage = FALSE;
                            pMe->m_hasSound = FALSE;
                            pMe->m_hasVideo = FALSE;
                            pMe->m_isMMS = FALSE;
                            pMe->m_isSendToAlbumOrEmain = FALSE; 
                        }                        
                    }  
                    else
                    {
                        char pszPath[AEE_MAX_FILE_NAME]={'\0'};
                        ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSVIDEO,pszPath, sizeof(pszPath)); 
                    }                      
                	(void) ISHELL_PostEventEx(pMe->m_pShell, 
                                        EVTFLG_ASYNC,
                                        AEECLSID_WMSAPP,
                                        EVT_USER_REDRAW,
                                        0, 
                                        0);
                    return TRUE;
                }     

                case IDS_SEND_TO_ALBUM_OR_EMAIL:
                {
                    if ((SEND_MSG_REPLY != pMe->m_eCreateWMSType) &&
                        (SEND_MSG_RESEND != pMe->m_eCreateWMSType) &&
                        (SEND_MSG_EDITRESERVE != pMe->m_eCreateWMSType) &&
                        (SEND_MSG_RESERVE != pMe->m_eCreateWMSType))
                    {
                        pMe->m_eMakeAddListMode = MAKEADDLIST_INIT;
                    }
                    else
                    {
                        pMe->m_eMakeAddListMode = MAKEADDLIST_NONE;
                    }                    
                    pMe->m_isSendToAlbumOrEmain = TRUE; 
                    CLOSE_DIALOG(DLGRET_SEND)
                    return TRUE;
                }                
#endif
                    
                // 插入表情符号: 
                case IDS_INSERTEMOTIONSYMBOL:
//                // 插入域名符号: 
//                case IDS_INSERTDOMAIM:
                    ITEXTCTL_SetActive(pIText, TRUE);
//                    if (wParam == IDS_INSERTDOMAIM)
//                    {
//                        (void)ITEXTCTL_SetInputMode(pIText, AEE_TM_NET_SYMBOL);
//                    }
//                    else
                    {
                        (void)ITEXTCTL_SetInputMode(pIText, AEE_TM_FACE_SYMBOL);
                    } 
                    return TRUE;

				//Add By zzg 2010_09_11
				case IDS_SAVETODRAFT:
				{
	                AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pIText);
	                int nLen = 0;
#ifdef FEATURE_USES_MMS                     
                    if(pMe->m_isMMS)
                    {
                        uint8 len = 0;
                        char pszPath[AEE_MAX_FILE_NAME]={'\0'};    
                        IFileMgr *pIFileMgr = NULL;
                        int result = 0;
                        uint32 pdwTotal = 0;
                        uint32 pdwFree = 0;
                        result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);
                        if (SUCCESS != result)
                        {
                            MSG_FATAL("IDD_WRITEMSG_Handler: Open file error %x", result,0,0);
                            return FALSE;
                        }            
                        pdwFree = IFILEMGR_GetFreeSpace(pIFileMgr, &pdwTotal); 
                        IFILEMGR_Release(pIFileMgr);    
                        MSG_FATAL("IDD_WRITEMSG_Handler pdwFree=%d, pdwTotal=%d",pdwFree, pdwTotal, 0);
                        if((pdwFree > MSG_MAX_PACKET_SIZE+50))   
                        {
                            if(NULL == pMe->m_EncData.pMessage)
                            {
                                pMe->m_EncData.pMessage = MALLOC(sizeof(MMS_WSP_MESSAGE_SEND));
                                DBGPRINTF("WMS Address = 0x%x", pMe->m_EncData.pMessage);
                                MEMSET(pMe->m_EncData.pMessage,NULL,sizeof(MMS_WSP_MESSAGE_SEND));
                                DBGPRINTF("WMS Address = 0x%x", pMe->m_EncData.pMessage);
                            }
                            DBGPRINTF("IDD_SENDING_Handler to:%s",pMe->m_EncData.pMessage->hTo);
                            DBGPRINTF("WMS Address = 0x%x", pMe->m_EncData.pMessage);
                            pMe->m_EncData.pMessage->iRetrieveStatus = MMS_VALUE_USELESSNESS;
                            pMe->m_EncData.pMessage->iDate = MMS_VALUE_USELESSNESS;
                            pMe->m_EncData.pMessage->iPriority = MMS_VALUE_USELESSNESS;
                            pMe->m_EncData.pMessage->bSenderVisibility = pMe->m_isMMSSenderVisibility;
                            pMe->m_EncData.pMessage->iRetrieveStatus = MMS_VALUE_USELESSNESS;
                            pMe->m_EncData.pMessage->iExpiry = MMS_VALUE_USELESSNESS;
                            pMe->m_EncData.pMessage->iDeliveryTime = MMS_VALUE_USELESSNESS;
                            pMe->m_EncData.pMessage->bReadRep = pMe->m_isMMSReadReply;
                            pMe->m_EncData.pMessage->bDelRep = pMe->m_isMMSDeliveryReport;                                                
    	                    WMS_MMSState(WMS_MMS_PDU_DRAFT,0,(uint32)&pMe->m_EncData);

                            ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSIMAGE,pszPath, sizeof(pszPath));      
                            ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSSOUND,pszPath, sizeof(pszPath)); 
                            ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSVIDEO,pszPath, sizeof(pszPath)); 
                            FREEIF(pMe->m_EncData.pMessage);
                            pMe->m_hasImage = FALSE;
                            pMe->m_hasSound = FALSE;
                            pMe->m_hasVideo = FALSE;
                            pMe->m_isMMS = FALSE; 
                            pMe->m_isSendToAlbumOrEmain = FALSE; 
                        }

						
						CLOSE_DIALOG(DLGRET_CANCELED)						
    					return TRUE;                        
                    }
#endif                    
	                pMe->m_CurInputMode = ITEXTCTL_GetInputMode(pIText, NULL);
	                
	                if (NULL != pwstrText)
	                {
	                    nLen = WSTRLEN(pwstrText);
	                }
	                
	                if (nLen > 0)
	                {	
                    	// 程序被中断退出，保存输入到草稿箱
                        int32  nItems = 0;
                        uint16 nMsgs = 0;

						pMe->m_msSend.m_szMessage = WSTRDUP(ITEXTCTL_GetTextPtr(pIText));
                        
                        // 释放用户数据列表
                        WmsApp_FreeUserDataMOList(pMe->m_pUserDataMOList);
                        
                        // 打包消息
                        WmsApp_PrepareUserDataMOList(pMe);
                        pMe->m_idxUserdata = 0;
                        
                        nItems = IVector_Size(pMe->m_pUserDataMOList);

                        // 获取草稿箱消息数
                        wms_cacheinfolist_getcounts(WMS_MB_DRAFT, NULL, NULL, &nMsgs);
                        
                        if ((nMsgs+nItems) <= DRAFT_MAX)
                        {// 存储空间足够，保存中断的输入到草稿箱
                            wms_client_message_s_type *pClientMsg = NULL;
                            int nRet;

                            WmsApp_FreeMultiSendList(pMe->m_pSendList);
                            
                            pClientMsg = WmsApp_GetClientMsgMO(pMe, FALSE);
                            while (pClientMsg != NULL)
                            {
                                // Must modify message tag!
                                pClientMsg->msg_hdr.tag = WMS_TAG_MO_DRAFT;
                                
                                // 保存消息
                                nRet = ENOMEMORY;
                                do 
                                {
                                    nRet = IWMS_MsgWrite(pMe->m_pwms, 
                                                         pMe->m_clientId, 
                                                         &pMe->m_callback,
                                                         (void*)pMe,
                                                         WMS_WRITE_MODE_INSERT,
                                                         pClientMsg);
                                                         
#ifndef WIN32
                                    if (nRet == SUCCESS)
                                    {// 休眠10毫秒以确保有时间执行保存消息的操作
                                        MSLEEP(10);
                                    }
#endif
                                } while(nRet != SUCCESS);
                                                    
                                FREE(pClientMsg);
                                pClientMsg = WmsApp_GetClientMsgMO(pMe, FALSE);
                            }

                            WmsApp_FreeMultiSendList(pMe->m_pSendList);
                            
                            // 释放用户数据列表
                            WmsApp_FreeUserDataMOList(pMe->m_pUserDataMOList);
                            
                            pMe->m_idxUserdata = 0;
                        }
                        else
                        {
                            // 释放用户数据列表
                            WmsApp_FreeUserDataMOList(pMe->m_pUserDataMOList);
                        }
	                
	            	}


					if(pMe->m_eAppStatus == WMSAPP_SUSPEND)
		            {
		                nMode = ITEXTCTL_GetInputMode(pIText,NULL);
		            }

					MSG_FATAL("***zzg WMSDialog WmsApp_ShowMsgBox IDS_SAVED_EX***", 0, 0, 0);
						
					#ifdef FEATURE_VERSION_W317A
					CLOSE_DIALOG(DLGRET_SAVE_TO_DRAFT)
					#else
					CLOSE_DIALOG(DLGRET_CANCELED)
					#endif		

					return TRUE;
					
				}
				//Add End

                //保存为模板短信
                case IDS_SAVEASPRESET:
                    CLOSE_DIALOG(DLGRET_SAVE)
                    return TRUE;

                //发送选项，即原来的sendopt对话框
                case IDS_SETTING:
                    CLOSE_DIALOG(DLGRET_SENDOPT)
                    return TRUE;

                case IDS_EXIT_EDITOR:
                    {
                        AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pIText);
                        int nLen=0;
                         
                        if (NULL != pwstrText)
                        {
                            nLen = WSTRLEN(pwstrText);
                        }
                        
                        if(nLen > 0)
                        {
                            CLOSE_DIALOG(DLGRET_EXIT_EDITOR)
                        }
                        else
                        {
                            CLOSE_DIALOG(DLGRET_CANCELED)
                        }
                        return TRUE;
                     }
                    
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
			                MSG_FATAL("IDD_WRITEMSG_Handler EVT_PEN_UP wXPos=%d, wYPos=%d",wXPos,wYPos,0);
							nBarH = GetBottomBarHeight(pMe->m_pDisplay);
					
							MEMSET(&devinfo, 0, sizeof(devinfo));
							ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
							SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
			
							if(WMSAPP_PT_IN_RECT(wXPos,wYPos,rc))
							{
                                MSG_FATAL("IDD_WRITEMSG_Handler EVT_PEN_UP WMSAPP_PT_IN_RECT",0,0,0);
								if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
								{
                                    MSG_FATAL("IDD_WRITEMSG_Handler EVT_PEN_UP left",0,0,0);
									return IDD_WRITEMSG_Handler((void *)pMe,EVT_KEY,AVK_SELECT,0);
								}
								else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
								{					
									boolean rt;
									int len = WSTRLEN(ITEXTCTL_GetTextPtr(pIText));
			                        MSG_FATAL("IDD_WRITEMSG_Handler EVT_PEN_UP left len=%d",len,0,0);
									if((ITEXTCTL_IsActive(pIText)) && (len > 0))
										return ITEXTCTL_HandleEvent(pIText,EVT_KEY,AVK_CLR,0);
									else
                                        return IDD_WRITEMSG_Handler((void *)pMe, EVT_KEY, AVK_CLR, 0);
								}
							}
			
						}
						return TRUE;
#endif 

        default:
            break;            
    }

    return FALSE;
} // IDD_WRITEMSG_Handler

/*==============================================================================
函数:
    IDD_TEMPLATES_Handler

说明:
    WMS Applet对话框 IDD_TEMPLATES 事件处理函数。显示预置短消息列表。。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_TEMPLATES_Handler(void   *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;
   
    if (pMe == NULL)
    {
        return FALSE;
    }
    pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg,
                                    IDC_TEMPLATES_MENU);

    if (NULL == pMenu)
    {
        return FALSE;
    }
   
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            // 设置菜单矩形区域
            //SetControlRect(pMe,pMenu);
            
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
#endif
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,                                
                        IDS_TEMPLATES,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);
            {// 添加模板项
                CtlAddItem  ai;
                uint16  i;
                wms_cache_info_node  *pnode = NULL;
                #if defined(FEATURE_VERSION_HITZ181)||defined(FEATURE_VERSION_MTM)
                uint16  nCmdID[PHRASE_MAX] = {IDS_TEMPLATEHITZ0,
                                              IDS_TEMPLATEHITZ1,
                                              IDS_TEMPLATEHITZ2,
                                              IDS_TEMPLATEHITZ3,
                                              IDS_TEMPLATEHITZ4,
                                              IDS_TEMPLATEHITZ5,
                                              IDS_TEMPLATEHITZ6,
                                              IDS_TEMPLATEHITZ7,
                                              IDS_TEMPLATEHITZ8,
                                              IDS_TEMPLATEHITZ9,
                                              0};
                #elif defined(FEATURE_VERSION_C337)
                uint16  nCmdID[PHRASE_MAX] = {IDS_TEMPLATETATA1,
                                              IDS_TEMPLATETATA2,
                                              IDS_TEMPLATETATA3,
                                              IDS_TEMPLATETATA4,
                                              IDS_TEMPLATETATA5,
                                              IDS_TEMPLATETATA6,
                                              IDS_TEMPLATETATA7,
                                              IDS_TEMPLATETATA8,
                                              IDS_TEMPLATETATA9,
                                              IDS_TEMPLATETATA10,
                                              0};
                #else
                uint16  nCmdID[PHRASE_MAX] = {IDS_TEMPLATE1,
                                              IDS_TEMPLATE2,
                                              IDS_TEMPLATE3,
                                              IDS_TEMPLATE4,
                                              IDS_TEMPLATE5,
                                              IDS_TEMPLATE6,
                                              IDS_TEMPLATE7,
                                              IDS_TEMPLATE8,
                                              IDS_TEMPLATE9,
                                              IDS_TEMPLATE10,
                                              0};
                #endif
                wms_cacheinfolist_enumbegin(WMS_MB_TEMPLATE);
                pnode = wms_cacheinfolist_enumnext(WMS_MB_TEMPLATE);
                MEMSET(&ai, 0, sizeof(ai));
                ai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
                ai.wImage = IDB_TEMPLATE;
                while (NULL != pnode)
                {
                    if (pnode->mem_store == WMS_MEMORY_STORE_RUIM)
                    {// 卡上模板
                        ai.wItemID = pnode->index+MSG_CMD_BASE;
                        ai.pszResText = NULL;
                        if (NULL != pnode->pwszTitle)
                        {
                            ai.pText = pnode->pwszTitle;
                            (void)IMENUCTL_AddItemEx(pMenu, &ai);
                        }
                        pnode = wms_cacheinfolist_enumnext(WMS_MB_TEMPLATE);
                        continue;
                    }
                    i = pnode->index - PHRASE_START;
                    nCmdID[i] = pnode->index+MSG_CMD_BASE;
                    pnode = wms_cacheinfolist_enumnext(WMS_MB_TEMPLATE);
                }
                for (i=0; i<PHRASE_MAX; i++)
                {
                    ai.wItemID = nCmdID[i];
                    
                    if ((ai.wItemID<MSG_CMD_BASE) && (ai.wItemID > 0))
                    {// 内置常用语
                        ai.pText = NULL;
                        ai.pszResText = AEE_WMSAPPRES_LANGFILE;
                        ai.wText = ai.wItemID;
                        (void)IMENUCTL_AddItemEx(pMenu, &ai);
                    }
                    else
                    {// 用户常用语
                        ai.pszResText = NULL;
                        pnode = wms_cacheinfolist_getnode(WMS_MB_TEMPLATE, WMS_MEMORY_STORE_NV_CDMA, nCmdID[i]-MSG_CMD_BASE);
                        if ((NULL != pnode) && (NULL != pnode->pwszTitle))
                        {
                            ai.pText = pnode->pwszTitle;
                            (void)IMENUCTL_AddItemEx(pMenu, &ai);
                        }
                    }
                }
                IMENUCTL_SetSel(pMenu, pMe->m_wCurTemplate);
            }
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            
            return TRUE;

        case EVT_USER_REDRAW:
            // 绘制底条提示
            // Select       Back
            //DRAW_BOTTOMBAR(BTBAR_SELECT_BACK)
            
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            //(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                    
                case AVK_INFO:
                    {
                        uint16 nSelID;
                        
                        nSelID = IMENUCTL_GetSel(pMenu);
                        if (pMe->m_eInsertType == INSERT_NONE)
                        {
                            pMe->m_wCurTemplate = nSelID;
                        }
                        pMe->m_wInsertTemplate = nSelID;
                    }
                    CLOSE_DIALOG(DLGRET_LOAD)
                    return TRUE;
   
                default:
                    break;
            }
            return FALSE;

        case EVT_COMMAND:
            if (pMe->m_eInsertType == INSERT_NONE)
            {
                pMe->m_wCurTemplate = wParam;
            }
            pMe->m_wInsertTemplate = wParam;
            CLOSE_DIALOG(DLGRET_OPT)
            
            return TRUE;
            
        default:
            break;
    }

    return FALSE;
} // IDD_TEMPLATES_Handler

/*==============================================================================
函数:
    IDD_EDITTEMPLATE_Handler

说明:
    WMS Applet对话框 IDD_EDITTEMPLATE 事件处理函数。处理消息发往目标地址输入。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_EDITTEMPLATE_Handler(void *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    ITextCtl *pIText = NULL;
    WmsApp *pMe = (WmsApp *)pUser;
   
    if (NULL == pMe)
    {
        return FALSE;
    }

    pIText = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_EDITTEMPLATE_TEXT);

    if (NULL == pIText)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            // add the code on 080818
            ITEXTCTL_SetMaxSize(pIText, WMS_TEMPMSG_MAXCHARS);
			#ifdef FEATURE_VERSION_C316
			ITEXTCTL_SetProperties(pIText, TP_GRAPHIC_BGBLUE|TP_GRAPHIC_BG| TP_FRAME | TP_MULTILINE|TP_STARKEY_SWITCH | TP_DISPLAY_COUNT | TP_NOUPDATE|TP_FOCUS_NOSEL);
			#else
            ITEXTCTL_SetProperties(pIText, TP_GRAPHIC_BG| TP_FRAME | TP_MULTILINE|TP_STARKEY_SWITCH | TP_DISPLAY_COUNT | TP_NOUPDATE|TP_FOCUS_NOSEL);
			#endif
            {
                AEERect rc;
                
                SETAEERECT(&rc, 0, 0, pMe->m_rc.dx, pMe->m_rc.dy - GetBottomBarHeight(pMe->m_pDisplay));
                ITEXTCTL_SetRect(pIText, &rc);
                
                if (NULL != pMe->m_msSend.m_szMessage)
                {
                    (void)ITEXTCTL_SetText(pIText, pMe->m_msSend.m_szMessage, -1);
                }
            }
            // add end
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);            
            return TRUE;

        case  EVT_USER_REDRAW:
            if (pMe->m_dwInsertPos == 0)
            {
                ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
            }
            else
            {
                ITEXTCTL_SetCursorPos(pIText, pMe->m_dwInsertPos);
            }
            ITEXTCTL_SetActive(pIText, TRUE);
            
            // 绘制底条提示
            {
                AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pIText);
                int nLen=0;
                
                if (NULL != pwstrText)
                {
                    nLen = WSTRLEN(pwstrText);
                }
                #ifndef FEATURE_ALL_KEY_PAD
                if (nLen > 0)
                {
                    // Save       Delete
                    DRAW_BOTTOMBAR(BTBAR_SAVE_DELETE)
                }
                else
                #endif
                {
                    // Save       Back
                    DRAW_BOTTOMBAR(BTBAR_SAVE_BACK)
                }
            }
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            return TRUE;
            
        case EVT_DIALOG_END:
            {
                AECHAR *pWstr = ITEXTCTL_GetTextPtr(pIText);
                
                if (NULL != pMe->m_msSend.m_szMessage)
                {
                    FREE(pMe->m_msSend.m_szMessage);
                    pMe->m_msSend.m_szMessage = NULL;
                }
                if ((NULL != pWstr) && (WSTRLEN(pWstr)>0))
                {
                    pMe->m_msSend.m_szMessage = WSTRDUP(pWstr);
                }
            }
            pMe->m_dwInsertPos = ITEXTCTL_GetCursorPos(pIText);
            return TRUE;
            
        //进入符号输入方式返回重新画图
        case EVT_CTL_TEXT_MODECHANGED:  //切换输入法
            {
                AEETextInputMode nInputMode;
                AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pIText);
                int nLen=0;
                
                nInputMode = ITEXTCTL_GetInputMode(pIText,NULL);
                
                if (NULL != pwstrText)
                {
                    nLen = WSTRLEN(pwstrText);
                }
   
                if (nInputMode != AEE_TM_SYMBOLS && nInputMode != AEE_TM_FACE_SYMBOL)
                {
                    // 绘制底条提示
                    #ifndef FEATURE_ALL_KEY_PAD
                    if (nLen > 0)
                    {
                        // Save     Delete
                        DRAW_BOTTOMBAR(BTBAR_SAVE_DELETE)
                    }
                    else
                    #endif
                    {
                        // Save     Back 
                        DRAW_BOTTOMBAR(BTBAR_SAVE_BACK)
                    }
                    
                    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                }
            }
            return TRUE; 
            
        case EVT_KEY_RELEASE:
        case EVT_UPDATE_ARROW_BUTTON: 
            {
                AEETextInputMode nInputMode;
                AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pIText);
                int nLen=0;
                
                nInputMode = ITEXTCTL_GetInputMode(pIText,NULL);
                if (nInputMode == AEE_TM_SYMBOLS)
                {
                    return TRUE;
                }
                
                if (NULL != pwstrText)
                {
                    nLen = WSTRLEN(pwstrText);
                }
            
                // 绘制底条提示
                #ifndef FEATURE_ALL_KEY_PAD
                if (nLen >= 1)
                {
                    // Save     Delete
                    DRAW_BOTTOMBAR(BTBAR_SAVE_DELETE)
                    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                }
                else if (nLen <= 0)
                #endif
                {
                    // Save     Back 
                    DRAW_BOTTOMBAR(BTBAR_SAVE_BACK)
                    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                }
            }
            
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
   
                case AVK_INFO:
                case AVK_SELECT:
                    CLOSE_DIALOG(DLGRET_SAVE)
                    return TRUE;
                    
                default:
                    break;
            }
            return FALSE;
            
        case EVT_CTL_TEXT_TEXTFULL:
            CLOSE_DIALOG(DLGRET_TEXTFULL)
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

				if(WMSAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt;
						int len = WSTRLEN(ITEXTCTL_GetTextPtr(pIText));

						if((ITEXTCTL_IsActive(pIText)) && (len > 0))
							rt = ITEXTCTL_HandleEvent(pIText,EVT_KEY,AVK_CLR,0);
						else
							rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_CLR,0);
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
} // IDD_EDITTEMPLATE_Handler

/*==============================================================================
函数:
    SetControlRect

说明:
    设置控件的矩形框。

参数:
    pMe [in]:指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    Ctl [in]:可转换为IControl接口指针的指针。

返回值:
    无。

备注:

==============================================================================*/
static void SetControlRect(WmsApp *pMe, void  *Ctl)
{
    AEERect ctlRect;
	
    if (pMe == NULL)
    {
        return;
    }
   
    SETAEERECT(&ctlRect,  0, 0,
                pMe->m_rc.dx,
                pMe->m_rc.dy - GetBottomBarHeight(pMe->m_pDisplay));
    ICONTROL_SetRect((IControl*)Ctl, &ctlRect);
}// SetControlRect

/*==============================================================================
函数:
    IDD_MSGOPTS_Handler

说明:
    WMS Applet对话框 IDD_MSGOPTS 事件处理函数。显示针对收件箱内一条消息
    的相关操作选项。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_MSGOPTS_Handler(void *pUser,
    AEEEvent     eCode,
    uint16       wParam,
    uint32       dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;
    AEERect rc={0};
    AEERect Temprc={0};
    if (pMe == NULL)
    {
        return FALSE;
    }
    
    pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_MSGOPTS_MENU);
    if (NULL == pMenu)
    {
        return FALSE;
    }
    
    switch (eCode) 
    {
        case EVT_DIALOG_INIT:
            return TRUE;
        
        case EVT_DIALOG_START:
            // 设置菜单矩形区域
            SetControlRect(pMe,pMenu);
            
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);
            // 此对话框为几个状态公用，菜单项差别较大，故采取动态建立菜单项的方式
            if (WMSST_TEMPLATESOPTS == pMe->m_currState)
            {
                MSG_FATAL("pMe->m_eOptType=%d",pMe->m_eOptType,0,0);
                if(pMe->m_eOptType == OPT_VIA_VIEWMSG)
                {
                    MENU_ADDITEM(pMenu, IDS_EDIT);
                    MENU_ADDITEM(pMenu, IDS_SEND);
                }
                else
                {
#ifndef FEATURE_CARRIER_THAILAND_HUTCH               
                    if (pMe->m_eOptType == OPT_VIA_LISTMSG)
                    {
                        MENU_ADDITEM(pMenu, IDS_VIEW);
                    }
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH
                    MENU_ADDITEM(pMenu, IDS_SEND);
                    //MENU_ADDITEM(pMenu, IDS_EDIT);
                    MENU_ADDITEM(pMenu, IDS_ADDNEW);
                    
                    if (pMe->m_wCurTemplate>=MSG_CMD_BASE)
                    {
                        // 用户常用语才允许删除
                        MENU_ADDITEM(pMenu, IDS_EDIT);
                        MENU_ADDITEM(pMenu, IDS_DELETE);
                    }
                }
            }
            else
            {
                switch (pMe->m_eMBoxType)
                {
#ifdef FEATURE_USES_MMS
                    case WMS_MB_OUTBOX_MMS:
                    {
                        MENU_ADDITEM(pMenu, IDS_RESEND);
                        MENU_ADDITEM(pMenu, IDS_DELETE);
                        MENU_ADDITEM(pMenu, IDS_DELETEALL);
                    }
                    break;
                    case WMS_MB_INBOX_MMS:
                    {
                        MENU_ADDITEM(pMenu, IDS_DELETE);
                        MENU_ADDITEM(pMenu, IDS_DELETEALL);
                    }
                    break;
                    case WMS_MB_DRAFTBOX_MMS:
                    {
                        MENU_ADDITEM(pMenu, IDS_DELETE);
                        MENU_ADDITEM(pMenu, IDS_DELETEALL);
                    }
                    break;                    
#endif
                    case WMS_MB_INBOX:
                        if (pMe->m_eOptType == OPT_VIA_LISTMSG)
                        {
                            MENU_ADDITEM(pMenu, IDS_VIEW);
                        }
                        MENU_ADDITEM(pMenu, IDS_REPLY);
                        MENU_ADDITEM(pMenu, IDS_FORWARD);
                        MENU_ADDITEM(pMenu, IDS_CALL);
                        MENU_ADDITEM(pMenu, IDS_DELETE);
                        MENU_ADDITEM(pMenu, IDS_DELETEALL);
#ifdef FEATURE_VERSION_W208S                        
                        MENU_ADDITEM(pMenu, IDS_ADD_TO_RESTRICT_LIST);
#endif
                        if (pMe->m_eOptType != OPT_VIA_LISTMSG)
                        {
#ifdef FEATURE_CONTACT_APP
                            MENU_ADDITEM(pMenu, IDS_SAVENUM);
                            MENU_ADDITEM(pMenu, IDS_SAVEEMAIL);
                            MENU_ADDITEM(pMenu, IDS_SAVEURL);
                            //MENU_ADDITEM(pMenu, IDS_EXTRACTDETAIL);
#endif               
                        }
#ifdef FEATRUE_SET_IP_NUMBER
                        MENU_ADDITEM(pMenu, IDS_IPCALL);
#endif
    
                        if (IsRunAsUIMVersion())
                        {     
#ifdef FEATURE_CDSMS_RUIM
                            if (pMe->m_eCurStore == WMS_MEMORY_STORE_NV_CDMA)
                            {
                                MENU_ADDITEM(pMenu, IDS_COPYTORUIM);
                                MENU_ADDITEM(pMenu, IDS_MOVETORUIM);
                            }
                            else if (pMe->m_eCurStore == WMS_MEMORY_STORE_RUIM)
                            {
                                MENU_ADDITEM(pMenu, IDS_COPYTOPNONE);
                                MENU_ADDITEM(pMenu, IDS_MOVETOPHONE);
                            }
#endif
                        }
                        break;
                    
                    case WMS_MB_OUTBOX:
                        if (pMe->m_eOptType == OPT_VIA_LISTMSG)
                        {
                            MENU_ADDITEM(pMenu, IDS_VIEW);
                        }
						#ifndef FEATURE_VERSION_W208S						
                        MENU_ADDITEM(pMenu, IDS_RESEND);
						#endif
                        MENU_ADDITEM(pMenu, IDS_FORWARD);
                        MENU_ADDITEM(pMenu, IDS_DELETE);
                        MENU_ADDITEM(pMenu, IDS_DELETEALL);
#ifdef FEATURE_VERSION_W208S                        
                        MENU_ADDITEM(pMenu, IDS_ADD_TO_RESTRICT_LIST);
#endif
                        break;
                        
                    case WMS_MB_DRAFT:
                        if (pMe->m_eOptType == OPT_VIA_LISTMSG)
                        {
                            MENU_ADDITEM(pMenu, IDS_VIEW);
                        }
                        MENU_ADDITEM(pMenu, IDS_EDIT);
                        MENU_ADDITEM(pMenu, IDS_DELETE);
                        MENU_ADDITEM(pMenu, IDS_DELETEALL);
                        //MENU_ADDITEM(pMenu, IDS_FORWARD);
#ifdef FEATURE_VERSION_W208S                        
                        MENU_ADDITEM(pMenu, IDS_ADD_TO_RESTRICT_LIST);
#endif
                        break;
                
                    case WMS_MB_VOICEMAIL:
                        MENU_ADDITEM(pMenu, IDS_LISTENVM);
#if !defined(FEATURE_CARRIER_VENEZUELA_MOVILNET)
                        MENU_ADDITEM(pMenu, IDS_DELETE);
#endif                        
                        break;
                        
                    case WMS_MB_RESERVE:
                    case WMS_MB_RSVFAILED:
                        if (pMe->m_eOptType == OPT_VIA_LISTMSG)
                        {
                            MENU_ADDITEM(pMenu, IDS_VIEW);
                        }
                        MENU_ADDITEM(pMenu, IDS_EDIT);
                        if(pMe->m_eMBoxType == WMS_MB_RESERVE)
                        {
                            MENU_ADDITEM(pMenu, IDS_SEND);
                        }
                        else
                        {
                            MENU_ADDITEM(pMenu, IDS_RESEND);
                        }
                        MENU_ADDITEM(pMenu, IDS_DELETE);
                        MENU_ADDITEM(pMenu, IDS_DELETEALL);
                        break;
                    
                    default:
                        return TRUE;
                }
            }
            IMENUCTL_SetPopMenuRect(pMenu);
			
            IMENUCTL_SetProperties(pMenu,MP_BIND_ITEM_TO_NUMBER_KEY|MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;
            
        case EVT_USER_REDRAW:
            // 绘制底条提示
            // Select       Back
            //DRAW_BOTTOMBAR(BTBAR_SELECT_BACK)
            
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            //(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;
            
        case EVT_KEY:
            switch (wParam) 
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                    
                default:
                    break;
            }
            return FALSE;

        case EVT_COMMAND:
            switch (wParam)
            {
                // 查看
                case IDS_VIEW:
                    CLOSE_DIALOG(DLGRET_VIEW)
                    return TRUE;
                    
                // 回复
                case IDS_REPLY:
                    CLOSE_DIALOG(DLGRET_REPLY)
                    return TRUE;
                    
                // 转发
                case IDS_FORWARD:
                    CLOSE_DIALOG(DLGRET_FORWARD)
                    return TRUE;
                    
#ifdef FEATURE_CONTACT_APP
                //提取号码
                case IDS_SAVENUM:
                    CLOSE_DIALOG(DLGRET_SAVENUM)
                    return TRUE;

                //提取email
                case IDS_SAVEEMAIL:
                    CLOSE_DIALOG(DLGRET_SAVEEMAIL)
                    return TRUE;

                //提取URL
                case IDS_SAVEURL:
                    CLOSE_DIALOG(DLGRET_SAVEURL)
                    return TRUE;

                // 提取详情
                case IDS_EXTRACTDETAIL:
                    CLOSE_DIALOG(DLGRET_EXTACTDETAILS)
                    return TRUE;
#endif   

                // 拨打号码
                case IDS_CALL:
#if 1//defined(FEATURE_CARRIER_VENEZUELA_MOVILNET) || defined(FEATURE_CARRIER_THAILAND_HUTCH)
                    // 该运营商要求 CBN 优先
                    if (WSTRLEN(pMe->m_msCur.m_szCallBkNum) > 0)
                    {
                        // 调用呼叫接口，本 Applet 会被挂起，返回时回到当前状态
                        WMSExtApp_CallPhoneNumber(pMe, pMe->m_msCur.m_szCallBkNum, FALSE);
                    }
                    else 
#endif                    
                    if (WSTRLEN(pMe->m_msCur.m_szNum) > 0)
                    {
                        // 调用呼叫接口，本 Applet 会被挂起，返回时回到当前状态
                        WMSExtApp_CallPhoneNumber(pMe, pMe->m_msCur.m_szNum, FALSE);
                    }
                    return TRUE;
                    
#ifdef FEATRUE_SET_IP_NUMBER
                // 拨打IP号码
                case IDS_IPCALL:
#if 1//defined(FEATURE_CARRIER_VENEZUELA_MOVILNET)
                    // 该运营商要求 CBN 优先
                    if (WSTRLEN(pMe->m_msCur.m_szCallBkNum) > 0)
                    {
                        // 调用呼叫接口，本 Applet 会被挂起，返回时回到当前状态
                        WMSExtApp_CallPhoneNumber(pMe, pMe->m_msCur.m_szCallBkNum, TRUE);
                    }
                    else 
#endif                    
                    if (WSTRLEN(pMe->m_msCur.m_szNum) > 0)
                    {
                        // 调用呼叫接口，本 Applet 会被挂起，返回时回到当前状态
                        WMSExtApp_CallPhoneNumber(pMe, pMe->m_msCur.m_szNum, TRUE);
                    }
                    return TRUE;
#endif
                // 删除
                case IDS_DELETE:
                    CLOSE_DIALOG(DLGRET_DELETE)
                    return TRUE;
                    
                // 删除全部
                case IDS_DELETEALL:
                    CLOSE_DIALOG(DLGRET_CLEARALL)
                    return TRUE;
#ifdef FEATURE_VERSION_W208S
                // 添加到黑名单
                case IDS_ADD_TO_RESTRICT_LIST:
                    {
                        uint8 byMax = 0;
                        sms_restrict_recive_info		sms_restrict_reciveList[MAX_SMS_RESTRICT];
                        MSG_FATAL("IDS_ADD_TO_RESTRICT_LIST",0,0,0);
                        DBGPRINTF("pMe->m_msCur.m_szNum=%S",pMe->m_msCur.m_szNum);
                        if (WSTRLEN(pMe->m_msCur.m_szNum) > 0)
                        {
                            sms_restrict_recive_info info = {0};
                            
                            (void) ICONFIG_GetItem(pMe->m_pConfig,
                            CFGI_SMS_RESTRICT_RECEIVE_TOTAL,
                            &byMax,
                            sizeof(byte));  
                            
                        	//初始化拒收短信黑名单的信息
                        	(void) ICONFIG_GetItem(pMe->m_pConfig,
                        						   CFGI_SMS_RESTRICT_RECEIVE_INFO,
                        						   (void*)sms_restrict_reciveList,
                        						   sizeof(sms_restrict_reciveList));           
                            
                            MEMCPY(sms_restrict_reciveList[byMax].szName, pMe->m_msCur.m_szNum, sizeof(sms_restrict_reciveList[byMax].szName));
                            byMax++;
                            DBGPRINTF("szName=%S, byMax=%d",pMe->m_msCur.m_szNum, byMax);
                           (void) ICONFIG_SetItem(pMe->m_pConfig,
                                                  CFGI_SMS_RESTRICT_RECEIVE_TOTAL,
                                                  &byMax,
                                                  sizeof(uint8));
                           
                           (void) ICONFIG_SetItem(pMe->m_pConfig,
                                                  CFGI_SMS_RESTRICT_RECEIVE_INFO,
                                                  (void*) sms_restrict_reciveList,
                                                  sizeof(sms_restrict_reciveList));                           
                        }

                    }
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;                    
#endif                    
#ifdef FEATURE_CDSMS_RUIM
                // 复制到 UIM
                case IDS_COPYTORUIM:
                    CLOSE_DIALOG(DLGRET_COPYTORUIM)
                    return TRUE;
                    
                // 复制到 Phone
                case IDS_COPYTOPNONE:
                    CLOSE_DIALOG(DLGRET_COPYTOPNONE)
                    return TRUE;
                    
                // 移至 UIM
                case IDS_MOVETORUIM:
                    CLOSE_DIALOG(DLGRET_MOVETORUIM)
                    return TRUE;
                    
                // 移至 Phone
                case IDS_MOVETOPHONE:
                    CLOSE_DIALOG(DLGRET_MOVETOPNONE)
                    return TRUE;
#endif                    
                    
                //重发
                case IDS_RESEND: 
                    CLOSE_DIALOG(DLGRET_RESEND)
                    return TRUE;
                
                //听语音留言
                case IDS_LISTENVM:
                    {
                        AECHAR  voiceMailNum[MAX_PH_DIGITS];
                        MEMSET(voiceMailNum, 0, sizeof(voiceMailNum));
                                
                        if (SUCCESS == ICONFIG_GetItem(pMe->m_pConfig,
                                    CFGI_VOICEMAIL_NUMBER,
                                    voiceMailNum,
                                    sizeof(voiceMailNum)))
                        {
                            // 调用呼叫接口，本 Applet 会被挂起，返回时回到当前状态
                            WMSExtApp_CallPhoneNumber(pMe, voiceMailNum, FALSE);
                        }
                    }
                    return TRUE;
                    
                case IDS_SEND:
                    CLOSE_DIALOG(DLGRET_SEND)
                    return TRUE;
                    
                case IDS_EDIT:
                    CLOSE_DIALOG(DLGRET_EDIT)
                    return TRUE;
                    
                case IDS_ADDNEW:
                    CLOSE_DIALOG(DLGRET_ADDNEW)
                    return TRUE;
                    
                default:
                    break;
            }
            return FALSE;
            
        default:
            break;
    }
    
    return FALSE;
}  // IDD_MSGOPTS_Handler

#ifdef FEATURE_CDSMS_RUIM
/*==============================================================================
函数:
    IDD_MSGCOPY_Handler

说明:
    WMS Applet对话框 IDD_MSGCOPY 事件处理函数。显示针对收件箱内一条消息
    的相关操作选项。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_MSGCOPY_Handler(void *pUser,
    AEEEvent     eCode,
    uint16       wParam,
    uint32       dwParam
)
{
    WmsApp *pMe = (WmsApp *)pUser;
    
    if (pMe == NULL)
    {
        return FALSE;
    }
    
    if (NULL == pMe->m_pStatic)
    {
        return FALSE;
    }

	MSG_FATAL("***zzg IDD_MSGCOPY_Handler eCode=%x***", eCode, 0, 0);
    
    switch (eCode) 
    {
        case EVT_DIALOG_INIT:
            pMe->m_idxCur = 0;
            return TRUE;
        
        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;
            
        case EVT_USER_REDRAW:
            // 绘制提示界面
            {
                wms_msg_copy_type copy_type;
                                
                copy_type.memStore_src = WMS_MEMORY_STORE_RUIM;
                copy_type.memStore_dest = WMS_MEMORY_STORE_NV_CDMA;
                copy_type.index_dest = WMS_DUMMY_MESSAGE_INDEX;
                copy_type.delete_src = FALSE;
                switch (pMe->m_CopyType)
                {
                    case MOVETORUIM:
                    case COPYTORUIM:
                        copy_type.memStore_src = WMS_MEMORY_STORE_NV_CDMA;
                        copy_type.memStore_dest = WMS_MEMORY_STORE_RUIM;
                        if (pMe->m_CopyType == MOVETORUIM)
                        {
                            copy_type.delete_src = TRUE;
                        }
                        break;
                        
                    case MOVETOPHONEINBOX:
                    case COPYTOPHONEINBOXE:
                        if (pMe->m_CopyType == MOVETOPHONEINBOX)
                        {
                            copy_type.delete_src = TRUE;
                        }
                        break;
                    
                    default:
                        CLOSE_DIALOG(DLGRET_OK)
                        return TRUE;
                }
                
                if (pMe->m_idxCur == 0)
                {
                    AECHAR *wstrTips = NULL;
                    int32  nSize;
                    PromptMsg_Param_type  Msg_Param={0};
                    
                    nSize = 64 * sizeof(AECHAR);
                    wstrTips = (AECHAR *)MALLOC(nSize);
                    DBGPRINTF("WMS Address = 0x%x", wstrTips);
                    if (NULL == wstrTips)
                    {
                        CLOSE_DIALOG(DLGRET_OK)
                        return TRUE;
                    }
                    
                    (void)ISHELL_LoadResString(pMe->m_pShell, 
                                AEE_WMSAPPRES_LANGFILE,
                                (copy_type.delete_src ? IDS_MSGMOVING : IDS_MSGCOPYING),
                                wstrTips, 
                                nSize);
                                
                    // 调用公共函数绘制界面
                    Msg_Param.ePMsgType = MESSAGE_WAITING;
                    Msg_Param.pwszMsg = wstrTips;
                    Msg_Param.eBBarType = BTBAR_NONE;
                    DrawPromptMessage(pMe->m_pDisplay, pMe->m_pStatic, &Msg_Param);
                    
                    FREE(wstrTips);
                }
                
                
                // 启动消息复制/移动过程
                for (; pMe->m_idxCur<LONGSMS_MAX_PACKAGES; pMe->m_idxCur++)
                {
                    if ((pMe->m_CurMsgNodes[pMe->m_idxCur] != NULL) &&
                        (pMe->m_CurMsgNodes[pMe->m_idxCur]->mem_store != copy_type.memStore_dest))
                    {
                        int nRet;
                        
                        copy_type.index_src = pMe->m_CurMsgNodes[pMe->m_idxCur]->index;
                        
                        // 发送读消息命令
                        nRet = IWMS_MsgCopy(pMe->m_pwms,
                                            pMe->m_clientId,
                                            &pMe->m_callback,
                                            (void *)pMe,
                                            &copy_type);
                                           
                        if (nRet != SUCCESS)
                        {
                            CLOSE_DIALOG(DLGRET_OK)
                            return TRUE;
                        }
                        break;
                    }
                }
                
                if (pMe->m_idxCur == LONGSMS_MAX_PACKAGES)
                {// 复制/移动完成
                    CLOSE_DIALOG(DLGRET_OK)
                    return TRUE;
                }
            }
            return TRUE;
            
        case EVT_DIALOG_END:
            pMe->m_eDlgReturn = DLGRET_OK;
            ISTATIC_Release(pMe->m_pStatic);
            pMe->m_pStatic = NULL;
            return TRUE;
            
        case EVT_KEY:
            return TRUE;
            
        case EVT_WMS_CMD_STATUS:
            {
                wmsapp_cmd_status_type *pStatus = (wmsapp_cmd_status_type *)dwParam;
                
                if ((NULL == pStatus) || (pStatus->cmd != WMS_CMD_MSG_COPY))
                {
                    return TRUE;
                }
                
                ERR("Cmd:%d---Err:%d", pStatus->cmd, pStatus->cmd_err, 0);
                
                pMe->m_idxCur++;
                (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                        EVTFLG_ASYNC,
                                        AEECLSID_WMSAPP,
                                        EVT_USER_REDRAW,
                                        0, 
                                        0);
            }
            
            return TRUE;

        default:
            break;
    }
    
    return FALSE;
}  // IDD_MSGCOPY_Handler

/*==============================================================================
函数:
    IDD_MSGCOPY_HandlerExt

说明:
    WMS Applet对话框 IDD_MSGCOPY 事件处理函数。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_MSGCOPY_HandlerExt(void *pUser,
    AEEEvent     eCode,
    uint16       wParam,
    uint32       dwParam
)
{
    static wms_message_list_s_type *pSrcList = NULL;
    WmsApp *pMe = (WmsApp *)pUser;
    static int nCopyNum;
    
    if (pMe == NULL)
    {
        return FALSE;
    }
    
    if (NULL == pMe->m_pStatic)
    {
        return FALSE;
    }

	MSG_FATAL("***zzg IDD_MSGCOPY_HandlerExt eCode=%x***", eCode, 0, 0);
	
    switch (eCode) 
    {
        case EVT_DIALOG_INIT:
            nCopyNum = pMe->m_nCopyNum;
            return TRUE;
        
        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;
            
        case EVT_USER_REDRAW:
            // 绘制提示界面
            {
                AECHAR *wstrTips = NULL;
                int32  nSize;
                PromptMsg_Param_type  Msg_Param={0};
                wms_memory_store_e_type mem_store;
                int nRet;
                
                nSize = 64 * sizeof(AECHAR);
                wstrTips = (AECHAR *)MALLOC(nSize);
                DBGPRINTF("WMS Address = 0x%x", wstrTips);
                if (NULL == wstrTips)
                {
                    CLOSE_DIALOG(DLGRET_OK)
                    return TRUE;
                }
                
                if( COPYALLMTTORUIM == pMe->m_CopyType || COPYALLRUIMTOPHONE == pMe->m_CopyType)
                {
                    (void)ISHELL_LoadResString(pMe->m_pShell, 
                                AEE_WMSAPPRES_LANGFILE,
                                IDS_MSGCOPYING,
                                wstrTips, 
                                nSize);
                }
                else if( MOVEALLMTTORUIM == pMe->m_CopyType || MOVEALLRUIMTOPHONE == pMe->m_CopyType)
                {
                    (void)ISHELL_LoadResString(pMe->m_pShell,
                                AEE_WMSAPPRES_LANGFILE,
                                IDS_MSGMOVING,
                                wstrTips,
                                nSize);
                }
                            
                // 调用公共函数绘制界面
                Msg_Param.ePMsgType = MESSAGE_WAITING;
                Msg_Param.pwszMsg = wstrTips;
                Msg_Param.eBBarType = BTBAR_NONE;
                DrawPromptMessage(pMe->m_pDisplay, pMe->m_pStatic, &Msg_Param);
                FREE(wstrTips);
                
                if (pMe->m_CopyType == COPYALLRUIMTOPHONE || pMe->m_CopyType == MOVEALLRUIMTOPHONE)
                {
                    mem_store = WMS_MEMORY_STORE_RUIM;
                }
                else
                {
                    mem_store = WMS_MEMORY_STORE_NV_CDMA;
                }
                
                // 发送获取复制源消息列表命令
                nRet = IWMS_CfgGetMessageList(pMe->m_pwms,
                                    pMe->m_clientId,
                                    &pMe->m_callback,
                                    (void *)pMe,
                                    mem_store,
                                    WMS_TAG_NONE);
                                    
                if (nRet != SUCCESS)
                {
                    CLOSE_DIALOG(DLGRET_OK)
                    return TRUE;
                }
                
            }
            return TRUE;
            
        case EVT_DIALOG_END:
            //pMe->m_eDlgReturn = DLGRET_OK; 
            ISTATIC_Release(pMe->m_pStatic);
            pMe->m_pStatic = NULL;
            FREEIF(pSrcList);
            return TRUE;
            
        case EVT_KEY:
            return TRUE;
            
        case EVT_WMS_CMD_STATUS:
            {
                wmsapp_cmd_status_type  *pStatus = (wmsapp_cmd_status_type *)dwParam;
                
                if (NULL == pStatus)
                {
                    return TRUE;
                }
                
                switch (pStatus->cmd)
                {
                    case WMS_CMD_CFG_GET_MESSAGE_LIST:
                    case WMS_CMD_MSG_READ:
                        if (WMS_CMD_ERR_NONE != pStatus->cmd_err)
                        {
                            CLOSE_DIALOG(DLGRET_OK)
                        }
                        break;
                        
                    default:
                        break;
                }
            }
            return TRUE;
            
        case EVT_WMS_CFG_MESSAGE_LIST:
            if ((NULL != pSrcList) || (NULL == dwParam))
            {
                return TRUE;
            }
            {
                wms_cfg_event_info_s_type *Info = (wms_cfg_event_info_s_type*)dwParam;
                wms_memory_store_e_type mem_store;
                
                if (pMe->m_CopyType == COPYALLRUIMTOPHONE || pMe->m_CopyType == MOVEALLRUIMTOPHONE)
                {
                    mem_store = WMS_MEMORY_STORE_RUIM;
                }
                else
                {
                    mem_store = WMS_MEMORY_STORE_NV_CDMA;
                }
                
                if (Info->message_list.mem_store != mem_store)
                {
                    break;
                }
                
                pSrcList = (wms_message_list_s_type *)MALLOC(sizeof(wms_message_list_s_type));
                if (NULL == pSrcList)
                {
                    CLOSE_DIALOG(DLGRET_OK)
                    break;
                }
                MEMCPY(pSrcList, &Info->message_list, sizeof(wms_message_list_s_type));
                
                // 这里利用 pSrcList->voice_mail_index 变量来做索引变量
                pSrcList->voice_mail_index = 0;
                
                // 发送下一轮拷贝命令
                (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                        EVTFLG_ASYNC,
                                        AEECLSID_WMSAPP,
                                        EVT_MSGCOPY,
                                        0, 
                                        0);
            }
            return TRUE;
            
        case EVT_MSGCOPY:
            {
                int nMinIndex, nMaxIndex;
                int nRet;
                
                if (pMe->m_CopyType == COPYALLRUIMTOPHONE || pMe->m_CopyType == MOVEALLRUIMTOPHONE)
                {
                    nMinIndex = 0;
                    nMaxIndex = pMe->m_memruimStatuse.max_slots-1;
                }
                else
                {
                    nMinIndex = IN_START;
                    nMaxIndex = IN_END;
                }
                
                if(nCopyNum == 0)
                {
                    CLOSE_DIALOG(DLGRET_OK)
                    return TRUE;
                }
                nCopyNum --;
                
                for (; pSrcList->voice_mail_index<pSrcList->len; pSrcList->voice_mail_index++)
                {
                    if ((pSrcList->indices[pSrcList->voice_mail_index]>=nMinIndex) &&
                        (pSrcList->indices[pSrcList->voice_mail_index]<=nMaxIndex) &&
                        (pSrcList->tags[pSrcList->voice_mail_index]==WMS_TAG_MT_READ ||
                         pSrcList->tags[pSrcList->voice_mail_index]==WMS_TAG_MT_NOT_READ))
                    {
                        // 发送读消息命令
                        nRet = IWMS_MsgRead(pMe->m_pwms,
                                           pMe->m_clientId,
                                           &pMe->m_callback,
                                           (void *)pMe,
                                           pSrcList->mem_store,
                                           pSrcList->indices[pSrcList->voice_mail_index]);
                                           
                        if (nRet != SUCCESS)
                        {
                            CLOSE_DIALOG(DLGRET_OK)
                        }
                        return TRUE;
                    }
                }
            }
            // 没有符合条件的记录
            CLOSE_DIALOG(DLGRET_OK)
            
            return TRUE;
            
        case EVT_WMS_MSG_READ:
            if (NULL != pSrcList)
            {// 读取消息
                wms_memory_store_e_type mem_store;
                wms_message_index_type  wIdx;
                wms_msg_event_info_s_type *Info = (wms_msg_event_info_s_type*)dwParam;
                
                mem_store = Info->status_info.message.msg_hdr.mem_store;
                wIdx = Info->status_info.message.msg_hdr.index;
                
                if ((mem_store == pSrcList->mem_store) && 
                    (pSrcList->indices[pSrcList->voice_mail_index] == wIdx))
                {
                    if (pMe->m_CopyType == COPYALLRUIMTOPHONE || pMe->m_CopyType == MOVEALLRUIMTOPHONE)
                    {
                        Info->status_info.message.msg_hdr.mem_store = WMS_MEMORY_STORE_NV_CDMA;
                    }
                    else
                    {
                        Info->status_info.message.msg_hdr.mem_store = WMS_MEMORY_STORE_RUIM;
                    }
                    
                    // 将消息写入目标地址
                    (void)IWMS_MsgWrite(pMe->m_pwms, 
                                        pMe->m_clientId, 
                                        &pMe->m_callback,
                                        (void*)pMe,
                                        WMS_WRITE_MODE_INSERT,
                                        &(Info->status_info.message));
                    
                    if( pMe->m_CopyType == MOVEALLRUIMTOPHONE || pMe->m_CopyType == MOVEALLMTTORUIM)
                    {
                        if(pMe->m_CopyType == MOVEALLRUIMTOPHONE)
                        {
                            mem_store = WMS_MEMORY_STORE_RUIM;
                        }
                        else
                        {
                            mem_store = WMS_MEMORY_STORE_NV_CDMA;
                        }
                        (void)IWMS_MsgDelete(pMe->m_pwms, 
                                                pMe->m_clientId, 
                                                &pMe->m_callback, 
                                                (void*)pMe, 
                                                mem_store, 
                                                pSrcList->indices[pSrcList->voice_mail_index]);
                    }
                                         
                    pSrcList->voice_mail_index++;
                    // 发送下一轮拷贝命令
                    (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                            EVTFLG_ASYNC,
                                            AEECLSID_WMSAPP,
                                            EVT_MSGCOPY,
                                            0, 
                                            0);
                }
            }
            
            return TRUE;

        default:
            break;
    }
    
    return FALSE;
}  // IDD_MSGCOPY_HandlerExt
#endif /* FEATURE_CDSMS_RUIM */

/*==============================================================================
函数:
    IDD_MEMSTATUS_Handler

说明:
    IDD_MEMSTATUS (检查信息容量)事件处理函数。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_MEMSTATUS_Handler(void *pUser,
    AEEEvent     eCode,
    uint16       wParam,
    uint32       dwParam
)
{
    WmsApp *pMe = (WmsApp *)pUser;
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);

        case EVT_USER_REDRAW:
        // 有新消息
        case EVT_WMS_MSG_RECEIVED_MESSAGE:
            {
                AECHAR  wstrDevice[32]={0};
                AECHAR  wstrVal[32]={0};
                char    strVal[32]={0};
                AEERect rc;
                int     nLineHeight,y;
                uint16  nMsgs=0;
                uint16  nOnUIMs=0;
#ifdef FEATURE_FUNCS_THEME    
                RGBVAL      oldColor;
                Theme_Param_type  Theme_Param={0};
                
                Appscom_GetThemeParameters(&Theme_Param);
#endif /* FEATURE_FUNCS_THEME */
				{
				// 画标题条
	            
                    TitleBar_Param_type  TitleBar_Param = {0};
                    
                    TitleBar_Param.pwszTitle = wstrDevice;
                    TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER;
                    (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                                IDS_MENSTATUS, wstrDevice,sizeof(wstrDevice));
                	#if 0
	                // 画标题条
	                {
	                    TitleBar_Param_type  TitleBar_Param = {0};
	                    
	                    TitleBar_Param.pwszTitle = wstrDevice;
	                    TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER;
	                    (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
	                                IDS_MENSTATUS, wstrDevice,sizeof(wstrDevice));
	                    
	                    DrawTitleBar(pMe->m_pDisplay, &TitleBar_Param);
	                    MEMSET(wstrDevice, 0, sizeof(wstrDevice));
	                }
					#else
					IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,wstrDevice);
					#endif
				}
                //获得系统字体高度
                nLineHeight = IDISPLAY_GetFontMetrics(pMe->m_pDisplay,
                                AEE_FONT_NORMAL,
                                NULL,
                                NULL);

                // 先清屏
                y = 0;//GetTitleBarHeight(pMe->m_pDisplay);
                SETAEERECT(&rc,0, y,pMe->m_rc.dx, pMe->m_rc.dy - GetBottomBarHeight(pMe->m_pDisplay) - y);
#ifdef FEATURE_FUNCS_THEME    
                IDISPLAY_FillRect(pMe->m_pDisplay, &rc, Theme_Param.bkcolor);
                
                // 更改文本字体颜色
                oldColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, Theme_Param.textColor);
#else

                IDISPLAY_FillRect(pMe->m_pDisplay, &rc, RGB_BLACK);
                IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);

#endif /* FEATURE_FUNCS_THEME */
                
                // 绘制话机收件箱使用情况
                SETAEERECT(&rc,3, y,pMe->m_rc.dx - 6, pMe->m_rc.dy - GetBottomBarHeight(pMe->m_pDisplay) - y);
                
#ifdef FEATURE_CDSMS_RUIM
                if (IsRunAsUIMVersion())
                {
                    (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                                IDS_PHONE, wstrDevice,sizeof(wstrDevice));
                    IDISPLAY_DrawText( pMe->m_pDisplay, 
                                AEE_FONT_BOLD, wstrDevice,
                                -1, 1, y, &rc, 
                                IDF_TEXT_TRANSPARENT|IDF_ALIGN_LEFT);
                    y += nLineHeight + 1;
                }
                else
                {
                    y += nLineHeight + 4;
                }
#endif /* FEATURE_CDSMS_RUIM */

                // 收件箱
                // 获取消息数
                wms_cacheinfolist_getcounts(WMS_MB_INBOX, NULL, &nOnUIMs, &nMsgs);
                (void)SPRINTF(strVal, "%d/%d", nMsgs-nOnUIMs, IN_MAX); 
                (void)STRTOWSTR(strVal, wstrVal, sizeof(wstrVal));
                (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                            IDS_INBOX, wstrDevice,sizeof(wstrDevice));
                IDISPLAY_DrawText( pMe->m_pDisplay, 
                            AEE_FONT_NORMAL, wstrDevice,
                            -1, 1, y, &rc, 
                            IDF_TEXT_TRANSPARENT|IDF_ALIGN_LEFT);
                IDISPLAY_DrawText( pMe->m_pDisplay, 
                            AEE_FONT_NORMAL, wstrVal,
                            -1, 1, y, &rc, 
                            IDF_TEXT_TRANSPARENT|IDF_ALIGN_RIGHT);
                            
                // 发件箱
                // 获取消息数
                wms_cacheinfolist_getcounts(WMS_MB_OUTBOX, NULL, &nOnUIMs, &nMsgs);
                y += nLineHeight + 1;
                (void)SPRINTF(strVal, "%d/%d", (nMsgs-nOnUIMs), OUT_MAX); 
                (void)STRTOWSTR(strVal, wstrVal, sizeof(wstrVal));
                (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                            IDS_OUTBOX, wstrDevice,sizeof(wstrDevice));
                IDISPLAY_DrawText( pMe->m_pDisplay, 
                            AEE_FONT_NORMAL, wstrDevice,
                            -1, 1, y, &rc, 
                            IDF_TEXT_TRANSPARENT|IDF_ALIGN_LEFT);
                IDISPLAY_DrawText( pMe->m_pDisplay, 
                            AEE_FONT_NORMAL, wstrVal,
                            -1, 1, y, &rc, 
                            IDF_TEXT_TRANSPARENT|IDF_ALIGN_RIGHT);
                            
                // 草稿箱
                // 获取消息数
                wms_cacheinfolist_getcounts(WMS_MB_DRAFT, NULL, NULL, &nMsgs);
                y += nLineHeight + 1;
                (void)SPRINTF(strVal, "%d/%d", nMsgs, DRAFT_MAX); 
                (void)STRTOWSTR(strVal, wstrVal, sizeof(wstrVal));
                (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                            IDS_DRAFT, wstrDevice,sizeof(wstrDevice));
                IDISPLAY_DrawText( pMe->m_pDisplay, 
                            AEE_FONT_NORMAL, wstrDevice,
                            -1, 1, y, &rc, 
                            IDF_TEXT_TRANSPARENT|IDF_ALIGN_LEFT);
                IDISPLAY_DrawText( pMe->m_pDisplay, 
                            AEE_FONT_NORMAL, wstrVal,
                            -1, 1, y, &rc, 
                            IDF_TEXT_TRANSPARENT|IDF_ALIGN_RIGHT);
                
#ifdef FEATURE_CDSMS_RUIM
                if (IsRunAsUIMVersion())
                {
                    y += nLineHeight;
                    (void)STRTOWSTR("----------------------------", wstrDevice, sizeof(wstrDevice));
                    IDISPLAY_DrawText( pMe->m_pDisplay, 
                                AEE_FONT_NORMAL, wstrDevice,
                                -1, 1, y, &rc, 
                                IDF_TEXT_TRANSPARENT|IDF_ALIGN_LEFT);
                    
                    // 绘制UIM使用情况
                    y += nLineHeight/2 + 1;
                    (void)SPRINTF(strVal, "%d/%d", 
                                  pMe->m_memruimStatuse.used_tag_slots, 
                                  pMe->m_memruimStatuse.max_slots); 
                    (void)STRTOWSTR(strVal, wstrVal, sizeof(wstrVal));
                    (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                                IDS_RUIM, wstrDevice,sizeof(wstrDevice));
                                
                    IDISPLAY_DrawText( pMe->m_pDisplay, 
                                AEE_FONT_BOLD, wstrDevice,
                                -1, 1, y, &rc, 
                                IDF_TEXT_TRANSPARENT|IDF_ALIGN_LEFT);
                    IDISPLAY_DrawText( pMe->m_pDisplay, 
                                AEE_FONT_BOLD, wstrVal,
                                -1, 1, y, &rc, 
                                IDF_TEXT_TRANSPARENT|IDF_ALIGN_RIGHT);
                }
#endif /* FEATURE_CDSMS_RUIM */

#ifdef FEATURE_FUNCS_THEME    
                // 恢复文本字体颜色
                (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, oldColor);
#else
                (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
#endif /* FEATURE_FUNCS_THEME */
            }
            
            // 绘制底条提示
            //          Back
            DRAW_BOTTOMBAR(BTBAR_BACK)
            
            // 立即更新显示
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
    
                default:
                    break;
            }
            return FALSE;
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

				if(WMSAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_CLR,0);
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
}//IDD_MEMSTATUS_Handler


/*==============================================================================
函数:
    IDD_DELETING_Handler

说明:
    IDD_DELETING 事件处理函数。 删除信息。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注::

==============================================================================*/
static boolean IDD_DELETING_Handler(void        *pUser,
                                    AEEEvent    eCode,
                                    uint16      wParam,
                                    uint32      dwParam)
{
    WmsApp *pMe = (WmsApp *)pUser;

    if (NULL == pMe)
    {
        return FALSE;
    }
    
    if (NULL == pMe->m_pStatic)
    {
        return FALSE;
    }
    MSG_FATAL("IDD_DELETING_Handler Start eCode=0x%x", eCode,0,0);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;
            
        case EVT_USER_REDRAW:
            {
                uint16 nResID = IDS_DELETING;
                AECHAR wstrText[MSGBOX_MAXTEXTLEN];
                int nRet=EFAILED;
                PromptMsg_Param_type  Msg_Param={0};
                MSG_FATAL("IDD_DELETING_Handler m_eEraseWMSType=%d", pMe->m_eEraseWMSType,0,0);
                switch (pMe->m_eEraseWMSType)
                {
                    case ERASE_DRAFT_ONE:
                    case ERASE_INBOXES_ONE:
                    case ERASE_OUTBOX_ONE:
                    case ERASE_RESERVED_ONE:
                        //nResID = IDS_DELETED;
                        // 对长短信需逐个节点删除
                        {
                            int i;
                            wms_cache_info_node  *pnode = NULL;
                            MSG_FATAL("IDD_DELETING_Handler pMe->m_wCurindex=%d", pMe->m_wCurindex,0,0);
                            for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
                            {
                                if (pMe->m_CurMsgNodes[i] != NULL)
                                {
                                    pnode = pMe->m_CurMsgNodes[i];
                                    
                                    // 发布删除消息命令
                                    nRet = ENOMEMORY;
                                    do
                                    {
                                        nRet = IWMS_MsgDelete(pMe->m_pwms,
                                                           pMe->m_clientId,
                                                           &pMe->m_callback,
                                                           (void *)pMe,
                                                           pnode->mem_store,
                                                           pnode->index);
                                    } while(nRet != SUCCESS);
                                    pMe->m_CurMsgNodes[i] = NULL;
                                }
                            }
                        }
                        break;
                        
                    case ERASE_VOICEMAIL_ONE:
                        //nResID = IDS_DELETED;
                        // 清除语音消息
                        // 发布删除消息命令
                        nRet = IWMS_MsgDelete(pMe->m_pwms,
                                    pMe->m_clientId, 
                                    &pMe->m_callback,
                                    (void*)pMe,
                                    WMS_MEMORY_STORE_NV_CDMA,
                                    0);
                        break;
                        
                    case ERASE_TEMPLATE_ONE:
                        //nResID = IDS_DELETED;
                        // 清除当前消息模板
                        if (pMe->m_wCurTemplate>=MSG_CMD_BASE)
                        {
                            wms_message_index_type    index = pMe->m_wCurTemplate-MSG_CMD_BASE;
                            
                            // 发布删除消息命令
                            if (index>=PHRASE_START)
                            {
                                nRet = IWMS_MsgDelete(pMe->m_pwms,
                                            pMe->m_clientId, 
                                            &pMe->m_callback,
                                            (void*)pMe,
                                            WMS_MEMORY_STORE_NV_CDMA,
                                            index);
                            }
                            else
                            {// 卡上消息模板
                                nRet = IWMS_MsgDeleteTemplate(pMe->m_pwms,
                                            pMe->m_clientId, 
                                            &pMe->m_callback,
                                            (void*)pMe,
                                            WMS_MEMORY_STORE_RUIM,
                                            index);
                            }
                        }
                        break;
#ifdef FEATURE_USES_MMS
                    case ERASE_INBOX_MMS_ONE:
                    {
                        WMS_MMS_DeleteMMS(pMe->m_wSelItemxuhao - 1,MMS_INBOX);
                    }
                    break;
                    case ERASE_OUTBOX_MMS_ONE:
                    {
                        WMS_MMS_DeleteMMS(pMe->m_wSelItemxuhao - 1,MMS_OUTBOX);
                    }
                    break;
                    case CLEAR_INBOX_MMS:
                    {
                        WMS_MMS_DeleteMMSALL(MMS_INBOX);
                    }
                    break;
                    case CLEAR_OUTBOX_MMS:
                    {
                        WMS_MMS_DeleteMMSALL(MMS_OUTBOX);
                    }
                    break;
                    case ERASE_DRAFTBOX_MMS_ONE:
                    {
                        WMS_MMS_DeleteMMS(pMe->m_wSelItemxuhao - 1,MMS_DRAFTBOX);
                    }
                    break; 
                    case CLEAR_DRAFTBOX_MMS:
                    {
                        WMS_MMS_DeleteMMSALL(MMS_DRAFTBOX);
                    }
                    break;                    
#endif
                    case CLEAR_INBOXES:
#ifdef FEATURE_CDSMS_RUIM
                        if (IsRunAsUIMVersion())
                        {
                            nRet = IWMS_MsgDeleteBox(pMe->m_pwms,
                                        pMe->m_clientId, 
                                        &pMe->m_callback,
                                        (void*)pMe,
                                        WMS_INBOX_DEL_ALL);
                        }
                        else
#endif                            
                        {
                            nRet = IWMS_MsgDeleteBox(pMe->m_pwms,
                                        pMe->m_clientId, 
                                        &pMe->m_callback,
                                        (void*)pMe,
                                        WMS_INBOX_DEL_PHONE);
                        }
                        break;
                        
#ifdef FEATURE_CDSMS_RUIM
                    case CLEAR_PHONE:
                        nRet = IWMS_MsgDeleteBox(pMe->m_pwms,
                                    pMe->m_clientId, 
                                    &pMe->m_callback,
                                    (void*)pMe,
                                    WMS_INBOX_DEL_PHONE);
                        break;
                        
                    case CLEAR_RUIM:
                        nRet = IWMS_MsgDeleteBox(pMe->m_pwms,
                                    pMe->m_clientId, 
                                    &pMe->m_callback,
                                    (void*)pMe,
                                    WMS_INBOX_DEL_RUIM);
                        break;
#endif
                                                                        
                    case CLEAR_OUTBOX:
                        nRet = IWMS_MsgDeleteBox(pMe->m_pwms,
                                    pMe->m_clientId, 
                                    &pMe->m_callback,
                                    (void*)pMe,
                                    WMS_OUTBOX_DEL_ALL);
                        break;
                                    
                    case CLEAR_DRAFT:
                        nRet = IWMS_MsgDeleteBox(pMe->m_pwms,
                                    pMe->m_clientId, 
                                    &pMe->m_callback,
                                    (void*)pMe,
                                    WMS_DRAFT_DEL_ALL);
                        break;
                        
                    case CLEAR_RESERVED:
                        nRet = IWMS_MsgDeleteBox(pMe->m_pwms,
                                    pMe->m_clientId, 
                                    &pMe->m_callback,
                                    (void*)pMe,
                                    WMS_RESERVE_DEL_ALL);
                        break;
                        
                    case CLEAR_RESERVEFAILED:
                        nRet = IWMS_MsgDeleteBox(pMe->m_pwms,
                                    pMe->m_clientId, 
                                    &pMe->m_callback,
                                    (void*)pMe,
                                    WMS_RSVFAILED_DEL_ALL);
                        break;
                        
                    case CLEAR_ALL:
                        nRet = IWMS_MsgDeleteBox(pMe->m_pwms,
                                    pMe->m_clientId, 
                                    &pMe->m_callback,
                                    (void*)pMe,
                                    WMS_BOXES_DEL_ALL);
                        
                        if(nRet == SUCCESS)
                        {
                            nRet = IWMS_MsgDeleteBox(pMe->m_pwms,
                                        pMe->m_clientId,
                                        &pMe->m_callback,
                                        (void*)pMe,
                                        WMS_RSVANDRSVFAILED_DEL_ALL);
                        }
                        break;

                    case CLEAR_ALLRESERVED:
                        nRet = IWMS_MsgDeleteBox(pMe->m_pwms,
                                    pMe->m_clientId,
                                    &pMe->m_callback,
                                    (void*)pMe,
                                    WMS_RSVANDRSVFAILED_DEL_ALL);
                        break;
                        
                    default:
                        break;
                }
                
                (void) ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                                nResID, wstrText, sizeof(wstrText));
            
                // 调用公共函数绘制界面
                Msg_Param.ePMsgType = MESSAGE_WAITING;
                Msg_Param.pwszMsg = wstrText;
                Msg_Param.eBBarType = BTBAR_NONE;
                DrawPromptMessage(pMe->m_pDisplay, pMe->m_pStatic, &Msg_Param);
                                    
                // 更新界面
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                
                // 设置定时器函数以关闭该对话框
                if (nRet != SUCCESS)
                {
                    (void) ISHELL_SetTimer(pMe->m_pShell,
                            MSGBOX_TIMEOUT,
                            DialogTimeoutCallback, pMe);
                }
                pMe->m_eDlgReturn = DLGRET_DONE;
            }
            return TRUE;

        case EVT_WMS_MSG_DELETE:
        case EVT_WMS_MSG_DELETE_BOX:
        case EVT_WMS_MSG_DELETE_TEMPLATE:
            {
                AECHAR  wstrText[MSGBOX_MAXTEXTLEN];
                PromptMsg_Param_type  Msg_Param={0};
                MSG_FATAL("IDD_DELETING_Handler DELETE MMS", 0,0,0);
                pMe->m_eDlgReturn = DLGRET_DONE;
                
                //删除完成
                (void) ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                                IDS_DELETED, wstrText, sizeof(wstrText));
                                
                // 调用公共函数绘制界面
                Msg_Param.ePMsgType = MESSAGE_INFORMATIVE;
                Msg_Param.pwszMsg = wstrText;
                Msg_Param.eBBarType = BTBAR_NONE;
                DrawPromptMessage(pMe->m_pDisplay, pMe->m_pStatic, &Msg_Param);
                                    
                // 更新界面
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
    
                // 设置定时器函数以关闭该对话框
                (void) ISHELL_SetTimer(pMe->m_pShell,
                        MSGBOX_TIMEOUT,
                        DialogTimeoutCallback, pMe);
            }
            return TRUE;
            
        case EVT_DIALOG_END:
            (void) ISHELL_CancelTimer(pMe->m_pShell,DialogTimeoutCallback,pMe);
            ISTATIC_Release(pMe->m_pStatic);
            pMe->m_pStatic = NULL;
            return TRUE;

        case EVT_KEY:
            break;

        case EVT_DISPLAYDIALOGTIMEOUT:
            CLOSE_DIALOG(DLGRET_DONE)
            return TRUE;

        default:
            break;
    }

    return FALSE;
} // IDD_DELETING_Handler()

/*==============================================================================
函数:
    IDD_MSGBOX_Handler

说明:
    WMS Applet对话框 IDD_MSGBOX 事件处理函数。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_MSGBOX_Handler(void *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    WmsApp *pMe = (WmsApp *)pUser;
    
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    if (NULL == pMe->m_pStatic)
    {
        return FALSE;
    }

	MSG_FATAL("***zzg IDD_MSGBOX_Handler***", 0, 0, 0);
    
    switch(eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            // 启动发送关闭对话框事件的定时器
            (void) ISHELL_SetTimer(pMe->m_pShell,
                                PROMPTMSG_TIMER,
                                DialogTimeoutCallback,
                                pMe);
            
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);

            return TRUE;

        case EVT_USER_REDRAW:
            {
                PromptMsg_Param_type  Msg_Param={0};
                AECHAR  wstrText[MSGBOX_MAXTEXTLEN] = {0};
                
                switch (pMe->m_wMsgResID)
                {
                    case IDS_YEARERR:
                        {
                            AECHAR  wstrTep[MSGBOX_MAXTEXTLEN]={0};
                            JulianType jtdate;
                            
                            (void)ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_WMSAPPRES_LANGFILE,                                
                                            pMe->m_wMsgResID,
                                            wstrTep,
                                            sizeof(wstrTep));
                            {
								int32  dwSecs;

								/*
								byte	btTimeStamp = 0;
							    
								(void) ICONFIG_GetItem(pMe->m_pConfig,
							                           CFGI_SMS_TIMESTAMP,
							                           &btTimeStamp,
							                           sizeof(btTimeStamp));

								if (btTimeStamp == OEMNV_SMS_TIMESTAMP_ADJUST)
								{
									dwSecs = GETUTCSECONDS();
									MSG_FATAL("***zzg GETUTCSECONDS 7 dwSecs=%d***", dwSecs, 0, 0);
								}
								else
								{
									dwSecs = GETTIMESECONDS();
									MSG_FATAL("***zzg GETTIMESECONDS 7 dwSecs=%d***", dwSecs, 0, 0);
								}   	
								*/
							
	                            GETJULIANDATE(GETTIMESECONDS(), &jtdate);
	                            //GETJULIANDATE(dwSecs, &jtdate);
	                            WSPRINTF(wstrText, sizeof(wstrText), wstrTep, jtdate.wYear);
							}
                        }
                        break;

                    case IDS_SENDDONE:
                        {
                            AECHAR wstrTep[MSGBOX_MAXTEXTLEN]={0};

                            (void)ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_WMSAPPRES_LANGFILE,
                                            pMe->m_wMsgResID,
                                            wstrTep,
                                            sizeof(wstrTep));
                            WSPRINTF(wstrText, sizeof(wstrText), wstrTep, pMe->m_SucNum, pMe->m_FailNum);

                        }
                        break;

                    case IDS_MOVED:
                    case IDS_COPIED:
                        {
                            AECHAR wstrTep[MSGBOX_MAXTEXTLEN]={0};						
							
							(void)ISHELL_LoadResString(pMe->m_pShell,
				                                            AEE_WMSAPPRES_LANGFILE,
				                                            pMe->m_wMsgResID,
				                                            wstrTep,
				                                            sizeof(wstrTep));
							#ifdef FEATURE_VERSION_W317A
							if (COPYTORUIM == pMe->m_CopyType || COPYTOPHONEINBOXE == pMe->m_CopyType ||
								MOVETORUIM == pMe->m_CopyType ||MOVETOPHONEINBOX == pMe->m_CopyType)
							{								
								WSPRINTF(wstrText, sizeof(wstrText), wstrTep, 1);
							}
							else
							#endif
	                        {
	                        	WSPRINTF(wstrText, sizeof(wstrText), wstrTep, pMe->m_nCopyNum);
							}
							
                        }
                        break;

                    case IDS_SAVE:
                        {
                            (void) ISHELL_CancelTimer(pMe->m_pShell,DialogTimeoutCallback,pMe);
                            (void)ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_WMSAPPRES_LANGFILE,
                                            pMe->m_wMsgResID,
                                            wstrText,
                                            sizeof(wstrText)); 
                            
                            Msg_Param.eBBarType = BTBAR_YES_NO_CANCEL;
                        }
                    default:
                        // 从资源文件取消息内容
                        (void)ISHELL_LoadResString(pMe->m_pShell,
                                        AEE_WMSAPPRES_LANGFILE,                                
                                        pMe->m_wMsgResID,
                                        wstrText,
                                        sizeof(wstrText));
                        break;
                }
                                
                Msg_Param.ePMsgType = pMe->m_ePMsgType;
                Msg_Param.pwszMsg = wstrText;
                DrawPromptMessage(pMe->m_pDisplay, pMe->m_pStatic, &Msg_Param);
            }
            // 更新界面
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);   
            return TRUE;

        case EVT_DIALOG_END:
            (void) ISHELL_CancelTimer(pMe->m_pShell,DialogTimeoutCallback,pMe);
  
            // 此对话框返回值仅为 DLGRET_MSGBOX_OK ，为防止挂起 Applet 
            // 关闭对话框回到错误状态，显示给对话框返回值赋值
            //pMe->m_eDlgReturn = DLGRET_MSGBOX_OK;
            
            ISTATIC_Release(pMe->m_pStatic);
            pMe->m_pStatic = NULL;
            
            return TRUE;

        // 禁止通过按键关闭对话框
        case EVT_KEY:
            if(pMe->m_wMsgResID == IDS_SAVE)
            {
                switch(wParam)
                {
                    case AVK_CLR:
                        CLOSE_DIALOG(DLGRET_WRITEMSG)
                        break;

                    case AVK_SELECT:
                        CLOSE_DIALOG(DLGRET_SAVEDRAFT)
                        break;

                    case AVK_INFO:
                        CLOSE_DIALOG(DLGRET_CANCELED)
                        break;

                    default:
                        break;
                }
            }
            return TRUE;
        
        case EVT_DISPLAYDIALOGTIMEOUT:
            MSG_FATAL("DLGRET_MSGBOX_OK",0,0,0);
            CLOSE_DIALOG(DLGRET_MSGBOX_OK)
            return TRUE;
        
#ifdef FEATURE_LCD_TOUCH_ENABLE
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

                if(WMSAPP_PT_IN_RECT(wXPos,wYPos,rc))
                {
                    if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
                    {
                        boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_SELECT,0);
                        return rt;
                    }
                    else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
                    {
                         boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_INFO,0);
                         return rt;
                    }
                    else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
                    {                       
                         boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_CLR,0);
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
} // IDD_MSGBOX_Handler

#ifdef FEATURE_CONTACT_APP
/*==============================================================================
函数:
    IDD_EXTARCTDETAILS_Handler

说明:
    WMS Applet对话框 IDD_EXTARCTDETAILS 事件处理函数。显示针对收件箱内一条消息
    的相关操作选项。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_EXTARCTDETAILS_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;
    
    if (pMe == NULL)
    {
        return FALSE;
    }
    
    pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_MENU_EXTRACT);
    if (NULL == pMenu)
    {
        return FALSE;
    }
    
    switch (eCode) 
    {
        case EVT_DIALOG_INIT:
            return TRUE;
        
        case EVT_DIALOG_START:
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
#endif
			#if 0
            (void) IMENUCTL_SetTitle(pMenu, AEE_WMSAPPRES_LANGFILE, IDS_EXTRACTDETAIL, NULL);
			#else
			{
					AECHAR WTitle[40] = {0};
					(void)ISHELL_LoadResString(pMe->m_pShell,
                                    AEE_WMSAPPRES_LANGFILE,                                
                                    IDS_EXTRACTDETAIL,
                                    WTitle,
                                    sizeof(WTitle));
					IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
			}
			#endif
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);
            MENU_ADDITEM(pMenu, IDS_SAVENUM);
            MENU_ADDITEM(pMenu, IDS_SAVEEMAIL);
            MENU_ADDITEM(pMenu, IDS_SAVEURL);
            IMENUCTL_SetSel(pMenu, pMe->m_wPrevMenuSel);
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;
            
        case EVT_USER_REDRAW:
            //(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;
            
        case EVT_KEY:
            switch (wParam) 
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                    
                default:
                    break;
            }
            return FALSE;

        case EVT_COMMAND:
            pMe->m_wPrevMenuSel = wParam;
            
            switch (wParam)
            {
                case IDS_SAVENUM:
                    CLOSE_DIALOG(DLGRET_SAVENUM)
                    break;
                    
                case IDS_SAVEEMAIL:
                    CLOSE_DIALOG(DLGRET_SAVEEMAIL)
                    break;
                    
                case IDS_SAVEURL:
                    CLOSE_DIALOG(DLGRET_SAVEURL)
                    break;
                    
                default:
                    break;
            }
            return TRUE;
            
        default:
            break;
    }
    
    return FALSE;
}  // IDD_EXTARCTDETAILS_Handler

/*==============================================================================
函数:
    IDD_EXTARCTEDITEMLIST_Handler

说明:
    WMS Applet对话框 IDD_EXTARCTEDITEMLIST 事件处理函数。显示针对收件箱内一条消息
    的相关操作选项。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_EXTARCTEDITEMLIST_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;
    
    if (pMe == NULL)
    {
        return FALSE;
    }
    
    pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_SAVENUMLIST_MENU);
    if (NULL == pMenu)
    {
        return FALSE;
    }
    
    switch (eCode) 
    {
        case EVT_DIALOG_INIT:
            return TRUE;
        
        case EVT_DIALOG_START:
            // 设置菜单矩形区域
            //SetControlRect(pMe,pMenu);
            
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
#endif
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,                                
                        IDS_SAVENUM,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            MENU_SETBOTTOMBAR(pMenu, BTBAR_OPTION_BACK);
            // 此对话框菜单项需动态建立
            {
                int32     i, dwCount;
                AECHAR    *pItem = NULL;
                
                dwCount = IVector_Size(pMe->m_pSaveNumList);
                
                for (i=0; i<dwCount; i++)
                {
                    pItem = (AECHAR *)IVector_ElementAt(pMe->m_pSaveNumList, i);
                    if (NULL != pItem)
                    {
                        IMENUCTL_AddItem(pMenu, NULL, 0, MSG_CMD_BASE+i, pItem, 0); 
                    }
                }
            }
            if (pMe->m_ExtractType == EXTRACT_EMAILADD)
            {
                #if 0
                (void) IMENUCTL_SetTitle(pMenu, AEE_WMSAPPRES_LANGFILE, IDS_SAVEEMAIL, NULL);
				#else
				{
					AECHAR WTitle[40] = {0};
					(void)ISHELL_LoadResString(pMe->m_pShell,
                                    AEE_WMSAPPRES_LANGFILE,                                
                                    IDS_SAVEEMAIL,
                                    WTitle,
                                    sizeof(WTitle));
					IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
				}
				
				#endif
            }
            else if (pMe->m_ExtractType == EXTRACT_URL)
            {
                #if 0
                (void) IMENUCTL_SetTitle(pMenu, AEE_WMSAPPRES_LANGFILE, IDS_SAVEURL, NULL);
				#else
				{
					AECHAR WTitle[40] = {0};
					(void)ISHELL_LoadResString(pMe->m_pShell,
                                    AEE_WMSAPPRES_LANGFILE,                                
                                    IDS_SAVEURL,
                                    WTitle,
                                    sizeof(WTitle));
					IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
				}
				#endif
            }
            IMENUCTL_SetSel(pMenu, pMe->m_wPrevMenuSel);
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;
            
        case EVT_USER_REDRAW:
            // 绘制底条提示
            // Select       Back
            //DRAW_BOTTOMBAR(BTBAR_SELECT_BACK)
            
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            //(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;
            
        case EVT_KEY:
            switch (wParam) 
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                    
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

					MSG_FATAL("***zzg AVK_SEND CallPhoneNumber 333 m_ExtractType=%x", pMe->m_ExtractType, 0, 0);
					
                    if (pMe->m_ExtractType == EXTRACT_NUM)
                    {
                        int32     i;
                        AECHAR    *pItem = NULL;
                        
                        i = IMENUCTL_GetSel(pMenu) - MSG_CMD_BASE;
                        pItem = (AECHAR *)IVector_ElementAt(pMe->m_pSaveNumList, i);
                        if (NULL != pItem)
                        {
                            // 调用呼叫接口，本 Applet 会被挂起，返回时回到当前状态
                            WMSExtApp_CallPhoneNumber(pMe, pItem, FALSE);
                        }
                    }
                    return TRUE;
#if defined(FEATURE_VERSION_C306)||defined(FEATURE_VERSION_W0216A)|| defined(FEAUTRE_VERSION_N450)|| defined(FEATURE_VERSION_N021)|| defined(FEATURE_VERSION_C01)
					case AVK_CAMERA:
					{
						nv_item_type		SimChoice;
						OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
						if(SimChoice.sim_select ==AVK_SEND_TWO)
						{
							if (pMe->m_ExtractType == EXTRACT_NUM)
		                    {
		                        int32     i;
		                        AECHAR    *pItem = NULL;
		                        
		                        i = IMENUCTL_GetSel(pMenu) - MSG_CMD_BASE;
		                        pItem = (AECHAR *)IVector_ElementAt(pMe->m_pSaveNumList, i);
		                        if (NULL != pItem)
		                        {
		                            // 调用呼叫接口，本 Applet 会被挂起，返回时回到当前状态
		                            WMSExtApp_CallPhoneNumber(pMe, pItem, FALSE);
		                        }
		                    }
		                    return TRUE;
						}
					}
					break;
#endif

                default:
                    break;
            }
            return FALSE;

        case EVT_COMMAND:
            pMe->m_wPrevMenuSel = wParam;
            pMe->m_CurExtractItem = wParam - MSG_CMD_BASE;
            {
                AECHAR    *pItem = NULL;

                pItem = (AECHAR *)IVector_ElementAt(pMe->m_pSaveNumList, pMe->m_CurExtractItem);
                if (NULL != pItem)
                {
                    CLOSE_DIALOG(DLGRET_OK)
                }
            }
            return TRUE;
            
        default:
            break;
    }
    
    return FALSE;
}  // IDD_EXTARCTEDITEMLIST_Handler

/*==============================================================================
函数:
    IDD_EXTARCTEDITEM_OPT_Handler

说明:
    WMS Applet对话框 IDD_EXTARCTEDITEM_OPT 事件处理函数。显示针对收件箱内一条消息
    的相关操作选项。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_EXTARCTEDITEM_OPT_Handler(void *pUser,
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;
    
    if (pMe == NULL)
    {
        return FALSE;
    }
    
    pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_SAVENUMLISTOPT_MENU);
    if (NULL == pMenu)
    {
        return FALSE;
    }
    
    switch (eCode) 
    {
        case EVT_DIALOG_INIT:
            return TRUE;
                    
        case EVT_DIALOG_START:
            // 设置菜单矩形区域
            //SetControlRect(pMe,pMenu);
            
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
#endif
            
            // 初始化菜单项
            if (pMe->m_ExtractType == EXTRACT_NUM)
            {// 只有提取号码才有此项
                MENU_ADDITEM(pMenu, IDS_SAVE);
            }
            
#if defined(FEATURE_SUPPORT_WAP_APP)
            if (pMe->m_ExtractType == EXTRACT_URL)
            {// 只有URL才有此项
                MENU_ADDITEM(pMenu, IDS_BROWSE);
            }
#endif          

            MENU_ADDITEM(pMenu, IDS_SAVETOCONTACT);
            if (pMe->m_ExtractType == EXTRACT_NUM)
            {
                MENU_ADDITEM(pMenu, IDS_SENDMSG);
            }
            
            IMENUCTL_SetSel(pMenu, pMe->m_wPrevMenuSel);
            IMENUCTL_SetPopMenuRect(pMenu);
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;
            
        case EVT_USER_REDRAW:
            // 绘制底条提示
            // Select       Back
            //DRAW_BOTTOMBAR(BTBAR_SELECT_BACK)
            
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            //(void)IMENUCTL_Redraw(pMenu);   //dele by yangdecai
            return TRUE;
            
        case EVT_DIALOG_END:
            if (pMe->m_eDlgReturn == DLGRET_CREATE)
            {
                pMe->m_currState = pMe->m_prevState;
            }
            return TRUE;
            
        case EVT_KEY:
            switch (wParam) 
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                    
                default:
                    break;
            }
            return FALSE;

        case EVT_COMMAND:
            pMe->m_wPrevMenuSel = wParam;
            
            switch (wParam) 
            {
                case IDS_SAVE:
                case IDS_SAVETOCONTACT:
                    {
                        AECHAR    *pItem = NULL;
        
                        pItem = (AECHAR *)IVector_ElementAt(pMe->m_pSaveNumList, pMe->m_CurExtractItem);
                        if (NULL == pItem)
                        {
                            return TRUE;
                        }
                        
                        // 调用通信簿接口，本 Applet 会被挂起，返回时回到当前状态
                        switch (pMe->m_ExtractType)
                        {
                            case EXTRACT_NUM:
                                WMSExtApp_SaveExtractDetails(pMe, pItem, NULL, NULL);
                                break;
                                
                            case EXTRACT_EMAILADD:
                                WMSExtApp_SaveExtractDetails(pMe, NULL, NULL, pItem);
                                break;
                                
                            case EXTRACT_URL:
                                WMSExtApp_SaveExtractDetails(pMe, NULL, pItem, NULL);
                                break;
                            
                            default:
                                break;
                        }
                    }
                    pMe->m_currState = pMe->m_prevState;
                    return TRUE;
                    
                case IDS_SENDMSG:
                    CLOSE_DIALOG(DLGRET_WRITEMSG)
                    return TRUE;
                    
#if defined(FEATURE_SUPPORT_WAP_APP)
                case IDS_BROWSE:
                    {
						/*
                        AECHAR    *pItem = NULL;
                        char      *pBuf = NULL;
                        int32     nLen, nSize;

						MSG_FATAL("***zzg WMSDialogHandler IDS_BROWSE***", 0, 0, 0);
                        
                        pItem = (AECHAR *)IVector_ElementAt(pMe->m_pSaveNumList, pMe->m_CurExtractItem);
                        if (NULL == pItem)
                        {
                        	MSG_FATAL("***zzg WMSDialogHandler NULL == pItem return***", 0, 0, 0);
                            return TRUE;
                        }
                        nSize = 256*sizeof(char);
                        pBuf = (char *)MALLOC(nSize);
                        if (NULL == pBuf)
                        {
                        	MSG_FATAL("***zzg WMSDialogHandler NULL == pBuf***", 0, 0, 0);
                            return TRUE;
                        }
                        pBuf[0]=0;
                        STRCPY(pBuf, "brw_app:LoadURL?view=normal&url=");
                        nLen = STRLEN(pBuf);
                        nSize -= nLen*sizeof(char);
                        WSTRTOSTR(pItem, &pBuf[nLen], nSize);

						MSG_FATAL("***zzg WMSDialogHandler pBuf=%s***", pBuf, 0, 0);
                        
                        ISHELL_PostURL(pMe->m_pShell, pBuf);
                        FREEIF(pBuf);    
						*/

						char	buf[12];
						int		Result;
						AECHAR    *pItem = NULL;                        
						char	  pbuf[256];
                        int32     nLen, nSize;

						MSG_FATAL("***zzg WMSDialogHandler IDS_BROWSE Start***", 0, 0, 0);
		
						OEM_SetBROWSER_ADSAccount();	//Add By zzg 2011_07_08
						
						pItem = (AECHAR *)IVector_ElementAt(pMe->m_pSaveNumList, pMe->m_CurExtractItem);
						
						if (NULL == pItem)
						{							
							return TRUE;
						}						
						
						DBGPRINTF("***zzg WMSDialogHandler pItem=%x***", pItem);
																		
						WSTRTOSTR(pItem, pbuf, sizeof(pbuf));

						DBGPRINTF("***zzg IDS_BROWSE pbuf=%s***", pbuf, 0, 0);
										    	
				    	start_info.appid_fx = APP_ORIGINATOR_BROWSER;
				        start_info.subtype_keycode = APP_BROWSER_START_URL;
				        start_info.par_keychar = 0;						
						
				        start_info.fInfo.url_info.title = "Browser";				        
				        start_info.fInfo.url_info.url = pbuf;	//"http://www.163.com";			        
																		
						DBGPRINTF("***zzg IDS_BROWSE url=%s***", start_info.fInfo.url_info.url);						
						
				        SPRINTF(buf, "%p", &start_info);
						Result = ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_NF3, buf);

						MSG_FATAL("***zzg IDS_BROWSE End***", 0, 0, 0);

						//FREEIF(pBuf); 
						
                    }
                    return TRUE;
#endif               
                
                default:
                    break;
            }
            return TRUE;
            
        default:
            break;
    }
    
    return FALSE;
}  // IDD_EXTARCTEDITEM_OPT_Handler
#endif

/*==============================================================================
函数:
    FormatMessageForDisplay

说明:
    将给定消息格式化已显示给用户。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    pMessage [in]: 给定消息。

返回值:
    指向格式化后的文本字符串。

备注:
    返回的字符串所用空间属于动态分配，需调用者释放。
        
==============================================================================*/
static AECHAR  * FormatMessageForDisplay(WmsApp *pMe, WMSMessageStruct *pMessage)
{
    uint16    nHeaderResID = 0;
    int32     dwSize,dwTotalLen = 0;
    int       nLen = 0;
    
    AECHAR    *wstrPriFld = NULL;      // 消息优先级部份
    AECHAR    *wstrContentFld = NULL;  // 消息正文部份
    AECHAR    *wstrOptCall = NULL;     // 语音邮件听取选项
    AECHAR    *wstrOptDel = NULL;      // 语音邮件删除选项
    AECHAR    *wstrAddFld = NULL;      // 消息接收或发送人信息部份
    AECHAR    *wstrDtFld = NULL;       // 消息时间戳部份
    AECHAR    *wstrRet = NULL;         // 格式化后消息数据
    
    AECHAR    wszNewLine[] = {(AECHAR)'\n', 0};
    AECHAR    wstrFmt[32] = {0};

    if ((NULL == pMe) || (NULL == pMessage))
    {
    	MSG_FATAL("pMe IS NULL.....pMessage IS NULL ",0,0,0);
        return NULL;
    }
    
    // 先处理消息接收或发送人信息部份

    switch (pMe->m_currState)
    {
        case WMSST_VIEWOUTBOXMSG:
        case WMSST_VIEWRESERVEDMSG:
        
            // 发送至:
            nHeaderResID = IDS_TO;
            break;
            
        case WMSST_VIEWINBOXMSG:
        case WMSST_VIEWVOICEMAIL:
        
            // 来自:
            nHeaderResID = IDS_FROM;
            break;
            
        default:
            break;
    }
   
    MSG_FATAL("nHeaderResID==========================%d ",nHeaderResID,0,0);
    if (nHeaderResID != 0)
    {   // 带标题
        AECHAR  wszTitle[MAX_TITLE_LEN]={0};
        AECHAR  wszName[MAX_TITLE_LEN]={0};
        int     nLenNum  = 0;
        int     nName  = 0;
    
#if 0        
        // 将优先级放在标题栏
        // 准备 wstrPriFld 
        if ((nHeaderResID == IDS_FROM) && 
            ((pMessage->m_epriority == WMS_PRIORITY_URGENT) || (pMessage->m_epriority == WMS_PRIORITY_EMERGENCY)))
        {
            wszTitle[0] = 0;
            wstrFmt[0] = 0;
            (void)STRTOWSTR("%s !\n", wstrFmt, sizeof(wstrFmt));
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                        AEE_WMSAPPRES_LANGFILE,
                                        ((pMessage->m_epriority == WMS_PRIORITY_URGENT) ? IDS_URGENT : IDS_EMERGENCY),
                                        wszTitle,
                                        sizeof(wszTitle));
                                        
            dwSize = (WSTRLEN(wszTitle)+WSTRLEN(wstrFmt)+1) * sizeof(AECHAR);
            wstrPriFld = (AECHAR *)MALLOC(dwSize);
            
            if (NULL == wstrPriFld)
            {
                ERR("MALLOC Failed!",0,0,0);
                goto FMTMSG_ERR;
            }
            WSPRINTF(wstrPriFld, dwSize, wstrFmt, wszTitle);

            dwTotalLen += WSTRLEN(wstrPriFld);
        }
#endif        
        
        // 准备 wstrAddFld 
        nLenNum = WSTRLEN(pMessage->m_szNum);
        MSG_FATAL("nLenNum===================%d",nLenNum,0,0);
        if (nLenNum>0)
        {
            wszTitle[0] = 0;
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                        AEE_WMSAPPRES_LANGFILE,
                                        nHeaderResID,
                                        wszTitle,
                                        sizeof(wszTitle));
            
            // 从电话本中取人名
            WMSUtil_GetContactName(pMe, 
                                   pMessage->m_szNum, 
                                   wszName, 
                                   MAX_TITLE_LEN); 
            
            wstrFmt[0] = 0;
            nName = WSTRLEN(wszName);
            if (nName > 0)
            {
                // 发送至/来自：\n 号码 \n 人名 \n\n
                (void)STRTOWSTR("%s\n%s\n%s\n\n", wstrFmt, sizeof(wstrFmt));
            }
            else
            {
                // 发送至/来自：\n 号码\n\n
                (void)STRTOWSTR("%s\n%s\n\n", wstrFmt, sizeof(wstrFmt));
            }
            dwSize = (WSTRLEN(wstrFmt) + nName + WSTRLEN(wszTitle) + nLenNum + 1) * sizeof(AECHAR);
            
            wstrAddFld = (AECHAR *)MALLOC(dwSize);
            DBGPRINTF("WMS Address = 0x%x", wstrAddFld);
            if (NULL == wstrAddFld)
            {
                ERR("MALLOC Failed!",0,0,0);
                goto FMTMSG_ERR;
            }
            
            if (nName > 0)
            {
                // 发送至/来自：\n 号码 \n 人名 \n\n
                WSPRINTF(wstrAddFld, dwSize, wstrFmt, wszTitle, pMessage->m_szNum, wszName);
            }
            else
            {
                // 发送至/来自：\n 号码\n\n
                WSPRINTF(wstrAddFld, dwSize, wstrFmt, wszTitle, pMessage->m_szNum);
            }
        }
        
        // 准备回叫号码段
        nLenNum = WSTRLEN(pMessage->m_szCallBkNum);
        if (nLenNum)
        {
            int nTepLen = WSTRLEN(wstrAddFld);
            
            wszTitle[0] = 0;
            wszName[0] = 0;
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                        AEE_WMSAPPRES_LANGFILE,
                                        IDS_CALLBACKNUM,
                                        wszTitle,
                                        sizeof(wszTitle));
            
            // 从电话本中取人名
            WMSUtil_GetContactName(pMe, 
                                   pMessage->m_szCallBkNum, 
                                   wszName, 
                                   MAX_TITLE_LEN); 
            
            wstrFmt[0] = 0;
            nName = WSTRLEN(wszName);
            if (nName > 0)
            {
                // 回叫号码：\n 号码 \n 人名 \n\n
                (void)STRTOWSTR("%s:\n%s\n%s\n\n", wstrFmt, sizeof(wstrFmt));
            }
            else
            {
                // 回叫号码：\n 号码\n\n
                (void)STRTOWSTR("%s:\n%s\n\n", wstrFmt, sizeof(wstrFmt));
            }
            dwSize = (WSTRLEN(wstrFmt) + nName + WSTRLEN(wszTitle) + nLenNum + nTepLen + 1) * sizeof(AECHAR);
            
            wstrAddFld = (AECHAR *)REALLOC((void *)wstrAddFld, dwSize);
            if (NULL == wstrAddFld)
            {
                ERR("MALLOC Failed!",0,0,0);
                goto FMTMSG_ERR;
            }
            
            if (nName > 0)
            {
                // 回叫号码：\n 号码 \n 人名 \n\n
                WSPRINTF(&wstrAddFld[nTepLen], dwSize, wstrFmt, wszTitle, pMessage->m_szCallBkNum, wszName);
            }
            else
            {
                // 回叫号码：\n 号码\n\n
                WSPRINTF(&wstrAddFld[nTepLen], dwSize, wstrFmt, wszTitle, pMessage->m_szCallBkNum);
            }
        }
        dwTotalLen += WSTRLEN(wstrAddFld);
    }
    
    // 消息时间日期部份
    // 准备 wstrDtFld
    if (pMessage->m_dwTime > 0)
    {
        AECHAR  wstrDT[MAX_TITLE_LEN]={0};
        WMSUtil_SecsToDateString(pMe, pMessage->m_dwTime, wstrDT, sizeof(wstrDT));
        wstrDtFld = WSTRDUP(wstrDT);
        
        if (NULL == wstrDtFld)
        {
            ERR("MALLOC Failed!",0,0,0);
            goto FMTMSG_ERR;
        }

        dwTotalLen += WSTRLEN(wstrDtFld);
    }

    // 加消息文本体
    if (pMe->m_eMBoxType == WMS_MB_VOICEMAIL)
    { // 语音通知消息显示内容与一般短信不同
        AECHAR wstrMsg[64] = {0};

        wstrFmt[0] = 0;
        (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                    IDS_VOICEMAIL_FMT,
                    wstrFmt, sizeof(wstrFmt));
        WSPRINTF(wstrMsg, sizeof(wstrMsg), wstrFmt, pMessage->m_NumMsgs);
        wstrContentFld = WSTRDUP(wstrMsg);
        if (NULL == wstrContentFld)
        {
            ERR("MALLOC Failed!",0,0,0);
            goto FMTMSG_ERR;
        }
        dwTotalLen += (WSTRLEN(wstrContentFld)+WSTRLEN(wszNewLine)*2);
        
        // 准备 wstrOptCall
        wstrMsg[0] = 0;
        wstrFmt[0] = 0;
        (void)STRTOWSTR("1. %s\n", wstrFmt, sizeof(wstrFmt));
        (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                    IDS_LISTENVM,
                    wstrMsg, sizeof(wstrMsg));
        dwSize = (WSTRLEN(wstrMsg) + WSTRLEN(wstrFmt) + 1) * sizeof(AECHAR);
        wstrOptCall = (AECHAR *)MALLOC(dwSize);
        DBGPRINTF("WMS Address = 0x%x", wstrOptCall);
        if (NULL == wstrOptCall)
        {
            ERR("MALLOC Failed!",0,0,0);
            goto FMTMSG_ERR;
        }
        WSPRINTF(wstrOptCall, dwSize, wstrFmt, wstrMsg);
        dwTotalLen += WSTRLEN(wstrOptCall);
        
#if !defined(FEATURE_CARRIER_VENEZUELA_MOVILNET)
        // 准备 wstrOptDel
        wstrMsg[0] = 0;
        wstrFmt[0] = 0;
        (void)STRTOWSTR("2. %s\n", wstrFmt, sizeof(wstrFmt));
        (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                    IDS_DELETE,
                    wstrMsg, sizeof(wstrMsg));
        dwSize = (WSTRLEN(wstrMsg) + WSTRLEN(wstrFmt) + 1) * sizeof(AECHAR);
        wstrOptDel = (AECHAR *)MALLOC(dwSize);
        DBGPRINTF("WMS Address = 0x%x", wstrOptDel);
        if (NULL == wstrOptDel)
        {
            ERR("MALLOC Failed!",0,0,0);
            goto FMTMSG_ERR;
        }
        WSPRINTF(wstrOptDel, dwSize, wstrFmt, wstrMsg);
        dwTotalLen += WSTRLEN(wstrOptDel);
#endif        
    }
    else
    {
        if (NULL != pMessage->m_szMessage)
        {
            wstrContentFld = WSTRDUP(pMessage->m_szMessage);
            if (NULL == wstrContentFld)
            {
                ERR("MALLOC Failed!",0,0,0);
                goto FMTMSG_ERR;
            }
            dwTotalLen += (WSTRLEN(wstrContentFld)+WSTRLEN(wszNewLine)*2);
        }
    }
    
    // 准备最终输出部份
    dwTotalLen++;
    dwSize = dwTotalLen * sizeof(AECHAR);

    wstrRet = (AECHAR *)MALLOC(dwSize);
    DBGPRINTF("WMS Address = 0x%x", wstrRet);
    if (NULL == wstrRet)
    {
        ERR("MALLOC Failed!",0,0,0);
        goto FMTMSG_ERR;
    }
    
    nLen = 0;
    if (wstrPriFld)
    {
        (void)WSTRCPY(&wstrRet[nLen], wstrPriFld);
        
        nLen = WSTRLEN(wstrRet);
    }
    
    if (wstrContentFld)
    {
        (void)WSTRCPY(&wstrRet[nLen], wstrContentFld);
        nLen = WSTRLEN(wstrRet);
        
        (void)WSTRCPY(&wstrRet[nLen], wszNewLine);
        nLen = WSTRLEN(wstrRet);
        
        (void)WSTRCPY(&wstrRet[nLen], wszNewLine);
        nLen = WSTRLEN(wstrRet);
    }
    
    if (wstrOptCall)
    {
        (void)WSTRCPY(&wstrRet[nLen], wstrOptCall);
        
        nLen = WSTRLEN(wstrRet);
    }
    
    if (wstrOptDel)
    {
        (void)WSTRCPY(&wstrRet[nLen], wstrOptDel);
        
        nLen = WSTRLEN(wstrRet);
    }
    
    if (wstrAddFld)
    {
        (void)WSTRCPY(&wstrRet[nLen], wstrAddFld);
        
        nLen = WSTRLEN(wstrRet);
    }
    
    if (wstrDtFld)
    {
        (void)WSTRCPY(&wstrRet[nLen], wstrDtFld);
        
        nLen = WSTRLEN(wstrRet);
    }
    
FMTMSG_ERR:
    FREEIF(wstrPriFld);
    FREEIF(wstrContentFld);
    FREEIF(wstrAddFld);
    FREEIF(wstrDtFld);
    FREEIF(wstrOptCall);
    FREEIF(wstrOptDel);
    
    return wstrRet;
} // FormatMessageForDisplay

/*==============================================================================
函数:
    FormatFlashSMSForDisplay

说明:
    将给定消息格式化已显示给用户。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    pMessage [in]: 给定消息。

返回值:
    指向格式化后的文本字符串。

备注:
    返回的字符串所用空间属于动态分配，需调用者释放。
        
==============================================================================*/
static AECHAR  * FormatFlashSMSForDisplay(WmsApp *pMe, WMSMessageStruct *pMessage)
{
    uint16    nHeaderResID = 0;
    int32     dwSize,dwTotalLen = 0;
    int       nLen = 0;
    
    AECHAR    *wstrPriFld = NULL;      // 消息优先级部份
    AECHAR    *wstrContentFld = NULL;  // 消息正文部份
    AECHAR    *wstrOptCall = NULL;     // 语音邮件听取选项
    AECHAR    *wstrOptDel = NULL;      // 语音邮件删除选项
    AECHAR    *wstrAddFld = NULL;      // 消息接收或发送人信息部份
    AECHAR    *wstrDtFld = NULL;       // 消息时间戳部份
    AECHAR    *wstrRet = NULL;         // 格式化后消息数据
    
    AECHAR    wszNewLine[] = {(AECHAR)'\n', 0};
    AECHAR    wstrFmt[32] = {0};
    AECHAR    wstrSpace[2] = {L"  "};

    if ((NULL == pMe) || (NULL == pMessage))
    {
    	MSG_FATAL("pMe IS NULL.....pMessage IS NULL ",0,0,0);
        return NULL;
    }
    
    // 先处理消息接收或发送人信息部份

    nHeaderResID = IDS_FROM;

    MSG_FATAL("nHeaderResID==========================%d ",nHeaderResID,0,0);
    if (nHeaderResID != 0)
    {   // 带标题
        AECHAR  wszTitle[MAX_TITLE_LEN]={0};
        AECHAR  wszName[MAX_TITLE_LEN]={0};
        int     nLenNum  = 0;
        int     nName  = 0;

        // 准备 wstrAddFld 
        nLenNum = WSTRLEN(pMessage->m_szNum);
        MSG_FATAL("nLenNum===================%d",nLenNum,0,0);
        if (nLenNum>0)
        {
            wszTitle[0] = 0;
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                        AEE_WMSAPPRES_LANGFILE,
                                        nHeaderResID,
                                        wszTitle,
                                        sizeof(wszTitle));
            
            // 从电话本中取人名
            WMSUtil_GetContactName(pMe, 
                                   pMessage->m_szNum, 
                                   wszName, 
                                   MAX_TITLE_LEN); 
            
            wstrFmt[0] = 0;
            nName = WSTRLEN(wszName);
            if (nName > 0)
            {
                // 发送至/来自：\n 号码 \n 人名 \n\n
                (void)STRTOWSTR("%s\n%s\n", wstrFmt, sizeof(wstrFmt));
            }
            else
            {
                // 发送至/来自：\n 号码\n\n
                (void)STRTOWSTR("%s\n", wstrFmt, sizeof(wstrFmt));
            }
            dwSize = (WSTRLEN(wstrFmt) + nName + WSTRLEN(wszTitle) + nLenNum + 1) * sizeof(AECHAR);
            
            wstrAddFld = (AECHAR *)MALLOC(dwSize);
            DBGPRINTF("WMS Address = 0x%x", wstrAddFld);
            if (NULL == wstrAddFld)
            {
                ERR("MALLOC Failed!",0,0,0);
                goto FMTMSG_ERR;
            }
            
            if (nName > 0)
            {
                // 发送至/来自：\n 号码 \n 人名 \n\n
                WSPRINTF(wstrAddFld, dwSize, wstrFmt, pMessage->m_szNum, wszName);
            }
            else
            {
                // 发送至/来自：\n 号码\n\n
                WSPRINTF(wstrAddFld, dwSize, wstrFmt, pMessage->m_szNum);
            }
        }
        
        // 准备回叫号码段
        nLenNum = WSTRLEN(pMessage->m_szCallBkNum);
        if (nLenNum)
        {
            int nTepLen = WSTRLEN(wstrAddFld);
            
            wszTitle[0] = 0;
            wszName[0] = 0;
            (void) ISHELL_LoadResString(pMe->m_pShell,
                                        AEE_WMSAPPRES_LANGFILE,
                                        IDS_CALLBACKNUM,
                                        wszTitle,
                                        sizeof(wszTitle));
            
            // 从电话本中取人名
            WMSUtil_GetContactName(pMe, 
                                   pMessage->m_szCallBkNum, 
                                   wszName, 
                                   MAX_TITLE_LEN); 
            
            wstrFmt[0] = 0;
            nName = WSTRLEN(wszName);
            if (nName > 0)
            {
                // 回叫号码：\n 号码 \n 人名 \n\n
                (void)STRTOWSTR("%s:\n%s\n", wstrFmt, sizeof(wstrFmt));
            }
            else
            {
                // 回叫号码：\n 号码\n\n
                (void)STRTOWSTR("%s:\n%s\n", wstrFmt, sizeof(wstrFmt));
            }
            dwSize = (WSTRLEN(wstrFmt) + nName + WSTRLEN(wszTitle) + nLenNum + nTepLen + 1) * sizeof(AECHAR);
            
            wstrAddFld = (AECHAR *)REALLOC((void *)wstrAddFld, dwSize);
            if (NULL == wstrAddFld)
            {
                ERR("MALLOC Failed!",0,0,0);
                goto FMTMSG_ERR;
            }
            
            if (nName > 0)
            {
                // 回叫号码：\n 号码 \n 人名 \n\n
                WSPRINTF(&wstrAddFld[nTepLen], dwSize, wstrFmt, pMessage->m_szCallBkNum, wszName);
            }
            else
            {
                // 回叫号码：\n 号码\n\n
                WSPRINTF(&wstrAddFld[nTepLen], dwSize, wstrFmt, pMessage->m_szCallBkNum);
            }
        }
        dwTotalLen += WSTRLEN(wstrAddFld);
    }
    
    // 消息时间日期部份
    // 准备 wstrDtFld
    if (pMessage->m_dwTime > 0)
    {
        AECHAR  wstrDT[MAX_TITLE_LEN]={0};
        WMSUtil_SecsToDateString(pMe, pMessage->m_dwTime, wstrDT, sizeof(wstrDT));
        wstrDtFld = WSTRDUP(wstrDT);
        
        if (NULL == wstrDtFld)
        {
            ERR("MALLOC Failed!",0,0,0);
            goto FMTMSG_ERR;
        }

        dwTotalLen += WSTRLEN(wstrDtFld);
    }
    
    if (NULL != pMessage->m_szMessage)
    {
        wstrContentFld = WSTRDUP(pMessage->m_szMessage);
        if (NULL == wstrContentFld)
        {
            ERR("MALLOC Failed!",0,0,0);
            goto FMTMSG_ERR;
        }
        dwTotalLen += (WSTRLEN(wstrContentFld)+WSTRLEN(wszNewLine)*2);
    }
    
    // 准备最终输出部份
    dwTotalLen++;
    dwSize = dwTotalLen * sizeof(AECHAR);

    wstrRet = (AECHAR *)MALLOC(dwSize);
    DBGPRINTF("WMS Address = 0x%x", wstrRet);
    if (NULL == wstrRet)
    {
        ERR("MALLOC Failed!",0,0,0);
        goto FMTMSG_ERR;
    }
    
    nLen = 0;
    if (wstrPriFld)
    {
        (void)WSTRCPY(&wstrRet[nLen], wstrPriFld);
        
        nLen = WSTRLEN(wstrRet);
    }
    if (wstrAddFld)
    {
        (void)WSTRCPY(&wstrRet[nLen], wstrAddFld);
        nLen = WSTRLEN(wstrRet);

    }
    if (wstrDtFld)
    {
        (void)WSTRCPY(&wstrRet[nLen], wstrDtFld);
        nLen = WSTRLEN(wstrRet);
        
        (void)WSTRCPY(&wstrRet[nLen], wszNewLine);
        nLen = WSTRLEN(wstrRet);
    }
    if (wstrContentFld)
    {
        (void)WSTRCPY(&wstrRet[nLen], wstrContentFld);
        nLen = WSTRLEN(wstrRet);
        
		/*
        (void)WSTRCPY(&wstrRet[nLen], wszNewLine);
        nLen = WSTRLEN(wstrRet);
        
        (void)WSTRCPY(&wstrRet[nLen], wszNewLine);
        nLen = WSTRLEN(wstrRet);
        */
    }
    /*
    if (wstrOptCall)
    {
        (void)WSTRCPY(&wstrRet[nLen], wstrOptCall);
        
        nLen = WSTRLEN(wstrRet);
    }
    
    
    if (wstrOptDel)
    {
        (void)WSTRCPY(&wstrRet[nLen], wstrOptDel);
        
        nLen = WSTRLEN(wstrRet);
    }
    
    if (wstrAddFld)
    {
        (void)WSTRCPY(&wstrRet[nLen], wstrAddFld);
        
        nLen = WSTRLEN(wstrRet);
    }
    
    if (wstrDtFld)
    {
        (void)WSTRCPY(&wstrRet[nLen], wstrDtFld);
        
        nLen = WSTRLEN(wstrRet);
    }
    */
FMTMSG_ERR:
    FREEIF(wstrPriFld);
    FREEIF(wstrContentFld);
    FREEIF(wstrAddFld);
    FREEIF(wstrDtFld);
    FREEIF(wstrOptCall);
    FREEIF(wstrOptDel);
    
    return wstrRet;
} // FormatMessageForDisplay

/*==============================================================================
函数:
    IDD_VOICEMAIL_Handler

说明:
    WMS Applet对话框 IDD_VOICEMAIL 事件处理函数。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_VOICEMAIL_Handler(void   *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;

    if (NULL == pMe)
    {
        return FALSE;
    }

    pMenu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_VOICEMAIL_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
#endif
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,                                
                        IDS_VOICEMAIL,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);
            
            // 菜单项初始化
            MENU_ADDITEM(pMenu, IDS_VOICEMAIL);
            MENU_ADDITEM(pMenu, IDS_LISTENVM);
            MENU_ADDITEM(pMenu, IDS_VMNUM);
            IMENUCTL_SetSel(pMenu, pMe->m_wPrevMenuSel);
            return TRUE;

        case EVT_DIALOG_START:
        // 有新消息
        case EVT_WMS_MSG_RECEIVED_MESSAGE:
            {
                AECHAR *pwsz = NULL;
                uint32 dwSize;
                int nLen;
                AECHAR wstrFmt[20] = {0};
                uint16 nMsgs=0;
            
                (void)STRTOWSTR("(%d)",wstrFmt,sizeof(wstrFmt));
                if (eCode != EVT_WMS_MSG_RECEIVED_MESSAGE)
                {
                    (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                            EVTFLG_ASYNC,
                                            AEECLSID_WMSAPP,
                                            EVT_USER_REDRAW,
                                            0, 
                                            0);
                }
                
                dwSize = 64 * sizeof(AECHAR);
                pwsz = (AECHAR *)MALLOC(dwSize);
                DBGPRINTF("WMS Address = 0x%x", pwsz);
                if (NULL == pwsz)
                {
                    return FALSE;
                }
                
                // 格式化语音信箱菜单项文本
                nMsgs = 0;
                wms_cacheinfolist_getcounts(WMS_MB_VOICEMAIL, NULL, NULL, &nMsgs);

#if defined(FEATURE_CARRIER_THAILAND_HUTCH)
                (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                            IDS_VOICEMAIL_NOTI,
                            pwsz, dwSize);

#else
                (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                            IDS_VOICEMAIL,
                            pwsz, dwSize);
#endif   

                nLen = WSTRLEN(pwsz);
                //if (nLen<30)
                {
                    WSPRINTF(&pwsz[nLen], (64-nLen)*sizeof(AECHAR), wstrFmt, nMsgs); 
                }
#if defined(FEATURE_CARRIER_THAILAND_HUTCH)
                IMENUCTL_SetItemText(pMenu, IDS_VOICEMAIL_NOTI, NULL, NULL, pwsz);
#else
                IMENUCTL_SetItemText(pMenu, IDS_VOICEMAIL, NULL, NULL, pwsz);
#endif           
                FREE(pwsz);
            }

            return TRUE;

        case EVT_USER_REDRAW:
           // (void)IMENUCTL_Redraw(pMenu);   //dele by yangdecai
            return TRUE;

        case EVT_CTL_SEL_CHANGED:
            pMe->m_wPrevMenuSel = wParam;
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            pMe->m_wPrevMenuSel = wParam;
            switch (wParam)
            {
                // 查看语音短信
#if defined(FEATURE_CARRIER_THAILAND_HUTCH)
                case IDS_VOICEMAIL_NOTI:
#else
                case IDS_VOICEMAIL:
#endif
                    {
                        wms_cache_info_node  *pnode = NULL;
                        
                        pnode = wms_cacheinfolist_getnode(pMe->m_eMBoxType, WMS_MEMORY_STORE_NV_CDMA, 0);
                        if (NULL != pnode)
                        {
                            // 发送读消息命令
                            (void)IWMS_MsgRead(pMe->m_pwms,
                                               pMe->m_clientId,
                                               &pMe->m_callback,
                                               (void *)pMe,
                                               pnode->mem_store,
                                               0);
                                               
                            if (pnode->msg_tag == WMS_TAG_MT_NOT_READ)
                            {
                                (void)IWMS_MsgModifyTag(pMe->m_pwms,
                                                        pMe->m_clientId,
                                                        &pMe->m_callback,
                                                        (void *)pMe,
                                                        pnode->mem_store,
                                                        pnode->index,
                                                        WMS_TAG_MT_READ);
                            }
                            
                            return TRUE;
                        }
                    }
                    CLOSE_DIALOG(DLGRET_VIEWVOICEMSG)
                    return TRUE;

                // 拨打语音信箱
                case IDS_LISTENVM:
                    {// 检查语音信箱号码是否设置，若设置则直接拨出
                        AECHAR    wstrVMNum[OEMNV_VOICEMAIL_MAXLEN]={0};
                        
                        (void) ICONFIG_GetItem(pMe->m_pConfig,
                                               CFGI_VOICEMAIL_NUMBER,
                                               wstrVMNum,
                                               sizeof(wstrVMNum));
                        
                        if (WSTRLEN(wstrVMNum)>0)
                        {
                            // 调用呼叫接口，本 Applet 会被挂起，返回时回到当前状态
                            WMSExtApp_CallPhoneNumber(pMe, wstrVMNum, FALSE);
                        }
                        else
                        {
                            CLOSE_DIALOG(DLGRET_OK)
                        }
                    }
                    
                    return TRUE;
                
                // 配置语音信箱号码
                case IDS_VMNUM:
                    CLOSE_DIALOG(DLGRET_VMNUM)
                    return TRUE;
                
                default:
                    break;
            }
            
            return FALSE;

        case EVT_WMS_MSG_READ:
            // 读消息命令返回
            {
                wms_memory_store_e_type mem_store;
                wms_message_index_type  wIndex;
                wms_msg_event_info_s_type *Info = (wms_msg_event_info_s_type*)dwParam;
                
                mem_store = Info->status_info.message.msg_hdr.mem_store;
                wIndex = Info->status_info.message.msg_hdr.index;
                
                if ((mem_store == WMS_MEMORY_STORE_NV_CDMA) && (wIndex == 0))
                {
                    WmsApp_ConvertClientMsgToMS(&(Info->status_info.message),
                                                &(pMe->m_msCur));
                    pMe->m_msCur.mem_store = mem_store;
                    MSG_FATAL("EVT_WMS_MSG_READ",0,0,0);
                    // 更新消息图标
                    WmsApp_UpdateAnnunciators(pMe);
                    
                    CLOSE_DIALOG(DLGRET_VIEWVOICEMSG)
                }
            }
            return TRUE;
            
        default:
            break;
    }

    return FALSE;
} // IDD_VOICEMAIL_Handler

/*==============================================================================
函数:
    IDD_VMNUM_Handler

说明:
    WMS Applet对话框 IDD_VMNUM 事件处理函数。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_VMNUM_Handler(void   *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    ITextCtl *pTextCtl = NULL;
    WmsApp *pMe = (WmsApp *)pUser;
    MSG_FATAL("IDD_VMNUM_Handler eCode=0x%x",eCode, 0, 0);
    if (NULL == pMe)
    {
        return FALSE;
    }

    pTextCtl = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_VMNUM_TEXT);
    if (NULL == pTextCtl)
    {
        return FALSE;
    }

    switch(eCode)
    {
        case EVT_DIALOG_INIT:
            ITEXTCTL_SetProperties(pTextCtl, TP_FRAME | TP_NOUPDATE|TP_FOCUS_NOSEL);
            return TRUE;

        case EVT_DIALOG_START:
            SetControlRect(pMe, pTextCtl);
            ITEXTCTL_SetMaxSize(pTextCtl, OEMNV_VOICEMAIL_MAXLEN-1);
            
            if (WSTRLEN(pMe->m_msSend.m_szNum)>0)
            {// 这是从挂起状态恢复
                (void) ITEXTCTL_SetText(pTextCtl, pMe->m_msSend.m_szNum, -1);
            }
            else
            {// 检查语音信箱号码是否设置，若设置则直接拨出
                AECHAR    wstrVMNum[OEMNV_VOICEMAIL_MAXLEN]={0};
                
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_VOICEMAIL_NUMBER,
                                       wstrVMNum,
                                       sizeof(wstrVMNum));
                
                (void) ITEXTCTL_SetText(pTextCtl, wstrVMNum, -1);
            }

            (void) ITEXTCTL_SetInputMode(pTextCtl, AEE_TM_NUMBERS);       
            {
              AECHAR WTitle[40] = {0};
              (void)ISHELL_LoadResString(pMe->m_pShell,
              AEE_WMSAPPRES_LANGFILE,                                
              IDS_VOICEMAIL,
              WTitle,
              sizeof(WTitle));
              if(pMe->m_pIAnn != NULL)
              {
                  IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
              }
            }    
            
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;

        case EVT_USER_REDRAW:
        case EVT_KEY_RELEASE:
            // 绘制底条提示
            {
                AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pTextCtl);
                int nLen = 0;
                
                if (NULL != pwstrText)
                {
                    nLen = WSTRLEN(pwstrText);
                }
                
                if (nLen > 0)
                {
                	#ifndef FEATURE_ALL_KEY_PAD
                    if (ITEXTCTL_GetCursorPos(pTextCtl) != TC_CURSORSTART)
                    {
                        // Save       Delete
                        DRAW_BOTTOMBAR(BTBAR_SAVE_DELETE)
                    }
                    else
                    #endif
                    {
                        // Save       Back
                        DRAW_BOTTOMBAR(BTBAR_SAVE_BACK)
                    }
                }
                else
                {
                    //            Back
                    DRAW_BOTTOMBAR(BTBAR_BACK)
                }
            }
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            #ifdef FEATURE_LCD_TOUCH_ENABLE
            TSIM_NumberKeypad(FALSE);
            #endif
            (void)ITEXTCTL_Redraw(pTextCtl);
            
            return TRUE;
            
        case EVT_DIALOG_END:
            if ((pMe->m_eDlgReturn != DLGRET_CANCELED) ||
                (pMe->m_eDlgReturn != DLGRET_OK))
            {
                // Applet 被挂起时关闭本对话框，保存输入，待返回时恢复
                MEMSET(pMe->m_msSend.m_szNum, 0, sizeof(pMe->m_msSend.m_szNum));
                        
                (void) ITEXTCTL_GetText(pTextCtl, pMe->m_msSend.m_szNum, MAX_PH_DIGITS);
            }
            #ifdef FEATURE_LCD_TOUCH_ENABLE
            TSIM_NumberKeypad(FALSE);
            #endif
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                case AVK_INFO:
                case AVK_SELECT:
                    {
                        AECHAR    wstrVMNum[OEMNV_VOICEMAIL_MAXLEN]={0};
                        
                        (void) ITEXTCTL_GetText(pTextCtl,
                                        wstrVMNum,
                                        OEMNV_VOICEMAIL_MAXLEN);
      
                        if (WSTRLEN(wstrVMNum) == 0) 
                        {
                            return TRUE;
                        }   
                        
                        (void) ICONFIG_SetItem(pMe->m_pConfig,
                                    CFGI_VOICEMAIL_NUMBER,
                                    wstrVMNum,
                                    sizeof(wstrVMNum));
                    }
      
                    CLOSE_DIALOG(DLGRET_OK)
                    return TRUE;
  
                default:
                    break;
            }
            return TRUE;
#ifdef FEATURE_LCD_TOUCH_ENABLE//andrew add for LCD touch
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

				if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
				{
                    if ((wXPos>0)&&(wXPos<devinfo.cxScreen/3)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
                    {
                        boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_SELECT,0);
                        return rt;
                    }
                    else if ((wXPos>2*(devinfo.cxScreen/3))&&(wXPos<devinfo.cxScreen)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
                    {
                        AECHAR *pwstrText = ITEXTCTL_GetTextPtr(pTextCtl);
                        int nLen = 0;
                        boolean rt = FALSE;
                        int nCount = 0;
                        if (NULL != pwstrText)
                        {
                            nLen = WSTRLEN(pwstrText);
                        }
                        MSG_FATAL("IDD_VMNUM_Handler:nLen=%d",nLen,0,0);
                        if(nLen == 0)
                        {
                            rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_CLR,0);
                        }
                        else
                        {

                            AECHAR newstrText[MAX_PH_DIGITS] = {0};
                            nCount = WSTRNCOPYN(newstrText,sizeof(newstrText),pwstrText,nLen-1);
                            if(nCount == 0)
                            {
                                ITEXTCTL_Reset(pTextCtl);
                                ITEXTCTL_SetActive(pTextCtl,TRUE);
                            }
                            else
                            {
                                rt = ITEXTCTL_SetText(pTextCtl,newstrText,WSTRLEN(newstrText));
                            }
                            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
                        }
                        MSG_FATAL("IDD_VMNUM_Handler:rt=%d,nCount=%d",rt,nCount,0);
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
} // IDD_VMNUM_Handler()

/*==============================================================================
函数:
    IDD_LOADINGMSG_Handler

说明:
    WMS Applet对话框 IDD_LOADINGMSG 事件处理函数。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:
    对话框大小为0, 以隐藏在后台加载消息。
==============================================================================*/
static boolean IDD_LOADINGMSG_Handler(void   *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    WmsApp *pMe = (WmsApp *)pUser;
    MSG_FATAL("IDD_LOADINGMSG_Handler eCode=0x%x, wParam=0x%x, dwParam=0x%x",eCode,wParam,dwParam);
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    if (NULL == pMe->m_pStatic)
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
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;
            
        case EVT_USER_REDRAW:
            MSG_FATAL("IDD_LOADINGMSG_Handler EVT_USER_REDRAW m_currState=%d",pMe->m_currState,0,0);
            if (WMSST_TEMPLATES == pMe->m_currState)
            {
                // 读取模版
                wms_message_index_type index = pMe->m_wInsertTemplate-MSG_CMD_BASE;
                MSG_FATAL("IDD_LOADINGMSG_Handler EVT_USER_REDRAW 1 index=%d",index,0,0);
                
                if (pMe->m_wInsertTemplate>=MSG_CMD_BASE)
                {// 用户常用语
                    if (index >= PHRASE_START)
                    {
                        (void)IWMS_MsgRead(pMe->m_pwms,
                                           pMe->m_clientId,
                                           &pMe->m_callback,
                                           (void *)pMe,
                                           WMS_MEMORY_STORE_NV_CDMA,
                                           index);
                    }
                    else
                    {// 卡上模板消息
                        (void)IWMS_MsgReadTemplate(pMe->m_pwms,
                                           pMe->m_clientId,
                                           &pMe->m_callback,
                                           (void *)pMe,
                                           WMS_MEMORY_STORE_RUIM,
                                           index);
                    }
                }
                else
                {// 内置常用语
                    (void) IDD_LOADINGMSG_Handler(pMe, EVT_WMS_MSG_READ, 0, 0);
                }
            }
            else
            {// 读取消息
                uint16 wIndex=0;
                wms_cache_info_node  *pnode = NULL;
                int nRet,i,nCount=0;
                boolean bUIMSMS = FALSE;
                MSG_FATAL("IDD_LOADINGMSG_Handler EVT_USER_REDRAW 2 wIndex=%d",pMe->m_wCurindex,0,0);
                wIndex = pMe->m_wCurindex;
                MSG_FATAL("EVT_USER_REDRAW,read.....wIndex=%d",wIndex,0,0);
                // 取消息 cache info 节点
                if (wIndex>=RUIM_MSGINDEX_BASE)
                {
                    wIndex = wIndex - RUIM_MSGINDEX_BASE;
                    pnode = wms_cacheinfolist_getnode(pMe->m_eMBoxType, WMS_MEMORY_STORE_RUIM, wIndex);
                }
                else
                {
                    pnode = wms_cacheinfolist_getnode(pMe->m_eMBoxType, WMS_MEMORY_STORE_NV_CDMA, wIndex);
                }
                MSG_FATAL("EVT_USER_REDRAW, wIndex=%d",wIndex,0,0);
                if (NULL == pnode)
                {
                    CLOSE_DIALOG(DLGRET_LOADFAILED)
                    return TRUE;
                }
                
                // 重置当前消息列表
                MEMSET(pMe->m_CurMsgNodes, 0, sizeof(pMe->m_CurMsgNodes));
                WmsApp_FreeMsgNodeMs(pMe);
                
                pMe->m_idxCur = 0;
#ifdef FEATURE_SMS_UDH
                if (pnode->pItems != NULL)
                {
                    MEMCPY(pMe->m_CurMsgNodes, pnode->pItems, sizeof(pMe->m_CurMsgNodes));
                    
                    for (; pMe->m_idxCur<LONGSMS_MAX_PACKAGES; pMe->m_idxCur++)
                    {
                        if (pMe->m_CurMsgNodes[pMe->m_idxCur] != NULL)
                        {
                            pnode = pMe->m_CurMsgNodes[pMe->m_idxCur];
                            break;
                        }
                    }
                }
                else
#endif
                {
                    pMe->m_CurMsgNodes[0] = pnode;
                }
                
                // 确定是否需要显示界面
                for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
                {
                    if (pMe->m_CurMsgNodes[i] != NULL)
                    {
                        nCount++;
                        if ((pMe->m_CurMsgNodes[i])->mem_store == WMS_MEMORY_STORE_RUIM)
                        {
                            bUIMSMS = FALSE;  // 这里画出loading会与下一界面的option菜单重叠，所以干脆不画
                        }
                    }
                }
                
                if ((nCount>2) && bUIMSMS)
                {// 考虑到慢卡，需要显示界面
                    PromptMsg_Param_type  Msg_Param={0};
                    AECHAR  wstrText[MSGBOX_MAXTEXTLEN];
                    
                    // 从资源文件取消息内容
                    (void)ISHELL_LoadResString(pMe->m_pShell,
                                    AEE_WMSAPPRES_LANGFILE,                                
                                    IDS_LOADMSG,
                                    wstrText,
                                    sizeof(wstrText));
                                    
                    Msg_Param.ePMsgType = MESSAGE_INFORMATION;
                    Msg_Param.pwszMsg = wstrText;
                    Msg_Param.eBBarType = BTBAR_NONE;
                    DrawPromptMessage(pMe->m_pDisplay, pMe->m_pStatic, &Msg_Param);

                    // 更新界面
                    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                }
                
                pMe->m_eCurStore = pnode->mem_store;
                
                // 发送读消息命令
                nRet = IWMS_MsgRead(pMe->m_pwms,
                                   pMe->m_clientId,
                                   &pMe->m_callback,
                                   (void *)pMe,
                                   pnode->mem_store,
                                   pnode->index);
                MSG_FATAL("READ msg....... index=%d",pnode->index,0,0);
                                   
                if (nRet != SUCCESS)
                {
                    CLOSE_DIALOG(DLGRET_LOADFAILED)
                    return TRUE;
                }
            }
            return TRUE;

        case EVT_KEY_RELEASE:
            return TRUE;

        case EVT_DIALOG_END:
            if (pMe->m_eDlgReturn==DLGRET_CREATE)
            {
                pMe->m_eDlgReturn = DLGRET_LOADCANCELED;
            }
            
            ISTATIC_Release(pMe->m_pStatic);
            pMe->m_pStatic = NULL;
            return TRUE;
            
        case EVT_KEY:
            switch(wParam)
            { 
                case AVK_CLR:
                    MSG_FATAL (" AVK_CLR  ", 0, 0, 0);
                    CLOSE_DIALOG(DLGRET_LOADCANCELED)
                    return TRUE;
                    
                default: 
                    break;
            }
            return TRUE;
            
        case EVT_WMS_MSG_READ_TEMPLATE:
        case EVT_WMS_MSG_READ:
            MSG_FATAL("EVT_WMS_MSG_READ.............receve",0,0,0);
            if (eCode == WMS_MSG_EVENT_READ_TEMPLATE && 
                WMSST_TEMPLATES != pMe->m_currState)
            {
                return TRUE;
            }
            // 读消息命令返回
            if ((WMSST_TEMPLATES == pMe->m_currState) ||
                (eCode == WMS_MSG_EVENT_READ_TEMPLATE))
            {// 读取模版
                AECHAR      *wstrTemplate=NULL;
                int32       dwSize;
                
                dwSize = (MAX_TEMPLATECHARS + 1) * sizeof(AECHAR);
                wstrTemplate = (AECHAR*)MALLOC(dwSize);
                DBGPRINTF("WMS Address = 0x%x", wstrTemplate);
                if (NULL == wstrTemplate)
                {
                    CLOSE_DIALOG(DLGRET_LOADCANCELED)
                    return TRUE;
                }
                if (pMe->m_wInsertTemplate>=MSG_CMD_BASE)
                {// 用户常用语--通过读命令返回
                    wms_msg_event_info_s_type *Info = (wms_msg_event_info_s_type*)dwParam;
                    WMSMessageStruct  *pMsg = (WMSMessageStruct  *)MALLOC(sizeof(WMSMessageStruct));
                    DBGPRINTF("WMS Address = 0x%x", pMsg);    
                    if (pMsg == NULL)
                    {
                        FREE(wstrTemplate);
                        CLOSE_DIALOG(DLGRET_LOADCANCELED)
                        return TRUE;
                    }
                        
                    WmsApp_ConvertClientMsgToMS(&(Info->status_info.message), pMsg);
                    
                    if (pMsg->m_szMessage != NULL)
                    {
                        (void)WSTRCPY(wstrTemplate, pMsg->m_szMessage);
                    }
                    
                    WMSMessageStruct_Reset(pMsg);
                    FREE(pMsg);
                }
                else
                {// 内置常用语直接读取资源文件
                    (void)ISHELL_LoadResString(pMe->m_pShell, 
                                AEE_WMSAPPRES_LANGFILE,
                                pMe->m_wInsertTemplate,
                                wstrTemplate, 
                                dwSize);
                }
                
                if ((pMe->m_eInsertType == INSERT_EDITWMS) ||
                    (pMe->m_eInsertType == INSERT_EDITTEPLATE))
                {// 插入模板
                    AECHAR* wszTep;
                    AECHAR* szMessage = pMe->m_msSend.m_szMessage;
                    int startPos = pMe->m_dwInsertPos; 
                    int  nLen,nCanInsert,nLentemp;
                    int  nMax = 0;
                    int  nSize = 0;
                    
                    if (startPos < 0)
                    {
                        startPos = 0;
                    }
                    
                    if (NULL == pMe->m_msSend.m_szMessage)
                    {
                        pMe->m_msSend.m_szMessage = wstrTemplate;
                        MSG_FATAL("Loading....................ok1",0,0,0);
                        CLOSE_DIALOG(DLGRET_LOADOK)
                        return TRUE;
                    }
                    nLen = WSTRLEN(szMessage);
                    nLentemp = WSTRLEN(wstrTemplate);
                    
                    pMe->m_bTextFullAlert = TRUE;
                    
                    if (pMe->m_eInsertType == INSERT_EDITWMS)
                    {
                        nMax = pMe->m_nMOmaxChars;
                    }
                    else
                    {
                        nMax = MAX_TEMPLATECHARS;
                    }
    
                    // 计算当前最多允许插入的字符数
                    if (nLen >= nMax)
                    {// 不能插入
                        nCanInsert = 0;
                    }
                    else
                    {
                        if (pMe->m_eInsertType == INSERT_EDITWMS)
                        {
#if defined FEATURE_CARRIER_THAILAND_HUTCH || defined FEATURE_CARRIER_THAILAND_CAT        
                            if (AEE_TM_THAI == pMe->m_CurInputMode || 
                                AEE_TM_THAI_R == pMe->m_CurInputMode ||
                                WMSUtil_HaveNoneASCIIChar(szMessage, NULL))
                            {
                                nCanInsert = MAX_UNICODMSG_PAYLOAD - nLen;
                                
                                if (nCanInsert < 1)
                                {// 不能插入
                                    nCanInsert = 0;
                                }
                                else if (nCanInsert > nLentemp)
                                {
                                    pMe->m_bTextFullAlert = FALSE;
                                }
                            }
                            else
                            {
                                int nPos;
                                
                                if (nLen >= MAX_ASCIIMSG_PAYLOAD)
                                {// 不能插入
                                    nCanInsert = 0;
                                }
                                else if (WMSUtil_HaveNoneASCIIChar(wstrTemplate, &nPos))
                                {
                                    if (nLen + nLentemp < MAX_UNICODMSG_PAYLOAD)
                                    {// 可全部插入
                                        nCanInsert = nLentemp;
                                        pMe->m_bTextFullAlert = FALSE;
                                    }
                                    else if (nLen + nLentemp > MAX_UNICODMSG_PAYLOAD)
                                    {// 只能部分插入
                                        // 消息本身不含宽字符, 只能插入前面的非宽字符序列
                                        nCanInsert = (MAX_ASCIIMSG_PAYLOAD - nLen) >= nPos ? 
                                                    nPos : (MAX_ASCIIMSG_PAYLOAD - nLen);
                                                        
                                        if (nCanInsert+nLen < MAX_ASCIIMSG_PAYLOAD)
                                        {
                                            pMe->m_bTextFullAlert = FALSE;
                                        }               
                                    }
                                    else
                                    {
                                        nCanInsert = nLentemp;
                                    }
                                }
                                else
                                {
                                    nCanInsert = (MAX_ASCIIMSG_PAYLOAD - nLen) >= nLentemp ? 
                                                nLentemp : (MAX_ASCIIMSG_PAYLOAD - nLen);
                                                    
                                    if (nCanInsert+nLen < MAX_ASCIIMSG_PAYLOAD)
                                    {
                                        pMe->m_bTextFullAlert = FALSE;
                                    }               
                                }
                            }
#else
                            nCanInsert = nMax - nLen;
                            
                            if (nCanInsert < 1)
                            {// 不能插入
                                nCanInsert = 0;
                            }
                            else if (nCanInsert > nLentemp)
                            {
                                pMe->m_bTextFullAlert = FALSE;
                            }
#endif
                        }
                        else
                        {
                            nCanInsert = (nMax - nLen) >= nLentemp ? 
                                        nLentemp : (nMax - nLen);
                        }
                                        
                        if (nCanInsert+nLen < nMax)
                        {
                            pMe->m_bTextFullAlert = FALSE;
                        }               
                    }
                    
                    if (nCanInsert <= 0)
                    {
                        FREE(wstrTemplate);
                        
                        CLOSE_DIALOG(DLGRET_LOADOK)
                        return TRUE;
                    }
                    
                    nSize = (nLen + nCanInsert + 1) * sizeof(AECHAR);
                    wszTep = (AECHAR *)MALLOC(nSize);
                    DBGPRINTF("WMS Address = 0x%x", wszTep);    
                    if (wszTep != NULL)
                    {
                        MEMSET(wszTep, 0, nSize);
                        (void)WSTRNCOPYN(wszTep, (startPos+1), szMessage, startPos);
                        (void)WSTRNCOPYN(&wszTep[startPos], (nCanInsert+1), wstrTemplate, nCanInsert);
                        (void)WSTRCAT(wszTep, &szMessage[startPos]);
                        
                        wszTep[nLen+nCanInsert] = 0;
                        pMe->m_dwInsertPos = startPos + nCanInsert;
                        FREE(szMessage);
                        pMe->m_msSend.m_szMessage = wszTep;
                    }
                    
                    FREE(wstrTemplate);
                }
                else
                {
                    if (NULL != pMe->m_msSend.m_szMessage)
                    {
                        FREE(pMe->m_msSend.m_szMessage);
                    }
                    pMe->m_msSend.m_szMessage = wstrTemplate;
                }
                
                CLOSE_DIALOG(DLGRET_LOADOK)
            }
            else
            {// 读取消息
                wms_cache_info_node  *pnode = NULL;
                wms_memory_store_e_type mem_store;
                wms_message_index_type  wIdx;
                int nRet;
                wms_msg_event_info_s_type *Info = (wms_msg_event_info_s_type*)dwParam;
                
                mem_store = Info->status_info.message.msg_hdr.mem_store;
                wIdx = Info->status_info.message.msg_hdr.index;
                pnode = pMe->m_CurMsgNodes[pMe->m_idxCur];
                
                if (NULL == pnode)
                {
                    CLOSE_DIALOG(DLGRET_LOADFAILED)
                    return TRUE;
                }
                if ((mem_store == pnode->mem_store) && (pnode->index == wIdx))
                {
                    WMSMessageStruct *pTms = (WMSMessageStruct *)MALLOC(sizeof(WMSMessageStruct));
                    DBGPRINTF("WMS Address = 0x%x", pTms);   
                    // 保存读取的数据
                    if (NULL != pTms)
                    {
                    	
                        WmsApp_ConvertClientMsgToMS(&(Info->status_info.message), pTms);
                        pMe->m_CurMsgNodesMS[pMe->m_idxCur] = pTms;
                    }
                    
                    if ((pnode->msg_tag == WMS_TAG_MT_NOT_READ) &&
                        (pMe->m_eOptType == OPT_VIA_VIEWMSG))
                    {// 只有用户明确选择查看时才修改消息标记
                        nRet = IWMS_MsgModifyTag(pMe->m_pwms,
                                                 pMe->m_clientId,
                                                 &pMe->m_callback,
                                                 (void *)pMe,
                                                 pnode->mem_store,
                                                 pnode->index,
                                                 WMS_TAG_MT_READ);
                                                 
                        if (nRet != SUCCESS)
                        {
                            CLOSE_DIALOG(DLGRET_LOADFAILED)
                        }
                        
                        return TRUE;
                    }
                    
                    pnode = NULL;
                    
                    pMe->m_idxCur++;
                    for (; pMe->m_idxCur<LONGSMS_MAX_PACKAGES; pMe->m_idxCur++)
                    {
                    	
                        if (pMe->m_CurMsgNodes[pMe->m_idxCur] != NULL)
                        {
                            pnode = pMe->m_CurMsgNodes[pMe->m_idxCur];
                            break;
                        }
                    }
                    
                    if (NULL != pnode)
                    {// 读下一数据包
                        // 发送读消息命令
                        
                        nRet = IWMS_MsgRead(pMe->m_pwms,
                                           pMe->m_clientId,
                                           &pMe->m_callback,
                                           (void *)pMe,
                                           pnode->mem_store,
                                           pnode->index);
                                           
                        if (nRet != SUCCESS)
                        {
                            CLOSE_DIALOG(DLGRET_LOADFAILED)
                        }
                                           
                        return TRUE;
                    }
                    else
                    {
                        MSG_FATAL("WmsApp_CombinateMsg sssssss pMe->m_idxCur=%d",pMe->m_idxCur,0,0);
                        WmsApp_CombinateMsg(pMe);
                    	
                        CLOSE_DIALOG(DLGRET_LOADOK)
                    }
                }
            }
            
            return TRUE;
            
        case EVT_WMS_CMD_STATUS:
            {
                wmsapp_cmd_status_type *pStatus = (wmsapp_cmd_status_type *)dwParam;
                
                if (NULL == pStatus)
                {
                    return TRUE;
                }
                
                if (WMS_CMD_MSG_MODIFY_TAG == pStatus->cmd)
                {
                    wms_cache_info_node  *pnode = NULL;
                    int nRet;
                    
                    pMe->m_idxCur++;
                    for (; pMe->m_idxCur<LONGSMS_MAX_PACKAGES; pMe->m_idxCur++)
                    {
                        if (pMe->m_CurMsgNodes[pMe->m_idxCur] != NULL)
                        {
                            pnode = pMe->m_CurMsgNodes[pMe->m_idxCur];
                            break;
                        }
                    }
                    
                    if (NULL != pnode)
                    {// 读下一数据包
                        // 发送读消息命令
                        nRet = IWMS_MsgRead(pMe->m_pwms,
                                           pMe->m_clientId,
                                           &pMe->m_callback,
                                           (void *)pMe,
                                           pnode->mem_store,
                                           pnode->index);
                                           
                        if (nRet != SUCCESS)
                        {
                            CLOSE_DIALOG(DLGRET_LOADFAILED)
                        }
                                           
                        return TRUE;
                    }
                    else
                    {
                        MSG_FATAL("WmsApp_CombinateMsg sssssss",0,0,0);
                        WmsApp_CombinateMsg(pMe);
                    	
                        CLOSE_DIALOG(DLGRET_LOADOK)
                    }
                }
                ERR("Cmd:%d---Err:%d", pStatus->cmd, pStatus->cmd_err, 0);
            }
            
            return TRUE;
  
        default:
            break;
    }
  
    return FALSE;
} // IDD_LOADINGMSG_Handler

/*==============================================================================
函数:
    WmsApp_PlaySendingAni

说明:
    函数播放发送短信提示动画。

参数:
    pUser [in]: 

返回值:
    无。

备注:

==============================================================================*/
static void WmsApp_PlaySendingAni(void *pUser)
{
    AEEImageInfo info;
    AEERect rect;
    static int imageIndex = 0;
	AECHAR wszPrsend[64] = {0};
	AECHAR fmt_str[4] = {0};
	RGBVAL oldColor = 0;
    WmsApp *pMe = (WmsApp *)pUser;

    (void) ISHELL_CancelTimer(pMe->m_pShell, WmsApp_PlaySendingAni,pMe);
    STRTOWSTR("%d", fmt_str, sizeof(fmt_str));
    if (pMe->m_pImage == NULL)
    {
        //IDISPLAY_FillRect(pMe->m_pDisplay, &pMe->m_rc, SENDINGSMS_ANI_G);
        return;
    }
    
    imageIndex++;
    if(imageIndex >= SENDINGSMS_ANI_N)
    {
        imageIndex = 0;
    }
    
    IIMAGE_SetParm(pMe->m_pImage, IPARM_NFRAMES, SENDINGSMS_ANI_N, 0);
    IIMAGE_GetInfo(pMe->m_pImage, &info);
    #ifdef FEATURE_VERSION_FLEXI203P
    SETAEERECT(&rect,  (pMe->m_rc.dx - info.cxFrame)/2, SENDINGSMS_ANI_Y, info.cx, info.cy+8);
    #else
    SETAEERECT(&rect,  (pMe->m_rc.dx - info.cxFrame)/2, SENDINGSMS_ANI_Y, info.cx, info.cy);
    #endif
    
#ifdef FEATURE_CARRIER_CHINA_VERTU
    {
        IImage *pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
        
        Appscommon_ResetBackground(pMe->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &rect, 0, 0);
        if(pImageBg != NULL)
        {
            IImage_Release(pImageBg);
        }
    }
#else
    Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &rect, TRUE);
#endif
    IIMAGE_DrawFrame(pMe->m_pImage, imageIndex, rect.x, rect.y);
    //x = (pMe->m_rc.dx - info.cx/SENDINGSMS_ANI_N)/2;
    //y = SENDINGSMS_ANI_Y;
    #ifdef FEATURE_SUPPORT_ID
    MEMSET(wszPrsend,0,sizeof(wszPrsend));
     WSPRINTF(wszPrsend,
                sizeof(wszPrsend),
                fmt_str,
                pMe->m_idxCurSend);
	WSTRCAT(wszPrsend,L"/");
	WSPRINTF(wszPrsend+2,
                sizeof(wszPrsend),
                fmt_str,
                pMe->m_nSendItems);
	oldColor = IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, SENDINGSMS_TEXT_COLOR);
	#ifdef FEATURE_VERSION_FLEXI203P
    IDISPLAY_DrawText( pMe->m_pDisplay, 
                                AEE_FONT_NORMAL, wszPrsend,
                                -1, 0, (rect.dy-8), NULL, 
                                IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER);
	#else
	IDISPLAY_DrawText( pMe->m_pDisplay, 
                                AEE_FONT_NORMAL, wszPrsend,
                                -1, 0, (rect.dy), NULL, 
                                IDF_TEXT_TRANSPARENT|IDF_ALIGN_CENTER);
	#endif
	IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, oldColor);
     //MSG_FATAL("EVT_USER_REDRAW...::::::::::::::::::::::::: WMS:%d:::%d",pMe->m_idxCurSend,pMe->m_nSendItems,0);
	 #endif
    (void) ISHELL_SetTimer(pMe->m_pShell,
                                            SENDINGSMS_ANI_R,
                                            (PFNNOTIFY)WmsApp_PlaySendingAni,
                                            (void *)pMe);
    //IIMAGE_SetAnimationRate(pMe->m_pImage, SENDINGSMS_ANI_R);
    //IIMAGE_SetParm(pMe->m_pImage, IPARM_NFRAMES, SENDINGSMS_ANI_N, 0);
    
    //SETAEERECT(&rect,  0, 0, pMe->m_rc.dx, y+info.cy);
    //IDISPLAY_FillRect(pMe->m_pDisplay, &rect, SENDINGSMS_ANI_G);
    
    //IIMAGE_Start(pMe->m_pImage, x, y);
    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
}
#if 0
/*==============================================================================
函数:
    WmsApp_StopSendingAni

说明:
    函数终止短信提示动画。

参数:
    pMe [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
    无。

备注:

==============================================================================*/
static void WmsApp_StopSendingAni(WmsApp *pMe)
{
    if (pMe->m_pImage != NULL)
    {
        IIMAGE_Stop(pMe->m_pImage);
    }
}
#endif
/*==============================================================================
函数:
    IDD_MANAGEMENT_Handler

说明:
    WMS Applet对话框 IDD_MANAGEMENT 事件处理函数。显示关于短消息处理界面给用户作选择。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_MANAGEMENT_Handler(void   *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;

    if (NULL == pMe)
    {
        return FALSE;
    }

    pMenu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_MANAGEMENT_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            
            // 菜单项初始化
            MENU_ADDITEM(pMenu, IDS_DELETEMSGS);
            if (IsRunAsUIMVersion())
            {
                MENU_ADDITEM(pMenu, IDS_COPYINBOX);
                MENU_ADDITEM(pMenu, IDS_MOVEINBOX);
#ifdef FEATURE_CDSMS_RUIM
                MENU_ADDITEM(pMenu, IDS_STORAGE);
#endif       

            }
            MENU_ADDITEM(pMenu, IDS_MENSTATUS);
#ifdef FEATURE_USES_MMS
            MENU_ADDITEM(pMenu, IDS_MMS_MENSTATUS);
#endif
            
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
#endif
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);            
            IMENUCTL_SetSel(pMenu, pMe->m_wPrevMenuSel);
            {
                AECHAR WTitle[40] = {0};
                (void)ISHELL_LoadResString(pMe->m_pShell,
                AEE_WMSAPPRES_LANGFILE,                                
                IDS_MSGMANAGEMENT,
                WTitle,
                sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
            }              
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);

            return TRUE;

        case EVT_USER_REDRAW:
            //(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
            return TRUE;

        case EVT_CTL_SEL_CHANGED:
            pMe->m_wPrevMenuSel = wParam;
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            pMe->m_wPrevMenuSel = wParam;
            switch (wParam)
            {
                // 删除短信
                case IDS_DELETEMSGS:
                    CLOSE_DIALOG(DLGRET_DELETEMSGS)
                    return TRUE;

                // 拷贝收件箱信息
                case IDS_COPYINBOX:
                    CLOSE_DIALOG(DLGRET_COPYINBOX)
                    return TRUE;

                // 移动收件箱信息
                case IDS_MOVEINBOX:
                    CLOSE_DIALOG(DLGRET_MOVEINBOX)
                    return TRUE; 

#ifdef FEATURE_CDSMS_RUIM
                // 存储设备使用设置
                case IDS_STORAGE:
                    CLOSE_DIALOG(DLGRET_STORAGE)
                    return TRUE;
#endif                     

                case IDS_MENSTATUS:
                    CLOSE_DIALOG(DLGRET_MEMSTATUS)
                    return TRUE;

#ifdef FEATURE_USES_MMS
                case IDS_MMS_MENSTATUS:
                    CLOSE_DIALOG(DLGRET_MMS_MEMSTATUS)
                    return TRUE;
#endif                    
                                
                default:
                    break;
            }
            
            return FALSE;

        default:
            break;
    }

    return FALSE;
} // IDD_MANAGEMENT_Handler


/*==============================================================================
函数:
    IDD_COPYINBOX_Handler

说明:
    WMS Applet对话框 IDD_MANAGEMENT 事件处理函数。显示关于短消息处理界面给用户作选择。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_COPYINBOX_Handler(void   *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;

    if (NULL == pMe)
    {
        return FALSE;
    }

    pMenu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_COPYINBOX_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            
            // 菜单项初始化
            MENU_ADDITEM(pMenu, IDS_UIMTOPHONE);
            MENU_ADDITEM(pMenu, IDS_PHONETOUIM);
            
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
#endif
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);            
            IMENUCTL_SetSel(pMenu, pMe->m_wPrevMenuSel);
            {
                AECHAR WTitle[40] = {0};
                (void)ISHELL_LoadResString(pMe->m_pShell,
                AEE_WMSAPPRES_LANGFILE,                                
                IDS_COPYINBOX,
                WTitle,
                sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
            }       
            
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);

            return TRUE;

        case EVT_USER_REDRAW:
            //(void)IMENUCTL_Redraw(pMenu);   //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            pMe->m_wPrevMenuSel = wParam;
            switch (wParam)
            {
                // UIM卡到手机
                case IDS_UIMTOPHONE:
                    pMe->m_CopyType = COPYALLRUIMTOPHONE;
                    CLOSE_DIALOG(DLGRET_COPYRUIMMSGTOINBOX)

                    return TRUE;

                // 手机到UIM卡
                case IDS_PHONETOUIM:
                    pMe->m_CopyType = COPYALLMTTORUIM;
                    CLOSE_DIALOG(DLGRET_COPYINBOXMSGTORUIM)
                    return TRUE;

                    return TRUE;
                                
                default:
                    break;
            }
            
            return FALSE;

        default:
            break;
    }

    return FALSE;
}//IDD_COPYINBOX


/*==============================================================================
函数:
    IDD_MOVEINBOX_Handler

说明:
    WMS Applet对话框 IDD_MANAGEMENT 事件处理函数。显示关于短消息处理界面给用户作选择。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_MOVEINBOX_Handler(void   *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;

    if (NULL == pMe)
    {
        return FALSE;
    }

    pMenu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_MOVEINBOX_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            
            // 菜单项初始化
            MENU_ADDITEM(pMenu, IDS_PHONETOUIM);
            MENU_ADDITEM(pMenu, IDS_UIMTOPHONE);
            
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MESSAGE_BACKGROUND);
#endif
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);            
            IMENUCTL_SetSel(pMenu, pMe->m_wPrevMenuSel);
            {
                AECHAR WTitle[40] = {0};
                (void)ISHELL_LoadResString(pMe->m_pShell,
                AEE_WMSAPPRES_LANGFILE,                                
                IDS_MOVEINBOX,
                WTitle,
                sizeof(WTitle));
                if(pMe->m_pIAnn != NULL)
                {
                    IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
                }
            }               
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);

            return TRUE;

        case EVT_USER_REDRAW:
            //(void)IMENUCTL_Redraw(pMenu);   //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
  
                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            pMe->m_wPrevMenuSel = wParam;
            switch (wParam)
            {
                // UIM卡到手机
                case IDS_UIMTOPHONE:
                    pMe->m_CopyType = MOVEALLRUIMTOPHONE;
                    CLOSE_DIALOG(DLGRET_MOVERUIMMSGTOINBOX)
                    return TRUE;

                // 手机到UIM卡
                case IDS_PHONETOUIM:
                    pMe->m_CopyType = MOVEALLMTTORUIM;
                    CLOSE_DIALOG(DLGRET_MOVEINBOXMSGTORUIM)
                    return TRUE;
                                
                default:
                    break;
            }
            
            return FALSE;

        default:
            break;
    }

    return FALSE;
}//IDD_MOVEINBOX

/*==============================================================================
函数:
    IDD_SELECTFROMOPT_Handler

说明:
    WMS Applet对话框 IDD_SELECTFROMOPT 事件处理函数。显示关于短消息处理界面给用户作选择。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_SELECTFROMOPT_Handler(void   *pUser,
    AEEEvent eCode,
    uint16   wParam,
    uint32   dwParam
)
{
    IMenuCtl *pMenu = NULL;
    WmsApp *pMe = (WmsApp *)pUser;

    if (NULL == pMe)
    {
        return FALSE;
    }

    pMenu = (IMenuCtl* )IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_SELECTFROM_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:

            // 菜单项初始化
            MENU_ADDITEM(pMenu, IDS_CONTACTS);
            MENU_ADDITEM(pMenu, IDS_GROUPLIST);
            #if 0
            MENU_ADDITEM(pMenu, IDS_RECENT_CALLS);
            #endif
            
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetPopMenuRect(pMenu);
            MENU_SETBOTTOMBAR(pMenu, BTBAR_SELECT_BACK);            
            IMENUCTL_SetSel(pMenu, IMENUCTL_GetItemID(pMenu, 0));
            
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);

            return TRUE;

        case EVT_USER_REDRAW:
           // (void)IMENUCTL_Redraw(pMenu);   //dele by yangdecai
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
            return TRUE;

        case EVT_COMMAND:
            //pMe->m_wPrevMenuSel = wParam;
            pMe->m_currState = WMSST_TONUMLIST;
            switch (wParam)
            {
                // 电话本列表
                case IDS_CONTACTS:
                    if(pMe->m_eMakeAddListMode == MAKEADDLIST_NONE)
                    {
                        WMSExtApp_GetAddresseeFromContactsApp(pMe, GETADDR_ONEADD);
                    }
                    else
                    {
                        WMSExtApp_GetAddresseeFromContactsApp(pMe, GETADDR_MULTIADD);
                    }
                    return TRUE;

                // 群组
                case IDS_GROUPLIST:
                    // GETADDR_GROUPLIST_SINGLE
                    if(pMe->m_eMakeAddListMode == MAKEADDLIST_NONE)
                    {
                        WMSExtApp_GetAddresseeFromContactsApp(pMe, GETADDR_GROUPLIST_ONEADD);
                    }
                    else
                    {
                        WMSExtApp_GetAddresseeFromContactsApp(pMe, GETADDR_GROUPlIST);
                    }
                    return TRUE;
                                
                 // 通话记录
                case IDS_RECENT_CALLS:
                    if(pMe->m_eMakeAddListMode == MAKEADDLIST_NONE)
                    {
                        WMSExtApp_GetNumFromRecentCalls(pMe, GETADDR_ONEADD); // 单选
                    }
                    else
                    {
                        WMSExtApp_GetNumFromRecentCalls(pMe, GETADDR_MULTIADD); // 多选
                    }

                    return TRUE;

                default:
                    break;
            }
            
            return FALSE;

        default:
            break;
    }

    return FALSE;
}//IDD_SELECTFROMOPT

//add by yangdecai
/*==============================================================================
函数:
    IDD_RESEND_CONFIRM_Handler

说明:
    WMS Applet对话框  IDD_RESEND_CONFIRM 事件处理函数。

参数:
    pMe [in]:       指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]:     事件代码。
    wParam:         事件参数
    dwParam [in]:   与wParam关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean IDD_RESEND_CONFIRM_Handler(void	 *pUser,
		AEEEvent eCode,
		uint16	 wParam,
		uint32	 dwParam
)
{
	WmsApp *pMe = (WmsApp *)pUser;
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    if (NULL == pMe->m_pStatic)
    {
        return FALSE;
    }
	
	switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;

        case EVT_USER_REDRAW:
            {
                AECHAR  wstrText[62];
                PromptMsg_Param_type  Msg_Param={0};
                (void) ISHELL_LoadResString(pMe->m_pShell,
                                AEE_WMSAPPRES_LANGFILE,
                                pMe->m_wMsgResID,
                                wstrText,
                                sizeof(wstrText));
  
                // 调用公共函数绘制界面
                Msg_Param.ePMsgType = MESSAGE_CONFIRM;
                Msg_Param.pwszMsg = wstrText;
#ifdef FEATURE_CARRIER_THAILAND_HUTCH                  
                Msg_Param.eBBarType = BTBAR_OK_BACK;
#else
                Msg_Param.eBBarType = BTBAR_OK_CANCEL;
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH		
                DrawPromptMessage(pMe->m_pDisplay, pMe->m_pStatic, &Msg_Param);
            }
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            
            return TRUE;

        case EVT_DIALOG_END:
            ISTATIC_Release(pMe->m_pStatic);
            pMe->m_pStatic = NULL;
            return TRUE;
  
        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                case AVK_SELECT:
                    {
						CLOSE_DIALOG(DLGRET_OK)
                    	//PFNNOTIFY pfn = WmsApp_ReSendMsgTimer;
						//(void)ISHELL_SetTimer(pMe->m_pShell, 300, pfn, pMe);
                    }
                    return TRUE;
  
                default:
                    break;
            }
            break;

        default:
            break;
    }

    return FALSE;
}//IDD_RESEND_CONFIRM
/*==============================================================================
函数:
    IDD_WMSNEWMSG_Handler

说明:
    CoreApp 对话框 IDD_WMSTIPS 事件处理函数。

参数:
    pMe [in]:       指向Core Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]:     事件代码。
    wParam:         事件参数
    dwParam [in]:   与wParam关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/
static boolean	IDD_WMSNEWMSG_Handler(void *pUser,
											 AEEEvent	eCode,
											 uint16 	wParam,
											 uint32 	dwParam)
{
	static IStatic * pStatic = NULL;
    static IBitmap * pDevBmp = NULL;
    WmsApp *pMe = (WmsApp *)pUser;
    MSG_FATAL("%x %x %x IDD_WMSNEWMSG_Handler",eCode,wParam,dwParam);
	if (NULL == pMe)
    {
        return FALSE;
    }
    
    if (NULL == pStatic)
    {
        AEERect rc = {0};

        if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_STATIC,
                (void**)&pStatic) == SUCCESS)
        {
            ISTATIC_SetRect(pStatic, &rc);
        }
        else
        {
            return FALSE;
        }
    }
	switch (eCode)
    {
        case EVT_DIALOG_INIT:
            if(pMe->m_pIAnn != NULL)
            {
                IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,NULL);
            }
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;

        case EVT_USER_REDRAW:
            {
				AECHAR  wstrText[64]={0};
                AECHAR  wstrFMT[64]={0};
                uint16  nNewsVmail=0, nNewsSMS=0;
                PromptMsg_Param_type  Msg_Param={0};
                db_items_value_type  need_capture;
				int i = 0;
				int temp = 0;
                need_capture.b_capture = DB_CAPTURE_NONE;
                db_put(DB_CAPTURE_WALLPER,&need_capture);
                // 获取消息数
                wms_cacheinfolist_getcounts(WMS_MB_VOICEMAIL, &nNewsVmail, NULL, NULL);
                wms_cacheinfolist_getcounts(WMS_MB_INBOX, &nNewsSMS, NULL, NULL);
				if (gbWmsVMailNtf && !(gbWmsLastNtfIsSMS && gbWmsSMSNtf))
                {
                    // 从资源文件取消息内容
                    (void)ISHELL_LoadResString(pMe->m_pShell,
                                    AEE_WMSAPPRES_LANGFILE,                                
                                    IDS_VMAILTIPS,
                                    wstrFMT,
                                    sizeof(wstrFMT));
                                    
                    WSPRINTF(wstrText, sizeof(wstrText), wstrFMT, nNewsVmail);
                }
                else
                {
                    // 从资源文件取消息内容
                    if(nNewsSMS == 0)
                    {
                    	nNewsSMS = 1;
                    }
                    (void)ISHELL_LoadResString(pMe->m_pShell,
                                    AEE_WMSAPPRES_LANGFILE,                                
                                    IDS_NEWWMSTIPS,
                                    wstrFMT,
                                    sizeof(wstrFMT));
                                    
                    WSPRINTF(wstrText, sizeof(wstrText), wstrFMT, nNewsSMS);
                }
  
                Msg_Param.ePMsgType = MESSAGE_INFORMATION;
                Msg_Param.pwszMsg = wstrText;
                Msg_Param.eBBarType = BTBAR_READ_BACK;

                if(pDevBmp != NULL)
                {
                    IDISPLAY_BitBlt(pMe->m_pDisplay, 0, 0, pMe->m_rc.dx, pMe->m_rc.dy, 
                            pDevBmp, 0, 0, AEE_RO_COPY);
                }
                DrawPromptMessage(pMe->m_pDisplay, pStatic, &Msg_Param);
				IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            	return TRUE;
            }
			break;
		case EVT_DIALOG_END:
			{
				if(pMe->m_bActive)
	            {
	                if(pDevBmp != NULL)
	                {
	                    IBITMAP_Release(pDevBmp);
	                    pDevBmp = NULL;
	                }
	            }
	            else if(NULL == pDevBmp)
	            {
	                IBitmap *pTempBmp = NULL;

	                IDISPLAY_GetDeviceBitmap(pMe->m_pDisplay, &pTempBmp);
	                IBITMAP_CreateCompatibleBitmap(pTempBmp, &pDevBmp, pMe->m_rc.dx, pMe->m_rc.dy);
	                if(NULL != pTempBmp && NULL != pDevBmp)
	                {
	                    IBITMAP_BltIn(pDevBmp, 0, 0, pMe->m_rc.dx, pMe->m_rc.dy, pTempBmp, 0, 0, AEE_RO_COPY);
	                }
	            }
	            ISTATIC_Release(pStatic);
	            pStatic = NULL;
	            return TRUE;
			}
			break;
		case EVT_KEY_PRESS:
			{
				switch(wParam)
            	{
					case AVK_CLR:
		            case AVK_END:
						{
							CLOSE_DIALOG(DLGGET_SMSNEW_OK)
                    		return TRUE;
						}
						break;
					case AVK_SELECT:
						{
							CLOSE_DIALOG(DLGRET_SMSVIEWS)
                    		return TRUE;
						}
  						break;
                	default:
                    	break;
				}
			}
			break;
		default:
            break;
		}
	return FALSE;
}
/*==============================================================================
函数:
    IDD_FLASHSMS_Handler

说明:
    WMSApp 对话框 IDD_FLASHSMS 事件处理函数。

参数:
    pMe [in]:       指向WMSApp Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]:     事件代码。
    wParam:         事件参数
    dwParam [in]:   与wParam关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/

static boolean IDD_FLASHSMS_Handler(void *pUser,
									 AEEEvent eCode,
									 uint16 wParam,
									 uint32 dwParam)
{
	static IStatic * pStatic = NULL;
    //static IBitmap * pDevBmp = NULL;
    WmsApp *pMe = (WmsApp *)pUser;
    MSG_FATAL("%x %x %x IDD_WMSTIPS_Handler",eCode,wParam,dwParam);
    pStatic = (IStatic*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_STATIC_FLASHSMS);
	if (NULL == pMe)
    {
        return FALSE;
    }
    
    if (NULL == pStatic)
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
				SETAEERECT(&rc,  0, pMe->m_rc.y ,
                            pMe->m_rc.dx,
                            pMe->m_rc.dy - BOTTOMBAR_HEIGHT);
                ISTATIC_SetRect(pStatic, &rc);
            }
            IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
            // 设置静态文本控件属性
            ISTATIC_SetProperties(pStatic, ST_CENTERTITLE | ST_NOSCROLL | ST_DISPLATSMS | ST_GRAPHIC_BG);
            return TRUE;
		case EVT_WMS_MSG_READ:
            {
            wms_cache_info_node  *pnode = NULL;
            wms_memory_store_e_type mem_store;
            wms_message_index_type  wIdx;
            int nRet;
            wms_cache_info_list   *pList = NULL;
            wms_msg_event_info_s_type *Info = (wms_msg_event_info_s_type*)dwParam;
            pMe->m_eMBoxType = WMS_MB_INBOX;
            pList = wms_get_cacheinfolist(pMe->m_eMBoxType);
            if (NULL != pList)
            {
                
                pMe->m_wCurindex = pList->nBranches;
                
            }
            MSG_FATAL("EVT_WMS_MSG_READ....IDD_FLASHSMS_Handler.........receve",0,0,0);
            mem_store = Info->status_info.message.msg_hdr.mem_store;
            wIdx = Info->status_info.message.msg_hdr.index;
            MSG_FATAL("pMe->m_idxCur===========%d,pMe->m_wCurindex=%d",pMe->m_idxCur,pMe->m_wCurindex,0);
            pnode = wms_cacheinfolist_getnode(pMe->m_eMBoxType, WMS_MEMORY_STORE_NV_CDMA, (pMe->m_wCurindex));//pMe->m_CurMsgNodes[pMe->m_idxCur];
            
            if (NULL == pnode)
            {
            	MSG_FATAL("read SUCCESS................................2",0,0,0);
            	(void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
                return TRUE;
            }
             MSG_FATAL("read SUCCESS................................3",0,0,0);
            if ((mem_store == pnode->mem_store) && (pnode->index == wIdx))
            {
                WMSMessageStruct *pTms = (WMSMessageStruct *)MALLOC(sizeof(WMSMessageStruct));
                DBGPRINTF("WMS Address = 0x%x", pTms);   
                // 保存读取的数据
                if (NULL != pTms)
                {
                    WmsApp_ConvertClientMsgToMS(&(Info->status_info.message), pTms);
                    pMe->m_CurMsgNodesMS[pMe->m_idxCur] = pTms;
                }
                
                
                pnode = NULL;
                pMe->m_idxCur++;
                for (; pMe->m_idxCur<LONGSMS_MAX_PACKAGES; pMe->m_idxCur++)
                {
                	
                    if (pMe->m_CurMsgNodes[pMe->m_idxCur] != NULL)
                    {
                        pnode = pMe->m_CurMsgNodes[pMe->m_idxCur];
                        break;
                    }
                }
                 MSG_FATAL("read SUCCESS................................4",0,0,0);
                if (NULL != pnode)
                {// 读下一数据包
                    // 发送读消息命令
                    
                    nRet = IWMS_MsgRead(pMe->m_pwms,
                                       pMe->m_clientId,
                                       &pMe->m_callback,
                                       (void *)pMe,
                                       pnode->mem_store,
                                       pnode->index);
                                       
                    if (nRet != SUCCESS)
                    {
                        CLOSE_DIALOG(DLGRET_LOADFAILED)
                    }
                                       
                    return TRUE;
                }
                else
                {
                    MSG_FATAL("WmsApp_CombinateMsg sssssss",0,0,0);
                    WmsApp_CombinateMsg(pMe);
                    MSG_FATAL("read SUCCESS................................5",0,0,0);
                    (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
                }
            }
            return TRUE;
            }
            break;
        case EVT_DIALOG_START:
          {
        	 uint16 wIndex=0;
             wms_cache_info_node  *pnode = NULL;
             int nRet,i,nCount=0;
             boolean bUIMSMS = FALSE;
             wms_cache_info_list   *pList = NULL;
            
             pMe->m_eMBoxType = WMS_MB_INBOX;
             pList = wms_get_cacheinfolist(pMe->m_eMBoxType);
             if (NULL != pList)
             {
                pMe->m_wCurindex = pList->nBranches;
             }
        	 wIndex = pMe->m_wCurindex;
             MSG_FATAL("EVT_DIALOG_INIT...wIndex=%d,pMe->m_eMBoxType=%d",wIndex,pMe->m_eMBoxType,0);
             // 取消息 cache info 节点
             if (wIndex>=RUIM_MSGINDEX_BASE)
             {
                 wIndex = wIndex - RUIM_MSGINDEX_BASE;
                 pnode = wms_cacheinfolist_getnode(pMe->m_eMBoxType, WMS_MEMORY_STORE_RUIM, wIndex);
             }
             else
             {
                 pnode = wms_cacheinfolist_getnode(pMe->m_eMBoxType, WMS_MEMORY_STORE_NV_CDMA, wIndex);
             }
             MSG_FATAL("EVT_USER_REDRAW,......,EVT_DIALOG_INIT...1...",0,0,0);
             if (NULL == pnode)
             {
             	MSG_FATAL("EVT_USER_REDRAW,......,EVT_DIALOG_INIT...2...",0,0,0);
                 CLOSE_DIALOG(DLGRET_LOADFAILED)
                 return TRUE;
             }
             MSG_FATAL("EVT_USER_REDRAW,......,EVT_DIALOG_INIT...3...",0,0,0);
             // 重置当前消息列表
             MEMSET(pMe->m_CurMsgNodes, 0, sizeof(pMe->m_CurMsgNodes));
             WmsApp_FreeMsgNodeMs(pMe);
             
             pMe->m_idxCur = 0;
 #ifdef FEATURE_SMS_UDH
             if (pnode->pItems != NULL)
             {
                 MEMCPY(pMe->m_CurMsgNodes, pnode->pItems, sizeof(pMe->m_CurMsgNodes));
                 MSG_FATAL("pnode->pItems is  NULL",0,0,0);
                 for (; pMe->m_idxCur<LONGSMS_MAX_PACKAGES; pMe->m_idxCur++)
                 {
                     if (pMe->m_CurMsgNodes[pMe->m_idxCur] != NULL)
                     {
                         pnode = pMe->m_CurMsgNodes[pMe->m_idxCur];
                         break;
                     }
                 }
             }
             else
 #endif
             {
             	MSG_FATAL("pnode->pItems is not NULL",0,0,0);
                 pMe->m_CurMsgNodes[0] = pnode;
             }
            
             pMe->m_eCurStore = pnode->mem_store;
             MSG_FATAL("EVT_USER_REDRAW,......,EVT_DIALOG_INIT...4...",0,0,0);
             // 发送读消息命令
             nRet = IWMS_MsgRead(pMe->m_pwms,
                                pMe->m_clientId,
                                &pMe->m_callback,
                                (void *)pMe,
                                pnode->mem_store,
                                pnode->index);
             MSG_FATAL("READ msg.........................",0,0,0);
            return TRUE;
		}
        case EVT_USER_REDRAW:
            {
            	
                AECHAR *pFormatedText = NULL;
                AECHAR wszTitle[32] = {0};
                uint16 nTitleID=0;
                TitleBar_Param_type     TBarParam = {0};
                nTitleID = IDS_STR_FLASHSMS;
                MSG_FATAL("IDD_FLASHSMS_Handler EVT_USER_REDRAW...",0,0,0);
                if (nTitleID != 0)
                {
                    (void) ISHELL_LoadResString(pMe->m_pShell,
                            AEE_WMSAPPRES_LANGFILE,
                            nTitleID,
                            wszTitle,
                            sizeof(wszTitle));
                }

                TBarParam.pwszTitle = wszTitle;
				#if 0
                TBarParam.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
                DrawTitleBar(pMe->m_pDisplay, &TBarParam);
				#else
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,wszTitle);
				#endif

                MEMSET(wszTitle, 0, sizeof(wszTitle));

                // 格式化消息便于阅读
                if ((pMe->m_currState == WMSST_VIEWTEMPLATE) &&
                    (pMe->m_msSend.m_szMessage != NULL))
                {
                	MSG_FATAL("pFormatedText........................0000000",0,0,0);
                    pFormatedText = FormatFlashSMSForDisplay(pMe, &pMe->m_msSend);
                }
                else
                {
                	MSG_FATAL("pFormatedText........................00000001",0,0,0);
                    pFormatedText = FormatFlashSMSForDisplay(pMe, &pMe->m_msCur);
                }
                MSG_FATAL("pFormatedText........................11111",0,0,0);
                if (NULL != pFormatedText)
                {
                    // 设置静态控件文本
                    MSG_FATAL("pFormatedText........................IS NOT NULL",0,0,0);
                    (void)ISTATIC_SetText(pStatic,
                            WSTRLEN(wszTitle)>0 ? wszTitle : NULL,
                            pFormatedText,
                            AEE_FONT_BOLD,
                            AEE_FONT_NORMAL);
                    
                    // 释放格式化消息时动态分配的空间
                    FREE(pFormatedText);
            	}
            
	            ISTATIC_SetActive(pStatic, TRUE);
	            (void) ISTATIC_Redraw(pStatic);
	            
	            // 绘制底条提示
	            // Option       Back
	            DRAW_BOTTOMBAR(BTBAR_SAVE_BACK)
        	 	WmsApp_UpdateAnnunciators(pMe);
	            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
			}
			break;
		case EVT_DIALOG_END:
			{
				
	            ISTATIC_Release(pStatic);
	            pStatic = NULL;
	            return TRUE;
			}
			break;
		case EVT_WMS_MSG_DELETE:
        case EVT_WMS_MSG_DELETE_BOX:
        	{
        		MSG_FATAL("EVT_WMS_MSG_DELETE..................",0,0,0);
        		CLOSE_DIALOG(DLGGET_FLASHSMS_END)
        		return TRUE;
        	}
        	break;
		case EVT_KEY:
			{
				switch(wParam)
            	{
					case AVK_CLR:
		            case AVK_END:
						{
							int i;
							int nRet=EFAILED;
                            wms_cache_info_node  *pnode = NULL;
                            wms_cache_info_list   *pList = NULL;
    			            pMe->m_eMBoxType = WMS_MB_INBOX;
    			            pList = wms_get_cacheinfolist(pMe->m_eMBoxType);
    			            if (NULL != pList)
    			            {
    			                pMe->m_wCurindex = pList->nBranches;
    			            }
                            pnode = wms_cacheinfolist_getnode(pMe->m_eMBoxType, WMS_MEMORY_STORE_NV_CDMA, (pMe->m_wCurindex));
                             #ifdef FEATURE_SMS_UDH
  			             	if (pnode->pItems != NULL)
  			             	{
  			                 	MEMCPY(pMe->m_CurMsgNodes, pnode->pItems, sizeof(pMe->m_CurMsgNodes));
  			                 	MSG_FATAL("pnode->pItems is  NULL",0,0,0);
  			                 	for (; pMe->m_idxCur<LONGSMS_MAX_PACKAGES; pMe->m_idxCur++)
  			                 	{
  			                     	if (pMe->m_CurMsgNodes[pMe->m_idxCur] != NULL)
  			                     	{
  			                         	pnode = pMe->m_CurMsgNodes[pMe->m_idxCur];
  			                         	break;
  			                     	}
  			                 	}
  			             	}
  			             	else
  			 				#endif
  			             	{
  			             		MSG_FATAL("pnode->pItems is not NULL",0,0,0);
  			                 	pMe->m_CurMsgNodes[0] = pnode;
             				}
                            for (i=0; i<LONGSMS_MAX_PACKAGES; i++)
                            {
                                if (pMe->m_CurMsgNodes[i] != NULL)
                                {
                                    pnode = pMe->m_CurMsgNodes[i];
                                    
                                    // 发布删除消息命令
                                    nRet = ENOMEMORY;
                                    do
                                    {
                                        nRet = IWMS_MsgDelete(pMe->m_pwms,
                                                           pMe->m_clientId,
                                                           &pMe->m_callback,
                                                           (void *)pMe,
                                                           pnode->mem_store,
                                                           pnode->index);
                                         MSG_FATAL("DELEING...................",0,0,0);
                                    } while(nRet != SUCCESS);
                                    pMe->m_CurMsgNodes[i] = NULL;
                                }
                            }
							
                    		return TRUE;
						}
						break;
					case AVK_SELECT:
						{
							uint16 wIndex=0;
 			             	wms_cache_info_node  *pnode = NULL;
 			             	int nRet,i,nCount=0;
 			             	wms_cache_info_list   *pList = NULL;
				            pMe->m_eMBoxType = WMS_MB_INBOX;
				            pList = wms_get_cacheinfolist(pMe->m_eMBoxType);
				            if (NULL != pList)
				            {
				               pMe->m_wCurindex = pList->nBranches;
				            }
				        	wIndex = pMe->m_wCurindex;
				        	pnode = wms_cacheinfolist_getnode(pMe->m_eMBoxType, WMS_MEMORY_STORE_NV_CDMA, (pMe->m_wCurindex));
 			             	nRet = IWMS_MsgModifyTag(pMe->m_pwms,
 			                                         pMe->m_clientId,
 			                                         &pMe->m_callback,
 			                                         (void *)pMe,
 			                                         pnode->mem_store,
 			                                         pnode->index,
 			                                         WMS_TAG_MT_READ); 
							CLOSE_DIALOG(DLGGET_FLASHSMS_END)
        					return TRUE;
						}
						break;
                	default:
                    	break;
				}
			}
			break;
		default:
            break;
		}
	return FALSE;
}

/*==============================================================================
函数:
    IDD_POPMSG_Handler

说明:
    CoreApp 对话框 IDD_POPMSG 事件处理函数。

参数:
    pMe [in]:       指向Core Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]:     事件代码。
    wParam:         事件参数
    dwParam [in]:   与wParam关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/

static boolean IDD_POPMSG_Handler(void *pUser,AEEEvent eCode,uint16 wParam,uint32 dwParam)
{
	static IStatic * pStatic = NULL;       
    WmsApp *pMe = (WmsApp *)pUser;
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    MSG_FATAL("IDD_POPMSG_Handler Start",0,0,0); 
    if (NULL == pStatic)
    {
         AEERect rect = {0};
         if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                                  AEECLSID_STATIC,
                                                  (void **)&pStatic))
         
         {
             return FALSE;
             MSG_FATAL("ISHELL_CreateInstance,AEECLSID_STATIC 2",0,0,0);
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
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0,
                                    0); 
            return TRUE;                                                                       
            
        case EVT_USER_REDRAW:
            {
                PromptMsg_Param_type PromptMsg={0};  
                AECHAR  wstrText[128];  
                
                (void)ISHELL_LoadResString(pMe->m_pShell,
                                        AEE_WMSAPPRES_LANGFILE,
                                        IDS_NO_CARD,
                                        wstrText,
                                        sizeof(wstrText)); 
                PromptMsg.ePMsgType = MESSAGE_INFORMATIVE;
                PromptMsg.pwszMsg = wstrText;
                DrawPromptMessage(pMe->m_pDisplay, pStatic, &PromptMsg);                         
            }   
			(void) ISHELL_SetTimer( pMe->m_pShell,
                            1000,
                            WMSApp_DialogTimeout,
                            pMe );  
            IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
            return TRUE;
            
        case EVT_DIALOG_END:
             ISTATIC_Release(pStatic); 
             pStatic = NULL;   
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_INFO:
                case AVK_CLR:
                case AVK_SELECT:
                    MSG_FATAL("DLGRET_MSGBOX_OK",0,0,0);
                    CLOSE_DIALOG(DLGRET_MSGBOX_OK);
                    return TRUE;
                    
                default:
                    break;                    
            }
            return TRUE;
			
		case EVT_USER:
			MSG_FATAL("EVT_USER::::::::::::::::1111111111111111111111111111111111111111111",0,0,0);
            MSG_FATAL("DLGRET_MSGBOX_OK",0,0,0);
			CLOSE_DIALOG(DLGRET_MSGBOX_OK);
			break;
        
        case EVT_COMMAND:
            break;
                     
            
        default:
            break;            
    }
    
    return FALSE;
}

/*==============================================================================
函数: 
       WMSApp_DialogTimeout
       
说明: 
       定时器回调函数。主要用于自动关闭消息对话框。
       
参数: 
       pme [in]：void *类型指针，暗指CWMS结构指针。
       
返回值:
       无。
       
备注:
       
==============================================================================*/
static void WMSApp_DialogTimeout(void *pme)
{
    WmsApp *pMe = (WmsApp *)pme;
    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent( pMe->m_pShell,
                            AEECLSID_WMSAPP,
                            EVT_USER,
                            0,
                            0);
}
static void WmsApp_ReadMsg(void *pUser)
{
	WmsApp *pMe = (WmsApp *)pUser;
	// 读取消息
    uint16 wIndex=0;
    wms_cache_info_node  *pnode = NULL;
    int nRet,i,nCount=0;
    boolean bUIMSMS = FALSE;
    
    wIndex = pMe->m_wCurindex;
   
    // 取消息 cache info 节点
    if (wIndex>=RUIM_MSGINDEX_BASE)
    {
        wIndex = wIndex - RUIM_MSGINDEX_BASE;
        pnode = wms_cacheinfolist_getnode(pMe->m_eMBoxType, WMS_MEMORY_STORE_RUIM, wIndex);
    }
    else
    {
        pnode = wms_cacheinfolist_getnode(pMe->m_eMBoxType, WMS_MEMORY_STORE_NV_CDMA, wIndex);
    }
    
    if (NULL == pnode)
    {
    	
        return ;
    }
    
    // 重置当前消息列表
    MEMSET(pMe->m_CurMsgNodes, 0, sizeof(pMe->m_CurMsgNodes));
    WmsApp_FreeMsgNodeMs(pMe);
    
    pMe->m_idxCur = 0;
#ifdef FEATURE_SMS_UDH
    if (pnode->pItems != NULL)
    {
        MEMCPY(pMe->m_CurMsgNodes, pnode->pItems, sizeof(pMe->m_CurMsgNodes));
        
        for (; pMe->m_idxCur<LONGSMS_MAX_PACKAGES; pMe->m_idxCur++)
        {
            if (pMe->m_CurMsgNodes[pMe->m_idxCur] != NULL)
            {
                pnode = pMe->m_CurMsgNodes[pMe->m_idxCur];
                break;
            }
        }
    }
    else
#endif
    {
        pMe->m_CurMsgNodes[0] = pnode;
    }
}
#ifdef FEATURE_USES_MMS
/*==============================================================================
函数:
    IDD_VIEWMSG_MMS_Handler

说明:
    WMS Applet 对话框 IDD_VIEWMSG_MMS 事件处理函数。(查看一条消息，并对用户
    的操作作处理）

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam: 事件参数
    dwParam [in]: 与wParam关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:
    New
    
==============================================================================*/
extern char* MMS_WSP_MineType2MormalMimeType(const char* pszSrc);
extern const char *MMS_GetMimeType(const char *pszSrc);
static boolean IDD_VIEWMSG_MMS_Handler(void *pUser, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    WmsApp *pMe = (WmsApp *)pUser;

   // IImageCtl*pImageCtl = NULL;
    IStatic * pStatic = NULL;
    IMenuCtl* pMenuCtl = NULL;
    uint32 dwMask;
    static MMS_WSP_DEC_DATA *pDecdata = NULL;
    static AECHAR *pViewText = NULL;
    static boolean hasAnimate = FALSE;
    //AnimateState == 0:当前没有对动画操作
    //AnimateState == 1:当前光标停留在动画上
    //AnimateState == 2:当前光标停留在动画上，并按下了INFO键，开始显示动画
    static uint8 AnimateState = 0;
    MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] eCode:0x%x, wParam=0x%x",eCode,wParam,0);
    if (NULL == pMe)
    {
        return FALSE;
    }

    if (pMe->m_pMenu != NULL)
    {
        if (IMENUCTL_HandleEvent(pMe->m_pMenu, eCode, wParam, dwParam))
        {
            return TRUE;
        }
    }
    
    pStatic = (IStatic*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_VIEWMSG_MMS_STATIC);
    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_VIEWMSG_MMS_MENU);
    
    if ((NULL == pStatic) || (pMenuCtl == NULL))
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            {
                IImage* pIImage = NULL;
                IImage* pISound = NULL;
                IImage* pIVideo = NULL;       
                AECHAR wszTitle[32] = {0};
                boolean hasIStatic = FALSE;
                MMSData mmsDataInfoList[MAX_MMS_STORED];
                WSP_MMS_DATA *pContent = NULL;
                MMSData* pMmsDataInfoCur = NULL;
                char* pMimeType = NULL;
                uint8 ePDUType;
                uint8 result = 0;
                uint8 index = 0;
                uint8 i = 0;
                uint8 nImageIndex = 0;
                uint8 AnimateStep = 0;
                IFile* pIFile = NULL;
                IFileMgr *pIFileMgr = NULL;
                int rSize = 0;
                void* pBuffer = NULL;
                int nTitleName = 0;
                int nMmsDataInfo = 0;
                int nInfoIndex = pMe->m_wCurindex - 1;
                CtlAddItem ai;
                AEERect rc={0};
                AEEDeviceInfo devinfo={0};
                MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] EVT_DIALOG_INIT",0 ,0 , 0);
                MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] g_mmsDataInfoMax=%d", g_mmsDataInfoMax,0,0);
                pDecdata = &pMe->m_DecData;
                dwMask = IDIALOG_GetProperties(pMe->m_pActiveIDlg);
                dwMask |= DLG_NOT_SET_FOCUS_AUTO;
                IDIALOG_SetProperties(pMe->m_pActiveIDlg, dwMask); 
                rc = pMe->m_rc;
                ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
                rc.y = GetBottomBarHeight(pMe->m_pDisplay)*2; 
                rc.dy = devinfo.cyScreen;
                rc.dy -= (GetBottomBarHeight(pMe->m_pDisplay)*3);   
                MSG_FATAL("IDD_VIEWMSG_MMS_Handler rc.x=%d, rc.y=%d,rc.dy=%d", rc.x, rc.y, rc.dy);
                //IMENUCTL_SetRect(pMenuCtl, &rc);
                IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL| OEMMP_USE_MENU_INFO_SELECT);
                IMENUCTL_SetOemProperties(pMenuCtl, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY|OEMMP_USE_MENU_STYLE);
                (void)IMENUCTL_DeleteAll(pMenuCtl); 

                // Create Instance
                result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);
                if (SUCCESS != result)
                {
                	MSG_FATAL("[IDD_VIEWMSG_MMS_Handler]: Create FileMgr Failed %x", result,0,0);
                	return FALSE;
                }
                if (SUCCESS == IFILEMGR_Test(pIFileMgr, "fs:/hsmm/MMSVideoTemp"))
                {
                    AEEFileInfo myInfo;
                    if( IFILEMGR_EnumInit(pIFileMgr, "fs:/hsmm/MMSVideoTemp", FALSE ) == SUCCESS )
                    {
                        while( IFILEMGR_EnumNext(pIFileMgr, &myInfo ) )
                        {
                            result = IFILEMGR_Remove(pIFileMgr, myInfo.szName);
                            if( result != SUCCESS )
                            {
                                DBGPRINTF( "File %s could not be removed result=%d", myInfo.szName, result );
                                //return result;
                            }
                        }
                        IFILEMGR_RmDir(pIFileMgr, "fs:/hsmm/MMSVideoTemp");
                    }
                }
                if(pMe->m_Animate != NULL)
                {
                    IIMAGE_Stop(pMe->m_Animate);
                    IIMAGE_Release(pMe->m_Animate);
                    pMe->m_Animate = NULL;
                }
                
                // Init 
                MEMSET((void*)&pDecdata->message,NULL,sizeof(MMS_WSP_DEC_MESSAGE_RECEIVED));
                pContent = &pDecdata->message.mms_data;

                
                switch(pMe->m_eMBoxType)
                {
                    case WMS_MB_OUTBOX_MMS:
                    {
                        nTitleName = IDS_OUTBOX_MMS;
                        nMmsDataInfo = CFGI_MMSOUTDATA_INFO;
                    }
                    break;
                    case WMS_MB_INBOX_MMS:
                    {
                        nTitleName = IDS_INBOX_MMS;
                        nMmsDataInfo = CFGI_MMSINDATA_INFO;
                    }
                    break;
                    case WMS_MB_DRAFTBOX_MMS:
                    {
                        nTitleName = IDS_DRAFT_MMS;
                        nMmsDataInfo = CFGI_MMSDRAFTDATA_INFO;
                    }
                    break;                    
                };
                (void) ISHELL_LoadResString(pMe->m_pShell,
                        AEE_WMSAPPRES_LANGFILE,
                        nTitleName,
                        wszTitle,
                        sizeof(wszTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,wszTitle);
                MEMSET(wszTitle, 0, sizeof(wszTitle));
                
                (void) ICONFIG_GetItem(pMe->m_pConfig,
                                   nMmsDataInfo,
                                   (void*)mmsDataInfoList,
                                   sizeof(mmsDataInfoList));                  

               

                pMmsDataInfoCur  = &mmsDataInfoList[nInfoIndex];   
                DBGPRINTF("FileName Strlen=%d", STRLEN(pMmsDataInfoCur->MMSDataFileName));
                DBGPRINTF("[IDD_VIEWMSG_MMS_Handler] nInfoIndex=%d,FileName:%s",nInfoIndex,pMmsDataInfoCur->MMSDataFileName);
                pIFile = IFILEMGR_OpenFile( pIFileMgr, pMmsDataInfoCur->MMSDataFileName, _OFM_READ);
                if(pIFile != NULL)
                {
                    MSG_FATAL("IDD_VIEWMSG_MMS_Handler 3", 0,0,0);
                    pBuffer = MALLOC(pMmsDataInfoCur->MMSDatasize*sizeof(uint8));
                    DBGPRINTF("WMS Address = 0x%x", pBuffer);   
                    if(pBuffer == NULL)
                    {
                        MSG_FATAL("pBuffer MALLOC FAILED", 0,0,0);
                        break;
                    }
                    MEMSET(pBuffer, 0, pMmsDataInfoCur->MMSDatasize*sizeof(uint8));
                    rSize = IFILE_Read(pIFile, pBuffer, pMmsDataInfoCur->MMSDatasize*sizeof(uint8));
                    MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] rSize=%d, MMSDatasize=%d", rSize, pMmsDataInfoCur->MMSDatasize*sizeof(uint8),0);
                }
                else
                {
                    MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] File Open Failed Error:0x%x", IFILEMGR_GetLastError(pIFileMgr),0,0);
                    RELEASEIF(pIFile);
                    RELEASEIF(pIFileMgr);
                    break;
                }
                RELEASEIF(pIFile);
                RELEASEIF(pIFileMgr);
               
                result = WMS_MMS_PDU_Decode(pDecdata,(uint8*)pBuffer,pMmsDataInfoCur->MMSDatasize, &ePDUType);
                MSG_FATAL("IDD_VIEWMSG_MMS_Handler result=%d", result,0,0);
                if(!pMmsDataInfoCur->MMSDataReaded && pDecdata->message.bReadRep)
                {
                    if(NULL == pMe->m_EncData.pReadReport)
                    {
                        pMe->m_EncData.pReadReport = (MMS_WSP_ENC_READ_REPORT*)MALLOC(sizeof(MMS_WSP_ENC_READ_REPORT));
                    }
                    DBGPRINTF("WMS Address = 0x%x", pMe->m_EncData.pReadReport);  
                    STRCPY((char*)pMe->m_EncData.pReadReport->hTransactionID,(char*)pDecdata->message.hTransactionID);
                    STRCPY((char*)pMe->m_EncData.pReadReport->hTo,(char*)pDecdata->message.hFrom);
                    STRCPY((char*)pMe->m_EncData.pReadReport->hMessageID,(char*)pDecdata->message.hMessageID);

                    WMS_MMSState(WMS_MMS_PDU_MReadRecInd,NULL,(uint32)&pMe->m_EncData);
                    
                }
                if(!pMmsDataInfoCur->MMSDataReaded)
                {
                    MSG_FATAL("IDD_VIEWMSG_MMS_Handler MMSDataReaded", 0,0,0);
                    mmsDataInfoList[nInfoIndex].MMSDataReaded = TRUE;
                }
                (void) ICONFIG_SetItem(pMe->m_pConfig,
                               nMmsDataInfo,
                               (void*)mmsDataInfoList,
                               sizeof(mmsDataInfoList));                  
                MSG_FATAL("IDD_VIEWMSG_MMS_Handler result=%d, frag_num=%d, pMe->m_wCurindex=%d", result, pDecdata->message.mms_data.frag_num, pMe->m_wCurindex);
                for(; index < pDecdata->message.mms_data.frag_num; index++)
                {
                    DBGPRINTF("fragment[%d].hContentType=%s",index,(char*)(pDecdata->message.mms_data.fragment[index].hContentType));
                    if(STRISTR((char*)(pDecdata->message.mms_data.fragment[index].hContentType), "text/"))
                    {
                        FREEIF(pViewText);
                        pViewText = (AECHAR*)MALLOC((rSize+1)*sizeof(AECHAR));
                        DBGPRINTF("WMS Address = 0x%x", pViewText);  
                        rSize = pDecdata->message.mms_data.fragment[index].size;
                        MSG_FATAL("IDD_VIEWMSG_MMS_Handler 4 size=%d", rSize,0,0);
                        UTF8TOWSTR((byte*)(pDecdata->message.mms_data.fragment[index].pContent),rSize, pViewText, (rSize+1)*sizeof(AECHAR));
                        
                        DBGPRINTF("IDD_VIEWMSG_MMS_Handler pContent=%s", (char*)(pDecdata->message.mms_data.fragment[index].pContent));
                        
                        
                        DBGPRINTF("pViewText=%S", pViewText);
                        ISTATIC_SetText(pStatic, NULL, pViewText,AEE_FONT_BOLD, AEE_FONT_BOLD);
                        
                        if (NULL != pViewText)
                        {
                            ISTATIC_SetProperties(pStatic, ST_GRAPHIC_BG);  
                            ISTATIC_SetBackGround(pStatic, AEE_APPSCOMMONRES_IMAGESFILE, IDB_BACKGROUND);
                            hasIStatic = TRUE;
                          //  pMe->m_ResData.textData.nCount++;
                        }
                        MSG_FATAL("IMENUCTL_GetItemCount0=%d", IMENUCTL_GetItemCount(pMenuCtl),0,0);
                        //break;
                    }
                    else if(pMimeType = MMS_WSP_MineType2MormalMimeType((const char*)pDecdata->message.mms_data.fragment[index].hContentType))
                    {
                        pMe->m_isMMS = TRUE;
                        DBGPRINTF("[IDD_VIEWMSG_MMS_Handler] pMimeType:%s", pMimeType);
                        if(STRISTR(pMimeType, IMAGE_MIME_BASE))
                        {   
                            AECHAR menuItemName[100] = {0};
                            AEEImageInfo info;
                            STRTOWSTR((char*)pDecdata->message.mms_data.fragment[index].hContentName,
                                menuItemName,
                                100 * sizeof(AECHAR));
                            MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] Image Count++", 0, 0, 0);
                            pMe->m_ResData.imageData.data[pMe->m_ResData.imageData.nCount].nResIndex = index;
                            pMe->m_ResData.imageData.data[pMe->m_ResData.imageData.nCount].type = pMimeType;
                            pMe->m_ResData.imageData.nCount++;
                            MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] Image 1", 0, 0, 0);
                            pIImage = WmsLoadImageFromData(pMe,
                                        pMe->m_ResData.imageData.data[pMe->m_ResData.imageData.nIndex].nResIndex,
                                        pMe->m_ResData.imageData.data[pMe->m_ResData.imageData.nIndex].type);
                            DBGPRINTF("pIImage ADDRESS=0x%X", pIImage);
                            MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] Image 2", 0, 0, 0);
                            if(pIImage != NULL)
                            {
                                MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] pIImage != NULL", 0, 0, 0);
                                IIMAGE_GetInfo(pIImage, &info);
                                if(!info.bAnimated)
                                {
                                    IIMAGE_SetParm(pIImage,IPARM_SCALE, 30, 30);
                                    MEMSET(&ai, 0, sizeof(ai));
                                    ai.wItemID   = index;
                                    ai.pImage = pIImage;
                                    if(FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
                                    {
                                       MSG_FATAL("Failed to Add Opts item %d", ai.wItemID,0,0);
                                       return EFAILED;
                                    }   
                                    IMENUCTL_SetItemText(pMenuCtl, index, NULL, 0, menuItemName);
                                    DBGPRINTF("Image menuItemName=%S, ItemCount=%d, wItemID=%d", menuItemName, IMENUCTL_GetItemCount(pMenuCtl), ai.wItemID);
                                    MSG_FATAL("IMENUCTL_GetItemCount1=%d", IMENUCTL_GetItemCount(pMenuCtl),0,0);
                                }
                                else
                                {
                                    IImage* pIImageTemp = NULL;
                                    pIImageTemp = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDB_MMS_SPACE);  
                                    IIMAGE_SetParm(pIImageTemp,IPARM_SCALE, 30, 30);
                                    MEMSET(&ai, 0, sizeof(ai));
                                    ai.wItemID   = index;
                                    ai.pImage = pIImageTemp;
                                    if(FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
                                    {
                                       MSG_FATAL("Failed to Add Opts item %d", ai.wItemID,0,0);
                                       return EFAILED;
                                    }   
                                    IMENUCTL_SetItemText(pMenuCtl, index, NULL, 0, menuItemName);
                                    DBGPRINTF("Image menuItemName=%S, ItemCount=%d, wItemID=%d", menuItemName, IMENUCTL_GetItemCount(pMenuCtl), ai.wItemID);
                                    MSG_FATAL("IMENUCTL_GetItemCount1=%d", IMENUCTL_GetItemCount(pMenuCtl),0,0);
                                    RELEASEIF(pIImageTemp);
                                    if(pMe->m_Animate != NULL)
                                    {
                                        IIMAGE_Stop(pMe->m_Animate);
                                        IIMAGE_Release(pMe->m_Animate);
                                        pMe->m_Animate = NULL;
                                    }
                                    pMe->m_Animate = WmsLoadImageFromData(pMe,
                                        pMe->m_ResData.imageData.data[pMe->m_ResData.imageData.nIndex].nResIndex,
                                        pMe->m_ResData.imageData.data[pMe->m_ResData.imageData.nIndex].type);;
                                    DBGPRINTF("pMe->m_Animate=0x%x", pMe->m_Animate);
                                    hasAnimate = TRUE;
                                }
                                RELEASEIF(pIImage);
                            }
                        }
                        else if(STRISTR(pMimeType, SOUND_MIME_BASE))
                        {
                            AECHAR menuItemName[100] = {0};
                            STRTOWSTR((char*)pDecdata->message.mms_data.fragment[index].hContentName,
                                menuItemName,
                                100 * sizeof(AECHAR));                            
                            MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] Sound Count++", 0, 0, 0);
                            MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] Sound Type:%s", pMimeType, 0, 0);
                            pMe->m_ResData.soundData.data[pMe->m_ResData.soundData.nCount].nResIndex = index;
                            pMe->m_ResData.soundData.data[pMe->m_ResData.soundData.nCount].type = pMimeType;
                            pMe->m_ResData.soundData.nCount++;
                            pISound = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_MUSIC);  
                            DBGPRINTF("pISound ADDRESS=0x%X", pISound);
                            IIMAGE_SetParm(pISound,IPARM_SCALE, 30, 30);                        
                            MEMSET(&ai, 0, sizeof(ai));
                            MEMSET(&ai, 0, sizeof(ai));
                            ai.wItemID   = index;
                            ai.pImage = pISound;
                            if(FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
                            {
                               MSG_FATAL("Failed to Add Opts item %d", ai.wItemID,0,0);
                               return EFAILED;
                            }                   
                            MSG_FATAL("SoundIndex=%d", index,0,0);
                            IMENUCTL_SetItemText(pMenuCtl, index, NULL, 0, menuItemName);   
                            DBGPRINTF("Sound menuItemName=%s, ItemCount=%d, wItemID=%d", menuItemName, IMENUCTL_GetItemCount(pMenuCtl), ai.wItemID);
                            WmsLoadSoundFromData(pMe,
                                pMe->m_ResData.soundData.data[pMe->m_ResData.soundData.nIndex].nResIndex,
                                pMe->m_ResData.soundData.data[pMe->m_ResData.soundData.nIndex].type);
                            MSG_FATAL("IMENUCTL_GetItemCount2=%d", IMENUCTL_GetItemCount(pMenuCtl),0,0);
                            RELEASEIF(pISound);
                        }
                        else if(STRISTR(pMimeType, VIDEO_MIME_BASE))
                        {
                            AECHAR menuItemName[100] = {0};
                            STRTOWSTR((char*)pDecdata->message.mms_data.fragment[index].hContentName,
                                menuItemName,
                                100 * sizeof(AECHAR));                              
                            MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] Video Count++", 0, 0, 0);
                            pMe->m_ResData.videoData.data[pMe->m_ResData.videoData.nCount].nResIndex = index;
                            pMe->m_ResData.videoData.data[pMe->m_ResData.videoData.nCount].type = pMimeType;
                            pMe->m_ResData.videoData.nCount ++;
                            pIVideo = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_VIDEO);  
                            IIMAGE_SetParm(pIVideo,IPARM_SCALE, 30, 30);                        
                            MEMSET(&ai, 0, sizeof(ai));
                            MEMSET(&ai, 0, sizeof(ai));
                            ai.wItemID   = index;
                            ai.pImage = pIVideo;
                            if(FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
                            {
                               MSG_FATAL("Failed to Add Opts item %d", ai.wItemID,0,0);
                               return EFAILED;
                            }                   
                            MSG_FATAL("SoundIndex=%d", index,0,0);
                            IMENUCTL_SetItemText(pMenuCtl, index, NULL, 0, menuItemName);    
                            RELEASEIF(pIVideo);
                        }
                        else
                        {/*
                            AECHAR menuItemName[100] = {0};
                            
                            STRTOWSTR((char*)pDecdata->message.mms_data.fragment[index].hContentName,
                                menuItemName,
                                100 * sizeof(AECHAR));

                            MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] Len:%d", WSTRLEN(menuItemName),0 , 0);

                            IMENUCTL_AddItem(pMenuCtl,
                                NULL,
                                0,
                                0,
                                menuItemName,
                                (uint32)pDecdata->message.mms_data.fragment[index].pContent);*/
                        }
                    }
                    else
                    {/*
                        AECHAR menuItemName[100] = {0};
                        STRTOWSTR((char*)pDecdata->message.mms_data.fragment[index].hContentName,
                            menuItemName,
                            100 * sizeof(AECHAR));

                        MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] Len:%d", WSTRLEN(menuItemName),0 , 0);
                        IMENUCTL_AddItem(pMenuCtl,
                            NULL,
                            0,
                            0,
                            menuItemName,
                            (uint32)pDecdata->message.mms_data.fragment[index].pContent);*/
                       //ICONTROL_SetActive((IControl*)pListCtl,TRUE);
                    }
                }
                MSG_FATAL("IMENUCTL Count=%d, hasIStatic=%d",IMENUCTL_GetItemCount(pMenuCtl),hasIStatic,0);
                if((IMENUCTL_GetItemCount(pMenuCtl) == 0) && hasIStatic)
                {
                    AEERect StaticRect={0};
                    AEERect MenuRect={0};
                    MSG_FATAL("IMENUCTL_GetItemCount 4", 0,0,0);
                    SETAEERECT(&StaticRect,  0, 0, pMe->m_rc.dx, pMe->m_rc.dy - BOTTOMBAR_HEIGHT);
                    ISTATIC_SetRect (pStatic, &StaticRect);
                    ISTATIC_SetActive(pStatic, TRUE);
                    ISTATIC_Redraw(pStatic);
                    IMENUCTL_SetActive(pMenuCtl, FALSE);
                    IDIALOG_SetFocus(pMe->m_pActiveIDlg, IDC_VIEWMSG_MMS_STATIC);
                }
                else if((IMENUCTL_GetItemCount(pMenuCtl) > 0) && !hasIStatic)
                {
                    AEERect Rect={0};
                    Rect = pMe->m_rc;
                    Rect.y = 0; 
                    Rect.dy = devinfo.cyScreen - GetBottomBarHeight(pMe->m_pDisplay);                  
                    IMENUCTL_SetRect(pMenuCtl, &Rect); 
                    ISTATIC_SetActive(pStatic, FALSE);
                    IMENUCTL_SetActive(pMenuCtl, TRUE);   
                    IMENUCTL_Redraw(pMenuCtl);
                    IDIALOG_SetFocus(pMe->m_pActiveIDlg, IDC_VIEWMSG_MMS_MENU);
                }
                else
                {
                    AEERect StaticRect={0};
                    AEERect Rect={0};
                    MSG_FATAL("IMENUCTL_GetItemCount 6", 0,0,0);
                    SETAEERECT(&StaticRect,  0, 0 ,pMe->m_rc.dx, GetBottomBarHeight(pMe->m_pDisplay)*2);
                    ISTATIC_SetRect (pStatic, &StaticRect);
                    ISTATIC_Redraw(pStatic);
                    ISTATIC_SetActive(pStatic, FALSE);
                    IMENUCTL_SetActive(pMenuCtl, TRUE);
                    
                    Rect = pMe->m_rc;
                    Rect.y = GetBottomBarHeight(pMe->m_pDisplay)*2; 
                    Rect.dy = devinfo.cyScreen;
                    Rect.dy -= (GetBottomBarHeight(pMe->m_pDisplay)*3); 
                    IMENUCTL_SetRect(pMenuCtl, &Rect); 
                    IMENUCTL_Redraw(pMenuCtl);
                    IDIALOG_SetFocus(pMe->m_pActiveIDlg, IDC_VIEWMSG_MMS_MENU);
                }                
                pMe->m_CurrentState == PLAYER_IDLE;
                MSG_FATAL("IDD_VIEWMSG_MMS_Handler init m_CurrentState=%d", pMe->m_CurrentState,0,0);
                
                pMe->m_ResData.nIndex = IDC_VIEWMSG_MMS_STATIC;
                if(pBuffer != NULL)
                {
                    FREE(pBuffer);
                }
                IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE);            
            }
            return TRUE;

        case EVT_DIALOG_START:
            MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] EVT_DIALOG_START",0 ,0 , 0);
            if(pMe->m_pMedia)
            {
                int result = 0;
                MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] Draw Play",0 ,0 , 0);
                result = IMEDIA_Play(pMe->m_pMedia);
                MSG_FATAL("IDD_VIEWMSG_MMS_Handler] IMEDIA_Play result=%d", result,0,0);
                	//注册播放回调
	            result = IMEDIA_RegisterNotify(pMe->m_pMedia, (PFNMEDIANOTIFY)WMSMMS_MediaNotify, pMe);
                MSG_FATAL("IDD_VIEWMSG_MMS_Handler] EVT_DIALOG_STAR result=%d", result,0,0);
            } 
            
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);                 
            return TRUE;

        case EVT_USER_REDRAW:
            {
                char* pMimeType = NULL;
                AEERect rc;
                uint8 MenuSelectdId = IMENUCTL_GetSel(pMenuCtl); 
                CtlAddItem ai;
                char menuItemName[100] = {0};
                MSG_FATAL("[IDD_VIEWMSG_MMS_Handler 1] AnimateState=%d",AnimateState ,0 , 0);
                if(AnimateState == 2)
                {
                    if(hasAnimate)
                    {
                        AEEImageInfo info;
                        uint8 x = 0;
                        uint8 y = 0;
                        AEEDeviceInfo devinfo={0};
                        ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
                        if(pMe->m_Animate != NULL)
                        {
                            MSG_FATAL("[IDD_VIEWMSG_MMS_Handler 2] pMe->m_Animate != NULL",0 ,0 , 0);
                            ISTATIC_SetActive(pStatic, FALSE);
                            IMENUCTL_SetActive(pMenuCtl, FALSE);                               
                            IDISPLAY_ClearScreen(pMe->m_pDisplay);
                            SETAEERECT(&rc, 0, 0, devinfo.cxScreen, devinfo.cyScreen);
                            IDISPLAY_FillRect(pMe->m_pDisplay, &rc, MAKE_RGB(0, 0, 0));                                
                            IIMAGE_GetInfo(pMe->m_Animate, &info);
                            if(info.cx < devinfo.cxScreen)
                            {
                                x = (devinfo.cxScreen- info.cx)/2;
                            }
                            if(info.cy < devinfo.cyScreen)
                            {
                                y = (devinfo.cyScreen- info.cy)/2;
                                if(y + info.cy > devinfo.cyScreen - BOTTOMBAR_HEIGHT)
                                {
                                    y = 0;
                                }
                            }
                            ISTATIC_SetText(pStatic, NULL, L" ",AEE_FONT_BOLD, AEE_FONT_BOLD);
                            IIMAGE_Start(pMe->m_Animate, x, y);                         
                        }
                    }                    
                }
                else if(IMENUCTL_GetItem(pMenuCtl, MenuSelectdId, &ai))
                {
                    char *pext = NULL;
                    WSTRTOSTR(ai.pText, menuItemName, sizeof(menuItemName));
                    DBGPRINTF("gif name=%s", menuItemName);
                    pext = STRRCHR(menuItemName, '.');
                    if((pext != NULL) && (pext+1 != NULL))
                    {
                        if(STRICMP(pext+1, "gif") == 0)
                        {
                            DBGPRINTF("drawAnimate = TRUE");
                            AnimateState = 1;   //当前光标停留在动画上                                               
                        }
                        else
                        {
                            AnimateState = 0;   //当前动画无操作
                        }
                    }
                    else
                    {
                        AnimateState = 0;   //当前动画无操作
                    }
                }   
                MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] AnimateState=%d",AnimateState ,0 , 0);
                if(AnimateState != 2)
                {
                    //只有当不是在动画播放界面时，才画其他的元素
                    DBGPRINTF("EVT_USER_REDRAW pViewText=%S", pViewText);     
                    IDISPLAY_ClearScreen(pMe->m_pDisplay);
                    SETAEERECT(&rc, 0, 0, pMe->m_rc.dx, pMe->m_rc.dy);
                    IDISPLAY_FillRect(pMe->m_pDisplay, &rc, MAKE_RGB(0, 0, 0));  
                    ISTATIC_SetActive(pStatic, FALSE);
                    IMENUCTL_SetActive(pMenuCtl, TRUE);                        
                    ISTATIC_SetText(pStatic, NULL, pViewText,AEE_FONT_BOLD, AEE_FONT_BOLD);
                    ISTATIC_Redraw(pStatic);
                    if(IMENUCTL_GetItemCount(pMenuCtl) > 0)
                    {
                        IMENUCTL_Redraw(pMenuCtl);             
                    }
                }
                MSG_FATAL("m_CurrentState=%d, soundData.nCount=%d, m_eMBoxType=%d",pMe->m_CurrentState,pMe->m_ResData.soundData.nCount,pMe->m_eMBoxType);
                
                if(pDecdata != NULL)
                {
                    DBGPRINTF("AVK_INFO MenuSelectdItem hContentType=%s", (const char*)pDecdata->message.mms_data.fragment[MenuSelectdId].hContentType);
                    pMimeType = MMS_WSP_MineType2MormalMimeType((const char*)pDecdata->message.mms_data.fragment[MenuSelectdId].hContentType);
                    if(pMimeType != NULL)
                    {
                        DBGPRINTF("pMimeType=%s", pMimeType);
                        if(STRISTR(pMimeType, VIDEO_MIME_BASE))
                        {
                            if(pMe->m_eMBoxType == WMS_MB_OUTBOX_MMS)
                            {
                                DRAW_BOTTOMBAR(BTBAR_PLAY_BACK);
                                return TRUE;
                            }
                            else
                            {
                                DRAW_BOTTOMBAR(BTBAR_OPTION_PLAY_BACK);
                                return TRUE;
                            }
                        }
                    }
                }
                if(AnimateState == 1)
                {
                    if(pMe->m_eMBoxType == WMS_MB_OUTBOX_MMS)
                    {
                        DRAW_BOTTOMBAR(BTBAR_PLAY_BACK);
                    }
                    else
                    {
                        DRAW_BOTTOMBAR(BTBAR_OPTION_PLAY_BACK);
                    }
                }
                else if(AnimateState == 2)
                {
                    DRAW_BOTTOMBAR(BTBAR_BACK);
                }
                else if(pMe->m_eMBoxType == WMS_MB_OUTBOX_MMS)
                {
                    if(pMe->m_ResData.soundData.nCount < 1)
                    {
                        DRAW_BOTTOMBAR(BTBAR_BACK);
                        pMe->m_CurrentState == PLAYER_IDLE;
                    }      
                    else if(pMe->m_CurrentState == PLAYER_IDLE)
                    {
                        DRAW_BOTTOMBAR(BTBAR_BACK);
                    }
                    else if(pMe->m_CurrentState == PLAYER_PAUSE)
                    {
                        DRAW_BOTTOMBAR(BTBAR_PLAY_BACK);
                    }
                    else if(pMe->m_CurrentState == PLAYER_PLAY)
                    {
                        DRAW_BOTTOMBAR(BTBAR_STOP_BACK);
                    }
                    else if(pMe->m_CurrentState == PLAYER_STOP)
                    {
                        DRAW_BOTTOMBAR(BTBAR_PLAY_BACK);
                    }
                    else
                    {
                        DRAW_BOTTOMBAR(BTBAR_BACK);
                    }                    
                }
                else
                {
                    if(pMe->m_ResData.soundData.nCount < 1)
                    {
                        DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
                        pMe->m_CurrentState == PLAYER_IDLE;
                    }
                    else if(pMe->m_CurrentState == PLAYER_IDLE)
                    {
                        DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
                    }
                    else if(pMe->m_CurrentState == PLAYER_PAUSE)
                    {
                        DRAW_BOTTOMBAR(BTBAR_OPTION_PLAY_BACK);
                    }
                    else if(pMe->m_CurrentState == PLAYER_PLAY)
                    {
                        DRAW_BOTTOMBAR(BTBAR_OPTION_STOP_BACK);
                    }
                    else if(pMe->m_CurrentState == PLAYER_STOP)
                    {
                        DRAW_BOTTOMBAR(BTBAR_OPTION_PLAY_BACK);
                    }
                    else
                    {
                        DRAW_BOTTOMBAR(BTBAR_OPTION_BACK);
                    }
                }
                IDISPLAY_Update(pMe->m_pDisplay);         
                return TRUE;
            }
        
		case EVT_WMS_MSG_RECEIVED_MESSAGE:
			return TRUE;

        case EVT_DIALOG_END:
            {
                uint8 i = 0;
                MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] EVT_DIALOG_END 1",0 ,0 , 0);
                pMe->m_isMMS = FALSE;
              /*   
                if(pMe->m_pMedia != NULL)
                {
                    IMedia_Stop(pMe->m_pMedia);
                }
                */
                FREEIF(pViewText);
                pMe->m_CurrentState == PLAYER_IDLE;
                RELEASEIF(pMe->m_pMedia);
                 if (NULL != pMe->m_pMenu)
                {
                    IMENUCTL_Release(pMe->m_pMenu);
                    pMe->m_pMenu = NULL;
                }    
                MEMSET(&pMe->m_ResData,NULL,sizeof(WSP_MMS_RESOURCE));
                FREEIF(pMe->m_DecData.message.hBody);
                FREEIF(pMe->m_DecData.message.mms_data.head_info.pContent);
                for(i = 0;i < pMe->m_DecData.message.mms_data.frag_num;i++)
                {
                    FREEIF(pMe->m_DecData.message.mms_data.fragment[i].pContent);
                }  
                MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] EVT_DIALOG_END 2",0 ,0 , 0);
                if(pMe->m_Animate != NULL)
                {
                    IIMAGE_Stop(pMe->m_Animate);
                    IIMAGE_Release(pMe->m_Animate);
                    pMe->m_Animate = NULL;
                }
                AnimateState = 0;
                MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] EVT_DIALOG_END 3",0 ,0 , 0);
            }
            return TRUE;

        case EVT_CTL_SEL_CHANGED:   
            if (pMe->m_pMenu == NULL)
            {
                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            }
            return TRUE;

        case EVT_KEY_RELEASE:
            {
                if (pMe->m_pMenu != NULL)
                {
                    return TRUE;
                }
            }
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);             
            return TRUE;
            
        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    if(AnimateState == 2)
                    {
                        if(pMe->m_Animate != NULL)
                        {
                            IIMAGE_Stop(pMe->m_Animate);
                        }                        
                        AnimateState = 1;
                        (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                            EVTFLG_ASYNC,
                                            AEECLSID_WMSAPP,
                                            EVT_USER_REDRAW,
                                            0, 
                                            0);                        
                        return TRUE;
                    }
#ifdef FEATURE_ALL_KEY_PAD
                    if (NULL != pMe->m_pMenu)
                    {
                        //ISTATIC_SetProperties(pStatic, ST_GRAPHIC_BG);  
                        IMENUCTL_Release(pMe->m_pMenu);
                        pMe->m_pMenu = NULL;
                        (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                            EVTFLG_ASYNC,
                                            AEECLSID_WMSAPP,
                                            EVT_USER_REDRAW,
                                            0, 
                                            0);
                    }
                    else
                    {
                        CLOSE_DIALOG(DLGRET_CANCELED)
                    }
#else
                    if (NULL == pMe->m_pMenu)
                    {
                        CLOSE_DIALOG(DLGRET_CANCELED)
                    }
                    else
                    {
                        MSG_FATAL("pMe->m_pMenu != NULL",0,0,0);
                        //ISTATIC_SetProperties(pStatic, ST_GRAPHIC_BG);  
                        IMENUCTL_Release(pMe->m_pMenu);
                        pMe->m_pMenu = NULL;
                        (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                                EVTFLG_ASYNC,
                                                AEECLSID_WMSAPP,
                                                EVT_USER_REDRAW,
                                                0, 
                                                0);
                    }
#endif      

                    return TRUE;
                       
                    case AVK_SELECT:
                        if ((pMe->m_pMenu != NULL) || (AnimateState == 2))
                        {
                            return TRUE;
                        }
                        MSG_FATAL("IDD_VIEWMSG_MMS_Handler AVK_SELECT m_eMBoxType=%d",pMe->m_eMBoxType,0,0);
                        //ISTATIC_SetProperties(pStatic, ST_GRAPHIC_BG|ST_NOSCROLL);  
                        ISTATIC_SetText(pStatic, NULL, L" ",AEE_FONT_BOLD, AEE_FONT_BOLD);
                        if(hasAnimate)
                        {
                            if(pMe->m_Animate != NULL)
                            {
                                IIMAGE_Stop(pMe->m_Animate);
                            }
                        }                        
                        if(pMe->m_eMBoxType == WMS_MB_INBOX_MMS)
                        {
                            // 显示弹出菜单
                            if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_MENUCTL, 
                                    (void **) &pMe->m_pMenu) == SUCCESS)
                            {
                                MSG_FATAL("IDD_VIEWMSG_MMS_Handler AVK_SELECT 1",0,0,0);
                                // 动态添加菜单项
                                MENU_ADDITEM(pMe->m_pMenu, IDS_REPLY);
                                MENU_ADDITEM(pMe->m_pMenu, IDS_FORWARD);
                                MENU_ADDITEM(pMe->m_pMenu, IDS_CALL);
                                MENU_ADDITEM(pMe->m_pMenu, IDS_SAVE_CURRENT_ITEM);    //Add By zzg 2010_09_11                     
                                MENU_ADDITEM(pMe->m_pMenu, IDS_DELETE);
#ifdef FEATURE_VERSION_W208S                                
                                MENU_ADDITEM(pMe->m_pMenu, IDS_ADD_TO_RESTRICT_LIST);
#endif
                                // 设置菜单属性
                                IMENUCTL_SetPopMenuRect(pMe->m_pMenu);

                                IMENUCTL_SetProperties(pMe->m_pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
                                IMENUCTL_SetBottomBarType(pMe->m_pMenu,BTBAR_SELECT_BACK);
                                IMENUCTL_SetActive(pMenuCtl, FALSE);
                                IMENUCTL_SetActive(pMe->m_pMenu, TRUE);
                                
                                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                                MSG_FATAL("IDD_VIEWMSG_MMS_Handler AVK_SELECT 2",0,0,0);
                            }
                        }
                        else if(pMe->m_eMBoxType == WMS_MB_DRAFTBOX_MMS)
                        {
                            // 显示弹出菜单
                            if (ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_MENUCTL, 
                                    (void **) &pMe->m_pMenu) == SUCCESS)
                            {
                                MENU_ADDITEM(pMe->m_pMenu, IDS_EDIT);
                                MENU_ADDITEM(pMe->m_pMenu, IDS_DELETE);
                                MENU_ADDITEM(pMe->m_pMenu, IDS_DELETEALL);   

                                // 设置菜单属性
                                IMENUCTL_SetPopMenuRect(pMe->m_pMenu);

                                IMENUCTL_SetProperties(pMe->m_pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
                                IMENUCTL_SetBottomBarType(pMe->m_pMenu,BTBAR_SELECT_BACK);
                                IMENUCTL_SetActive(pMenuCtl, FALSE);
                                IMENUCTL_SetActive(pMe->m_pMenu, TRUE);
                                
                                IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                            }
                        }                        
                        return TRUE;
                    
                    case AVK_INFO:
                        if((pMe->m_pMenu == NULL) && (AnimateState != 2))
                        {
                            char* pMimeType = NULL;
                            uint8 MenuSelectdId = IMENUCTL_GetSel(pMenuCtl);  
                            if(pDecdata != NULL)
                            {
                                DBGPRINTF("AVK_INFO MenuSelectdItem hContentType=%s", (const char*)pDecdata->message.mms_data.fragment[MenuSelectdId].hContentType);
                                pMimeType = MMS_WSP_MineType2MormalMimeType((const char*)pDecdata->message.mms_data.fragment[MenuSelectdId].hContentType);
                                if(pMimeType != NULL)
                                {
                                    DBGPRINTF("pMimeType=%s", pMimeType);
                                    if(STRISTR(pMimeType, VIDEO_MIME_BASE))
                                    {
                                        IVideoPlayer *pVideoPlayer = NULL;
                                        IFile* pIFile = NULL;
                    		            IFileMgr *pIFileMgr = NULL;
                                        char FilePath[MMS_MAX_CONTENT_NAME];  
                                        int result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);                                        
                                        if(result != SUCCESS)
                                        {
                                            return FALSE;
                                        }
                                        if (SUCCESS != IFILEMGR_Test(pIFileMgr, "fs:/hsmm/MMSVideoTemp"))
                                        {
                                            (void)IFILEMGR_MkDir(pIFileMgr, "fs:/hsmm/MMSVideoTemp");
                                        }  
                                        SPRINTF(FilePath,"fs:/hsmm/MMSVideoTemp/%s",(char*)(pDecdata->message.mms_data.fragment[MenuSelectdId].hContentName));
                                        DBGPRINTF("FilePath=%s",FilePath);
                                        result = EBADPARM;
                                        if(IFILEMGR_Test(pIFileMgr, FilePath) ==  SUCCESS)
                                        {
                                            IFILEMGR_Remove(pIFileMgr, FilePath);
                                        }
                                        pIFile = IFILEMGR_OpenFile( pIFileMgr, FilePath, _OFM_CREATE);
                                        result = IFILEMGR_GetLastError(pIFileMgr);
                                        if ( (pIFile != NULL ) && (result == SUCCESS))
                                        {
                                            MSG_FATAL("IDD_VIEWMSG_MMS_Handler VideoPlay 1", 0,0,0);
                                            IFILE_Write( pIFile, pDecdata->message.mms_data.fragment[MenuSelectdId].pContent, pDecdata->message.mms_data.fragment[MenuSelectdId].size);
                                            result = IFILEMGR_GetLastError(pIFileMgr);
                                            MSG_FATAL("size=%d",pDecdata->message.mms_data.fragment[MenuSelectdId].size,0,0);
                                        }  
                                        else
                                        {
                                            MSG_FATAL("IDD_VIEWMSG_MMS_Handler VideoPlay 2",0,0,0);
                                            if ( pIFile != NULL )
                                            {
                                                IFILE_Release( pIFile);
                                                pIFile = NULL;
                                            }
                                            if ( pIFileMgr != NULL )
                                            {
                                                IFILEMGR_Release(pIFileMgr);
                                                pIFileMgr = NULL;
                                            }                                            
                                            return FALSE;
                                        }
                                        if ( pIFile != NULL )
                                        {
                                            IFILE_Release( pIFile);
                                            pIFile = NULL;
                                        }
                                        if ( pIFileMgr != NULL )
                                        {
                                            IFILEMGR_Release(pIFileMgr);
                                            pIFileMgr = NULL;
                                        }                                        
                                        ISHELL_CreateInstance(pMe->m_pShell,
                                             AEECLSID_VIDEOPLAYER,
                                             (void**)&pVideoPlayer);
                                       MSG_FATAL("IDD_VIEWMSG_MMS_Handler VideoPlay 3",0,0,0);
                                       if(pVideoPlayer != NULL)
                                       {
                                          MSG_FATAL("IDD_VIEWMSG_MMS_Handler VideoPlay 4",0,0,0);
                                          IVideoPlayer_Play(pVideoPlayer, FilePath);
                                          IVideoPlayer_Release(pVideoPlayer);
                                          pVideoPlayer = NULL;
                                       }
                                       else
                                       {
                                          MSG_FATAL("IDD_VIEWMSG_MMS_Handler VideoPlay 5",0,0,0);
                                          return FALSE;
                                       }
                                       return TRUE;        
                                    }
                                    else if(STRISTR(pMimeType, SOUND_MIME_BASE))
                                    {
                                        if(pMe->m_pMedia != NULL)
                                        {
                                            int result = 0;
                                            MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] AVK_INFO m_CurrentState=%d",pMe->m_CurrentState ,0 , 0);
                                            if(pMe->m_CurrentState == PLAYER_PAUSE)
                                            {
                                                result = IMedia_Resume(pMe->m_pMedia);
                                                if(result != SUCCESS)
                                                {
                                                    result = IMedia_Stop(pMe->m_pMedia);
                                                    result = IMedia_Play(pMe->m_pMedia);
                                                }                                    
                                            }
                                            else if(pMe->m_CurrentState == PLAYER_PLAY)
                                            {
                                                result = IMedia_Pause(pMe->m_pMedia);
                                            }
                                            else if(pMe->m_CurrentState == PLAYER_STOP)
                                            {
                                                result = IMedia_Play(pMe->m_pMedia);
                                                if(result != SUCCESS)
                                                {
                                                    result = IMedia_Stop(pMe->m_pMedia);
                                                    result = IMedia_Play(pMe->m_pMedia);
                                                }                                    
                                            }
                                            else
                                            {
                                                result = IMedia_Stop(pMe->m_pMedia);
                                            }
                                            if(result != SUCCESS)
                                            {
                                                result = IMedia_Stop(pMe->m_pMedia);
                                                result = IMedia_Play(pMe->m_pMedia);
                                            }
                                            MSG_FATAL("IDD_VIEWMSG_MMS_Handler] State=%d, result=%d", pMe->m_CurrentState, result, 0);
                                            return TRUE;
                                        }                                              
                                    }
                                    else if(STRISTR(pMimeType, IMAGE_MIME_BASE))
                                    {
                                        MSG_FATAL("IDD_VIEWMSG_MMS_Handler IMAGE_MIME_BASE 4",0,0,0);
                                        if(AnimateState != 2)
                                        {
                                            char *pext = NULL;
                                            DBGPRINTF("AVK_INFO gif name=%s", pMimeType);
                                            pext = STRRCHR(pMimeType, '/');
                                            if((pext != NULL) && (pext+1 != NULL))
                                            {
                                                if(STRICMP(pext+1, "gif") == 0)
                                                {
                                                    //当前光标停留在动画上，并按下INFO键，这时动画状态变为播放状态2
                                                    AnimateState = 2;                                                    
                                                }
                                            }                                            
                                            (void)ISHELL_PostEventEx(pMe->m_pShell, 
                                                                    EVTFLG_ASYNC,
                                                                    AEECLSID_WMSAPP,
                                                                    EVT_USER_REDRAW,
                                                                    0, 
                                                                    0);                                            
                                        }
                                        return TRUE;
                                    }
                                }
                            }
                        }
                        return IDD_WRITEMSG_Handler((void *)pMe, EVT_COMMAND, IDS_SEND, 0);
                        
                    default:
                        break;
            }
            return TRUE;

        case EVT_COMMAND:
            if (NULL != pMe->m_pMenu)
            {
                IMENUCTL_Release(pMe->m_pMenu);
                pMe->m_pMenu = NULL;
            }
            switch(wParam)
            {
                // 回复
                case IDS_REPLY:                       
                    CLOSE_DIALOG(DLGRET_REPLY)
                    return TRUE;

                // 转发
                case IDS_EDIT:
                case IDS_FORWARD:     
                {
                    uint8 index = 0;
                    int rSize = 0;
                    char* pMimeType = NULL;
                    AECHAR *pFormatedText = NULL;
                    char FilePath[MMS_MAX_CONTENT_NAME];  
                    for(; index < pDecdata->message.mms_data.frag_num; index++)
                    {
                        if(STRISTR((char*)(pDecdata->message.mms_data.fragment[index].hContentType), "text/"))
                        {
                            char* pContentText = NULL;
                            rSize = pDecdata->message.mms_data.fragment[index].size;
                            
                            MSG_FATAL("index=%d,hContentType=%s",index,(char*)(pDecdata->message.mms_data.fragment[index].hContentType),0);
                            if(STRSTR((char*)pDecdata->message.mms_data.fragment[index].hContentEnCode,"utf-8"))
                            {
                                pContentText = (char*)MALLOC(rSize + 4);// 4 format token
                                pContentText[0] = 0xef;
                                pContentText[1] = 0xbb;
                                pContentText[2] = 0xbf;
                                DBGPRINTF("WMS Address = 0x%x", pContentText); 
                                MEMCPY((void*)&pContentText[3],(void*)pDecdata->message.mms_data.fragment[index].pContent,rSize);
                                rSize += 3;
                            }
                            else
                            {
                                MEMCPY((void*)pContentText,(void*)pDecdata->message.mms_data.fragment[index].pContent,rSize);
                            }
                            pFormatedText = (AECHAR *)MALLOC((rSize*sizeof(AECHAR)));
                            DBGPRINTF("WMS Address = 0x%x", pFormatedText); 
                            MEMSET((void*)pFormatedText, 0, rSize*sizeof(AECHAR));
                            MSG_FATAL("size=%d", rSize,0,0);
                            DBGPRINTF("IDD_VIEWMSG_MMS_Handler pContent=%s", pDecdata->message.mms_data.fragment[index].pContent);
                            
                            UTF8TOWSTR((byte*)pContentText,rSize, pFormatedText, rSize*sizeof(AECHAR));
                            DBGPRINTF("pContent=%S", pFormatedText);
                            FREEIF(pContentText);
                            
                            if (NULL != pFormatedText)
                            {
                                pMe->m_msSend.m_szMessage = WSTRDUP(pFormatedText);
                                FREEIF(pFormatedText);
                            }
                        }
                        else if(pMimeType = MMS_WSP_MineType2MormalMimeType((const char*)pDecdata->message.mms_data.fragment[index].hContentType))
                        {
                            IFile* pIFile = NULL;
        		            IFileMgr *pIFileMgr = NULL;
                            int result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);
                            pMe->m_isMMS = TRUE;
                            MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] pMimeType:%d", pMimeType, 0, 0);
                            if(pDecdata != NULL)
                            {
                                DBGPRINTF("MenuSelectdItem hContentType=%s", (const char*)pDecdata->message.mms_data.fragment[index].hContentType);
                                pMimeType = MMS_WSP_MineType2MormalMimeType((const char*)pDecdata->message.mms_data.fragment[index].hContentType);
                                if (SUCCESS != IFILEMGR_Test(pIFileMgr, "fs:/hsmm/MMSTemp"))
                                {
                                    (void)IFILEMGR_MkDir(pIFileMgr, "fs:/hsmm/MMSTemp");
                                }  
                                SPRINTF(FilePath,"fs:/hsmm/MMSTemp/%s",(char*)(pDecdata->message.mms_data.fragment[index].hContentName));
                                DBGPRINTF("FilePath=%s",FilePath);
                                result = EBADPARM;
                                if(IFILEMGR_Test(pIFileMgr, FilePath) ==  SUCCESS)
                                {
                                    IFILEMGR_Remove(pIFileMgr, FilePath);
                                }
                                pIFile = IFILEMGR_OpenFile( pIFileMgr, FilePath, _OFM_CREATE);
                                result = IFILEMGR_GetLastError(pIFileMgr);
                                if ( (pIFile != NULL ) && (result == SUCCESS))
                                {
                                    MSG_FATAL("IDS_SAVE  pIFile != NULL ", 0,0,0);
                                    IFILE_Write( pIFile, pDecdata->message.mms_data.fragment[index].pContent, pDecdata->message.mms_data.fragment[index].size);
                                    result = IFILEMGR_GetLastError(pIFileMgr);
                                    MSG_FATAL("size=%d",pDecdata->message.mms_data.fragment[index].size,0,0);
                                }

                                if(result == EFSFULL)
                                {
                                    AEEFileInfo myInfo;
                                    if( IFILEMGR_EnumInit(pIFileMgr, "fs:/hsmm/MMSTemp", FALSE ) == SUCCESS )
                                    {
                                        while( IFILEMGR_EnumNext(pIFileMgr, &myInfo ) )
                                        {
                                            result = IFILEMGR_Remove(pIFileMgr, myInfo.szName);
                                            if( result != SUCCESS )
                                            {
                                                DBGPRINTF( "File %s could not be removed result=%d", myInfo.szName, result );
                                                //return result;
                                            }
                                        }
                                        IFILEMGR_RmDir(pIFileMgr, "fs:/hsmm/MMSTemp");
                                    }
                                    if ( pIFile != NULL )
                                    {
                                        IFILE_Release( pIFile);
                                        pIFile = NULL;
                                    }
                                    if ( pIFileMgr != NULL )
                                    {
                                        IFILEMGR_Release(pIFileMgr);
                                        pIFileMgr = NULL;
                                    }                                    
                                    CLOSE_DIALOG(DLGRET_EFSFULL_MMS)
                                    return TRUE;
                                }
                                else if(result == SUCCESS)
                                {
                                    if(STRISTR(pMimeType, IMAGE_MIME_BASE))
                                    {   
                                        ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSIMAGE,FilePath, sizeof(FilePath));
                                    }
                                    else if(STRISTR(pMimeType, SOUND_MIME_BASE))
                                    {
                                        ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSSOUND,FilePath, sizeof(FilePath)); 
                                    }
                                    else if(STRISTR(pMimeType, VIDEO_MIME_BASE))
                                    {
                                        ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSVIDEO,FilePath, sizeof(FilePath)); 
                                    }       
                                }
                                if ( pIFile != NULL )
                                {
                                    IFILE_Release( pIFile);
                                    pIFile = NULL;
                                }
                                if ( pIFileMgr != NULL )
                                {
                                    IFILEMGR_Release(pIFileMgr);
                                    pIFileMgr = NULL;
                                }
                                MSG_FATAL("IDS_SAVE  2", 0,0,0);
                            }
                        }
                        else
                        {
                            ;
                        }
                    }
                    pMe->m_isForward = TRUE;
                    CLOSE_DIALOG(DLGRET_WRITEMSG)   
                    return TRUE;
                }                 

                //呼叫
                case IDS_CALL:  
                    {
                        int nInfoIndex = pMe->m_wCurindex - 1;
                        MMSData mmsDataInfoList[MAX_MMS_STORED];
                        MMSData* pMmsDataInfoCur = NULL;
                        AECHAR phoneNumber[13] = {0};  
                        (void) ICONFIG_GetItem(pMe->m_pConfig,
                                           CFGI_MMSINDATA_INFO,
                                           (void*)mmsDataInfoList,
                                           sizeof(mmsDataInfoList));   
                        pMmsDataInfoCur  = &mmsDataInfoList[nInfoIndex]; 
                        STRTOWSTR(pMmsDataInfoCur->phoneNumber,phoneNumber,sizeof(phoneNumber));                         
                        if (WSTRLEN(phoneNumber) > 0)
                        {
                            DBGPRINTF("phoneNumber=%S, nInfoIndex=%d", phoneNumber, nInfoIndex);
                            // 调用呼叫接口，本 Applet 会被挂起，返回时回到当前状态
                            WMSExtApp_CallPhoneNumber(pMe, phoneNumber, FALSE);
                        }                
                        else
                        {
                            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                                EVTFLG_ASYNC,
                                                AEECLSID_WMSAPP,
                                                EVT_USER_REDRAW,
                                                0, 
                                                0);                    
                        }
                        return TRUE;
                    }
                #ifdef FEATURE_VERSION_W208S
                // 添加到黑名单
                case IDS_ADD_TO_RESTRICT_LIST:
                    {
                        uint8 byMax = 0;
                        AECHAR          phoneNumber[32 + 1];  
                        sms_restrict_recive_info		sms_restrict_reciveList[MAX_SMS_RESTRICT];
                        MSG_FATAL("IDD_VIEWMSG_MMS_Handler",0,0,0);
                        DBGPRINTF("pDecdata->message.hFrom=%s", (char*)(pDecdata->message.hFrom));
                        STRTOWSTR((char*)(pDecdata->message.hFrom), phoneNumber, sizeof(phoneNumber));
                        DBGPRINTF("phoneNumber=%S", phoneNumber);
                        if (WSTRLEN(phoneNumber) > 0)
                        {
                            sms_restrict_recive_info info = {0};
                            
                            (void) ICONFIG_GetItem(pMe->m_pConfig,
                            CFGI_SMS_RESTRICT_RECEIVE_TOTAL,
                            &byMax,
                            sizeof(byte));  
                            
                        	//初始化拒收短信黑名单的信息
                        	(void) ICONFIG_GetItem(pMe->m_pConfig,
                        						   CFGI_SMS_RESTRICT_RECEIVE_INFO,
                        						   (void*)sms_restrict_reciveList,
                        						   sizeof(sms_restrict_reciveList));           
                            
                            MEMCPY(sms_restrict_reciveList[byMax].szName, phoneNumber, sizeof(sms_restrict_reciveList[byMax].szName));
                            byMax++;
                           (void) ICONFIG_SetItem(pMe->m_pConfig,
                                                  CFGI_SMS_RESTRICT_RECEIVE_TOTAL,
                                                  &byMax,
                                                  sizeof(uint8));
                           
                           (void) ICONFIG_SetItem(pMe->m_pConfig,
                                                  CFGI_SMS_RESTRICT_RECEIVE_INFO,
                                                  (void*) sms_restrict_reciveList,
                                                  sizeof(sms_restrict_reciveList));                           
                        }

                    }
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;  
                #endif
                //保存
                case IDS_SAVE_CURRENT_ITEM:  
                    {
                        char FilePath[MMS_MAX_CONTENT_NAME];  
                        char* pMimeType = NULL;
                        IFile* pIFile = NULL;
            		    IFileMgr *pIFileMgr = NULL; 
                        uint8 step = 1;
                        uint8 MenuSelectdId = IMENUCTL_GetSel(pMenuCtl);
            			int result = ISHELL_CreateInstance(AEE_GetShell(), AEECLSID_FILEMGR,(void **)&pIFileMgr);
                        MSG_FATAL("IDS_SAVE MenuSelectdId=%d", MenuSelectdId,0,0);
            			if (SUCCESS != result)
            		    {
            				MSG_FATAL("MRS: Open file error %x", result,0,0);
            				return TRUE;
            		    } 
                        if(pDecdata != NULL)
                        {
                            DBGPRINTF("MenuSelectdItem hContentType=%s", (const char*)pDecdata->message.mms_data.fragment[MenuSelectdId].hContentType);
                            pMimeType = MMS_WSP_MineType2MormalMimeType((const char*)pDecdata->message.mms_data.fragment[MenuSelectdId].hContentType);
                            if(pMimeType != NULL)
                            {
                                DBGPRINTF("pMimeType=%s", pMimeType);
                            }
                            if(STRISTR(pMimeType, IMAGE_MIME_BASE))
                            {
                                MSG_FATAL("IDS_SAVE IMAGE_MIME_BASE", 0,0,0);
                                if (SUCCESS != IFILEMGR_Test(pIFileMgr, MG_MASSCARDPICTURE_PATH))
                            	{
                            	    (void)IFILEMGR_MkDir(pIFileMgr, MG_MASSCARDPICTURE_PATH);
                            	}  
                                SPRINTF(FilePath,"%s%s",MG_MASSCARDPICTURE_PATH,(char*)(pDecdata->message.mms_data.fragment[MenuSelectdId].hContentName));
                                while(SUCCESS == IFILEMGR_Test(pIFileMgr, FilePath))
                                {
                                    MEMSET(FilePath, 0, sizeof(FilePath));
                                    SPRINTF(FilePath,"%s(%d)%s", MG_MASSCARDPICTURE_PATH, step++,(char*)(pDecdata->message.mms_data.fragment[MenuSelectdId].hContentName));                                
                                }  
                            }
                            else if(STRISTR(pMimeType, SOUND_MIME_BASE))
                            {
                                MSG_FATAL("IDS_SAVE SOUND_MIME_BASE", 0,0,0);
                                if (SUCCESS != IFILEMGR_Test(pIFileMgr, MG_MASSCARDMUSIC_PATH))
                            	{
                            	    (void)IFILEMGR_MkDir(pIFileMgr, MG_MASSCARDMUSIC_PATH);
                            	}                              
                                SPRINTF(FilePath,"%s%s",MG_MASSCARDMUSIC_PATH,(char*)(pDecdata->message.mms_data.fragment[MenuSelectdId].hContentName));
                                while(SUCCESS == IFILEMGR_Test(pIFileMgr, FilePath))
                                {
                                    MEMSET(FilePath, 0, sizeof(FilePath));
                                    SPRINTF(FilePath,"%s(%d)%s", MG_MASSCARDMUSIC_PATH,step++,(char*)(pDecdata->message.mms_data.fragment[MenuSelectdId].hContentName));                                
                                }  
                            }   
                            else if(STRISTR(pMimeType, VIDEO_MIME_BASE))
                            {
                                MSG_FATAL("IDS_SAVE VIDEO_MIME_BASE", 0,0,0);
                                if (SUCCESS != IFILEMGR_Test(pIFileMgr, MG_MASSCARDVIDEO_PATH))
                            	{
                            	    (void)IFILEMGR_MkDir(pIFileMgr, MG_MASSCARDVIDEO_PATH);
                            	}                              
                                SPRINTF(FilePath,"%s/%s",MG_MASSCARDVIDEO_PATH,(char*)(pDecdata->message.mms_data.fragment[MenuSelectdId].hContentName));
                                while(SUCCESS == IFILEMGR_Test(pIFileMgr, FilePath))
                                {
                                    MEMSET(FilePath, 0, sizeof(FilePath));
                                    SPRINTF(FilePath,"%s(%d)%s", MG_MASSCARDVIDEO_PATH,step++,(char*)(pDecdata->message.mms_data.fragment[MenuSelectdId].hContentName));                                
                                }  
                            }                              
                            else
                            {
                                MSG_FATAL("IDS_SAVE SOUND_MIME_BASE", 0,0,0);
                                if (SUCCESS != IFILEMGR_Test(pIFileMgr, MG_MASSCARDDOWNLOAD_PATH))
                            	{
                            	    (void)IFILEMGR_MkDir(pIFileMgr, MG_MASSCARDDOWNLOAD_PATH);
                            	}                              
                                SPRINTF(FilePath,"%s%s",MG_MASSCARDDOWNLOAD_PATH,(char*)(pDecdata->message.mms_data.fragment[MenuSelectdId].hContentName));
                                while(SUCCESS == IFILEMGR_Test(pIFileMgr, FilePath))
                                {
                                    MEMSET(FilePath, 0, sizeof(FilePath));
                                    SPRINTF(FilePath,"%s(%d)%s", MG_MASSCARDDOWNLOAD_PATH, step++,(char*)(pDecdata->message.mms_data.fragment[MenuSelectdId].hContentName));                                
                                }                                  
                            }
                            DBGPRINTF("FilePath=%s",FilePath);
                            MSG_FATAL("IDS_SAVE  1", 0,0,0);
                            result = EBADPARM;
                            pIFile = IFILEMGR_OpenFile( pIFileMgr, FilePath, _OFM_CREATE);
                            result = IFILEMGR_GetLastError(pIFileMgr);
                            if ( pIFile != NULL )
                            {
                                MSG_FATAL("IDS_SAVE  pIFile != NULL ", 0,0,0);
                                IFILE_Write( pIFile, pDecdata->message.mms_data.fragment[MenuSelectdId].pContent, pDecdata->message.mms_data.fragment[MenuSelectdId].size);
                                result = IFILEMGR_GetLastError(pIFileMgr);
                                MSG_FATAL("size=%d",pDecdata->message.mms_data.fragment[MenuSelectdId].size,0,0);
                                IFILE_Release( pIFile);
                                pIFile = NULL;
                                IFILEMGR_Release(pIFileMgr);
                                pIFileMgr = NULL;
                            }
                            MSG_FATAL("IDS_SAVE  2", 0,0,0);
                        }
                        MSG_FATAL("IFILEMGR_GetLastError =%d", result,0,0);
                        if(result == SUCCESS)
                        {
                            CLOSE_DIALOG(DLGRET_SAVE)
                        }
                        else if(result == EFSFULL)
                        {
                            CLOSE_DIALOG(DLGRET_EFSFULL_MMS)
                        }
                    }
                    return TRUE;

                // 删除
                case IDS_DELETE:  
                	CLOSE_DIALOG(DLGRET_DELETE)
                    return TRUE;;

                // 删除全部
                case IDS_DELETEALL:
                    CLOSE_DIALOG(DLGRET_CLEARALL)
                    return TRUE;                    
                default:
                    break;
            }

        case EVT_DISPLAYDIALOGTIMEOUT:
            MSG_FATAL("IDD_VIEWMSG_MMS_Handler",0,0,0);
            CLOSE_DIALOG(DLGRET_MSGBOX_OK)
            return TRUE;
            
        default:
            break;
    }
    return FALSE;
} // IDD_VIEWMSG_MMS_Handler


static IImage* WmsLoadImageFromData(WmsApp *pMe,int nFragIndex,char* pMimeType)
{
    uint8* pData = NULL;
    int rSize = 0;
    WSP_MMS_DATA* pMmsData = &pMe->m_DecData.message.mms_data;
    IImage* pIImageCur = NULL;

    MSG_FATAL("[WmsLoadImageFromData] Enter", 0, 0, 0);
    rSize = pMmsData->fragment[nFragIndex].size;
    pData = (uint8 *)MALLOC((rSize*sizeof(uint8)));
    DBGPRINTF("WMS Address = 0x%x", pData); 
    MEMSET((void*)pData, NULL, rSize*sizeof(uint8));
    MEMCPY(pData,(void*)pMmsData->fragment[nFragIndex].pContent , rSize);
    
    if(NULL != pData)
    {
        AEECLSID cls;
        IMemAStream* pMemAStream = NULL;
        
        MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] pMimeType:%s", pMimeType, 0, 0);
        cls = ISHELL_GetHandler(pMe->m_pShell,
            HTYPE_VIEWER,
            pMimeType);

        MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] clsid:%d", cls, 0, 0);
        if(!ISHELL_CreateInstance(pMe->m_pShell,cls,(void**)&pIImageCur))
        {
            MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] Create SUCCESS", 0, 0, 0);
            if(!ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_MEMASTREAM,(void**)&pMemAStream))
            {
                AEEImageInfo info;
                AEERect rc;

                MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] AEECLSID_MEMASTREAM Build", 0, 0, 0);
                IMEMASTREAM_Set(pMemAStream,pData,rSize,0,FALSE);
                IImage_SetStream(pIImageCur,(IAStream*)pMemAStream);
            }
        }
        if(pMemAStream != NULL)
        {
            DBGPRINTF("pMemAStream ADDRESS=0x%X", pMemAStream);
            IMEMASTREAM_Release(pMemAStream);
        }
        else
        {
            DBGPRINTF("pData ADDRESS=0x%X", pData);
            FREEIF(pData);
        }
    }
    MSG_FATAL("[WmsLoadImageFromData] Exit", 0, 0, 0);
    return pIImageCur;
}

static void WmsLoadSoundFromData(WmsApp *pMe,int nFragIndex,char* pMimeType)
{
    
    uint32 rSize = 0;
    WSP_MMS_DATA* pMmsData = &pMe->m_DecData.message.mms_data;
    AEECLSID cls = 0;
    AEEMediaData mediaData = {0};
    int nResult = SUCCESS;
    
    MSG_FATAL("[WmsLoadSoundFromData] Enter", 0, 0, 0);
    rSize = pMmsData->fragment[nFragIndex].size;

    
    DBGPRINTF("[IDD_VIEWMSG_MMS_Handler] pMimeType:%s, rSize=%d", pMimeType, rSize);

    cls = ISHELL_GetHandler(pMe->m_pShell,
        HTYPE_SOUND,
        pMimeType);

    if(cls == 0)
    {
        MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] No Class id", 0, 0, 0);
    }

    mediaData.clsData = MMD_BUFFER;
    mediaData.pData = pMmsData->fragment[nFragIndex].pContent;
    DBGPRINTF("mediaData.pData ADDRESS=0x%X",mediaData.pData);
    mediaData.dwSize = rSize;
    DBGPRINTF("pMe->m_pMedia ADDRESS=0x%X",pMe->m_pMedia);
    RELEASEIF(pMe->m_pMedia);

    nResult = AEEMediaUtil_CreateMedia(pMe->m_pShell,&mediaData,&pMe->m_pMedia);
    DBGPRINTF("pMe->m_pMedia ADDRESS=0x%X",pMe->m_pMedia);
    if(nResult != SUCCESS)
    {
        MSG_FATAL("[IDD_VIEWMSG_MMS_Handler] CreateMedia Error:%s", nResult, 0, 0);
    }
    MSG_FATAL("[WmsLoadSoundFromData] Exit", 0, 0, 0);
    
}

boolean WmsApp_SaveToFile(char* pFileName,void* pData,uint32 nDataLen)
{                            
    IFileMgr* pFileMgr = NULL;
    IFile* pFile = NULL;
    int nResult = SUCCESS;

    if(NULL == pData || nDataLen == 0)
        return EFAILED;
        
    if(!(nResult = ISHELL_CreateInstance(AEE_GetShell(),AEECLSID_FILEMGR,(void**)&pFileMgr)))
    {   
        if(IFILEMGR_Test(pFileMgr,pFileName))
        {
            pFile = IFILEMGR_OpenFile(pFileMgr,pFileName,_OFM_READWRITE);
            MSG_FATAL("[WmsApp_SaveToFile] File Exist",0,0,0);
        }
        else
        {
            pFile = IFILEMGR_OpenFile(pFileMgr,pFileName,_OFM_CREATE);  
            MSG_FATAL("[WmsApp_SaveToFile] File Not Exist",0,0,0);
        }
        
        if(pFile)
        {
            int nDataCount = 0;
            nDataCount = IFILE_Write(pFile,pData,nDataLen);
            MSG_FATAL("[WmsApp_SaveToFile] Insert Data Count:%d",nDataCount,0,0);
        } 
        else
        {
            nResult = IFILEMGR_GetLastError(pFileMgr);
            MSG_FATAL("[WmsApp_SaveToFile] Error:%d",nResult,0,0);
        }
    }
    RELEASEIF(pFile);
    RELEASEIF(pFileMgr);

    return (SUCCESS == nResult);
}

int AddMimeResIntoMms(WmsApp *pMe,char* pPath)
{
    WSP_MMS_ENCODE_DATA* pEncData = NULL;
    char* pFilePath = NULL;    
    int nIndex = 0;
    uint32 size = 0;
    char* pFileType = NULL; 
    
    if(NULL == pMe)
    {
        MSG_FATAL("[AddMimeResIntoMms] Init Error",0,0,0);
        return  -1;
    }   

    if(NULL == pMe->m_EncData.pMessage)
    {
        pMe->m_EncData.pMessage = (MMS_WSP_MESSAGE_SEND*)MALLOC(sizeof(MMS_WSP_MESSAGE_SEND));
        DBGPRINTF("WMS Address = 0x%x", pMe->m_EncData.pMessage); 
        MSG_FATAL("[AddMimeResIntoMms] NEW MMS MSG",0,0,0);
    }

    if(NULL == pMe->m_EncData.pMessage)
    {
        MSG_FATAL("[AddMimeResIntoMms] No Memory",0,0,0);
        return -1;
    }    
    nIndex = MimeResCheckFileExist(pMe,pPath);
    
    if(nIndex != -1)
    {
        return nIndex;
    }
    else
    {
        nIndex = 1;//0;文字资源
    }
        
    pEncData = &pMe->m_EncData.pMessage->mms_data;
    pFilePath = (char*)pEncData->fragment[nIndex].hContentFile;
    while(nIndex < WMSMMS_FRAGMENTCOUNT)
    {
        pFilePath = (char*)pEncData->fragment[nIndex].hContentFile;
        
        if(STRLEN(pFilePath) == 0)
        {
            STRCPY(pFilePath, pPath);
            MSG_FATAL("[AddMimeResIntoMms] Add Success",0,0,0);
            return nIndex;   
        }
        nIndex ++;
    }
    MSG_FATAL("[AddMimeResIntoMms] Add Failed",0,0,0);
    return  -1;
}
int DeleteMimeResFromMms(WmsApp *pMe,char* pPath)
{
    WSP_MMS_ENCODE_DATA* pEncData = NULL;
    char* pFilePath = NULL;    
    int nIndex = 1;//0; 零为文本
    uint32 size = 0;
    char* pFileType = NULL; 
    
    if(NULL == pMe || !pMe->m_EncData.pMessage)
    {
        MSG_FATAL("[DeleteMimeResFromMms] Init Error",0,0,0);
        return  -1;
    }    
        
    pEncData = &pMe->m_EncData.pMessage->mms_data;
    pFilePath = (char*)pEncData->fragment[nIndex].hContentFile;
    while(nIndex < WMSMMS_FRAGMENTCOUNT)
    {
        pFilePath = (char*)pEncData->fragment[nIndex].hContentFile;
        
        if(STRCMP(pPath,pFilePath) == 0)
        {
            MEMSET(pFilePath,NULL,STRLEN(pFilePath));
            FREEIF(pEncData->fragment[nIndex].pBuf);
            MSG_FATAL("[DeleteMimeResFromMms] Delete Success",0,0,0);
            return nIndex;   
        }
        nIndex ++;
    }
    MSG_FATAL("[DeleteMimeResFromMms] Not Find File",0,0,0);
    return  -1;
}
int MimeResCheckFileExist(WmsApp *pMe,char* pPath)
{
    WSP_MMS_ENCODE_DATA* pEncData = NULL;
    char* pFilePath = NULL;    
    int nIndex = 1;//0; 0为文本
    uint32 size = 0;
    char* pFileType = NULL; 
    
    if(NULL == pMe || !pMe->m_EncData.pMessage)
    {
        MSG_FATAL("[MimeResCheckFileExist] Init Error",0,0,0);
        return  -1;
    }    
        
    pEncData = &pMe->m_EncData.pMessage->mms_data;
    pFilePath = (char*)pEncData->fragment[nIndex].hContentFile;
    while(nIndex < WMSMMS_FRAGMENTCOUNT)
    {
        pFilePath = (char*)pEncData->fragment[nIndex].hContentFile;
        
        if(STRNCMP(pFilePath,pPath,STRLEN(pPath)) == 0)
        {
            return nIndex;   
        }
        nIndex ++;
    }
    MSG_FATAL("[MimeResCheckFileExist] Add Failed",0,0,0);
    return  -1;
}

char* MimeResCheckTypeExist(WmsApp *pMe,char* pType,uint8** ppBuf,uint32* pBufLen)
{
    WSP_MMS_ENCODE_DATA* pEncData = NULL;
    char* pFilePath = NULL;    
    int nIndex = 1;//0;零为文本
    uint32 size = 0;
    int result = 0;
    boolean isNotInBREWMineType = FALSE;
    MSG_FATAL("[MimeResCheckTypeExist] Enter",0,0,0);
    
    if(NULL == pMe || !pMe->m_EncData.pMessage)
    {
        MSG_FATAL("[MimeResCheckTypeExist] Init Error",0,0,0);
        return NULL;
    }    
        
    pEncData = &pMe->m_EncData.pMessage->mms_data;
    pFilePath = (char*)pEncData->fragment[nIndex].hContentFile;
    DBGPRINTF("pFilePath 1:%s",pFilePath);
    if(pFilePath == NULL)
    {
        DBGPRINTF("pFilePath == NULL");
        return NULL;
    }
    while(STRLEN(pFilePath) != 0 && nIndex < WMSMMS_FRAGMENTCOUNT)
    {
        isNotInBREWMineType = FALSE;
        pFilePath = (char*)pEncData->fragment[nIndex].hContentFile;
        DBGPRINTF("pFilePath 2:%s, pType=%s",pFilePath, pType);
        if(STRSTR(MMS_GetMimeType(pFilePath), pType) == NULL)
        {
            isNotInBREWMineType = TRUE;
        }
        DBGPRINTF("isNotInBREWMineType:%d",isNotInBREWMineType);
        if(MMS_GetMimeType(pFilePath) != NULL)
        {
            DBGPRINTF("pFileType 2:%s",MMS_GetMimeType(pFilePath));
        }
        else
        {
            DBGPRINTF("pFileType is NULL");
        }
        if(NULL == pType && isNotInBREWMineType)
        {
            if(pEncData->fragment[nIndex].pBuf && pEncData->fragment[nIndex].nBufLen > 0)
            {
                if(ppBuf == NULL || pBufLen == NULL)
                {
                    nIndex ++;
                    continue;
                }    
                    
                *pBufLen = pEncData->fragment[nIndex].nBufLen;
                *ppBuf = pEncData->fragment[nIndex].pBuf;
            }
            DBGPRINTF("[MimeResCheckTypeExist] Find File:%s",pFilePath);
            return pFilePath;   
        }
        else if(STRSTR(MMS_GetMimeType(pFilePath), pType) != NULL)
        {
            if(pEncData->fragment[nIndex].pBuf && pEncData->fragment[nIndex].nBufLen > 0)
            {
                if(ppBuf == NULL || pBufLen == NULL)
                {
                    nIndex ++;
                    continue;
                }    
                    
                *pBufLen = pEncData->fragment[nIndex].nBufLen;
                *ppBuf = pEncData->fragment[nIndex].pBuf;
            }
            DBGPRINTF("[MimeResCheckTypeExist] Find File:%s",pFilePath);
            return pFilePath;   
        }
        nIndex ++;
    }

    MSG_FATAL("[MimeResCheckTypeExist] Not Find",0,0,0);
    return NULL;
}

int WMSMMS_GetResByExplorer(void* pv, FileNamesBuf pBuf, uint32 nBufSize)
{
    WmsApp* pMe = (WmsApp*)pv;
    WSP_MMS_ENCODE_DATA* pData = NULL;
    int nIndex = 1;//0;零为文本
    if(NULL == pMe->m_EncData.pMessage)
    {
        pMe->m_EncData.pMessage = (MMS_WSP_MESSAGE_SEND*)MALLOC(sizeof(MMS_WSP_MESSAGE_SEND));
        DBGPRINTF("WMS Address = 0x%x", pMe->m_EncData.pMessage); 
    }

    if(NULL == pMe->m_EncData.pMessage)
        return EFAILED;
    
    pData = &pMe->m_EncData.pMessage->mms_data;

    if(pData->frag_num >= WMSMMS_FRAGMENTCOUNT)
        return EFAILED;

    while(nIndex < WMSMMS_FRAGMENTCOUNT)
    {
        if(STRLEN((char*)pData->fragment[nIndex].hContentFile) == 0)
        {
            pData->frag_num ++;
            STRNCPY((char*)pData->fragment[nIndex].hContentFile,(char*)pBuf,nBufSize);
            return SUCCESS;
        }
        nIndex++;
    }
    return EFAILED;
    
}

static void WMSMMS_MediaNotify(void * pUser, AEEMediaCmdNotify *pCmdNotify)
{
	WmsApp* pMe = (WmsApp*)pUser;
	int          nSpecSize = 0;
    MSG_FATAL("WMSMMS_MediaNotify nCmd=%d, nStatus=%d", pCmdNotify->nCmd, pCmdNotify->nStatus, 0);
	if (pCmdNotify->nCmd == MM_CMD_PLAY)
	{
		switch(pCmdNotify->nStatus)
		{
			case  MM_STATUS_START:                          
				pMe->m_CurrentState = PLAYER_PLAY;
				break;
                
			case MM_STATUS_PAUSE:		/* pause的回调 */
				pMe->m_CurrentState = PLAYER_PAUSE;
                break;
                
			case MM_STATUS_RESUME:		/* resume的回调 */     
				pMe->m_CurrentState = PLAYER_PLAY;
				break;

			case MM_STATUS_DONE:
                pMe->m_CurrentState = PLAYER_STOP;
				break;
                
			case MM_STATUS_ABORT:
    			pMe->m_CurrentState = PLAYER_PAUSE;
    			break;
                
			default:
				break;		
		}
        if (NULL == pMe->m_pMenu)
        {
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                EVTFLG_ASYNC,
                                AEECLSID_WMSAPP,
                                EVT_USER_REDRAW,
                                0, 
                                0);    
        }
	}
}




/*==============================================================================
函数:
    IDD_EDIT_ALBUMOREMAIN_Handler

说明:
    WMS Applet对话框 IDD_ALBUMOREMAIN 事件处理函数。用于短消息文本输处理。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/

static boolean IDD_EDIT_ALBUMOREMAIN_Handler(void *pUser, 
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
)
{
    ITextCtl *pIText = NULL;   
    AEETextInputMode nInputMode;
    static AEETextInputMode nMode;
	AECHAR Annstr[20] = {0};
    WmsApp *pMe = (WmsApp *)pUser;
    MSG_FATAL("IDD_EDIT_ALBUMOREMAIN_Handler Start eCode=0x%x, wParam=0x%x",eCode,wParam,0);
    if (NULL == pMe)
    {
        return FALSE;
    }
    pIText = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_EDIT_ALBUMOREMAIN);

    if (NULL == pIText)
    {
        return FALSE;
    }
    MSG_FATAL("IDD_EDIT_ALBUMOREMAIN_Handler Start 1",0,0,0);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        	{
                AECHAR WTitle[2] = {0};
                MSG_FATAL("IDD_EDIT_ALBUMOREMAIN_Handler EVT_DIALOG_INIT",0,0,0);
                IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
                SetControlRect(pMe, pIText);   
				#ifdef FEATURE_VERSION_C316
				ITEXTCTL_SetProperties(pIText, TP_GRAPHIC_BGBLUE|TP_GRAPHIC_BG|TP_FRAME | TP_MULTILINE | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT | TP_DISPLAY_SMSCOUNT | TP_NOUPDATE|TP_FOCUS_NOSEL);
				#else
                ITEXTCTL_SetProperties(pIText, TP_GRAPHIC_BG|TP_FRAME | TP_MULTILINE | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT | TP_DISPLAY_SMSCOUNT | TP_NOUPDATE|TP_FOCUS_NOSEL);
				#endif
                (void)ITEXTCTL_SetTitle( pIText, NULL,0,WTitle);
                ITEXTCTL_SetMaxSize(pIText, MAX_EMAILADD_LEN);              
				if(NULL != pMe->m_szEmail)
				{
                    DBGPRINTF("pMe->m_szEmail=%S", pMe->m_szEmail);
					ITEXTCTL_SetMaxSize ( pIText, MAX_EMAILADD_LEN+1);
                	(void)ITEXTCTL_SetText(pIText,pMe->m_szEmail,-1);
				}
	            return TRUE;
            }
        
        case EVT_DIALOG_START:         
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;

        case EVT_USER_REDRAW:  
            {
                int32 InsertPos = 0;
    			(void)ISHELL_LoadResString(pMe->m_pShell,
                            AEE_WMSAPPRES_LANGFILE,                                
                            IDS_EDIT,
                            Annstr,
                            sizeof(Annstr));
                
    			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,Annstr);
                InsertPos = ITEXTCTL_GetCursorPos(pIText);
                DBGPRINTF("EVT_USER_REDRAW InsertPos=%d",InsertPos);
                if (InsertPos == 0)
                {
                    ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
                }
                else
                {
                    ITEXTCTL_SetCursorPos(pIText, InsertPos);
                }
                ITEXTCTL_SetActive(pIText, TRUE);  
                ITEXTCTL_Redraw(pIText);
                // 绘制底部操作提示条
                DRAW_BOTTOMBAR(BTBAR_OK_BACK)
                IDISPLAY_Update(pMe->m_pDisplay);  
#ifdef FEATURE_LCD_TOUCH_ENABLE
                TSIM_NumberKeypad(FALSE);
#endif
            }
            return TRUE; 

        case EVT_CTL_TEXT_MODECHANGED:  //切换输入法    这里增加画底条，否则从符号输入界面返回时无底条
            // 绘制底部操作提示条
            {
                nInputMode = ITEXTCTL_GetInputMode(pIText,NULL);
                if (nInputMode != AEE_TM_SYMBOLS && nInputMode != AEE_TM_FACE_SYMBOL)
                {
                    DRAW_BOTTOMBAR(BTBAR_OK_BACK)
                    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                }
            }
            return TRUE; 
            

        case EVT_DIALOG_END:
            {
                AECHAR *pwsText = ITEXTCTL_GetTextPtr(pIText);
                CMultiSendItemInfo *pItem = NULL;
                MSG_FATAL("IDD_EDIT_ALBUMOREMAIN_Handler EVT_DIALOG_END m_CurAddID=%d",pMe->m_CurAddID,0,0);  
                
                pItem = WmsApp_GetAddItem(pMe->m_pSendList, (pMe->m_CurAddID-MSG_CMD_BASE));
                if(pItem == NULL)
                {
                    pItem = (CMultiSendItemInfo *)sys_malloc(sizeof(CMultiSendItemInfo));
                    DBGPRINTF("WMS Address = 0x%x", pItem); 
                    if (NULL != pItem)
                    {// 缓冲区分配成功
                        MSG_FATAL("IDD_EDIT_ALBUMOREMAIN_Handler IVector_AddElement",0,0,0);
                        if (SUCCESS != IVector_AddElement(pMe->m_pSendList, pItem))
                        {
                            sys_free(pItem);
                            return TRUE;
                        }
                        else
                        {
                            MSG_FATAL("IDD_EDIT_ALBUMOREMAIN_Handler IVector_AddElement SUCCESS",0,0,0);
                            (void)ITEXTCTL_GetText(pIText, pItem->m_szEmail, MAX_EMAILADD_LEN + 1);
                            (void)ITEXTCTL_GetText(pIText, pItem->m_szTo, MAX_EMAILADD_LEN + 1);
                        }
                    }
                }
                else
                {
                    MSG_FATAL("IDD_EDIT_ALBUMOREMAIN_Handler pItem != NULL",0,0,0);
                    (void)ITEXTCTL_GetText(pIText, pItem->m_szEmail, MAX_EMAILADD_LEN + 1);
                    (void)ITEXTCTL_GetText(pIText, pItem->m_szTo, MAX_EMAILADD_LEN + 1);                    
                }
            }			
			
            return TRUE;

        case EVT_KEY:
            MSG_FATAL("IDD_EDIT_ALBUMOREMAIN_Handler EVT_KEY",0,0,0);
            switch (wParam)
            {
                case AVK_CLR:
                    MSG_FATAL("IDD_EDIT_ALBUMOREMAIN_Handler AVK_CLR 1",0,0,0);
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                case AVK_INFO:
                case AVK_SELECT:
                    MSG_FATAL("AVK_SELECT ",0,0,0);
                    CLOSE_DIALOG(DLGRET_MSGBOX_OK)
                    return TRUE;
                      
                default:
                    break;
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
	                MSG_FATAL("IDD_EDIT_ALBUMOREMAIN_Handler EVT_PEN_UP wXPos=%d, wYPos=%d",wXPos,wYPos,0);
					nBarH = GetBottomBarHeight(pMe->m_pDisplay);
			
					MEMSET(&devinfo, 0, sizeof(devinfo));
					ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
					SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
	
					if(WMSAPP_PT_IN_RECT(wXPos,wYPos,rc))
					{
                        MSG_FATAL("IDD_WRITEMSG_Handler EVT_PEN_UP WMSAPP_PT_IN_RECT",0,0,0);
						if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
						{
                            MSG_FATAL("IDD_WRITEMSG_Handler EVT_PEN_UP left",0,0,0);
							return IDD_EDIT_ALBUMOREMAIN_Handler((void *)pMe,EVT_KEY,AVK_SELECT,0);
						}
						else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
						{					
							boolean rt;
							int len = WSTRLEN(ITEXTCTL_GetTextPtr(pIText));
	                        MSG_FATAL("IDD_WRITEMSG_Handler EVT_PEN_UP left len=%d",len,0,0);
							if((ITEXTCTL_IsActive(pIText)) && (len > 0))
								return ITEXTCTL_HandleEvent(pIText,EVT_KEY,AVK_CLR,0);
							else
                                return IDD_EDIT_ALBUMOREMAIN_Handler((void *)pMe, EVT_KEY, AVK_CLR, 0);
						}
					}
	
				}
				return TRUE;
#endif 

        default:
            break;            
    }

    return FALSE;
} // IDD_WRITEMSG_Handler

/*==============================================================================
函数:
    IDD_MMS_SERVER_ADDRESS_Handler

说明:
    WMS Applet对话框 IDD_MMS_SERVER_ADDRESS_Handler 事件处理函数。用于短消息文本输处理。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/

static boolean IDD_MMS_SERVER_ADDRESS_Handler(void *pUser, 
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
)
{
    ITextCtl *pIText = NULL;   
    AEETextInputMode nInputMode;
    static AEETextInputMode nMode;
	AECHAR Annstr[20] = {0};
    WmsApp *pMe = (WmsApp *)pUser;
    MSG_FATAL("IDD_MMS_SERVER_ADDRESS_Handler Start eCode=0x%x, wParam=0x%x",eCode,wParam,0);
    if (NULL == pMe)
    {
        return FALSE;
    }
    pIText = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_MMS_SERVER_ADDRESS);

    if (NULL == pIText)
    {
        return FALSE;
    }
    MSG_FATAL("IDD_MMS_SERVER_ADDRESS_Handler Start 1",0,0,0);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        	{
                AECHAR WTitle[2] = {0};
                AECHAR MMSServerAddress[MAX_URL_LEN] = {0};
                char temp[MAX_URL_LEN] = {0};
                MSG_FATAL("IDD_MMS_SERVER_ADDRESS_Handler EVT_DIALOG_INIT",0,0,0);
                IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
                SetControlRect(pMe, pIText);       
                ITEXTCTL_SetProperties(pIText, TP_GRAPHIC_BG|TP_FRAME | TP_MULTILINE | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT | TP_DISPLAY_SMSCOUNT | TP_NOUPDATE|TP_FOCUS_NOSEL);
                (void)ITEXTCTL_SetTitle( pIText, NULL,0,WTitle);
                ITEXTCTL_SetMaxSize(pIText, MAX_URL_LEN);   
                ICONFIG_GetItem(pMe->m_pConfig, CFGI_MMS_SERVER_ADDRESS, temp, sizeof(temp));
				if(STRLEN(temp) != 0)
				{
                    DBGPRINTF("temp=%s", temp);
                    STRTOWSTR(temp, MMSServerAddress, sizeof(MMSServerAddress));
                	(void)ITEXTCTL_SetText(pIText,MMSServerAddress,-1);
				}
	            return TRUE;
            }
        
        case EVT_DIALOG_START:         
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;

        case EVT_USER_REDRAW:  
            {
                int32 InsertPos = 0;
    			(void)ISHELL_LoadResString(pMe->m_pShell,
                            AEE_WMSAPPRES_LANGFILE,                                
                            IDS_EDIT,
                            Annstr,
                            sizeof(Annstr));
                
    			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,Annstr);
                InsertPos = ITEXTCTL_GetCursorPos(pIText);
                DBGPRINTF("EVT_USER_REDRAW InsertPos=%d",InsertPos);
                if (InsertPos == 0)
                {
                    ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
                }
                else
                {
                    ITEXTCTL_SetCursorPos(pIText, InsertPos);
                }
                ITEXTCTL_SetActive(pIText, TRUE);  
                ITEXTCTL_Redraw(pIText);
                // 绘制底部操作提示条
                DRAW_BOTTOMBAR(BTBAR_OK_BACK)
                IDISPLAY_Update(pMe->m_pDisplay);  
#ifdef FEATURE_LCD_TOUCH_ENABLE
                TSIM_NumberKeypad(FALSE);
#endif
            }
            return TRUE; 

        case EVT_CTL_TEXT_MODECHANGED:  //切换输入法    这里增加画底条，否则从符号输入界面返回时无底条
            // 绘制底部操作提示条
            {
                nInputMode = ITEXTCTL_GetInputMode(pIText,NULL);
                if (nInputMode != AEE_TM_SYMBOLS && nInputMode != AEE_TM_FACE_SYMBOL)
                {
                    DRAW_BOTTOMBAR(BTBAR_OK_BACK)
                    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                }
            }
            return TRUE; 
            

        case EVT_DIALOG_END:
            {
                char MMSServerAddress[MAX_URL_LEN] = {0};
                AECHAR *pwsText = ITEXTCTL_GetTextPtr(pIText);
                WSTRTOSTR(pwsText, MMSServerAddress, MAX_URL_LEN);
                ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMS_SERVER_ADDRESS,MMSServerAddress, sizeof(MMSServerAddress)); 
                MSG_FATAL("IDD_MMS_SERVER_ADDRESS_Handler EVT_DIALOG_END 0",0,0,0);  
            }
            return TRUE;

        case EVT_KEY:
            MSG_FATAL("IDD_MMS_SERVER_ADDRESS_Handler EVT_KEY",0,0,0);
            switch (wParam)
            {
                case AVK_CLR:
                    MSG_FATAL("IDD_MMS_SERVER_ADDRESS_Handler AVK_CLR 1",0,0,0);
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                case AVK_INFO:
                case AVK_SELECT:
                    MSG_FATAL("AVK_SELECT ",0,0,0);
                    CLOSE_DIALOG(DLGRET_MSGBOX_OK)
                    return TRUE;
                      
                default:
                    break;
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
	                MSG_FATAL("IDD_MMS_SERVER_ADDRESS_Handler EVT_PEN_UP wXPos=%d, wYPos=%d",wXPos,wYPos,0);
					nBarH = GetBottomBarHeight(pMe->m_pDisplay);
			
					MEMSET(&devinfo, 0, sizeof(devinfo));
					ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
					SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
	
					if(WMSAPP_PT_IN_RECT(wXPos,wYPos,rc))
					{
                        MSG_FATAL("IDD_MMS_SERVER_ADDRESS_Handler EVT_PEN_UP WMSAPP_PT_IN_RECT",0,0,0);
						if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
						{
                            MSG_FATAL("IDD_MMS_SERVER_ADDRESS_Handler EVT_PEN_UP left",0,0,0);
							return IDD_MMS_SERVER_ADDRESS_Handler((void *)pMe,EVT_KEY,AVK_SELECT,0);
						}
						else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
						{					
							boolean rt;
							int len = WSTRLEN(ITEXTCTL_GetTextPtr(pIText));
	                        MSG_FATAL("IDD_MMS_SERVER_ADDRESS_Handler EVT_PEN_UP left len=%d",len,0,0);
							if((ITEXTCTL_IsActive(pIText)) && (len > 0))
								return ITEXTCTL_HandleEvent(pIText,EVT_KEY,AVK_CLR,0);
							else
                                return IDD_MMS_SERVER_ADDRESS_Handler((void *)pMe, EVT_KEY, AVK_CLR, 0);
						}
					}
	
				}
				return TRUE;
#endif 

        default:
            break;            
    }

    return FALSE;
} // IDD_MMS_SERVER_ADDRESS_Handler

/*==============================================================================
函数:
    IDD_MMS_PROXY_Handler

说明:
    WMS Applet对话框 IDD_MMS_PROXY_Handler 事件处理函数。用于短消息文本输处理。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/

static boolean IDD_MMS_PROXY_Handler(void *pUser, 
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
)
{
    ITextCtl *pIText = NULL;   
    AEETextInputMode nInputMode;
    static AEETextInputMode nMode;
	AECHAR Annstr[20] = {0};
    WmsApp *pMe = (WmsApp *)pUser;
    MSG_FATAL("IDD_MMS_PROXY_Handler Start eCode=0x%x, wParam=0x%x",eCode,wParam,0);
    if (NULL == pMe)
    {
        return FALSE;
    }
    pIText = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_MMS_PROXY);

    if (NULL == pIText)
    {
        return FALSE;
    }
    MSG_FATAL("IDD_MMS_PROXY_Handler Start 1",0,0,0);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        	{
                AECHAR WTitle[2] = {0};
                AECHAR MMSProxy[MAX_MMS_PROXY] = {0};
                char temp[MAX_MMS_PROXY] = {0};
                MSG_FATAL("IDD_MMS_PROXY_Handler EVT_DIALOG_INIT",0,0,0);
                IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
                SetControlRect(pMe, pIText);       
                ITEXTCTL_SetProperties(pIText, TP_GRAPHIC_BG|TP_FRAME | TP_MULTILINE | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT | TP_DISPLAY_SMSCOUNT | TP_NOUPDATE|TP_FOCUS_NOSEL);
                (void)ITEXTCTL_SetTitle( pIText, NULL,0,WTitle);
                ITEXTCTL_SetMaxSize(pIText, MAX_MMS_PROXY);   
                ITEXTCTL_SetInputMode(pIText, AEE_TM_NUMBERS);  
                ICONFIG_GetItem(pMe->m_pConfig, CFGI_MMS_PROXY, temp, sizeof(temp));
				if(STRLEN(temp) != 0)
				{
                    DBGPRINTF("temp=%s", temp);
                    STRTOWSTR(temp, MMSProxy, sizeof(MMSProxy));
                	(void)ITEXTCTL_SetText(pIText,MMSProxy,-1);
				}
	            return TRUE;
            }
        
        case EVT_DIALOG_START:         
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;

        case EVT_USER_REDRAW:  
            {
                int32 InsertPos = 0;
    			(void)ISHELL_LoadResString(pMe->m_pShell,
                            AEE_WMSAPPRES_LANGFILE,                                
                            IDS_EDIT,
                            Annstr,
                            sizeof(Annstr));
                
    			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,Annstr);
                InsertPos = ITEXTCTL_GetCursorPos(pIText);
                DBGPRINTF("EVT_USER_REDRAW InsertPos=%d",InsertPos);
                if (InsertPos == 0)
                {
                    ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
                }
                else
                {
                    ITEXTCTL_SetCursorPos(pIText, InsertPos);
                }
                ITEXTCTL_SetActive(pIText, TRUE);  
                ITEXTCTL_Redraw(pIText);
                // 绘制底部操作提示条
                DRAW_BOTTOMBAR(BTBAR_OK_BACK)
                IDISPLAY_Update(pMe->m_pDisplay);  
#ifdef FEATURE_LCD_TOUCH_ENABLE
                TSIM_NumberKeypad(FALSE);
#endif
            }
            return TRUE; 

        case EVT_CTL_TEXT_MODECHANGED:  //切换输入法    这里增加画底条，否则从符号输入界面返回时无底条
            // 绘制底部操作提示条
            {
                nInputMode = ITEXTCTL_GetInputMode(pIText,NULL);
                if (nInputMode != AEE_TM_SYMBOLS && nInputMode != AEE_TM_FACE_SYMBOL)
                {
                    DRAW_BOTTOMBAR(BTBAR_OK_BACK)
                    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                }
            }
            return TRUE; 
            

        case EVT_DIALOG_END:
            {
                char MMSProxy[MAX_MMS_PROXY] = {0};
                AECHAR *pwsText = ITEXTCTL_GetTextPtr(pIText);
                WSTRTOSTR(pwsText, MMSProxy, MAX_MMS_PROXY);
                ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMS_PROXY,MMSProxy, sizeof(MMSProxy)); 
                MSG_FATAL("IDD_MMS_PROXY_Handler EVT_DIALOG_END 0",0,0,0);  
            }
            return TRUE;

        case EVT_KEY:
            MSG_FATAL("IDD_MMS_PROXY_Handler EVT_KEY",0,0,0);
            switch (wParam)
            {
                case AVK_CLR:
                    MSG_FATAL("IDD_MMS_PROXY_Handler AVK_CLR 1",0,0,0);
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                case AVK_INFO:
                case AVK_SELECT:
                    MSG_FATAL("AVK_SELECT ",0,0,0);
                    CLOSE_DIALOG(DLGRET_MSGBOX_OK)
                    return TRUE;
                      
                default:
                    break;
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
	                MSG_FATAL("IDD_MMS_PROXY_Handler EVT_PEN_UP wXPos=%d, wYPos=%d",wXPos,wYPos,0);
					nBarH = GetBottomBarHeight(pMe->m_pDisplay);
			
					MEMSET(&devinfo, 0, sizeof(devinfo));
					ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
					SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
	
					if(WMSAPP_PT_IN_RECT(wXPos,wYPos,rc))
					{
                        MSG_FATAL("IDD_MMS_PROXY_Handler EVT_PEN_UP WMSAPP_PT_IN_RECT",0,0,0);
						if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
						{
                            MSG_FATAL("IDD_MMS_PROXY_Handler EVT_PEN_UP left",0,0,0);
							return IDD_MMS_PROXY_Handler((void *)pMe,EVT_KEY,AVK_SELECT,0);
						}
						else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
						{					
							boolean rt;
							int len = WSTRLEN(ITEXTCTL_GetTextPtr(pIText));
	                        MSG_FATAL("IDD_MMS_PROXY_Handler EVT_PEN_UP left len=%d",len,0,0);
							if((ITEXTCTL_IsActive(pIText)) && (len > 0))
								return ITEXTCTL_HandleEvent(pIText,EVT_KEY,AVK_CLR,0);
							else
                                return IDD_MMS_PROXY_Handler((void *)pMe, EVT_KEY, AVK_CLR, 0);
						}
					}
	
				}
				return TRUE;
#endif 

        default:
            break;            
    }

    return FALSE;
} // IDD_MMS_PROXY_Handler

/*==============================================================================
函数:
    IDD_MMS_PORT_Handler

说明:
    WMS Applet对话框 IDD_MMS_PORT_Handler 事件处理函数。用于短消息文本输处理。

参数:
    pUser [in]: 指向WMS Applet对象结构的指针。该结构包含小程序的特定信息。
    eCode [in]: 事件代码。
    wParam[in]: 事件参数
    dwParam [in]: 与事件关联的数据。

返回值:
    TRUE:  传入事件得到处理。
    FALSE: 传入事件没被处理。

备注:

==============================================================================*/

static boolean IDD_MMS_PORT_Handler(void *pUser, 
    AEEEvent eCode,
    uint16 wParam, 
    uint32 dwParam
)
{
    ITextCtl *pIText = NULL;   
    AEETextInputMode nInputMode;
    static AEETextInputMode nMode;
	AECHAR Annstr[20] = {0};
    WmsApp *pMe = (WmsApp *)pUser;
    MSG_FATAL("IDD_MMS_PORT_Handler Start eCode=0x%x, wParam=0x%x",eCode,wParam,0);
    if (NULL == pMe)
    {
        return FALSE;
    }
    pIText = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveIDlg, IDC_MMS_PORT);

    if (NULL == pIText)
    {
        return FALSE;
    }
    MSG_FATAL("IDD_MMS_PORT_Handler Start 1",0,0,0);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        	{
                AECHAR WTitle[2] = {0};
                AECHAR MMSPort[MAX_MMS_PORT] = {0};
                char temp[MAX_MMS_PORT] = {0};
                MSG_FATAL("IDD_MMS_PORT_Handler EVT_DIALOG_INIT",0,0,0);
                IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
                SetControlRect(pMe, pIText);       
                ITEXTCTL_SetProperties(pIText, TP_GRAPHIC_BG|TP_FRAME | TP_MULTILINE | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT | TP_DISPLAY_SMSCOUNT | TP_NOUPDATE|TP_FOCUS_NOSEL);
                (void)ITEXTCTL_SetTitle( pIText, NULL,0,WTitle);
                ITEXTCTL_SetMaxSize(pIText, MAX_MMS_PORT);   
                ICONFIG_GetItem(pMe->m_pConfig, CFGI_MMS_PORT, temp, sizeof(temp));
				if(STRLEN(temp) != 0)
				{
                    DBGPRINTF("temp=%s", temp);
                    STRTOWSTR(temp, MMSPort, sizeof(MMSPort));
                	(void)ITEXTCTL_SetText(pIText,MMSPort,-1);
				}
	            return TRUE;
            }
        
        case EVT_DIALOG_START:         
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);
            return TRUE;

        case EVT_USER_REDRAW:  
            {
                int32 InsertPos = 0;
    			(void)ISHELL_LoadResString(pMe->m_pShell,
                            AEE_WMSAPPRES_LANGFILE,                                
                            IDS_EDIT,
                            Annstr,
                            sizeof(Annstr));
                
    			IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,Annstr);
                InsertPos = ITEXTCTL_GetCursorPos(pIText);
                DBGPRINTF("EVT_USER_REDRAW InsertPos=%d",InsertPos);
                if (InsertPos == 0)
                {
                    ITEXTCTL_SetCursorPos(pIText, TC_CURSOREND);
                }
                else
                {
                    ITEXTCTL_SetCursorPos(pIText, InsertPos);
                }
                ITEXTCTL_SetActive(pIText, TRUE);  
                ITEXTCTL_Redraw(pIText);
                // 绘制底部操作提示条
                DRAW_BOTTOMBAR(BTBAR_OK_BACK)
                IDISPLAY_Update(pMe->m_pDisplay);  
#ifdef FEATURE_LCD_TOUCH_ENABLE
                TSIM_NumberKeypad(FALSE);
#endif
            }
            return TRUE; 

        case EVT_CTL_TEXT_MODECHANGED:  //切换输入法    这里增加画底条，否则从符号输入界面返回时无底条
            // 绘制底部操作提示条
            {
                nInputMode = ITEXTCTL_GetInputMode(pIText,NULL);
                if (nInputMode != AEE_TM_SYMBOLS && nInputMode != AEE_TM_FACE_SYMBOL)
                {
                    DRAW_BOTTOMBAR(BTBAR_OK_BACK)
                    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
                }
            }
            return TRUE; 
            

        case EVT_DIALOG_END:
            {
                char MMSPort[MAX_MMS_PORT] = {0};
                AECHAR *pwsText = ITEXTCTL_GetTextPtr(pIText);
                WSTRTOSTR(pwsText, MMSPort, MAX_MMS_PORT);
                ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMS_PORT,MMSPort, sizeof(MMSPort)); 
                MSG_FATAL("IDD_MMS_PORT_Handler EVT_DIALOG_END 0",0,0,0);  
            }
            return TRUE;

        case EVT_KEY:
            MSG_FATAL("IDD_MMS_PORT_Handler EVT_KEY",0,0,0);
            switch (wParam)
            {
                case AVK_CLR:
                    MSG_FATAL("IDD_MMS_PORT_Handler AVK_CLR 1",0,0,0);
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                case AVK_INFO:
                case AVK_SELECT:
                    MSG_FATAL("AVK_SELECT ",0,0,0);
                    CLOSE_DIALOG(DLGRET_MSGBOX_OK)
                    return TRUE;
                      
                default:
                    break;
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
	                MSG_FATAL("IDD_MMS_PORT_Handler EVT_PEN_UP wXPos=%d, wYPos=%d",wXPos,wYPos,0);
					nBarH = GetBottomBarHeight(pMe->m_pDisplay);
			
					MEMSET(&devinfo, 0, sizeof(devinfo));
					ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
					SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);
	
					if(WMSAPP_PT_IN_RECT(wXPos,wYPos,rc))
					{
                        MSG_FATAL("IDD_MMS_PORT_Handler EVT_PEN_UP WMSAPP_PT_IN_RECT",0,0,0);
						if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
						{
                            MSG_FATAL("IDD_MMS_PORT_Handler EVT_PEN_UP left",0,0,0);
							return IDD_MMS_PORT_Handler((void *)pMe,EVT_KEY,AVK_SELECT,0);
						}
						else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
						{					
							boolean rt;
							int len = WSTRLEN(ITEXTCTL_GetTextPtr(pIText));
	                        MSG_FATAL("IDD_MMS_PORT_Handler EVT_PEN_UP left len=%d",len,0,0);
							if((ITEXTCTL_IsActive(pIText)) && (len > 0))
								return ITEXTCTL_HandleEvent(pIText,EVT_KEY,AVK_CLR,0);
							else
                                return IDD_MMS_PORT_Handler((void *)pMe, EVT_KEY, AVK_CLR, 0);
						}
					}
	
				}
				return TRUE;
#endif 

        default:
            break;            
    }

    return FALSE;
} // IDD_MMS_PORT_Handler

static boolean IDD_MMS_MEMSTATUS_Handler(void *pUser,
    AEEEvent     eCode,
    uint16       wParam,
    uint32       dwParam
)
{
    WmsApp *pMe = (WmsApp *)pUser;
    if (NULL == pMe)
    {
        return FALSE;
    }
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            return TRUE;

        case EVT_DIALOG_START:
            (void) ISHELL_PostEventEx(pMe->m_pShell, 
                                    EVTFLG_ASYNC,
                                    AEECLSID_WMSAPP,
                                    EVT_USER_REDRAW,
                                    0, 
                                    0);

        case EVT_USER_REDRAW:
        // 有新消息
        case EVT_WMS_MSG_RECEIVED_MESSAGE:
            {
                AECHAR  wstrDevice[32]={0};
                AECHAR  wstrVal[32]={0};
                char    strVal[32]={0};
                AEERect rc;
                int     nLineHeight,y;
                uint8  nOnUIMs=0;
#ifdef FEATURE_FUNCS_THEME    
                RGBVAL      oldColor;
                Theme_Param_type  Theme_Param={0};
                
                Appscom_GetThemeParameters(&Theme_Param);
#endif /* FEATURE_FUNCS_THEME */
				{
				// 画标题条
	            
                    TitleBar_Param_type  TitleBar_Param = {0};
                    
                    TitleBar_Param.pwszTitle = wstrDevice;
                    TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER;
                    (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                                IDS_MMS_MENSTATUS, wstrDevice,sizeof(wstrDevice));
                	#if 0
	                // 画标题条
	                {
	                    TitleBar_Param_type  TitleBar_Param = {0};
	                    
	                    TitleBar_Param.pwszTitle = wstrDevice;
	                    TitleBar_Param.dwAlignFlags = IDF_ALIGN_MIDDLE | IDF_ALIGN_CENTER;
	                    (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
	                                IDS_MENSTATUS, wstrDevice,sizeof(wstrDevice));
	                    
	                    DrawTitleBar(pMe->m_pDisplay, &TitleBar_Param);
	                    MEMSET(wstrDevice, 0, sizeof(wstrDevice));
	                }
					#else
					IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,wstrDevice);
					#endif
				}
                //获得系统字体高度
                nLineHeight = IDISPLAY_GetFontMetrics(pMe->m_pDisplay,
                                AEE_FONT_NORMAL,
                                NULL,
                                NULL);

                // 先清屏
                y = 0;//GetTitleBarHeight(pMe->m_pDisplay);
                SETAEERECT(&rc,0, y,pMe->m_rc.dx, pMe->m_rc.dy - GetBottomBarHeight(pMe->m_pDisplay) - y);
#ifdef FEATURE_FUNCS_THEME    
                IDISPLAY_FillRect(pMe->m_pDisplay, &rc, Theme_Param.bkcolor);
                
                // 更改文本字体颜色
                oldColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, Theme_Param.textColor);
#else

                IDISPLAY_FillRect(pMe->m_pDisplay, &rc, RGB_BLACK);
                IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);

#endif /* FEATURE_FUNCS_THEME */
                
                // 绘制话机收件箱使用情况
                SETAEERECT(&rc,3, y,pMe->m_rc.dx - 6, pMe->m_rc.dy - GetBottomBarHeight(pMe->m_pDisplay) - y);
                
#ifdef FEATURE_CDSMS_RUIM
                if (IsRunAsUIMVersion())
                {
                    (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                                IDS_PHONE, wstrDevice,sizeof(wstrDevice));
                    IDISPLAY_DrawText( pMe->m_pDisplay, 
                                AEE_FONT_BOLD, wstrDevice,
                                -1, 1, y, &rc, 
                                IDF_TEXT_TRANSPARENT|IDF_ALIGN_LEFT);
                    y += nLineHeight + 1;
                }
                else
                {
                    y += nLineHeight + 4;
                }
#endif /* FEATURE_CDSMS_RUIM */

                // 收件箱
                // 获取消息数
                (void) ICONFIG_GetItem(pMe->m_pConfig,CFGI_MMS_INCOUNT,&nOnUIMs,sizeof(nOnUIMs));
                MSG_FATAL("IDD_MMS_MEMSTATUS_Handler INBOX nOnUIMs=%d", nOnUIMs, 0, 0);
                (void)SPRINTF(strVal, "%d/%d", nOnUIMs, MAX_MMS_STORED); 
                (void)STRTOWSTR(strVal, wstrVal, sizeof(wstrVal));
                (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                            IDS_INBOX_MMS, wstrDevice,sizeof(wstrDevice));
                IDISPLAY_DrawText( pMe->m_pDisplay, 
                            AEE_FONT_NORMAL, wstrDevice,
                            -1, 1, y, &rc, 
                            IDF_TEXT_TRANSPARENT|IDF_ALIGN_LEFT);
                IDISPLAY_DrawText( pMe->m_pDisplay, 
                            AEE_FONT_NORMAL, wstrVal,
                            -1, 1, y, &rc, 
                            IDF_TEXT_TRANSPARENT|IDF_ALIGN_RIGHT);
                            
                // 发件箱
                // 获取消息数
                (void) ICONFIG_GetItem(pMe->m_pConfig,CFGI_MMS_OUTCOUNT,&nOnUIMs,sizeof(nOnUIMs));
                MSG_FATAL("IDD_MMS_MEMSTATUS_Handler OUTBOX nOnUIMs=%d", nOnUIMs, 0, 0);
                y += nLineHeight + 1;
                (void)SPRINTF(strVal, "%d/%d", nOnUIMs, MAX_MMS_STORED); 
                (void)STRTOWSTR(strVal, wstrVal, sizeof(wstrVal));
                (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                            IDS_OUTBOX_MMS, wstrDevice,sizeof(wstrDevice));
                IDISPLAY_DrawText( pMe->m_pDisplay, 
                            AEE_FONT_NORMAL, wstrDevice,
                            -1, 1, y, &rc, 
                            IDF_TEXT_TRANSPARENT|IDF_ALIGN_LEFT);
                IDISPLAY_DrawText( pMe->m_pDisplay, 
                            AEE_FONT_NORMAL, wstrVal,
                            -1, 1, y, &rc, 
                            IDF_TEXT_TRANSPARENT|IDF_ALIGN_RIGHT);
                            
                // 草稿箱
                // 获取消息数
                (void) ICONFIG_GetItem(pMe->m_pConfig,CFGI_MMS_DRAFTCOUNT,&nOnUIMs,sizeof(nOnUIMs));
                MSG_FATAL("IDD_MMS_MEMSTATUS_Handler DRAFT nOnUIMs=%d", nOnUIMs, 0, 0);
                y += nLineHeight + 1;
                (void)SPRINTF(strVal, "%d/%d", nOnUIMs, MAX_MMS_STORED); 
                (void)STRTOWSTR(strVal, wstrVal, sizeof(wstrVal));
                (void)ISHELL_LoadResString(pMe->m_pShell, AEE_WMSAPPRES_LANGFILE,
                            IDS_DRAFT_MMS, wstrDevice,sizeof(wstrDevice));
                IDISPLAY_DrawText( pMe->m_pDisplay, 
                            AEE_FONT_NORMAL, wstrDevice,
                            -1, 1, y, &rc, 
                            IDF_TEXT_TRANSPARENT|IDF_ALIGN_LEFT);
                IDISPLAY_DrawText( pMe->m_pDisplay, 
                            AEE_FONT_NORMAL, wstrVal,
                            -1, 1, y, &rc, 
                            IDF_TEXT_TRANSPARENT|IDF_ALIGN_RIGHT);
                

#ifdef FEATURE_FUNCS_THEME    
                // 恢复文本字体颜色
                (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, oldColor);
#else
                (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
#endif /* FEATURE_FUNCS_THEME */
            }
            
            // 绘制底条提示
            //          Back
            DRAW_BOTTOMBAR(BTBAR_BACK)
            
            // 立即更新显示
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            return TRUE;
            
        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch (wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
    
                default:
                    break;
            }
            return FALSE;
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

				if(WMSAPP_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_WMSAPP,EVT_USER,AVK_CLR,0);
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
}//IDD_MMS_MEMSTATUS_Handler

#endif
