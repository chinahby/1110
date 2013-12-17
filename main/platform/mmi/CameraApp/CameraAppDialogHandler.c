/*==============================================================================
//           CameraApp   A P P L E T  M O D U L E
// 文件:
//        CameraAppDialogHandler.c
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
#include "CameraApp_priv.h" 
#include "MediaGallery.h"
#include "appscommonimages.brh"
#ifdef FEATURE_DSP
#include "cam_IF_ait_api.h"
#endif

#ifdef FEATURE_APP_MUSICPLAYER
#include "MusicPlayer.h"
#endif 

/*==============================================================================                                 
                                 宏定义和常数                                
==============================================================================*/

/*==============================================================================
                                 函数声明
==============================================================================*/
// 对话框IDD_CMAINMENU事件处理函数
static boolean  CameraApp_MainMenuHandleEvent(CCameraApp *pMe, 
                                              AEEEvent eCode, 
                                              uint16 wParam, 
                                              uint32 dwParam);
// 对话框IDD_CAMERA_PHOTO_MODE事件处理函数
static boolean  CameraApp_CameraPhotoModeHandleEvent(CCameraApp *pMe, 
                                              AEEEvent eCode, 
                                              uint16 wParam, 
                                              uint32 dwParam);

// 对话框IDD_CPREVIEW事件处理函数
static boolean  CameraApp_PreviewHandleEvent(CCameraApp *pMe, 
                                             AEEEvent eCode, 
                                             uint16 wParam, 
                                             uint32 dwParam);

// 对话框IDD_CCAMERACFG事件处理函数
static boolean CameraApp_CameraCFGHandleEvent(CCameraApp *pMe, 
                                              AEEEvent eCode, 
                                              uint16 wParam, 
                                              uint32 dwParam);

// 对话框IDD_CPIC事件处理函数
static boolean CameraApp_PicHandleEvent(CCameraApp *pMe, 
                                        AEEEvent eCode, 
                                        uint16 wParam, 
                                        uint32 dwParam);

// 对话框IDD_CVIDEO事件处理函数
static boolean CameraApp_VideoHandleEvent(CCameraApp *pMe, 
                                        AEEEvent eCode, 
                                        uint16 wParam, 
                                        uint32 dwParam);


// 对话框IDD_POPMSG事件处理函数
static boolean  CameraApp_PopMSGHandleEvent(CCameraApp *pMe,
                                            AEEEvent   eCode,
                                            uint16     wParam,
                                            uint32     dwParam);

// PopMenu事件路由处理函数
static boolean CameraApp_RoutePopMenuCommandEvent(CCameraApp *pMe, 
                                                  uint16 wParam);
#ifdef FEATURE_CAMERA_MULTI_SENSOR  //add by adrew
static boolean CameraApp_PopMenu_MultiCommandHandleEvent(CCameraApp *pMe,  uint16 wParam);
static boolean CameraApp_PopMenu_FlashCommandHandleEvent(CCameraApp *pMe,  uint16 wParam);                                                 
#endif
// PopMenu环境模式菜单处理函数                           
static boolean CameraApp_PopMenu_EnvironmentCommandHandleEvent(CCameraApp *pMe, 
                                                               uint16 wParam);
// PopMenu拍照质量设置菜单处理函数                                                                                                                 
static boolean CameraApp_PopMenu_QualityCommandHandleEvent(CCameraApp *pMe, 
                                                           uint16 wParam);

// PopMenu拍照尺寸设置菜单处理函数                                                                                                                      
static boolean CameraApp_PopMenu_SizeCommandHandleEvent(CCameraApp *pMe, 
                                                        uint16 wParam);
                                                                                                                    
// PopMenu快门声音设置菜单处理函数
static boolean CameraApp_PopMenu_ShutterToneCommandHandleEvent(CCameraApp *pMe, 
                                                               uint16 wParam);
                                                                                                                     
// PopMenu自拍设置菜单处理函数
static boolean CameraApp_PopMenu_SelfTimeCommandHandleEvent(CCameraApp *pMe, 
                                                            uint16 wParam);
                                                                                              
// PopMenu日光灯banding处理函数
static boolean CameraApp_PopMenu_BandingCommandHandleEvent(CCameraApp *pMe, 
                                                           uint16 wParam);

// PopMenu camera/video设置复位处理函数
static boolean CameraApp_PopMenu_ResetCFGCommandHandleEvent(CCameraApp *pMe, 
                                                            uint16 wParam);

// 设置菜单项选中图标
static void CameraApp_SetCFGMenuIcon(IMenuCtl *pm, 
                                     uint16 listId, 
                                     boolean bSel);

// 初始化设置菜单
static boolean CameraApp_InitpopMenu(CCameraApp *pMe, 
                                     IMenuCtl *popMenu);

// 初始化设置数据
static void CameraApp_InitCFGData(CCameraApp * pMe);
#ifdef FEATURE_CAMERA_MULTI_SENSOR
// 初始化拍照环境设置菜单
static void CameraApp_PopMenu_MultiInit(CCameraApp *pMe, 
                                              IMenuCtl *popMenu);
static void CameraApp_PopMenu_FlashInit(CCameraApp *pMe, 
                                              IMenuCtl *popMenu);
#endif
// 初始化拍照环境设置菜单
static void CameraApp_PopMenu_EnvironmentInit(CCameraApp *pMe, 
                                              IMenuCtl *popMenu);

// 初始化拍照质量设置菜单
static void CameraApp_PopMenu_QualityInit(CCameraApp *pMe, 
                                          IMenuCtl *popMenu);

// 初始化拍照尺寸设置菜单
static void CameraApp_PopMenu_SizeInit(CCameraApp *pMe, 
                                       IMenuCtl *popMenu);

// 初始化快门声设置菜单
static void CameraApp_PopMenu_ShutterToneInit(CCameraApp *pMe, 
                                              IMenuCtl *popMenu);

// 初始化自拍时间设置菜单
static void CameraApp_PopMenu_SelfTimeInit(CCameraApp *pMe, 
                                           IMenuCtl *popMenu);

// 初始化Bandig设置菜单
static void CameraApp_PopMenu_BandingInit(CCameraApp *pMe, 
                                          IMenuCtl *popMenu);

// 初始化恢复默认设置菜单
static void CameraApp_PopMenu_ResetCFGInit(CCameraApp *pMe, 
                                           IMenuCtl *popMenu);

// 拍照快门声处理函数
static void CameraApp_PlayShutterSound(CCameraApp *pMe);

// 快门声处理回调函数
static void CameraApp_MediaNotify(void *pUser, 
                                  AEEMediaCmdNotify *pCmdNotify);

//void CameraApp_AlphaDisplay(CCameraApp *pMe, IMenuCtl *pMenu);

// 设置弹出菜单矩形
static void CameraApp_SetPopMenuRect(CCameraApp *pMe, 
                                     IMenuCtl *popMenu, 
                                     int menuItemSum);

// 画TopBar的函数
static void CameraApp_DrawTopBar(CCameraApp *pMe);

//static void CameraApp_DrawpopMenuBg(CCameraApp *pMe, IMenuCtl *popMenu);

// 画Bottombar上的Camera/video图标函数
static void CameraApp_DrawMidPic(CCameraApp *pMe);

// 画CFGBar的提示文本
static void CameraApp_DrawCFGPromptText(CCameraApp *pMe);

// Camera Preview的启动函数
static void CameraApp_CPreviewStart(CCameraApp *pMe);

// Camera Snapshot的处理函数
static void CameraApp_RecordSnapShot(CCameraApp *pMe);


static void CameraApp_RecordVideo(CCameraApp *pMe);

static void CameraApp_OnRecordTimeFeedBack(void *pUser);

// 找T卡的函数
static boolean CameraApp_FindMemoryCardExist(CCameraApp *pMe);

// 找Sensor的函数
//static boolean CameraApp_FindSensorExist(CCameraApp *pMe);

// Dialog的定时处理函数
static void CameraApp_DialogTimeout(void *pme);


// Dialog的定时处理函数
static void CameraApp_PrevewTimeout(void *pme);


// PICDialog的定时处理函数
static void CameraApp_PicDialogTimeout(void *pme);

// 存储空间的检测函数
static boolean CameraApp_IsEnoughfMemorySpace(CCameraApp *pMe);

// 画bottom bar上的文本函数
static void CameraApp_DrawBottomBarText(CCameraApp *pMe, 
                                        BottomBar_e_Type eBarType);

// 自拍处理函数
static boolean CameraApp_SelfTimeRecordSnapShot(CCameraApp *pMe);

// 设置拍照文件名
static boolean CameraApp_SetDateForRecordFileName(CCameraApp *pMe,unsigned int fileType);

// 获取当前时间
static boolean CameraApp_GetDateForRecordFileName(CCameraApp *pMe,
                                                  char *pszDest, 
                                                  unsigned int fileType);

// 重新设置拍照大小的处理函数
static void CameraApp_SetCameraCaptureSize(CCameraApp *pMe, 
                                           uint16 wParam);

// 拍照后，照片处理函数
static void CameraApp_HandleSnapshotPic(CCameraApp *pMe);

// 设置Camera Preview的参数
static void CameraApp_SetParamAfterPreview(CCameraApp * pMe);

void CameraApp_InitCameraCheck(void *po);

static void CameraApp_CreateDirectory(CCameraApp *pMe);
static void CameraApp_UpdateFrame(CCameraApp *pMe);
static int CameraApp_SavePhoto(CCameraApp *pMe);
#ifdef FEATURE_DSP
static int CameraApp_UpdateInit(CCameraApp *pMe);
static int CameraApp_Update(CCameraApp *pMe);
#endif
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
}CCameraSize;

#ifdef FEATURE_CAMERA_MULTI_SENSOR
static const CCameraSize g_CameraFrontSizeCFG[] = 
{
#if defined(FEATURE_VERSION_X3)||defined(FEATURE_VERSION_K212)
    {240,320,L"240*320"}, // VGA
    {324,432,L"480*640"}, // VGA
    {0,0,NULL}
#else
	{180,240,L"180*240"}, // VGA
    {360,480,L"360*480"}, // VGA
    {0,0,NULL}
#endif
};

static const CCameraSize g_CameraBackSizeCFG[] = 
{
#if defined(FEATURE_VERSION_X3)||defined(FEATURE_VERSION_K212)
    {240,320,L"240*320"}, // VGA
    {324,432,L"480*640"}, // VGA
    {0,0,NULL}
#else
	{480,640,L"480*640"}, // VGA 
    {600,800,L"600*800"}, // VGA
    {0,0,NULL}
#endif
};
#else
// 最大OEMNV_CAMERA_SIZE_MAX
static const CCameraSize g_CameraSizeCFG[] = 
{
#if defined(FEATURE_DISP_160X128)
    {160,128,L"160*128"}, // FULL Screen
    {220,176,L"220*176"}, // QCIF
    {320,240,L"320*240"}, // QVGA
    {640,480,L"640*480"}, // VGA  
#elif defined(FEATURE_DISP_220X176)
    {220,176,L"220*176"}, // QCIF
    {320,240,L"320*240"}, // QVGA
    {640,480,L"640*480"}, // VGA      
#elif defined(FEATURE_DISP_128X160)
    {128,160,L"128*160"}, // FULL Screen
    {176,220,L"176*220"}, // QCIF
    {240,320,L"240*320"}, // QVGA   
 #if defined( FEATURE_VERSION_C316)//||defined(FEATURE_VERSION_C117_M74)
    {300,400,L"480*640"}, // VGA
 #elif defined (FEATURE_VERSION_C260_IC18)
    {352,464,L"352*464"}, // VGA 
 #endif
#elif defined(FEATURE_DISP_176X220)
    {176,220,L"176*220"}, // QCIF
    {240,320,L"240*320"}, // QVGA
#if !defined(FEATURE_VERSION_VG68)&&!defined(FEATURE_VERSION_K202_LM129C)&&!defined(FEATURE_VERSION_W516_C260)
#if defined (FEATURE_VERSION_C337)
    {352,464,L"352*464"}, // VGA    
#else
    {480,640,L"480*640"}, // VGA    
#endif    
#endif
#elif defined(FEATURE_DISP_240X320)
#if defined(FEATURE_VERSION_K212) 
	{240,304,L"240*320"}, // VGA
	{324,432,L"480*640"}, // VGA
#elif defined(FEATURE_VERSION_IC241A_MMX)
    {128,160,L"128*160"}, // FULL Screen
    {176,220,L"176*220"}, // QCIF
    {240,320,L"240*320"}, // QVGA   
    {352,464,L"352*464"}, // VGA  
#else
    //{128,160,L"128*160"}, // FULL Screen
    //{176,220,L"176*220"}, // QCIF
    {480,640,L"480*640"}, // VGA 
    {600,800,L"600*800"}, // VGA
#endif
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
static const CCameraSize g_CameraSizeCFG_10[] = 
{
#if defined(FEATURE_DISP_160X128)
    {160,128,L"160*128"}, // FULL Screen
    {220,176,L"220*176"}, // QCIF
    #ifndef FEATURE_CAMERA_SP0828
    {320,240,L"320*240"}, // QVGA
    #endif
#elif defined(FEATURE_DISP_220X176)
    {220,176,L"220*176"}, // QCIF
    {320,240,L"320*240"}, // QVGA    
#elif defined(FEATURE_DISP_128X160)
    {128,160,L"128*160"}, // FULL Screen
    {176,220,L"176*220"}, // QCIF
#if defined(FEATURE_VERSION_C11) || defined(FEATURE_VERSION_C180)
//#ifdef FEATURE_VERSION_M74
	{176,220,L"240*320"}, // QVGA
//#endif
#else
    {240,320,L"240*320"}, // QVGA
#endif    
#if defined( FEATURE_VERSION_C316)//||defined(FEATURE_VERSION_C117_M74)
    {300,400,L"480*640"}, // VGA
#elif defined (FEATURE_VERSION_C260_IC18)
    {352,464,L"352*464"}, // VGA    
#endif

#elif defined(FEATURE_DISP_176X220)
//1110w516 siv121d
#if defined(FEATURE_VERSION_W208S)
    {176,220,L"176*220"}, // QCIF
    {320,240,L"320*240"}, // QVGA
	{360,480,L"640*480"},
#elif defined(FEATURE_VERSION_W516)
    {180,240,L"180*240"}, // VGA 
    {360,480,L"640*480"},
#else
    {176,220,L"176*220"}, // QCIF
    {240,320,L"240*320"}, // QVGA
#if !defined (FEATURE_VERSION_VG68)&&!defined(FEATURE_VERSION_K202_LM129C)&&!defined(FEATURE_VERSION_W516_C260)
#if defined (FEATURE_VERSION_C337)
    {352,464,L"352*464"}, // VGA    
#else
    {480,640,L"480*640"}, // VGA    
#endif     
#endif
#endif     
#elif defined(FEATURE_DISP_240X320)
#if defined(FEATURE_VERSION_K212)
	//{120,160,L"240*320"}, // VGA
	{180,240,L"480*640"}, // VGA
#elif defined(FEATURE_VERSION_IC241A_MMX)
    {128,160,L"128*160"}, // FULL Screen
    {176,220,L"176*220"}, // QCIF
    {240,320,L"240*320"},
    {352,464,L"352*464"}, // VGA 
#elif defined(FEATURE_VERSION_K212_VHOPE)
    {200,240,L"240*320"}, // VGA
#else
	{200,240,L"240*320"}, // VGA
#endif
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
#endif
//Add By zzg 2011_12_17
//10万像素的选项
static const CCameraSize g_VideoSizeCFG_10[] = 
{
#if defined(FEATURE_DISP_160X128)
	//{128,160,L"128*160"}, // FULL Screen
    //{144,176,L"144*176"}, // QCIF
    {160,128,L"160*128"}, // FULL Screen
    {176,144,L"176*144"}, // QCIF
#elif defined(FEATURE_DISP_220X176)
	//{128,160,L"128*160"}, // FULL Screen
    //{144,176,L"144*176"}, // QCIF
    {160,128,L"160*128"}, // FULL Screen
    {176,144,L"176*144"}, // QCIF
#elif defined(FEATURE_DISP_128X160)
	//{128,160,L"128*160"}, // FULL Screen
    //{144,176,L"144*176"}, // QCIF
    {128,160,L"128*160"}, // FULL Screen
    {144,176,L"144*176"}, // QCIF
#elif defined(FEATURE_DISP_176X220)
	//{128,160,L"128*160"}, // FULL Screen
    //{144,176,L"144*176"}, // QCIF
    {128,160,L"128*160"}, // FULL Screen
    {144,176,L"144*176"}, // QCIF
#if defined(FEATURE_VERSION_W208S)	
    {320,240,L"320*240"}, // QCIF    
#endif
#elif defined(FEATURE_DISP_240X320)
	//{128,160,L"128*160"}, // FULL Screen
    //{176,220,L"176*220"}, // QCIF
#ifdef FEATURE_CAMERA_8W       
    {144,176,L"144*176"}, // QCIF
#elif defined(FEATURE_VERSION_IC241A_MMX)
    {128,160,L"128*160"}, // FULL Screen
    {144,176,L"144*176"}, // QCIF
#else  
    {480,640,L"480*640"}, // VGA 
    {600,800,L"600*800"}, // VGA   
#endif
#elif defined(FEATURE_DISP_320X240)
	//{128,160,L"128*160"}, // FULL Screen
    //{144,176,L"144*176"}, // QCIF
    {160,128,L"160*128"}, // FULL Screen
    {176,144,L"176*144"}, // QCIF
#else
	//{128,160,L"128*160"}, // FULL Screen
    //{144,176,L"144*176"}, // QCIF
    {160,128,L"160*128"}, // FULL Screen
    {176,144,L"176*144"}, // QCIF
#endif
    {0,0,NULL}
};
//Add End

/*==============================================================================
                                 函数定义
==============================================================================*/


/*==============================================================================
函数:
       CameraApp_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向CameraApp Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:
       

==============================================================================*/
void CameraApp_ShowDialog(CCameraApp *pMe,uint16  dlgResId)
{
    int nRet;

    // At most one dialog open at once
    if(ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // Looks like there is one dialog already opened.
        // Flag an error an return without doing anything.
        return;
    }
    
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

    
    nRet = ISHELL_CreateDialog(pMe->m_pShell, AEE_APPSCAMERAAPP_RES_FILE, dlgResId, NULL);
    
    if(nRet != SUCCESS)
    {
        return;
    }
}

/*==============================================================================
函数:
       CameraApp_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向CameraApp Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean CameraApp_RouteDialogEvent(CCameraApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{   
    if((NULL == pMe) || (NULL == pMe->m_pActiveDlg))
    {
        return FALSE;
    }
    switch(pMe->m_pActiveDlgID)
    {
        case IDD_CMAINMENU:
            return CameraApp_MainMenuHandleEvent(pMe, eCode, wParam, dwParam);
            
        case IDD_CPREVIEW:
            return CameraApp_PreviewHandleEvent(pMe, eCode, wParam, dwParam);

        case IDD_CCAMERACFG:
            return CameraApp_CameraCFGHandleEvent(pMe, eCode, wParam, dwParam);
  
        case IDD_CPIC:
            return CameraApp_PicHandleEvent(pMe, eCode, wParam, dwParam);

		case IDD_CVIDEO:
            return CameraApp_VideoHandleEvent(pMe, eCode, wParam, dwParam);	

        case IDD_POPMSG:
            return CameraApp_PopMSGHandleEvent(pMe, eCode, wParam, dwParam);

        case IDD_CAMERA_PHOTO_MODE:
            return CameraApp_CameraPhotoModeHandleEvent(pMe,eCode,wParam,dwParam);
                           
        default:
            return FALSE;
    }
}

/*==============================================================================
函数：
       CameraApp_MainMenuHandleEvent
说明：
       IDD_CMAINMENU对话框事件处理函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean CameraApp_MainMenuHandleEvent(CCameraApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
        
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_CAMERA_MAINMENU);
	AECHAR WTitle[40] = {0};
    
    if(pMenu == NULL)
    {
        return FALSE;
    }
   
    switch(eCode) 
    {
        case EVT_DIALOG_INIT:
            pMe->m_bIsPreview = FALSE;
			pMe->m_bCanPress = TRUE;
			pMe->m_bCapturePic = FALSE;
            pMe->m_nCameraState = CAM_START;
            
            pMe->m_wMsgID = IDS_MSG_WAITING;
            pMe->m_nMsgTimeout = TIMEOUT_MS_MSGBOX;
            #ifdef FEATURE_VERSION_K212_ND
			(void)ISHELL_LoadResString(pMe->m_pShell,
                                AEE_APPSCAMERAAPP_RES_FILE,                                
                                IDS_ITEM_CAMERA1,
                                WTitle,
                                sizeof(WTitle));
			#else
			(void)ISHELL_LoadResString(pMe->m_pShell,
                                AEE_APPSCAMERAAPP_RES_FILE,                                
                                IDS_ITEM_CAMERA,
                                WTitle,
                                sizeof(WTitle));
			#endif
			if(pMe->m_pIAnn != NULL)
        	{
		    	IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
			}
                
            IDISPLAY_SetClipRect(pMe->m_pDisplay, NULL); 

            IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,TRUE);

            // 设置菜单属性
            IMENUCTL_SetProperties(pMenu, MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
            
            // 初始化菜单项
            if (pMe->m_isRecordMode == TRUE)
            {
				//IMENUCTL_AddItem(pMenu, AEE_APPSCAMERAAPP_RES_FILE, IDS_VIDEO_CAMERA, IDS_VIDEO_CAMERA, NULL, NULL);
            	//IMENUCTL_AddItem(pMenu, AEE_APPSCAMERAAPP_RES_FILE, IDS_ITEM_CAMERA_GALLERY, IDS_ITEM_CAMERA_GALLERY, NULL, NULL);
			}
			else
			{
				IMENUCTL_AddItem(pMenu, AEE_APPSCAMERAAPP_RES_FILE, IDS_ITEM_CAMERA, IDS_ITEM_CAMERA, NULL, NULL);
#ifdef FEATURE_USES_MMS                 
                if(!pMe->m_isFormMMS)
#endif                    
                {
            	    IMENUCTL_AddItem(pMenu, AEE_APPSCAMERAAPP_RES_FILE, IDS_ITEM_CAMERA_GALLERY, IDS_ITEM_CAMERA_GALLERY, NULL, NULL);
                }
			}            
            #if defined(FEATURE_VERSION_W515V3) || defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_K212_VHOPE)
            IMENUCTL_AddItem(pMenu, AEE_APPSCAMERAAPP_RES_FILE, IDS_CAMERA_PHOTO_MODE, IDS_CAMERA_PHOTO_MODE, NULL, NULL);
            #endif
			if(pMe->m_bIsbackMed)
			{
				pMe->m_nMainMenuItemSel = IDS_ITEM_CAMERA_GALLERY;
			}
            IMENUCTL_SetSel(pMenu, pMe->m_nMainMenuItemSel);
            IMENUCTL_SetBottomBarType(pMenu, BTBAR_SELECT_BACK);
            IANNUNCIATOR_Redraw(pMe->m_pIAnn);
            return TRUE;
     
        case EVT_DIALOG_START:
            if(pMe->m_pCamera)
            {
                ICAMERA_Release(pMe->m_pCamera);
                pMe->m_pCamera = NULL;
            }       
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;
            
        case EVT_COMMAND:          
            switch(wParam) 
            {
            	case IDS_VIDEO_CAMERA:
                case IDS_ITEM_CAMERA: 
#if defined(FEATURE_VERSION_W515V3) || defined(FEATURE_VERSION_S1000T)||defined(FEATURE_VERSION_K212_VHOPE)
                    {
                      boolean cameraphotopath = FALSE;
                      OEM_GetConfig(CFGI_CAMERA_PHOTO_MODE,&cameraphotopath, sizeof(cameraphotopath));
                      pMe->m_isStartFromFacebook = cameraphotopath;
                    } 
#endif
                    // set the annunciator disable
                    IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,FALSE);
                    
                    pMe->m_nMainMenuItemSel = wParam;	//IDS_ITEM_CAMERA;

                    pMe->m_bMemoryCardExist = CameraApp_FindMemoryCardExist(pMe);
#ifdef FEATURE_VERSION_FLEXI203P

                    // 如果T卡不存在，defualt保存在手机里
                    if(!pMe->m_bMemoryCardExist)
                    {
                        pMe->m_nCameraStorage = OEMNV_CAMERA_STORAGE_PHONE;
                        
                        pMe->m_nCameraSize = OEMNV_CAMERA_SIZE_DEFAULT;

                        (void)ICONFIG_SetItem(pMe->m_pConfig,
                                              CFGI_CAMERA_STORAGE,
                                              &pMe->m_nCameraStorage,
                                              sizeof(pMe->m_nCameraStorage));  

                        (void)ICONFIG_SetItem(pMe->m_pConfig,
                                              CFGI_CAMERA_SIZE,
                                              &pMe->m_nCameraSize,
                                              sizeof(pMe->m_nCameraSize)); 
                    }
                    else
                    {
                        pMe->m_nCameraStorage = OEMNV_CAMERA_STORAGE_MEMORY_CARD;
                    }
#else
                    if ( pMe->m_isStartFromFacebook == TRUE )
					{
						pMe->m_nCameraStorage = OEMNV_CAMERA_STORAGE_PHONE;
					}
					else
					{
                    	pMe->m_nCameraStorage = OEMNV_CAMERA_STORAGE_MEMORY_CARD;
                    }
#endif
                    CLOSE_DIALOG(DLGRET_POPMSG);                    
                    break;

                case IDS_ITEM_CAMERA_GALLERY:
                    pMe->m_nMainMenuItemSel = IDS_ITEM_CAMERA_GALLERY;
                    CMediaGallery_FileExplorer(GALLERY_PHOTO_BROWSE, NULL);
                    break;
#if defined(FEATURE_VERSION_W515V3) || defined(FEATURE_VERSION_S1000T)|| defined(FEATURE_VERSION_K212_VHOPE)                 
                case IDS_CAMERA_PHOTO_MODE:
                    pMe->m_nMainMenuItemSel = IDS_CAMERA_PHOTO_MODE;
                    MSG_FATAL("IDS_CAMERA_PHOTO_MODE-------------",0,0,0);
                    CLOSE_DIALOG(DLGRET_CAMERAPHOTOMODE);
                    break;
#endif

                    
                default:         
                    break;  
            }
            return TRUE;        
             
        case EVT_KEY:
            if(wParam == AVK_CLR)
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
       CameraApp_MainMenuHandleEvent
说明：
       IDD_CameraPhotoMode对话框事件处理函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean CameraApp_CameraPhotoModeHandleEvent(CCameraApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
      
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_PHOTOMODE);
	AECHAR WTitle[40] = {0};  
    if(pMenu == NULL)
    {
        return FALSE;
    }
   
    switch(eCode) 
    {
        case EVT_DIALOG_INIT:
            
			(void)ISHELL_LoadResString(pMe->m_pShell,
                                AEE_APPSCAMERAAPP_RES_FILE,                                
                                IDS_CAMERA_PHOTO_MODE,
                                WTitle,
                                sizeof(WTitle));
			if(pMe->m_pIAnn != NULL)
        	{
		    	IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
			}
                
            IDISPLAY_SetClipRect(pMe->m_pDisplay, NULL); 

            IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,TRUE);

            // 设置菜单属性
           // IMENUCTL_SetProperties(pMenu, MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
           // IMENUCTL_SetOemProperties(pMenu, OEMMP_USE_MENU_STYLE);
            
            // 初始化菜单项
            IMENUCTL_AddItem(pMenu, AEE_APPSCAMERAAPP_RES_FILE, IDS_CAMERA_PHOTO_PHONE, IDS_CAMERA_PHOTO_PHONE, NULL, NULL);
            IMENUCTL_AddItem(pMenu, AEE_APPSCAMERAAPP_RES_FILE, IDS_CAMERA_PHOTO_TFCARD, IDS_CAMERA_PHOTO_TFCARD, NULL, NULL);
            
           // IMENUCTL_SetSel(pMenu, pMe->m_nMainMenuItemSel);
            IMENUCTL_SetBottomBarType(pMenu, BTBAR_SELECT_BACK);
            
            return TRUE;
     
        case EVT_DIALOG_START:
            {
              boolean cameraphotopath = FALSE;
              uint16 ui16_return = IDS_CAMERA_PHOTO_TFCARD;
              OEM_GetConfig(CFGI_CAMERA_PHOTO_MODE,&cameraphotopath, sizeof(cameraphotopath));
             	if(cameraphotopath)
             	{
             		ui16_return = IDS_CAMERA_PHOTO_PHONE;
             	}
              InitMenuIcons(pMenu);
              SetMenuIcon(pMenu, ui16_return, TRUE);
              IMENUCTL_SetSel(pMenu, ui16_return);
              IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);
              IMENUCTL_SetOemProperties( pMenu , OEMMP_USE_MENU_STYLE);    
           }
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;
            
        case EVT_COMMAND: 
            {
                boolean cameraphotopath = FALSE;
                switch(wParam) 
                {
                    case IDS_CAMERA_PHOTO_PHONE:
                        pMe->m_nMainMenuItemSel = IDS_CAMERA_PHOTO_PHONE;
                        cameraphotopath=TRUE;
                        break;
                    case IDS_CAMERA_PHOTO_TFCARD:
                        pMe->m_nMainMenuItemSel = IDS_CAMERA_PHOTO_TFCARD;
                        cameraphotopath=FALSE;
                        break;

                        
                    default:         
                        break;  
                }
                OEM_SetConfig(CFGI_CAMERA_PHOTO_MODE,&cameraphotopath, sizeof(cameraphotopath));
                CLOSE_DIALOG(DLGRET_MAINMENU);
            }
            return TRUE;        
             
        case EVT_KEY:
            if(wParam == AVK_CLR)
            {
                CLOSE_DIALOG(DLGRET_MAINMENU);
            }
            return TRUE;
   
        default:
            break;
    }
    return FALSE;
}

/*==============================================================================
函数：
       CameraApp_PreviewHandleEvent
说明：
       IDD_CPREVIEW对话框事件处理函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean CameraApp_PreviewHandleEvent(CCameraApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    int   nCameraSelfTime = 0;

	//uint32  dwTotal = 0;
	//uint32 free = 0;
	//GETFSFREE(&dwTotal);
	//free = GETRAMFREE(NULL,NULL);
	//MSG_FATAL("ydc CameraApp dwTotal======%d,free====%d",dwTotal,free,0);

    MSG_FATAL("CameraApp_PreviewHandleEvent eCode=0x%x, wParam=0x%x",eCode, wParam, 0);
	
    if(pMe->m_isPicCamera)
    {
        if(eCode==EVT_KEY)
          return TRUE;
    }
    switch(eCode) 
    {
        case EVT_DIALOG_INIT:
            pMe->m_bCapturePic = FALSE;
			pMe->m_bIsSnaping = FALSE;
			pMe->m_bCanPress = FALSE;
             // 如下代码限制在此界面的快速按键
            //#if defined (FEATURE_VERSION_C310) || defined (FEATURE_VERSION_C337)
            //1200
			#ifdef FEATURE_VERSION_K212
            (void)ISHELL_SetTimer( pMe->m_pShell,
                                                500,
                                                CameraApp_PrevewTimeout,
                                                pMe );
            #else
			(void)ISHELL_SetTimer( pMe->m_pShell,
                                                300,
                                                CameraApp_PrevewTimeout,
                                                pMe );
			#endif
            //#endif
            IDISPLAY_SetClipRect(pMe->m_pDisplay, NULL); 


            (void)ICONFIG_GetItem(pMe->m_pConfig,
                                  CFGI_CAMERA_SIZE,
                                  &pMe->m_nCameraSize,
                                  sizeof(pMe->m_nCameraSize));

            (void)ICONFIG_GetItem(pMe->m_pConfig,
                                  CFGI_CAMERA_QUALITY,
                                  &pMe->m_nCameraQuality,
                                  sizeof(pMe->m_nCameraQuality));
            

            (void)ICONFIG_GetItem(pMe->m_pConfig,
                                  CFGI_CAMERA_ENVIROMENT,
                                  &pMe->m_nCameraEnviroment,
                                  sizeof(pMe->m_nCameraEnviroment));

            (void)ICONFIG_GetItem(pMe->m_pConfig,
                                  CFGI_CAMERA_BANDING,
                                  &pMe->m_nCameraBanding,
                                  sizeof(pMe->m_nCameraBanding));
                                  
            IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);
            return TRUE;
            
        case EVT_DIALOG_START:     
        	MSG_FATAL("CameraApp_PreviewHandleEvent  EVT_DIALOG_START",0,0,0);
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
                	
            pMe->m_bCanCapture = CameraApp_IsEnoughfMemorySpace(pMe);
            // For redraw the dialog
            (void)ISHELL_PostEvent(pMe->m_pShell,
                                   AEECLSID_APP_CAMERA,
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
            //#if defined (FEATURE_VERSION_C310) || defined (FEATURE_VERSION_C337)
            pMe->m_bCanPress = FALSE;
 			pMe->m_bIsSnaping = FALSE;
			pMe->m_isPicCamera = FALSE;
            //#endif
            (void)ISHELL_CancelTimer(pMe->m_pShell,
                                     CameraApp_PrevewTimeout,
                                     pMe);
 			//Add End
            
            ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
            return TRUE;
            
        case EVT_USER_REDRAW:
			MSG_FATAL("EVT_USER_REDRAW........0000000000000000000",0,0,0);
            #ifdef FEATURE_VERSION_X3
            if(pMe->m_nCameraMulti == OEMNV_CAMERA_MULTI_ONE)
            {
              gc0329_0p1mp_write_more_registers(0);
            }
            else
            {
              gc0329_0p1mp_write_more_registers(1);
            }
            #endif
            // camera preview start....
#ifndef FEATURE_DSP            
            if(pMe->m_pCamera && (!pMe->m_bIsPreview))
            {
                CameraApp_CPreviewStart(pMe);
				MSG_FATAL("CameraApp_CPreviewStart........000000",0,0,0);
                //return TRUE;
            }            
            if(pMe->m_bRePreview && pMe->m_pCamera)
            {
                CameraApp_CPreviewStart(pMe);
                pMe->m_bRePreview = FALSE;
				MSG_FATAL("CameraApp_CPreviewStart........2222222",0,0,0);
                //return TRUE;
                
            }
#else
			if(pMe->m_pCamera && (!pMe->m_bIsPreview))
            {
            	MSG_FATAL("EVT_USER_REDRAW........11111111111111",0,0,0);
                CameraApp_CPreviewStart(pMe);
                return TRUE;
            }            
            if(pMe->m_bRePreview && pMe->m_pCamera)
            {
            	MSG_FATAL("EVT_USER_REDRAW........222222222222222",0,0,0);
                CameraApp_CPreviewStart(pMe);
                pMe->m_bRePreview = FALSE;
                return TRUE;
                
            }
            pMe->m_nCameraState = CAM_PREVIEW;  
            CameraApp_UpdateInit(pMe);
#endif
            if(pMe->m_nLeftTime)
            {
                IImage *pImage = NULL;
                
                pImage = ISHELL_LoadResImage(pMe->m_pShell, CAMERAAPP_IMAGE_RES_FILE, IDI_COUNT_DOWN);
                
                if(pImage)
                {
                    IImage_SetDrawSize(pImage, 60, 60);
                    
                    IImage_SetOffset(pImage, (pMe->m_nLeftTime-1)*60, 0);

                    IImage_Draw(pImage, (pMe->m_rc.dx-60)/2, (pMe->m_rc.dy-60)/2);

                    IImage_Release(pImage);

                    pImage = NULL;
                }
            }
            
            MSG_FATAL("EVT_USER_REDRAW....................",0,0,0);
            CameraApp_DrawMidPic(pMe);

            if ( pMe->m_nCameraState == CAM_RECORDING )
            {
            	AECHAR      wszRecordTime[20]={0};  //hour:min:second
            	
            	(void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_RED);

				MEMSET(wszRecordTime, 0, sizeof(AECHAR)*10);

				WSPRINTF(wszRecordTime, 18, L"%02d:%02d:%02d", pMe->RecordingTime.Hour, pMe->RecordingTime.Min, pMe->RecordingTime.Sec);
			    // 绘制文本-左键
			    if(WSTRLEN(wszRecordTime)>0)
			    {      
			        DrawTextWithProfile(pMe->m_pShell, 
			                            pMe->m_pDisplay, 
			                            RGB_BLACK, 
			                            AEE_FONT_NORMAL, 
			                            wszRecordTime, 
			                            -1, 
			                            0, 
			                            0, 
			                            &pMe->m_rc, 
			                            IDF_ALIGN_TOP|IDF_ALIGN_LEFT|IDF_TEXT_TRANSPARENT);
			    }

			    (void)IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
				CameraApp_DrawBottomBarText(pMe, BTBAR_SEND_BACK);
            }
            else
            {
            	CameraApp_DrawBottomBarText(pMe, BTBAR_OPTION_BACK);
            }
            CameraApp_SetParamAfterPreview(pMe);
#ifdef FEATURE_DSP
            CameraApp_Update(pMe);
#else
            IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
#endif

			//GETFSFREE(&dwTotal);
			//free = GETRAMFREE(NULL,NULL);
			//MSG_FATAL("preview CameraApp dwTotal======%d,free===%d",dwTotal,free,0);
            return TRUE;

        case EVT_KEY:
            //#if defined (FEATURE_VERSION_C310) || defined (FEATURE_VERSION_C337)
                if(!pMe->m_bCanPress)
                {
                    return TRUE;
                }
            //#endif
            MSG_FATAL("CameraApp_PreviewHandleEvent EVT_KEY wParam=0x%x", wParam,0,0);
            switch(wParam)
            {
                case AVK_SELECT: 
                    if(pMe->m_bCapturePic == FALSE)
                    {
                        CLOSE_DIALOG(DLGRET_CAMERACFG);
						pMe->m_bIsSnaping = FALSE;
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
			MSG_FATAL("***zzg CameraApp_PreviewHandleEvent EVT_KEY_PRESS wParam=%x, m_bCapturePic=%x", wParam,pMe->m_bCapturePic,0);
            //#if defined (FEATURE_VERSION_C310) || defined (FEATURE_VERSION_C337)
                if(!pMe->m_bCanPress)
                {
                    return TRUE;
                }
            //#endif
            // 如果当前在拍照，不处理AVK_END按键以外的按键事件
            if((wParam != AVK_END) && (pMe->m_bCapturePic == TRUE))
            {
                return TRUE;
            }
            MSG_FATAL("CameraApp_PreviewHandleEvent EVT_KEY_PRESS wParam=0x%x", wParam,0,0);
            switch(wParam){
            case AVK_END:
                ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
                if(pMe->m_nCameraState == CAM_PREVIEW)
                {
                    ICAMERA_Stop(pMe->m_pCamera);
                    pMe->m_nCameraState = CAM_STOP;
                }
                return FALSE;

            case AVK_CLR:
                if((pMe->m_isFormQuicktest)       
#ifdef FEATURE_USES_MMS                    
                   || (pMe->m_isFormMMS) 
#endif
                    )
                {
                    ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
                    if(pMe->m_nCameraState == CAM_PREVIEW)
                    {
                        ICAMERA_Stop(pMe->m_pCamera);
                        pMe->m_nCameraState = CAM_STOP;
                    }                      
#ifdef FEATURE_USES_MMS  
                    pMe->m_isFormMMS = FALSE;
#endif
                    pMe->m_isFormQuicktest = FALSE;
                    pMe->m_nLeftTime = 0;
                    ISHELL_CloseApplet(pMe->m_pShell, FALSE);
                    return TRUE;
                }
                if ( pMe->m_nCameraState == CAM_RECORDING )
                {
                	pMe->m_nCameraState = CAM_STOPRECORD;
					if(SUCCESS == ICAMERA_Stop(pMe->m_pCamera))
	                {
	                    pMe->m_bIsPreview = FALSE;
	                    (void)ISHELL_SendEvent(pMe->m_pShell, AEECLSID_APP_CAMERA, EVT_USER_REDRAW, 0, 0);						
	                } 
                }
                else
                {
                		#if defined(FEATURE_VERSION_K212) || defined(FEATURE_LOW_MEM_BIGFONT)
                	     nv_item_type	SimChoice;
						(void)OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
						if(SimChoice.sim_select != 1)
						{
							if(SUCCESS == ICAMERA_Stop(pMe->m_pCamera))
	                		{
	                    		pMe->m_bIsPreview = FALSE;
	                    		pMe->m_nLeftTime  = 0;
	                    		pMe->m_nCameraState = CAM_STOP;
	                    		CLOSE_DIALOG(DLGRET_CANCELED);
	                		}  
						}
						else
						{
							   pMe->m_bIsPreview = FALSE;
	                    		pMe->m_nLeftTime  = 0;
	                    		pMe->m_nCameraState = CAM_STOP;
	                    		CLOSE_DIALOG(DLGRET_CANCELED);
						}
						#else
						if(SUCCESS == ICAMERA_Stop(pMe->m_pCamera))
	                	{
	                    		pMe->m_bIsPreview = FALSE;
	                    		pMe->m_nLeftTime  = 0;
	                    		pMe->m_nCameraState = CAM_STOP;
	                    		CLOSE_DIALOG(DLGRET_CANCELED);
	                	}  
						#endif
	                      
                }
                break;

            case AVK_INFO:
                MSG_FATAL("***zzg Camera AVK_INFO m_bIsSnaping=%d***pMe->m_isRecordMode=%d",pMe->m_bIsSnaping,pMe->m_isRecordMode,0);
				
                MSG_FATAL("***zzg Camera AVK_INFO pMe->m_nCameraState=%d***",pMe->m_nCameraState,0,0);    
                //Add By zzg 2013_03_21
                if (pMe->m_isRecordMode == FALSE)   //防止拍照时快速按键造成卡死
                {
    				if (pMe->m_bIsSnaping == FALSE)
                    {
                        pMe->m_bIsSnaping = TRUE;
                    }
                    else
                    {
                        break;
                    }
                }
                else
                {
                    if (pMe->m_nCameraState == CAM_STARTINGRECORD)//防止录像时快速按键造成卡死
                    {
                        break;
                    }                    
                }
                //Add End
                
                // 防止快速按键，导致hotkey Text存在于LCD上 
                MSG_FATAL("AVK_INFO...................",0,0,0);
                ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
#ifdef FEATURE_CAMERA_MULTI_SENSOR				
				(void)ICONFIG_GetItem(pMe->m_pConfig,
                              CFGT_CAMERA_FLASH,
                              &pMe->n_nCameraFlash,
                              sizeof(pMe->n_nCameraFlash));
#endif
                
				MSG_FATAL("AVK_INFO...................end",0,0,0);
                if ( pMe->m_isRecordMode == FALSE)
                {
                	
	                if(!pMe->m_bCanCapture)
	                {
	                	if ( pMe->m_isStartFromFacebook == TRUE)
	                	{
#if defined(FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)   	 
				            pMe->m_wMsgID = IDS_NOMEMORY;
#else
	                    	pMe->m_wMsgID = IDS_MSG_NOMEMORY;
#endif
	                    }
	                    else
	                    {
							if(pMe->m_bMemoryCardExist)
		                    {
#if defined(FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)   	 
    				            pMe->m_wMsgID = IDS_NOMEMORY;
#else
    	                    	pMe->m_wMsgID = IDS_MSG_NOMEMORY;
#endif
		                    }
		                    else
		                    {
#ifdef FEATURE_VERSION_FLEXI203P
								pMe->m_wMsgID = IDS_MSG_PHONE_MEMERY_FULL_AND_NOSDCARD;
#else
	                        	pMe->m_wMsgID = IDS_MSG_NOSDCARD;
#endif
		                    }
	                    }

	                    pMe->m_nMsgTimeout = TIMEOUT_MS_MSGBOX;
	                    ICAMERA_Stop(pMe->m_pCamera);
	                    pMe->m_bIsPreview = FALSE;
	                    CLOSE_DIALOG(DLGRET_POPMSG);	                   
						
	                    return TRUE;
	                }
	                
	                switch(pMe->m_nSelfTimeItemSel)
	                {
	                    case IDS_SELFTIME_OFF:
	                        nCameraSelfTime = 0;
	                        break;

	                    case IDS_SELFTIME_SEC1:
	                        nCameraSelfTime = 6;
	                        break;

	                    case IDS_SELFTIME_SEC2:
	                        nCameraSelfTime = 11;
	                        break;

	                    case IDS_SELFTIME_SEC3:
	                        nCameraSelfTime = 16;
	                        break;

                    	default:
                        	return FALSE;        
                	}
                
                	pMe->m_nLeftTime = nCameraSelfTime;              
                
                	if(nCameraSelfTime == 0)
                	{
                		#ifdef FEATURE_LCD_TOUCH_ENABLE
                		if(pMe->m_nCameraMulti == OEMNV_CAMERA_MULTI_TWO)
                		{
#ifdef FEATURE_CAMERA_MULTI_SENSOR                		
                			if(pMe->n_nCameraFlash == OEMNV_CAMERA_FLASH_ON)
                			{
                				ICAMERA_ControlFlash(pMe->m_pCamera,TRUE);
                			}
#endif							
                		}
						#endif
	                    pMe->m_nCameraState = CAM_CAPTURE;
#ifdef FEATURE_DSP
	                    CameraApp_RecordSnapShot(pMe);
	                    ICAMERA_Stop(pMe->m_pCamera);
	                    CameraApp_SavePhoto(pMe);
	                    CameraApp_PlayShutterSound(pMe);
	                    CLOSE_DIALOG(DLGRET_PICMENU);
#else
	                    ICAMERA_Stop(pMe->m_pCamera);
#endif
						#ifdef FEATURE_LCD_TOUCH_ENABLE
						ICAMERA_ControlFlash(pMe->m_pCamera,FALSE);
						#endif
	                }
	                else
	                {
	                    // 倒计时拍摄                       
	                    CameraApp_SelfTimeRecordSnapShot(pMe);
	                }
                }
                else
                {
					if(!pMe->m_bCanCapture)
	                {
						if(pMe->m_bMemoryCardExist)
	                    {
#if defined(FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)   	 
				            pMe->m_wMsgID = IDS_NOMEMORY;
#else
	                    	pMe->m_wMsgID = IDS_MSG_NOMEMORY;
#endif
	                    }
	                    else
	                    {
                        	pMe->m_wMsgID = IDS_MSG_NOSDCARD;
	                    }

	                    pMe->m_nMsgTimeout = TIMEOUT_MS_MSGBOX;
	                    ICAMERA_Stop(pMe->m_pCamera);
	                    pMe->m_bIsPreview = FALSE;
	                    CLOSE_DIALOG(DLGRET_POPMSG);	                    
	                    return TRUE;
	                }            

	                ICAMERA_Stop(pMe->m_pCamera);   

					//Add By zzg 2012_01_02
					MSG_FATAL("***zzg Priview m_nCameraState=%x***", pMe->m_nCameraState, 0, 0);
					
					if (pMe->m_nCameraState == CAM_RECORDING)
					{
						CLOSE_DIALOG(DLGRET_VIDEOMENU);
					}
					else
					//Add End	
					{
						pMe->m_nCameraState = CAM_STARTRECORD;	
	                	CameraApp_RecordVideo(pMe);
					}
                }
                break;
                    
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
                MSG_FATAL("CameraApp_PreviewHandleEvent EVT_PEN_UP, wXPos=%d, wYPos=%d",wXPos, wYPos, 0);
				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

				if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
				{
                    IImage *pImage = NULL;
                    AEEImageInfo myInfo;
                    MSG_FATAL("CameraApp_PreviewHandleEvent TOUCH_PT_IN_RECT",0, 0, 0);
                    pImage = ISHELL_LoadResImage(pMe->m_pShell,
                                             CAMERAAPP_IMAGE_RES_FILE,
                                             IDI_MID_CAMERA);
                    if(pImage != NULL)
                    {
                        IIMAGE_GetInfo(pImage, &myInfo);
                        IIMAGE_Release(pImage);
                        pImage = NULL;
                    }
                    
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  CameraApp_PreviewHandleEvent(pMe, EVT_KEY, AVK_SELECT, 0);
                        MSG_FATAL("CameraApp_PreviewHandleEvent left rt=%d",rt, 0, 0);
						return rt;
					}
					else if(wXPos >= (pMe->m_cxWidth - myInfo.cx)/2   && wXPos < (pMe->m_cxWidth - myInfo.cx)/2+myInfo.cx )//中
					{
						 boolean rt = CameraApp_PreviewHandleEvent(pMe, EVT_KEY_PRESS, AVK_INFO, 0);
                         MSG_FATAL("CameraApp_PreviewHandleEvent mid rt=%d",rt, 0, 0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//右
					{						
						 boolean rt = CameraApp_PreviewHandleEvent(pMe, EVT_KEY_PRESS, AVK_CLR, 0);
                         MSG_FATAL("CameraApp_PreviewHandleEvent right rt=%d",rt, 0, 0);
						 return rt;
					}
				}

			}
			break;
#endif            

        case EVT_NO_CLOSEBACKLIGHT:
            return TRUE;
            
        default:
            break;
    }
    
    return FALSE;
}

/*==============================================================================
函数：
       CameraApp_CameraCFGHandleEvent
说明：
       IDD_CCAMERACFG对话框事件处理函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean CameraApp_CameraCFGHandleEvent(CCameraApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    IMenuCtl *popMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_CAMERA_CFGMENU);    
    MSG_FATAL("CameraApp_CameraCFGHandleEvent eCode=0x%x, wParam=0x%x",eCode,wParam,0);
    if(popMenu == NULL)
    {
        return FALSE;
    }
    switch(eCode) 
    {
        case EVT_DIALOG_INIT:
            IMENUCTL_SetProperties(popMenu, MP_WRAPSCROLL|MP_TRANSPARENT_UNSEL|MP_NO_REDRAW);
            IMENUCTL_SetOemProperties(popMenu, OEMMP_GRAPHIC_UNDERLINE);
            
            CameraApp_InitpopMenu(pMe, popMenu);
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
#ifdef FEATURE_DSP
            ISHELL_PostEvent(pMe->m_pShell, AEECLSID_APP_CAMERA, EVT_USER_REDRAW, NULL, NULL); 
#endif
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
#ifdef FEATURE_CAMERA_NOFULLSCREEN
            IDISPLAY_FillRect(pMe->m_pDisplay, &pMe->m_rc, RGB_BLACK);
            IDISPLAY_Update(pMe->m_pDisplay);
#endif
            return TRUE;
      
        case EVT_USER_REDRAW: 
        	MSG_FATAL("CameraApp_CameraCFGHandleEvent.....EVT_USER_REDRAW",0,0,0);
#ifndef FEATURE_DSP
			if(pMe->m_bRePreview)
            {
                CameraApp_CPreviewStart(pMe);
                pMe->m_bRePreview = FALSE;
            }
#else
			if(pMe->m_bRePreview)
            {
            	MSG_FATAL("EVT_USER_REDRAW...CFG.....0000000000000000000",0,0,0);
                CameraApp_CPreviewStart(pMe);
                pMe->m_bRePreview = FALSE;
            }
            pMe->m_nCameraState = CAM_PREVIEW; 
            pMe->m_bRePreview = TRUE;
            CameraApp_UpdateInit(pMe);
#endif
            IDISPLAY_SetClipRect(pMe->m_pDisplay, NULL);
            {
                #ifdef FEATURE_LOW_MEM_BIGFONT1
				#ifdef FEATURE_VERSION_K212_VHOPE
                IImage *pFrameImage = NULL;
                pFrameImage = ISHELL_LoadResImage(pMe->m_pShell, CAMERAAPP_IMAGE_RES_FILE, IDI_FRAME_11);
                if(pFrameImage)
                {
                    IImage_Draw(pFrameImage, 20, 0);
                    IImage_Draw(pFrameImage, 20, 280);
                    IImage_Release(pFrameImage);
                    pFrameImage = NULL;
                }
                 pFrameImage = ISHELL_LoadResImage(pMe->m_pShell, CAMERAAPP_IMAGE_RES_FILE, IDI_FRAME_12);
                if(pFrameImage)
                {
                    IImage_Draw(pFrameImage, 0, 0);
                    IImage_Draw(pFrameImage, 220, 0);
                    IImage_Release(pFrameImage);
                    pFrameImage = NULL;
                }
                #else
                IImage *pFrameImage = NULL;
                pFrameImage = ISHELL_LoadResImage(pMe->m_pShell, CAMERAAPP_IMAGE_RES_FILE, IDI_FRAME_11);
                if(pFrameImage)
                {
                    IImage_Draw(pFrameImage, 0, 275);
                    IImage_Release(pFrameImage);
                    pFrameImage = NULL;
                }
                 pFrameImage = ISHELL_LoadResImage(pMe->m_pShell, CAMERAAPP_IMAGE_RES_FILE, IDI_FRAME_12);
                if(pFrameImage)
                {
                    IImage_Draw(pFrameImage, 0, 0);
                    IImage_Release(pFrameImage);
                    pFrameImage = NULL;
                }
                 pFrameImage = ISHELL_LoadResImage(pMe->m_pShell, CAMERAAPP_IMAGE_RES_FILE, IDI_FRAME_13);
                if(pFrameImage)
                {
                    IImage_Draw(pFrameImage, 0, 0);
                    IImage_Release(pFrameImage);
                    pFrameImage = NULL;
                }
                 pFrameImage = ISHELL_LoadResImage(pMe->m_pShell, CAMERAAPP_IMAGE_RES_FILE, IDI_FRAME_14);
                if(pFrameImage)
                {
                    IImage_Draw(pFrameImage, 203, 0);
                    IImage_Release(pFrameImage);
                    pFrameImage = NULL;
                }
				#endif
                #endif
                
            }
            CameraApp_DrawBottomBarText(pMe, BTBAR_SELECT_BACK);
            CameraApp_DrawTopBar(pMe);   
            CameraApp_DrawCFGPromptText(pMe);                        
            
            IMENUCTL_Redraw(popMenu);		
#ifdef FEATURE_DSP
            CameraApp_Update(pMe);
#endif
            
            return TRUE;

        case EVT_KEY:
        case EVT_KEY_RELEASE:
        	#ifdef FEATURE_DSP
            ISHELL_SendEvent(pMe->m_pShell, AEECLSID_APP_CAMERA, EVT_USER_REDRAW, NULL, NULL);
			#endif
            return TRUE;
            
        case EVT_KEY_PRESS:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED);
                    return TRUE;

                case AVK_LEFT:  
                    MSG_FATAL("CameraApp_CameraCFGHandleEvent:L:%d",pMe->m_nCameraCFG,0,0);
                    if(pMe->m_nCameraCFG == CAMERACFGFIRST)
                    {
                        pMe->m_nCameraCFG = CAMERACFGLAST;
                    }
                    else
                    {
                        pMe->m_nCameraCFG--;
                    }
/*                    
#ifdef FEATURE_VERSION_IC241A_MMX
                    if(pMe->m_nCameraCFG == CAMERACFGSELFTIME )
                    {
                        if(pMe->m_isRecordMode == TRUE)
                        {                           
                            pMe->m_nCameraCFG -= 3;
                        }
                    }
					
					if(pMe->m_nCameraCFG == CAMERACFGTONE )
                    {
                        if(pMe->m_isRecordMode == TRUE)
                        {
                            pMe->m_nCameraCFG -= 2;                            
                        }
                    }

                    if(pMe->m_nCameraCFG == CAMERACFGSIZE )
                    {
                        pMe->m_nCameraCFG -= 1;  
                    }
#else
*/
                    if(pMe->m_nCameraCFG == CAMERACFGSELFTIME )
                    {
                        if(pMe->m_isRecordMode == TRUE)
                        {
                            //pMe->m_nCameraCFG--;
                            pMe->m_nCameraCFG -= 2;
                        }
                    }
					
					if(pMe->m_nCameraCFG == CAMERACFGTONE )
                    {
                        if(pMe->m_isRecordMode == TRUE)
                        {
                            pMe->m_nCameraCFG--;                            
                        }
                    }
//#endif                    
                    break;
   
                case AVK_RIGHT:
                    MSG_FATAL("CameraApp_CameraCFGHandleEvent:R:%d",pMe->m_nCameraCFG,0,0);
                    if(pMe->m_nCameraCFG == CAMERACFGLAST)
                    {
					    pMe->m_nCameraCFG = CAMERACFGFIRST;
                    }
                    else
                    {
                        pMe->m_nCameraCFG++;
                    }
/*
#ifdef FEATURE_VERSION_IC241A_MMX
                    if(pMe->m_nCameraCFG == CAMERACFGSIZE )
                    {
                        if(pMe->m_isRecordMode == TRUE)
                        {
                            pMe->m_nCameraCFG += 3;  
                        }
                        else
                        {
                            pMe->m_nCameraCFG += 1;  
                        }
                    }

                    if(pMe->m_nCameraCFG == CAMERACFGTONE )
                    {
                        if(pMe->m_isRecordMode == TRUE)
                        {
                            pMe->m_nCameraCFG += 2;
                        }
                    }
                    
                    if(pMe->m_nCameraCFG == CAMERACFGSELFTIME )
                    {
                        if(pMe->m_isRecordMode == TRUE)
                        {
                            pMe->m_nCameraCFG += 1;
                        }
                    }
                    
#else   
*/
                    if(pMe->m_nCameraCFG == CAMERACFGSELFTIME )
                    {
                        if(pMe->m_isRecordMode == TRUE)
                        {
                            pMe->m_nCameraCFG++;
                        }
                    }
					
					if(pMe->m_nCameraCFG == CAMERACFGTONE )
                    {
                        if(pMe->m_isRecordMode == TRUE)
                        {
                            pMe->m_nCameraCFG+=2;
                        }
                    }
//#endif                    
					
                    MSG_FATAL("CameraApp_CameraCFGHandleEvent RIGHT END:%d",pMe->m_nCameraCFG,0,0);
                    break;

#if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
                case AVK_1:
                     pMe->m_nCameraCFG=CAMERACFGENVIRMENT;
                     break;

                case AVK_2:
                     pMe->m_nCameraCFG=CAMERACFGQUALITY;
                     break;
/*
#if defined(FEATURE_VERSION_IC241A_MMX)     
                case AVK_3:
                     pMe->m_nCameraCFG=CAMERACFGTONE;
                     break;
   
                case AVK_4:
                     pMe->m_nCameraCFG=CAMERACFGSELFTIME;
                     break;
   
                case AVK_5:
                     pMe->m_nCameraCFG=CAMERACFGRESET;
                     break;
#else
*/
                case AVK_3:
                     pMe->m_nCameraCFG=CAMERACFGSIZE;
                     break;
   
                case AVK_4:
                     pMe->m_nCameraCFG=CAMERACFGTONE;
                     break;
   
                case AVK_5:
                     pMe->m_nCameraCFG=CAMERACFGSELFTIME;
                     break;
   
                case AVK_6:
                     pMe->m_nCameraCFG=CAMERACFGRESET;
                     break;
//#endif                     
#endif
                
                case AVK_UP:
                case AVK_DOWN:   
                	#ifdef FEATURE_DSP
            		ISHELL_SendEvent(pMe->m_pShell, AEECLSID_APP_CAMERA, EVT_USER_REDRAW, NULL, NULL);
					#endif
                    return TRUE;

                case AVK_END:
                	MSG_FATAL("AVK_END...................",0,0,0);
                	CLOSE_DIALOG(DLGRET_CANCELED);
                	ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
	                if(pMe->m_nCameraState == CAM_PREVIEW)
	                {
	                    ICAMERA_Stop(pMe->m_pCamera);
	                    pMe->m_nCameraState = CAM_STOP;
	                }
                	MSG_FATAL("AVK_END...................2",0,0,0);
                return FALSE;
                    
                default:
                    return TRUE;
            }
            
            //左右键切换不同的设置菜单，需要重新初始化菜单项
            CameraApp_InitpopMenu(pMe, popMenu);
#ifdef FEATURE_DSP
			
            ISHELL_SendEvent(pMe->m_pShell, AEECLSID_APP_CAMERA, EVT_USER_REDRAW, NULL, NULL);
#endif
            return TRUE;//ISHELL_SendEvent(pMe->m_pShell, AEECLSID_APP_CAMERA, EVT_USER_REDRAW, NULL, NULL);
#if defined (FEATURE_VERSION_W317A) || defined (FEATURE_VERSION_K212)||defined(FEATURE_LOW_MEM_BIGFONT)
        case EVT_APP_RESUME:    
            CLOSE_DIALOG(DLGRET_CANCELED);
            return TRUE;
#endif
        case EVT_COMMAND:        
            {
                int n = SUCCESS;
                MSG_FATAL("EVT_COMMAND......................",0,0,0);
#ifdef FEATURE_DSP
                
#endif
                DBGPRINTF("cfg key code ----------------%d\n", wParam);
                n = CameraApp_RoutePopMenuCommandEvent(pMe, wParam);
#ifdef FEATURE_DSP
				//if(pMe->m_bRePreview)
				//{
				//	CameraApp_CPreviewStart(pMe);
				//	pMe->m_bRePreview = FALSE;
				//}

                CameraApp_Update(pMe); 
                
#endif
                return n;
            } 
        case EVT_NO_CLOSEBACKLIGHT:
            return TRUE;

#ifdef FEATURE_LCD_TOUCH_ENABLE//andrew add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
                AEERect bar_range[CAMERACFGLAST];
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);
                int i;
                MSG_FATAL("CameraApp_PreviewHandleEvent EVT_PEN_UP, wXPos=%d, wYPos=%d",wXPos, wYPos, 0);
				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

                for (i=CAMERACFGFIRST; i < CAMERACFGLAST+1; i++)
                {
                    SETAEERECT(&(bar_range[i]),
                                   (int16)(0 + (i) * 21),
                                    0,
                                   (int16)(0 + 21+ (i) * 21),
                                    21);
                }
                for(i=CAMERACFGFIRST; i < CAMERACFGLAST+1 ; i++)
                {
                   if(TOUCH_PT_IN_RECT(wXPos,wYPos,bar_range[i])) 
                   {
                      pMe->m_nCameraCFG = i;
                      CameraApp_InitpopMenu(pMe, popMenu);
                   }
                }
                 
                    
				if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
				{
                    IImage *pImage = NULL;
                    AEEImageInfo myInfo;
                    MSG_FATAL("CameraApp_CameraCFGHandleEvent TOUCH_PT_IN_RECT",0, 0, 0);
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  IMENUCTL_HandleEvent(popMenu,eCode,wParam,dwParam);
                        uint16 index = IMENUCTL_GetSel( popMenu );
                        rt = CameraApp_RoutePopMenuCommandEvent(pMe, index);
#ifdef FEATURE_DSP                        
                        CameraApp_Update(pMe); 
#endif
                        MSG_FATAL("CameraApp_CameraCFGHandleEvent left rt=%d index=%d",rt, index, 0);
						return rt;
					}
                    if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//右
					{						
						 boolean rt = CameraApp_CameraCFGHandleEvent(pMe, EVT_KEY_PRESS, AVK_CLR, 0);
                         MSG_FATAL("CameraApp_CameraCFGHandleEvent right rt=%d",rt, 0, 0);
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

/*==============================================================================
函数：
       CameraApp_PicHandleEvent
说明：
       IDD_CPIC对话框事件处理函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean CameraApp_PicHandleEvent(CCameraApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
    MSG_FATAL("CameraApp_PicHandleEvent eCode=0x%x, wParam=0x%x, dwParam=0x%x",eCode, wParam, dwParam);
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
			
            ISHELL_PostEvent(pMe->m_pShell, AEECLSID_APP_CAMERA, EVT_USER_REDRAW, NULL, NULL);
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
            CameraApp_HandleSnapshotPic(pMe);
            return TRUE;
            
        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    pMe->m_isPicCamera=TRUE;
                    if(pMe->m_pFileMgr)
                    {
                        IFILEMGR_Remove(pMe->m_pFileMgr, pMe->m_sCurrentFileName);
                    }
                    break;
                    
                case AVK_SELECT:
                case AVK_INFO:
                    pMe->m_isPicCamera=TRUE;
                    MSG_FATAL("CameraApp_PicHandleEvent AVK_INFO",0,0,0);
#ifdef FEATURE_USES_MMS                    
                    if(pMe->m_isFormMMS)
                    {
                       DBGPRINTF("CameraApp_PicHandleEvent CurrentFileName=%s",pMe->m_sCurrentFileName); 
                       ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSIMAGE,
                                       pMe->m_sCurrentFileName, sizeof(pMe->m_sCurrentFileName));   
                        pMe->m_isFormMMS = FALSE;
                        ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
                        if(pMe->m_nCameraState == CAM_PREVIEW)
                        {
                            ICAMERA_Stop(pMe->m_pCamera);
                            pMe->m_nCameraState = CAM_STOP;
                        }
                        ISHELL_CloseApplet(pMe->m_pShell, FALSE);
                        MSG_FATAL("CameraApp_PopMSGHandleEvent END",0,0,0); 
                        return TRUE;
                    }
#endif                    
                    break;
                    
                default:
                    return TRUE;
            } 
            
            pMe->m_wMsgID = IDS_DONE;
            pMe->m_nMsgTimeout = TIMEOUT_MS_MSGDONE;
           (void)ISHELL_SetTimer(pMe->m_pShell,
                                 3000,
                                 CameraApp_PicDialogTimeout,
                                 pMe);
            CLOSE_DIALOG(DLGRET_POPMSG);
            return TRUE;
 
        case EVT_KEY_PRESS:   
            return TRUE;
 
        case EVT_KEY_RELEASE:
            return TRUE;
 
        case EVT_KEY_HELD:
            return TRUE;  

#ifdef FEATURE_LCD_TOUCH_ENABLE//andrew add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);
                MSG_FATAL("CameraApp_PicHandleEvent EVT_PEN_UP, wXPos=%d, wYPos=%d",wXPos, wYPos, 0);
				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

				if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
				{
                    MSG_FATAL("CameraApp_CameraCFGHandleEvent TOUCH_PT_IN_RECT",0, 0, 0);
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						return CameraApp_PicHandleEvent(pMe, EVT_KEY, AVK_SELECT, 0);
					}
                    if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//右
					{						
						 return CameraApp_PicHandleEvent(pMe, EVT_KEY, AVK_CLR, 0);
					}
				}

			}
			break;
#endif              
    }
    return FALSE;

}

/*==============================================================================
函数：
       CameraApp_VideoHandleEvent
说明：
       IDD_CPIC对话框事件处理函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean CameraApp_VideoHandleEvent(CCameraApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
    MSG_FATAL("CameraApp_VideoHandleEvent eCode=0x%x, wParam=0x%x, dwParam=0x%x",eCode, wParam, dwParam);
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
			
            ISHELL_PostEvent(pMe->m_pShell, AEECLSID_APP_CAMERA, EVT_USER_REDRAW, NULL, NULL);
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
		{
			AECHAR wfileName[MIN_PIC_WCHAR_NAME_LEN];
			AEERect bottom_rc;
            #ifdef FEATURE_VERSION_W317A	
            AECHAR pcText[MIN_PIC_WCHAR_NAME_LEN];
			IImage *StringBgMsgImg; 
            AEEImageInfo StringBgImgInfo = {0};
            #endif
            
			SETAEERECT(&bottom_rc, 0, pMe->m_rc.dy - BOTTOMBAR_HEIGHT, pMe->m_rc.dx, BOTTOMBAR_HEIGHT);
			
			IDISPLAY_EraseRect(pMe->m_pDisplay, &bottom_rc);
    
    		STRTOWSTR(pMe->m_sCaptureFileName, wfileName, MIN_PIC_WCHAR_NAME_LEN);	
           
			if (WSTRLEN(wfileName)>0)
			{      
#if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
            pMe->m_rc.y += 20;    
            DrawTextWithProfile(pMe->m_pShell, 
			                        pMe->m_pDisplay, 
			                        RGB_BLACK, 
			                        AEE_FONT_NORMAL, 
			                        wfileName, 
			                        -1, 
			                        0, 
			                        0, 
			                        &pMe->m_rc, 
			                        IDF_ALIGN_TOP|IDF_ALIGN_LEFT|IDF_TEXT_TRANSPARENT);
            pMe->m_rc.y -= 20; 
#else
            DrawTextWithProfile(pMe->m_pShell, 
			                        pMe->m_pDisplay, 
			                        RGB_BLACK, 
			                        AEE_FONT_NORMAL, 
			                        wfileName, 
			                        -1, 
			                        0, 
			                        0, 
			                        &pMe->m_rc, 
			                        IDF_ALIGN_TOP|IDF_ALIGN_RIGHT|IDF_TEXT_TRANSPARENT);
#endif
			    
			}
            #ifdef FEATURE_VERSION_W317A
            StringBgMsgImg = ISHELL_LoadResImage(pMe->m_pShell,
                            AEE_APPSCOMMONRES_IMAGESFILE,
                            IDB_PROMPT_MSG_BG);
            if(StringBgMsgImg != NULL)
            {
                IIMAGE_GetInfo(StringBgMsgImg, &StringBgImgInfo);
                IIMAGE_Draw(StringBgMsgImg, (pMe->m_rc.dx- StringBgImgInfo.cx)/2, (pMe->m_rc.dy - StringBgImgInfo.cy)/2);
                IIMAGE_Release(StringBgMsgImg);
                StringBgMsgImg = NULL; 
            }
            (void)ISHELL_LoadResString(pMe->m_pShell, 
                               AEE_APPSCAMERAAPP_RES_FILE,       
                               IDS_CAMERA_SAVE_VIDEOFILE, 
                               pcText, 
                               sizeof(pcText));
            DrawTextWithProfile(pMe->m_pShell, 
			                        pMe->m_pDisplay, 
			                        RGB_BLACK, 
			                        AEE_FONT_NORMAL, 
			                        pcText, 
			                        -1, 
			                        (pMe->m_rc.dx- StringBgImgInfo.cx)/2+5, 
			                        (pMe->m_rc.dy - StringBgImgInfo.cy)/2+40, 
			                        &pMe->m_rc, 
			                        IDF_ALIGN_CENTER|IDF_TEXT_TRANSPARENT);
            #endif
            //CameraApp_DrawBottomBarText(pMe, BTBAR_SAVE_DELETE);
            {
				BottomBar_Param_type BarParam;					
				MEMSET(&BarParam, 0, sizeof(BarParam)); 
				#ifdef FEATURE_VERSION_W317A
				BarParam.eBBarType = BTBAR_YES_NO;
				#else
				BarParam.eBBarType = BTBAR_SAVE_DELETE; 
				#endif
				
				DrawBottomBar(pMe->m_pDisplay, &BarParam);					
			}            

            //Add By zzg 2013_03_25
            pMe->m_bIsSnaping = FALSE;
            pMe->m_bCanPress = TRUE;
            //Add End            
    
			IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            return TRUE;
        }
            
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
                    MSG_FATAL("CameraApp_VideoHandleEvent AVK_INFO",0,0,0);
#ifdef FEATURE_USES_MMS                    
                    if(pMe->m_isFormMMS)
                    {
                       DBGPRINTF("CameraApp_VideoHandleEvent CurrentFileName=%s",pMe->m_sCurrentFileName); 
                       ICONFIG_SetItem(pMe->m_pConfig, CFGI_MMSIMAGE,
                                       pMe->m_sCurrentFileName, sizeof(pMe->m_sCurrentFileName));   
                        pMe->m_isFormMMS = FALSE;
                        ISHELL_CancelTimer(pMe->m_pShell, NULL, pMe);
                        if(pMe->m_nCameraState == CAM_PREVIEW)
                        {
                            ICAMERA_Stop(pMe->m_pCamera);
                            pMe->m_nCameraState = CAM_STOP;
                        }
                        ISHELL_CloseApplet(pMe->m_pShell, FALSE);
                        MSG_FATAL("CameraApp_PopMSGHandleEvent END",0,0,0); 
                        return TRUE;
                    }
#endif                    
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

#ifdef FEATURE_LCD_TOUCH_ENABLE//andrew add for LCD touch
		case EVT_PEN_UP:
			{
				AEEDeviceInfo devinfo;
				int nBarH ;
				AEERect rc;
				int16 wXPos = (int16)AEE_GET_X(dwParam);
				int16 wYPos = (int16)AEE_GET_Y(dwParam);
                MSG_FATAL("CameraApp_VideoHandleEvent EVT_PEN_UP, wXPos=%d, wYPos=%d",wXPos, wYPos, 0);
				nBarH = GetBottomBarHeight(pMe->m_pDisplay);
        
				MEMSET(&devinfo, 0, sizeof(devinfo));
				ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
				SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, nBarH);

				if(TOUCH_PT_IN_RECT(wXPos,wYPos,rc))
				{
                    MSG_FATAL("CameraApp_CameraCFGHandleEvent TOUCH_PT_IN_RECT",0, 0, 0);
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						return CameraApp_PicHandleEvent(pMe, EVT_KEY, AVK_SELECT, 0);
					}
                    if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//右
					{						
						 return CameraApp_PicHandleEvent(pMe, EVT_KEY, AVK_CLR, 0);
					}
				}

			}
			break;
#endif              
    }
    return FALSE;

}




/*==============================================================================
函数：
       CameraApp_PopMSGHandleEvent
说明：
       IDD_POPMSGC对话框事件处理函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean  CameraApp_PopMSGHandleEvent(CCameraApp *pMe,
                                            AEEEvent   eCode,
                                            uint16     wParam,
                                            uint32     dwParam)
{
    static IStatic *pStatic = NULL;     
	AECHAR WTitle[40] = {0};
    
    switch (eCode)
    {
        case EVT_DIALOG_INIT: 
			if ((pMe->m_isRecordMode == TRUE)
#ifdef FEATURE_USES_MMS  
                || (pMe->m_isFormMMS)
#endif                
                )
			{
				pMe->m_bIsPreview = FALSE;
	            pMe->m_nCameraState = CAM_START;
                //Add By zzg 2013_03_28
#if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
                if (pMe->m_wMsgID  != IDS_MSG_NOSDCARD)             
                {
                    pMe->m_wMsgID = IDS_MSG_WAITING; 
                }  
#else
                pMe->m_wMsgID = IDS_MSG_WAITING; 
#endif
                //Add End
                
	            pMe->m_nMsgTimeout = TIMEOUT_MS_MSGBOX;
	            
				(void)ISHELL_LoadResString(pMe->m_pShell,
	                                AEE_APPSCAMERAAPP_RES_FILE,                                
	                                IDS_ITEM_CAMERA,
	                                WTitle,
	                                sizeof(WTitle));
				if(pMe->m_pIAnn != NULL)
	        	{
			    	IANNUNCIATOR_SetFieldTextEx(pMe->m_pIAnn,WTitle,FALSE);
				}
	                
	            IDISPLAY_SetClipRect(pMe->m_pDisplay, NULL); 

	            IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,TRUE);            
			
			}
            
            pMe->m_bIsSnaping = FALSE;  //Add By zzg 2013_03_22
           
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
			if ((pMe->m_isRecordMode == TRUE)
#ifdef FEATURE_USES_MMS  
                || (pMe->m_isFormMMS)
#endif
                )
			{
				if(pMe->m_pCamera)
	            {
	                ICAMERA_Release(pMe->m_pCamera);
	                pMe->m_pCamera = NULL;
	            }   

				// set the annunciator disable
				IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,FALSE);

				pMe->m_bMemoryCardExist = CameraApp_FindMemoryCardExist(pMe);
#ifdef FEATURE_VERSION_FLEXI203P

				// 如果T卡不存在，defualt保存在手机里
				if(!pMe->m_bMemoryCardExist)
				{
				    pMe->m_nCameraStorage = OEMNV_CAMERA_STORAGE_PHONE;
				    
				    pMe->m_nCameraSize = OEMNV_CAMERA_SIZE_DEFAULT;

				    (void)ICONFIG_SetItem(pMe->m_pConfig,
				                          CFGI_CAMERA_STORAGE,
				                          &pMe->m_nCameraStorage,
				                          sizeof(pMe->m_nCameraStorage));  

				    (void)ICONFIG_SetItem(pMe->m_pConfig,
				                          CFGI_CAMERA_SIZE,
				                          &pMe->m_nCameraSize,
				                          sizeof(pMe->m_nCameraSize)); 
				}
				else
				{
				    pMe->m_nCameraStorage = OEMNV_CAMERA_STORAGE_MEMORY_CARD;
				}
#else
				if ( pMe->m_isStartFromFacebook == TRUE )
				{
					pMe->m_nCameraStorage = OEMNV_CAMERA_STORAGE_PHONE;
				}
				else
				{
					pMe->m_nCameraStorage = OEMNV_CAMERA_STORAGE_MEMORY_CARD;
				}
#endif
			}
			
            if((pMe->m_nMsgTimeout != 0) && (pMe->m_wMsgID != IDS_MSG_WAITING))           
            {
                (void)ISHELL_SetTimer(pMe->m_pShell,
                                      pMe->m_nMsgTimeout,
                                      CameraApp_DialogTimeout,
                                      pMe);
            }
            
            // For redraw the dialog
            (void)ISHELL_PostEvent(pMe->m_pShell,
                                   AEECLSID_APP_CAMERA,
                                   EVT_USER_REDRAW,
                                   0,
                                   0);  
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
            return TRUE;                                                                       
            
        case EVT_USER_REDRAW:
            //if(pMe->m_ePreState == STATE_CPIC)
            if ((pMe->m_ePreState == STATE_CPIC) || (pMe->m_ePreState == STATE_CVIDEO)) 
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
                                               AEE_APPSCAMERAAPP_RES_FILE,
                                               pMe->m_wMsgID,
                                               wstrText,
                                               sizeof(wstrText));
                }
               
                switch(pMe->m_wMsgID)
                {
                  #ifdef FEATURE_VERSION_FLEXI203P
					case IDS_MSG_PHONE_MEMERY_FULL_AND_NOSDCARD:
                  #endif
                    case IDS_MSG_NOSDCARD:
                    case IDS_MSG_NOCAMERA:
                    case IDS_MSG_NOMEMORY:
#if defined(FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)   
		            case IDS_NOMEMORY:	
#endif
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
#ifdef FEATURE_DSP
            if(pMe->m_wMsgID == IDS_MSG_WAITING || pMe->m_wMsgID == IDS_DONE
               || pMe->m_wMsgID == IDS_MSG_NOSDCARD || pMe->m_wMsgID == IDS_MSG_NOMEMORY
#if defined(FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)      
		       || pMe->m_wMsgID == IDS_NOMEMORY
#endif
               ) // &&(!pMe->m_pCamera)
            {
               CameraApp_InitCameraCheck(pMe);    

                (void)ISHELL_PostEvent(pMe->m_pShell,
                                       AEECLSID_APP_CAMERA,
                                       EVT_APP_DIALOG_TIMEOUT,
                                       0,
                                       0);
            }
#else                 
            
             //Add By zzg 2013_03_28
#if defined (FEATURE_VERSION_C337) || defined(FEATURE_VERSION_IC241A_MMX)
            if(((pMe->m_wMsgID == IDS_MSG_WAITING) || pMe->m_wMsgID == IDS_MSG_NOSDCARD) &&(!pMe->m_pCamera))
#else
            if((pMe->m_wMsgID == IDS_MSG_WAITING) &&(!pMe->m_pCamera))
#endif      
            //Add End
            {
                CALLBACK_Init(&pMe->m_CallBack,
                              CameraApp_InitCameraCheck,
                              (void *)pMe);          
            
                ISHELL_Resume(pMe->m_pShell, &pMe->m_CallBack);

                (void)ISHELL_PostEvent(pMe->m_pShell,
                                       AEECLSID_APP_CAMERA,
                                       EVT_APP_DIALOG_TIMEOUT,
                                       0,
                                       0);
            }
#endif
            
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
                                     CameraApp_DialogTimeout,
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
                                                CameraApp_DialogTimeout,
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
            	if(pMe->m_pCamera)
            	{
                	CLOSE_DIALOG(DLGRET_PREVIEW);
            	}
				else
				{
					CLOSE_DIALOG(DLGRET_CANCELED);	
				}
            }
            else if(pMe->m_wMsgID == IDS_MSG_NOMEMORY)
            {
                (void)ISHELL_CloseApplet(pMe->m_pShell, TRUE);
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
       CameraApp_RoutePopMenuCommandEvent
说明：
       分发设置菜单命令事件处理路由函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean CameraApp_RoutePopMenuCommandEvent(CCameraApp *pMe, uint16 wParam)
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
        switch(pMe->m_nCameraCFG)
        {
        	#ifdef FEATURE_CAMERA_MULTI_SENSOR
			case CAMERACFGMULTI:
				return CameraApp_PopMenu_MultiCommandHandleEvent(pMe, wParam);
                break;
			case CAMERACFGFLASH:
				return CameraApp_PopMenu_FlashCommandHandleEvent(pMe, wParam);
				break;
		    #endif
            case CAMERACFGENVIRMENT:
                return CameraApp_PopMenu_EnvironmentCommandHandleEvent(pMe, wParam);

            case CAMERACFGQUALITY:
                return CameraApp_PopMenu_QualityCommandHandleEvent(pMe, wParam);

            #ifndef FEATURE_VERSION_K212
			#ifndef FEATURE_LOW_MEM_BIGFONT
            case CAMERACFGSIZE:
                return CameraApp_PopMenu_SizeCommandHandleEvent(pMe, wParam);
            #endif
			#endif
            case CAMERACFGTONE:
                return CameraApp_PopMenu_ShutterToneCommandHandleEvent(pMe, wParam);
   
            case CAMERACFGSELFTIME:
                return CameraApp_PopMenu_SelfTimeCommandHandleEvent(pMe, wParam);
   #if 0
            case CAMERACFGBANDING:
                return CameraApp_PopMenu_BandingCommandHandleEvent(pMe, wParam);
                #endif
                
            case CAMERACFGRESET:
                return CameraApp_PopMenu_ResetCFGCommandHandleEvent(pMe, wParam);
   
            default:
                return FALSE;
        }
    }
    return TRUE;
}
#ifdef FEATURE_CAMERA_MULTI_SENSOR  //add by adrew
/*==============================================================================
函数：
       CameraApp_PopMenu_MultiCommandHandleEvent
说明：
       环境设置菜单命令处理函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

       
备注：
       
==============================================================================*/

static boolean CameraApp_PopMenu_MultiCommandHandleEvent(CCameraApp *pMe, 
                                                               uint16 wParam)
{
	
	 switch(wParam)
            {
                case IDS_MULTI_PRE:
					MSG_FATAL("CameraApp_PopMenu_MultiCommandHandleEvent OEMNV_CAMERA_MULTI_ONE",0,0,0);
                    pMe->m_nCameraMulti = OEMNV_CAMERA_MULTI_ONE;
                    //dwFPS = 0;
                    break;

                case IDS_MULTI_BACK:
					MSG_FATAL("CameraApp_PopMenu_MultiCommandHandleEvent OEMNV_CAMERA_MULTI_TWO",0,0,0);
                    pMe->m_nCameraMulti = OEMNV_CAMERA_MULTI_TWO;
                    //dwFPS = 3;	
                    break;
				default:
					break;
			 }
	 (void)ICONFIG_SetItem(pMe->m_pConfig,
                                  CFGI_CAMERA_MULTI,
                                  &pMe->m_nCameraMulti,
                                  sizeof(pMe->m_nCameraMulti));
	 MSG_FATAL("CameraApp_PopMenu_MultiCommandHandleEvent.................0000",0,0,0);
	 if(SUCCESS == ICAMERA_Stop(pMe->m_pCamera))
    {
    	MSG_FATAL("CameraApp_PopMenu_MultiCommandHandleEvent.................11111",0,0,0);
        pMe->m_bIsPreview = FALSE;
        pMe->m_nCameraState = CAM_STOP;
		if(pMe->m_pCamera != NULL)
		{
			ICAMERA_Release(pMe->m_pCamera);
            pMe->m_pCamera = NULL;
		}
		CameraApp_InitCameraCheck(pMe);
        CLOSE_DIALOG(DLGRET_CANCELED);
    }
    
    return TRUE;
}

#ifdef FEATURE_CAMERA_MULTI_SENSOR
static boolean CameraApp_PopMenu_FlashCommandHandleEvent(CCameraApp *pMe,  uint16 wParam)
{
	 switch(wParam)
            {
                case IDS_TONE_ENABLE:
					MSG_FATAL("CameraApp_PopMenu_MultiCommandHandleEvent OEMNV_CAMERA_FLASH_ON",0,0,0);
                    pMe->n_nCameraFlash = OEMNV_CAMERA_FLASH_ON;
                    //dwFPS = 0;
                    break;

                case IDS_TONE_DISABLE:
					MSG_FATAL("CameraApp_PopMenu_MultiCommandHandleEvent OEMNV_CAMERA_FLASH_OFF",0,0,0);
                    pMe->n_nCameraFlash = OEMNV_CAMERA_FLASH_OFF;
                    //dwFPS = 3;	
                    break;
				default:
					break;
			 }
	 (void)ICONFIG_SetItem(pMe->m_pConfig,
                                  CFGT_CAMERA_FLASH,
                                  &pMe->n_nCameraFlash,
                                  sizeof(pMe->n_nCameraFlash));
     CLOSE_DIALOG(DLGRET_CANCELED); 
    return TRUE;
}
#endif

#endif

/*==============================================================================
函数：
       CameraApp_PopMenu_EnvironmentCommandHandleEvent
说明：
       环境设置菜单命令处理函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

       
备注：
       
==============================================================================*/
static boolean CameraApp_PopMenu_EnvironmentCommandHandleEvent(CCameraApp *pMe, uint16 wParam)
{ 
    uint32 dwFPS = 5;
        
    switch(pMe->m_pActiveDlgID)
    {
        case IDD_CCAMERACFG:
            switch(wParam)
            {
                case IDS_ENVIR_MODE_AUTO:
                    pMe->m_nCameraEnviroment = OEMNV_CAMERA_ENVIR_AUTO;
                    dwFPS = CAM_WB_AUTO;	//0;
                    break;

                case IDS_ENVIR_MODE_NIGHT:
                    pMe->m_nCameraEnviroment = OEMNV_CAMERA_ENVIR_NIGHT;
                    dwFPS = CAM_WB_FLUORESCENT; //3;
                    break;

                case IDS_ENVIR_MODE_SUNNY:
                    pMe->m_nCameraEnviroment = OEMNV_CAMERA_ENVIR_SUNNY;
                    dwFPS = CAM_WB_DAYLIGHT;	//1;
                    break;

                case IDS_ENVIR_MODE_CLOUDY:
                    pMe->m_nCameraEnviroment = OEMNV_CAMERA_ENVIR_CLOUDY;
                    dwFPS = CAM_WB_CLOUDY_DAYLIGHT; //2;
                    break;

                default:
                    break;                 
            }

            (void)ICONFIG_SetItem(pMe->m_pConfig,
                                  CFGI_CAMERA_ENVIROMENT,
                                  &pMe->m_nCameraEnviroment,
                                  sizeof(pMe->m_nCameraEnviroment));
            
            ICAMERA_SetWb(pMe->m_pCamera, dwFPS);
        
            break;

        case IDD_CVIDEOCFG:
            switch(wParam)
            {
                case IDS_ENVIR_MODE_AUTO:
                    pMe->m_nCameraEnviroment = OEMNV_CAMERA_ENVIR_AUTO;
                    dwFPS = CAM_WB_AUTO;	//0;
                    break;

                case IDS_ENVIR_MODE_NIGHT:
                    pMe->m_nCameraEnviroment = OEMNV_CAMERA_ENVIR_NIGHT;
                    dwFPS = CAM_WB_FLUORESCENT;	//3;
                    break;

                case IDS_ENVIR_MODE_SUNNY:
                    pMe->m_nCameraEnviroment = OEMNV_CAMERA_ENVIR_SUNNY;
                    dwFPS = CAM_WB_DAYLIGHT;	//1;
                    break;

                case IDS_ENVIR_MODE_CLOUDY:
                    pMe->m_nCameraEnviroment = OEMNV_CAMERA_ENVIR_CLOUDY;
                    dwFPS = CAM_WB_CLOUDY_DAYLIGHT;	//2;
                    break;

                default:
                    break;                 
            }

            (void)ICONFIG_SetItem(pMe->m_pConfig,
                                  CFGI_VIDEO_ENVIROMENT,
                                  &pMe->m_nCameraEnviroment,
                                  sizeof(pMe->m_nCameraEnviroment));
            
            ICAMERA_SetWb(pMe->m_pCamera, dwFPS);
            break;
            
    }
    
    CLOSE_DIALOG(DLGRET_CANCELED); 
    return TRUE;
}

/*==============================================================================
函数：
       CameraApp_PopMenu_QualityCommandHandleEvent
说明：
       拍照质量设置菜单命令处理函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
      
备注：
       
==============================================================================*/
static boolean CameraApp_PopMenu_QualityCommandHandleEvent(CCameraApp *pMe, uint16 wParam)
{
    int16 quality = 0;
    
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_QUALITY,
                          &pMe->m_nCameraQuality,
                          sizeof(pMe->m_nCameraQuality));

    if(wParam != pMe->m_nCameraQuality)
    {
        switch(wParam)
        {
            case IDS_QUALITY_HIGH:
                pMe->m_nCameraQuality = OEMNV_CAMERA_QUALITY_HIGH; 
                quality = CAMERA_APP_QUALITY_HIGH;
                break;

            case IDS_QUALITY_MED:
                pMe->m_nCameraQuality = OEMNV_CAMERA_QUALITY_MED;
                quality = CAMERA_APP_QUALITY_MED;
                break;

            case IDS_QUALITY_LOW:
                pMe->m_nCameraQuality = OEMNV_CAMERA_QUALITY_LOW;
                quality = CAMERA_APP_QUALITY_LOW;
                break;

            default:
                return FALSE;
        }
        
        (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_CAMERA_QUALITY,
                              &pMe->m_nCameraQuality,
                              sizeof(pMe->m_nCameraQuality));

        ICAMERA_SetQuality(pMe->m_pCamera, quality);

        pMe->m_bRePreview = TRUE;
        ICAMERA_Stop(pMe->m_pCamera);
        pMe->m_nCameraState = CAM_STOP;
    }
    
    CLOSE_DIALOG(DLGRET_CANCELED);
    return TRUE;
}

/*==============================================================================
函数：
       CameraApp_PopMenu_SizeCommandHandleEvent
说明：
       拍照大小设置菜单命令处理函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/
static boolean CameraApp_PopMenu_SizeCommandHandleEvent(CCameraApp *pMe, uint16 wParam)
{  
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_SIZE,
                          &pMe->m_nCameraSize,
                          sizeof(pMe->m_nCameraSize));

    if(wParam != pMe->m_nCameraSize)
    { 
        CameraApp_SetCameraCaptureSize(pMe, wParam);
        pMe->m_bRePreview = TRUE;
        ICAMERA_Stop(pMe->m_pCamera);
        pMe->m_nCameraState = CAM_STOP;
    }
#if defined(FEATURE_DISP_128X128)
    if(pMe->m_nCameraSize == OEMNV_CAMERA_SIZE_INDEX_0)
    {
        ICAMERA_SetParm(pMe->m_pCamera, CAM_PARM_PREVIEWWITHFRAME, 1, 0);
    }
    else
#endif
    {
        ICAMERA_SetParm(pMe->m_pCamera, CAM_PARM_PREVIEWWITHFRAME, 0, 0);
    }
  
    CLOSE_DIALOG(DLGRET_CANCELED);
    return TRUE;
}

/*==============================================================================
函数：
       CameraApp_PopMenu_ShutterToneCommandHandleEvent
说明：
       拍照声音设置菜单命令处理函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam：事件相关数据。      
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/                                                                                                                    
static boolean CameraApp_PopMenu_ShutterToneCommandHandleEvent(CCameraApp *pMe, uint16 wParam)
{   
    switch(wParam)
    {
#ifdef FEATURE_CARRIER_CHINA_TELCOM
        case IDS_TONE_SHUTTER2:
            pMe->m_nCameraTone = OEMNV_CAMERA_SHUTTER_TONE_SHUTTER2; 
            break;

        case IDS_TONE_SHUTTER1:
            pMe->m_nCameraTone = OEMNV_CAMERA_SHUTTER_TONE_SHUTTER1;
            break;
#else    
        case IDS_TONE_DISABLE:
            pMe->m_nCameraTone = OEMNV_CAMERA_SHUTTER_TONE_DISABLE; 
            break;

        case IDS_TONE_ENABLE:
            pMe->m_nCameraTone = OEMNV_CAMERA_SHUTTER_TONE_ENABLE;
            break;
#endif			

        default:
            return FALSE;
    }
    
    (void)ICONFIG_SetItem(pMe->m_pConfig,
                          CFGI_CAMERA_TONE,
                          &pMe->m_nCameraTone,
                          sizeof(pMe->m_nCameraTone));
        
    CLOSE_DIALOG(DLGRET_CANCELED);
    return TRUE;
}

/*==============================================================================
函数：
       CameraApp_PopMenu_SelfTimeCommandHandleEvent
说明：
       拍照时间设置菜单命令处理函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
       
备注：
       
==============================================================================*/                                                                                                                       
static boolean CameraApp_PopMenu_SelfTimeCommandHandleEvent(CCameraApp *pMe, uint16 wParam)
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
       CameraApp_PopMenu_BandingCommandHandleEvent
说明：
       Banding设置菜单命令处理函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。
      
备注：
       
==============================================================================*/                                                                                                                    
static boolean CameraApp_PopMenu_BandingCommandHandleEvent(CCameraApp *pMe, uint16 wParam)
{  
    switch(wParam)
    {
        case IDS_BANDING_MODE1:
            pMe->m_nCameraBanding = OEMNV_CAMERA_BANDING_50HZ;
            break;

        case IDS_BANDING_MODE2:
            pMe->m_nCameraBanding = OEMNV_CAMERA_BANDING_60HZ;
            break;

        default:
            return FALSE;
    }
    ICAMERA_SetParm(pMe->m_pCamera, CAM_PARM_BANDING, pMe->m_nCameraBanding, 0);
    (void)ICONFIG_SetItem(pMe->m_pConfig,
                          CFGI_CAMERA_BANDING,
                          &pMe->m_nCameraBanding,
                          sizeof(pMe->m_nCameraBanding));
    
    CLOSE_DIALOG(DLGRET_CANCELED);
    return TRUE;
}

/*==============================================================================
函数：
       CameraApp_PopMenu_ResetCFGCommandHandleEvent
说明：
       恢复默认设置菜单命令处理函数
       
参数：
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       wParam：事件相关数据。
       
返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。 
       
备注：
       
==============================================================================*/
static boolean CameraApp_PopMenu_ResetCFGCommandHandleEvent(CCameraApp *pMe, uint16 wParam)
{    
    if(wParam == IDS_CFG_RESET)
    {
        CameraApp_InitCFGData(pMe);    
    }

    if(SUCCESS == ICAMERA_Stop(pMe->m_pCamera))
    {
        pMe->m_bIsPreview = FALSE;
        pMe->m_nCameraState = CAM_STOP;
        CLOSE_DIALOG(DLGRET_CANCELED);
    }
    
    return TRUE;
}

/*==============================================================================
函数：
       CameraApp_SetCFGMenuIcon
说明：
       设置菜单项选中图标
       
参数：      
       pm [in]：Imenuctl菜单控件
       listId:  选中的菜单项
       bSel:    选中条有无的判断
             
返回值： 
       
备注：
       
==============================================================================*/
static void CameraApp_SetCFGMenuIcon(IMenuCtl *pm, uint16 listId, boolean bSel)
{
    CtlAddItem cai;
    MEMSET(&cai, 0, sizeof(cai));
    cai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
    cai.wImage = bSel ? IDB_RADIO_FILLED : IDB_RADIO_UNFILLED;

    MSG_FATAL("***zzg CameraApp_SetCFGMenuIcon listId=%d, bSel=%d***", listId, bSel, 0);

    
    (void)IMENUCTL_SetItem(pm, listId, MSIF_IMAGE, &cai);
    if (bSel)
    {
        IMENUCTL_SetSel (pm, listId);
    }
    IMENUCTL_SetStyle(pm, NULL, NULL);
}

/*==============================================================================
函数：
       CameraApp_InitpopMenu
说明：
       初始化设置菜单
       
参数：      
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
       popMenu [in]: 设置弹出菜单
             
返回值： 
       TRUE: 初始化弹出菜单完成
       
备注：
       
==============================================================================*/
static boolean CameraApp_InitpopMenu(CCameraApp *pMe, IMenuCtl *popMenu)
{  
    if(pMe->m_pActiveDlgID == IDD_CCAMERACFG)
    {
        switch(pMe->m_nCameraCFG)
        {
        	#ifdef FEATURE_CAMERA_MULTI_SENSOR
			case CAMERACFGMULTI:
 				CameraApp_PopMenu_MultiInit(pMe,popMenu);
				break;
			case CAMERACFGFLASH:
				CameraApp_PopMenu_FlashInit(pMe,popMenu);
				break;
			#endif
            case CAMERACFGENVIRMENT:
                MSG_FATAL("CAMERACFGQUALITY",0,0,0);
                CameraApp_PopMenu_EnvironmentInit(pMe, popMenu);
                break;

            case CAMERACFGQUALITY:
            	MSG_FATAL("CAMERACFGQUALITY",0,0,0);
                CameraApp_PopMenu_QualityInit(pMe, popMenu);
                break;
#ifndef FEATURE_VERSION_K212
#ifndef FEATURE_LOW_MEM_BIGFONT
            case CAMERACFGSIZE:
                MSG_FATAL("CAMERACFGSIZE",0,0,0);
                CameraApp_PopMenu_SizeInit(pMe, popMenu);
                break;
#endif
#endif
            case CAMERACFGTONE:
            	MSG_FATAL("CAMERACFGQUALITY",0,0,0);
                CameraApp_PopMenu_ShutterToneInit(pMe, popMenu);
                break;
   
            case CAMERACFGSELFTIME:
            	MSG_FATAL("CAMERACFGSELFTIME",0,0,0);
                CameraApp_PopMenu_SelfTimeInit(pMe, popMenu);
                break;
   #if 0
            case CAMERACFGBANDING:
            	MSG_FATAL("CAMERACFGBANDING",0,0,0);
                CameraApp_PopMenu_BandingInit(pMe, popMenu);
                break;
#endif
            case CAMERACFGRESET:
            	MSG_FATAL("CAMERACFGRESET",0,0,0);
                CameraApp_PopMenu_ResetCFGInit(pMe, popMenu);
                break;
                
            default:
                break; 
        }
    }
    return TRUE;
}

/*==============================================================================
函数：
       CameraApp_InitCFGData
说明：
       重置Camera cfg项为默认值
       
参数：      
       pMe [in]：指向CameraApp Applet对象结构的指针。该结构包含小程序的特定信息。
             
返回值： 
       
备注：
       
==============================================================================*/
static void CameraApp_InitCFGData(CCameraApp * pMe)
{
    if(pMe->m_pActiveDlgID == IDD_CCAMERACFG)
    {
    	cam_sensor_model_pair_type sensor_type;
    	
        pMe->m_nCameraEnviroment = OEMNV_CAMERA_ENVIR_AUTO;
        pMe->m_nCameraQuality = OEMNV_CAMERA_QUALITY_HIGH;       
#ifdef FEATURE_CARRIER_CHINA_TELCOM
        pMe->m_nCameraTone = OEMNV_CAMERA_SHUTTER_TONE_SHUTTER1;
#else			
        pMe->m_nCameraTone = OEMNV_CAMERA_SHUTTER_TONE_ENABLE;
#endif
        pMe->m_nCameraBanding = OEMNV_CAMERA_BANDING_50HZ;        

#ifdef FEATURE_VERSION_FLEXI203P

        if(CameraApp_FindMemoryCardExist(pMe))
        {
            pMe->m_nCameraStorage = OEMNV_CAMERA_STORAGE_MEMORY_CARD;
	        pMe->m_nCameraSize = OEMNV_CAMERA_SIZE_DEFAULT;
        }

		
        else
        {
            pMe->m_nCameraStorage = OEMNV_CAMERA_STORAGE_PHONE;
            pMe->m_nCameraSize = OEMNV_CAMERA_SIZE_DEFAULT;
        }
#else

		//手机内存小，所以固定存储位置是T卡
		//Add By zzg 2010_07_25
		pMe->m_nCameraStorage = OEMNV_CAMERA_STORAGE_MEMORY_CARD;
	    pMe->m_nCameraSize = OEMNV_CAMERA_SIZE_DEFAULT;
		//Add End
#endif        
        pMe->m_nSelfTimeItemSel = IDS_SELFTIME_OFF;

 		#ifdef FEATURE_CAMERA_MULTI_SENSOR  //add by andrew
		(void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_CAMERA_MULTI,
                              &pMe->m_nCameraMulti,
                              sizeof(pMe->m_nCameraMulti));
		(void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGT_CAMERA_FLASH,
                              &pMe->n_nCameraFlash,
                              sizeof(pMe->n_nCameraFlash));
		#endif
        (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_CAMERA_ENVIROMENT,
                              &pMe->m_nCameraEnviroment,
                              sizeof(pMe->m_nCameraEnviroment));

        (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_CAMERA_QUALITY,
                              &pMe->m_nCameraQuality,
                              sizeof(pMe->m_nCameraQuality));

        (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_CAMERA_SIZE,
                              &pMe->m_nCameraSize,
                              sizeof(pMe->m_nCameraSize));

        (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_CAMERA_TONE,
                              &pMe->m_nCameraTone,
                              sizeof(pMe->m_nCameraTone));

        (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_CAMERA_BANDING,
                              &pMe->m_nCameraBanding,
                              sizeof(pMe->m_nCameraBanding));

        (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_CAMERA_STORAGE,
                              &pMe->m_nCameraStorage,
                              sizeof(pMe->m_nCameraStorage)); 
		#ifdef FEATURE_CAMERA_MULTI_SENSOR  //add by andrew
		pMe->n_nCameraFlash = OEMNV_CAMERA_FLASH_OFF;
		if(pMe->m_nCameraMulti == OEMNV_CAMERA_MULTI_ONE)
		{
		    	pMe->m_nCameraMulti = OEMNV_CAMERA_MULTI_TWO;
				if(SUCCESS == ICAMERA_Stop(pMe->m_pCamera))
		    	{
			    	MSG_FATAL("CameraApp_PopMenu_MultiCommandHandleEvent.................11111",0,0,0);
			        pMe->m_bIsPreview = FALSE;
			        pMe->m_nCameraState = CAM_STOP;
					if(pMe->m_pCamera != NULL)
					{
						ICAMERA_Release(pMe->m_pCamera);
			            pMe->m_pCamera = NULL;
					}
					CameraApp_InitCameraCheck(pMe);
			        CLOSE_DIALOG(DLGRET_CANCELED);
		    	}
		}
		#endif
    }
}
#ifdef FEATURE_CAMERA_MULTI_SENSOR
// 初始化拍照环境设置菜单
static void CameraApp_PopMenu_MultiInit(CCameraApp *pMe, 
                                              IMenuCtl *popMenu)
{
	IMENUCTL_DeleteAll(popMenu);
    CameraApp_SetPopMenuRect(pMe, popMenu, 2);
	IMENUCTL_AddItem(popMenu, 
                     AEE_APPSCAMERAAPP_RES_FILE, 
                     IDS_MULTI_PRE, 
                     IDS_MULTI_PRE, 
                     NULL, 
                     0);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSCAMERAAPP_RES_FILE,
                     IDS_MULTI_BACK, 
                     IDS_MULTI_BACK, 
                     NULL,
                     0);
	InitMenuIcons(popMenu);
	 (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_MULTI,
                          &pMe->m_nCameraMulti,
                          sizeof(pMe->m_nCameraMulti));
                                           
    switch(pMe->m_nCameraMulti)
    {
        case OEMNV_CAMERA_MULTI_ONE:
            CameraApp_SetCFGMenuIcon(popMenu, IDS_MULTI_PRE, TRUE);
            break;
                
        case OEMNV_CAMERA_MULTI_TWO:
            CameraApp_SetCFGMenuIcon(popMenu, IDS_MULTI_BACK, TRUE);
            break;
   
        default:
            break;  
    }
}

#ifdef FEATURE_CAMERA_MULTI_SENSOR
static void CameraApp_PopMenu_FlashInit(CCameraApp *pMe, 
                                              IMenuCtl *popMenu)
{
	IMENUCTL_DeleteAll(popMenu);
    CameraApp_SetPopMenuRect(pMe, popMenu, 2);
	IMENUCTL_AddItem(popMenu, 
                     AEE_APPSCAMERAAPP_RES_FILE, 
                     IDS_TONE_ENABLE, 
                     IDS_TONE_ENABLE, 
                     NULL, 
                     0);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSCAMERAAPP_RES_FILE,
                     IDS_TONE_DISABLE, 
                     IDS_TONE_DISABLE, 
                     NULL,
                     0);
	InitMenuIcons(popMenu);
	 (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGT_CAMERA_FLASH,
                          &pMe->n_nCameraFlash,
                          sizeof(pMe->n_nCameraFlash));
                                           
    switch(pMe->n_nCameraFlash)
    {
        case OEMNV_CAMERA_FLASH_ON:
            CameraApp_SetCFGMenuIcon(popMenu, IDS_TONE_ENABLE, TRUE);
            break;
                
        case OEMNV_CAMERA_FLASH_OFF:
            CameraApp_SetCFGMenuIcon(popMenu, IDS_TONE_DISABLE, TRUE);
            break;
   
        default:
            break;  
    }
}
#endif

#endif

static void CameraApp_PopMenu_EnvironmentInit(CCameraApp *pMe, IMenuCtl *popMenu)
{
    IMENUCTL_DeleteAll(popMenu);
    CameraApp_SetPopMenuRect(pMe, popMenu, 4);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSCAMERAAPP_RES_FILE, 
                     IDS_ENVIR_MODE_AUTO, 
                     IDS_ENVIR_MODE_AUTO, 
                     NULL, 
                     0);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSCAMERAAPP_RES_FILE,
                     IDS_ENVIR_MODE_NIGHT, 
                     IDS_ENVIR_MODE_NIGHT, 
                     NULL,
                     0);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSCAMERAAPP_RES_FILE, 
                     IDS_ENVIR_MODE_SUNNY, 
                     IDS_ENVIR_MODE_SUNNY, 
                     NULL,
                     0);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSCAMERAAPP_RES_FILE, 
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
                                  &pMe->m_nCameraEnviroment,
                                  sizeof(pMe->m_nCameraEnviroment));
    
            switch(pMe->m_nCameraEnviroment)
            {
                case OEMNV_CAMERA_ENVIR_AUTO:
                    CameraApp_SetCFGMenuIcon(popMenu, IDS_ENVIR_MODE_AUTO, TRUE);
                    break;
    
                case OEMNV_CAMERA_ENVIR_NIGHT:
                    CameraApp_SetCFGMenuIcon(popMenu, IDS_ENVIR_MODE_NIGHT, TRUE);
                    break;
    
                case OEMNV_CAMERA_ENVIR_SUNNY:
                    CameraApp_SetCFGMenuIcon(popMenu, IDS_ENVIR_MODE_SUNNY, TRUE);
                    break;
    
                case OEMNV_CAMERA_ENVIR_CLOUDY:
                    CameraApp_SetCFGMenuIcon(popMenu, IDS_ENVIR_MODE_CLOUDY, TRUE);
                    break;

                default:
                    break;
            }
    
            break;
    
        case IDD_CVIDEOCFG:
            (void)ICONFIG_GetItem(pMe->m_pConfig,
                                  CFGI_VIDEO_ENVIROMENT,
                                  &pMe->m_nCameraEnviroment,
                                  sizeof(pMe->m_nCameraEnviroment));
    
            switch(pMe->m_nCameraEnviroment)
            {
                case OEMNV_CAMERA_ENVIR_AUTO:
                    CameraApp_SetCFGMenuIcon(popMenu, IDS_ENVIR_MODE_AUTO, TRUE);
                    break;
    
                case OEMNV_CAMERA_ENVIR_NIGHT:
                    CameraApp_SetCFGMenuIcon(popMenu, IDS_ENVIR_MODE_NIGHT, TRUE);
                    break;
    
                case OEMNV_CAMERA_ENVIR_SUNNY:
                    CameraApp_SetCFGMenuIcon(popMenu, IDS_ENVIR_MODE_SUNNY, TRUE);
                    break;
    
                case OEMNV_CAMERA_ENVIR_CLOUDY:
                    CameraApp_SetCFGMenuIcon(popMenu, IDS_ENVIR_MODE_CLOUDY, TRUE);
                    break;
    
                default:
                    break;
            }
    
            break;
                        
        default:
            break;
    }

}

static void CameraApp_PopMenu_QualityInit(CCameraApp *pMe, IMenuCtl *popMenu)
{
    IMENUCTL_DeleteAll(popMenu);
    CameraApp_SetPopMenuRect(pMe, popMenu, 3);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSCAMERAAPP_RES_FILE, 
                     IDS_QUALITY_HIGH, 
                     IDS_QUALITY_HIGH, 
                     NULL,
                     0);
    IMENUCTL_AddItem(popMenu,
                     AEE_APPSCAMERAAPP_RES_FILE, 
                     IDS_QUALITY_MED,
                     IDS_QUALITY_MED, 
                     NULL, 
                     0);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSCAMERAAPP_RES_FILE, 
                     IDS_QUALITY_LOW,
                     IDS_QUALITY_LOW, 
                     NULL, 
                     0);            
    InitMenuIcons(popMenu);
    
                
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_QUALITY,
                          &pMe->m_nCameraQuality,
                          sizeof(pMe->m_nCameraQuality));
                                           
    switch(pMe->m_nCameraQuality)
    {
        case OEMNV_CAMERA_QUALITY_HIGH:
            CameraApp_SetCFGMenuIcon(popMenu, IDS_QUALITY_HIGH, TRUE);
            break;
                
        case OEMNV_CAMERA_QUALITY_MED:
            CameraApp_SetCFGMenuIcon(popMenu, IDS_QUALITY_MED, TRUE);
            break;
                
        case OEMNV_CAMERA_QUALITY_LOW:
            CameraApp_SetCFGMenuIcon(popMenu, IDS_QUALITY_LOW, TRUE);
            break;
                
        default:
            break;  
    }
}
           
static void CameraApp_PopMenu_SizeInit(CCameraApp *pMe, IMenuCtl *popMenu)
{
    IMENUCTL_DeleteAll(popMenu);
    if(pMe->m_nCameraStorage == OEMNV_CAMERA_STORAGE_MEMORY_CARD)
    {
        int i=0;

        while(1)
        {
        #ifdef FEATURE_CAMERA_MULTI_SENSOR
			

			MSG_FATAL("CameraApp_PopMenu_SizeInit m_sensor_model=%d",pMe->m_sensor_model,0,0);
			if(pMe->m_nCameraMulti == OEMNV_CAMERA_MULTI_ONE)
            {
                MSG_FATAL("add SizeItem",0,0,0);
                if(g_CameraFrontSizeCFG[i].dx == 0)
                {
                    break;
                }
                IMENUCTL_AddItem(popMenu, 
                                 NULL, 
                                 0,
                                 i, 
                                 g_CameraFrontSizeCFG[i].pStr, 
                                 NULL);
                i++;
            }
            else if(pMe->m_nCameraMulti == OEMNV_CAMERA_MULTI_TWO)
            {
				MSG_FATAL("add SizeItem",0,0,0);
                if(g_CameraBackSizeCFG[i].dx == 0)
                {
                    break;
                }
                IMENUCTL_AddItem(popMenu, 
                                 NULL, 
                                 0,
                                 i, 
                                 g_CameraBackSizeCFG[i].pStr, 
                                 NULL);
                i++;
            }
		#else
        	MSG_FATAL("CameraApp_PopMenu_SizeInit m_sensor_model=%d",pMe->m_sensor_model,0,0);
            if(pMe->m_sensor_model == 30)
            {
                MSG_FATAL("add SizeItem",0,0,0);
                if(g_CameraSizeCFG[i].dx == 0)
                {
                    break;
                }
                IMENUCTL_AddItem(popMenu, 
                                 NULL, 
                                 0,
                                 i, 
                                 g_CameraSizeCFG[i].pStr, 
                                 NULL);
                i++;
            }
            else if(pMe->m_sensor_model == 10)
            {
                 MSG_FATAL("add SizeItem",0,0,0);
				 
#ifdef FEATURE_MOVIE_RECORD_SUPPORT
				 if ( pMe->m_isRecordMode )
				 {
					if (g_VideoSizeCFG_10[i].dx == 0)
					{
						break;
					}
					
					IMENUCTL_AddItem(popMenu, 
					     NULL, 
					     0,
					     i, 
					     g_VideoSizeCFG_10[i].pStr, 
					     NULL);
					
					i++;            
					 
				 }
				 else
				 {
					if (g_CameraSizeCFG_10[i].dx == 0)
					{
						break;
					}
					
					IMENUCTL_AddItem(popMenu, 
					             NULL, 
					             0,
					             i, 
					             g_CameraSizeCFG_10[i].pStr, 
					             NULL);
					i++;   
				 }
#else
				if (g_CameraSizeCFG_10[i].dx == 0)
				{
					break;
				}
				
				IMENUCTL_AddItem(popMenu, 
				             NULL, 
				             0,
				             i, 
				             g_CameraSizeCFG_10[i].pStr, 
				             NULL);
				i++;   
#endif		 
                         
            }
            else
            {
                 MSG_FATAL("add SizeItem",0,0,0);

#ifdef FEATURE_MOVIE_RECORD_SUPPORT
				 if ( pMe->m_isRecordMode )
				 {
					if (g_VideoSizeCFG_10[i].dx == 0)
					{
						break;
					}
					
					IMENUCTL_AddItem(popMenu, 
						 NULL, 
						 0,
						 i, 
						 g_VideoSizeCFG_10[i].pStr, 
						 NULL);
					
					i++;			
					 
				 }
				 else
				 {
					if (g_CameraSizeCFG_10[i].dx == 0)
					{
						break;
					}
					IMENUCTL_AddItem(popMenu, 
								 NULL, 
								 0,
								 i, 
								 g_CameraSizeCFG_10[i].pStr, 
								 NULL);
					i++;   
				 }
#else
				if (g_CameraSizeCFG_10[i].dx == 0)
				{
					break;
				}
				
				IMENUCTL_AddItem(popMenu, 
							 NULL, 
							 0,
							 i, 
							 g_CameraSizeCFG_10[i].pStr, 
							 NULL);
				i++;   
#endif


				 
            	
            }
        #endif
        }
         MSG_FATAL("CameraApp_SetPopMenuRect",0,0,0);
		#if defined (FEATURE_VERSION_K212)|| defined (FEATURE_QVGA_INHERIT_K212)||defined(FEATURE_LOW_MEM_BIGFONT)
        #if defined (FEATURE_VERSION_IC241A_MMX)
        CameraApp_SetPopMenuRect(pMe, popMenu, i);
        #else
		CameraApp_SetPopMenuRect(pMe, popMenu, i+1);
        #endif
		#else
        CameraApp_SetPopMenuRect(pMe, popMenu, i);
		#endif
    }
    else
    {
    #ifdef FEATURE_CAMERA_MULTI_SENSOR
		MSG_FATAL("CameraApp_PopMenu_SizeInit m_sensor_model=%d",pMe->m_sensor_model,0,0);
		if(pMe->m_nCameraMulti == OEMNV_CAMERA_MULTI_ONE)
        { 
            IMENUCTL_AddItem(popMenu, 
                         NULL, 
                         0,
                         0, 
                         g_CameraFrontSizeCFG[OEMNV_CAMERA_SIZE_DEFAULT].pStr, 
                         NULL);
        }
        else if(pMe->m_nCameraMulti == OEMNV_CAMERA_MULTI_TWO)
        { 
            IMENUCTL_AddItem(popMenu, 
                         NULL, 
                         0,
                         0, 
                         g_CameraBackSizeCFG[OEMNV_CAMERA_SIZE_DEFAULT].pStr, 
                         NULL);
        }
    #else
        if(pMe->m_sensor_model == 30)
        {
            IMENUCTL_AddItem(popMenu, 
                             NULL, 
                             0,
                             0, 
                             g_CameraSizeCFG[OEMNV_CAMERA_SIZE_DEFAULT].pStr, 
                             NULL);
        }
        else if(pMe->m_sensor_model == 10)
        {
#ifdef FEATURE_MOVIE_RECORD_SUPPORT
			if (pMe->m_isRecordMode)
			{
				IMENUCTL_AddItem(popMenu, 
                             NULL, 
                             0,
                             0, 
                             g_VideoSizeCFG_10[OEMNV_CAMERA_SIZE_DEFAULT].pStr, 
                             NULL);  
			}
			else
			{
				IMENUCTL_AddItem(popMenu, 
                             NULL, 
                             0,
                             0, 
                             g_CameraSizeCFG_10[OEMNV_CAMERA_SIZE_DEFAULT].pStr, 
                             NULL);  
			}				
			 	
#else
			IMENUCTL_AddItem(popMenu, 
                             NULL, 
                             0,
                             0, 
                             g_CameraSizeCFG_10[OEMNV_CAMERA_SIZE_DEFAULT].pStr, 
                             NULL);      
#endif
              
        }
        else
        {
#ifdef FEATURE_MOVIE_RECORD_SUPPORT
			if (pMe->m_isRecordMode)
			{
				IMENUCTL_AddItem(popMenu, 
                             NULL, 
                             0,
                             0, 
                             g_VideoSizeCFG_10[OEMNV_CAMERA_SIZE_DEFAULT].pStr, 
                             NULL);  
			}
			else
			{
				IMENUCTL_AddItem(popMenu, 
                             NULL, 
                             0,
                             0, 
                             g_CameraSizeCFG_10[OEMNV_CAMERA_SIZE_DEFAULT].pStr, 
                             NULL);  
			}				
			 	
#else
			IMENUCTL_AddItem(popMenu, 
                             NULL, 
                             0,
                             0, 
                             g_CameraSizeCFG_10[OEMNV_CAMERA_SIZE_DEFAULT].pStr, 
                             NULL);      
#endif
        }
        #endif
        CameraApp_SetPopMenuRect(pMe, popMenu, 1);
    }
    
    InitMenuIcons(popMenu);

    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_SIZE,
                          &pMe->m_nCameraSize,
                          sizeof(pMe->m_nCameraSize));

    MSG_FATAL("***zzg CameraApp PopMenu SizeInit m_nCameraSize=%d***", pMe->m_nCameraSize, 0, 0);

    //Add By zzg 2013_03_27
    if(pMe->m_sensor_model == 10)
    {
#ifdef FEATURE_MOVIE_RECORD_SUPPORT
        if ( pMe->m_isRecordMode )
        {
            if(pMe->m_nCameraSize >= (sizeof(g_VideoSizeCFG_10)/sizeof(CCameraSize)-1))
                pMe->m_nCameraSize=OEMNV_CAMERA_SIZE_INDEX_0;
        }
#endif        
    }
    //Add End

    CameraApp_SetCFGMenuIcon(popMenu, pMe->m_nCameraSize, TRUE);
}
         
static void CameraApp_PopMenu_ShutterToneInit(CCameraApp *pMe, IMenuCtl *popMenu)  
{
    IMENUCTL_DeleteAll(popMenu);   
    #if defined (FEATURE_VERSION_K212)||defined(FEATURE_DISP_240X320)||defined(FEATURE_LOW_MEM_BIGFONT)
    #if defined (FEATURE_VERSION_IC241A_MMX)
    CameraApp_SetPopMenuRect(pMe, popMenu, 2);
    #else
	CameraApp_SetPopMenuRect(pMe, popMenu, 3);
    #endif
	#else
    CameraApp_SetPopMenuRect(pMe, popMenu, 2);
	#endif
#ifdef FEATURE_CARRIER_CHINA_TELCOM
    IMENUCTL_AddItem(popMenu,
                     AEE_APPSCAMERAAPP_RES_FILE, 
                     IDS_TONE_SHUTTER1, 
                     IDS_TONE_SHUTTER1,
                     NULL,
                     0);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSCAMERAAPP_RES_FILE,
                     IDS_TONE_SHUTTER2, 
                     IDS_TONE_SHUTTER2, 
                     NULL, 
                     0);
#else		
    IMENUCTL_AddItem(popMenu,
                     AEE_APPSCAMERAAPP_RES_FILE, 
                     IDS_TONE_DISABLE, 
                     IDS_TONE_DISABLE,
                     NULL,
                     0);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSCAMERAAPP_RES_FILE,
                     IDS_TONE_ENABLE, 
                     IDS_TONE_ENABLE, 
                     NULL, 
                     0);            
#endif	
    InitMenuIcons(popMenu);
    
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_TONE,
                          &pMe->m_nCameraTone,
                          sizeof(pMe->m_nCameraTone));
    
    switch(pMe->m_nCameraTone)
    {
#ifdef FEATURE_CARRIER_CHINA_TELCOM
        case OEMNV_CAMERA_SHUTTER_TONE_SHUTTER2:
            CameraApp_SetCFGMenuIcon(popMenu, IDS_TONE_SHUTTER2, TRUE);
            break;
    
        case OEMNV_CAMERA_SHUTTER_TONE_SHUTTER1:
            CameraApp_SetCFGMenuIcon(popMenu, IDS_TONE_SHUTTER1, TRUE);
            break;
#else        
        case OEMNV_CAMERA_SHUTTER_TONE_DISABLE:
            CameraApp_SetCFGMenuIcon(popMenu, IDS_TONE_DISABLE, TRUE);
            break;
    
        case OEMNV_CAMERA_SHUTTER_TONE_ENABLE:
            CameraApp_SetCFGMenuIcon(popMenu, IDS_TONE_ENABLE, TRUE);
            break;
#endif			
    
        default:
            break;  
    }
}

static void CameraApp_PopMenu_SelfTimeInit(CCameraApp *pMe, IMenuCtl *popMenu)
{
    IMENUCTL_DeleteAll(popMenu);   
    CameraApp_SetPopMenuRect(pMe, popMenu, 4);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSCAMERAAPP_RES_FILE, 
                     IDS_SELFTIME_OFF, 
                     IDS_SELFTIME_OFF, 
                     NULL,
                     0);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSCAMERAAPP_RES_FILE, 
                     IDS_SELFTIME_SEC1, 
                     IDS_SELFTIME_SEC1, 
                     NULL, 
                     0);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSCAMERAAPP_RES_FILE, 
                     IDS_SELFTIME_SEC2, 
                     IDS_SELFTIME_SEC2, 
                     NULL, 
                     0);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSCAMERAAPP_RES_FILE,
                     IDS_SELFTIME_SEC3,
                     IDS_SELFTIME_SEC3,
                     NULL, 
                     0);         
    InitMenuIcons(popMenu);
    CameraApp_SetCFGMenuIcon(popMenu, pMe->m_nSelfTimeItemSel, TRUE);
}
            
static void CameraApp_PopMenu_BandingInit(CCameraApp *pMe, IMenuCtl *popMenu)
{
    IMENUCTL_DeleteAll(popMenu);  
    CameraApp_SetPopMenuRect(pMe, popMenu, 2);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSCAMERAAPP_RES_FILE, 
                     IDS_BANDING_MODE1, 
                     IDS_BANDING_MODE1, 
                     NULL, 
                     0);
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSCAMERAAPP_RES_FILE, 
                     IDS_BANDING_MODE2, 
                     IDS_BANDING_MODE2, 
                     NULL, 
                     0);
               
    InitMenuIcons(popMenu);
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_BANDING,
                          &pMe->m_nCameraBanding,
                          sizeof(pMe->m_nCameraBanding));
    
    switch(pMe->m_nCameraBanding)
    {
        case OEMNV_CAMERA_BANDING_50HZ:
            CameraApp_SetCFGMenuIcon(popMenu, IDS_BANDING_MODE1, TRUE);
            break;
    
        case OEMNV_CAMERA_BANDING_60HZ:
            CameraApp_SetCFGMenuIcon(popMenu, IDS_BANDING_MODE2, TRUE);
            break;
    
        default:
            break;
    }
}

static void CameraApp_PopMenu_ResetCFGInit(CCameraApp *pMe, IMenuCtl *popMenu)
{
    IMENUCTL_DeleteAll(popMenu);   
    #if defined (FEATURE_VERSION_K212) || defined(FEATURE_DISP_240X320)
    #if defined(FEATURE_VERSION_IC241A_MMX)
    CameraApp_SetPopMenuRect(pMe, popMenu, 1);
    #else
    CameraApp_SetPopMenuRect(pMe, popMenu, 2);  
    #endif
	#elif defined(FEATURE_VERSION_K212_ND)||defined(FEATURE_VERSION_W516_C260)
	CameraApp_SetPopMenuRect(pMe, popMenu, 2);
	#else
	CameraApp_SetPopMenuRect(pMe, popMenu, 1);
	#endif
    IMENUCTL_AddItem(popMenu, 
                     AEE_APPSCAMERAAPP_RES_FILE, 
                     IDS_CFG_RESET, 
                     IDS_CFG_RESET, 
                     NULL, 
                     0);   

}

static void CameraApp_PlayShutterSound(CCameraApp *pMe)
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
     
        if(IMEDIA_RegisterNotify(pMe->m_pMedia, CameraApp_MediaNotify, pMe) != SUCCESS)
        {
            cmd.nCmd    = MM_CMD_PLAY;
            cmd.nStatus = MM_STATUS_DONE;
            CameraApp_MediaNotify((void *)pMe, &cmd);
            return;
        }

        if(IMEDIA_Play(pMe->m_pMedia) != SUCCESS)
        {
            cmd.nCmd    = MM_CMD_PLAY;
            cmd.nStatus = MM_STATUS_DONE;
            CameraApp_MediaNotify((void *)pMe, &cmd);
            return;
        }
    }
    else
    {
        cmd.nCmd    = MM_CMD_PLAY;
        cmd.nStatus = MM_STATUS_DONE;
        CameraApp_MediaNotify((void *)pMe, &cmd);
    }
}

static void CameraApp_MediaNotify(void *pUser, AEEMediaCmdNotify *pCmdNotify)
{
    CCameraApp *pMe = (CCameraApp *)pUser;

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

/*void CameraApp_AlphaDisplay(CCameraApp *pMe, IMenuCtl *popMenu)
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

static void CameraApp_SetPopMenuRect(CCameraApp *pMe, IMenuCtl* popMenu, int menuItemSum)
{
    AEERect mRec;
    int16  x,y,dx,dy;

    if(!pMe || !popMenu)
    {
        return;
    }
    
    dx = POPMENU_WIDTH;
    #ifdef FEATURE_DISP_240X320
    #ifdef FEATURE_VERSION_IC241A_MMX
    pMe->m_nItemH = 38;
    #else
	pMe->m_nItemH = 28;
    #endif
	#endif
	#if defined( FEATURE_VERSION_K202)&&defined(FEATURE_LANG_CHINESE)
	pMe->m_nItemH = 16;
	#endif
    // the number of popMenu Item is no more than 4
    if(menuItemSum < 4)
    {
        //dy = menuItemSum*(pMe->m_nItemH+7);
        #ifdef FEATURE_VERSION_K202
        dy = menuItemSum*(pMe->m_nItemH+6);
		#else
        #if defined (FEATURE_VERSION_C260_IC18) || defined(FEATURE_VERSION_IC241A_MMX)
        dy = menuItemSum*(pMe->m_nItemH+2);
        #else
		dy = menuItemSum*(pMe->m_nItemH+2);
        #endif
		#endif
    }
    else
    {
        //dy = 4*(pMe->m_nItemH+7);
        #ifdef FEATURE_VERSION_K202
        dy = 4*(pMe->m_nItemH+6);
		#else
        #if defined (FEATURE_VERSION_C260_IC18) || defined(FEATURE_VERSION_IC241A_MMX)
        dy = 4*(pMe->m_nItemH+2);
        #else
		dy = 4*(pMe->m_nItemH+2);
        #endif
		#endif
    }

    x = CFGBAR_TEXT_X;
       
    //y = pMe->m_cyHeight - dy - POPMENU_BOTTOM_Y;
    y = 2*CFGBAR_TEXT_Y;
 
    SETAEERECT(&mRec, x, y, dx, dy);

    IMENUCTL_SetRect(popMenu, &mRec);
}

static void CameraApp_DrawTopBar(CCameraApp *pMe)
{
    int i;
    int16  nResID[CAMERACFGMAX];
    IImage *pTopBarImage = NULL; 
    
#ifdef FEATURE_CAMERA_MULTI_SENSOR  //add by andrew
		(void)ICONFIG_GetItem(pMe->m_pConfig,
                              CFGI_CAMERA_MULTI,
                              &pMe->m_nCameraMulti,
                              sizeof(pMe->m_nCameraMulti));
	   switch(pMe->m_nCameraMulti)
    	{
    		case OEMNV_CAMERA_MULTI_ONE:
				nResID[CAMERACFGMULTI] = IDI_MULTI;
				break;
			case OEMNV_CAMERA_MULTI_TWO:
				nResID[CAMERACFGMULTI] = IDI_MULTI;
				break;
			default:
				break;
	   	}
	   
	(void)ICONFIG_GetItem(pMe->m_pConfig,
                              CFGT_CAMERA_FLASH,
                              &pMe->n_nCameraFlash,
                              sizeof(pMe->n_nCameraFlash));
	   switch(pMe->n_nCameraFlash)
    	{
    		case OEMNV_CAMERA_FLASH_ON:
				nResID[CAMERACFGFLASH] = IDI_CAMER_FLASH;
				break;
			case OEMNV_CAMERA_FLASH_OFF:
				nResID[CAMERACFGFLASH] = IDI_CAMER_FLASH;
				break;
			default:
				break;
	   	}
#endif

#if defined(FEATURE_VERSION_W515V3)||defined(FEATURE_VERSION_S1000T)
#else
    // enviroment cfgID
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_ENVIROMENT,
                          &pMe->m_nCameraEnviroment,
                          sizeof(pMe->m_nCameraEnviroment));
    
    switch(pMe->m_nCameraEnviroment)
    {
	    case OEMNV_CAMERA_ENVIR_AUTO:
	        nResID[CAMERACFGENVIRMENT] = IDI_ENV_AUTO;
	        break;
	        
	    case OEMNV_CAMERA_ENVIR_SUNNY:
	        nResID[CAMERACFGENVIRMENT] = IDI_ENV_SUNNY;
	        break;

	    case OEMNV_CAMERA_ENVIR_CLOUDY:
	        nResID[CAMERACFGENVIRMENT] = IDI_ENV_CLOUDY;
	        break;

	    case OEMNV_CAMERA_ENVIR_NIGHT:
	        nResID[CAMERACFGENVIRMENT] = IDI_ENV_NIGHT;
	        break;
	        
	    default:
	        nResID[CAMERACFGENVIRMENT] = IDI_ENV_AUTO;
	        break;
    }
#endif
    // quality cfgID
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_QUALITY,
                          &pMe->m_nCameraQuality,
                          sizeof(pMe->m_nCameraQuality));
    
    switch(pMe->m_nCameraQuality)
    {
	    case OEMNV_CAMERA_QUALITY_HIGH:
	        nResID[CAMERACFGQUALITY] = IDI_QUALITY_HIGH;
	        break;

	    case OEMNV_CAMERA_QUALITY_MED:
	        nResID[CAMERACFGQUALITY] = IDI_QUALITY_MED;
	        break;

	    case OEMNV_CAMERA_QUALITY_LOW:
	        nResID[CAMERACFGQUALITY] = IDI_QUALITY_LOW;
	        break;
	        
		default:
	        nResID[CAMERACFGQUALITY] = IDI_QUALITY_HIGH;
	        break;
	}

//#if !defined (FEATURE_VERSION_IC241A_MMX)
#if defined(FEATURE_VERSION_X3)
	// size cfgID
	(void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_SIZE,
                         &pMe->m_nCameraSize,
                          sizeof(pMe->m_nCameraSize));

    switch(pMe->m_nCameraSize)
    {
	    case OEMNV_CAMERA_SIZE_INDEX_0:
	        nResID[CAMERACFGSIZE] = IDI_SIZE_240_320;
	        break;

	    case OEMNV_CAMERA_SIZE_INDEX_1:
	        nResID[CAMERACFGSIZE] = IDI_SIZE_480_640;
	        break;
	        
	    default:
	        nResID[CAMERACFGSIZE] = IDI_SIZE_240_320;
	        break;
    }

#endif

#ifdef FEATURE_VERSION_W208S
    // size cfgID
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_SIZE,
                         &pMe->m_nCameraSize,
                          sizeof(pMe->m_nCameraSize));

    switch(pMe->m_nCameraSize)
    {
	    case OEMNV_CAMERA_SIZE_INDEX_0:
	        nResID[CAMERACFGSIZE] = IDI_SIZE_160_128;
	        break;

	    case OEMNV_CAMERA_SIZE_INDEX_1:
	        nResID[CAMERACFGSIZE] = IDI_SIZE_220_176;
	        break;

	    case OEMNV_CAMERA_SIZE_INDEX_2:
	        nResID[CAMERACFGSIZE] = IDI_SIZE_320_240;
	        break;

	    case OEMNV_CAMERA_SIZE_INDEX_3:
	        nResID[CAMERACFGSIZE] = IDI_SIZE_640_480;
	        break;
	        
	    default:
	        nResID[CAMERACFGSIZE] = IDI_SIZE_160_128;
	        break;
    }
#endif

#ifdef FEATURE_VERSION_K212_20D
    // size cfgID
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_SIZE,
                         &pMe->m_nCameraSize,
                          sizeof(pMe->m_nCameraSize));

    switch(pMe->m_nCameraSize)
    {
	    case OEMNV_CAMERA_SIZE_INDEX_1:
	        nResID[CAMERACFGSIZE] = IDI_SIZE_220_176;
	        break;

	    case OEMNV_CAMERA_SIZE_INDEX_2:
	        nResID[CAMERACFGSIZE] = IDI_SIZE_320_240;
	        break;
	    default:
	        nResID[CAMERACFGSIZE] = IDI_SIZE_220_176;
	        break;
    }
#endif
#ifdef FEATURE_VERSION_K212_ND
    // size cfgID
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_SIZE,
                         &pMe->m_nCameraSize,
                          sizeof(pMe->m_nCameraSize));

    switch(pMe->m_nCameraSize)
    {
	    case OEMNV_CAMERA_SIZE_INDEX_0:
	        nResID[CAMERACFGSIZE] = IDI_SIZE_160_128;
	        break;

	    case OEMNV_CAMERA_SIZE_INDEX_1:
	        nResID[CAMERACFGSIZE] = IDI_SIZE_220_176;
	        break;
		case OEMNV_CAMERA_SIZE_INDEX_2:
	        nResID[CAMERACFGSIZE] = IDI_SIZE_320_240;
	        break;
	    default:
	        nResID[CAMERACFGSIZE] = IDI_SIZE_160_128;
	        break;
    }
#endif


#ifdef FEATURE_VERSION_1110W516
    // size cfgID
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_SIZE,
                         &pMe->m_nCameraSize,
                          sizeof(pMe->m_nCameraSize));

        MSG_FATAL("pMe->m_isRecordMode=%d",pMe->m_isRecordMode,0,0);
        if(pMe->m_isRecordMode == FALSE)
        {
            switch(pMe->m_nCameraSize)
            {
    	    case OEMNV_CAMERA_SIZE_INDEX_0:
    	        nResID[CAMERACFGSIZE] = IDI_SIZE_180_240;
    	        break;
    	    case OEMNV_CAMERA_SIZE_INDEX_1:
    	        nResID[CAMERACFGSIZE] = IDI_SIZE_640_480;
    	        break;	        
    	    default:
    	        nResID[CAMERACFGSIZE] = IDI_SIZE_180_240;
    	        break;
            }
        }
        else
        {
            switch(pMe->m_nCameraSize)
            {
                
    	    case OEMNV_CAMERA_SIZE_INDEX_0:
    	        nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
    	        break;
    	    case OEMNV_CAMERA_SIZE_INDEX_1:
    	        nResID[CAMERACFGSIZE] = IDI_SIZE_144_176;
    	        break;	        
    	    default:
    	        nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
    	        break;
            }
        }
#endif	

//Add By zzg 2012_10_30
#if defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337) 
#if defined (FEATURE_VERSION_C260_IC18)
    // size cfgID
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_SIZE,
                         &pMe->m_nCameraSize,
                          sizeof(pMe->m_nCameraSize));

	if(pMe->m_isRecordMode == FALSE)
	{
	    switch(pMe->m_nCameraSize)
	    {
	        case OEMNV_CAMERA_SIZE_INDEX_0:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
		        break;
		    case OEMNV_CAMERA_SIZE_INDEX_1:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_176_220;
		        break;

		    case OEMNV_CAMERA_SIZE_INDEX_2:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_240_320;
		        break;
			case OEMNV_CAMERA_SIZE_INDEX_3:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_480_640;
		        break;	
		    default:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
		        break;
	    }
	}
	else
	{
		switch(pMe->m_nCameraSize)
	    {
		    case OEMNV_CAMERA_SIZE_INDEX_0:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
		        break;

		    case OEMNV_CAMERA_SIZE_INDEX_1:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_144_176;
		        break;
		    default:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
		        break;
	    }
	}
#else
    // size cfgID
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_SIZE,
                         &pMe->m_nCameraSize,
                          sizeof(pMe->m_nCameraSize));

	if(pMe->m_isRecordMode == FALSE)
	{
	    switch(pMe->m_nCameraSize)
	    {
		    case OEMNV_CAMERA_SIZE_INDEX_0:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_176_220;
		        break;

		    case OEMNV_CAMERA_SIZE_INDEX_1:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_240_320;
		        break;
			case OEMNV_CAMERA_SIZE_INDEX_2:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_480_640;
		        break;	
		    default:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_176_220;
		        break;
	    }
	}
	else
	{
		switch(pMe->m_nCameraSize)
	    {
		    case OEMNV_CAMERA_SIZE_INDEX_0:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
		        break;

		    case OEMNV_CAMERA_SIZE_INDEX_1:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_144_176;
		        break;
		    default:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
		        break;
	    }
	}
#endif
#endif	
//Add End

//Add by pyuangui 20121229
 #if defined( FEATURE_VERSION_C316)//||defined(FEATURE_VERSION_C117_M74)
    // size cfgID
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_SIZE,
                         &pMe->m_nCameraSize,
                          sizeof(pMe->m_nCameraSize));

	if(pMe->m_isRecordMode == FALSE)
	{
	    switch(pMe->m_nCameraSize)
	    {
		    case OEMNV_CAMERA_SIZE_INDEX_0:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
		        break;
				
            case OEMNV_CAMERA_SIZE_INDEX_1:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_176_220;
		        break;
				
		    case OEMNV_CAMERA_SIZE_INDEX_2:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_240_320;
		        break;
				
			case OEMNV_CAMERA_SIZE_INDEX_3:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_480_640;
		        break;	
				
		    default:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
		        break;
	    }
	}
	else
	{
		switch(pMe->m_nCameraSize)
	    {
		    case OEMNV_CAMERA_SIZE_INDEX_0:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
		        break;

		    case OEMNV_CAMERA_SIZE_INDEX_1:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_144_176;
		        break;
		    default:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
		        break;
	    }
	}
#endif	
//Add End

#if defined (FEATURE_VERSION_VG68)||defined(FEATURE_VERSION_K202_LM129C)||defined(FEATURE_VERSION_W516_C260)
    // size cfgID
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_SIZE,
                         &pMe->m_nCameraSize,
                          sizeof(pMe->m_nCameraSize));

    switch(pMe->m_nCameraSize)
    {
	    case OEMNV_CAMERA_SIZE_INDEX_0:
	        nResID[CAMERACFGSIZE] = IDI_SIZE_176_220;
	        break;

	    case OEMNV_CAMERA_SIZE_INDEX_1:
	        nResID[CAMERACFGSIZE] = IDI_SIZE_320_240;
	        break;
	    default:
	        nResID[CAMERACFGSIZE] = IDI_SIZE_176_220;
	        break;
    }
#endif	

#if defined(FEATURE_CAMERA_8W) 
(void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_SIZE,
                         &pMe->m_nCameraSize,
                          sizeof(pMe->m_nCameraSize));
    switch(pMe->m_nCameraSize)
    {
#if !defined(FEATURE_DISP_240X320)
        case OEMNV_CAMERA_SIZE_INDEX_0:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
		        break;
		case OEMNV_CAMERA_SIZE_INDEX_1:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_176_220;
		        break;
               
	    case OEMNV_CAMERA_SIZE_INDEX_2:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_240_320;
		        break;	
	    default:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
		        break;
#else
        default:
              //  nResID[CAMERACFGSIZE] = IDI_SIZE_240_320;
		        break;	
#endif 
        
    }
#endif

#if defined(FEATURE_CAMERA_8W) 
(void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_SIZE,
                         &pMe->m_nCameraSize,
                          sizeof(pMe->m_nCameraSize));
    if(pMe->m_isRecordMode == FALSE)
    {

        switch(pMe->m_nCameraSize)
        {
#if !defined(FEATURE_DISP_240X320)
            case OEMNV_CAMERA_SIZE_INDEX_0:
    		        nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
    		        break;
    		case OEMNV_CAMERA_SIZE_INDEX_1:
    		        nResID[CAMERACFGSIZE] = IDI_SIZE_176_220;
    		        break;
    	    case OEMNV_CAMERA_SIZE_INDEX_2:
    		        nResID[CAMERACFGSIZE] = IDI_SIZE_240_320;
    		        break;	
                   
    	    default:
    		        nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
    		        break;
#else 
            default:
                    nResID[CAMERACFGSIZE] = IDI_SIZE_240_320;
                    break;

#endif
        }
    }
    else
    {
         switch(pMe->m_nCameraSize)
        {
#if !defined(FEATURE_DISP_240X320)
            case OEMNV_CAMERA_SIZE_INDEX_0:
    		        nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
    		        break;
    		case OEMNV_CAMERA_SIZE_INDEX_1:
    		        nResID[CAMERACFGSIZE] = IDI_SIZE_144_176;
    		        break;   
    	    default:
    		        nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
    		        break;
#else
            default:
    		        nResID[CAMERACFGSIZE] = IDI_SIZE_144_176;
    		        break;

#endif
        }
    }
#endif

#if  defined(FEATURE_VERSION_C01) ||defined(FEATURE_VERSION_C11)||defined(FEATURE_VERSION_C180)
    // size cfgID
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_SIZE,
                         &pMe->m_nCameraSize,
                          sizeof(pMe->m_nCameraSize));
#if defined(FEATURE_VERSION_C310)
 switch(pMe->m_nCameraSize)
    {

        case OEMNV_CAMERA_SIZE_INDEX_0:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_176_220;
		        break;
		case OEMNV_CAMERA_SIZE_INDEX_1:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_240_320;
		        break;
	    case OEMNV_CAMERA_SIZE_INDEX_2:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_480_640;
		        break;	
	    default:
		        nResID[CAMERACFGSIZE] = IDI_SIZE_176_220;
		        break;
}

#else
    switch(pMe->m_nCameraSize)
    {
        case OEMNV_CAMERA_SIZE_INDEX_0:
            nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
            break;

        case OEMNV_CAMERA_SIZE_INDEX_1:
            nResID[CAMERACFGSIZE] = IDI_SIZE_176_220;
            break;
        #if defined(FEATURE_VERSION_C11) || defined(FEATURE_VERSION_C180)
		case OEMNV_CAMERA_SIZE_INDEX_2:
            nResID[CAMERACFGSIZE] = IDI_SIZE_240_320;
        #else
        case OEMNV_CAMERA_SIZE_INDEX_2:
            nResID[CAMERACFGSIZE] = IDI_SIZE_240_320;
            break;   
        #endif
        default:
            nResID[CAMERACFGSIZE] = IDI_SIZE_240_320;
            break;
}
#endif

#endif

#ifdef FEATURE_VERSION_IC241A_MMX
    // size cfgID
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_SIZE,
                         &pMe->m_nCameraSize,
                          sizeof(pMe->m_nCameraSize));

    if(pMe->m_isRecordMode == FALSE)
    {
        switch(pMe->m_nCameraSize)
        {
            case OEMNV_CAMERA_SIZE_INDEX_0:
                nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
                break;

            case OEMNV_CAMERA_SIZE_INDEX_1:
                nResID[CAMERACFGSIZE] = IDI_SIZE_176_220;
                break;       
    		
            case OEMNV_CAMERA_SIZE_INDEX_2:
                nResID[CAMERACFGSIZE] = IDI_SIZE_240_320;
                break;  
                
            case OEMNV_CAMERA_SIZE_INDEX_3:
                nResID[CAMERACFGSIZE] = IDI_SIZE_480_640;
                break;     
           
            default:
                nResID[CAMERACFGSIZE] = IDI_SIZE_240_320;
                break;
        }
    }
    else
    {
        switch(pMe->m_nCameraSize)
        {
            case OEMNV_CAMERA_SIZE_INDEX_0:
                nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
                break;

            case OEMNV_CAMERA_SIZE_INDEX_1:
                nResID[CAMERACFGSIZE] = IDI_SIZE_144_176;
                break;                         
            default:
                nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
                break;
        }
    }    
#endif


#if defined (FEATURE_VERSION_W027_IC18) || defined (FEATURE_VERSION_W027_IC18_YINNI)
    // size cfgID
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_SIZE,
                         &pMe->m_nCameraSize,
                          sizeof(pMe->m_nCameraSize));

    if(pMe->m_isRecordMode == FALSE)
    {
        switch(pMe->m_nCameraSize)
        {
            case OEMNV_CAMERA_SIZE_INDEX_0:
                nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
                break;

            case OEMNV_CAMERA_SIZE_INDEX_1:
                nResID[CAMERACFGSIZE] = IDI_SIZE_176_220;
                break;       
    		
            case OEMNV_CAMERA_SIZE_INDEX_2:
                nResID[CAMERACFGSIZE] = IDI_SIZE_240_320;
                break;             
            default:
                nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
                break;
        }
    }
    else
    {
        switch(pMe->m_nCameraSize)
        {
            case OEMNV_CAMERA_SIZE_INDEX_0:
                nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
                break;

            case OEMNV_CAMERA_SIZE_INDEX_1:
                nResID[CAMERACFGSIZE] = IDI_SIZE_144_176;
                break;                         
            default:
                nResID[CAMERACFGSIZE] = IDI_SIZE_128_160;
                break;
        }
    }    
#endif


//#endif      //!defined FEATURE_VERSION_IC241A_MMX

//#endif
    // sound cfgID
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_TONE,
                          &pMe->m_nCameraTone,
                          sizeof(pMe->m_nCameraTone));
    
    switch(pMe->m_nCameraTone)
    {       
	    case OEMNV_CAMERA_SHUTTER_TONE_ENABLE:
	        nResID[CAMERACFGTONE] = IDI_TONE_ENABLE;
	        break;

	    case OEMNV_CAMERA_SHUTTER_TONE_DISABLE:
	        nResID[CAMERACFGTONE] = IDI_TONE_DISABLE;
	        break;		
	        
	    default:
	        nResID[CAMERACFGTONE] = IDI_TONE_ENABLE;
	        break;
    }
    
    // draw selftime cfg    
    switch(pMe->m_nSelfTimeItemSel)
    {
	    case IDS_SELFTIME_OFF:
	        nResID[CAMERACFGSELFTIME] = IDI_SELFTIME_OFF;
	        break;

	    case IDS_SELFTIME_SEC1:
	        nResID[CAMERACFGSELFTIME] = IDI_SELFTIME_5;
	        break;

	    case IDS_SELFTIME_SEC2:
	        nResID[CAMERACFGSELFTIME] = IDI_SELFTIME_10;
	        break;

	    case IDS_SELFTIME_SEC3:
	        nResID[CAMERACFGSELFTIME] = IDI_SELFTIME_15;
	        break;
	        
		default:
	        nResID[CAMERACFGSELFTIME] = IDI_SELFTIME_OFF;
	        break;
    }
    #if 0
    // banding cfgID
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                          CFGI_CAMERA_BANDING,
                          &pMe->m_nCameraBanding,
                          sizeof(pMe->m_nCameraBanding));
    
    switch(pMe->m_nCameraBanding){
    case OEMNV_CAMERA_BANDING_50HZ:
        nResID[CAMERACFGBANDING] = IDI_BANDING_50;
        break;

    case OEMNV_CAMERA_BANDING_60HZ:
        nResID[CAMERACFGBANDING] = IDI_BANDING_60;
        break;
        
    default:
        nResID[CAMERACFGBANDING] = IDI_BANDING_50;
        break;
    }
    #endif
    nResID[CAMERACFGRESET] = IDI_RESET;
    #if defined(FEATURE_VERSION_W515V3)||defined(FEATURE_VERSION_S1000T)
    for(i = CAMERACFGFIRST; i < CAMERACFGMAX; i++)
    {
        pTopBarImage = ISHELL_LoadResImage(pMe->m_pShell, 
                                           CAMERAAPP_IMAGE_RES_FILE, 
                                           nResID[i]);
        if(pTopBarImage)
        {
            IIMAGE_Draw(pTopBarImage, (i-1)*(TOPBAR_ICON_WIDTH+TOPBAR_ICON_SPACE), TOPBAR_ICON_Y);  // + 5
            IIMAGE_Release(pTopBarImage);
            pTopBarImage = NULL;
        }
    }

    #else
    for(i = 0; i < CAMERACFGMAX; i++)
    {
        if((i == CAMERACFGSELFTIME) || (i == CAMERACFGTONE))
        {
            if(pMe->m_isRecordMode == TRUE)
            { 
                continue;
            }
        }
/*
#ifdef FEATURE_VERSION_IC241A_MMX
        if (i == CAMERACFGSIZE)
        {
            continue;
        }
#endif
*/
        pTopBarImage = ISHELL_LoadResImage(pMe->m_pShell, 
                                           CAMERAAPP_IMAGE_RES_FILE, 
                                           nResID[i]);
        if(pTopBarImage)
        {
        	//IIMAGE_Draw(pTopBarImage, i*(TOPBAR_ICON_WIDTH+TOPBAR_ICON_SPACE), TOPBAR_ICON_Y);	// + 5

/*
#ifdef FEATURE_VERSION_IC241A_MMX
            if (pMe->m_isRecordMode == TRUE) 
        	{
        	    if (i == CAMERACFGRESET)
				{
				    IIMAGE_Draw(pTopBarImage, (i-3)*(TOPBAR_ICON_WIDTH+TOPBAR_ICON_SPACE), TOPBAR_ICON_Y);	// + 5				
                }                
                else
                {
				    IIMAGE_Draw(pTopBarImage, i*(TOPBAR_ICON_WIDTH+TOPBAR_ICON_SPACE), TOPBAR_ICON_Y);	// + 5			
                }    
			}
			else
			{
			    if ((i == CAMERACFGRESET) || (i == CAMERACFGSELFTIME) || (i == CAMERACFGTONE))
				{
				    IIMAGE_Draw(pTopBarImage, (i-1)*(TOPBAR_ICON_WIDTH+TOPBAR_ICON_SPACE), TOPBAR_ICON_Y);	// + 5				
                }                
                else
                {
				    IIMAGE_Draw(pTopBarImage, i*(TOPBAR_ICON_WIDTH+TOPBAR_ICON_SPACE), TOPBAR_ICON_Y);	// + 5			
                }                
			}
#else
*/
        	if ((pMe->m_isRecordMode == TRUE) && (i == CAMERACFGRESET))
        	{
				IIMAGE_Draw(pTopBarImage, (i-2)*(TOPBAR_ICON_WIDTH+TOPBAR_ICON_SPACE), TOPBAR_ICON_Y);	// + 5				
			}
			else
			{
				IIMAGE_Draw(pTopBarImage, i*(TOPBAR_ICON_WIDTH+TOPBAR_ICON_SPACE), TOPBAR_ICON_Y);	// + 5
			}
//#endif            
            
            IIMAGE_Release(pTopBarImage);
            pTopBarImage = NULL;
        }
    }
    #endif 

	//Add By zzg 2010_07_25
    {
	    IImage *pCameraCFGChooseIcon = ISHELL_LoadResImage(pMe->m_pShell, CAMERAAPP_IMAGE_RES_FILE, IDI_CHOOSE_REC);
	    
	    if (pCameraCFGChooseIcon)
	    {  
			IIMAGE_SetDrawSize(pCameraCFGChooseIcon, TOPBAR_ICON_WIDTH, CFGBAR_TEXT_HEIGHT);
			#if defined(FEATURE_VERSION_W515V3)||defined(FEATURE_VERSION_S1000T)
			IIMAGE_Draw(pCameraCFGChooseIcon, (TOPBAR_ICON_SPACE+TOPBAR_ICON_WIDTH)*(pMe->m_nCameraCFG-1), TOPBAR_ICON_Y);	//Add By zzg 2010_07_25
			#else
/*            
#ifdef FEATURE_VERSION_IC241A_MMX
            if (pMe->m_isRecordMode == TRUE) 
        	{
        	    if (pMe->m_nCameraCFG == CAMERACFGRESET)
				{
				    IIMAGE_Draw(pCameraCFGChooseIcon, (pMe->m_nCameraCFG-3)*(TOPBAR_ICON_WIDTH+TOPBAR_ICON_SPACE), TOPBAR_ICON_Y);	// + 5			
                }                
                else
                {
				    IIMAGE_Draw(pCameraCFGChooseIcon, (TOPBAR_ICON_SPACE+TOPBAR_ICON_WIDTH)*(pMe->m_nCameraCFG), TOPBAR_ICON_Y);			
                }    
			}
			else
			{
			    if ((pMe->m_nCameraCFG == CAMERACFGRESET) || (pMe->m_nCameraCFG == CAMERACFGSELFTIME) || (pMe->m_nCameraCFG == CAMERACFGTONE))
				{
				    IIMAGE_Draw(pCameraCFGChooseIcon, (TOPBAR_ICON_SPACE+TOPBAR_ICON_WIDTH)*(pMe->m_nCameraCFG-1), TOPBAR_ICON_Y);
                }                
                else
                {
				    IIMAGE_Draw(pCameraCFGChooseIcon, (TOPBAR_ICON_SPACE+TOPBAR_ICON_WIDTH)*(pMe->m_nCameraCFG), TOPBAR_ICON_Y);			
                }                
			}
#else  
*/
			if ((pMe->m_isRecordMode == TRUE) && (pMe->m_nCameraCFG == CAMERACFGRESET))
        	{
				IIMAGE_Draw(pCameraCFGChooseIcon, (pMe->m_nCameraCFG-2)*(TOPBAR_ICON_WIDTH+TOPBAR_ICON_SPACE), TOPBAR_ICON_Y);	// + 5				
			}
			else
            {
            	IIMAGE_Draw(pCameraCFGChooseIcon, (TOPBAR_ICON_SPACE+TOPBAR_ICON_WIDTH)*(pMe->m_nCameraCFG), TOPBAR_ICON_Y);
			}
//#endif            
            #endif
	        IIMAGE_Release(pCameraCFGChooseIcon);
	        pCameraCFGChooseIcon = NULL;
	    }
	}
	//Add End
}

static void CameraApp_DrawMidPic(CCameraApp *pMe)
{
    IImage *pImage = NULL;
    AEEImageInfo myInfo;
    
    switch(pMe->m_pActiveDlgID)
    {
        case IDD_CPREVIEW:
        #ifdef FEATURE_MOVIE_RECORD_SUPPORT
        	if ( pMe->m_isRecordMode )
        	{
           		pImage = ISHELL_LoadResImage(pMe->m_pShell,
                                         CAMERAAPP_IMAGE_RES_FILE,
                                         IDI_MID_RECORD);
	            if(pImage != NULL)
	            {
	                IIMAGE_GetInfo(pImage, &myInfo);
	                IIMAGE_Draw(pImage, (pMe->m_cxWidth - myInfo.cx)/2, pMe->m_cyHeight - myInfo.cy -1 );
	                IIMAGE_Release(pImage);
	                pImage = NULL;
	            }
            }
            else
        #endif
        	{
                {
                    #ifdef FEATURE_LOW_MEM_BIGFONT1
                    #ifdef FEATURE_VERSION_K212_VHOPE
                    IImage *pFrameImage = NULL;
                    pFrameImage = ISHELL_LoadResImage(pMe->m_pShell, CAMERAAPP_IMAGE_RES_FILE, IDI_FRAME_11);
                    if(pFrameImage)
                    {
                        IImage_Draw(pFrameImage, 20, 0);
                        IImage_Draw(pFrameImage, 20, 300);
                        IImage_Release(pFrameImage);
                        pFrameImage = NULL;
                    }
                     pFrameImage = ISHELL_LoadResImage(pMe->m_pShell, CAMERAAPP_IMAGE_RES_FILE, IDI_FRAME_12);
                    if(pFrameImage)
                    {
                        IImage_Draw(pFrameImage, 0, 0);
                        IImage_Draw(pFrameImage, 220, 0);
                        IImage_Release(pFrameImage);
                        pFrameImage = NULL;
                    }
                    #else
                    IImage *pFrameImage = NULL;
                    pFrameImage = ISHELL_LoadResImage(pMe->m_pShell, CAMERAAPP_IMAGE_RES_FILE, IDI_FRAME_11);
                    if(pFrameImage)
                    {
                        IImage_Draw(pFrameImage, 0, 275);
                        IImage_Release(pFrameImage);
                        pFrameImage = NULL;
                    }
                     pFrameImage = ISHELL_LoadResImage(pMe->m_pShell, CAMERAAPP_IMAGE_RES_FILE, IDI_FRAME_12);
                    if(pFrameImage)
                    {
                        IImage_Draw(pFrameImage, 0, 0);
                        IImage_Release(pFrameImage);
                        pFrameImage = NULL;
                    }
                     pFrameImage = ISHELL_LoadResImage(pMe->m_pShell, CAMERAAPP_IMAGE_RES_FILE, IDI_FRAME_13);
                    if(pFrameImage)
                    {
                        IImage_Draw(pFrameImage, 0, 0);
                        IImage_Release(pFrameImage);
                        pFrameImage = NULL;
                    }
                     pFrameImage = ISHELL_LoadResImage(pMe->m_pShell, CAMERAAPP_IMAGE_RES_FILE, IDI_FRAME_14);
                    if(pFrameImage)
                    {
                        IImage_Draw(pFrameImage, 203, 0);
                        IImage_Release(pFrameImage);
                        pFrameImage = NULL;
                    }
                    #endif
					#endif
                    pImage = ISHELL_LoadResImage(pMe->m_pShell,
                                             CAMERAAPP_IMAGE_RES_FILE,
                                             IDI_MID_CAMERA);
    	            if(pImage != NULL)
    	            {
    	                IIMAGE_GetInfo(pImage, &myInfo);
    	                IIMAGE_Draw(pImage, (pMe->m_cxWidth - myInfo.cx)/2, pMe->m_cyHeight - myInfo.cy -1 );
    	                IIMAGE_Release(pImage);
    	                pImage = NULL;
    	            }
	            }
            }
            break;
            
        default:
            break;
    }
}

static void CameraApp_DrawCFGPromptText(CCameraApp *pMe)
{  
    AEERect prc;
    int16   nResID = 0;       
    AECHAR  pcText[20]; 
    
    if(pMe->m_pActiveDlgID == IDD_CCAMERACFG)
    {
        switch(pMe->m_nCameraCFG)
        {
        	#ifdef FEATURE_CAMERA_MULTI_SENSOR
			case CAMERACFGMULTI:
				nResID = IDS_CFG_MULTI;
                break;
			case CAMERACFGFLASH:
				nResID = IDS_CFG_FLASH;
				break;
		    #endif
            case CAMERACFGENVIRMENT:
                #ifdef FEATURE_VERSION_W317A
                nResID = IDS_CFG_SCENEMODE;
                #else
                nResID = IDS_CFG_ENVIR;
                #endif
                break;
                
            case CAMERACFGQUALITY:
                #ifdef FEATURE_VERSION_W317A
                nResID =IDS_CFG_IMAGEQUALITY;
                #else
                nResID = IDS_CFG_QUALITY;
                #endif
                break;

            #ifndef FEATURE_VERSION_K212      
			#ifndef FEATURE_LOW_MEM_BIGFONT
            case CAMERACFGSIZE:
                #ifdef FEATURE_VERSION_W317A
                nResID = IDS_CFG_IMAGESIZE;
                #else
                nResID = IDS_CFG_SIZE;
                #endif
                break;
            #endif    
            #endif
            case CAMERACFGTONE:
                #ifdef FEATURE_VERSION_W317A
                nResID = IDS_CFG_SOUND;
                #else
                nResID = IDS_CFG_TONE;
                #endif
                break;
                
            case CAMERACFGSELFTIME:
                nResID = IDS_CFG_SELFTIMER;
                break;
           #if 0     
            case CAMERACFGBANDING:
                nResID = IDS_CFG_BANDING;
                break;
           #endif
            case CAMERACFGRESET:
                nResID = IDS_CFG_RESET;
                break;
                
            default:
                break; 
        }
    }
    
	SETAEERECT(&prc, CFGBAR_TEXT_X, CFGBAR_TEXT_Y, CFGBAR_TEXT_WIDTH, CFGBAR_TEXT_HEIGHT);	//Add By zzg 2010_07_24

    MEMSET(pcText, 0, sizeof(pcText)); 
    (void)ISHELL_LoadResString(pMe->m_pShell, 
                               AEE_APPSCAMERAAPP_RES_FILE,       
                               nResID, 
                               pcText, 
                               sizeof(pcText));     

    if(pMe->m_nCameraSize != OEMNV_CAMERA_SIZE_INDEX_0|| (pMe->m_pActiveDlgID == IDD_CVIDEOCFG))
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

static void CameraApp_CPreviewStart(CCameraApp *pMe)
{
    AEESize displaySize, captureSize; 
    int16 quality = 0;
#ifdef FEATURE_CAMERA_MULTI_SENSOR
	cam_sensor_model_pair_type sensor_type;

#ifdef FEATURE_CAMERA_MULTI_NEW_AUTO_DETECT
	ICAMERA_GetCurrentSensor(pMe->m_pCamera,&sensor_type,&sensor_type);
#endif

	MSG_FATAL("CameraApp_CPreviewStart sensor_type=%d",sensor_type,0,0);
	if ( sensor_type == CAM_PARM_ID_PAIR_SIV121A_SID130B 
      || sensor_type == CAM_PARM_ID_PAIR_SIV121D_SID130B 
      || sensor_type == CAM_PARM_ID_PAIR_SIV120A_SID130B )
	{
		if ( pMe->m_nCameraMulti == OEMNV_CAMERA_MULTI_ONE )
		{
			captureSize.cx = g_CameraFrontSizeCFG[pMe->m_nCameraSize].dx;//480;
		    captureSize.cy = g_CameraFrontSizeCFG[pMe->m_nCameraSize].dy;//640;
		    displaySize.cx = 240;
		    displaySize.cy = 320;
		}
		else if ( pMe->m_nCameraMulti == OEMNV_CAMERA_MULTI_TWO)
		{
			captureSize.cx = g_CameraBackSizeCFG[pMe->m_nCameraSize].dx;//600;
		    captureSize.cy = g_CameraBackSizeCFG[pMe->m_nCameraSize].dy;//800
		    displaySize.cx = 240;
		    displaySize.cy = 320;
		}
	}
	else if ( sensor_type == CAM_PARM_ID_PAIR_SP0838_SP0A18 || sensor_type == CAM_PARM_ID_PAIR_SP0A18_SP0838 )
	{
		captureSize.cx = 480;
	    captureSize.cy = 640;
	    displaySize.cx = 240;
	    displaySize.cy = 320;
	}
    else if(sensor_type == CAM_PARM_ID_PAIR_GC0329_GC0329)
    {
        if ( pMe->m_nCameraMulti == OEMNV_CAMERA_MULTI_ONE )
		{
			captureSize.cx = g_CameraFrontSizeCFG[pMe->m_nCameraSize].dx;//480;
		    captureSize.cy = g_CameraFrontSizeCFG[pMe->m_nCameraSize].dy;//640;
		    displaySize.cx = 240;
		    displaySize.cy = 320;
		}
		else if ( pMe->m_nCameraMulti == OEMNV_CAMERA_MULTI_TWO)
		{
			captureSize.cx = g_CameraBackSizeCFG[pMe->m_nCameraSize].dx;//600;
		    captureSize.cy = g_CameraBackSizeCFG[pMe->m_nCameraSize].dy;//800
		    displaySize.cx = 240;
		    displaySize.cy = 320;
		}
    }
#else

    if(pMe->m_sensor_model == 30)
    {
        captureSize.cx = g_CameraSizeCFG[pMe->m_nCameraSize].dx;
        captureSize.cy = g_CameraSizeCFG[pMe->m_nCameraSize].dy;
#ifndef FEATURE_CAMERA_NOFULLSCREEN
		displaySize.cx = g_CameraSizeCFG[0].dx;
		displaySize.cy = g_CameraSizeCFG[0].dy;
#endif
    }
    else if(pMe->m_sensor_model == 10)
    {
#ifdef FEATURE_MOVIE_RECORD_SUPPORT
		if ( pMe->m_isRecordMode )
		{
			if(pMe->m_nCameraSize >= (sizeof(g_VideoSizeCFG_10)/sizeof(CCameraSize)-1))
				pMe->m_nCameraSize=OEMNV_CAMERA_SIZE_INDEX_0;
			captureSize.cx = g_VideoSizeCFG_10[pMe->m_nCameraSize].dx;
	        captureSize.cy = g_VideoSizeCFG_10[pMe->m_nCameraSize].dy;            
            
#ifndef FEATURE_CAMERA_NOFULLSCREEN	        
			displaySize.cx = SCREEN_WIDTH;
	        displaySize.cy = SCREEN_HEIGHT;
#endif	
		}
		else
		{
			captureSize.cx = g_CameraSizeCFG_10[pMe->m_nCameraSize].dx;
	        captureSize.cy = g_CameraSizeCFG_10[pMe->m_nCameraSize].dy;
#ifndef FEATURE_CAMERA_NOFULLSCREEN
#if defined (FEATURE_VERSION_K212)||defined(FEATURE_DISP_240X320)
			displaySize.cx = 240;
	        displaySize.cy = 320;
#else
	        displaySize.cx = g_CameraSizeCFG_10[0].dx;
	        displaySize.cy = g_CameraSizeCFG_10[0].dy;
#endif
#endif
		}
#else
		captureSize.cx = g_CameraSizeCFG_10[pMe->m_nCameraSize].dx;
        captureSize.cy = g_CameraSizeCFG_10[pMe->m_nCameraSize].dy;
#ifndef FEATURE_CAMERA_NOFULLSCREEN
#if defined (FEATURE_VERSION_K212)||defined(FEATURE_DISP_240X320)
					displaySize.cx = 240;
					displaySize.cy = 320;
#else

        displaySize.cx = g_CameraSizeCFG_10[0].dx;
        displaySize.cy = g_CameraSizeCFG_10[0].dy;
#endif
#endif
#endif
        
    }

    MSG_FATAL("***zzg CameraApp CPreview m_nCameraSize=%d***", pMe->m_nCameraSize, 0, 0);

	MSG_FATAL("***zzg CPreviewStart captureSize:%d,%d***", captureSize.cx, captureSize.cy, 0);
	MSG_FATAL("***zzg CPreviewStart displaySize:%d,%d***", displaySize.cx, displaySize.cy , 0);
	
#endif
#ifdef FEATURE_CAMERA_NOFULLSCREEN
#if defined(FEATURE_DISP_160X128)
    displaySize.cx = 96;
    displaySize.cy = 96;
#elif defined(FEATURE_DISP_220X176)
    displaySize.cx = 176;
    displaySize.cy = 128;
#elif defined(FEATURE_DISP_128X160)
    displaySize.cx = 128;
    displaySize.cy = 96; 
#elif defined(FEATURE_DISP_176X220)
    displaySize.cx = 176;
    displaySize.cy = 128;
#elif defined(FEATURE_DISP_240X320)
#ifdef FEATURE_VERSION_K212_HUALU
#if defined(FEATURE_VERSION_K212_VHOPE)
    displaySize.cx = 240;  //200
    displaySize.cy = 300;  //270
#else
	displaySize.cx = 240;  //200
    displaySize.cy = 300;  //270
#endif
#else
	displaySize.cx = 96;
    displaySize.cy = 96;
#endif
#else
    displaySize.cx = 96;
    displaySize.cy = 96;
#endif
#ifdef FEATURE_LOW_MEM_BIGFONT
    pMe->m_rcPreview.x=0; 
    pMe->m_rcPreview.y=0; 
#else
    pMe->m_rcPreview.x  = pMe->m_rc.x+(pMe->m_rc.dx-displaySize.cx)/2;
    pMe->m_rcPreview.y  = pMe->m_rc.y+(pMe->m_rc.dy-displaySize.cy)/2;
#endif
    pMe->m_rcPreview.dx = displaySize.cx;
    pMe->m_rcPreview.dy = displaySize.cy;

    ICAMERA_SetParm(pMe->m_pCamera, CAM_PARM_LCD_DIRECT_ACCESS, (int32)FALSE, (int32)&(pMe->m_rcPreview));
#elif defined(T_QSC1110)
{
	
    CameraDirectMode myMode;
    myMode.clsDisplay = 0;
    myMode.nLayer     = 0;
    myMode.xDst       = 0;
    myMode.yDst       = 0;
    myMode.cxDst      = displaySize.cx;
    myMode.cyDst      = displaySize.cy;
    //ICAMERA_SetParm(pMe->m_pCamera, CAM_PARM_LCD_DIRECT_ACCESS_EX, (int32)TRUE, (int32)&(myMode));
}
#endif
        
    // set camera quality
    switch(pMe->m_nCameraQuality)
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

    MSG_FATAL("displaySize.cx=%d,displaySize.cy=%d",displaySize.cx,displaySize.cy,0);
	MSG_FATAL("captureSize.cx=%d,captureSize.cy=%d",captureSize.cx,captureSize.cy,0);
#ifndef FEATURE_DSP
	if ( pMe->m_isRecordMode )
	{
		ICAMERA_SetParm(pMe->m_pCamera, CAM_PARM_PREVIEW_TYPE, CAM_PREVIEW_MOVIE, 0);
	}
	else
	{
		ICAMERA_SetParm(pMe->m_pCamera, CAM_PARM_PREVIEW_TYPE, CAM_PREVIEW_SNAPSHOT, 0);
		ICAMERA_SetParm(pMe->m_pCamera, CAM_PARM_MULTISHOT, 1, 0);
	}
	
    ICAMERA_SetQuality(pMe->m_pCamera, quality);    
    ICAMERA_SetSize(pMe->m_pCamera, &captureSize);
    ICAMERA_SetDisplaySize(pMe->m_pCamera, &displaySize);

    
    ICAMERA_Preview(pMe->m_pCamera);
#endif

    pMe->m_nCameraState = CAM_PREVIEW;  


    pMe->m_bIsPreview = TRUE;
}

//this function used to upgrade the record time
static void CameraApp_OnRecordTimeFeedBack(void *pUser)
{
	CCameraApp *pThis = (CCameraApp*)pUser;
	AEERect     rc = pThis->m_rc;

	MSG_FATAL("!!!!!!!!!!! CameraApp_OnRecordTimeFeedBack!!",0,0,0);
	if (pThis->m_nCameraState == CAM_RECORDING)
	{
		uint32 time = GETTIMESECONDS();
		uint32 elapsedTime = 0;
		JulianType date = {0};

		if (pThis->RecordingTime_parm.RecordStartTime > 0)
		{
			elapsedTime = time - pThis->RecordingTime_parm.RecordStartTime;
		}
		elapsedTime += pThis->RecordingTime_parm.RecordTotalTime;

		if (pThis->RecordingTime_parm.LastTime != time)
		{
			// GETJULIANDATE returns the current date/time when seconds == 0
			if (elapsedTime > 0)
			{
				GETJULIANDATE(elapsedTime, &date);
			}
			
			pThis->RecordingTime.Hour = date.wHour;
			pThis->RecordingTime.Min = date.wMinute;
			pThis->RecordingTime.Sec = date.wSecond;
			//upgrade the time to UI

			ISHELL_PostEvent(pThis->m_pShell, AEECLSID_APP_CAMERA, EVT_USER_REDRAW, NULL, NULL);
			pThis->RecordingTime_parm.LastTime = time;
		}

		ISHELL_SetTimer(pThis->m_pShell, 500, (PFNNOTIFY)CameraApp_OnRecordTimeFeedBack, pThis);
	}
}

static void CameraApp_RecordVideo(CCameraApp *pMe)
{
#ifdef FEATURE_MOVIE_RECORD_SUPPORT
	int result = 0;
    char sFileName[MIN_PIC_CHAR_NAME_LEN];
    MSG_FATAL("CameraApp_RecordVideo!!!",0,0,0);
    MEMSET(pMe->m_sCurrentFileName, '\0', sizeof(pMe->m_sCurrentFileName));
    MEMSET(sFileName, '\0', sizeof(sFileName));
    
    CameraApp_GetDateForRecordFileName(pMe, pMe->m_sCurrentFileName, FILE_TYPE_3GP);
    CameraApp_SetDateForRecordFileName(pMe,FILE_TYPE_3GP);
   
    // copy the pic name to sFilename buffer
    if(pMe->m_nCameraStorage == OEMNV_CAMERA_STORAGE_MEMORY_CARD)
    {
        STRCPY(pMe->m_sCaptureFileName, pMe->m_sCurrentFileName+STRLEN(MG_MASSCARDVIDEO_PATH));
    }
    else
    {
        STRCPY(pMe->m_sCaptureFileName, pMe->m_sCurrentFileName+STRLEN(MG_PHONEVIDEOS_PATH));
    }

	/*
	if (HS_HEADSET_ON())
	{
		MSG_FATAL("***zzg HS_HEADSET_ON() TRUE", 0,0,0);
		snd_set_device(SND_DEVICE_HANDSET, SND_MUTE_MUTED, SND_MUTE_MUTED, NULL, NULL);	
		snd_set_device(SND_DEVICE_HEADSET, SND_MUTE_UNMUTED, SND_MUTE_UNMUTED, NULL, NULL);	
	}
	else
	{	
		MSG_FATAL("***zzg HS_HEADSET_ON() FALSE", 0,0,0);
		snd_set_device(SND_DEVICE_HEADSET, SND_MUTE_MUTED, SND_MUTE_MUTED, NULL, NULL);	
		snd_set_device(SND_DEVICE_HANDSET, SND_MUTE_UNMUTED, SND_MUTE_UNMUTED, NULL, NULL);	
	}
	*/
	

	//Add By zzg 2011_12_30		
	//result = ICAMERA_SetAudioEncode(pMe->m_pCamera, AEECLSID_MEDIAAAC, 0);	
	result = ICAMERA_SetAudioEncode(pMe->m_pCamera, AEECLSID_MEDIAQCP, 0);	//MM_QCP_FORMAT_FIXED_12_2_AMR
	MSG_FATAL("***zzg ICAMERA_SetAudioEncode result=%d",result,0,0);
	//Add End

	    
    pMe->m_nCameraState = CAM_STARTINGRECORD;

    result = ICAMERA_SetVideoEncode(pMe->m_pCamera,AEECLSID_MEDIAMPEG4,0);
	MSG_FATAL("ICAMERA_SetVideoEncode result=%d",result,0,0);
    
    // 拍照状态的处理
    result = ICAMERA_RecordMovie(pMe->m_pCamera);
    MSG_FATAL("ICAMERA_RecordMovie result=%d",result,0,0);
    if(SUCCESS != result )
    {
        // 拍照失败,默认保留已经拍照成功的相片,并返回到预览界面,避免UI层出现死机现象
        // Vc848.c中处理过,如果拍照失败,直接删除失败的文件.
        pMe->m_wMsgID = IDS_CAMERA_RECORD_FAILED;
        pMe->m_nMsgTimeout = TIMEOUT_MS_MSGBOX;
        ICAMERA_Stop(pMe->m_pCamera);
        pMe->m_nCameraState = CAM_STOP;
        pMe->m_bRePreview = TRUE;
        CLOSE_DIALOG(DLGRET_POPMSG);        
        return;
    }
#else
	return;
#endif
}

static void CameraApp_RecordSnapShot(CCameraApp *pMe)
{ 
    char sFileName[MIN_PIC_CHAR_NAME_LEN];
    cam_sensor_model_pair_type sensor_type;

    
    MSG_FATAL("CameraApp_RecordSnapShot..........",0,0,0);
    MEMSET(pMe->m_sCurrentFileName, '\0', sizeof(pMe->m_sCurrentFileName));
    MEMSET(sFileName, '\0', sizeof(sFileName));
    
    CameraApp_GetDateForRecordFileName(pMe, pMe->m_sCurrentFileName, FILE_TYPE_JPG);
    CameraApp_SetDateForRecordFileName(pMe,FILE_TYPE_JPG);
   
    // copy the pic name to sFilename buffer
    if ( pMe->m_isStartFromFacebook == TRUE )
    {
		STRCPY(pMe->m_sCaptureFileName, pMe->m_sCurrentFileName+STRLEN(MG_PHONEPICTURE_PATH));
    }
    else
    {
	    if(pMe->m_nCameraStorage == OEMNV_CAMERA_STORAGE_MEMORY_CARD)
	    {
	        STRCPY(pMe->m_sCaptureFileName, pMe->m_sCurrentFileName+STRLEN(MG_MASSCARDPICTURE_PATH));
	    }
	    else
	    {
	        STRCPY(pMe->m_sCaptureFileName, pMe->m_sCurrentFileName+STRLEN(MG_PHONEPICTURE_PATH));
	    }
    }
    
    pMe->m_nCameraState = CAM_SAVE;
    pMe->m_bCapturePic  = TRUE;
#ifndef FEATURE_DSP
    (void)ICAMERA_DeferEncode(pMe->m_pCamera, TRUE);
#endif

#ifdef FEATURE_CAMERA_MULTI_SENSOR

#ifdef FEATURE_CAMERA_MULTI_NEW_AUTO_DETECT
	ICAMERA_GetCurrentSensor(pMe->m_pCamera,&sensor_type,&sensor_type);
#endif

	if ( sensor_type == CAM_PARM_ID_PAIR_SIV121A_SID130B || sensor_type == CAM_PARM_ID_PAIR_SIV121D_SID130B || sensor_type == CAM_PARM_ID_PAIR_SIV120A_SID130B )
	{
		if ( pMe->m_nCameraMulti == OEMNV_CAMERA_MULTI_TWO)
		{
			ICAMERA_RotateEncode(pMe->m_pCamera, 180);
		}
		else if(pMe->m_nCameraMulti == OEMNV_CAMERA_MULTI_ONE)
		{
			ICAMERA_RotateEncode(pMe->m_pCamera, 0);
		}
	}
#endif
    
    
    // 拍照状态的处理
    if(SUCCESS != ICAMERA_RecordSnapshot(pMe->m_pCamera))
    {
        // 拍照失败,默认保留已经拍照成功的相片,并返回到预览界面,避免UI层出现死机现象
        // Vc848.c中处理过,如果拍照失败,直接删除失败的文件.
        pMe->m_wMsgID = IDS_MSG_CAPTURE_FAILED;
        pMe->m_nMsgTimeout = TIMEOUT_MS_MSGBOX;
        ICAMERA_Stop(pMe->m_pCamera);
        pMe->m_nCameraState = CAM_STOP;
        pMe->m_bRePreview = TRUE;
        CLOSE_DIALOG(DLGRET_POPMSG);        
        return;
    }

    pMe->m_bCapturePic = FALSE;
}

static void CameraApp_PicDialogTimeout(void *pme)
{
    CCameraApp *pMe = (CCameraApp *)pme;
        
    if(NULL == pMe)
        return;
    pMe->m_isPicCamera=FALSE;
    (void)ISHELL_CancelTimer(pMe->m_pShell,
                             CameraApp_PicDialogTimeout,
                             pMe);
}

static void CameraApp_DialogTimeout(void *pme)
{
    CCameraApp *pMe = (CCameraApp *)pme;
        
    if(NULL == pMe)
        return;

    (void)ISHELL_PostEvent(pMe->m_pShell,
                           AEECLSID_APP_CAMERA,
                           EVT_APP_DIALOG_TIMEOUT,
                           0,
                           0);
}
static void CameraApp_PrevewTimeout(void *pme)
{
    CCameraApp *pMe = (CCameraApp *)pme;
        
    if(NULL == pMe)
        return;
//#if defined (FEATURE_VERSION_C310) || defined (FEATURE_VERSION_C337)
    pMe->m_bCanPress = TRUE;
//#endif
}

static boolean CameraApp_FindMemoryCardExist(CCameraApp *pMe)
{   
    return (IFILEMGR_Test(pMe->m_pFileMgr, AEEFS_CARD0_DIR)==SUCCESS)?TRUE:FALSE;	
}

// 检测拍照，摄像是否有足够剩余空间容量
static boolean CameraApp_IsEnoughfMemorySpace(CCameraApp * pMe)
{
    if ( pMe->m_isStartFromFacebook == TRUE)
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
	else
	{
	    if(pMe->m_nCameraStorage == OEMNV_CAMERA_STORAGE_MEMORY_CARD)
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
}


// 自拍
static boolean CameraApp_SelfTimeRecordSnapShot(CCameraApp *pMe)
{
    // 倒计时先清空屏幕
    //IDisplay_ClearScreen(pMe->m_pDisplay);

    (void)ISHELL_SendEvent(pMe->m_pShell,
                           AEECLSID_APP_CAMERA,
                           EVT_USER_REDRAW,
                           0,
                           0);
        
    if(pMe->m_nLeftTime  == 0)
    {
        ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY)CameraApp_SelfTimeRecordSnapShot, pMe);

        if(pMe->m_pCamera)
        {
        	#ifdef FEATURE_LCD_TOUCH_ENABLE
			#ifdef FEATURE_CAMERA_MULTI_SENSOR
	   		if(pMe->m_nCameraMulti == OEMNV_CAMERA_MULTI_TWO)
	   		{
	   			if(pMe->n_nCameraFlash == OEMNV_CAMERA_FLASH_ON)
                {
	   				ICAMERA_ControlFlash(pMe->m_pCamera,TRUE);
	   			}
	   		}
			#endif
	   		#endif
            pMe->m_nCameraState = CAM_CAPTURE;
            ICAMERA_Stop(pMe->m_pCamera);
            //CameraApp_RecordSnapShot(pMe);
            #ifdef FEATURE_LCD_TOUCH_ENABLE
			ICAMERA_ControlFlash(pMe->m_pCamera,FALSE);
			#endif
        }
        
        return TRUE;
    }
    else
    {  
        pMe->m_nLeftTime--;
        
        (void)ISHELL_SetTimer(pMe->m_pShell,
                              1000,
                              (PFNNOTIFY)CameraApp_SelfTimeRecordSnapShot,
                              pMe);
    }

    return TRUE;
}

// Draw Bottom Bar Text
static void CameraApp_DrawBottomBarText(CCameraApp *pMe, BottomBar_e_Type eBarType)
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

#ifdef FEATURE_MOVIE_RECORD_SUPPORT
		case BTBAR_SEND_BACK:
			nResID_L = IDS_CAMERA_RECORD_STOP;
            nResID_R = IDS_BACK;
            break;
#endif            
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

static boolean CameraApp_SetDateForRecordFileName(CCameraApp *pMe,unsigned int fileType)
{
    AEEMediaData   md;
    int result = 0;
    //Fill media data
    md.clsData = MMD_FILE_NAME;
    md.pData = (void *)pMe->m_sCurrentFileName;
    md.dwSize = 0;

	if ( fileType == FILE_TYPE_3GP )
    {
    	result = ICAMERA_SetMediaData(pMe->m_pCamera, &md, CAM_FILE_FORMAT_3GP);
	}
	else
	{
		result = ICAMERA_SetMediaData(pMe->m_pCamera, &md, 0);	
	}
	
	MSG_FATAL("CameraApp_SetDateForRecordFileName result=%d",result,0,0);
    return TRUE;
}

static boolean CameraApp_GetDateForRecordFileName(CCameraApp *pMe, char * pszDest, unsigned int fileType)
{
    JulianType julian;
 
    GETJULIANDATE(GETTIMESECONDS(), &julian);
   
    if(pMe->m_nCameraStorage == OEMNV_CAMERA_STORAGE_MEMORY_CARD)
    {
        STRCPY(pszDest, AEEFS_CARD0_DIR);
    }
    else
    {
        STRCPY(pszDest, MG_PHONE_ROOTDIR);
    }
    
    switch(fileType)
    {
    	case FILE_TYPE_3GP:
    		STRCAT(pszDest, FS_CARD_VIDEOS_FOLDER);
		    SPRINTF(pszDest+STRLEN(pszDest), "%02d%02d%02d%02d.3gp", julian.wDay, julian.wHour, julian.wMinute, julian.wSecond);
		    break;
		    
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

static void CameraApp_SetCameraCaptureSize(CCameraApp *pMe, uint16 wParam)
{
    AEESize displaySize, captureSize;
     
    pMe->m_nCameraSize = wParam;

#ifdef FEATURE_CAMERA_MULTI_SENSOR

	if(pMe->m_nCameraMulti == OEMNV_CAMERA_MULTI_ONE)
    { 
        captureSize.cx = g_CameraFrontSizeCFG[pMe->m_nCameraSize].dx;
        captureSize.cy = g_CameraFrontSizeCFG[pMe->m_nCameraSize].dy;
        
        displaySize.cx = 240;
        displaySize.cy = 320;
    }
    else if(pMe->m_nCameraMulti == OEMNV_CAMERA_MULTI_TWO)
    {
		captureSize.cx = g_CameraBackSizeCFG[pMe->m_nCameraSize].dx;
        captureSize.cy = g_CameraBackSizeCFG[pMe->m_nCameraSize].dy;
        
        displaySize.cx = 240;
        displaySize.cy = 320;
    }
	
#else
    if(pMe->m_sensor_model == 30)
    {
        captureSize.cx = g_CameraSizeCFG[pMe->m_nCameraSize].dx;
        captureSize.cy = g_CameraSizeCFG[pMe->m_nCameraSize].dy;
        
        displaySize.cx = g_CameraSizeCFG[0].dx;
        displaySize.cy = g_CameraSizeCFG[0].dy;
    }
    else if(pMe->m_sensor_model == 10)
    {
#ifdef FEATURE_MOVIE_RECORD_SUPPORT
		if ( pMe->m_isRecordMode )
		{
			captureSize.cx = g_VideoSizeCFG_10[pMe->m_nCameraSize].dx;
	        captureSize.cy = g_VideoSizeCFG_10[pMe->m_nCameraSize].dy;
#ifdef T_QSC1110
	        #if defined(FEATURE_DISP_128X160)
	        displaySize.cx = g_VideoSizeCFG_10[1].dx;
	        displaySize.cy = g_VideoSizeCFG_10[1].dy;
	        #else
	        displaySize.cx = SCREEN_WIDTH;
	        displaySize.cy = SCREEN_HEIGHT;
	        #endif
#else
	        displaySize.cx = g_VideoSizeCFG_10[0].dx;
	        displaySize.cy = g_VideoSizeCFG_10[0].dy;
#endif
		}
		else
		{
			captureSize.cx = g_CameraSizeCFG_10[pMe->m_nCameraSize].dx;
	        captureSize.cy = g_CameraSizeCFG_10[pMe->m_nCameraSize].dy;
#ifdef T_QSC1110
	        #if defined(FEATURE_DISP_128X160)
	        displaySize.cx = g_CameraSizeCFG_10[1].dx;
	        displaySize.cy = g_CameraSizeCFG_10[1].dy;
	        #else
			
            #if defined (FEATURE_VERSION_K212)||defined(FEATURE_DISP_240X320)			
			displaySize.cx = 240;
	        displaySize.cy = 320;
			#else
	        displaySize.cx = g_CameraSizeCFG_10[0].dx;
	        displaySize.cy = g_CameraSizeCFG_10[0].dy;
			#endif
	        #endif
#else
	        displaySize.cx = g_CameraSizeCFG_10[0].dx;
	        displaySize.cy = g_CameraSizeCFG_10[0].dy;
#endif
		}
#else
		captureSize.cx = g_CameraSizeCFG_10[pMe->m_nCameraSize].dx;
        captureSize.cy = g_CameraSizeCFG_10[pMe->m_nCameraSize].dy;
        #ifdef T_QSC1110
        #if defined(FEATURE_DISP_128X160)
        displaySize.cx = g_CameraSizeCFG_10[1].dx;
        displaySize.cy = g_CameraSizeCFG_10[1].dy;
        #else
		#if defined (FEATURE_VERSION_K212)||defined(FEATURE_DISP_240X320)
		displaySize.cx = 240;
	    displaySize.cy = 320;
		#else
        displaySize.cx = g_CameraSizeCFG_10[0].dx;
        displaySize.cy = g_CameraSizeCFG_10[0].dy;
		#endif
        #endif
        #else
        displaySize.cx = g_CameraSizeCFG_10[0].dx;
        displaySize.cy = g_CameraSizeCFG_10[0].dy;
        #endif
#endif        
    }
#endif
    (void)ICONFIG_SetItem(pMe->m_pConfig,
                          CFGI_CAMERA_SIZE,
                          &pMe->m_nCameraSize,
                          sizeof(pMe->m_nCameraSize));
 
    ICAMERA_SetSize(pMe->m_pCamera, &captureSize);
    
    ICAMERA_SetDisplaySize(pMe->m_pCamera, &displaySize);
}

static void CameraApp_HandleSnapshotPic(CCameraApp *pMe)
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
            MSG_FATAL("111myInfo.cx=====%d,myInfo.cy===%d",myInfo.cx,myInfo.cy,0);
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
            #ifndef FEATURE_VERSION_K212_HUALU
            x = (myInfo.cx-pMe->m_rc.dx)/2;
            y = (myInfo.cy-pMe->m_rc.dy)/2;
			MSG_FATAL("myInfo.cx=====%d,myInfo.cy===%d",myInfo.cx,myInfo.cy,0);
			MSG_FATAL("x=========%d,y=========%d",x,y,0);
			#ifdef  FEATURE_CAMERA_MULTI_SENSOR
			if(pMe->m_nCameraMulti == OEMNV_CAMERA_MULTI_ONE)
			{
				if(myInfo.cx>(SCREEN_WIDTH+5))
				{
					MSG_FATAL(",..........",0,0,0);
					//x = (myInfo.cx-SCREEN_WIDTH)/2;
					if(myInfo.cx>430)
					{
						IImage_SetParm(pImage,
                        	       IPARM_SCALE,
                            	   285,
                               	   320);
						MSG_FATAL(",.........1111.",0,0,0);
					}
					else
					{
						IImage_SetParm(pImage,
                        	       IPARM_SCALE,
                            	   240,
                               	   320);
						MSG_FATAL(",.........2222.",0,0,0);
						x = 0;
					}
				}
			}
			#endif
			MSG_FATAL("x=========%d,y=========%d",x,y,0);
            IImage_SetOffset(pImage,x,y);
            IImage_Draw(pImage,0,0);
            #else
            x = (240 - myInfo.cx)/2;
            y = (320 - myInfo.cy)/2;
            IImage_Draw(pImage,x,y);
            #endif
           
            //IDISPLAY_BitBlt(pMe->m_pDisplay, pMe->m_rc.x, pMe->m_rc.y, pMe->m_rc.dx, pMe->m_rc.dy, pMe->m_pIBitmap, 0, 0, AEE_RO_COPY);
            IImage_Release(pImage);
        }
    }

    //Add By zzg 2013_03_22   
    pMe->m_bIsSnaping = FALSE;
    pMe->m_bCanPress = TRUE;        
    //Add End
    
    CameraApp_DrawBottomBarText(pMe, BTBAR_SAVE_DELETE);
    
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

// Camera一些设置参数,需要在Preview后设置才能生效
static void CameraApp_SetParamAfterPreview(CCameraApp *pMe)
{
    uint32 dwFPS = 0;

    //set camera enviroment
    switch(pMe->m_nCameraEnviroment)
    {
        case OEMNV_CAMERA_ENVIR_AUTO:
            dwFPS =CAM_WB_AUTO;// 0;
            break;

        case OEMNV_CAMERA_ENVIR_NIGHT:
            dwFPS =CAM_WB_FLUORESCENT;// 3;
            break;

        case OEMNV_CAMERA_ENVIR_SUNNY:
            dwFPS =CAM_WB_DAYLIGHT;// 1;
            break;
        
        case OEMNV_CAMERA_ENVIR_CLOUDY:
            dwFPS =CAM_WB_CLOUDY_DAYLIGHT;//  2;
            break;

        default:
            break;
    }

	MSG_FATAL("pMe->m_sensor_model=%d",pMe->m_sensor_model,0,0);
 	if ( pMe->m_sensor_model == 10)
    {
    	ICAMERA_SetParm(pMe->m_pCamera, CAM_PARM_ROTATE_PREVIEW, 90, 0);
    }
    //ICAMERA_SetFramesPerSecond(pMe->m_pCamera, dwFPS);
    ICAMERA_SetWb(pMe->m_pCamera, dwFPS);
    
    ICAMERA_SetParm(pMe->m_pCamera, CAM_PARM_BANDING, pMe->m_nCameraBanding, 0);
}

/*===========================================================================
FUNCTION CameraApp_UpdateFrame

  DESCRIPTION
    This function updates the display with the frame from the viewfinder.

  PARAMETERS:
    pMe - Pointer to  QCam struct

  RETURN VALUE:
    None
===========================================================================*/
static void CameraApp_UpdateFrame(CCameraApp *pMe)
{
  IBitmap *pFrame = NULL;

  if (!pMe)
    return;
#ifndef FEATURE_DSP
  (void)ICAMERA_GetFrame(pMe->m_pCamera, &pFrame);

#endif
  if (!pFrame)
    return;
  
  // Display the frame at center location of the screen
#ifdef FEATURE_CAMERA_NOFULLSCREEN
  IDISPLAY_FillRect(pMe->m_pDisplay, &pMe->m_rc, RGB_BLACK);
  IDISPLAY_BitBlt(pMe->m_pDisplay, pMe->m_rcPreview.x, pMe->m_rcPreview.y, pMe->m_rcPreview.dx, pMe->m_rcPreview.dy, pFrame, 0, 0, AEE_RO_COPY);
#else
  IDISPLAY_BitBlt(pMe->m_pDisplay, pMe->m_rc.x, pMe->m_rc.y, pMe->m_rc.dx, pMe->m_rc.dy, pFrame, 0, 0, AEE_RO_COPY);
#endif
  RELEASEIF(pFrame);
}
#ifdef FEATURE_DSP
static int CameraApp_UpdateInit(CCameraApp *pMe)
{
	IBitmap* pbmp = NULL;
    IDISPLAY_GetDeviceBitmap(pMe->m_pDisplay, &pbmp);
    if(pMe->m_nCameraState == CAM_PREVIEW)
    {
        IBITMAP_FillRect(pbmp, &pMe->m_rc, TRANS_COLOR,AEE_RO_COPY);//,
        IBITMAP_Release(pbmp);  
        //DrawRect(pMe->m_pDisplay, &pMe->m_rc, TRANS_COLOR,AEE_RO_COPY);
        return SUCCESS;
    }
    else
    {
        return EBADSTATE;
    }
        
}

static int CameraApp_Update(CCameraApp *pMe)
{
    //DBGPRINTF("camera state--------------%d\n", pMe->m_nCameraState);
    if(pMe->m_nCameraState == CAM_PREVIEW)
    {
        IBitmap* pbmp = NULL;
        IDIB* pdib = NULL;
        IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE); 
        IDISPLAY_GetDeviceBitmap(pMe->m_pDisplay, &pbmp);           
        IBITMAP_QueryInterface(pbmp, AEECLSID_DIB, (void**)&pdib);                
        ICAMERA_UpdateScreen(pMe->m_pCamera, (uint32)pdib->pBmp);        
        DBGPRINTF("bmp addr:%0x--- width:%d-----height:%d------depth:%d\n", pdib->pBmp, pdib->cx, pdib->cy, pdib->nDepth);
        IBITMAP_Release(pbmp);  
        IDIB_Release(pdib);
    }    
    else
    {
        return EBADSTATE;
    }
}
#endif
/*===========================================================================
FUNCTION CameraApp_SavePhoto

  DESCRIPTION
    This function starts the encoding processing to save a new photo to the file.

  PARAMETERS:
    pMe - pointer to QCam data struct

  RETURN VALUE:
    NONE
===========================================================================*/
static int CameraApp_SavePhoto(CCameraApp *pMe)
{
	AEEMediaData   md;

	MSG_FATAL("CameraApp_SavePhoto!!!!!",0,0,0);
	if (!pMe)
		return EBADPARM;

	//Fill media data
	md.clsData = MMD_FILE_NAME;
	md.pData = (void *)pMe->m_sCurrentFileName;
	md.dwSize = 0;

	//Start encoding processing
	(void)ICAMERA_SetMediaData(pMe->m_pCamera, &md, 0);
#ifndef FEATURE_VERSION_C01
	ICAMERA_SetVideoEncode(pMe->m_pCamera, AEECLSID_JPEG, 0);

		
	ICAMERA_DeferEncode(pMe->m_pCamera, TRUE);
#endif			
  	return ICAMERA_EncodeSnapshot(pMe->m_pCamera);
} /* END CameraApp_SavePhoto */

static void CameraApp_EventNotify(CCameraApp *pMe, AEECameraNotify *pcn)
{
    if (!pMe || !pcn || !pMe->m_pCamera)
        return;
    
    switch (pcn->nCmd){
    case CAM_CMD_START:
        switch (pcn->nStatus){
        case CAM_STATUS_FRAME:
            DBGPRINTF("CameraApp_EventNotify Frame");
            //It is the image from the viewfinder.
            CameraApp_UpdateFrame(pMe);
            ISHELL_PostEvent(pMe->m_pShell, AEECLSID_APP_CAMERA, EVT_USER_REDRAW, NULL, NULL);
            break;
            
        default:
            ISHELL_PostEvent(pMe->m_pShell, AEECLSID_APP_CAMERA, EVT_CAMERA_NOTIFY, pcn->nCmd, pcn->nStatus);
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
        ISHELL_PostEvent(pMe->m_pShell, AEECLSID_APP_CAMERA, EVT_CAMERA_NOTIFY, pcn->nCmd, pcn->nStatus);
        break;
    
    default:
        break;
    }
}

void CameraApp_InitCameraCheck(void *po)
{
    CCameraApp *pMe = (CCameraApp *)po; 
    CameraApp_CreateDirectory(pMe);
	MSG_FATAL("CameraApp_InitCameraCheck.................",0,0,0);
	#ifdef FEATURE_CAMERA_MULTI_SENSOR
	(void)ICONFIG_GetItem(pMe->m_pConfig,
                                  CFGI_CAMERA_MULTI,
                                  &pMe->m_nCameraMulti,
                                  sizeof(pMe->m_nCameraMulti));
    #endif
    if(pMe->m_pCamera == NULL)
    {
    	MSG_FATAL("AEECLSID_CAMERA create.....m_pCamera....",0,0,0);
		#ifdef FEATURE_CAMERA_MULTI_SENSOR
		MSG_FATAL("pMe->m_nCameraMulti=============%d",pMe->m_nCameraMulti,0,0);
		switch(pMe->m_nCameraMulti)
			{
				case OEMNV_CAMERA_MULTI_ONE:
        			ISHELL_CreateInstance(pMe->m_pShell, 
                              AEECLSID_CAMERA1, 
                              (void **)&pMe->m_pCamera);
				break;
				case OEMNV_CAMERA_MULTI_TWO:
					ISHELL_CreateInstance(pMe->m_pShell, 
                              AEECLSID_CAMERA2, 
                              (void **)&pMe->m_pCamera);
				break;
				default:
					break;
			}
		#else
		ISHELL_CreateInstance(pMe->m_pShell, 
                              AEECLSID_CAMERA, 
                              (void **)&pMe->m_pCamera);
		#endif
    }
    if(pMe->m_pCamera != NULL)
    {
#ifndef FEATURE_DSP
        MSG_FATAL("pMe->m_pCamera != NULL",0,0,0);
        ICAMERA_GetParm(pMe->m_pCamera, CAM_PARM_SENSOR_MODEL, &(pMe->m_sensor_model), NULL);
        MSG_FATAL("m_sensor_model=%d",pMe->m_sensor_model,0,0); 
#endif
    }
    else
    {
        MSG_FATAL("pMe->m_pCamera == NULL",0,0,0);
    }
    
    if(pMe->m_pCamera)
    {
#ifdef FEATURE_DSP
         ICAMERA_Preview(pMe->m_pCamera);
#else
		 MSG_FATAL("pMe->m_pCamera != NULL.................",0,0,0);
         ICAMERA_RegisterNotify(pMe->m_pCamera,(PFNCAMERANOTIFY)CameraApp_EventNotify,po);
#endif
    }
}

void CameraApp_AppEventNotify(CCameraApp *pMe, int16 nCmd, int16 nStatus)
{
    if (!pMe || !pMe->m_pCamera)
        return;

    MSG_FATAL("CameraApp_AppEventNotify nCmd=%d,nStatus=%d",nCmd,nStatus,0);
    switch (nCmd){
    case CAM_CMD_START:
        switch (nStatus)
        {
	        case CAM_STATUS_START:
	        	
	        	if ( pMe->m_nCameraState == CAM_STARTINGRECORD)
	        	{

	        		MSG_FATAL("!!!!!!!!!!! Start recording!!!",0,0,0);
					pMe->m_nCameraState = CAM_RECORDING;
					pMe->RecordingTime_parm.RecordTotalTime = 0;
					pMe->RecordingTime_parm.RecordStartTime = GETTIMESECONDS();
					pMe->RecordingTime_parm.LastTime = pMe->RecordingTime_parm.RecordStartTime;

					//upgrade bottom bar here
					ISHELL_SetTimer(pMe->m_pShell, 700, (PFNNOTIFY)CameraApp_OnRecordTimeFeedBack, pMe);
	        	}
#ifdef FEATURE_CAMERA_NOFULLSCREEN
	            IDISPLAY_FillRect(pMe->m_pDisplay, &pMe->m_rc, RGB_BLACK);
	            IDISPLAY_Update(pMe->m_pDisplay);
#endif
	            break;
	            
	        case CAM_STATUS_DONE:
	            if(pMe->m_nCameraState == CAM_CAPTURE)
	            {
	                CameraApp_RecordSnapShot(pMe);
	            }
	            else if(pMe->m_nCameraState == CAM_SAVE)
	            {
	                CameraApp_SavePhoto(pMe);
	                //CLOSE_DIALOG(DLGRET_PICMENU);
	            }
	            else if(pMe->m_nCameraState == CAM_STARTINGRECORD)
	            {
					//
	            }
	            
	            break;

	        case CAM_STATUS_ABORT:
	        case CAM_STATUS_FAIL:
	            if(pMe->m_nCameraState == CAM_CAPTURE || pMe->m_nCameraState == CAM_SAVE )
	            {
	                pMe->m_bRePreview = TRUE;
	                pMe->m_wMsgID = IDS_MSG_CAPTURE_FAILED;
	                pMe->m_nMsgTimeout = TIMEOUT_MS_MSGDONE;
	                CLOSE_DIALOG(DLGRET_POPMSG);	                
	            }
	        #ifdef FEATURE_MOVIE_RECORD_SUPPORT
	            else if(pMe->m_nCameraState == CAM_STARTINGRECORD)
	            {
	                pMe->m_bRePreview = TRUE;
	                pMe->m_wMsgID = IDS_CAMERA_RECORD_FAILED;
	                pMe->m_nMsgTimeout = TIMEOUT_MS_MSGDONE;
	                CLOSE_DIALOG(DLGRET_POPMSG);	                
	            }
	        #endif
	            break;
	        case CAM_STATUS_SPACE_ERROR:
                {
                    pMe->m_wMsgID = IDS_MSG_NOMEMORY;
    	            pMe->m_nMsgTimeout = TIMEOUT_MS_MSGBOX;
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
            if(pMe->m_nCameraState == CAM_SAVE)
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
                      &pMe->m_nCameraTone,
                      sizeof(pMe->m_nCameraTone));

#ifdef FEATURE_CARRIER_CHINA_TELCOM
{
            	CameraApp_PlayShutterSound(pMe);
}
#else
            if(pMe->m_nCameraTone == OEMNV_CAMERA_SHUTTER_TONE_ENABLE)
            {
            		#ifdef FEATURE_SOUND_BO
                    /*
					nv_item_type	SimChoice;
            		(void)OEMNV_Get(NV_SIM_SELECT_I,&SimChoice);
					if(SimChoice.sim_select != 1)
                    */    
                    if (GetMp3PlayerStatus() == MP3STATUS_NONE)
					{
                		CameraApp_PlayShutterSound(pMe);
					}
					#else
					CameraApp_PlayShutterSound(pMe);
					#endif
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
static void CameraApp_CreateDirectory(CCameraApp *pMe)
{
    // 手机上的相片保存目录
    if(SUCCESS != IFILEMGR_Test(pMe->m_pFileMgr, MG_PHONEPICTURE_PATH))
    {
        IFILEMGR_MkDir(pMe->m_pFileMgr, MG_PHONEPICTURE_PATH);
    }
	
    pMe->m_bMemoryCardExist = CameraApp_FindMemoryCardExist(pMe);	

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

