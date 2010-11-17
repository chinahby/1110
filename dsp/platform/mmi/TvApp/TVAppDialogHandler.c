/*==============================================================================
//           TVApp   A P P L E T  M O D U L E
// 文件:
//        TVAppDialogHandler.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//        
// 文件说明：
//        拍照摄像模块对话框事件处理函数相关文件
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
#include "TVApp_priv.h" 
#include "MediaGallery.h"
#include "appscommonimages.brh"

//#define  TVAPP_IMAGE_RES_FILE "fs:/mod/tvapp/tvapp_image.bar"

/*==============================================================================                                 
                                 宏定义和常数                                
==============================================================================*/

// 画界面底部提示条宏定义
#define TV_DRAW_BOTTOMBAR(x)                           \
{                                                   \
    BottomBar_Param_type BarParam;                  \
    MEMSET(&BarParam, 0, sizeof(BarParam));         \
    BarParam.eBBarType = x;                         \
    DrawBottomBar(pMe->m_pDisplay, &BarParam);      \
}

/*Macro for add an item into no icon menuctl*/
#define TVMENU_ADDITEM(pMenu, ITEMID)            \
         IMENUCTL_AddItem((pMenu), (AEE_APPSTVAPP_RES_FILE), (ITEMID), (ITEMID), NULL, 0)
         

#define MMI_SUCCESS(result) (result == SUCCESS? TRUE:FALSE)
#define MMI_FAILURE(result) (result == SUCCESS? FALSE:TRUE)

struct ICameraEx
{
    VTBL(ICameraEx)    *pvt;
    int32             m_nRefs;
    OEMCamera        *m_pOEMCamera;
};

/*==============================================================================
                                 函数声明
==============================================================================*/
// 对话框IDD_CMAINMENU事件处理函数
static boolean  TVApp_MainMenuHandleEvent(CTVApp *pMe, 
                                              AEEEvent eCode, 
                                              uint16 wParam, 
                                              uint32 dwParam);

// 对话框IDD_CPREVIEW事件处理函数
static boolean  TVApp_PreviewHandleEvent(CTVApp *pMe, 
                                             AEEEvent eCode, 
                                             uint16 wParam, 
                                             uint32 dwParam);

// 对话框IDD_CCAMERACFG事件处理函数
static boolean TVApp_TVCFGHandleEvent(CTVApp *pMe, 
                                              AEEEvent eCode, 
                                              uint16 wParam, 
                                              uint32 dwParam);

// 对话框IDD_CPIC事件处理函数
static boolean TVApp_PicHandleEvent(CTVApp *pMe, 
                                        AEEEvent eCode, 
                                        uint16 wParam, 
                                        uint32 dwParam);

// 对话框IDD_POPMSG事件处理函数
static boolean  TVApp_PopMSGHandleEvent(CTVApp *pMe,
                                            AEEEvent   eCode,
                                            uint16     wParam,
                                            uint32     dwParam);

// PopMenu事件路由处理函数
static boolean TVApp_RoutePopMenuCommandEvent(CTVApp *pMe, 
                                                  uint16 wParam);
// PopMenu环境模式菜单处理函数                           
static boolean TVApp_PopMenu_EnvironmentCommandHandleEvent(CTVApp *pMe, 
                                                               uint16 wParam);
// PopMenu拍照质量设置菜单处理函数                                                                                                                 
static boolean TVApp_PopMenu_QualityCommandHandleEvent(CTVApp *pMe, 
                                                           uint16 wParam);

// PopMenu拍照尺寸设置菜单处理函数                                                                                                                      
static boolean TVApp_PopMenu_SizeCommandHandleEvent(CTVApp *pMe, 
                                                        uint16 wParam);
                                                                                                                    
// PopMenu快门声音设置菜单处理函数
static boolean TVApp_PopMenu_ShutterToneCommandHandleEvent(CTVApp *pMe, 
                                                               uint16 wParam);
                                                                                                                     
// PopMenu自拍设置菜单处理函数
static boolean TVApp_PopMenu_SelfTimeCommandHandleEvent(CTVApp *pMe, 
                                                            uint16 wParam);
                                                                                              
// PopMenu日光灯banding处理函数
static boolean TVApp_PopMenu_BandingCommandHandleEvent(CTVApp *pMe, 
                                                           uint16 wParam);

// PopMenu camera/video设置复位处理函数
static boolean TVApp_PopMenu_ResetCFGCommandHandleEvent(CTVApp *pMe, 
                                                            uint16 wParam);

// 设置菜单项选中图标
static void TVApp_SetCFGMenuIcon(IMenuCtl *pm, 
                                     uint16 listId, 
                                   boolean bSel);

// 初始化设置菜单
static boolean TVApp_InitpopMenu(CTVApp *pMe, 
                                     IMenuCtl *popMenu);

// 初始化设置数据
static void TVApp_InitCFGData(CTVApp * pMe);

// 初始化拍照环境设置菜单
static void TVApp_PopMenu_EnvironmentInit(CTVApp *pMe, 
                                              IMenuCtl *popMenu);

// 初始化拍照质量设置菜单
static void TVApp_PopMenu_QualityInit(CTVApp *pMe, 
                                          IMenuCtl *popMenu);

// 初始化拍照尺寸设置菜单
static void TVApp_PopMenu_SizeInit(CTVApp *pMe, 
                                       IMenuCtl *popMenu);

// 初始化快门声设置菜单
static void TVApp_PopMenu_ShutterToneInit(CTVApp *pMe, 
                                              IMenuCtl *popMenu);

// 初始化自拍时间设置菜单
static void TVApp_PopMenu_SelfTimeInit(CTVApp *pMe, 
                                           IMenuCtl *popMenu);

// 初始化Bandig设置菜单
static void TVApp_PopMenu_BandingInit(CTVApp *pMe, 
                                          IMenuCtl *popMenu);

// 初始化恢复默认设置菜单
static void TVApp_PopMenu_ResetCFGInit(CTVApp *pMe, 
                                           IMenuCtl *popMenu);
/*TV主弹菜单事件处理函数*/
static boolean TV_MainOptsMenu_HandleEvent(CTVApp *pMe,
                                              AEEEvent eCode,
                                              uint16 wParam,
                                              uint32 dwParam);
/*TV地域菜单事件处理函数*/
static boolean TV_REGIONMenu_HandleEvent(CTVApp *pMe,
                                              AEEEvent eCode,
                                              uint16 wParam,
                                              uint32 dwParam);
/*ASIA菜单事件处理函数*/
static boolean TV_ASIAMenu_HandleEvent(CTVApp *pMe,
                                              AEEEvent eCode,
                                              uint16 wParam,
                                              uint32 dwParam);
/*LATIN菜单事件处理函数*/
static boolean TV_LATINMenu_HandleEvent(CTVApp *pMe,
                                              AEEEvent eCode,
                                              uint16 wParam,
                                              uint32 dwParam);
/*EUROPE菜单事件处理函数*/
static boolean TV_EUROPEMenu_HandleEvent(CTVApp *pMe,
                                              AEEEvent eCode,
                                              uint16 wParam,
                                              uint32 dwParam);
/*AFRICA菜单事件处理函数*/
static boolean TV_AFRICAMenu_HandleEvent(CTVApp *pMe,
                                              AEEEvent eCode,
                                              uint16 wParam,
                                              uint32 dwParam);
/*OCEANIA菜单事件处理函数*/
static boolean TV_OCEANIAMenu_HandleEvent(CTVApp *pMe,
                                              AEEEvent eCode,
                                              uint16 wParam,
                                              uint32 dwParam);

/*DRAWTOPBAR菜单事件处理函数*/
static boolean TV_DRAWTOPBAR_HandleEvent(CTVApp *pMe,
                                              AEEEvent eCode,
                                              uint16 wParam,
                                              uint32 dwParam);

/*DRAWBAR1菜单事件处理函数*/
static boolean TV_DRAWBAR1_HandleEvent(CTVApp *pMe,
                                              AEEEvent eCode,
                                              uint16 wParam,
                                              uint32 dwParam);

/*DRAWBAR2菜单事件处理函数*/
static boolean TV_DRAWBAR2_HandleEvent(CTVApp *pMe,
                                              AEEEvent eCode,
                                              uint16 wParam,
                                              uint32 dwParam);

/*DRAWBAR3菜单事件处理函数*/
static boolean TV_DRAWBAR3_HandleEvent(CTVApp *pMe,
                                              AEEEvent eCode,
                                              uint16 wParam,
                                              uint32 dwParam);
/*DRAWBAR4菜单事件处理函数*/
static boolean TV_DRAWBAR4_HandleEvent(CTVApp *pMe,
                                              AEEEvent eCode,
                                              uint16 wParam,
                                              uint32 dwParam);
/*DRAWBAR5菜单事件处理函数*/
static boolean TV_DRAWBAR5_HandleEvent(CTVApp *pMe,
                                              AEEEvent eCode,
                                              uint16 wParam,
                                              uint32 dwParam);
/*DRAWBAR6菜单事件处理函数*/
static boolean TV_DRAWBAR6_HandleEvent(CTVApp *pMe,
                                              AEEEvent eCode,
                                              uint16 wParam,
                                              uint32 dwParam);
/*DRAWBAR7菜单事件处理函数*/
static boolean TV_DRAWBAR7_HandleEvent(CTVApp *pMe,
                                              AEEEvent eCode,
                                              uint16 wParam,
                                              uint32 dwParam);

//TV主菜单
static void TV_Build_MainOpts_Menu(CTVApp *pMe,IMenuCtl *pMenuCtl);

// 拍照快门声处理函数
static void TVApp_PlayShutterSound(CTVApp *pMe);

// 快门声处理回调函数
static void TVApp_MediaNotify(void *pUser, 
                                  AEEMediaCmdNotify *pCmdNotify);

//void TVApp_AlphaDisplay(CTVApp *pMe, IMenuCtl *pMenu);

// 设置弹出菜单矩形
static void TVApp_SetPopMenuRect(CTVApp *pMe, 
                                     IMenuCtl *popMenu, 
                                     int menuItemSum);

// 画TopBar的函数
static void TVApp_DrawTopBar(CTVApp *pMe);

//static void TVApp_DrawpopMenuBg(CTVApp *pMe, IMenuCtl *popMenu);

// 画Bottombar上的TV/video图标函数
static void TVApp_DrawMidPic(CTVApp *pMe);

// 画CFGBar的提示文本
static void TVApp_DrawCFGPromptText(CTVApp *pMe);

// TV Preview的启动函数
static void TVApp_CPreviewStart(CTVApp *pMe);

// TV Snapshot的处理函数
static void TVApp_RecordSnapShot(CTVApp *pMe);

// 找T卡的函数
static boolean TVApp_FindMemoryCardExist(CTVApp *pMe);

// 找Sensor的函数
//static boolean TVApp_FindSensorExist(CTVApp *pMe);

// Dialog的定时处理函数
static void TVApp_DialogTimeout(void *pme);

// 存储空间的检测函数
static boolean TVApp_IsEnoughfMemorySpace(CTVApp *pMe);

// 画bottom bar上的文本函数
static void TVApp_DrawBottomBarText(CTVApp *pMe, 
                                        BottomBar_e_Type eBarType);

// 自拍处理函数
static boolean TVApp_SelfTimeRecordSnapShot(CTVApp *pMe);

// 设置拍照文件名
static boolean TVApp_SetDateForRecordFileName(CTVApp *pMe);

// 获取当前时间
static boolean TVApp_GetDateForRecordFileName(CTVApp *pMe,
                                                  char *pszDest, 
                                                  unsigned int fileType);

// 重新设置拍照大小的处理函数
static void TVApp_SetTVCaptureSize(CTVApp *pMe, 
                                           uint16 wParam);

// 拍照后，照片处理函数
static void TVApp_HandleSnapshotPic(CTVApp *pMe);


// 设置TV Preview的参数
static void TVApp_SetParamAfterPreview(CTVApp * pMe);

/*添加REGIONMenu主菜单*/
static void TV_REGIONMenu(CTVApp *pMe,IMenuCtl *pMenuCtl);

/*添加ASIAMenu主菜单*/
static void TV_ASIAMenu(CTVApp *pMe,IMenuCtl *pMenuCtl);

/*添加LATINMenu主菜单*/
static void TV_LATINMenu(CTVApp *pMe,IMenuCtl *pMenuCtl);

/*添加EUROPEMenu主菜单*/
static void TV_EUROPEMenu(CTVApp *pMe,IMenuCtl *pMenuCtl);

/*添加AFRICAMenu主菜单*/
static void TV_AFRICAMenu(CTVApp *pMe,IMenuCtl *pMenuCtl);

/*添加OCEANIAMenu主菜单*/
static void TV_OCEANIAMenu(CTVApp *pMe,IMenuCtl *pMenuCtl);



/*画图处理*/
static int TV_UpdateInit(CTVApp *pMe);
static int TV_Update(CTVApp *pMe);
static void TV_StartPreview(CTVApp *pMe);
static void TV_StopSearchAnimation(CTVApp *pITvUtil);



void TVApp_InitTVCheck(void *po);

static void TVApp_CreateDirectory(CTVApp *pMe);
static void TVApp_UpdateFrame(CTVApp *pMe);
static int TVApp_SavePhoto(CTVApp *pMe);
static void TVApp_DrawImage(CTVApp *pMe, uint16 ResID, int x, int y);

/*==============================================================================
                                 全局数据
==============================================================================*/

/*==============================================================================
                                 本地（静态）数据
==============================================================================*/

typedef struct
{
    uint16 dx;
    uint16 dy;
    AECHAR *pStr;
}CTVSize;

// 最大OEMNV_CAMERA_SIZE_MAX
static const CTVSize g_TVSizeCFG[] = 
{
#if defined(FEATURE_DISP_160X128)
    {160,128,L"160*128"}, // FULL Screen
    {220,176,L"220*176"}, // QCIF
    {320,240,L"320*240"}, // QVGA
    {640,480,L"640*480"}, // VGA  
#elif defined(FEATURE_DISP_220X176)
    {160,128,L"160*128"}, // FULL Screen
    {220,176,L"220*176"}, // QCIF
    {320,240,L"320*240"}, // QVGA
    {640,480,L"640*480"}, // VGA      
#elif defined(FEATURE_DISP_128X160)
    {128,160,L"128*160"}, // FULL Screen
    {176,220,L"176*220"}, // QCIF
    {240,320,L"240*320"}, // QVGA
    {480,640,L"480*640"}, // VGA    
#elif defined(FEATURE_DISP_176X220)
    //{128,160,L"128*160"}, // FULL Screen
    {176,220,L"176*220"}, // QCIF
    {240,320,L"240*320"}, // QVGA
    {480,640,L"480*640"}, // VGA    
#elif defined(FEATURE_DISP_240X320)
    //{128,160,L"128*160"}, // FULL Screen
    //{176,220,L"176*220"}, // QCIF
    {240,320,L"240*320"}, // QVGA
    {480,640,L"480*640"}, // VGA   
#elif defined(FEATURE_DISP_320X240)
    //{128,160,L"128*160"}, // FULL Screen
    //{176,220,L"176*220"}, // QCIF
    {240,320,L"320*240"}, // QVGA
    {480,640,L"640*480"}, // VGA       
#else
    {128,128,L"128*128"}, // FULL Screen
    {160,120,L"160*120"}, // QQVGA
    {220,176,L"220*176"}, // QCIF
    {320,240,L"320*240"}, // QVGA
    {640,480,L"640*480"}, // VGA
#endif
    {0,0,NULL}
};

//10万像素的选项
static const CTVSize g_TVSizeCFG_10[] = 
{
#if defined(FEATURE_DISP_160X128)
    {160,128,L"160*128"}, // FULL Screen
    {220,176,L"220*176"}, // QCIF
    {320,240,L"320*240"}, // QVGA
#elif defined(FEATURE_DISP_220X176)
    {160,128,L"160*128"}, // FULL Screen
    {220,176,L"220*176"}, // QCIF
    {320,240,L"320*240"}, // QVGA    
#elif defined(FEATURE_DISP_128X160)
    {128,160,L"128*160"}, // FULL Screen
    {176,220,L"176*220"}, // QCIF
    {240,320,L"240*320"}, // QVGA    
#elif defined(FEATURE_DISP_176X220)
    //{128,160,L"128*160"}, // FULL Screen
    {176,220,L"176*220"}, // QCIF
    {240,320,L"240*320"}, // QVGA   
#elif defined(FEATURE_DISP_240X320)
    //{128,160,L"128*160"}, // FULL Screen
    //{176,220,L"176*220"}, // QCIF
    {240,320,L"240*320"}, // QVGA    
#elif defined(FEATURE_DISP_320X240)
    //{128,160,L"128*160"}, // FULL Screen
    //{176,220,L"176*220"}, // QCIF
    {240,320,L"320*240"}, // QVGA        
#else
    {128,128,L"128*128"}, // FULL Screen
    {160,120,L"160*120"}, // QQVGA
    {220,176,L"220*176"}, // QCIF
    {320,240,L"320*240"}, // QVGA
#endif
    {0,0,NULL}
};

/*==============================================================================
                                 函数定义
==============================================================================*/


/*==============================================================================
函数:
       TVApp_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向TVApp Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:
       

==============================================================================*/
void TVApp_ShowDialog(CTVApp *pMe,uint16  dlgResId)
{
    int nRet;

    // At most one dialog open at once
    if(ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // Looks like there is one dialog already opened.
        // Flag an error an return without doing anything.
        return;
    }
    MSG_FATAL("TVApp_ShowDialog------------------------------------",0,0,0);
    if (NULL != pMe->m_pDisplay)
    {
        if (dlgResId == IDD_CMAINMENU)
        {
            (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:1", STRLEN("a:1"));
            
            SetDeviceState(DEVICE_TYPE_CAMERA, DEVICE_CAMERA_STATE_OFF);
        }
        else
        {
            if(dlgResId == IDD_CPREVIEW)
            {
                (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:0", STRLEN("a:0"));
            }
            
            SetDeviceState(DEVICE_TYPE_CAMERA, DEVICE_CAMERA_STATE_ON);
        }       
    }

    
    nRet = ISHELL_CreateDialog(pMe->m_pShell, AEE_APPSTVAPP_RES_FILE, dlgResId, NULL);
    
    if(nRet != SUCCESS)
    {
        return;
    }
}

/*==============================================================================
函数:
       TVApp_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向TVApp Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean TVApp_RouteDialogEvent(CTVApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{   
    if((NULL == pMe) || (NULL == pMe->m_pActiveDlg))
    {
        return FALSE;
    }
    
    switch(pMe->m_pActiveDlgID)
    {
    MSG_FATAL("TVApp_RouteDialogEvent---------------------------------",0,0,0);
        case IDD_CMAINMENU:
            return TVApp_MainMenuHandleEvent(pMe, eCode, wParam, dwParam);
            
        case IDD_CPREVIEW:
            return TVApp_PreviewHandleEvent(pMe, eCode, wParam, dwParam);

        case IDD_CCAMERACFG:
            return TVApp_TVCFGHandleEvent(pMe, eCode, wParam, dwParam);
  
        case IDD_CPIC:
            return TVApp_PicHandleEvent(pMe, eCode, wParam, dwParam);

        case IDD_POPMSG:
			MSG_FATAL("IDD_POPMSG---------------------------------",0,0,0);
			return	TV_MainOptsMenu_HandleEvent(pMe, eCode,wParam, dwParam);


		case IDD_REGION:
			return  TV_REGIONMenu_HandleEvent(pMe, eCode,wParam, dwParam);

		case IDD_REGIONASIA:
			return  TV_ASIAMenu_HandleEvent(pMe, eCode,wParam, dwParam);

		case IDD_REGIONLATIN:
			return  TV_LATINMenu_HandleEvent(pMe, eCode,wParam, dwParam);
			
		case IDD_REGIONEUROPE:
			return  TV_EUROPEMenu_HandleEvent(pMe, eCode,wParam, dwParam);
			
		case IDD_REGIONAFRICA:
			return  TV_AFRICAMenu_HandleEvent(pMe, eCode,wParam, dwParam);
			
		case IDD_REGIONOCEANIA:
			return  TV_OCEANIAMenu_HandleEvent(pMe, eCode,wParam, dwParam);

		case IDD_DRAWTOPBAR:
			return  TV_DRAWTOPBAR_HandleEvent(pMe, eCode,wParam, dwParam);

		case IDD_DIALOG1:
			return  TV_DRAWBAR1_HandleEvent(pMe, eCode,wParam, dwParam);

		case IDD_DIALOG2:
			return  TV_DRAWBAR2_HandleEvent(pMe, eCode,wParam, dwParam);

		case IDD_DIALOG3:
			return  TV_DRAWBAR3_HandleEvent(pMe, eCode,wParam, dwParam);

		case IDD_DIALOG4:
			return  TV_DRAWBAR4_HandleEvent(pMe, eCode,wParam, dwParam);

		case IDD_DIALOG5:
			return  TV_DRAWBAR5_HandleEvent(pMe, eCode,wParam, dwParam);

		case IDD_DIALOG6:
			return  TV_DRAWBAR6_HandleEvent(pMe, eCode,wParam, dwParam);

		case IDD_DIALOG7:
			return  TV_DRAWBAR7_HandleEvent(pMe, eCode,wParam, dwParam);
		
        default:
            return FALSE;
    }
}


/*==============================================================================
函数：
       TVApp_MainMenuHandleEvent
说明：
       IDD_CMAINMENU对话框事件处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
extern   MMITV_SETTINGS	*pTvSetting;


static boolean TVApp_MainMenuHandleEvent(CTVApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
        
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_CAMERA_MAINMENU);
	AECHAR WTitle[40] = {0};
    byte nBackLight;
    
    if(pMenu == NULL)
    {
        return FALSE;
    }
   MSG_FATAL("TVApp_MainMenuHandleEvent eCode=%d, wParam=%d, dwParam=%d",eCode,wParam,dwParam);
    switch(eCode) 
    {
        case EVT_DIALOG_INIT:
		   IDISPLAY_SetClipRect(pMe->m_pDisplay, NULL); 
		   MSG_FATAL("IMMITv_StartPreview----------------------EVT_DIALOG_INIT",0,0,0);
           pMe->m_bIsPreview = FALSE;
           pMe->m_nTVState = TV_START;
            
           pMe->m_wMsgID = IDS_MSG_WAITING;
          pMe->m_nMsgTimeout = TIMEOUT_MS_MSGBOX;

		 {
			int result = SUCCESS;
							   
			result = ISHELL_CreateInstance(pMe->m_pShell, MMI_CLSID_IMMITV, (void**)&pMe->pIMMITv);
			if(pMe->pIMMITv == NULL)
			 {
			  return FALSE;
			 }
						
						
			result = IMMITv_CreateTv(pMe->pIMMITv, &pMe->myICBMMITv);
			if(MMI_SUCCESS(result))
			 {
				 MSG_FATAL("IMMITv_CreateTv---------------------success",0,0,0);
			 }
			//pTvSetting->region=TLG_REGION_SHENZHEN;
			//result = IMMITv_SetRegion(pMe->pIMMITv,TLG_REGION_SHENZHEN );
			MSG_FATAL("IMMITv_StartPreview----------------------success12",0,0,0);
			result = IMMITv_StartPreview(pMe->pIMMITv);
			if(MMI_SUCCESS(result))
			{
				MSG_FATAL("IMMITv_StartPreview----------------------success",0,0,0);
			}
			MSG_FATAL("IMMITv_StartPreview----------------------will--end",0,0,0);

		   // IMMITv_AutoScanTV(pMe->pIMMITv); 
		   //  TV_StopSearchAnimation(pMe);
  
		 }
			
            //IMENUCTL_SetPopMenuRect(pMenu);
			//IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            //IMENUCTL_SetBottomBarType(pMenu, BTBAR_OPTION_BACK);//BTBAR_SELECT_BACK
			 IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);	
            return TRUE;
        	
        case EVT_DIALOG_START:
           if(pMe->m_pTV)
            {
                ICAMERAEX_Release(pMe->m_pTV);
                pMe->m_pTV = NULL;
            }
            (void)ISHELL_PostEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL);            
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_USER_REDRAW: 

			 //(void)IMENUCTL_Redraw(pMenu);

			// TV_UpdateInit(pMe);
			// TVApp_DrawBottomBarText(pMe, BTBAR_OPTION_BACK); 
			// TV_Update(pMe);
			//IDISPLAY_Update(pMe->m_pDisplay); //刷屏
            return TRUE;
            
        case EVT_COMMAND:          
            switch(wParam) 
            {
                case IDS_ITEM_TV: 
                    CLOSE_DIALOG(DLGRET_POPMSG);
                    break;

                case IDS_ITEM_CAMERA_GALLERY:
                    pMe->m_nMainMenuItemSel = IDS_ITEM_CAMERA_GALLERY;
                    CMediaGallery_FileExplorer(GALLERY_PHOTO_BROWSE, NULL);
                    break;
                    
                default:         
                    break;  
            }
            return TRUE;        
             
        case EVT_KEY:

			//TV_MainOptsMenu_HandleEvent(pMe, eCode,wParam, dwParam);
            if(wParam == AVK_SELECT)
            {
             IMMITv_StopPreview(pMe->pIMMITv);
             CLOSE_DIALOG(DLGRET_POPMSG);//STATE_CPOPMSG
			      
            }
			if(wParam == AVK_CLR)
            {
                CLOSE_DIALOG(DLGRET_CANCELED);
            }

			if(wParam == AVK_LEFT)
            {
                CLOSE_DIALOG(DLGRET_DRAWTOPBAR);
            }
            return TRUE;

   
        default:
            break;
    }
    return FALSE;
}

static void TV_StopSearchAnimation(CTVApp *pITvUtil)
{  
    int result = SUCCESS;
    int i = 0;
    char title[3];
    char channel[4];
    CTVApp* pThis = (CTVApp*)pITvUtil;
    uint16 *ableChannelArray = IMMITv_getAbleChannelArray(pThis->pIMMITv);
    int channelCount = IMMITv_getChannelCountAble(pThis->pIMMITv);
    

  //  MMI_DEBUG(ATV,("CTvUtil_StopSearchAnimation Start"));

    if(channelCount > 0)
    {
        pTvSetting->ChannelCountAble = channelCount;
        pTvSetting->CurrentChannel = ableChannelArray[0];//自动搜索完成后，会跳到索引为0的频道来
        pThis->currentlyChannel = ableChannelArray[0];
        pThis->currentlyChannelIndex = 0;
        result = IMMITv_SetTvChannel(pThis->pIMMITv, ableChannelArray[0],FALSE); 
       // MMI_DEBUG(ATV, ("search finish，have %d channel",channelCount));
        if (pThis->pIMMITv != NULL)
        {
           // IMMITv_SetProperty(pThis->pIMMITv, TV_PROPERTY_SOUND, pTvSetting->SoundStep);
            //IMMIAudioDevice_GetOutputVolumn(pThis->pIMMIAudioDevice,AUDIOVOLUME_LEVEL0);
        }
        for(; i < channelCount; i++)
        {
            SPRINTF(title, "%d", i+1);
            SPRINTF(channel, "%d",  ableChannelArray[i]);
           // CTvUtil_BookmarkOperator_InsertAt(channel, title, &pThis->BM_ListFocusIndex);             
        }
        IMMITv_ResetScanTag(pThis->pIMMITv);
       // CTvUtil_UI_ShowCurrentChannel(pThis, TRUE);                     
   }

	
   // IUTKForm_DeleteWidgetByID(pThis->pIViewForm, IDW_TV_FORM_IMG_SEARCH);
   // MMI_DEBUG(ATV,("CTvUtil_StopSearchAnimation END"));
}

static void TV_StartPreview(CTVApp * pMe)
{
    int result=SUCCESS;
	result = IMMITv_CreateTv(pMe->pIMMITv, &pMe->myICBMMITv);
		if(MMI_SUCCESS(result))
		{
			MSG_FATAL("IMMITv_CreateTv---------------------success",0,0,0);

		}
		
		result = IMMITv_StartPreview(pMe->pIMMITv);
		if(MMI_SUCCESS(result))
		{
			MSG_FATAL("IMMITv_StartPreview----------------------success",0,0,0);

		}
		ISHELL_CancelTimer(pMe->pIShell, (PFNNOTIFY)TV_StartPreview, pMe);

}


/*==============================================================================
函数：
       TV_DRAWTOPBAR_HandleEvent
说明：
       IDD_DRAWTOPBAR对话框事件处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean TV_DRAWTOPBAR_HandleEvent(CTVApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_DRAWTOPBAR);
    
    if(pMenu == NULL)
    {
        return FALSE;
    }
    switch(eCode) 
    {
        case EVT_DIALOG_INIT:
           // IMENUCTL_SetPopMenuRect(pMenu);
			//IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
           // IMENUCTL_SetBottomBarType(pMenu, BTBAR_OK_BACK);
		
            return TRUE;
        	
        case EVT_DIALOG_START:

		if(pMe->m_pTV)
            {
                ICAMERAEX_Release(pMe->m_pTV);
                pMe->m_pTV = NULL;
            }
            (void)ISHELL_PostEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL);            
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_USER_REDRAW:
			TV_UpdateInit(pMe);

            IMENUCTL_Redraw(pMenu);
				{
				IImage *pTopBarImage2 = NULL;
				AEEImageInfo myInfo;
				
			    pTopBarImage2 = ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_TOOLBAR); 
				if(pTopBarImage2)
	            {
	            
			    
	            IIMAGE_GetInfo(pTopBarImage2, &myInfo);
				MSG_FATAL("m_cxWidth--%d------m_cyHeight--%d-----myInfo----%d",pMe->m_cxWidth,pMe->m_cyHeight,myInfo.cy);
	            IIMAGE_Draw(pTopBarImage2, (pMe->m_cxWidth - myInfo.cx)/2, pMe->m_cyHeight-(myInfo.cy*2)-15);	
	         
	            IIMAGE_Release(pTopBarImage2);
	            pTopBarImage2 = NULL;
				TV_Update(pMe);
	            }
            	}
			//IDISPLAY_Update(pMe->m_pDisplay); //刷屏
            return TRUE;
            
        case EVT_COMMAND:          
            switch(wParam) 
            {
               case IDS_ITEM_CAMERA_GALLERY:
                    break;
                    
                default:         
                    break;  
            }
            return TRUE;        
             
        case EVT_KEY:

            if(wParam == AVK_RIGHT)
            {
             CLOSE_DIALOG(DLGRET_BAR1);//STATE_CPOPMSG

			}
			if(wParam == AVK_CLR)
            {
                CLOSE_DIALOG(DLGRET_MAINMENU);// DLGRET_CANCELED
            }
			
            return TRUE;

   
        default:
            break;
    }
    return FALSE;
}

/*==============================================================================
函数：
       TV_DRAWBAR1_HandleEvent
说明：
       IDD_DIALOG1对话框事件处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean TV_DRAWBAR1_HandleEvent(CTVApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_DIALOG1);
    
    if(pMenu == NULL)
    {
        return FALSE;
    }
    switch(eCode) 
    {
        case EVT_DIALOG_INIT:
            IMENUCTL_SetPopMenuRect(pMenu);
			IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetBottomBarType(pMenu, BTBAR_OK_BACK);
				
            return TRUE;
        	
        case EVT_DIALOG_START:

		if(pMe->m_pTV)
            {
                ICAMERAEX_Release(pMe->m_pTV);
                pMe->m_pTV = NULL;
            }
            (void)ISHELL_PostEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL);            
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_USER_REDRAW:

            IMENUCTL_Redraw(pMenu);
				{
				IImage *pTopBarImage2 = NULL;
				IImage *BarImage1 = NULL;
				IImage *BarImageBG = NULL;
				AEEImageInfo myInfo;
			    pTopBarImage2 = ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_TOOLBAR); 
				BarImage1 =ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_SELECT);
				BarImageBG =ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_SELECT_UNABLE);
				if(pTopBarImage2)
	            {
	             TV_UpdateInit(pMe);
	            IIMAGE_GetInfo(pTopBarImage2, &myInfo);
				MSG_FATAL("m_cxWidth--%d------m_cyHeight--%d-----myInfo----%d",pMe->m_cxWidth,pMe->m_cyHeight,myInfo.cy);
				IIMAGE_Draw(pTopBarImage2, (pMe->m_cxWidth - myInfo.cx)/2, pMe->m_cyHeight-(myInfo.cy*2)-15);	
                IIMAGE_Draw(BarImage1, (pMe->m_cxWidth - myInfo.cx)/2, pMe->m_cyHeight-(myInfo.cy*2)-12);	
	            IIMAGE_Draw(BarImageBG, (pMe->m_cxWidth - myInfo.cx)/2, pMe->m_cyHeight-(myInfo.cy*2)-12);

				IIMAGE_Release(pTopBarImage2);
	            pTopBarImage2 = NULL;
				TV_Update(pMe);
	            }
            	}
			//IDISPLAY_Update(pMe->m_pDisplay); //刷屏
            return TRUE;
            
        case EVT_COMMAND:          
            switch(wParam) 
            {
               case IDS_ITEM_CAMERA_GALLERY:
                    break;
                    
                default:         
                    break;  
            }
            return TRUE;        
             
        case EVT_KEY:

			if(wParam == AVK_RIGHT)
			{
			CLOSE_DIALOG(DLGRET_BAR2);//STATE_CPOPMSG
			}

            if(wParam == AVK_SELECT)
            {
            // CLOSE_DIALOG(DLGRET_POPMSG);

			}
			if(wParam == AVK_CLR)
            {
                CLOSE_DIALOG(DLGRET_DRAWTOPBAR);
            }
			
            return TRUE;

   
        default:
            break;
    }
    return FALSE;
}

/*==============================================================================
函数：
       TV_DRAWBAR1_HandleEvent
说明：
       IDD_DIALOG1对话框事件处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean TV_DRAWBAR2_HandleEvent(CTVApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_DIALOG2);
    
    if(pMenu == NULL)
    {
        return FALSE;
    }
    switch(eCode) 
    {
        case EVT_DIALOG_INIT:
            IMENUCTL_SetPopMenuRect(pMenu);
			IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetBottomBarType(pMenu, BTBAR_OK_BACK);
				
            return TRUE;
        	
        case EVT_DIALOG_START:

		if(pMe->m_pTV)
            {
                ICAMERAEX_Release(pMe->m_pTV);
                pMe->m_pTV = NULL;
            }
            (void)ISHELL_PostEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL);            
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_USER_REDRAW:

            IMENUCTL_Redraw(pMenu);
				{
				IImage *pTopBarImage2 = NULL;
				IImage *BarImage1 = NULL;
				IImage *BarImageBG = NULL;
				AEEImageInfo myInfo;
				AEEImageInfo bgInfo;
			    pTopBarImage2 = ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_TOOLBAR); 
				BarImage1 =ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_SELECT);
				BarImageBG =ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_SELECT_UNABLE);
				if(pTopBarImage2)
	            {
	             TV_UpdateInit(pMe);
	             IIMAGE_GetInfo(BarImageBG, &bgInfo);
	            IIMAGE_GetInfo(pTopBarImage2, &myInfo);
				MSG_FATAL("m_cxWidth--%d------m_cyHeight--%d-----myInfo----%d---------bgInfo--%d",pMe->m_cxWidth,pMe->m_cyHeight,myInfo.cy);
				MSG_FATAL("-----------------------------------bgInfo--%d",bgInfo.cx,0,0);

				IIMAGE_Draw(pTopBarImage2, (pMe->m_cxWidth - myInfo.cx)/2, pMe->m_cyHeight-(myInfo.cy*2)-15);	
                IIMAGE_Draw(BarImage1, ((pMe->m_cxWidth - myInfo.cx)/2)+bgInfo.cx, pMe->m_cyHeight-(myInfo.cy*2)-12);	
	            IIMAGE_Draw(BarImageBG, ((pMe->m_cxWidth - myInfo.cx)/2)+bgInfo.cx, pMe->m_cyHeight-(myInfo.cy*2)-12);

				IIMAGE_Release(pTopBarImage2);
	            pTopBarImage2 = NULL;
				TV_Update(pMe);
	            }
            	}
			//IDISPLAY_Update(pMe->m_pDisplay); //刷屏
            return TRUE;
            
        case EVT_COMMAND:          
            switch(wParam) 
            {
               case IDS_ITEM_CAMERA_GALLERY:
                    break;
                    
                default:         
                    break;  
            }
            return TRUE;        
             
        case EVT_KEY:
			if(wParam == AVK_RIGHT)
			{
			CLOSE_DIALOG(DLGRET_BAR3);//STATE_CPOPMSG
			}

            if(wParam == AVK_SELECT)
            {
            // CLOSE_DIALOG(DLGRET_POPMSG);

			}
			if(wParam == AVK_CLR)
            {
                CLOSE_DIALOG(DLGRET_DRAWTOPBAR);
            }
			
            return TRUE;

   
        default:
            break;
    }
    return FALSE;
}

/*==============================================================================
函数：
       TV_DRAWBAR1_HandleEvent
说明：
       IDD_DIALOG1对话框事件处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean TV_DRAWBAR3_HandleEvent(CTVApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_DIALOG3);
    
    if(pMenu == NULL)
    {
        return FALSE;
    }
    switch(eCode) 
    {
        case EVT_DIALOG_INIT:
            IMENUCTL_SetPopMenuRect(pMenu);
			IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetBottomBarType(pMenu, BTBAR_OK_BACK);
				
            return TRUE;
        	
        case EVT_DIALOG_START:

		if(pMe->m_pTV)
            {
                ICAMERAEX_Release(pMe->m_pTV);
                pMe->m_pTV = NULL;
            }
            (void)ISHELL_PostEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL);            
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_USER_REDRAW:

            IMENUCTL_Redraw(pMenu);
				{
				IImage *pTopBarImage2 = NULL;
				IImage *BarImage1 = NULL;
				IImage *BarImageBG = NULL;
				AEEImageInfo myInfo;
				AEEImageInfo bgInfo;
			    pTopBarImage2 = ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_TOOLBAR); 
				BarImage1 =ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_SELECT);
				BarImageBG =ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_SELECT_UNABLE);
				if(pTopBarImage2)
	            {
	             IIMAGE_GetInfo(BarImageBG, &bgInfo);
	            IIMAGE_GetInfo(pTopBarImage2, &myInfo);
				MSG_FATAL("m_cxWidth--%d------m_cyHeight--%d-----myInfo----%d",pMe->m_cxWidth,pMe->m_cyHeight,myInfo.cy);
				IIMAGE_Draw(pTopBarImage2, (pMe->m_cxWidth - myInfo.cx)/2, pMe->m_cyHeight-(myInfo.cy*2)-15);	
                IIMAGE_Draw(BarImage1, ((pMe->m_cxWidth - myInfo.cx)/2)+(bgInfo.cx*2), pMe->m_cyHeight-(myInfo.cy*2)-12);	
	            IIMAGE_Draw(BarImageBG, ((pMe->m_cxWidth - myInfo.cx)/2)+(bgInfo.cx*2), pMe->m_cyHeight-(myInfo.cy*2)-12);



				IIMAGE_Release(pTopBarImage2);
	            pTopBarImage2 = NULL;
	            }
            	}
			//IDISPLAY_Update(pMe->m_pDisplay); //刷屏
            return TRUE;
            
        case EVT_COMMAND:          
            switch(wParam) 
            {
               case IDS_ITEM_CAMERA_GALLERY:
                    break;
                    
                default:         
                    break;  
            }
            return TRUE;        
             
        case EVT_KEY:
			if(wParam == AVK_RIGHT)
			{
			CLOSE_DIALOG(DLGRET_BAR4);//STATE_CPOPMSG
			}

            if(wParam == AVK_SELECT)
            {
            // CLOSE_DIALOG(DLGRET_POPMSG);

			}
			if(wParam == AVK_CLR)
            {
                CLOSE_DIALOG(DLGRET_DRAWTOPBAR);
            }
			
            return TRUE;

   
        default:
            break;
    }
    return FALSE;
}

/*==============================================================================
函数：
       TV_DRAWBAR1_HandleEvent
说明：
       IDD_DIALOG1对话框事件处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean TV_DRAWBAR4_HandleEvent(CTVApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_DIALOG4);
    
    if(pMenu == NULL)
    {
        return FALSE;
    }
    switch(eCode) 
    {
        case EVT_DIALOG_INIT:
            IMENUCTL_SetPopMenuRect(pMenu);
			IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetBottomBarType(pMenu, BTBAR_OK_BACK);
				
            return TRUE;
        	
        case EVT_DIALOG_START:

		if(pMe->m_pTV)
            {
                ICAMERAEX_Release(pMe->m_pTV);
                pMe->m_pTV = NULL;
            }
            (void)ISHELL_PostEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL);            
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_USER_REDRAW:

            IMENUCTL_Redraw(pMenu);
				{
				IImage *pTopBarImage2 = NULL;
				IImage *BarImage1 = NULL;
				IImage *BarImageBG = NULL;
				AEEImageInfo myInfo;
				AEEImageInfo bgInfo;
			    pTopBarImage2 = ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_TOOLBAR); 
				BarImage1 =ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_SELECT);
				BarImageBG =ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_SELECT_UNABLE);
				if(pTopBarImage2)
	            {
	             IIMAGE_GetInfo(BarImageBG, &bgInfo);
	            IIMAGE_GetInfo(pTopBarImage2, &myInfo);
				MSG_FATAL("m_cxWidth--%d------m_cyHeight--%d-----myInfo----%d",pMe->m_cxWidth,pMe->m_cyHeight,myInfo.cy);
				IIMAGE_Draw(pTopBarImage2, (pMe->m_cxWidth - myInfo.cx)/2, pMe->m_cyHeight-(myInfo.cy*2)-15);	
                IIMAGE_Draw(BarImage1, ((pMe->m_cxWidth - myInfo.cx)/2)+(bgInfo.cx*3), pMe->m_cyHeight-(myInfo.cy*2)-12);	
	            IIMAGE_Draw(BarImageBG, ((pMe->m_cxWidth - myInfo.cx)/2)+(bgInfo.cx*3), pMe->m_cyHeight-(myInfo.cy*2)-12);



				IIMAGE_Release(pTopBarImage2);
	            pTopBarImage2 = NULL;
	            }
            	}
			//IDISPLAY_Update(pMe->m_pDisplay); //刷屏
            return TRUE;
            
        case EVT_COMMAND:          
            switch(wParam) 
            {
               case IDS_ITEM_CAMERA_GALLERY:
                    break;
                    
                default:         
                    break;  
            }
            return TRUE;        
             
        case EVT_KEY:
			if(wParam == AVK_RIGHT)
			{
			CLOSE_DIALOG(DLGRET_BAR5);//STATE_CPOPMSG
			}

            if(wParam == AVK_SELECT)
            {
            // CLOSE_DIALOG(DLGRET_POPMSG);

			}
			if(wParam == AVK_CLR)
            {
                CLOSE_DIALOG(DLGRET_DRAWTOPBAR);
            }
			
            return TRUE;

   
        default:
            break;
    }
    return FALSE;
}

/*==============================================================================
函数：
       TV_DRAWBAR1_HandleEvent
说明：
       IDD_DIALOG1对话框事件处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean TV_DRAWBAR5_HandleEvent(CTVApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_DIALOG5);
    
    if(pMenu == NULL)
    {
        return FALSE;
    }
    switch(eCode) 
    {
        case EVT_DIALOG_INIT:
            IMENUCTL_SetPopMenuRect(pMenu);
			IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetBottomBarType(pMenu, BTBAR_OK_BACK);
				
            return TRUE;
        	
        case EVT_DIALOG_START:

		if(pMe->m_pTV)
            {
                ICAMERAEX_Release(pMe->m_pTV);
                pMe->m_pTV = NULL;
            }
            (void)ISHELL_PostEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL);            
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_USER_REDRAW:

            IMENUCTL_Redraw(pMenu);
				{
				IImage *pTopBarImage2 = NULL;
				IImage *BarImage1 = NULL;
				IImage *BarImageBG = NULL;
				AEEImageInfo myInfo;
				AEEImageInfo bgInfo;
			    pTopBarImage2 = ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_TOOLBAR); 
				BarImage1 =ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_SELECT);
				BarImageBG =ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_SELECT_UNABLE);
				if(pTopBarImage2)
	            {
	             IIMAGE_GetInfo(BarImageBG, &bgInfo);
	            IIMAGE_GetInfo(pTopBarImage2, &myInfo);
				MSG_FATAL("m_cxWidth--%d------m_cyHeight--%d-----myInfo----%d",pMe->m_cxWidth,pMe->m_cyHeight,myInfo.cy);
				IIMAGE_Draw(pTopBarImage2, (pMe->m_cxWidth - myInfo.cx)/2, pMe->m_cyHeight-(myInfo.cy*2)-15);	
                IIMAGE_Draw(BarImage1, ((pMe->m_cxWidth - myInfo.cx)/2)+(bgInfo.cx*4), pMe->m_cyHeight-(myInfo.cy*2)-12);	
	            IIMAGE_Draw(BarImageBG, ((pMe->m_cxWidth - myInfo.cx)/2)+(bgInfo.cx*4), pMe->m_cyHeight-(myInfo.cy*2)-12);



				IIMAGE_Release(pTopBarImage2);
	            pTopBarImage2 = NULL;
	            }
            	}
			//IDISPLAY_Update(pMe->m_pDisplay); //刷屏
            return TRUE;
            
        case EVT_COMMAND:          
            switch(wParam) 
            {
               case IDS_ITEM_CAMERA_GALLERY:
                    break;
                    
                default:         
                    break;  
            }
            return TRUE;        
             
        case EVT_KEY:
			if(wParam == AVK_RIGHT)
			{
			CLOSE_DIALOG(DLGRET_BAR6);//STATE_CPOPMSG
			}

            if(wParam == AVK_SELECT)
            {
            // CLOSE_DIALOG(DLGRET_POPMSG);

			}
			if(wParam == AVK_CLR)
            {
                CLOSE_DIALOG(DLGRET_DRAWTOPBAR);
            }
			
            return TRUE;

   
        default:
            break;
    }
    return FALSE;
}

/*==============================================================================
函数：
       TV_DRAWBAR1_HandleEvent
说明：
       IDD_DIALOG1对话框事件处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean TV_DRAWBAR6_HandleEvent(CTVApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_DIALOG6);
    
    if(pMenu == NULL)
    {
        return FALSE;
    }
    switch(eCode) 
    {
        case EVT_DIALOG_INIT:
            IMENUCTL_SetPopMenuRect(pMenu);
			IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetBottomBarType(pMenu, BTBAR_OK_BACK);
				
            return TRUE;
        	
        case EVT_DIALOG_START:

		if(pMe->m_pTV)
            {
                ICAMERAEX_Release(pMe->m_pTV);
                pMe->m_pTV = NULL;
            }
            (void)ISHELL_PostEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL);            
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_USER_REDRAW:

            IMENUCTL_Redraw(pMenu);
				{
				IImage *pTopBarImage2 = NULL;
				IImage *BarImage1 = NULL;
				IImage *BarImageBG = NULL;
				AEEImageInfo myInfo;
				AEEImageInfo bgInfo;
			    pTopBarImage2 = ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_TOOLBAR); 
				BarImage1 =ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_SELECT);
				BarImageBG =ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_SELECT_UNABLE);
				if(pTopBarImage2)
	            {
	             IIMAGE_GetInfo(BarImageBG, &bgInfo);
	            IIMAGE_GetInfo(pTopBarImage2, &myInfo);
				MSG_FATAL("m_cxWidth--%d------m_cyHeight--%d-----myInfo----%d",pMe->m_cxWidth,pMe->m_cyHeight,myInfo.cy);
				IIMAGE_Draw(pTopBarImage2, (pMe->m_cxWidth - myInfo.cx)/2, pMe->m_cyHeight-(myInfo.cy*2)-15);	
                IIMAGE_Draw(BarImage1, ((pMe->m_cxWidth - myInfo.cx)/2)+(bgInfo.cx*5), pMe->m_cyHeight-(myInfo.cy*2)-12);	
	            IIMAGE_Draw(BarImageBG, ((pMe->m_cxWidth - myInfo.cx)/2)+(bgInfo.cx*5), pMe->m_cyHeight-(myInfo.cy*2)-12);



				IIMAGE_Release(pTopBarImage2);
	            pTopBarImage2 = NULL;
	            }
            	}
			//IDISPLAY_Update(pMe->m_pDisplay); //刷屏
            return TRUE;
            
        case EVT_COMMAND:          
            switch(wParam) 
            {
               case IDS_ITEM_CAMERA_GALLERY:
                    break;
                    
                default:         
                    break;  
            }
            return TRUE;        
             
        case EVT_KEY:
			if(wParam == AVK_RIGHT)
			{
			CLOSE_DIALOG(DLGRET_BAR7);//STATE_CPOPMSG
			}

            if(wParam == AVK_SELECT)
            {
            // CLOSE_DIALOG(DLGRET_POPMSG);

			}
			if(wParam == AVK_CLR)
            {
                CLOSE_DIALOG(DLGRET_DRAWTOPBAR);
            }
			
            return TRUE;

   
        default:
            break;
    }
    return FALSE;
}

/*==============================================================================
函数：
       TV_DRAWBAR1_HandleEvent
说明：
       IDD_DIALOG1对话框事件处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean TV_DRAWBAR7_HandleEvent(CTVApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_DIALOG7);
    
    if(pMenu == NULL)
    {
        return FALSE;
    }
    switch(eCode) 
    {
        case EVT_DIALOG_INIT:
            IMENUCTL_SetPopMenuRect(pMenu);
			IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetBottomBarType(pMenu, BTBAR_OK_BACK);
				
            return TRUE;
        	
        case EVT_DIALOG_START:

		if(pMe->m_pTV)
            {
                ICAMERAEX_Release(pMe->m_pTV);
                pMe->m_pTV = NULL;
            }
            (void)ISHELL_PostEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL);            
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_USER_REDRAW:

            IMENUCTL_Redraw(pMenu);
				{
				IImage *pTopBarImage2 = NULL;
				IImage *BarImage1 = NULL;
				IImage *BarImageBG = NULL;
				AEEImageInfo myInfo;
				AEEImageInfo bgInfo;
			    pTopBarImage2 = ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_TOOLBAR); 
				BarImage1 =ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_SELECT);
				BarImageBG =ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_SELECT_UNABLE);
				if(pTopBarImage2)
	            {
	             IIMAGE_GetInfo(BarImageBG, &bgInfo);
	            IIMAGE_GetInfo(pTopBarImage2, &myInfo);
				MSG_FATAL("m_cxWidth--%d------m_cyHeight--%d-----myInfo----%d",pMe->m_cxWidth,pMe->m_cyHeight,myInfo.cy);
				IIMAGE_Draw(pTopBarImage2, (pMe->m_cxWidth - myInfo.cx)/2, pMe->m_cyHeight-(myInfo.cy*2)-15);	
                IIMAGE_Draw(BarImage1, ((pMe->m_cxWidth - myInfo.cx)/2)+(bgInfo.cx*6), pMe->m_cyHeight-(myInfo.cy*2)-12);	
	            IIMAGE_Draw(BarImageBG, ((pMe->m_cxWidth - myInfo.cx)/2)+(bgInfo.cx*6), pMe->m_cyHeight-(myInfo.cy*2)-12);



				IIMAGE_Release(pTopBarImage2);
	            pTopBarImage2 = NULL;
	            }
            	}
			//IDISPLAY_Update(pMe->m_pDisplay); //刷屏
            return TRUE;
            
        case EVT_COMMAND:          
            switch(wParam) 
            {
               case IDS_ITEM_CAMERA_GALLERY:
                    break;
                    
                default:         
                    break;  
            }
            return TRUE;        
             
        case EVT_KEY:
			if(wParam == AVK_RIGHT)
			{
			CLOSE_DIALOG(DLGRET_BAR1);//STATE_CPOPMSG
			}
            if(wParam == AVK_SELECT)
            {
            // CLOSE_DIALOG(DLGRET_POPMSG);

			}
			if(wParam == AVK_CLR)
            {
                CLOSE_DIALOG(DLGRET_DRAWTOPBAR);
            }
			
            return TRUE;

   
        default:
            break;
    }
    return FALSE;
}






/*==============================================================================
函数：
       CTVApp_MainOptsMenu_HandleEvent
说明：
       IDD_MAINMENU 对话框事件处理函数
       
参数：
       pMe [in]：指向CMusicPlayer Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean TV_MainOptsMenu_HandleEvent(CTVApp *pMe,
                                            AEEEvent eCode,
                                            uint16 wParam,
                                            uint32 dwParam)
{
  IMenuCtl  *pMenuCtl;
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    MSG_FATAL("TV_MainOptsMenu_HandleEvent Start",0,0,0);

    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_TV_POPMENU);//IDC_CAMERA_MAINMENU
   if (pMenuCtl == NULL)
   {
      MSG_FATAL("TV_MainOptsMenu_HandleEvent  return 0",0,0,0);
       return FALSE;
   }
   MSG_FATAL("TV_MainOptsMenu_HandleEvent---%d",eCode,0,0);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			   TV_Build_MainOpts_Menu(pMe,pMenuCtl);
			   MSG_FATAL("TV_MainOptsMenu_HandleEvent ----------------EVT_DIALOG_INIT",0,0,0);
              return TRUE;
            
        case EVT_DIALOG_START:
        {         
			MSG_FATAL("TV_MainOptsMenu_HandleEvent ----------------EVT_DIALOG_START",0,0,0);
            IMENUCTL_SetPopMenuRect(pMenuCtl);
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetBottomBarType(pMenuCtl,BTBAR_SELECT_BACK);
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_TVAPP,
                                    EVT_USER_REDRAW,
                                    0,
                                    0); 
            return TRUE;
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
			//TV_UpdateInit(pMe);
			MSG_FATAL("TV_MainOptsMenu_HandleEvent ----------------EVT_USER_REDRAW",0,0,0);
            //IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
			
			IMENUCTL_Redraw(pMenuCtl);
			//TV_Update(pMe);
            return TRUE;
            
        case EVT_DIALOG_END:
		{	
            return TRUE;
        }
            
        case EVT_KEY:			
            switch (wParam)
            {
                case AVK_CLR:			
				{
                    CLOSE_DIALOG(DLGRET_MAINMENU);
                    return TRUE;
                }
                case AVK_BGPLAY:
				
                    return TRUE;
                    
                default:
                    break;                    
            }
            break; 
		case EVT_COMMAND:          
            switch(wParam) 
            {
            case IDS_REGION: 
				MSG_FATAL("IDS_REGION----------------------------",0,0,0);
				 CLOSE_DIALOG(DLGRET_REGION);
				break; 
				
			default:
				break;
            }
        default:
            break;
            
    }
    
    return FALSE;
}


/*==============================================================================
函数：
       CTVApp_MainOptsMenu_HandleEvent
说明：
       IDD_MAINMENU 对话框事件处理函数
       
参数：
       pMe [in]：指向CMusicPlayer Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean TV_REGIONMenu_HandleEvent(CTVApp *pMe,
                                            AEEEvent eCode,
                                            uint16 wParam,
                                            uint32 dwParam)
{
  IMenuCtl  *pMenuCtl;
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    MSG_FATAL("TV_REGIONMenu_HandleEvent Start",0,0,0);

    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_REGION);
   if (pMenuCtl == NULL)
   {
      MSG_FATAL("TV_REGIONMenu_HandleEvent  return 0",0,0,0);
       return FALSE;
   }
   MSG_FATAL("TV_REGIONMenu_HandleEvent---%d",eCode,0,0);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
               TV_REGIONMenu(pMe,pMenuCtl);
			   MSG_FATAL("TV_MainOptsMenu_HandleEvent ----------------EVT_DIALOG_INIT",0,0,0);
              return TRUE;
            
        case EVT_DIALOG_START:
        {         
			MSG_FATAL("TV_MainOptsMenu_HandleEvent ----------------EVT_DIALOG_START",0,0,0);
            IMENUCTL_SetPopMenuRect(pMenuCtl);
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetBottomBarType(pMenuCtl,BTBAR_SELECT_BACK);
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_TVAPP,
                                    EVT_USER_REDRAW,
                                    0,
                                    0); 
            return TRUE;
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
			MSG_FATAL("TV_MainOptsMenu_HandleEvent ----------------EVT_USER_REDRAW",0,0,0);
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_Redraw(pMenuCtl);
            return TRUE;
            
        case EVT_DIALOG_END:
		{	
            return TRUE;
        }
		break;
            
        case EVT_KEY:			
            switch (wParam)
            {
                case AVK_CLR:			
				{
                    CLOSE_DIALOG(DLGRET_POPMSG);//DLGRET_CANCELED
                    return TRUE;
                }
                case AVK_BGPLAY:
                    return TRUE;
                    
                default:
                    break;                    
            }
            break;

        case EVT_COMMAND:          
            switch(wParam) 
            {
			case IDS_ASIA: 
				MSG_FATAL("IDS_ASIA----------------------------",0,0,0);
				 CLOSE_DIALOG(DLGRET_ASIA);
				break; 
			case IDS_LATIN: 
				MSG_FATAL("IDS_LATIN----------------------------",0,0,0);
				 CLOSE_DIALOG(DLGRET_LATIN);
				break; 
			case IDS_EUROPE: 
				MSG_FATAL("IDS_EUROPE----------------------------",0,0,0);
				 CLOSE_DIALOG(DLGRET_EUROPE);
				break; 
			case IDS_AFRICA: 
				MSG_FATAL("IDS_AFRICA----------------------------",0,0,0);
				 CLOSE_DIALOG(DLGRET_AFRICA);
				break; 
			case IDS_OCEANIA: 
				MSG_FATAL("IDS_OCEANIA----------------------------",0,0,0);
				 CLOSE_DIALOG(DLGRET_OCEANIA);
				break; 
				
			default:
				break;
            }
		
        default:
            break;
            
    }
    
    return FALSE;
}


/*==============================================================================
函数：
       TV_ASIAMenu_HandleEvent
说明：
       IDD_REGIONASIA 对话框事件处理函数
       
参数：
       pMe [in]：指向TV Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean TV_ASIAMenu_HandleEvent(CTVApp *pMe,
                                            AEEEvent eCode,
                                            uint16 wParam,
                                            uint32 dwParam)
{
  IMenuCtl  *pMenuCtl;
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    MSG_FATAL("TV_REGIONMenu_HandleEvent Start",0,0,0);

    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_REGIONASIA);
   if (pMenuCtl == NULL)
   {
      MSG_FATAL("TV_REGIONMenu_HandleEvent  return 0",0,0,0);
       return FALSE;
   }
   MSG_FATAL("TV_REGIONMenu_HandleEvent---%d",eCode,0,0);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
               TV_ASIAMenu(pMe,pMenuCtl);
			   MSG_FATAL("TV_MainOptsMenu_HandleEvent ----------------EVT_DIALOG_INIT",0,0,0);
              return TRUE;
            
        case EVT_DIALOG_START:
        {         
			MSG_FATAL("TV_MainOptsMenu_HandleEvent ----------------EVT_DIALOG_START",0,0,0);
            //IMENUCTL_SetPopMenuRect(pMenuCtl);
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetBottomBarType(pMenuCtl,BTBAR_SELECT_BACK);
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_TVAPP,
                                    EVT_USER_REDRAW,
                                    0,
                                    0); 
            return TRUE;
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
			MSG_FATAL("TV_MainOptsMenu_HandleEvent ----------------EVT_USER_REDRAW",0,0,0);
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_Redraw(pMenuCtl);
            return TRUE;
            
        case EVT_DIALOG_END:
		{	
            return TRUE;
        }
		break;
            
        case EVT_KEY:			
            switch (wParam)
            {
                case AVK_CLR:			
				{
                    CLOSE_DIALOG(DLGRET_CANCELED);//DLGRET_CANCELED
                    return TRUE;
                }
                case AVK_BGPLAY:
                    return TRUE;
                    
                default:
                    break;                    
            }
            break;
        default:
            break;
            
    }
    
    return FALSE;
}

/*==============================================================================
函数：
       TV_ASIAMenu_HandleEvent
说明：
       IDD_REGIONASIA 对话框事件处理函数
       
参数：
       pMe [in]：指向TV Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean TV_LATINMenu_HandleEvent(CTVApp *pMe,
                                            AEEEvent eCode,
                                            uint16 wParam,
                                            uint32 dwParam)
{
  IMenuCtl  *pMenuCtl;
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    MSG_FATAL("TV_REGIONMenu_HandleEvent Start",0,0,0);

    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_REGIONLATIN);
   if (pMenuCtl == NULL)
   {
      MSG_FATAL("TV_REGIONMenu_HandleEvent  return 0",0,0,0);
       return FALSE;
   }
   MSG_FATAL("TV_REGIONMenu_HandleEvent---%d",eCode,0,0);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
               TV_LATINMenu(pMe,pMenuCtl);
			   MSG_FATAL("TV_MainOptsMenu_HandleEvent ----------------EVT_DIALOG_INIT",0,0,0);
              return TRUE;
            
        case EVT_DIALOG_START:
        {         
			MSG_FATAL("TV_MainOptsMenu_HandleEvent ----------------EVT_DIALOG_START",0,0,0);
           // IMENUCTL_SetPopMenuRect(pMenuCtl);
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetBottomBarType(pMenuCtl,BTBAR_SELECT_BACK);
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_TVAPP,
                                    EVT_USER_REDRAW,
                                    0,
                                    0); 
            return TRUE;
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
			MSG_FATAL("TV_MainOptsMenu_HandleEvent ----------------EVT_USER_REDRAW",0,0,0);
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_Redraw(pMenuCtl);
            return TRUE;
            
        case EVT_DIALOG_END:
		{	
            return TRUE;
        }
		break;
            
        case EVT_KEY:			
            switch (wParam)
            {
                case AVK_CLR:			
				{
                    CLOSE_DIALOG(DLGRET_CANCELED);//DLGRET_CANCELED
                    return TRUE;
                }
                case AVK_BGPLAY:
                    return TRUE;
                    
                default:
                    break;                    
            }
            break;
        default:
            break;
            
    }
    
    return FALSE;
}

/*==============================================================================
函数：
       TV_ASIAMenu_HandleEvent
说明：
       IDD_REGIONASIA 对话框事件处理函数
       
参数：
       pMe [in]：指向TV Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean TV_EUROPEMenu_HandleEvent(CTVApp *pMe,
                                            AEEEvent eCode,
                                            uint16 wParam,
                                            uint32 dwParam)
{
  IMenuCtl  *pMenuCtl;
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    MSG_FATAL("TV_REGIONMenu_HandleEvent Start",0,0,0);

    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_REGIONEUROPE);
   if (pMenuCtl == NULL)
   {
      MSG_FATAL("TV_REGIONMenu_HandleEvent  return 0",0,0,0);
       return FALSE;
   }
   MSG_FATAL("TV_REGIONMenu_HandleEvent---%d",eCode,0,0);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
               TV_EUROPEMenu(pMe,pMenuCtl);
			   MSG_FATAL("TV_MainOptsMenu_HandleEvent ----------------EVT_DIALOG_INIT",0,0,0);
              return TRUE;
            
        case EVT_DIALOG_START:
        {         
			MSG_FATAL("TV_MainOptsMenu_HandleEvent ----------------EVT_DIALOG_START",0,0,0);
            IMENUCTL_SetPopMenuRect(pMenuCtl);
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetBottomBarType(pMenuCtl,BTBAR_SELECT_BACK);
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_TVAPP,
                                    EVT_USER_REDRAW,
                                    0,
                                    0); 
            return TRUE;
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
			MSG_FATAL("TV_MainOptsMenu_HandleEvent ----------------EVT_USER_REDRAW",0,0,0);
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_Redraw(pMenuCtl);
            return TRUE;
            
        case EVT_DIALOG_END:
		{	
            return TRUE;
        }
		break;
            
        case EVT_KEY:			
            switch (wParam)
            {
                case AVK_CLR:			
				{
                    CLOSE_DIALOG(DLGRET_CANCELED);//DLGRET_CANCELED
                    return TRUE;
                }
                case AVK_BGPLAY:
                    return TRUE;
                    
                default:
                    break;                    
            }
            break;
        default:
            break;
            
    }
    
    return FALSE;
}

/*==============================================================================
函数：
       TV_ASIAMenu_HandleEvent
说明：
       IDD_REGIONASIA 对话框事件处理函数
       
参数：
       pMe [in]：指向TV Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean TV_AFRICAMenu_HandleEvent(CTVApp *pMe,
                                            AEEEvent eCode,
                                            uint16 wParam,
                                            uint32 dwParam)
{
  IMenuCtl  *pMenuCtl;
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    MSG_FATAL("TV_REGIONMenu_HandleEvent Start",0,0,0);

    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_REGIONAFRICA);
   if (pMenuCtl == NULL)
   {
      MSG_FATAL("TV_REGIONMenu_HandleEvent  return 0",0,0,0);
       return FALSE;
   }
   MSG_FATAL("TV_REGIONMenu_HandleEvent---%d",eCode,0,0);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
               TV_AFRICAMenu(pMe,pMenuCtl);
			   MSG_FATAL("TV_MainOptsMenu_HandleEvent ----------------EVT_DIALOG_INIT",0,0,0);
              return TRUE;
            
        case EVT_DIALOG_START:
        {         
			MSG_FATAL("TV_MainOptsMenu_HandleEvent ----------------EVT_DIALOG_START",0,0,0);
            //IMENUCTL_SetPopMenuRect(pMenuCtl);
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetBottomBarType(pMenuCtl,BTBAR_SELECT_BACK);
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_TVAPP,
                                    EVT_USER_REDRAW,
                                    0,
                                    0); 
            return TRUE;
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
			MSG_FATAL("TV_MainOptsMenu_HandleEvent ----------------EVT_USER_REDRAW",0,0,0);
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_Redraw(pMenuCtl);
            return TRUE;
            
        case EVT_DIALOG_END:
		{	
            return TRUE;
        }
		break;
            
        case EVT_KEY:			
            switch (wParam)
            {
                case AVK_CLR:			
				{
                    CLOSE_DIALOG(DLGRET_CANCELED);//DLGRET_CANCELED
                    return TRUE;
                }
                case AVK_BGPLAY:
                    return TRUE;
                    
                default:
                    break;                    
            }
            break;
        default:
            break;
            
    }
    
    return FALSE;
}

/*==============================================================================
函数：
       TV_ASIAMenu_HandleEvent
说明：
       IDD_REGIONASIA 对话框事件处理函数
       
参数：
       pMe [in]：指向TV Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean TV_OCEANIAMenu_HandleEvent(CTVApp *pMe,
                                            AEEEvent eCode,
                                            uint16 wParam,
                                            uint32 dwParam)
{
  IMenuCtl  *pMenuCtl;
#if defined(AEE_STATIC)
    ASSERT(pMe != NULL);
#endif
    MSG_FATAL("TV_REGIONMenu_HandleEvent Start",0,0,0);

    pMenuCtl = (IMenuCtl*)IDIALOG_GetControl( pMe->m_pActiveDlg,
                                              IDC_REGIONOCEANIA);
   if (pMenuCtl == NULL)
   {
      MSG_FATAL("TV_REGIONMenu_HandleEvent  return 0",0,0,0);
       return FALSE;
   }
   MSG_FATAL("TV_REGIONMenu_HandleEvent---%d",eCode,0,0);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
               TV_OCEANIAMenu(pMe,pMenuCtl);
			   MSG_FATAL("TV_MainOptsMenu_HandleEvent ----------------EVT_DIALOG_INIT",0,0,0);
              return TRUE;
            
        case EVT_DIALOG_START:
        {         
			MSG_FATAL("TV_MainOptsMenu_HandleEvent ----------------EVT_DIALOG_START",0,0,0);
           // IMENUCTL_SetPopMenuRect(pMenuCtl);
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetBottomBarType(pMenuCtl,BTBAR_SELECT_BACK);
            // For redraw the dialog
            (void)ISHELL_PostEvent( pMe->m_pShell,
                                    AEECLSID_TVAPP,
                                    EVT_USER_REDRAW,
                                    0,
                                    0); 
            return TRUE;
        }
        
        // Note: Fall through to the EVT_USER_REDRAW
        //lint -fallthrough
        
        case EVT_USER_REDRAW:
			MSG_FATAL("TV_MainOptsMenu_HandleEvent ----------------EVT_USER_REDRAW",0,0,0);
            IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_Redraw(pMenuCtl);
            return TRUE;
            
        case EVT_DIALOG_END:
		{	
            return TRUE;
        }
		break;
            
        case EVT_KEY:			
            switch (wParam)
            {
                case AVK_CLR:			
				{
                    CLOSE_DIALOG(DLGRET_CANCELED);//DLGRET_CANCELED
                    return TRUE;
                }
                case AVK_BGPLAY:
                    return TRUE;
                    
                default:
                    break;                    
            }
            break;
        default:
            break;
            
    }
    
    return FALSE;
}

/*==============================================================================
函数：
       TVApp_PreviewHandleEvent
说明：
       IDD_CPREVIEW对话框事件处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean TVApp_PreviewHandleEvent(CTVApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    int   nTVSelfTime = 0;
    
    switch(eCode) 
    {
        case EVT_DIALOG_INIT:
            pMe->m_bCapturePic = FALSE;

            IDISPLAY_SetClipRect(pMe->m_pDisplay, NULL); 


            (void)ICONFIG_GetItem(pMe->m_pConfig,
                                  CFGI_CAMERA_SIZE,
                                  &pMe->m_nTVSize,
                                  sizeof(pMe->m_nTVSize));

            (void)ICONFIG_GetItem(pMe->m_pConfig,
                                  CFGI_CAMERA_QUALITY,
                                  &pMe->m_nTVQuality,
                                  sizeof(pMe->m_nTVQuality));
            

            (void)ICONFIG_GetItem(pMe->m_pConfig,
                                  CFGI_CAMERA_ENVIROMENT,
                                  &pMe->m_nTVEnviroment,
                                  sizeof(pMe->m_nTVEnviroment));

            (void)ICONFIG_GetItem(pMe->m_pConfig,
                                  CFGI_CAMERA_BANDING,
                                  &pMe->m_nTVBanding,
                                  sizeof(pMe->m_nTVBanding));
                                  
            IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
            return TRUE;
            
        case EVT_DIALOG_START:     
        
			//Add By zzg 2010_09_01  			
#ifdef FEATURE_APP_MUSICPLAYER	
			if (app_media_scheduler() == APP_MEDIA_IMPACT_BY_MP3)
			{
				ISHELL_SendEvent(pMe->m_pShell,
				     AEECLSID_APP_MUSICPLAYER,
				     EVT_ALARM,
				     TRUE,
				     TRUE);
			}
#endif         
			//Add End
                	
            pMe->m_bCanCapture = TVApp_IsEnoughfMemorySpace(pMe);
            // For redraw the dialog
            (void)ISHELL_PostEvent(pMe->m_pShell,
                                   AEECLSID_TVAPP,
                                   EVT_USER_REDRAW,
                                   0,
                                   0);  
            return TRUE;
            
        case EVT_DIALOG_END:       

			//Add By zzg 2010_09_01  			
#ifdef FEATURE_APP_MUSICPLAYER
		    if (app_media_scheduler() == APP_MEDIA_IMPACT_BY_MP3)
		    {
		        ISHELL_SendEvent(pMe->m_pShell,
		                         AEECLSID_APP_MUSICPLAYER,
		                         EVT_ALARM,
		                         FALSE,
		                         TRUE);
		    }
 #endif
 			//Add End
        
            ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
            if(pMe->m_bSuspending)
            {
                (void)ICONFIG_SetItem(pMe->m_pConfig,
                                      CFGI_BACK_LIGHT,
                                      &pMe->m_nBacklightVal,
                                      sizeof(pMe->m_nBacklightVal));
            }
            return TRUE;
            
        case EVT_USER_REDRAW:
            // camera preview start....
            if(pMe->m_pTV && (!pMe->m_bIsPreview))
            {
                //TVApp_CPreviewStart(pMe);   不启用cma
                return TRUE;
            }
            
            if(pMe->m_bRePreview && pMe->m_pTV)
            {
               // TVApp_CPreviewStart(pMe);   不启用cma
                pMe->m_bRePreview = FALSE;
                return TRUE;
            }
            
            if(pMe->m_nLeftTime)
            {
                IImage *pImage = NULL;
                
                pImage = ISHELL_LoadResImage(pMe->m_pShell, TVAPP_IMAGE_RES_FILE, IDI_COUNT_DOWN);
                
                if(pImage)
                {
                    IImage_SetDrawSize(pImage, 60, 60);
                    
                    IImage_SetOffset(pImage, (pMe->m_nLeftTime)*60, 0);

                    IImage_Draw(pImage, (pMe->m_rc.dx-60)/2, (pMe->m_rc.dy-60)/2);

                    IImage_Release(pImage);

                    pImage = NULL;
                }
				//-----------------------
               {
				IImage *pTopBarImage2 = NULL;

				 pTopBarImage2 = ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_MID_CAMERA); //IDI_PNG_TV_TOOLBAR
				if(pTopBarImage2)
	            {
	            IIMAGE_Draw(pTopBarImage2, 20, 20);	// + 5
	            IIMAGE_Release(pTopBarImage2);
	            pTopBarImage2 = NULL;
	            }
            	}//------------------------
            }
            
            TVApp_DrawBottomBarText(pMe, BTBAR_OPTION_BACK);
            
            TVApp_DrawMidPic(pMe);
            
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_SELECT: 
                    if(pMe->m_bCapturePic == FALSE)
                    {
                        CLOSE_DIALOG(DLGRET_TVCFG);
                    }
                    break;

                default:
                    break;
            }
            return TRUE;
            
        case EVT_KEY_RELEASE:
        case EVT_KEY_HELD:         
            return TRUE;

        case EVT_KEY_PRESS:           
            // 如果当前在拍照，不处理AVK_END按键以外的按键事件
            if((wParam != AVK_END) && (pMe->m_bCapturePic == TRUE))
            {
                return TRUE;
            }
            
            switch(wParam){
            case AVK_END:
                ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
                if(pMe->m_nTVState == TV_PREVIEW)
                {
                    ICAMERAEX_Stop(pMe->m_pTV);
                    pMe->m_nTVState = TV_STOP;
                }
                return FALSE;

            case AVK_CLR:
                (void)ICONFIG_SetItem(pMe->m_pConfig,
                                      CFGI_BACK_LIGHT,
                                      &pMe->m_nBacklightVal,
                                      sizeof(pMe->m_nBacklightVal));
                IBACKLIGHT_Enable(pMe->m_pBacklight);
                if(pMe->m_isFormQuicktest)
                {
                    ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
                    if(pMe->m_nTVState == TV_PREVIEW)
                    {
                        ICAMERAEX_Stop(pMe->m_pTV);
                        pMe->m_nTVState = TV_STOP;
                    }                        
                    pMe->m_isFormQuicktest = FALSE;
                    ISHELL_CloseApplet(pMe->m_pShell, FALSE);
                    return TRUE;
                }
                if(SUCCESS == ICAMERAEX_Stop(pMe->m_pTV))
                {
                    pMe->m_bIsPreview = FALSE;
                    pMe->m_nTVState = TV_STOP;
                    CLOSE_DIALOG(DLGRET_CANCELED);
                }                    
                break;

            case AVK_INFO:
                // 防止快速按键，导致hotkey Text存在于LCD上 
                ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
                
                if(!pMe->m_bCanCapture)
                {
                    if(!pMe->m_bMemoryCardExist)
                    {
                        pMe->m_wMsgID = IDS_MSG_NOMEMORY;
                    }
                    else
                    {
                        pMe->m_wMsgID = IDS_MSG_NOSDCARD;
                    }
                    pMe->m_nMsgTimeout = TIMEOUT_MS_MSGBOX;
                    ICAMERAEX_Stop(pMe->m_pTV);
                    pMe->m_bIsPreview = FALSE;
                    CLOSE_DIALOG(DLGRET_POPMSG);
                    return TRUE;
                }
                
                switch(pMe->m_nSelfTimeItemSel)
                {
                    case IDS_SELFTIME_OFF:
                        nTVSelfTime = 0;
                        break;

                    case IDS_SELFTIME_SEC1:
                        nTVSelfTime = 6;
                        break;

                    case IDS_SELFTIME_SEC2:
                        nTVSelfTime = 11;
                        break;

                    case IDS_SELFTIME_SEC3:
                        nTVSelfTime = 16;
                        break;

                    default:
                        return FALSE;        
                }
                
                pMe->m_nLeftTime = nTVSelfTime;              
                
                if(nTVSelfTime == 0)
                {
                    pMe->m_nTVState = TV_CAPTURE;
                    ICAMERAEX_Stop(pMe->m_pTV);
                    //TVApp_RecordSnapShot(pMe);
                }
                else
                {
                    // 倒计时拍摄                       
                    TVApp_SelfTimeRecordSnapShot(pMe);
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
}

/*===========================================================================
  在指定位置画图
===========================================================================*/      
static void TVApp_DrawImage(CTVApp *pMe, uint16 ResID, int x, int y)
{
   //如果原来内存未释放，释放之
   if (pMe->m_pImage)
   {
      IIMAGE_Release(pMe->m_pImage);
      pMe->m_pImage = NULL;
   }
   //load图片
   pMe->m_pImage = ISHELL_LoadResImage(pMe->m_pShell, 
                                     TVAPP_IMAGE_RES_FILE, 
                                     ResID);
   if(pMe->m_pImage == NULL)
   {
       return;
   }
   
 
   //画图  
   if(pMe->m_pImage)
   {
      IIMAGE_Draw(pMe->m_pImage,x,y);
   } 
    if (pMe->m_pImage)
   {
      IIMAGE_Release(pMe->m_pImage);
      pMe->m_pImage = NULL;
   }
} 

/*==============================================================================
函数：
       TVApp_TVCFGHandleEvent
说明：
       IDD_CCAMERACFG对话框事件处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean TVApp_TVCFGHandleEvent(CTVApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    IMenuCtl *popMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_CAMERA_CFGMENU);    
    
    if(popMenu == NULL)
    {
        return FALSE;
    }
    switch(eCode) 
    {
        case EVT_DIALOG_INIT:
            IMENUCTL_SetProperties(popMenu, MP_WRAPSCROLL|MP_TRANSPARENT_UNSEL|MP_NO_REDRAW);
            IMENUCTL_SetOemProperties(popMenu, OEMMP_GRAPHIC_UNDERLINE);
            
            TVApp_InitpopMenu(pMe, popMenu);
            IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
            IMENUCTL_SetActive(popMenu, TRUE);
            return TRUE;
     
        case EVT_DIALOG_START:      
        	//Add By zzg 2010_09_01          	
#ifdef FEATURE_APP_MUSICPLAYER
		    if (app_media_scheduler() == APP_MEDIA_IMPACT_BY_MP3)
		    {
		        ISHELL_SendEvent(pMe->m_pShell,
		                         AEECLSID_APP_MUSICPLAYER,
		                         EVT_ALARM,
		                         TRUE,
		                         TRUE);
		    }
 #endif
 			//Add End
            //ISHELL_PostEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL); 
            return TRUE;
         
        case EVT_DIALOG_END:
        
        	//Add By zzg 2010_09_01          	
#ifdef FEATURE_APP_MUSICPLAYER
		    if (app_media_scheduler() == APP_MEDIA_IMPACT_BY_MP3)
		    {
		        ISHELL_SendEvent(pMe->m_pShell,
		                         AEECLSID_APP_MUSICPLAYER,
		                         EVT_ALARM,
		                         FALSE,
		                         TRUE);
		    }
 #endif
 			//Add End
            if(pMe->m_pDisplay != NULL)
            {
                IDISPLAY_SetClipRect(pMe->m_pDisplay, NULL);
            }
            return TRUE;
      
        case EVT_USER_REDRAW:                         
			if(pMe->m_bRePreview)
            {
                TVApp_CPreviewStart(pMe);
                pMe->m_bRePreview = FALSE;
            }
                  
            IDISPLAY_SetClipRect(pMe->m_pDisplay, NULL);

            TVApp_DrawBottomBarText(pMe, BTBAR_SELECT_BACK);
            TVApp_DrawTopBar(pMe);   
            TVApp_DrawCFGPromptText(pMe);                        
            
            IMENUCTL_Redraw(popMenu);
            return TRUE;

        case EVT_KEY:
        case EVT_KEY_RELEASE:
            return TRUE;
            
        case EVT_KEY_PRESS:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;

                case AVK_LEFT:   
                    if(pMe->m_nTVCFG == TVCFGFIRST)
                    {
                        pMe->m_nTVCFG = TVCFGLAST;
                    }
                    else
                    {
                        pMe->m_nTVCFG--;
                    }
                    break;
   
                case AVK_RIGHT:
                    if(pMe->m_nTVCFG == TVCFGLAST)
                    {
					    pMe->m_nTVCFG = TVCFGFIRST;
                    }
                    else
                    {
                        pMe->m_nTVCFG++;
                    }
                    break;

                case AVK_UP:
                case AVK_DOWN:                 
                    return TRUE;

                case AVK_END:
                    ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
                    ICAMERAEX_Stop(pMe->m_pTV);
                    pMe->m_nTVState = TV_STOP;
                    return FALSE;
                    
                default:
                    return TRUE;
            }
            
            //左右键切换不同的设置菜单，需要重新初始化菜单项
            TVApp_InitpopMenu(pMe, popMenu);

            return TRUE;//ISHELL_SendEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL);
            
        case EVT_COMMAND:
            return TVApp_RoutePopMenuCommandEvent(pMe, wParam);
  
        default:
            break;
    }
    return FALSE;
}

/*==============================================================================
函数：
       TVApp_PicHandleEvent
说明：
       IDD_CPIC对话框事件处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean TVApp_PicHandleEvent(CTVApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
    switch(eCode)
    {
        case EVT_DIALOG_INIT:          
            return TRUE;
 
        case EVT_DIALOG_START:   
        	//Add By zzg 2010_09_01  			
#ifdef FEATURE_APP_MUSICPLAYER	
			if (app_media_scheduler() == APP_MEDIA_IMPACT_BY_MP3)
			{
				ISHELL_SendEvent(pMe->m_pShell,
				     AEECLSID_APP_MUSICPLAYER,
				     EVT_ALARM,
				     TRUE,
				     TRUE);
			}
#endif         
			//Add End
			
            ISHELL_PostEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL);
            return TRUE;
            
        //Add By zzg 2010_09_01      
        case EVT_DIALOG_END:
        {        	
#ifdef FEATURE_APP_MUSICPLAYER
		    if (app_media_scheduler() == APP_MEDIA_IMPACT_BY_MP3)
		    {
		        ISHELL_SendEvent(pMe->m_pShell,
		                         AEECLSID_APP_MUSICPLAYER,
		                         EVT_ALARM,
		                         FALSE,
		                         TRUE);
		    } 
#endif 			
 			return TRUE;
 		}
 		//Add End    
            
        case EVT_USER_REDRAW:
            IDISPLAY_SetClipRect(pMe->m_pDisplay, NULL); 
            IDISPLAY_ClearScreen(pMe->m_pDisplay);
            TVApp_HandleSnapshotPic(pMe);
            return TRUE;
            
        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:                                                       
                    if(pMe->m_pFileMgr)
                    {
                        IFILEMGR_Remove(pMe->m_pFileMgr, pMe->m_sCurrentFileName);
                    }
                    break;
                    
                case AVK_SELECT:
                case AVK_INFO:
                    break;
                    
                default:
                    return TRUE;
            } 
            
            pMe->m_wMsgID = IDS_DONE;
            pMe->m_nMsgTimeout = TIMEOUT_MS_MSGDONE;
            CLOSE_DIALOG(DLGRET_POPMSG);
            return TRUE;
 
        case EVT_KEY_PRESS:   
            return TRUE;
 
        case EVT_KEY_RELEASE:
            return TRUE;
 
        case EVT_KEY_HELD:
            return TRUE;  
    }
    return FALSE;

}

/*==============================================================================
函数：
       TVApp_PopMSGHandleEvent
说明：
       IDD_POPMSGC对话框事件处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  TVApp_PopMSGHandleEvent(CTVApp *pMe,
                                            AEEEvent   eCode,
                                            uint16     wParam,
                                            uint32     dwParam)
{
    static IStatic *pStatic = NULL;       
    
    if(NULL == pStatic)
    {
         AEERect rect = {0};
         
         if(AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                                 AEECLSID_STATIC,
                                                 (void **)&pStatic))  
         {
             return FALSE;
         }
         
         ISTATIC_SetRect(pStatic, &rect);  
    }

    switch (eCode)
    {
        case EVT_DIALOG_INIT: 
            return TRUE;
        
        case EVT_DIALOG_START:
        	//Add By zzg 2010_09_01  			
#ifdef FEATURE_APP_MUSICPLAYER	
			if (app_media_scheduler() == APP_MEDIA_IMPACT_BY_MP3)
			{
				ISHELL_SendEvent(pMe->m_pShell,
				     AEECLSID_APP_MUSICPLAYER,
				     EVT_ALARM,
				     TRUE,
				     TRUE);
			}
#endif         
			//Add End
			
            if((pMe->m_nMsgTimeout != 0) && (pMe->m_wMsgID != IDS_MSG_WAITING))
            {
                (void)ISHELL_SetTimer(pMe->m_pShell,
                                      pMe->m_nMsgTimeout,
                                      TVApp_DialogTimeout,
                                      pMe);
            }
            
            // For redraw the dialog
            (void)ISHELL_PostEvent(pMe->m_pShell,
                                   AEECLSID_TVAPP,
                                   EVT_USER_REDRAW,
                                   0,
                                   0);  
   
            return TRUE;                                                                       
            
        case EVT_USER_REDRAW:
            if(pMe->m_ePreState == STATE_CPIC)
            {
                IDISPLAY_ClearScreen(pMe->m_pDisplay);
            }
            
            {             
                PromptMsg_Param_type PromptMsg={0};  
                AECHAR  wstrText[128];  

                if((pMe->m_wMsgID == IDS_CONFIRM_OFF_FM)||(pMe->m_wMsgID == IDS_CONFIRM_OFF_MP))
                {
                    (void)ISHELL_LoadResString(pMe->m_pShell,
                                               AEE_APPSCOMMONRES_LANGFILE,
                                               pMe->m_wMsgID,
                                               wstrText,
                                               sizeof(wstrText));
                }
                else
                {      
                    (void)ISHELL_LoadResString(pMe->m_pShell,
                                               AEE_APPSTVAPP_RES_FILE,
                                               pMe->m_wMsgID,
                                               wstrText,
                                               sizeof(wstrText));
                }
               
                switch(pMe->m_wMsgID)
                {
                    case IDS_MSG_NOSDCARD:
                    case IDS_MSG_NOCAMERA:
                    case IDS_MSG_NOMEMORY:
                    case IDS_MSG_CAPTURE_FAILED:
                        PromptMsg.ePMsgType = MESSAGE_WARNNING;
                        break;

                    case IDS_DONE:
                        PromptMsg.ePMsgType = MESSAGE_INFORMATIVE;
                        break;

                    case IDS_CONFIRM_OFF_FM:
                    case IDS_CONFIRM_OFF_MP:
                        PromptMsg.ePMsgType = MESSAGE_CONFIRM;
                        PromptMsg.eBBarType = BTBAR_OK_BACK;
                        break;

                    case IDS_MSG_WAITING:
                        PromptMsg.ePMsgType = MESSAGE_WAITING;
                        break;
                        
                    default:
                        PromptMsg.ePMsgType = MESSAGE_NONE;
                        break;
                }
                
                PromptMsg.pwszMsg = wstrText;
                
                DrawPromptMessage(pMe->m_pDisplay, pStatic, &PromptMsg);                         
            }  
            
            IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);

            if((pMe->m_wMsgID == IDS_MSG_WAITING) &&(!pMe->m_pTV))
            {
                //CALLBACK_Init(&pMe->m_CallBack,
                //              TVApp_InitTVCheck,
               //               (void *)pMe);          
            
                ISHELL_Resume(pMe->m_pShell, &pMe->m_CallBack);

                (void)ISHELL_PostEvent(pMe->m_pShell,
                                       AEECLSID_TVAPP,
                                       EVT_APP_DIALOG_TIMEOUT,
                                       0,
                                       0);
            }
            
            return TRUE;
            
        case EVT_DIALOG_END:
        	//Add By zzg 2010_09_01  			
#ifdef FEATURE_APP_MUSICPLAYER	
			if (app_media_scheduler() == APP_MEDIA_IMPACT_BY_MP3)
			{
				ISHELL_SendEvent(pMe->m_pShell,
				     AEECLSID_APP_MUSICPLAYER,
				     EVT_ALARM,
				     FALSE,
				     TRUE);
			}
#endif         
			//Add End
			
            if(pStatic != NULL)
            {
                ISTATIC_Release(pStatic); 
            }
            pStatic = NULL;                 
            (void)ISHELL_CancelTimer(pMe->m_pShell,
                                     TVApp_DialogTimeout,
                                     pMe);
            return TRUE;
            
        case EVT_KEY:
            // 进Preview的 时候限制按键
            if(pMe->m_wMsgID == IDS_MSG_WAITING)
            {
                return TRUE;
            }
            
            switch (wParam)
            {
                case AVK_INFO:
                    return TRUE;
                    
                case AVK_CLR:
                     CLOSE_DIALOG(DLGRET_CANCELED);
                     return TRUE;
                     
                case AVK_SELECT:
                    if(pMe->m_nMsgTimeout == 0)
                    {
                        int nMediaDevice;
                        nMediaDevice = app_media_scheduler();

                        switch(nMediaDevice)
                        {
                            case APP_MEDIA_IMPACT_BY_FM:
                                ISHELL_SendEvent(pMe->m_pShell, AEECLSID_APP_FMRADIO, EVT_CLOSEAPP, 0, 0);  
                                break;

                            case APP_MEDIA_IMPACT_BY_MP3:
                                ISHELL_SendEvent(pMe->m_pShell, AEECLSID_APP_MUSICPLAYER, EVT_CLOSEAPP, 0, 0);
                                break;

                            default:
                                break;
                        }
                  
                        CLOSE_DIALOG(DLGRET_MAINMENU);

                    }
                    else
                    {
                        // 如下代码限制在此界面的快速按键
                        (void)ISHELL_SetTimer( pMe->m_pShell,
                                                TIMEOUT_MS_QUICK,
                                                TVApp_DialogTimeout,
                                                pMe );
                    }
                    return TRUE;
                    
                default:
                    break;                    
            }
            return TRUE;

        
        case EVT_COMMAND:
            break;
            
        case EVT_APP_DIALOG_TIMEOUT:  
            if(pMe->m_wMsgID == IDS_MSG_WAITING)
            {
                CLOSE_DIALOG(DLGRET_PREVIEW);
            }
            else
            {
                CLOSE_DIALOG(DLGRET_CANCELED);
            }
            
            return TRUE;         
            
        default:
            break;            
    }
    
    return FALSE;
}

/*==============================================================================
函数：
       TVApp_RoutePopMenuCommandEvent
说明：
       分发设置菜单命令事件处理路由函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean TVApp_RoutePopMenuCommandEvent(CTVApp *pMe, uint16 wParam)
{
    if(NULL == pMe)
    {
        return FALSE;
    }

    if(NULL == pMe->m_pActiveDlg)
    {
        return FALSE;
    }

    if(pMe->m_pActiveDlgID == IDD_CCAMERACFG)
    {
        switch(pMe->m_nTVCFG)
        {
            case TVCFGENVIRMENT:
                return TVApp_PopMenu_EnvironmentCommandHandleEvent(pMe, wParam);

            case TVCFGQUALITY:
                return TVApp_PopMenu_QualityCommandHandleEvent(pMe, wParam);
   
            case TVCFGSIZE:
                return TVApp_PopMenu_SizeCommandHandleEvent(pMe, wParam);
   
            case TVCFGTONE:
                return TVApp_PopMenu_ShutterToneCommandHandleEvent(pMe, wParam);
   
            case TVCFGSELFTIME:
                return TVApp_PopMenu_SelfTimeCommandHandleEvent(pMe, wParam);
   
            case TVCFGBANDING:
                return TVApp_PopMenu_BandingCommandHandleEvent(pMe, wParam);
                
            case TVCFGRESET:
                return TVApp_PopMenu_ResetCFGCommandHandleEvent(pMe, wParam);
   
            default:
                return FALSE;
        }
    }
    return TRUE;
}

/*==============================================================================
函数：
       TVApp_PopMenu_EnvironmentCommandHandleEvent
说明：
       环境设置菜单命令处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

       
备注：
       
==============================================================================*/
static boolean TVApp_PopMenu_EnvironmentCommandHandleEvent(CTVApp *pMe, uint16 wParam)
{ 
    uint32 dwFPS = 5;
        
    switch(pMe->m_pActiveDlgID)
    {
        case IDD_CCAMERACFG:
            switch(wParam)
            {
                case IDS_ENVIR_MODE_AUTO:
                    pMe->m_nTVEnviroment = OEMNV_CAMERA_ENVIR_AUTO;
                    dwFPS = 0;
                    break;

                case IDS_ENVIR_MODE_NIGHT:
                    pMe->m_nTVEnviroment = OEMNV_CAMERA_ENVIR_NIGHT;
                    dwFPS = 3;
                    break;

                case IDS_ENVIR_MODE_SUNNY:
                    pMe->m_nTVEnviroment = OEMNV_CAMERA_ENVIR_SUNNY;
                    dwFPS = 1;
                    break;

                case IDS_ENVIR_MODE_CLOUDY:
                    pMe->m_nTVEnviroment = OEMNV_CAMERA_ENVIR_CLOUDY;
                    dwFPS = 2;
                    break;

                default:
                    break;                 
            }

            (void)ICONFIG_SetItem(pMe->m_pConfig,
                                  CFGI_CAMERA_ENVIROMENT,
                                  &pMe->m_nTVEnviroment,
                                  sizeof(pMe->m_nTVEnviroment));
            
            ICAMERAEX_SetFramesPerSecond(pMe->m_pTV, dwFPS);
            break;

        case IDD_CVIDEOCFG:
            switch(wParam)
            {
                case IDS_ENVIR_MODE_AUTO:
                    pMe->m_nTVEnviroment = OEMNV_CAMERA_ENVIR_AUTO;
                    dwFPS = 0;
                    break;

                case IDS_ENVIR_MODE_NIGHT:
                    pMe->m_nTVEnviroment = OEMNV_CAMERA_ENVIR_NIGHT;
                    dwFPS = 3;
                    break;

                case IDS_ENVIR_MODE_SUNNY:
                    pMe->m_nTVEnviroment = OEMNV_CAMERA_ENVIR_SUNNY;
                    dwFPS = 1;
                    break;

                case IDS_ENVIR_MODE_CLOUDY:
                    pMe->m_nTVEnviroment = OEMNV_CAMERA_ENVIR_CLOUDY;
                    dwFPS = 2;
                    break;

                default:
                    break;                 
            }

            (void)ICONFIG_SetItem(pMe->m_pConfig,
                                  CFGI_VIDEO_ENVIROMENT,
                                  &pMe->m_nTVEnviroment,
                                  sizeof(pMe->m_nTVEnviroment));
            
            ICAMERAEX_SetFramesPerSecond(pMe->m_pTV, dwFPS);
            break;
            
    }
    
    CLOSE_DIALOG(DLGRET_CANCELED); 
    return TRUE;
}

/*==============================================================================
函数：
       TVApp_PopMenu_QualityCommandHandleEvent
说明：
       拍照质量设置菜单命令处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
      
备注：
       
==============================================================================*/
static boolean TVApp_PopMenu_QualityCommandHandleEvent(CTVApp *pMe, uint16 wParam)
{
    int16 quality = 0;
    
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_QUALITY,
                          &pMe->m_nTVQuality,
                          sizeof(pMe->m_nTVQuality));

    if(wParam != pMe->m_nTVQuality)
    {
        switch(wParam)
        {
            case IDS_QUALITY_HIGH:
                pMe->m_nTVQuality = OEMNV_CAMERA_QUALITY_HIGH; 
                quality = CAMERA_APP_QUALITY_HIGH;
                break;

            case IDS_QUALITY_MED:
                pMe->m_nTVQuality = OEMNV_CAMERA_QUALITY_MED;
                quality = CAMERA_APP_QUALITY_MED;
                break;

            case IDS_QUALITY_LOW:
                pMe->m_nTVQuality = OEMNV_CAMERA_QUALITY_LOW;
                quality = CAMERA_APP_QUALITY_LOW;
                break;

            default:
                return FALSE;
        }
        
        (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_CAMERA_QUALITY,
                              &pMe->m_nTVQuality,
                              sizeof(pMe->m_nTVQuality));

        ICAMERAEX_SetQuality(pMe->m_pTV, quality);

        pMe->m_bRePreview = TRUE;
        ICAMERAEX_Stop(pMe->m_pTV);
        pMe->m_nTVState = TV_STOP;
    }
    
    CLOSE_DIALOG(DLGRET_CANCELED);
    return TRUE;
}

/*==============================================================================
函数：
       TVApp_PopMenu_SizeCommandHandleEvent
说明：
       拍照大小设置菜单命令处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean TVApp_PopMenu_SizeCommandHandleEvent(CTVApp *pMe, uint16 wParam)
{  
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_SIZE,
                          &pMe->m_nTVSize,
                          sizeof(pMe->m_nTVSize));

    if(wParam != pMe->m_nTVSize)
    { 
        TVApp_SetTVCaptureSize(pMe, wParam);
        pMe->m_bRePreview = TRUE;
        ICAMERAEX_Stop(pMe->m_pTV);
        pMe->m_nTVState = TV_STOP;
    }
#if defined(FEATURE_DISP_128X128)
    if(pMe->m_nTVSize == OEMNV_CAMERA_SIZE_INDEX_0)
    {
        ICAMERAEX_SetParm(pMe->m_pTV, CAM_PARM_PREVIEWWITHFRAME, 1, 0);
    }
    else
#endif
    {
        ICAMERAEX_SetParm(pMe->m_pTV, CAM_PARM_PREVIEWWITHFRAME, 0, 0);
    }
  
    CLOSE_DIALOG(DLGRET_CANCELED);
    return TRUE;
}

/*==============================================================================
函数：
       TVApp_PopMenu_ShutterToneCommandHandleEvent
说明：
       拍照声音设置菜单命令处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam：事件相关数据。      
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/                                                                                                                    
static boolean TVApp_PopMenu_ShutterToneCommandHandleEvent(CTVApp *pMe, uint16 wParam)
{   
    switch(wParam)
    {
#ifdef FEATURE_CARRIER_CHINA_TELCOM
        case IDS_TONE_SHUTTER2:
            pMe->m_nTVTone = OEMNV_CAMERA_SHUTTER_TONE_SHUTTER2; 
            break;

        case IDS_TONE_SHUTTER1:
            pMe->m_nTVTone = OEMNV_CAMERA_SHUTTER_TONE_SHUTTER1;
            break;
#else    
        case IDS_TONE_DISABLE:
            pMe->m_nTVTone = OEMNV_CAMERA_SHUTTER_TONE_DISABLE; 
            break;

        case IDS_TONE_ENABLE:
            pMe->m_nTVTone = OEMNV_CAMERA_SHUTTER_TONE_ENABLE;
            break;
#endif			

        default:
            return FALSE;
    }
    
    (void)ICONFIG_SetItem(pMe->m_pConfig,
                          CFGI_CAMERA_TONE,
                          &pMe->m_nTVTone,
                          sizeof(pMe->m_nTVTone));
        
    CLOSE_DIALOG(DLGRET_CANCELED);
    return TRUE;
}

/*==============================================================================
函数：
       TVApp_PopMenu_SelfTimeCommandHandleEvent
说明：
       拍照时间设置菜单命令处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/                                                                                                                       
static boolean TVApp_PopMenu_SelfTimeCommandHandleEvent(CTVApp *pMe, uint16 wParam)
{         
    switch(wParam)
    {
        case IDS_SELFTIME_OFF:
            pMe->m_nSelfTimeItemSel = IDS_SELFTIME_OFF;
            break;

        case IDS_SELFTIME_SEC1:
            pMe->m_nSelfTimeItemSel = IDS_SELFTIME_SEC1;
            break;

        case IDS_SELFTIME_SEC2:
            pMe->m_nSelfTimeItemSel = IDS_SELFTIME_SEC2;
            break;

        case IDS_SELFTIME_SEC3:
            pMe->m_nSelfTimeItemSel = IDS_SELFTIME_SEC3;
            break;

        default:
            return FALSE;
    }
    
    CLOSE_DIALOG(DLGRET_CANCELED); 
    return TRUE;
}

/*==============================================================================
函数：
       TVApp_PopMenu_BandingCommandHandleEvent
说明：
       Banding设置菜单命令处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
      
备注：
       
==============================================================================*/                                                                                                                    
static boolean TVApp_PopMenu_BandingCommandHandleEvent(CTVApp *pMe, uint16 wParam)
{  
    switch(wParam)
    {
        case IDS_BANDING_MODE1:
            pMe->m_nTVBanding = OEMNV_CAMERA_BANDING_50HZ;
            break;

        case IDS_BANDING_MODE2:
            pMe->m_nTVBanding = OEMNV_CAMERA_BANDING_60HZ;
            break;

        default:
            return FALSE;
    }
    ICAMERAEX_SetParm(pMe->m_pTV, CAM_PARM_BANDING, pMe->m_nTVBanding, 0);
    (void)ICONFIG_SetItem(pMe->m_pConfig,
                          CFGI_CAMERA_BANDING,
                          &pMe->m_nTVBanding,
                          sizeof(pMe->m_nTVBanding));
    
    CLOSE_DIALOG(DLGRET_CANCELED);
    return TRUE;
}

/*==============================================================================
函数：
       TVApp_PopMenu_ResetCFGCommandHandleEvent
说明：
       恢复默认设置菜单命令处理函数
       
参数：
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。 
       
备注：
       
==============================================================================*/
static boolean TVApp_PopMenu_ResetCFGCommandHandleEvent(CTVApp *pMe, uint16 wParam)
{    
    if(wParam == IDS_CFG_RESET)
    {
        TVApp_InitCFGData(pMe);    
    }

    if(SUCCESS == ICAMERAEX_Stop(pMe->m_pTV))
    {
        pMe->m_bIsPreview = FALSE;
        pMe->m_nTVState = TV_STOP;
        CLOSE_DIALOG(DLGRET_CANCELED);
    }
    
    return TRUE;
}

/*添加OPT主菜单*/
static void TV_Build_MainOpts_Menu(CTVApp *pMe,IMenuCtl *pMenuCtl)
{
    TVApp_SetPopMenuRect(pMe, pMenuCtl, 5);
    TVMENU_ADDITEM(pMenuCtl,IDS_TV_FAVORITE);
    TVMENU_ADDITEM(pMenuCtl,IDS_REGION);
    TVMENU_ADDITEM(pMenuCtl,IDS_AUTO_SCAN);
    TVMENU_ADDITEM(pMenuCtl,IDS_LANDSCAPE);
	TVMENU_ADDITEM(pMenuCtl,IDS_PORTRAIT);
         
}

/*添加REGIONMenu主菜单*/
static void TV_REGIONMenu(CTVApp *pMe,IMenuCtl *pMenuCtl)
{
    TVApp_SetPopMenuRect(pMe, pMenuCtl, 5);
    TVMENU_ADDITEM(pMenuCtl,IDS_ASIA);
    TVMENU_ADDITEM(pMenuCtl,IDS_LATIN);
    TVMENU_ADDITEM(pMenuCtl,IDS_EUROPE);
    TVMENU_ADDITEM(pMenuCtl,IDS_AFRICA);
	TVMENU_ADDITEM(pMenuCtl,IDS_OCEANIA);
         
}

/*添加ASIAMenu主菜单*/
static void TV_ASIAMenu(CTVApp * pMe,IMenuCtl * pMenuCtl)
{
 
    TVMENU_ADDITEM(pMenuCtl,IDS_CHINA);
    TVMENU_ADDITEM(pMenuCtl,IDS_SHENZHEN);
    TVMENU_ADDITEM(pMenuCtl,IDS_TAIWAN);
    TVMENU_ADDITEM(pMenuCtl,IDS_HONGKONG);
	TVMENU_ADDITEM(pMenuCtl,IDS_KOREA);
	TVMENU_ADDITEM(pMenuCtl,IDS_PHILIPPINES);
	TVMENU_ADDITEM(pMenuCtl,IDS_VIETNAM);
	TVMENU_ADDITEM(pMenuCtl,IDS_AFGHANISTAN);
	TVMENU_ADDITEM(pMenuCtl,IDS_SINGAPORE);
	TVMENU_ADDITEM(pMenuCtl,IDS_THAILAND);
	TVMENU_ADDITEM(pMenuCtl,IDS_CAMBODIA);
	TVMENU_ADDITEM(pMenuCtl,IDS_INDONESIA);
	TVMENU_ADDITEM(pMenuCtl,IDS_MALAYSIA);
	TVMENU_ADDITEM(pMenuCtl,IDS_LAOS);
	TVMENU_ADDITEM(pMenuCtl,IDS_PAKISTAN);
	TVMENU_ADDITEM(pMenuCtl,IDS_JAPAN);
	TVMENU_ADDITEM(pMenuCtl,IDS_INDIA);
	TVMENU_ADDITEM(pMenuCtl,IDS_BURMA);
	TVMENU_ADDITEM(pMenuCtl,IDS_TURKEY);
	TVMENU_ADDITEM(pMenuCtl,IDS_UAE);
	TVMENU_ADDITEM(pMenuCtl,IDS_NORTHKOREA);
	TVMENU_ADDITEM(pMenuCtl,IDS_MONGOLIA);
	TVMENU_ADDITEM(pMenuCtl,IDS_SAUDIARABIA);
	TVMENU_ADDITEM(pMenuCtl,IDS_AZERBAIJAN);
	TVMENU_ADDITEM(pMenuCtl,IDS_IRAQ);
	
         
}

/*添加LATINMenu主菜单*/
static void TV_LATINMenu(CTVApp * pMe,IMenuCtl * pMenuCtl)
{
 
    TVMENU_ADDITEM(pMenuCtl,IDS_USA);
    TVMENU_ADDITEM(pMenuCtl,IDS_CANADA);
    TVMENU_ADDITEM(pMenuCtl,IDS_MEXICO);
    TVMENU_ADDITEM(pMenuCtl,IDS_CHILE);
	TVMENU_ADDITEM(pMenuCtl,IDS_VENEZUELA);
	TVMENU_ADDITEM(pMenuCtl,IDS_BRAZIL);
	TVMENU_ADDITEM(pMenuCtl,IDS_ARGENTINA);
	TVMENU_ADDITEM(pMenuCtl,IDS_PERU);
	TVMENU_ADDITEM(pMenuCtl,IDS_JAMAICA);
	TVMENU_ADDITEM(pMenuCtl,IDS_BOLIVIA);
	TVMENU_ADDITEM(pMenuCtl,IDS_ECUADOR);
	TVMENU_ADDITEM(pMenuCtl,IDS_SURINAME);
         
}
/*添加EUROPEMenu主菜单*/
static void TV_EUROPEMenu(CTVApp * pMe,IMenuCtl * pMenuCtl)
{
 
    TVMENU_ADDITEM(pMenuCtl,IDS_WESTERNEUROPE);
    TVMENU_ADDITEM(pMenuCtl,IDS_PORTUGAL);
    TVMENU_ADDITEM(pMenuCtl,IDS_SWEDEN);
    TVMENU_ADDITEM(pMenuCtl,IDS_SPAIN);
	TVMENU_ADDITEM(pMenuCtl,IDS_UK);
	TVMENU_ADDITEM(pMenuCtl,IDS_CZETH);
	TVMENU_ADDITEM(pMenuCtl,IDS_POLAND);
	TVMENU_ADDITEM(pMenuCtl,IDS_RUSSION);
	TVMENU_ADDITEM(pMenuCtl,IDS_UKRAINE);
	TVMENU_ADDITEM(pMenuCtl,IDS_GREECE);
         
}

/*添加AFRICAMenu主菜单*/
static void TV_AFRICAMenu(CTVApp * pMe,IMenuCtl * pMenuCtl)
{
 
    TVMENU_ADDITEM(pMenuCtl,IDS_SOUTHAFRICA);
    TVMENU_ADDITEM(pMenuCtl,IDS_SUDAN);
    TVMENU_ADDITEM(pMenuCtl,IDS_YEMEN);
    TVMENU_ADDITEM(pMenuCtl,IDS_TUNISIA);
	TVMENU_ADDITEM(pMenuCtl,IDS_EGYPT);
	TVMENU_ADDITEM(pMenuCtl,IDS_MOROCCO);
         
}

/*添加OCEANIAMenu主菜单*/
static void TV_OCEANIAMenu(CTVApp * pMe,IMenuCtl * pMenuCtl)
{
 
    TVMENU_ADDITEM(pMenuCtl,IDS_AUSTRALIA);
    TVMENU_ADDITEM(pMenuCtl,NEWZEALAND);
         
}



/*==============================================================================
函数：
       TVApp_SetCFGMenuIcon
说明：
       设置菜单项选中图标
       
参数：      
       pm [in]：Imenuctl菜单控件
       listId:  选中的菜单项
       bSel:    选中条有无的判断
             
返回值： 
       
备注：
       
==============================================================================*/
static void TVApp_SetCFGMenuIcon(IMenuCtl *pm, uint16 listId, boolean bSel)
{
    CtlAddItem cai;
    MEMSET(&cai, 0, sizeof(cai));
    cai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
    cai.wImage = bSel ? IDB_RADIO_FILLED : IDB_RADIO_UNFILLED;
    (void)IMENUCTL_SetItem(pm, listId, MSIF_IMAGE, &cai);
    if (bSel)
    {
        IMENUCTL_SetSel (pm, listId);
    }
    IMENUCTL_SetStyle(pm, NULL, NULL);
}

/*==============================================================================
函数：
       TVApp_InitpopMenu
说明：
       初始化设置菜单
       
参数：      
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
       popMenu [in]: 设置弹出菜单
             
返回值： 
       TRUE: 初始化弹出菜单完成
       
备注：
       
==============================================================================*/
static boolean TVApp_InitpopMenu(CTVApp *pMe, IMenuCtl *popMenu)
{  
    if(pMe->m_pActiveDlgID == IDD_CCAMERACFG)
    {
        switch(pMe->m_nTVCFG)
        {
            case TVCFGENVIRMENT:
                TVApp_PopMenu_EnvironmentInit(pMe, popMenu);
                break;

            case TVCFGQUALITY:
                TVApp_PopMenu_QualityInit(pMe, popMenu);
                break;
   
            case TVCFGSIZE:
                TVApp_PopMenu_SizeInit(pMe, popMenu);
                break;
   
            case TVCFGTONE:
                TVApp_PopMenu_ShutterToneInit(pMe, popMenu);
                break;
   
            case TVCFGSELFTIME:
                TVApp_PopMenu_SelfTimeInit(pMe, popMenu);
                break;
   
            case TVCFGBANDING:
                TVApp_PopMenu_BandingInit(pMe, popMenu);
                break;
                
            case TVCFGRESET:
                TVApp_PopMenu_ResetCFGInit(pMe, popMenu);
                break;
                
            default:
                break; 
        }
    }
    return TRUE;
}

/*==============================================================================
函数：
       TVApp_InitCFGData
说明：
       重置TV cfg项为默认值
       
参数：      
       pMe [in]：指向TVApp Applet对象结构的指针。该结构包含小程序的特定信息。
             
返回值： 
       
备注：
       
==============================================================================*/
static void TVApp_InitCFGData(CTVApp * pMe)
{
    if(pMe->m_pActiveDlgID == IDD_CCAMERACFG)
    {
        pMe->m_nTVEnviroment = OEMNV_CAMERA_ENVIR_AUTO;
        pMe->m_nTVQuality = OEMNV_CAMERA_QUALITY_HIGH;       
#ifdef FEATURE_CARRIER_CHINA_TELCOM
        pMe->m_nTVTone = OEMNV_CAMERA_SHUTTER_TONE_SHUTTER1;
#else			
        pMe->m_nTVTone = OEMNV_CAMERA_SHUTTER_TONE_ENABLE;
#endif
        pMe->m_nTVBanding = OEMNV_CAMERA_BANDING_50HZ;        

		/*
        if(TVApp_FindMemoryCardExist(pMe))
        {
            pMe->m_nTVStorage = OEMNV_CAMERA_STORAGE_MEMORY_CARD;
	        pMe->m_nTVSize = OEMNV_CAMERA_SIZE_DEFAULT;
        }

		
        else
        {
            pMe->m_nTVStorage = OEMNV_CAMERA_STORAGE_PHONE;
            pMe->m_nTVSize = OEMNV_CAMERA_SIZE_DEFAULT;
        }
        */

		//手机内存小，所以固定存储位置是T卡
		//Add By zzg 2010_07_25
		pMe->m_nTVStorage = OEMNV_CAMERA_STORAGE_MEMORY_CARD;
	    pMe->m_nTVSize = OEMNV_CAMERA_SIZE_DEFAULT;
		//Add End
        
        pMe->m_nSelfTimeItemSel = IDS_SELFTIME_OFF;
 
        (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_CAMERA_ENVIROMENT,
                              &pMe->m_nTVEnviroment,
                              sizeof(pMe->m_nTVEnviroment));

        (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_CAMERA_QUALITY,
                              &pMe->m_nTVQuality,
                              sizeof(pMe->m_nTVQuality));

        (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_CAMERA_SIZE,
                              &pMe->m_nTVSize,
                              sizeof(pMe->m_nTVSize));

        (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_CAMERA_TONE,
                              &pMe->m_nTVTone,
                              sizeof(pMe->m_nTVTone));

        (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_CAMERA_BANDING,
                              &pMe->m_nTVBanding,
                              sizeof(pMe->m_nTVBanding));

        (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_CAMERA_STORAGE,
                              &pMe->m_nTVStorage,
                              sizeof(pMe->m_nTVStorage)); 
    }
}

static void TVApp_PopMenu_EnvironmentInit(CTVApp *pMe, IMenuCtl *popMenu)
{
    IMENUCTL_DeleteAll(popMenu);
    TVApp_SetPopMenuRect(pMe, popMenu, 4);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSTVAPP_RES_FILE, 
                     IDS_ENVIR_MODE_AUTO, 
                     IDS_ENVIR_MODE_AUTO, 
                     NULL, 
                     0);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSTVAPP_RES_FILE,
                     IDS_ENVIR_MODE_NIGHT, 
                     IDS_ENVIR_MODE_NIGHT, 
                     NULL,
                     0);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSTVAPP_RES_FILE, 
                     IDS_ENVIR_MODE_SUNNY, 
                     IDS_ENVIR_MODE_SUNNY, 
                     NULL,
                     0);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSTVAPP_RES_FILE, 
                     IDS_ENVIR_MODE_CLOUDY, 
                     IDS_ENVIR_MODE_CLOUDY, 
                     NULL, 
                     0);
    
    InitMenuIcons(popMenu);
    
    switch(pMe->m_pActiveDlgID)
    {
        case IDD_CCAMERACFG:
            (void)ICONFIG_GetItem(pMe->m_pConfig,
                                  CFGI_CAMERA_ENVIROMENT,
                                  &pMe->m_nTVEnviroment,
                                  sizeof(pMe->m_nTVEnviroment));
    
            switch(pMe->m_nTVEnviroment)
            {
                case OEMNV_CAMERA_ENVIR_AUTO:
                    TVApp_SetCFGMenuIcon(popMenu, IDS_ENVIR_MODE_AUTO, TRUE);
                    break;
    
                case OEMNV_CAMERA_ENVIR_NIGHT:
                    TVApp_SetCFGMenuIcon(popMenu, IDS_ENVIR_MODE_NIGHT, TRUE);
                    break;
    
                case OEMNV_CAMERA_ENVIR_SUNNY:
                    TVApp_SetCFGMenuIcon(popMenu, IDS_ENVIR_MODE_SUNNY, TRUE);
                    break;
    
                case OEMNV_CAMERA_ENVIR_CLOUDY:
                    TVApp_SetCFGMenuIcon(popMenu, IDS_ENVIR_MODE_CLOUDY, TRUE);
                    break;

                default:
                    break;
            }
    
            break;
    
        case IDD_CVIDEOCFG:
            (void)ICONFIG_GetItem(pMe->m_pConfig,
                                  CFGI_VIDEO_ENVIROMENT,
                                  &pMe->m_nTVEnviroment,
                                  sizeof(pMe->m_nTVEnviroment));
    
            switch(pMe->m_nTVEnviroment)
            {
                case OEMNV_CAMERA_ENVIR_AUTO:
                    TVApp_SetCFGMenuIcon(popMenu, IDS_ENVIR_MODE_AUTO, TRUE);
                    break;
    
                case OEMNV_CAMERA_ENVIR_NIGHT:
                    TVApp_SetCFGMenuIcon(popMenu, IDS_ENVIR_MODE_NIGHT, TRUE);
                    break;
    
                case OEMNV_CAMERA_ENVIR_SUNNY:
                    TVApp_SetCFGMenuIcon(popMenu, IDS_ENVIR_MODE_SUNNY, TRUE);
                    break;
    
                case OEMNV_CAMERA_ENVIR_CLOUDY:
                    TVApp_SetCFGMenuIcon(popMenu, IDS_ENVIR_MODE_CLOUDY, TRUE);
                    break;
    
                default:
                    break;
            }
    
            break;
                        
        default:
            break;
    }

}

static void TVApp_PopMenu_QualityInit(CTVApp *pMe, IMenuCtl *popMenu)
{
    IMENUCTL_DeleteAll(popMenu);
    TVApp_SetPopMenuRect(pMe, popMenu, 3);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSTVAPP_RES_FILE, 
                     IDS_QUALITY_HIGH, 
                     IDS_QUALITY_HIGH, 
                     NULL,
                     0);
    IMENUCTL_AddItem(popMenu,
                     AEE_APPSTVAPP_RES_FILE, 
                     IDS_QUALITY_MED,
                     IDS_QUALITY_MED, 
                     NULL, 
                     0);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSTVAPP_RES_FILE, 
                     IDS_QUALITY_LOW,
                     IDS_QUALITY_LOW, 
                     NULL, 
                     0);            
    InitMenuIcons(popMenu);
    
                
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_QUALITY,
                          &pMe->m_nTVQuality,
                          sizeof(pMe->m_nTVQuality));
                                           
    switch(pMe->m_nTVQuality)
    {
        case OEMNV_CAMERA_QUALITY_HIGH:
            TVApp_SetCFGMenuIcon(popMenu, IDS_QUALITY_HIGH, TRUE);
            break;
                
        case OEMNV_CAMERA_QUALITY_MED:
            TVApp_SetCFGMenuIcon(popMenu, IDS_QUALITY_MED, TRUE);
            break;
                
        case OEMNV_CAMERA_QUALITY_LOW:
            TVApp_SetCFGMenuIcon(popMenu, IDS_QUALITY_LOW, TRUE);
            break;
                
        default:
            break;  
    }
}
           
static void TVApp_PopMenu_SizeInit(CTVApp *pMe, IMenuCtl *popMenu)
{
    IMENUCTL_DeleteAll(popMenu);
    if(pMe->m_nTVStorage == OEMNV_CAMERA_STORAGE_MEMORY_CARD)
    {
        int i=0;

        while(1)
        {
            if(pMe->m_sensor_model == 30)
            {
                if(g_TVSizeCFG[i].dx == 0)
                {
                    break;
                }
                IMENUCTL_AddItem(popMenu, 
                                 NULL, 
                                 0,
                                 i, 
                                 g_TVSizeCFG[i].pStr, 
                                 NULL);
                i++;
            }
            else if(pMe->m_sensor_model == 10)
            {
                if(g_TVSizeCFG_10[i].dx == 0)
                {
                    break;
                }
                IMENUCTL_AddItem(popMenu, 
                                 NULL, 
                                 0,
                                 i, 
                                 g_TVSizeCFG_10[i].pStr, 
                                 NULL);
                i++;            
            }
        }
        
        TVApp_SetPopMenuRect(pMe, popMenu, i);
    }
    else
    {
        if(pMe->m_sensor_model == 30)
        {
            IMENUCTL_AddItem(popMenu, 
                             NULL, 
                             0,
                             0, 
                             g_TVSizeCFG[OEMNV_CAMERA_SIZE_DEFAULT].pStr, 
                             NULL);
        }
        else if(pMe->m_sensor_model == 10)
        {
            IMENUCTL_AddItem(popMenu, 
                             NULL, 
                             0,
                             0, 
                             g_TVSizeCFG_10[OEMNV_CAMERA_SIZE_DEFAULT].pStr, 
                             NULL);        
        }
        TVApp_SetPopMenuRect(pMe, popMenu, 1);
    }
    
    InitMenuIcons(popMenu);

    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_SIZE,
                          &pMe->m_nTVSize,
                          sizeof(pMe->m_nTVSize));

    TVApp_SetCFGMenuIcon(popMenu, pMe->m_nTVSize, TRUE);
}
         
static void TVApp_PopMenu_ShutterToneInit(CTVApp *pMe, IMenuCtl *popMenu)  
{
    IMENUCTL_DeleteAll(popMenu);   
    TVApp_SetPopMenuRect(pMe, popMenu, 2);
#ifdef FEATURE_CARRIER_CHINA_TELCOM
    IMENUCTL_AddItem(popMenu,
                     AEE_APPSTVAPP_RES_FILE, 
                     IDS_TONE_SHUTTER1, 
                     IDS_TONE_SHUTTER1,
                     NULL,
                     0);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSTVAPP_RES_FILE,
                     IDS_TONE_SHUTTER2, 
                     IDS_TONE_SHUTTER2, 
                     NULL, 
                     0);
#else		
    IMENUCTL_AddItem(popMenu,
                     AEE_APPSTVAPP_RES_FILE, 
                     IDS_TONE_DISABLE, 
                     IDS_TONE_DISABLE,
                     NULL,
                     0);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSTVAPP_RES_FILE,
                     IDS_TONE_ENABLE, 
                     IDS_TONE_ENABLE, 
                     NULL, 
                     0);            
#endif	
    InitMenuIcons(popMenu);
    
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_TONE,
                          &pMe->m_nTVTone,
                          sizeof(pMe->m_nTVTone));
    
    switch(pMe->m_nTVTone)
    {
#ifdef FEATURE_CARRIER_CHINA_TELCOM
        case OEMNV_CAMERA_SHUTTER_TONE_SHUTTER2:
            TVApp_SetCFGMenuIcon(popMenu, IDS_TONE_SHUTTER2, TRUE);
            break;
    
        case OEMNV_CAMERA_SHUTTER_TONE_SHUTTER1:
            TVApp_SetCFGMenuIcon(popMenu, IDS_TONE_SHUTTER1, TRUE);
            break;
#else        
        case OEMNV_CAMERA_SHUTTER_TONE_DISABLE:
            TVApp_SetCFGMenuIcon(popMenu, IDS_TONE_DISABLE, TRUE);
            break;
    
        case OEMNV_CAMERA_SHUTTER_TONE_ENABLE:
            TVApp_SetCFGMenuIcon(popMenu, IDS_TONE_ENABLE, TRUE);
            break;
#endif			
    
        default:
            break;  
    }
}

static void TVApp_PopMenu_SelfTimeInit(CTVApp *pMe, IMenuCtl *popMenu)
{
    IMENUCTL_DeleteAll(popMenu);   
    TVApp_SetPopMenuRect(pMe, popMenu, 4);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSTVAPP_RES_FILE, 
                     IDS_SELFTIME_OFF, 
                     IDS_SELFTIME_OFF, 
                     NULL,
                     0);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSTVAPP_RES_FILE, 
                     IDS_SELFTIME_SEC1, 
                     IDS_SELFTIME_SEC1, 
                     NULL, 
                     0);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSTVAPP_RES_FILE, 
                     IDS_SELFTIME_SEC2, 
                     IDS_SELFTIME_SEC2, 
                     NULL, 
                     0);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSTVAPP_RES_FILE,
                     IDS_SELFTIME_SEC3,
                     IDS_SELFTIME_SEC3,
                     NULL, 
                     0);         
    InitMenuIcons(popMenu);
    TVApp_SetCFGMenuIcon(popMenu, pMe->m_nSelfTimeItemSel, TRUE);
}
            
static void TVApp_PopMenu_BandingInit(CTVApp *pMe, IMenuCtl *popMenu)
{
    IMENUCTL_DeleteAll(popMenu);  
    TVApp_SetPopMenuRect(pMe, popMenu, 2);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSTVAPP_RES_FILE, 
                     IDS_BANDING_MODE1, 
                     IDS_BANDING_MODE1, 
                     NULL, 
                     0);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSTVAPP_RES_FILE, 
                     IDS_BANDING_MODE2, 
                     IDS_BANDING_MODE2, 
                     NULL, 
                     0);
               
    InitMenuIcons(popMenu);
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_BANDING,
                          &pMe->m_nTVBanding,
                          sizeof(pMe->m_nTVBanding));
    
    switch(pMe->m_nTVBanding)
    {
        case OEMNV_CAMERA_BANDING_50HZ:
            TVApp_SetCFGMenuIcon(popMenu, IDS_BANDING_MODE1, TRUE);
            break;
    
        case OEMNV_CAMERA_BANDING_60HZ:
            TVApp_SetCFGMenuIcon(popMenu, IDS_BANDING_MODE2, TRUE);
            break;
    
        default:
            break;
    }
}

static void TVApp_PopMenu_ResetCFGInit(CTVApp *pMe, IMenuCtl *popMenu)
{
    IMENUCTL_DeleteAll(popMenu);   
    TVApp_SetPopMenuRect(pMe, popMenu, 1);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSTVAPP_RES_FILE, 
                     IDS_CFG_RESET, 
                     IDS_CFG_RESET, 
                     NULL, 
                     0);   

}

static void TVApp_PlayShutterSound(CTVApp *pMe)
{
    AEEMediaCmdNotify cmd;
    
    // 如果pMe->m_pMedia接口为空，创建接口
    if(!pMe->m_pMedia)
    {
        AEEMediaData      md;
       
        if(!pMe)
           return;

        md.clsData = MMD_FILE_NAME;  		   
        md.pData = (void *)"fs:/cameraapp/shutter.mid";
        md.dwSize = 0;
       
        (void)AEEMediaUtil_CreateMedia(pMe->m_pShell, &md, &pMe->m_pMedia);
    }
   
    if(pMe->m_pMedia)
    {        
        IMEDIA_SetVolume(pMe->m_pMedia, AEE_MAX_VOLUME); //max volum is 100
     
        if(IMEDIA_RegisterNotify(pMe->m_pMedia, TVApp_MediaNotify, pMe) != SUCCESS)
        {
            cmd.nCmd    = MM_CMD_PLAY;
            cmd.nStatus = MM_STATUS_DONE;
            TVApp_MediaNotify((void *)pMe, &cmd);
            return;
        }

        if(IMEDIA_Play(pMe->m_pMedia) != SUCCESS)
        {
            cmd.nCmd    = MM_CMD_PLAY;
            cmd.nStatus = MM_STATUS_DONE;
            TVApp_MediaNotify((void *)pMe, &cmd);
            return;
        }
    }
    else
    {
        cmd.nCmd    = MM_CMD_PLAY;
        cmd.nStatus = MM_STATUS_DONE;
        TVApp_MediaNotify((void *)pMe, &cmd);
    }
}

static void TVApp_MediaNotify(void *pUser, AEEMediaCmdNotify *pCmdNotify)
{
    CTVApp *pMe = (CTVApp *)pUser;

    if(!pMe || !pCmdNotify)
        return;

    if(pCmdNotify->nCmd == MM_CMD_PLAY)  // IMEDIA_Play events
    {
        switch (pCmdNotify->nStatus)
        {
            case MM_STATUS_ABORT:            
                break;

            case MM_STATUS_DONE:    // playback done
                    //IMEDIA_Play(pMe->m_pMedia);              
#ifdef FEATURE_CARRIER_CHINA_TELCOM                    
                    pMe->m_pMedia = NULL;
#endif
                break;
            default:
                break;
        }
    }
}

/*void TVApp_AlphaDisplay(CTVApp *pMe, IMenuCtl *popMenu)
{
    IDisplay *pdis = pMe->m_pDisplay;
    IBitmap  *pBgBitmap=NULL;
   // IBitmap  *popMenuBg = NULL;
    IDIB     *pBgDIB=NULL;
    int      count;
    AEERect  prc;
    uint16   wClr;
    uint8 red, green, blue;
 
    // 获取显示器位图
    IDISPLAY_GetDeviceBitmap(pdis, &pBgBitmap);
    if(pBgBitmap == NULL)
        return;

    //popMenuBg = IDISPLAY_GetDestination(pdis);
    
    IMENUCTL_GetRect(popMenu, &prc);

    ERR("x=%d,y=%d,dx=%d",prc.x, prc.y, prc.dx);

    //IBITMAP_BltIn(popMenuBg, prc->x, prc->y, prc->dx, prc->dy, pBgBitmap, prc->x, prc->y, AEE_RO_COPY);
    
    // 获取显示缓冲区
    IBITMAP_QueryInterface(pBgBitmap, AEECLSID_DIB, (void**)&pBgDIB);
    if(pBgDIB == NULL)
        return;
 
    
    count = pBgDIB->cx * pBgDIB->cy;
    
    if(pBgDIB->nDepth == 16) 
    { 
        // 默认当16位处理
        int16* bgpixel = (int16*)pBgDIB->pBmp;
     
        // 按alpha=0.5暗化
        while(count--) 
        {
            wClr = *bgpixel;
            // 565格式, IDIB_COLORSCHEME_565
            red = ((wClr & 0x1f) >> 1);
            wClr >>= 5;
            green = ((wClr & 0x3f) >> 1);
            wClr >>= 6;
            blue = (wClr >> 1);
 #define _RGB16BIT(r, g, b) ( (b % 32) + ( (g % 64) << 5)+( (r % 32) << 11))
           *bgpixel = _RGB16BIT(red, green, blue);
           ++bgpixel;
       }
    }
   
    IDISPLAY_BitBlt(pdis, prc.x, prc.y, prc.dx, prc.dy, pBgBitmap, prc.x, prc.y, AEE_RO_COPY);
     //IDISPLAY_BitBlt(pdis, 0, 0, po->cxScreen, po->ButtonLine, pBgBitmap, 0, 0, AEE_RO_COPY);
 
     // 释放资源
    IDIB_Release(pBgDIB);
    IBITMAP_Release(pBgBitmap);
    //IBITMAP_Release(popMenuBg);   
}*/

static void TVApp_SetPopMenuRect(CTVApp *pMe, IMenuCtl* popMenu, int menuItemSum)
{
    AEERect mRec;
    int16  x,y,dx,dy;

    if(!pMe || !popMenu)
    {
        return;
    }
    
    dx = POPMENU_WIDTH;
    
    // the number of popMenu Item is no more than 4
    if(menuItemSum < 4)
    {
        //dy = menuItemSum*(pMe->m_nItemH+7);
        dy = menuItemSum*(pMe->m_nItemH+2);
    }
    else
    {
        //dy = 4*(pMe->m_nItemH+7);
        dy = 4*(pMe->m_nItemH+2);
    }

    x = CFGBAR_TEXT_X;
       
    //y = pMe->m_cyHeight - dy - POPMENU_BOTTOM_Y;
    y = 2*CFGBAR_TEXT_Y;
 
    SETAEERECT(&mRec, x, y, dx, dy);

    IMENUCTL_SetRect(popMenu, &mRec);
}

static void TVApp_DrawTopBar(CTVApp *pMe)
{
    int i;
    int16  nResID[TVCFGMAX];
    IImage *pTopBarImage = NULL; 
          
    // enviroment cfgID
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_ENVIROMENT,
                          &pMe->m_nTVEnviroment,
                          sizeof(pMe->m_nTVEnviroment));
    
    switch(pMe->m_nTVEnviroment){
    case OEMNV_CAMERA_ENVIR_AUTO:
        nResID[TVCFGENVIRMENT] = IDI_ENV_AUTO;
        break;
        
    case OEMNV_CAMERA_ENVIR_SUNNY:
        nResID[TVCFGENVIRMENT] = IDI_ENV_SUNNY;
        break;

    case OEMNV_CAMERA_ENVIR_CLOUDY:
        nResID[TVCFGENVIRMENT] = IDI_ENV_CLOUDY;
        break;

    case OEMNV_CAMERA_ENVIR_NIGHT:
        nResID[TVCFGENVIRMENT] = IDI_ENV_NIGHT;
        break;
        
    default:
        nResID[TVCFGENVIRMENT] = IDI_ENV_AUTO;
        break;
    }

    // quality cfgID
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_QUALITY,
                          &pMe->m_nTVQuality,
                          sizeof(pMe->m_nTVQuality));
    
    switch(pMe->m_nTVQuality){
    case OEMNV_CAMERA_QUALITY_HIGH:
        nResID[TVCFGQUALITY] = IDI_QUALITY_HIGH;
        break;

    case OEMNV_CAMERA_QUALITY_MED:
        nResID[TVCFGQUALITY] = IDI_QUALITY_MED;
        break;

    case OEMNV_CAMERA_QUALITY_LOW:
        nResID[TVCFGQUALITY] = IDI_QUALITY_LOW;
        break;
        
   default:
        nResID[TVCFGQUALITY] = IDI_QUALITY_HIGH;
        break;
   }
    
    // size cfgID
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_SIZE,
                         &pMe->m_nTVSize,
                          sizeof(pMe->m_nTVSize));

    switch(pMe->m_nTVSize){
    case OEMNV_CAMERA_SIZE_INDEX_0:
        nResID[TVCFGSIZE] = IDI_SIZE_160_128;
        break;

    case OEMNV_CAMERA_SIZE_INDEX_1:
        nResID[TVCFGSIZE] = IDI_SIZE_220_176;
        break;

    case OEMNV_CAMERA_SIZE_INDEX_2:
        nResID[TVCFGSIZE] = IDI_SIZE_320_240;
        break;

    case OEMNV_CAMERA_SIZE_INDEX_3:
        nResID[TVCFGSIZE] = IDI_SIZE_640_480;
        break;
        
    default:
        nResID[TVCFGSIZE] = IDI_SIZE_160_128;
        break;
    }

    // sound cfgID
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_TONE,
                          &pMe->m_nTVTone,
                          sizeof(pMe->m_nTVTone));
    
    switch(pMe->m_nTVTone){       
    case OEMNV_CAMERA_SHUTTER_TONE_ENABLE:
        nResID[TVCFGTONE] = IDI_TONE_ENABLE;
        break;

    case OEMNV_CAMERA_SHUTTER_TONE_DISABLE:
        nResID[TVCFGTONE] = IDI_TONE_DISABLE;
        break;		
        
    default:
        nResID[TVCFGTONE] = IDI_TONE_ENABLE;
        break;
    }
    
    // draw selftime cfg    
    switch(pMe->m_nSelfTimeItemSel){
    case IDS_SELFTIME_OFF:
        nResID[TVCFGSELFTIME] = IDI_SELFTIME_OFF;
        break;

    case IDS_SELFTIME_SEC1:
        nResID[TVCFGSELFTIME] = IDI_SELFTIME_5;
        break;

    case IDS_SELFTIME_SEC2:
        nResID[TVCFGSELFTIME] = IDI_SELFTIME_10;
        break;

    case IDS_SELFTIME_SEC3:
        nResID[TVCFGSELFTIME] = IDI_SELFTIME_15;
        break;
        
   default:
        nResID[TVCFGSELFTIME] = IDI_SELFTIME_OFF;
        break;
    }
    
    // banding cfgID
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_BANDING,
                          &pMe->m_nTVBanding,
                          sizeof(pMe->m_nTVBanding));
    
    switch(pMe->m_nTVBanding){
    case OEMNV_CAMERA_BANDING_50HZ:
        nResID[TVCFGBANDING] = IDI_BANDING_50;
        break;

    case OEMNV_CAMERA_BANDING_60HZ:
        nResID[TVCFGBANDING] = IDI_BANDING_60;
        break;
        
    default:
        nResID[TVCFGBANDING] = IDI_BANDING_50;
        break;
    }
    
    nResID[TVCFGRESET] = IDI_RESET;
    
    for(i = 0; i < TVCFGMAX; i++)
    {
        pTopBarImage = ISHELL_LoadResImage(pMe->m_pShell, 
                                           TVAPP_IMAGE_RES_FILE, 
                                           nResID[i]);
        if(pTopBarImage)
        {
            IIMAGE_Draw(pTopBarImage, i*(TOPBAR_ICON_WIDTH+3), TOPBAR_ICON_Y);	// + 5
            IIMAGE_Release(pTopBarImage);
            pTopBarImage = NULL;
        }
    }

	//Add By zzg 2010_07_25
    {
	    IImage *pTVCFGChooseIcon = ISHELL_LoadResImage(pMe->m_pShell, TVAPP_IMAGE_RES_FILE, IDI_CHOOSE_REC);
	    
	    if (pTVCFGChooseIcon)
	    {  
			IIMAGE_SetDrawSize(pTVCFGChooseIcon, TOPBAR_ICON_WIDTH, CFGBAR_TEXT_HEIGHT);
			IIMAGE_Draw(pTVCFGChooseIcon, (3+TOPBAR_ICON_WIDTH)*(pMe->m_nTVCFG), TOPBAR_ICON_Y);	//Add By zzg 2010_07_25
			
	        IIMAGE_Release(pTVCFGChooseIcon);
	        pTVCFGChooseIcon = NULL;
	    }
	}
	//Add End
}

static void TVApp_DrawMidPic(CTVApp *pMe)
{
    IImage *pImage = NULL;
    AEEImageInfo myInfo;
    
    switch(pMe->m_pActiveDlgID)
    {
        case IDD_CPREVIEW:
            pImage = ISHELL_LoadResImage(pMe->m_pShell,
                                         TVAPP_IMAGE_RES_FILE,
                                         IDI_MID_CAMERA);
            if(pImage != NULL)
            {
                IIMAGE_GetInfo(pImage, &myInfo);
                IIMAGE_Draw(pImage, (pMe->m_cxWidth - myInfo.cx)/2, pMe->m_cyHeight - myInfo.cy -1 );
                IIMAGE_Release(pImage);
                pImage = NULL;
            }
            break;
            
        default:
            break;
    }
}

static void TVApp_DrawCFGPromptText(CTVApp *pMe)
{  
    AEERect prc;
    int16   nResID = 0;       
    AECHAR  pcText[20]; 
    
    if(pMe->m_pActiveDlgID == IDD_CCAMERACFG)
    {
        switch(pMe->m_nTVCFG)
        {
            case TVCFGENVIRMENT:
                nResID = IDS_CFG_ENVIR;
                break;
                
            case TVCFGQUALITY:
                nResID = IDS_CFG_QUALITY;
                break;
                
            case TVCFGSIZE:
                nResID = IDS_CFG_SIZE;
                break;
                
            case TVCFGTONE:
                nResID = IDS_CFG_TONE;
                break;
                
            case TVCFGSELFTIME:
                nResID = IDS_CFG_SELFTIMER;
                break;
                
            case TVCFGBANDING:
                nResID = IDS_CFG_BANDING;
                break;

            case TVCFGRESET:
                nResID = IDS_CFG_RESET;
                break;
                
            default:
                break; 
        }
    }
    
	SETAEERECT(&prc, CFGBAR_TEXT_X, CFGBAR_TEXT_Y, CFGBAR_TEXT_WIDTH, CFGBAR_TEXT_HEIGHT);	//Add By zzg 2010_07_24

    MEMSET(pcText, 0, sizeof(pcText)); 
    (void)ISHELL_LoadResString(pMe->m_pShell, 
                               AEE_APPSTVAPP_RES_FILE,       
                               nResID, 
                               pcText, 
                               sizeof(pcText));     

    if(pMe->m_nTVSize != OEMNV_CAMERA_SIZE_INDEX_0|| (pMe->m_pActiveDlgID == IDD_CVIDEOCFG))
    {
        (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);
        
        DrawTextWithProfile(pMe->m_pShell, 
                            pMe->m_pDisplay, 
                            WHITE_TEXT, 
                            AEE_FONT_NORMAL, 
                            pcText, 
                            -1, 
                            0, 
                            0, 
                            &prc, 
                            IDF_ALIGN_LEFT|IDF_ALIGN_MIDDLE|IDF_TEXT_TRANSPARENT);	//IDF_ALIGN_CENTER

        (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
    }
    else
    {   
        (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, WHITE_TEXT);
        
        DrawTextWithProfile(pMe->m_pShell, 
                            pMe->m_pDisplay, 
                            RGB_BLACK, 
                            AEE_FONT_NORMAL, 
                            pcText, 
                            -1, 
                            0, 
                            0, 
                            &prc, 
                            IDF_ALIGN_LEFT|IDF_ALIGN_MIDDLE|IDF_TEXT_TRANSPARENT);	//IDF_ALIGN_CENTER

        (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
    }
}

static void TVApp_CPreviewStart(CTVApp *pMe)
{
    AEESize displaySize, captureSize; 
    int16 quality = 0;
    if(pMe->m_sensor_model == 30)
    {
        captureSize.cx = g_TVSizeCFG[pMe->m_nTVSize].dx;
        captureSize.cy = g_TVSizeCFG[pMe->m_nTVSize].dy;
        displaySize.cx = g_TVSizeCFG[0].dx;
        displaySize.cy = g_TVSizeCFG[0].dy;
    }
    else if(pMe->m_sensor_model == 10)
    {
        captureSize.cx = g_TVSizeCFG_10[pMe->m_nTVSize].dx;
        captureSize.cy = g_TVSizeCFG_10[pMe->m_nTVSize].dy;
        displaySize.cx = g_TVSizeCFG_10[0].dx;
        displaySize.cy = g_TVSizeCFG_10[0].dy;    
    }
    
    // set camera quality
    switch(pMe->m_nTVQuality)
    {
        case OEMNV_CAMERA_QUALITY_HIGH:
            quality = CAMERA_APP_QUALITY_HIGH;
            break;

        case OEMNV_CAMERA_QUALITY_MED:
            quality = CAMERA_APP_QUALITY_MED;
            break;

        case OEMNV_CAMERA_QUALITY_LOW:
            quality = CAMERA_APP_QUALITY_LOW;
            break; 

        default:
            break;
    }
    
    ICAMERAEX_SetParm(pMe->m_pTV, CAM_PARM_PREVIEW_TYPE, CAM_PREVIEW_SNAPSHOT, 0);       
    ICAMERAEX_SetParm(pMe->m_pTV, CAM_PARM_MULTISHOT, 1, 0);
    ICAMERAEX_SetQuality(pMe->m_pTV, quality);    
    ICAMERAEX_SetSize(pMe->m_pTV, &captureSize);
    ICAMERAEX_SetDisplaySize(pMe->m_pTV, &displaySize);
    
    ICAMERAEX_Preview(pMe->m_pTV); 

    pMe->m_nTVState = TV_PREVIEW;  

    TVApp_SetParamAfterPreview(pMe);

    pMe->m_bIsPreview = TRUE;
}

static void TVApp_RecordSnapShot(CTVApp *pMe)
{ 
    char sFileName[MIN_PIC_CHAR_NAME_LEN];
    
    MEMSET(pMe->m_sCurrentFileName, '\0', sizeof(pMe->m_sCurrentFileName));
    MEMSET(sFileName, '\0', sizeof(sFileName));
    
    TVApp_GetDateForRecordFileName(pMe, pMe->m_sCurrentFileName, FILE_TYPE_JPG);
    TVApp_SetDateForRecordFileName(pMe);
   
    // copy the pic name to sFilename buffer
    if(pMe->m_nTVStorage == OEMNV_CAMERA_STORAGE_MEMORY_CARD)
    {
        STRCPY(pMe->m_sCaptureFileName, pMe->m_sCurrentFileName+STRLEN(MG_MASSCARDPICTURE_PATH));
    }
    else
    {
        STRCPY(pMe->m_sCaptureFileName, pMe->m_sCurrentFileName+STRLEN(MG_PHONEPICTURE_PATH));
    }
    
    pMe->m_nTVState = TV_SAVE;
    pMe->m_bCapturePic  = TRUE;
    (void)ICAMERAEX_DeferEncode(pMe->m_pTV, TRUE);
    
    // 拍照状态的处理
    if(SUCCESS != ICAMERAEX_RecordSnapshot(pMe->m_pTV))
    {
        // 拍照失败,默认保留已经拍照成功的相片,并返回到预览界面,避免UI层出现死机现象
        // Vc848.c中处理过,如果拍照失败,直接删除失败的文件.
        pMe->m_wMsgID = IDS_MSG_CAPTURE_FAILED;
        pMe->m_nMsgTimeout = TIMEOUT_MS_MSGBOX;
        ICAMERAEX_Stop(pMe->m_pTV);
        pMe->m_nTVState = TV_STOP;
        pMe->m_bRePreview = TRUE;
        CLOSE_DIALOG(DLGRET_POPMSG);
        return;
    }

    pMe->m_bCapturePic = FALSE;
}

static void TVApp_DialogTimeout(void *pme)
{
    CTVApp *pMe = (CTVApp *)pme;
        
    if(NULL == pMe)
        return;

    (void)ISHELL_PostEvent(pMe->m_pShell,
                           AEECLSID_TVAPP,
                           EVT_APP_DIALOG_TIMEOUT,
                           0,
                           0);
}

static boolean TVApp_FindMemoryCardExist(CTVApp *pMe)
{   
    return (IFILEMGR_Test(pMe->m_pFileMgr, AEEFS_CARD0_DIR)==SUCCESS)?TRUE:FALSE;	
}

// 检测拍照，摄像是否有足够剩余空间容量
static boolean TVApp_IsEnoughfMemorySpace(CTVApp * pMe)
{
    if(pMe->m_nTVStorage == OEMNV_CAMERA_STORAGE_MEMORY_CARD)
    {
        IFILEMGR_GetFreeSpaceEx(pMe->m_pFileMgr, 
                                MG_MASSCARD_ROOTDIR, 
                                &pMe->m_dwMemTotal, 
                                &pMe->m_dwMemFree);

        if((pMe->m_dwMemFree/(2*BYTE_SIZE) < MIN_FREE_MEMORY_CARD_SPACE) 
           ||(pMe->m_dwMemFree/(2*BYTE_SIZE) == MIN_FREE_MEMORY_CARD_SPACE))
        {               
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    }
    else
    {
        IFILEMGR_GetFreeSpaceEx(pMe->m_pFileMgr, 
                                MG_PHONE_ROOTDIR, 
                                &pMe->m_dwMemTotal, 
                                &pMe->m_dwMemFree);

        if((pMe->m_dwMemFree/(2*BYTE_SIZE) < MIN_FREE_PHONE_SPACE) 
           ||(pMe->m_dwMemFree/(2*BYTE_SIZE) == MIN_FREE_PHONE_SPACE))
        {               
            return FALSE;
        }
        else
        {
            return TRUE;
        }

    }
}


// 自拍
static boolean TVApp_SelfTimeRecordSnapShot(CTVApp *pMe)
{
    // 倒计时先清空屏幕
    //IDisplay_ClearScreen(pMe->m_pDisplay);

    (void)ISHELL_SendEvent(pMe->m_pShell,
                           AEECLSID_TVAPP,
                           EVT_USER_REDRAW,
                           0,
                           0);
        
    if(pMe->m_nLeftTime  == 0)
    {
        ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY)TVApp_SelfTimeRecordSnapShot, pMe);

        if(pMe->m_pTV)
        {
            pMe->m_nTVState = TV_CAPTURE;
            ICAMERAEX_Stop(pMe->m_pTV);
            //TVApp_RecordSnapShot(pMe);
        }
        
        return TRUE;
    }
    else
    {  
        pMe->m_nLeftTime--;
        
        (void)ISHELL_SetTimer(pMe->m_pShell,
                              1000,
                              (PFNNOTIFY)TVApp_SelfTimeRecordSnapShot,
                              pMe);
    }

    return TRUE;
}

// Draw Bottom Bar Text
static void TVApp_DrawBottomBarText(CTVApp *pMe, BottomBar_e_Type eBarType)
{
    uint16      nResID_L = 0;   // left
    uint16      nResID_R = 0;   // right
    uint16      nResID_M = 0;   // mid
    AEERect     rc = pMe->m_rc;
    AECHAR      wszBar_L[20]={0};// left
    AECHAR      wszBar_R[20]={0};// right
    AECHAR      wszBar_M[20]={0};// mid
    
    switch(eBarType)
    {
        case BTBAR_SELECT_BACK:
            nResID_L = IDS_OK; //IDS_SELECT;
            nResID_R = IDS_BACK;
            break;

        case BTBAR_OPTION_BACK:
            nResID_L = IDS_OPTION;
            nResID_R = IDS_BACK;
            break;

        case BTBAR_SAVE_DELETE:
            nResID_L = IDS_SAVE;
            nResID_R = IDS_DEL;
            break;

        default:
            return;
    }

    // 加载按钮文本
    if(nResID_L != 0)
    {
        (void)ISHELL_LoadResString(pMe->m_pShell,
                                   AEE_APPSCOMMONRES_LANGFILE,
                                   nResID_L,
                                   wszBar_L,
                                   sizeof(wszBar_L));
    }
    
    if(nResID_R != 0)
    {
        (void)ISHELL_LoadResString(pMe->m_pShell,
                                   AEE_APPSCOMMONRES_LANGFILE,
                                   nResID_R,
                                   wszBar_R,
                                   sizeof(wszBar_R));
    }
    
    if(nResID_M != 0)
    {
        (void)ISHELL_LoadResString(pMe->m_pShell,
                                   AEE_APPSCOMMONRES_LANGFILE,
                                   nResID_M,
                                   wszBar_M,
                                   sizeof(wszBar_M));
    }
            
    (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, WHITE_TEXT);

    // 绘制文本-左键
    if(WSTRLEN(wszBar_L)>0)
    {      
        DrawTextWithProfile(pMe->m_pShell, 
                            pMe->m_pDisplay, 
                            RGB_BLACK, 
                            AEE_FONT_NORMAL, 
                            wszBar_L, 
                            -1, 
                            0, 
                            0, 
                            &rc, 
                            IDF_ALIGN_BOTTOM|IDF_ALIGN_LEFT|IDF_TEXT_TRANSPARENT);
    }

    // 绘制文本-右键
    if(WSTRLEN(wszBar_R)>0)
    {
        DrawTextWithProfile(pMe->m_pShell, 
                            pMe->m_pDisplay, 
                            RGB_BLACK, 
                            AEE_FONT_NORMAL, 
                            wszBar_R, 
                            -1, 
                            0, 
                            0, 
                            &rc, 
                            IDF_ALIGN_BOTTOM|IDF_ALIGN_RIGHT|IDF_TEXT_TRANSPARENT);
    }

     // 绘制文本-中间键
    if(WSTRLEN(wszBar_M)>0)
    {
         DrawTextWithProfile(pMe->m_pShell, 
                            pMe->m_pDisplay, 
                            RGB_BLACK, 
                            AEE_FONT_NORMAL, 
                            wszBar_M, 
                            -1, 
                            0, 
                            0, 
                            &rc, 
                            IDF_ALIGN_BOTTOM|IDF_ALIGN_CENTER|IDF_TEXT_TRANSPARENT);
    }

    (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
}

static boolean TVApp_SetDateForRecordFileName(CTVApp *pMe)
{
    AEEMediaData   md;
    
    //Fill media data
    md.clsData = MMD_FILE_NAME;
    md.pData = (void *)pMe->m_sCurrentFileName;
    md.dwSize = 0;

    (void)ICAMERAEX_SetMediaData(pMe->m_pTV, &md, 0);

    return TRUE;
}

static boolean TVApp_GetDateForRecordFileName(CTVApp *pMe, char * pszDest, unsigned int fileType)
{
    JulianType julian;
 
    GETJULIANDATE(GETTIMESECONDS(), &julian);
   
    if(pMe->m_nTVStorage == OEMNV_CAMERA_STORAGE_MEMORY_CARD)
    {
        STRCPY(pszDest, AEEFS_CARD0_DIR);
    }
    else
    {
        STRCPY(pszDest, MG_PHONE_ROOTDIR);
    }
    
    switch(fileType){
    case FILE_TYPE_JPG:
    default:
        STRCAT(pszDest, FS_CARD_PICTURES_FOLDER);
#ifdef FEATURE_JPEG_ENCODER
        SPRINTF(pszDest+STRLEN(pszDest), "%02d%02d%02d%02d.jpg", julian.wDay, julian.wHour, julian.wMinute, julian.wSecond);
#else
        SPRINTF(pszDest+STRLEN(pszDest), "%02d%02d%02d%02d.png", julian.wDay, julian.wHour, julian.wMinute, julian.wSecond);
#endif
        break;
   }
   return TRUE;
}

static void TVApp_SetTVCaptureSize(CTVApp *pMe, uint16 wParam)
{
    AEESize displaySize, captureSize;
     
    pMe->m_nTVSize = wParam;
    if(pMe->m_sensor_model == 30)
    {
        captureSize.cx = g_TVSizeCFG[pMe->m_nTVSize].dx;
        captureSize.cy = g_TVSizeCFG[pMe->m_nTVSize].dy;
        
        displaySize.cx = g_TVSizeCFG[0].dx;
        displaySize.cy = g_TVSizeCFG[0].dy;
    }
    else if(pMe->m_sensor_model == 10)
    {
        captureSize.cx = g_TVSizeCFG_10[pMe->m_nTVSize].dx;
        captureSize.cy = g_TVSizeCFG_10[pMe->m_nTVSize].dy;
        
        displaySize.cx = g_TVSizeCFG_10[0].dx;
        displaySize.cy = g_TVSizeCFG_10[0].dy;
    }

    (void)ICONFIG_SetItem(pMe->m_pConfig,
                          CFGI_CAMERA_SIZE,
                          &pMe->m_nTVSize,
                          sizeof(pMe->m_nTVSize));
 
    ICAMERAEX_SetSize(pMe->m_pTV, &captureSize);
    
    ICAMERAEX_SetDisplaySize(pMe->m_pTV, &displaySize);
}

static void TVApp_HandleSnapshotPic(CTVApp *pMe)
{
    AECHAR wfileName[MIN_PIC_WCHAR_NAME_LEN];
    
    STRTOWSTR(pMe->m_sCaptureFileName, wfileName, MIN_PIC_WCHAR_NAME_LEN);
    
    // Load Image
    {
        IImage *pImage = ISHELL_LoadImage(pMe->m_pShell,pMe->m_sCurrentFileName);
        if(pImage)
        {
            AEEImageInfo myInfo;
            int x,y;
            IImage_GetInfo(pImage,&myInfo);
            
#ifdef FEATURE_BREW_SCALE
            if(myInfo.cy > 0 && pMe->m_rc.dy > 0)
            {
                if((myInfo.cx*1000)/myInfo.cy > (pMe->m_rc.dx*1000)/pMe->m_rc.dy)
                {
                    myInfo.cx = (myInfo.cx*pMe->m_rc.dy)/myInfo.cy;
                    myInfo.cy = pMe->m_rc.dy;
                }
                else
                {
                    myInfo.cy = (myInfo.cy*pMe->m_rc.dx)/myInfo.cx;
                    myInfo.cx = pMe->m_rc.dx;
                }
                
                IImage_SetParm(pImage,
                               IPARM_SCALE,
                               myInfo.cx,
                               myInfo.cy);
            }
#endif
            x = (myInfo.cx-pMe->m_rc.dx)/2;
            y = (myInfo.cy-pMe->m_rc.dy)/2;
            IImage_SetOffset(pImage,x,y);
            IImage_Draw(pImage,0,0);
            IImage_Release(pImage);
        }
    }
    
    TVApp_DrawBottomBarText(pMe, BTBAR_SAVE_DELETE);
    
    IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, WHITE_TEXT);

    DrawTextWithProfile(pMe->m_pShell, 
                        pMe->m_pDisplay, 
                        RGB_BLACK, 
                        AEE_FONT_NORMAL, 
                        wfileName, 
                        -1, 
                        0, 
                        0, 
                        &pMe->m_rc, 
                        IDF_ALIGN_CENTER|IDF_ALIGN_TOP|IDF_TEXT_TRANSPARENT);
    
    IDisplay_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE); 
    
    IDisplay_UpdateEx(pMe->m_pDisplay, FALSE); 
}

// TV一些设置参数,需要在Preview后设置才能生效
static void TVApp_SetParamAfterPreview(CTVApp *pMe)
{
    uint32 dwFPS = 0;

    //set camera enviroment
    switch(pMe->m_nTVEnviroment)
    {
        case OEMNV_CAMERA_ENVIR_AUTO:
            dwFPS = 0;
            break;

        case OEMNV_CAMERA_ENVIR_NIGHT:
            dwFPS = 3;
            break;

        case OEMNV_CAMERA_ENVIR_SUNNY:
            dwFPS = 1;
            break;
        
        case OEMNV_CAMERA_ENVIR_CLOUDY:
            dwFPS = 2;
            break;

        default:
            break;
    }

    ICAMERAEX_SetFramesPerSecond(pMe->m_pTV, dwFPS);
    ICAMERAEX_SetParm(pMe->m_pTV, CAM_PARM_BANDING, pMe->m_nTVBanding, 0);
}

/*===========================================================================
FUNCTION TVApp_UpdateFrame

  DESCRIPTION
    This function updates the display with the frame from the viewfinder.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static void TVApp_UpdateFrame(CTVApp *pMe)
{
  IBitmap *pFrame = NULL;

  if (!pMe)
    return;
  
  //(void)ICAMERAEX_GetFrame(pMe->m_pTV, &pFrame);

  if (!pFrame)
    return;
  
  // Display the frame at center location of the screen
  IDISPLAY_BitBlt(pMe->m_pDisplay, pMe->m_rc.x, pMe->m_rc.y, pMe->m_rc.dx, pMe->m_rc.dy, pFrame, 0, 0, AEE_RO_COPY);
  RELEASEIF(pFrame);
}

/*===========================================================================
FUNCTION TVApp_SavePhoto

  DESCRIPTION
    This function starts the encoding processing to save a new photo to the file.

  PARAMETERS:
    pMe - pointer to QCam data struct

  RETURN VALUE:
    NONE
===========================================================================*/
static int TVApp_SavePhoto(CTVApp *pMe)
{
  AEEMediaData   md;
  
  if (!pMe)
    return EBADPARM;

  //Fill media data
  md.clsData = MMD_FILE_NAME;
  md.pData = (void *)pMe->m_sCurrentFileName;
  md.dwSize = 0;

  //Start encoding processing
  (void)ICAMERAEX_SetMediaData(pMe->m_pTV, &md, 0);
  
  return ICAMERAEX_EncodeSnapshot(pMe->m_pTV);
} /* END TVApp_SavePhoto */

static void TVApp_EventNotify(CTVApp *pMe, AEECameraNotify *pcn)
{
    if (!pMe || !pcn || !pMe->m_pTV)
        return;
    
    switch (pcn->nCmd){
    case CAM_CMD_START:
        switch (pcn->nStatus){
        case CAM_STATUS_FRAME:
            //It is the image from the viewfinder.
            TVApp_UpdateFrame(pMe);
            ISHELL_PostEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL);
            break;
            
        default:
            ISHELL_PostEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_CAMERA_NOTIFY, pcn->nCmd, pcn->nStatus);
            break;
        }
        break;
    
    case CAM_CMD_SETPARM:
        switch (pcn->nStatus)
        {
        case CAM_STATUS_UPDATE:
        case CAM_STATUS_FAIL:
        case CAM_STATUS_DONE:
            break;
        default:
            break;
        }
        break;
    
    case CAM_CMD_ENCODESNAPSHOT:
        ISHELL_PostEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_CAMERA_NOTIFY, pcn->nCmd, pcn->nStatus);
        break;
    
    default:
        break;
    }
}

void TVApp_InitTVCheck(void *po)
{
    CTVApp *pMe = (CTVApp *)po; 
    TVApp_CreateDirectory(pMe);
 
    if(pMe->m_pTV == NULL)
    {
        ISHELL_CreateInstance(pMe->m_pShell, 
                              AEECLSID_CAMERA, 
                              (void **)&pMe->m_pTV);
    }
    if(pMe->m_pTV != NULL)
    {
        MSG_FATAL("pMe->m_pTV != NULL",0,0,0);
        ICAMERAEX_GetParm(pMe->m_pTV, CAM_PARM_SENSOR_MODEL, &(pMe->m_sensor_model), NULL);
        MSG_FATAL("m_sensor_model=%d",pMe->m_sensor_model,0,0);            
    }
    else
    {
        MSG_FATAL("pMe->m_pTV == NULL",0,0,0);
    }
    if(pMe->m_pTV)
    {
        ICAMERAEX_RegisterNotify(pMe->m_pTV,(PFNCAMERANOTIFY)TVApp_EventNotify,po);
    }
}

void TVApp_AppEventNotify(CTVApp *pMe, int16 nCmd, int16 nStatus)
{
    if (!pMe || !pMe->m_pTV)
        return;
    
    switch (nCmd){
    case CAM_CMD_START:
        switch (nStatus){
        case CAM_STATUS_START:
            break;
            
        case CAM_STATUS_DONE:
            if(pMe->m_nTVState == TV_CAPTURE)
            {
                TVApp_RecordSnapShot(pMe);
            }
            else if(pMe->m_nTVState == TV_SAVE)
            {
                TVApp_SavePhoto(pMe);
                //CLOSE_DIALOG(DLGRET_PICMENU);
            }
            break;

        case CAM_STATUS_ABORT:
        case CAM_STATUS_FAIL:
            if(pMe->m_nTVState == TV_CAPTURE)
            {
                pMe->m_bRePreview = TRUE;
                pMe->m_wMsgID = IDS_MSG_CAPTURE_FAILED;
                pMe->m_nMsgTimeout = TIMEOUT_MS_MSGDONE;
                CLOSE_DIALOG(DLGRET_POPMSG);
            }
            break;
            
        default:
            break;
        }
        break;
    
    case CAM_CMD_SETPARM:
        switch (nStatus){
        case CAM_STATUS_UPDATE:
        case CAM_STATUS_FAIL:
        case CAM_STATUS_DONE:
            break;
        default:
            break;
        }
        break;
    case CAM_CMD_ENCODESNAPSHOT:
        switch (nStatus){
        case CAM_STATUS_ABORT:
        case CAM_STATUS_FAIL:
            if(pMe->m_nTVState == TV_SAVE)
            {
                pMe->m_bRePreview = TRUE;
                pMe->m_wMsgID = IDS_MSG_CAPTURE_FAILED;
                pMe->m_nMsgTimeout = TIMEOUT_MS_MSGDONE;
                CLOSE_DIALOG(DLGRET_POPMSG);
            }
            break;
            
        case CAM_STATUS_DONE:
            (void)ICONFIG_GetItem(pMe->m_pConfig,
                      CFGI_CAMERA_TONE,
                      &pMe->m_nTVTone,
                      sizeof(pMe->m_nTVTone));

#ifdef FEATURE_CARRIER_CHINA_TELCOM
            TVApp_PlayShutterSound(pMe);
#else
            if(pMe->m_nTVTone == OEMNV_CAMERA_SHUTTER_TONE_ENABLE)
            {
                TVApp_PlayShutterSound(pMe);
            }
#endif
            CLOSE_DIALOG(DLGRET_PICMENU);
            break;
            
        default:
            break;
        }
        
        break;
        
   default:
    break;
  }
}

// 创建文件夹目录
static void TVApp_CreateDirectory(CTVApp *pMe)
{
    // 手机上的相片保存目录
    if(SUCCESS != IFILEMGR_Test(pMe->m_pFileMgr, MG_PHONEPICTURE_PATH))
    {
        IFILEMGR_MkDir(pMe->m_pFileMgr, MG_PHONEPICTURE_PATH);
    }
	
    pMe->m_bMemoryCardExist = TVApp_FindMemoryCardExist(pMe);	

    if(pMe->m_bMemoryCardExist)
    {
         // T卡上的相片保存目录
         if(SUCCESS != IFILEMGR_Test(pMe->m_pFileMgr, MG_MASSCARDPICTURE_PATH))
         {
             IFILEMGR_MkDir(pMe->m_pFileMgr, MG_MASSCARDPICTURE_PATH);
         }
    
         // T卡上的录像保存目录
         if(SUCCESS != IFILEMGR_Test(pMe->m_pFileMgr, MG_MASSCARDVIDEO_PATH))
         {
             IFILEMGR_MkDir(pMe->m_pFileMgr, MG_MASSCARDVIDEO_PATH);
         }   
    }           
}



static int TV_UpdateInit(CTVApp *pMe)
{
   // if(pMe->m_nCameraState == CAM_PREVIEW)
   // {
        IDISPLAY_FillRect(pMe->m_pDisplay, &pMe->m_rc, TRANS_COLOR);
   MSG_FATAL("TV_UpdateInit1-------x%d---------y%d",pMe->m_rc.dx,pMe->m_rc.dy,0);
   MSG_FATAL("TV_UpdateInit2-------x%d---------y%d",pMe->m_rc.x,pMe->m_rc.y,0);
     //   return SUCCESS;
   // }
   // else
   /// {
   //     return EBADSTATE;
   // }
        
}
static int TV_Update(CTVApp *pMe)
{
    //DBGPRINTF("camera state--------------%d\n", pMe->m_nCameraState);
   // if(pMe->m_nCameraState == CAM_PREVIEW)
   // {
  
        IBitmap* pbmp = NULL;
        IDIB* pdib = NULL;
        IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE); 
        IDISPLAY_GetDeviceBitmap(pMe->m_pDisplay, &pbmp);           
        IBITMAP_QueryInterface(pbmp, AEECLSID_DIB, (void**)&pdib);
		MSG_FATAL("IMMITv_updateimg--------------------------start",0,0,0);
        IMMITv_updateimg(pMe->pIMMITv, (uint32)pdib->pBmp);
		MSG_FATAL("IMMITv_updateimg--------------------------end",0,0,0);
        IBITMAP_Release(pbmp);  
        IDIB_Release(pdib);
   // }    
   // else
   // {
   //     return EBADSTATE;
   // }
          
}



