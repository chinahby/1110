/*==============================================================================
// 文件：
//        SoundMenuDialogHandler.c
//        版权所有(c) 2009 Anylook Tech. CO.,LTD.
//
// 文件说明：
//
// 作者：Gemsea
// 创建日期：2007-11-01
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时       间      修 改 人    问题单号  修改内容、位置及原因
-----------      ----------  --------  -----------------------------------------
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "SoundMenu_priv.h"
#ifndef WIN32
#include "Snddev.h"
#include "Sndid.h"
#endif//WIN32
#ifdef FEATURE_APP_MUSICPLAYER
#include "MusicPlayer.h"
#endif 
#include "appscommonimages.brh"

#ifdef FEATURE_APP_MEDIAGALLERY
#include "MediaGallery.h"
#endif

//#include "uixsnd.h"


/*==============================================================================
                                 宏定义和常数
==============================================================================*/
// 关闭对话框宏
#define CLOSE_DIALOG(DlgRet)  {    \
                                     pMe->m_eDlgRet = DlgRet; \
                                     (void) ISHELL_EndDialog(pMe->m_pShell);  \
                                 }


/*==============================================================================
                                 类型定义
==============================================================================*/


/*==============================================================================
                                 函数声明
==============================================================================*/
// 对话框 IDD_SCENEMODE_MENU 事件处理函数
static boolean  HandleMainDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_SOUNDMENU_MENU 事件处理函数
//static boolean  HandleSoundMenuDialogEvent(CSoundMenu *pMe,
//    AEEEvent eCode,
//    uint16 wParam,
//    uint32 dwParam
//);

// 对话框 IDD_SOUNDMENU_PROFILES 事件处理函数
static boolean  HandleSoundMenuProfilesDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_SCENEMODE_SUB 事件处理函数
static boolean  HandleSceneModeSubDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_HINT_MENU 事件处理函数
static boolean  HandleHintDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_RINGER_MENU 事件处理函数
static boolean  HandleRingerDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
// 对话框 IDD_SMSRING_MENU 事件处理函数
static boolean  HandleSmsRingDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#endif

// 对话框 IDD_OTHERSEL_MENU 事件处理函数
static boolean  HandleOtherSelDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_FMMODE_MENU 事件处理函数
static boolean  HandleFMModeDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);



//#ifdef KEYSND_ZY
//// 对话框 IDD_KEYSND_MENU 事件处理函数
static boolean  HandleKeySndMenuDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
//#endif //KEYSND_ZY

// 对话框 IDD_KEYTONE_LENGTH 事件处理函数
//static boolean  HandleKeyToneLengthDialogEvent(CSoundMenu *pMe,
//    AEEEvent eCode,
//    uint16 wParam,
//    uint32 dwParam
//);
#if 0
// 对话框 IDD_AUTOANSWER_SUB 事件处理函数
static boolean  HandleAutoAnswerSubDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#endif
// 对话框 IDD_VOLUME_MENU 事件处理函数
static boolean  HandleVolumeDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_VOLUME_SUB 事件处理函数
static boolean  HandleVolumeSubDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_WARNING_MESSEGE 事件处理函数
static boolean  HandleWarningMessegeDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#ifdef FEATURE_CS08
static boolean  HandleKeyVolumeDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);
#endif

#ifdef CUST_EDITION
#ifndef MG_MAX_FILE_NAME            
#define MG_MAX_FILE_NAME            128
#endif
typedef char (*FileNamesBuf)[MG_MAX_FILE_NAME];

#ifndef MEDIAGALLERY_H
typedef struct _ExplorerPara{
   /*下面几项为MMS选择文件使用*/
   AEECLSID    nClsId; /*the applet which invoke the interface class ID */
   const char* cpszPath;/*the can set to NULL by default*/
   int         nFileType; /*Please refer to File mime classfication up*/
   int         nFileMaxSize;/*the maximum size of file which listed*/

   /*下面项为添加音频文件时，所使用的*/
   uint16      nSelNum;/*maximum file number to select*/

   /*下面为情景模式调用设置铃声时使用，因为当前模式不一定是情景模式中编辑设置
    * 项的模式*/
   byte        nSelectProfile;
}ExplorerPara;
#endif
#endif

// 对话框 IDD_MESSAGE_DIALOG 事件处理函数
//static boolean  HandleMessageDialogEvent(CSoundMenu *pMe,
//    AEEEvent      eCode,
//    uint16        wParam,
//    uint32        dwParam
//);

//启动情景模式
static void SoundMenu_SceneModeActivate(CSoundMenu *pMe);

// 提示信息回调
static void SoundMenu_DialogTimeout(void *pme);

//浏览铃声add by lzhl 20011014
static void RingerPreview(void *pUser);

static void VolumePreview(void *pUser);

//static void SoundMenu_AutoScroll(IMenuCtl  *pMenu, AEEEvent eCode,   uint16 wParam);

static void SoundMenu_SetItemNumIcon(IMenuCtl   *pMenu);

//static void SoundMenu_StartRingerPreview(void *pUser,uint16 ring_id);

static void Sound_App_Add_Menu(IMenuCtl *pMenu,uint16 id);
static void notifyFMRadioAlertEvent( CSoundMenu *pMe, boolean toStartAlert);

static void ProfileNotifyMP3PlayerAlertEvent(CSoundMenu *pMe, boolean toStartAlert);
int SoundMenu_SetMp3ring(void* pv, FileNamesBuf pBuf, uint32 nBufSize);

/*==============================================================================
                                 全局数据
==============================================================================*/

/*==============================================================================
                                 本地（静态）数据
==============================================================================*/


/*==============================================================================
                                 函数定义
==============================================================================*/
/*==============================================================================
函数:
       SoundMenu_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向SoundMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:


==============================================================================*/
void SoundMenu_ShowDialog(CSoundMenu *pMe,uint16  dlgResId)
{
    int nRet;
    //SOUND_ERR("next show %d dialog", dlgResId, 0, 0);
    // At most one dialog open at once
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // Looks like there is one dialog already opened.
        // Flag an error an return without doing anything.
        ERR("Trying to create a dialog without closing the previous one",0,0,0);
        return;
    }

    nRet = ISHELL_CreateDialog(pMe->m_pShell,AEE_APPSSOUNDMENU_RES_FILE,dlgResId,NULL);

    if (nRet != SUCCESS)
    {
        ERR("Failed to create the dialog in the SoundMenu applet",0,0,0);
    }
}

/*==============================================================================
函数:
       SoundMenu_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向SoundMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean SoundMenu_RouteDialogEvent(CSoundMenu *pMe,
    AEEEvent    eCode,
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
    //SOUND_ERR("%d the active dialog ID",pMe->m_pActiveDlgID,0,0);
    switch (pMe->m_pActiveDlgID)
    {
        case IDD_SCENEMODE_MENU:
            return HandleMainDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_HINT_MENU:
            return HandleHintDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_RINGER_MENU:
            return HandleRingerDialogEvent(pMe,eCode,wParam,dwParam);

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
        case IDD_SMSRING_MENU:
            return HandleSmsRingDialogEvent(pMe,eCode,wParam,dwParam);
#endif

        case IDD_VOLUME_MENU:
            return HandleVolumeDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_OTHERSEL_MENU:
            return HandleOtherSelDialogEvent(pMe,eCode,wParam,dwParam);

		case IDD_FM_MENU:
			return HandleFMModeDialogEvent(pMe,eCode,wParam,dwParam);
        case IDD_VOLUME_SUB:
            return HandleVolumeSubDialogEvent(pMe,eCode,wParam,dwParam);

//#ifdef KEYSND_ZY
        case IDD_KEYSND_MENU:
            return HandleKeySndMenuDialogEvent(pMe,eCode,wParam,dwParam);
//#endif //KEYSND_ZY

        //case IDD_KEYTONE_LENGTH:
        //    return HandleKeyToneLengthDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_WARNING_MESSEGE:
            return HandleWarningMessegeDialogEvent(pMe,eCode,wParam,dwParam);
#if 0
        case IDD_AUTOANSWER_SUB:
            return HandleAutoAnswerSubDialogEvent(pMe,eCode,wParam,dwParam);
#endif
        //case IDD_SOUNDMENU_MENU:
        //    return HandleSoundMenuDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_SOUNDMENU_PROFILES:
            return HandleSoundMenuProfilesDialogEvent(pMe,eCode,wParam,dwParam);

        case IDD_SCENEMODE_SUB:
            return HandleSceneModeSubDialogEvent(pMe,eCode,wParam,dwParam);

        //case IDD_MESSAGE_DIALOG:
        //    return HandleMessageDialogEvent(pMe,eCode,wParam,dwParam);

        default:
            return FALSE;
    }
}


/*==============================================================================
函数：
       HandleMainDialogEvent
说明：
       IDD_SCENEMODE_MENU对话框事件处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleMainDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_SCENEMODE);

    //uisnd_notify_data_s_type sndInfo;   //Add By zzg 2013_01_10
    
    if (pMenu == NULL)
    {
        return FALSE;
    }
    SOUND_ERR("%x,%x,%x,HandleMainDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    //SoundMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSOUNDMENU_RES_FILE,                                
                        IDS_SCENEMODE_TITLE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            Sound_App_Add_Menu(pMenu,IDS_NORMALMODE);
            Sound_App_Add_Menu(pMenu,IDS_QUIETMODE);
            Sound_App_Add_Menu(pMenu,IDS_MEETING);
            Sound_App_Add_Menu(pMenu,IDS_NOISEMODE);
/*            
#ifdef FEATURE_VERSION_C316
            Sound_App_Add_Menu(pMenu,IDS_PROFILE_BLUETOOTH);
#endif
*/

#if 0          
            Sound_App_Add_Menu(pMenu,IDS_CARMODE);
#endif //#if defined FEATURE_CARRIER_THAILAND_HUTCH  
            pMe->m_nSubDlgId = IDS_ACTIVATE;
            return TRUE;

        case EVT_DIALOG_START:
            MSG_FATAL("pMe->m_CurProfile=========%d",pMe->m_CurProfile,0,0);
            switch(pMe->m_CurProfile)
            {
                case OEMNV_PROFILE_NORMALMODE:          //正常模式
                default:
                    pMe->m_currDlgId = IDS_NORMALMODE;
                    break;

                case OEMNV_PROFILE_QUIETMODE:           //安静模式
                    pMe->m_currDlgId = IDS_QUIETMODE;
                    break;

                case OEMNV_PROFILE_MEETING:             //会议模式
                    pMe->m_currDlgId = IDS_MEETING;
                    break;
                case OEMNV_PROFILE_NOISEMODE:           //户外模式
                    pMe->m_currDlgId = IDS_NOISEMODE;
                    break;
/*
#ifdef FEATURE_VERSION_C316
                case OEMNV_PROFILE_BLUETOOTH:           //蓝牙模式
                    pMe->m_currDlgId = IDS_PROFILE_BLUETOOTH;
                    break;
#endif
*/

                case OEMNV_PROFILE_CARMODE:             //车载模式
                    pMe->m_currDlgId = IDS_CARMODE;
                    break;
            }

            // 给菜单各菜单项加数字编号图标
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_OPTION_BACK);
            IMENUCTL_SetOemProperties( pMenu , OEMMP_DISTINGUISH_INFOKEY_SELECTKEY | OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
#ifdef FEATURE_VERSION_C337
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW|MP_ICON_ANIMATED);
#else
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
#endif

            IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_APP_SOUNDMENU,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;

        case EVT_USER_REDRAW:
            //(void)IMENUCTL_Redraw(pMenu);  //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:            
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;
                case AVK_INFO:         
/*                                       
                	uisnd_get_device(&sndInfo);
                	MSG_FATAL("***zzg UseBTDevice - dev=%d sMute=%d mMute=%d***", 
                	  			sndInfo.out_device, sndInfo.speaker_mute, sndInfo.microphone_mute);
#ifdef FEATURE_SUPPORT_BT_AUDIO
                	if ((SND_DEVICE_BT_HEADSET == sndInfo.out_device) || (SND_DEVICE_BT_A2DP_HEADSET == sndInfo.out_device))
                	{
                	    pMe->m_bBTHeadSetConnected = TRUE;     
                	}
                	else
                	{
                		pMe->m_bBTHeadSetConnected = FALSE; 
                	}
                	//Add End
#endif
*/

                    pMe->m_currDlgId = IMENUCTL_GetSel(pMenu);

                    switch (pMe->m_currDlgId )
                    {
                        case IDS_NORMALMODE:        //正常模式
/*                        
#ifdef FEATURE_VERSION_C316                       
                            if (pMe->m_bBTHeadSetConnected == FALSE)
#endif    
*/
                            {
                                pMe->m_ProfileType = SET_NORMALMODE;
                                pMe->m_CurProfile=OEMNV_PROFILE_NORMALMODE;
                                pMe->m_ScenemodesubType = SET_ACTIVATE;
                                pMe->m_sSubDlgId = IDS_ACTIVATING;                           
                                CLOSE_DIALOG(DLGRET_MESSAGE)
                            }                            
                            break;
                             
                        case IDS_QUIETMODE:         //安静模式
/*
#ifdef FEATURE_VERSION_C316                       
                            if (pMe->m_bBTHeadSetConnected == FALSE)
#endif     
*/
                            {
                                pMe->m_ProfileType = SET_QUIETMODE;
                                pMe->m_CurProfile=OEMNV_PROFILE_QUIETMODE;
                                pMe->m_ScenemodesubType = SET_ACTIVATE;                          
                                pMe->m_sSubDlgId = IDS_ACTIVATING;                           
                                CLOSE_DIALOG(DLGRET_MESSAGE)
                            }
                            break;
                        case IDS_MEETING:           //会议模式
/*                        
#ifdef FEATURE_VERSION_C316                       
                            if (pMe->m_bBTHeadSetConnected == FALSE)
#endif     
*/
                            {
                                pMe->m_ProfileType = SET_MEETING;
                                pMe->m_CurProfile=OEMNV_PROFILE_MEETING;
                                pMe->m_ScenemodesubType = SET_ACTIVATE;                          
                                pMe->m_sSubDlgId = IDS_ACTIVATING;                           
                                CLOSE_DIALOG(DLGRET_MESSAGE)
                            }
                            break;
                        case IDS_NOISEMODE:        //户外模式
/*
#ifdef FEATURE_VERSION_C316                       
                            if (pMe->m_bBTHeadSetConnected == FALSE)
#endif   
*/
                            {
                                pMe->m_ProfileType = SET_NOISEMODE;
                                pMe->m_CurProfile=OEMNV_PROFILE_NOISEMODE;
                                pMe->m_ScenemodesubType = SET_ACTIVATE;                          
                                pMe->m_sSubDlgId = IDS_ACTIVATING;                           
                                CLOSE_DIALOG(DLGRET_MESSAGE)
                            }
                            break;
/*
#ifdef FEATURE_VERSION_C316
                        case IDS_PROFILE_BLUETOOTH:        //户外模式                        
                        {      
                            if (pMe->m_bBTHeadSetConnected == TRUE)   //蓝牙已连接
                            {
                                pMe->m_ProfileType = SET_BLUETOOTH;
                                pMe->m_CurProfile=OEMNV_PROFILE_BLUETOOTH;
                                pMe->m_ScenemodesubType = SET_ACTIVATE;                          
                                pMe->m_sSubDlgId = IDS_ACTIVATING;                           
                                CLOSE_DIALOG(DLGRET_MESSAGE)
                            }
                            else
                            {
                                pMe->m_sSubDlgId = IDS_WARNING_NO_BT_HEADSET;
                                CLOSE_DIALOG(DLGRET_MESSAGE)
                            }
                            break;    
                        }
#endif       
*/
                        default:
                            ASSERT_NOT_REACHABLE;                           
                    }
                    
                    return TRUE;

                default:
                    break;               
             }
            return TRUE;
        case EVT_COMMAND:
            //pMe->m_currDlgId = IMENUCTL_GetSel(pMenu);
            pMe->m_currDlgId = wParam;
            switch (wParam)
            {
                case IDS_NORMALMODE:        //正常模式
                     pMe->m_ProfileType = SET_NORMALMODE;
                     pMe->m_CurProfile=OEMNV_PROFILE_NORMALMODE;
                     CLOSE_DIALOG(DLGRET_SCENEMODESUB)
                     break;

                     
                case IDS_QUIETMODE:         //安静模式
                     pMe->m_ProfileType = SET_QUIETMODE;
                     pMe->m_CurProfile=OEMNV_PROFILE_QUIETMODE;
                     CLOSE_DIALOG(DLGRET_SCENEMODESUB)
                     break;
                case IDS_MEETING:           //会议模式
                     pMe->m_ProfileType = SET_MEETING;
                     pMe->m_CurProfile=OEMNV_PROFILE_MEETING;
                     CLOSE_DIALOG(DLGRET_SCENEMODESUB)
                     break;
                case IDS_NOISEMODE:        //户外模式
                     pMe->m_ProfileType = SET_NOISEMODE;
                     pMe->m_CurProfile=OEMNV_PROFILE_NOISEMODE;
                     CLOSE_DIALOG(DLGRET_SCENEMODESUB)
                     break;

                case IDS_CARMODE:          //车载模式
                     pMe->m_ProfileType = SET_CARMODE;
                     pMe->m_CurProfile=OEMNV_PROFILE_CARMODE;
                     CLOSE_DIALOG(DLGRET_SCENEMODESUB)
                     break;
/*                     
#ifdef FEATURE_VERSION_C316
                case IDS_PROFILE_BLUETOOTH:          //蓝牙模式
                     pMe->m_ProfileType = SET_BLUETOOTH;
                     pMe->m_CurProfile=OEMNV_PROFILE_BLUETOOTH;
                     CLOSE_DIALOG(DLGRET_SCENEMODESUB)
                     break;
#endif
*/

                default:
                     ASSERT_NOT_REACHABLE;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // HandleSceneModeDialogEvent


/*==============================================================================
函数：
       HandleSceneModeSubDialogEvent
说明：
       SOUNDMENUST_SCENEMODESUB 对话框事件处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleSceneModeSubDialogEvent(CSoundMenu *pMe,
    AEEEvent       eCode,
    uint16         wParam,
    uint32         dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_SCENEMODE_SUB);

    //uisnd_notify_data_s_type sndInfo;       //Add By zzg 2013_01_10

    if (pMenu == NULL)
    {
        return FALSE;
    }
    SOUND_ERR("%x,%x,%x,HandleSceneModeSubDialogEvent",eCode,wParam,dwParam);
     //实现菜单循环滚动功能
    //SoundMenu_AutoScroll(pMenu,eCode,wParam);
#if 0    
    switch(pMe->m_CurProfile)
    {
        case OEMNV_PROFILE_NORMALMODE:
            (void)IMENUCTL_SetTitle(pMenu,
                              AEE_APPSSOUNDMENU_RES_FILE,
                              IDS_NORMALMODE,
                              NULL);
            break;
            
        case OEMNV_PROFILE_QUIETMODE:
           (void)IMENUCTL_SetTitle(pMenu,
                              AEE_APPSSOUNDMENU_RES_FILE,
                              IDS_QUIETMODE,
                              NULL);
           break;
           
        case OEMNV_PROFILE_MEETING:
           (void)IMENUCTL_SetTitle(pMenu,
                                  AEE_APPSSOUNDMENU_RES_FILE,
                                  IDS_MEETING,
                                  NULL);    
           break;
           
       case OEMNV_PROFILE_NOISEMODE:
           (void)IMENUCTL_SetTitle(pMenu,
                                  AEE_APPSSOUNDMENU_RES_FILE,
                                  IDS_NOISEMODE,
                                  NULL);    
           break;
           
       case OEMNV_PROFILE_CARMODE:
           (void)IMENUCTL_SetTitle(pMenu,
                                  AEE_APPSSOUNDMENU_RES_FILE,
                                  IDS_CARMODE,
                                  NULL);    
           break;
           
       default:
           break;
    }
#endif

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            Sound_App_Add_Menu(pMenu,IDS_ACTIVATE);
			#ifdef FEATURE_VERSION_K212
			if((pMe->m_CurProfile!=OEMNV_PROFILE_MEETING) && (pMe->m_CurProfile!=OEMNV_PROFILE_QUIETMODE))
			{
				Sound_App_Add_Menu(pMenu,IDS_PERSONALISE);
			}
			#else
			Sound_App_Add_Menu(pMenu,IDS_PERSONALISE);
			#endif
            Sound_App_Add_Menu(pMenu,IDS_PROFILE_RESTORE);
            return TRUE;

        case EVT_DIALOG_START:
            //设定标题格式
            IMENUCTL_SetPopMenuRect(pMenu);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);

            IMENUCTL_SetSel(pMenu, pMe->m_nSubDlgId);

            // 给菜单各菜单项加数字编号图标
            SoundMenu_SetItemNumIcon(pMenu);

            (void) ISHELL_PostEvent(pMe->m_pShell,
                                   AEECLSID_APP_SOUNDMENU,
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
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }

            return TRUE;

        case EVT_COMMAND:            
            pMe->m_nSubDlgId = IMENUCTL_GetSel(pMenu);            
            switch(wParam)
            {
                case IDS_ACTIVATE:         //启动            
/*            
                	uisnd_get_device(&sndInfo);
                	MSG_FATAL("***zzg UseBTDevice - dev=%d sMute=%d mMute=%d***", 
                	  			sndInfo.out_device, sndInfo.speaker_mute, sndInfo.microphone_mute);
#ifdef FEATURE_SUPPORT_BT_AUDIO
                	if ((SND_DEVICE_BT_HEADSET == sndInfo.out_device) || (SND_DEVICE_BT_A2DP_HEADSET == sndInfo.out_device))
                	{
                	    pMe->m_bBTHeadSetConnected = TRUE;     
                	}
                	else
                	{
                		pMe->m_bBTHeadSetConnected = FALSE; 
                	}
                	//Add End
#endif
*/


/*
#ifdef FEATURE_VERSION_C316
                    if (pMe->m_ProfileType == SET_BLUETOOTH)
                    {           
                        if (pMe->m_bBTHeadSetConnected == TRUE)   //蓝牙已连接
                        {
                            pMe->m_ScenemodesubType = SET_ACTIVATE;
                            pMe->m_sSubDlgId = IDS_ACTIVATING;
                            CLOSE_DIALOG(DLGRET_MESSAGE)
                        }
                        else
                        {
                            pMe->m_sSubDlgId = IDS_WARNING_NO_BT_HEADSET;
                            CLOSE_DIALOG(DLGRET_MESSAGE)
                        }
                    }
                    else
                    {
                        if (pMe->m_bBTHeadSetConnected == FALSE) 
                        {
                            pMe->m_ScenemodesubType = SET_ACTIVATE;
                            pMe->m_sSubDlgId = IDS_ACTIVATING;
                            CLOSE_DIALOG(DLGRET_MESSAGE)
                        }                        
                    }
#else   
*/
                    pMe->m_ScenemodesubType = SET_ACTIVATE;
                    pMe->m_sSubDlgId = IDS_ACTIVATING;
                    CLOSE_DIALOG(DLGRET_MESSAGE)
//#endif                    
                    break;

                case IDS_PERSONALISE:      // 编辑
                    CLOSE_DIALOG(DLGRET_SOUNDMENU)
                    break;

                case IDS_PROFILE_RESTORE:  //恢复默认设置
                    pMe->m_ScenemodesubType = SET_PROFILE_RESTORE;
                    pMe->m_sSubDlgId = IDS_RESTORED;
                    CLOSE_DIALOG(DLGRET_MESSAGE)
                    break;

                default:
                    ASSERT_NOT_REACHABLE;
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

				if(SOUNDMENU_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_SOUNDMENU,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_SOUNDMENU,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_SOUNDMENU,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif//FEATURE_LCD_TOUCH_ENABLE 
        default:
            break;
    }
    return FALSE;
} // HandleSceneModeSubDialogEvent


/*==============================================================================
函数：
       HandleSoundMenuProfilesDialogEvent
说明：
       IDD_SOUNDMENU_PROFILES对话框事件处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleSoundMenuProfilesDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_SOUNDMENU_PROFILES);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    SOUND_ERR("%x,%x,%x,HandleSoundMenuProfilesDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    //SoundMenu_AutoScroll(pMenu,eCode,wParam);
    switch(pMe->m_CurProfile)
    {
        case OEMNV_PROFILE_NORMALMODE:
			#if 0
            (void)IMENUCTL_SetTitle(pMenu,
                                    AEE_APPSSOUNDMENU_RES_FILE,
                                    IDS_NORMALMODE,
                                    NULL);
			#else
		    {
		  		AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSOUNDMENU_RES_FILE,                                
                        IDS_NORMALMODE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
		    }
		    #endif
            break;
        
        case OEMNV_PROFILE_QUIETMODE:
			#if 0
            (void)IMENUCTL_SetTitle(pMenu,
                                    AEE_APPSSOUNDMENU_RES_FILE,
                                    IDS_QUIETMODE,
                                    NULL); 
			#else
		    {
		  		AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSOUNDMENU_RES_FILE,                                
                        IDS_QUIETMODE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
		    }
		    #endif
            break;
        case OEMNV_PROFILE_MEETING:
			#if 0
            (void)IMENUCTL_SetTitle(pMenu,
                                    AEE_APPSSOUNDMENU_RES_FILE,
                                    IDS_MEETING,
                                    NULL); 
			#else
		    {
		  		AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSOUNDMENU_RES_FILE,                                
                        IDS_MEETING,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
		    }
		    #endif
            break;
        case OEMNV_PROFILE_NOISEMODE:
			#if 0
            (void)IMENUCTL_SetTitle(pMenu,
                                    AEE_APPSSOUNDMENU_RES_FILE,
                                    IDS_NOISEMODE,
                                    NULL);  
			#else
		    {
		  		AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSOUNDMENU_RES_FILE,                                
                        IDS_NOISEMODE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
		    }
		    #endif
            break;
        case OEMNV_PROFILE_CARMODE:
			#if 0
            (void)IMENUCTL_SetTitle(pMenu,
                                    AEE_APPSSOUNDMENU_RES_FILE,
                                    IDS_CARMODE,
                                    NULL);
			#else
		    {
		  		AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSOUNDMENU_RES_FILE,                                
                        IDS_CARMODE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
		    }
		    #endif
            break;
/*            
#ifdef FEATURE_VERSION_C316
        case OEMNV_PROFILE_BLUETOOTH:
			#if 0
            (void)IMENUCTL_SetTitle(pMenu,
                                    AEE_APPSSOUNDMENU_RES_FILE,
                                    IDS_PROFILE_BLUETOOTH,
                                    NULL);
			#else
		    {
		  		AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                                        AEE_APPSSOUNDMENU_RES_FILE,                                
                                        IDS_PROFILE_BLUETOOTH,
                                        WTitle,
                                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
		    }
		    #endif
            break;
#endif
*/
        default:
            break;
    }
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            Sound_App_Add_Menu(pMenu,IDS_VOLUME_TITLE);
            Sound_App_Add_Menu(pMenu,IDS_CALLRING);
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
#ifdef FEATURE_VERSION_W317A
			Sound_App_Add_Menu(pMenu,IDS_MSG_TONE);
#else
            Sound_App_Add_Menu(pMenu,IDS_SMSRING);
#endif
#endif
            Sound_App_Add_Menu(pMenu,IDS_ALARMRING);
            Sound_App_Add_Menu(pMenu,IDS_CALLHINT);
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
#ifdef FEATURE_VERSION_W317A
			Sound_App_Add_Menu(pMenu,IDS_MSG_ALERT_MODE);
#else
            Sound_App_Add_Menu(pMenu,IDS_SMSHINT);
#endif
#endif
#if defined(FEATURE_COLORKEYSND)
            Sound_App_Add_Menu(pMenu,IDS_COLORKEYSND);
#endif
#if !defined(FEATURE_LANG_ARABIC)
#ifndef FEATURE_VERSION_C337
            Sound_App_Add_Menu(pMenu,IDS_MISSEDCALL_ALERT);
#endif
#endif
            //if (pMe->m_bNaturalStart == NORMAL_ENTRY)
            //{
            //    Sound_App_Add_Menu(pMenu,IDS_HEADSET_AUTOANSWER_TITLE);
            //}

            //Sound_App_Add_Menu(pMenu,IDS_KEYTONE_LENGTH);

	
			#if defined (FEATURE_VERSION_W317A) || defined (FEATURE_VERSION_C337) || defined (FEATURE_VERSION_C316)
			/*
			if (pMe->m_ePreState != DLGRET_SCENEMODESUB)
			{
				Sound_App_Add_Menu(pMenu,IDS_STARTUP_RINGER);
            	Sound_App_Add_Menu(pMenu,IDS_SHUTDOWN_RINGER);	
			}
			*/
			#else
			Sound_App_Add_Menu(pMenu,IDS_STARTUP_RINGER);
            Sound_App_Add_Menu(pMenu,IDS_SHUTDOWN_RINGER);
			#endif            

#ifndef FEATURE_VERSION_C337
            Sound_App_Add_Menu(pMenu,IDS_POWERONOFF_ALERT);
#endif
           // Sound_App_Add_Menu(pMenu,IDS_FMRADIO_OPTION_MENU_PLAY_MODLE);
            return TRUE;

        case EVT_DIALOG_START:
            // 给菜单各菜单项加数字编号图标
            SoundMenu_SetItemNumIcon(pMenu);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties( pMenu , OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);

            IMENUCTL_SetSel(pMenu, pMe->m_sSubDlgId);

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_APP_SOUNDMENU,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;

        case EVT_USER_REDRAW:
            //(void)IMENUCTL_Redraw(pMenu);   //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:        	
            return TRUE;

        case EVT_CTL_SEL_CHANGED:
            pMe->m_sSubDlgId = IMENUCTL_GetSel(pMenu);            
            return TRUE;
            
        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:                   
                    pMe->m_sSubDlgId = 0;
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            //pMe->m_sSubDlgId = IMENUCTL_GetSel(pMenu);
            pMe->m_sSubDlgId = wParam;
            //初始化音量
            (void) ICONFIG_GetItem(pMe->m_pConfig,
                            CFGI_PROFILE_RINGER_VOL,
                            pMe->m_RingCurVol,
                            sizeof(pMe->m_RingCurVol));
            switch (wParam)
            {
                case IDS_CALLRING:                  //来电铃声
                    pMe->m_RingerType = SET_RINGER;
#if 0
                    if(pMe->m_RingCurVol[pMe->m_CurProfile] == OEMSOUND_MUTE_VOL)
                    {
                        CLOSE_DIALOG(DLGRET_WARNING)
                    }
                    else
#endif
                    {
                        CLOSE_DIALOG(DLGRET_RINGER)
                    }
                    break;

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
#ifdef FEATURE_VERSION_W317A
				case IDS_MSG_TONE:
#else
                case IDS_SMSRING:                  //消息铃声
#endif                
#ifdef FEATURE_SMSTONETYPE_MID                        
                    pMe->m_RingerType = SET_SMSTONE;
                    {
                        CLOSE_DIALOG(DLGRET_RINGER)
                    } 
#else
#if 0
                    if(pMe->m_RingCurVol[pMe->m_CurProfile] == OEMSOUND_MUTE_VOL)
                    {
                        CLOSE_DIALOG(DLGRET_WARNING)
                    }
                    else
#endif
                    {
                        pMe->m_RingerType = SET_SMSTONE;
                        CLOSE_DIALOG(DLGRET_SMSRING)
                    }
#endif //#if defined FEATURE_SMSTONETYPE_MID		    
                    break;
#endif

                case IDS_ALARMRING:                //闹钟铃声
                    pMe->m_RingerType = SET_ALARMRING;
#if 0
                    if(pMe->m_RingCurVol[pMe->m_CurProfile] == OEMSOUND_MUTE_VOL)
                    {
                        CLOSE_DIALOG(DLGRET_WARNING)
                    }
                    else
#endif
                    {
                        CLOSE_DIALOG(DLGRET_RINGER)
                    }
                    break;

                case IDS_STARTUP_RINGER:             //开机音乐
                    pMe->m_RingerType = SET_STARTUP;
#if 0
                    if(pMe->m_RingCurVol[pMe->m_CurProfile] == OEMSOUND_MUTE_VOL)
                    {
                        CLOSE_DIALOG(DLGRET_WARNING)
                    }
                    else
#endif
                    {
                        CLOSE_DIALOG(DLGRET_RINGER)
                    }
                    break;

                case IDS_SHUTDOWN_RINGER:            //关机音乐
                    pMe->m_RingerType = SET_SHUTDOWN;
#if 0
                    if(pMe->m_RingCurVol[pMe->m_CurProfile] == OEMSOUND_MUTE_VOL)
                    {
                        CLOSE_DIALOG(DLGRET_WARNING)
                    }
                    else
#endif
                    {
                        CLOSE_DIALOG(DLGRET_RINGER)
                    }
                    break;

                case IDS_CALLHINT:                  //来电提示方式
                	
                    pMe->m_HintType = SET_CALLHINT;
#if 0
                    if(pMe->m_RingCurVol[pMe->m_CurProfile] == OEMSOUND_MUTE_VOL)
                    {
                        CLOSE_DIALOG(DLGRET_WARNING)
                    }
                    else
#endif
                    {
                        CLOSE_DIALOG(DLGRET_HINT)
                    }
                    break;
                    
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
#ifdef FEATURE_VERSION_W317A
				case IDS_MSG_ALERT_MODE:
#else
                case IDS_SMSHINT:                   //短信提示方式
#endif                
                    pMe->m_HintType = SET_SMSHINT;
#if 0
                    if(pMe->m_RingCurVol[pMe->m_CurProfile] == OEMSOUND_MUTE_VOL)
                    {
                        CLOSE_DIALOG(DLGRET_WARNING)
                    }
                    else
#endif
                    {
                        CLOSE_DIALOG(DLGRET_HINT)
                    }
                    break;
#endif
#if defined(FEATURE_COLORKEYSND)
                case IDS_COLORKEYSND:              //多彩按键音
                    CLOSE_DIALOG(DLGRET_KEYSND)
                    break;
#endif
                //case IDS_KEYTONE_LENGTH:           //按键音长度
                //    CLOSE_DIALOG(DLGRET_KEYLENGTH)
                //    break;

                case IDS_VOLUME_TITLE:                    //音量
                 #ifdef FEATURE_APP_MUSICPLAYER
                    if(GetMp3PlayerStatus() == MP3STATUS_RUNONBACKGROUND)
                    {
                        pMe->m_sSubDlgId = IDS_CONFIRM_OFF_MP;
                        CLOSE_DIALOG(DLGRET_MESSAGE)
                    }
                    else
                 #endif       
                    {
                        CLOSE_DIALOG(DLGRET_VOLUME)
                    }
                    break;

                case IDS_MISSEDCALL_ALERT:        //未接来电提醒
                    pMe->m_OtherVolType = SET_MISSEDCALLALERT;
                    CLOSE_DIALOG(DLGRET_OTHERSEL)
                    break;
                    
                case IDS_POWERONOFF_ALERT:        //开关机铃声提示
                    pMe->m_OtherVolType = SET_POWERONOFFALERT;
                    CLOSE_DIALOG(DLGRET_OTHERSEL)
                    break;
                case IDS_FMRADIO_OPTION_MENU_PLAY_MODLE: //FM播放模式
                	CLOSE_DIALOG(DLGRET_FMMODE)
                	break;

                //case IDS_HEADSET_AUTOANSWER_TITLE:      //自动接听
                //    CLOSE_DIALOG(DLGRET_AUTOANSWERSUB)
                //    break;

                default:
                    ASSERT_NOT_REACHABLE;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // HandleSoundMenuProfilesDialogEvent

/*==============================================================================
函数：
       HandleHintDialogEvent
说明：
       IDD_HINT_MENU对话框事件处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleHintDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_HINT);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    SOUND_ERR("%x,%x,%x,HandleHintDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    //SoundMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};

				#ifdef FEATURE_VERSION_W317A
				(void)ISHELL_LoadResString(pMe->m_pShell,
					                        AEE_APPSSOUNDMENU_RES_FILE,                                
					                        IDS_MSG_ALERT_MODE,
					                        WTitle,
					                        sizeof(WTitle));
				#else
				(void)ISHELL_LoadResString(pMe->m_pShell,
					                        AEE_APPSSOUNDMENU_RES_FILE,                                
					                        IDS_SMSHINT,
					                        WTitle,
					                        sizeof(WTitle));
				#endif
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            Sound_App_Add_Menu(pMenu,IDS_ITEM_OFF);
            Sound_App_Add_Menu(pMenu,IDS_ALERTTYPE_RINGER);
            #if !defined(FEATURE_NO_VIBRATE)
            Sound_App_Add_Menu(pMenu,IDS_ALERTTYPE_VIB);
            #if !defined (FEATURE_VERSION_C306)
            Sound_App_Add_Menu(pMenu,IDS_ITEM_VIBRING);
#ifdef FEATURE_CARRIER_THAILAND_CAT
            if(SET_CALLHINT == pMe->m_HintType)
            {
                Sound_App_Add_Menu(pMenu,IDS_ALERTTYPE_VIBANDRING); 
            }
#else
           #if defined(FEATURE_VERSION_W027)||defined(FEATURE_VERSION_C117)||defined(FEATURE_VERSION_M74)
           #ifdef FEATURE_VERSION_C260_IC18
           Sound_App_Add_Menu(pMenu,IDS_ALERTTYPE_VIBANDRING); 
           #endif
           #else
            Sound_App_Add_Menu(pMenu,IDS_ALERTTYPE_VIBANDRING); 
           #endif
#endif
			#endif //FEATURE_VERSION_C306
            #endif//FEATURE_VERSION_W021_CT100
            return TRUE;

        case EVT_DIALOG_START:
            {
                uint16    ui16_return = IDS_ALERTTYPE_RINGER;
                byte      byte_return[PROFILENUMBER];
                switch(pMe->m_HintType)
                {
                    case SET_CALLHINT:     //来电提示方式
                        (void) ICONFIG_GetItem(pMe->m_pConfig,
                                               CFGI_PROFILE_ALERT_TYPE,
                                               byte_return,
                                               sizeof(byte_return));
                        pMe->m_wResID = IDS_CALLHINT;
                        break;

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
                    case SET_SMSHINT:     //短信提示方式
                        (void) ICONFIG_GetItem(pMe->m_pConfig,
                                               CFGI_PROFILE_SMS_RINGER,
                                               byte_return,
                                               sizeof(byte_return));
						#ifdef FEATURE_VERSION_W317A
						pMe->m_wResID = IDS_MSG_ALERT_MODE;
						#else
                        pMe->m_wResID = IDS_SMSHINT;
						#endif
                        break;
#endif

                    default:
                        return FALSE;
                 }
                MSG_FATAL("pMe->m_CurProfile=====HandleHintDialogEvent===%d",pMe->m_CurProfile,0,0);
                switch(byte_return[pMe->m_CurProfile])
                {
                    case OEMNV_SMS_OFF:           //关闭
                        ui16_return = IDS_ITEM_OFF;
                        break;

                    case OEMNV_SMS_RING:           //铃声
                    default:
                        ui16_return = IDS_ALERTTYPE_RINGER;
                        break;
                    case OEMNV_SMS_VIBONLY:        //振动
                        ui16_return = IDS_ALERTTYPE_VIB;
                        break;

                    case OEMNV_SMS_RINGVIB:        //响铃震动
                        ui16_return = IDS_ITEM_VIBRING;
                        break;
                        
                    case OEMNV_ALERTTYPE_VIBANDRINGER:        //响铃震动
                        ui16_return = IDS_ALERTTYPE_VIBANDRING;
                        break;  
                }
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, ui16_return, TRUE);
                IMENUCTL_SetSel(pMenu, ui16_return);
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
                IMENUCTL_SetOemProperties( pMenu , OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
				#if 0
                (void)IMENUCTL_SetTitle(pMenu,
                                        AEE_APPSSOUNDMENU_RES_FILE,
                                        pMe->m_wResID,
                                        NULL);
				#else
			    {
			  		AECHAR WTitle[40] = {0};
					(void)ISHELL_LoadResString(pMe->m_pShell,
	                        AEE_APPSSOUNDMENU_RES_FILE,                                
	                        pMe->m_wResID,
	                        WTitle,
	                        sizeof(WTitle));
					IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
			    }
			    #endif
                (void) ISHELL_PostEvent( pMe->m_pShell,
                                         AEECLSID_APP_SOUNDMENU,
                                         EVT_USER_REDRAW,
                                         0,
                                         0);
            }
            return TRUE;

        case EVT_USER_REDRAW:
            //(void)IMENUCTL_Redraw(pMenu);   //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
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
            byte      alerttype[PROFILENUMBER];
            byte      byte_set[PROFILENUMBER];
            switch (wParam)
            {
                case IDS_ITEM_OFF:      //关闭
                    alerttype[pMe->m_CurProfile] = OEMNV_SMS_OFF;                    
                    break;

                case IDS_ALERTTYPE_RINGER:   //铃声
                    alerttype[pMe->m_CurProfile] = OEMNV_SMS_RING;                   
                    break;
                case IDS_ALERTTYPE_VIB:      //振动
                    alerttype[pMe->m_CurProfile] = OEMNV_SMS_VIBONLY;                   
                    break;

                case IDS_ITEM_VIBRING:  //响铃震动
                    alerttype[pMe->m_CurProfile] = OEMNV_SMS_RINGVIB;                   
                    break;
                    
                case IDS_ALERTTYPE_VIBANDRING:  //响铃震动
                    alerttype[pMe->m_CurProfile] = OEMNV_ALERTTYPE_VIBANDRINGER;                    
                    break;
                default:
                    ASSERT_NOT_REACHABLE;
            }
            
            InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, wParam, TRUE);
            switch(pMe->m_HintType)
            {
                case SET_CALLHINT: //来电提示方式                	
                    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                            CFGI_PROFILE_ALERT_TYPE,
                                            byte_set,
                                            sizeof(byte_set));
                    byte_set[pMe->m_CurProfile] = alerttype[pMe->m_CurProfile];
                    (void) ICONFIG_SetItem(pMe->m_pConfig,
                                            CFGI_PROFILE_ALERT_TYPE,
                                            byte_set,
                                            sizeof(byte_set));
                    break;

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
                case SET_SMSHINT: //短信提示方式
                    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                            CFGI_PROFILE_SMS_RINGER,
                                            byte_set,
                                            sizeof(byte_set));
                    byte_set[pMe->m_CurProfile] = alerttype[pMe->m_CurProfile];
                    (void) ICONFIG_SetItem(pMe->m_pConfig,
                                            CFGI_PROFILE_SMS_RINGER,
                                            byte_set,
                                            sizeof(byte_set));
                    break;
#endif

                default:
                    return FALSE;
            }
            //从ISoundMenu_SoundList接口进入声音设置菜单，
            //或在已启动的情景模式中编辑，直接激活用户设置
            if (pMe->m_bNaturalStart == SOUNDLIST_ENTRY || pMe->m_active == pMe->m_CurProfile)
            {            	
                pMe->m_ScenemodesubType = SET_ACTIVATE;
                SoundMenu_SceneModeActivate(pMe);
            }
            CLOSE_DIALOG(DLGRET_MESSAGE)
            return TRUE;
        }


        default:
            break;
    }

    return FALSE;
} // HandleHintDialogEvent


/*==============================================================================
函数：
       HandleRingerDialogEvent
说明：
       IDD_RINGER_MENU对话框事件处理函数,包括来电铃声合闹钟铃声.

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleRingerDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl  *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_RINGER);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    SOUND_ERR("%x,%x,%x,HandleRingerDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    //SoundMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        // 获取当前设置的铃声,并给m_lastRingerPlayed赋正确的值.
        {
            notifyFMRadioAlertEvent( pMe, TRUE);
            SoundMenu_InitRingerList(pMe);
            pMe->m_RingerID[pMe->m_CurProfile].midID = 0;
			pMe->m_slecet_id = IMENUCTL_GetSel(pMenu);
            if (pMe->m_RingerType == SET_RINGER)
            {
                ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_CALL_RINGER,(void*)pMe->m_RingerID,sizeof(pMe->m_RingerID));
                if (pMe->m_RingerID[pMe->m_CurProfile].ringType == OEMNV_MP3_RINGER)
                {
                    pMe->m_RingID[pMe->m_CurProfile] = DOWNLOAD_MENU;
                    //pMe->m_RingerID[pMe->m_CurProfile].midID = OEMNV_DEFAULTRINGER;
                    //ICONFIG_SetItem(pMe->m_pConfig,CFGI_PROFILE_CALL_RINGER,(void*)pMe->m_RingerID, sizeof(pMe->m_RingerID));
                }
                else //if(pMe->m_RingerID[pMe->m_CurProfile].midID == 0)
                {
                    pMe->m_RingID[pMe->m_CurProfile] = pMe->m_RingerID[pMe->m_CurProfile].midID;
                }
            }
            else if(pMe->m_RingerType == SET_ALARMRING)
            {
                ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_ALARM_RINGER,(void*)pMe->m_RingerID,sizeof(pMe->m_RingerID));

                if (pMe->m_RingerID[pMe->m_CurProfile].ringType == OEMNV_MP3_RINGER)
                {
                    pMe->m_RingID[pMe->m_CurProfile] = DOWNLOAD_MENU;
                    //pMe->m_RingerID[pMe->m_CurProfile].midID = OEMNV_ALARM_RINGER;
                    //ICONFIG_SetItem(pMe->m_pConfig,CFGI_PROFILE_ALARM_RINGER,(void*)pMe->m_RingerID,sizeof(pMe->m_RingerID));
                }
                else
                {
                     pMe->m_RingID[pMe->m_CurProfile] = pMe->m_RingerID[pMe->m_CurProfile].midID;
                }
            }
            else if(pMe->m_RingerType == SET_STARTUP)
            {
                ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_STARTUP_MUSIC,pMe->m_RingID,sizeof(pMe->m_RingID));

                if (pMe->m_RingID[pMe->m_CurProfile] == 0)
                {
                    pMe->m_RingID[pMe->m_CurProfile] = OEMNV_STARTUP_MUSIC; //OEMNV_ALARM_RINGER;
                    ICONFIG_SetItem(pMe->m_pConfig,CFGI_PROFILE_STARTUP_MUSIC,pMe->m_RingID,sizeof(pMe->m_RingID));
                }
            }
            else if(pMe->m_RingerType == SET_SHUTDOWN)
            {
                ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_SHUTDOWN_MUSIC,pMe->m_RingID,sizeof(pMe->m_RingID));

                if (pMe->m_RingID[pMe->m_CurProfile] == 0)
                {
                    pMe->m_RingID[pMe->m_CurProfile] = OEMNV_SHUTDOWN_MUSIC; //OEMNV_ALARM_RINGER; 
                    ICONFIG_SetItem(pMe->m_pConfig,CFGI_PROFILE_SHUTDOWN_MUSIC,pMe->m_RingID,sizeof(pMe->m_RingID));
                }
            }
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
#ifdef FEATURE_SMSTONETYPE_MID             
            else if(pMe->m_RingerType == SET_SMSTONE)
            {
                ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_SMS_RINGER_ID,pMe->m_RingerID,sizeof(pMe->m_RingerID));

                if (pMe->m_RingerID[pMe->m_CurProfile].midID == 0)
                {
                    pMe->m_RingerID[pMe->m_CurProfile].midID = OEMNV_SMS_RINGER_ID;
                    ICONFIG_SetItem(pMe->m_pConfig,CFGI_PROFILE_SMS_RINGER_ID,pMe->m_RingerID,sizeof(pMe->m_RingerID));
                }
            }  
#endif //#if defined FEATURE_SMSTONETYPE_MID		    
#endif
            else if(pMe->m_RingerType == SET_ANOTHER)
            {
                if((pMe->m_enter_address[0] == (AECHAR)'\0'))
                {
                    pMe->m_RingID[pMe->m_CurProfile] = DEFAULT_MENU;
                }
                else
                {
                    IRingerMgr      *RingerMgr = NULL;
                    int ringerId = AEE_RINGER_ID_NONE;
                    char        filename[MAX_FILE_NAME]={0};
                    
                    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                                        AEECLSID_RINGERMGR,
                                                        (void **)&RingerMgr))
                    {
                        return FALSE;
                    }
                    
                    WSTRTOSTR(pMe->m_enter_address, filename, MAX_FILE_NAME);
                    
                    // 为了支持PEK 测试，电话本的RINGTONE 字段只能保存字符串类型，这里根据路径名查找是否有MID存在
                    ringerId = IRINGERMGR_GetRingerID(RingerMgr, filename);
                    if(AEE_RINGER_ID_NONE == ringerId)
                    {
                        // 是mp3
                        pMe->m_RingID[pMe->m_CurProfile] = DOWNLOAD_MENU;
                    }
                    else
                    {
                        // mid
                        pMe->m_RingID[pMe->m_CurProfile] = ringerId;
                    }
                    IRINGERMGR_Release(RingerMgr);
                }

            }

            pMe->m_lastRingerPlayed = pMe->m_RingID[pMe->m_CurProfile];
            pMe->m_eMakeListMode = MAKELIST_INIT;
            return TRUE;
        }


        case EVT_DIALOG_START:
        {
			#ifdef FEATURE_VERSION_K212
			{
				nv_item_type	SimChoice;
				SimChoice.sim_select =1;
				(void)OEMNV_Put(NV_SIM_SELECT_I,&SimChoice);
			}
            #endif
            if (pMe->m_RingerType == SET_RINGER||pMe->m_RingerType == SET_ANOTHER)
            {
                pMe->m_wResID = IDS_CALLRING;  //来电铃声title
            }
            else if(pMe->m_RingerType == SET_ALARMRING)
            {
                pMe->m_wResID = IDS_ALARMRING; //闹钟铃声title
            }
            else if(pMe->m_RingerType == SET_STARTUP)
            {
                pMe->m_wResID = IDS_STARTUP_RINGER;
            }
            else if(pMe->m_RingerType == SET_SHUTDOWN)
            {
                pMe->m_wResID = IDS_SHUTDOWN_RINGER;
            }
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
#ifdef FEATURE_SMSTONETYPE_MID               
            else if(pMe->m_RingerType == SET_SMSTONE)
            {
#ifdef FEATURE_VERSION_W317A
				pMe->m_wResID = IDS_MSG_TONE;
#else
                pMe->m_wResID = IDS_SMSRING;
#endif
            }  
#endif //#if defined FEATURE_SMSTONETYPE_MID		    
#endif
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties( pMenu , OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);

            // 设置标题条文本
            #if 0
            IMENUCTL_SetTitle(pMenu,AEE_APPSSOUNDMENU_RES_FILE,pMe->m_wResID,NULL);
			#else
		    {
		  		AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSOUNDMENU_RES_FILE,                                
                        pMe->m_wResID,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
		    }
		    #endif
            IRINGERMGR_RegisterNotify(pMe->m_pRingerMgr, NULL, NULL);
            SoundMenu_UpdateRingerListMenu(pMe, pMenu);
            InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, pMe->m_lastRingerPlayed, TRUE);
            ProfileNotifyMP3PlayerAlertEvent(pMe, TRUE);
            ISHELL_PostEvent( pMe->m_pShell,AEECLSID_APP_SOUNDMENU,EVT_USER_REDRAW,0,0);
            return TRUE;
        }

        case EVT_USER_REDRAW:
            // 统一更新界面
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);

            //初始化音量
            pMe->m_slecet_id = IMENUCTL_GetSel(pMenu);
            ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_RINGER_VOL,pMe->m_RingCurVol,sizeof(pMe->m_RingCurVol));

            if(pMe->m_fSubDlgId != DOWNLOAD_MENU &&
                                            pMe->m_RingCurVol[pMe->m_CurProfile] != OEMSOUND_MUTE_VOL)
            {
                //预听铃声
                MSG_FATAL("RingerPreview....................................................",0,0,0);
                ISHELL_SetTimer(pMe->m_pShell, 250, RingerPreview, pMe);
                //SoundMenu_StartRingerPreview(pMe,
                //                            pMe->m_lastRingerPlayed);

            }
            (void)IMENUCTL_Redraw(pMenu);
            return TRUE;

        case EVT_UPDATAMENU:
            IALERT_StopRingerAlert(pMe->m_pAlert);
            MSG_FATAL("%x,%x,%x,HandleRingerDialogEvent",eCode,wParam,dwParam);

            // 更新菜单列表
            SoundMenu_UpdateRingerListMenu(pMe, pMenu);
            if (pMe->m_eMakeListMode == MAKELIST_RESUMECURPAGE)
            {
                (void)IMENUCTL_Redraw(pMenu);
            }
            //InitMenuIcons(pMenu);
            if(pMe->m_fSubDlgId != DOWNLOAD_MENU &&pMe->m_RingCurVol[pMe->m_CurProfile] != OEMSOUND_MUTE_VOL && wParam != 0)
            {
                //预听铃声
                MSG_FATAL("RingerPreviewEVT_UPDATAMENU....................................................",0,0,0);
                ISHELL_SetTimer(pMe->m_pShell, 250, RingerPreview, pMe);
                //SoundMenu_StartRingerPreview(pMe,
                //                            pMe->m_lastRingerPlayed);
            }
            return TRUE;

        case EVT_DIALOG_END:
			#ifdef FEATURE_VERSION_K212
			{
				nv_item_type	SimChoice;
				SimChoice.sim_select =2;
				(void)OEMNV_Put(NV_SIM_SELECT_I,&SimChoice);
			}
            #endif
            if((!pMe->m_bSuspending) || (APP_MEDIA_IMPACT_BY_MP3 == app_media_scheduler()))
            {
                IALERT_StopRingerAlert(pMe->m_pAlert);
                notifyFMRadioAlertEvent( pMe, FALSE);
                ProfileNotifyMP3PlayerAlertEvent(pMe, FALSE);
                ISHELL_CancelTimer(pMe->m_pShell, RingerPreview, pMe);
            }          
            if(pMe->m_eDlgRet != DLGRET_CANCELED)
            {
                pMe->m_fSubDlgId = IMENUCTL_GetSel(pMenu);//记录挂起前的选项
                pMe->m_eMakeListMode = MAKELIST_RESUMECURPAGE;
            }
            else
            {
                pMe->m_fSubDlgId = 0;
            }
            return TRUE;

        //当更改了menu选项,需要停止播放前一个铃声,开始播放现在选中的铃声
        case EVT_CTL_SEL_CHANGED:
            ISHELL_CancelTimer(pMe->m_pShell, RingerPreview, pMe);
            IALERT_StopRingerAlert(pMe->m_pAlert);

            //InitMenuIcons(pMenu);
            if(pMe->m_lastRingerPlayed != wParam)
            {
                // only start if it has changed and it isn't DOWNLOAD_MENU
                pMe->m_lastRingerPlayed = wParam;

                //初始化音量
                ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_RINGER_VOL,pMe->m_RingCurVol,sizeof(pMe->m_RingCurVol));

                if(wParam != DOWNLOAD_MENU && pMe->m_RingCurVol[pMe->m_CurProfile] != OEMSOUND_MUTE_VOL)
                {
                    SOUND_ERR("pMe->m_lastRingerPlayed = %d",pMe->m_lastRingerPlayed,0,0);
					MSG_FATAL("EVT_CTL_SEL_CHANGED........................dwParam:%d,wParam:%d",dwParam,wParam,0);
					//MODI BY YANGDECAI 09-27
					pMe->m_slecet_id = wParam;
                    ISHELL_SetTimer(pMe->m_pShell, 250, RingerPreview, pMe);
                    //SoundMenu_StartRingerPreview(pMe,
                    //                           pMe->m_lastRingerPlayed);
                }
            }
            return TRUE;

        case EVT_KEY_PRESS:
        {
            int nCount = IMENUCTL_GetItemCount(pMenu);

            if (nCount < 1)
            {
                return FALSE;
            }

            switch(wParam)
            {
                case AVK_DOWN:
                    if (IMENUCTL_GetItemID(pMenu, nCount -1) ==  IMENUCTL_GetSel(pMenu))
                    {
                        // 菜单到底，需列举下一页菜单
                        pMe->m_eMakeListMode = MAKELIST_NEXTPAGE;

                        // 发送更新菜单列表事件
                        ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_SOUNDMENU, EVT_UPDATAMENU,0,0);
                    }
                    return TRUE;

                case AVK_UP:
                    if (IMENUCTL_GetItemID(pMenu, 0) ==  IMENUCTL_GetSel(pMenu))
                    {
                        // 菜单到顶，需列举上一页菜单
                        pMe->m_eMakeListMode = MAKELIST_PREPAGE;

                        // 发送更新菜单列表事件
                        ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_SOUNDMENU,EVT_UPDATAMENU,0, 0);
                    }
                    return TRUE;

                case AVK_LEFT:
                    // 列举上一页菜单
                    pMe->m_eMakeListMode = MAKELIST_PREPAGE;

                    // 发送更新菜单列表事件
                    //IALERT_StopRinger(pMe->m_pAlert);
                    ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_SOUNDMENU,EVT_UPDATAMENU, 0,0);
                    return TRUE;

                case AVK_RIGHT:
                    // 列举下一页菜单
                    pMe->m_eMakeListMode = MAKELIST_NEXTPAGE;

                    // 发送更新菜单列表事件
                    //IALERT_StopRinger(pMe->m_pAlert);
                    ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_SOUNDMENU,EVT_UPDATAMENU,0, 0);
                    return TRUE;

                default:
                    break;
            }
            return FALSE;
        }

        case EVT_KEY:
            switch(wParam)
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
            ringID nNewConfigRinger[PROFILENUMBER];   // FOR  CALL  ALARM  SMS
            uint16 nNewConfigRing[PROFILENUMBER];     // For  startup ringer  and  shut down ringer

            nNewConfigRinger[pMe->m_CurProfile].midID = (uint16)(wParam);
            nNewConfigRing[pMe->m_CurProfile] = (uint16)(wParam);
            
            InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, wParam, TRUE);

			MSG_FATAL("***zzg HandlerRingerDialogEvent wParam=%x***", wParam, 0, 0);
			
            if(wParam == DOWNLOAD_MENU)
            {
#ifdef FEATURE_APP_MEDIAGALLERY            
                ExplorerPara sSoundPara={0};
                
                sSoundPara.nSelectProfile = pMe->m_CurProfile;                
                MSG_ERROR("SELECT MEDIA GALLERY..........",0,0,0);
                CMediaGallery_RegisterCallback((PFNMGSELECTEDCB)SoundMenu_SetMp3ring, pMe);
                CMediaGallery_FileExplorer(GALLERY_MUSIC_SETTING, &sSoundPara);                
#endif                
                return TRUE;
            }
#if 0            //选中下载新铃声,启动铃声图片applet
            if (wParam == DOWNLOAD_MENU)
            {
                IALERT_StopRingerAlert(pMe->m_pAlert);
                //ISHELL_StartApplet(pMe->m_pShell,AEECLSID_IMAGEBROWSER); // 修改铃声图片下载ID name
                return TRUE;
            }
#endif            
            //来电铃声
            if(pMe->m_RingerType == SET_RINGER)
            {
                if (nNewConfigRinger[pMe->m_CurProfile].midID <  NUM_OF_MAXRINGER)
                {
                    ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_CALL_RINGER,(void*)nNewConfigRinger,sizeof(nNewConfigRinger));
                    nNewConfigRinger[pMe->m_CurProfile].midID = (uint16)(wParam);
                    nNewConfigRinger[pMe->m_CurProfile].ringType = OEMNV_MID_RINGER;
                    ICONFIG_SetItem(pMe->m_pConfig,CFGI_PROFILE_CALL_RINGER,(void*)nNewConfigRinger,sizeof(nNewConfigRinger));
                }
            }
            else if(pMe->m_RingerType == SET_ALARMRING)       //闹钟铃声
            {
                if (nNewConfigRinger[pMe->m_CurProfile].midID < NUM_OF_MAXRINGER)
                {
                    ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_ALARM_RINGER, (void*)nNewConfigRinger,sizeof(nNewConfigRinger));
                    nNewConfigRinger[pMe->m_CurProfile].midID = (uint16)(wParam);
                    nNewConfigRinger[pMe->m_CurProfile].ringType = OEMNV_MID_RINGER;
                    ICONFIG_SetItem(pMe->m_pConfig, CFGI_PROFILE_ALARM_RINGER,(void*)nNewConfigRinger,sizeof(nNewConfigRinger));
                }

            }
            else if(pMe->m_RingerType == SET_STARTUP)
            {
                if (nNewConfigRing[pMe->m_CurProfile] < NUM_OF_MAXRINGER)
                {
                    ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_STARTUP_MUSIC, nNewConfigRing,sizeof(nNewConfigRing));
                    nNewConfigRing[pMe->m_CurProfile] = (uint16)(wParam);
                    ICONFIG_SetItem(pMe->m_pConfig, CFGI_PROFILE_STARTUP_MUSIC,nNewConfigRing,sizeof(nNewConfigRing));
                }

            }
            else if(pMe->m_RingerType == SET_SHUTDOWN)
            {
                if (nNewConfigRing[pMe->m_CurProfile] < NUM_OF_MAXRINGER)
                {
                    ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_SHUTDOWN_MUSIC,nNewConfigRing,sizeof(nNewConfigRing));
                    nNewConfigRing[pMe->m_CurProfile] = (uint16)(wParam);
                    ICONFIG_SetItem(pMe->m_pConfig,CFGI_PROFILE_SHUTDOWN_MUSIC,nNewConfigRing,sizeof(nNewConfigRing));
                }
            }
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
#ifdef FEATURE_SMSTONETYPE_MID             
            else if(pMe->m_RingerType == SET_SMSTONE)
            {
                if (nNewConfigRinger[pMe->m_CurProfile] < NUM_OF_MAXRINGER)
                {
                    ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_SMS_RINGER_ID,(void*)nNewConfigRinger,sizeof(nNewConfigRinger));
                    nNewConfigRinger[pMe->m_CurProfile].midID = (uint16)(wParam);
                    nNewConfigRinger[pMe->m_CurProfile].ringType = OEMNV_MID_RINGER;
                    ICONFIG_SetItem(pMe->m_pConfig,CFGI_PROFILE_SMS_RINGER_ID,(void*)nNewConfigRinger,sizeof(nNewConfigRinger));
                }
            }
#endif //#if defined FEATURE_SMSTONETYPE_MID		    
#endif
            else if(pMe->m_RingerType == SET_ANOTHER)
            {
                if(wParam == DEFAULT_MENU)
                {
                    pMe->m_enter_address[0] = (AECHAR)'\0';
                }
                else if(pMe->m_enter_address!= NULL)
                {
                    IRingerMgr      *RingerMgr = NULL;
                    AEERingerInfo  info = {0};
                    AECHAR     filename[MAX_FILE_NAME] = {0};
                    
                    MSG_ERROR("%x %x %d SoundMenu_Ringer_List2",pMe->m_enter_address,pMe->m_enter_address, pMe->m_enter_address);
                    
                    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                                        AEECLSID_RINGERMGR,
                                                        (void **)&RingerMgr))
                    {
                        return FALSE;
                    }
                    
                    // 为了支持PEK 测试，电话本的RINGTONE 字段只能保存字符串类型，这里根据路径名查找是否有MID存在
                    if( SUCCESS == IRINGERMGR_GetRingerInfo(RingerMgr, wParam, &info))
                    {
                        STRTOWSTR(info.szFile, filename, MAX_FILE_NAME*sizeof(AECHAR));
                        WSTRCPY(pMe->m_enter_address, filename);
                    }
                    else
                    {
                        pMe->m_enter_address[0] = (AECHAR)'\0';
                    }
                    IRINGERMGR_Release(RingerMgr);
                }
                SOUND_ERR("%x %x %d SoundMenu_Ringer_List2",pMe->m_enter_addr,&pMe->m_enter_addr,*pMe->m_enter_addr);
            }

            //从ISoundMenu_SoundList接口进入声音设置菜单，
            //或在已启动的情景模式中编辑，直接激活用户设置
            if (pMe->m_bNaturalStart == SOUNDLIST_ENTRY || pMe->m_active == pMe->m_CurProfile)
            {
                pMe->m_ScenemodesubType = SET_ACTIVATE;
                SoundMenu_SceneModeActivate(pMe);
            }
            CLOSE_DIALOG(DLGRET_MESSAGE)
            return TRUE;
        }

        default:
            break;
    }
    return FALSE;
} // HandleRingerDialogEvent

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
/*==============================================================================
函数：
       HandleSmsRingDialogEvent
说明：
       IDD_SMSRING_MENU对话框事件处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleSmsRingDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_SMSRING);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    SOUND_ERR("%x,%x,%x,HandleSmsRingDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    //SoundMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            notifyFMRadioAlertEvent( pMe, TRUE);
            return TRUE;

        case EVT_DIALOG_START:
        {
            AECHAR        name[3];
            ringID        smsID[PROFILENUMBER];
            //uint16        smsID[pMe->m_CurProfile] = 78;
            uint16        wItemID;
            int           i = 0;
            //sms铃声ID,定义在sndid.h中
            //uint16 smsRingerIDList[8] = {78,77,52,58,59,60};
            uint16 smsRingerIDList[8] = { SND_HFK_CALL_ORIG,
                                          SND_VR_HFK_CALL_RECEIVED,
                                          SND_FADE_TONE,
                                          SND_MESSAGE_ALERT,
                                          SND_ABRV_ALERT,
                                          SND_PIP_TONE};
            //铃声名称
            char smsRingerName[8][3] = { "01",
                                        "02",
                                        "03",
                                        "04",
                                        "05",
                                        "06"};
            ICONFIG_GetItem(pMe->m_pConfig,  CFGI_PROFILE_SMS_RINGER_ID, (void*)smsID, sizeof(smsID));
            //若NV没有取出,此处重新设置
            #ifdef FEATURE_VERSION_K212
			{
				nv_item_type	SimChoice;
				SimChoice.sim_select =1;
				(void)OEMNV_Put(NV_SIM_SELECT_I,&SimChoice);
			}
           #endif
            switch(smsID[pMe->m_CurProfile].midID)
            {
                case SND_HFK_CALL_ORIG:
                case SND_VR_HFK_CALL_RECEIVED:
                case SND_FADE_TONE:
                case SND_MESSAGE_ALERT:
                case SND_ABRV_ALERT:
                case SND_PIP_TONE:
                    break;
                default:
                    ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_SMS_RINGER_ID,(void*)smsID,sizeof(smsID));
                    smsID[pMe->m_CurProfile].midID = SND_HFK_CALL_ORIG;
                    ICONFIG_SetItem(pMe->m_pConfig,CFGI_PROFILE_SMS_RINGER_ID,(void*)smsID,sizeof(smsID));
                    break;
            }
            if(smsID[pMe->m_CurProfile].ringType == OEMNV_MP3_RINGER)
            {
                pMe->m_lastSmsRingPlayed = IDS_GALLERY_PROFILE;
            }
            else
            {
                pMe->m_lastSmsRingPlayed = smsID[pMe->m_CurProfile].midID;
            }
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};

#ifdef FEATURE_VERSION_W317A
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSOUNDMENU_RES_FILE,                                
                        IDS_MSG_TONE,
                        WTitle,
                        sizeof(WTitle));
#else
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSOUNDMENU_RES_FILE,                                
                        IDS_SMSRING,
                        WTitle,
                        sizeof(WTitle));
#endif
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            IMENUCTL_AddItem(pMenu, AEE_APPSSOUNDMENU_RES_FILE, IDS_GALLERY_PROFILE, IDS_GALLERY_PROFILE, NULL, 0);
            // 添加铃声选项
            for (i = 0; i < MAX_SMS_RINGER; i++)
            {
                wItemID = smsRingerIDList[i];

                STRTOWSTR(smsRingerName[i], name, 6);

                IMENUCTL_AddItem(pMenu,NULL,0,wItemID,name,0);
            }

            InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, pMe->m_lastSmsRingPlayed, TRUE);
            IMENUCTL_SetSel(pMenu, pMe->m_lastSmsRingPlayed);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties( pMenu , OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
            ProfileNotifyMP3PlayerAlertEvent(pMe, TRUE);
            ISHELL_PostEvent( pMe->m_pShell,AEECLSID_APP_SOUNDMENU,EVT_USER_REDRAW,0,0);
            return TRUE;
        }


        case EVT_USER_REDRAW:
            //初始化音量
            ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_RINGER_VOL,pMe->m_RingCurVol,sizeof(pMe->m_RingCurVol));

            if(pMe->m_RingCurVol[pMe->m_CurProfile] != OEMSOUND_MUTE_VOL)
            {
                //铃声预览
                IALERT_StartSMSAlertPreview(pMe->m_pAlert,pMe->m_lastSmsRingPlayed);
                //SoundMenu_StartRingerPreview(pMe,pMe->m_lastSmsRingPlayed);
                //IRINGERMGR_Play(pMe->m_pRingerMgr, pMe->m_lastSmsRingPlayed, 0);
            }

            // 统一更新界面
            //IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
            (void)IMENUCTL_Redraw(pMenu);
            return TRUE;

        case EVT_DIALOG_END:
            //IALERT_StopSMSAlert(pMe->m_pAlert);
            #ifdef FEATURE_VERSION_K212
			{
				nv_item_type	SimChoice;
				SimChoice.sim_select =2;
				(void)OEMNV_Put(NV_SIM_SELECT_I,&SimChoice);
			}
           #endif
            IALERT_StopAlerting(pMe->m_pAlert);
            notifyFMRadioAlertEvent( pMe, FALSE);
            ProfileNotifyMP3PlayerAlertEvent(pMe, FALSE);
            return TRUE;

        //当更改了menu选项,需要停止播放前一个铃声,开始播放现在选中的铃声
        case EVT_CTL_SEL_CHANGED:
            IALERT_StopAlerting(pMe->m_pAlert);
            //IALERT_StopSMSAlert(pMe->m_pAlert);
            //InitMenuIcons(pMenu);
            //SetMenuIcon(pMenu, wParam, TRUE);
            if(pMe->m_lastSmsRingPlayed != wParam)
            {
                // only start if it has changed and it isn't DOWNLOAD_MENU
                pMe->m_lastSmsRingPlayed = wParam;

                if(pMe->m_RingCurVol[pMe->m_CurProfile] != OEMSOUND_MUTE_VOL)
                {
                    //铃声预览
                    IALERT_StartSMSAlertPreview(pMe->m_pAlert,pMe->m_lastSmsRingPlayed);
                    //SoundMenu_StartRingerPreview(pMe,pMe->m_lastSmsRingPlayed);
                    //IRINGERMGR_Play(pMe->m_pRingerMgr, pMe->m_lastSmsRingPlayed, 0);
                }
            }
            return TRUE;

        case EVT_KEY:
            switch(wParam)
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
            ringID nNewConfigRinger[PROFILENUMBER];

            InitMenuIcons(pMenu);
            SetMenuIcon(pMenu, wParam, TRUE);
            ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_SMS_RINGER_ID,(void*)nNewConfigRinger, sizeof(nNewConfigRinger));

			MSG_FATAL("***zzg HandlerSmsRingDialogEvent wParam=%x***", wParam, 0, 0);
			
            if(wParam == IDS_GALLERY_PROFILE)
            {
#ifdef FEATURE_APP_MEDIAGALLERY            
                ExplorerPara sSoundPara={0};
                
                sSoundPara.nSelectProfile = pMe->m_CurProfile;

				MSG_FATAL("***zzg HandlerSmsRingDialogEvent wParam==OEMNV_MP3_RINGER***", 0, 0, 0);
                
                CMediaGallery_RegisterCallback((PFNMGSELECTEDCB)SoundMenu_SetMp3ring, pMe);
                CMediaGallery_FileExplorer(GALLERY_MUSIC_SETTING, &sSoundPara);            
#endif                
                //pMe->m_eCurState = SOUNDMENUST_SOUNDMENU;
                return TRUE;                
            }
            else
            {
            	MSG_FATAL("***zzg HandlerSmsRingDialogEvent wParam==OEMNV_MID_RINGER***", 0, 0, 0);
                nNewConfigRinger[pMe->m_CurProfile].midID = (uint16)(wParam);
                nNewConfigRinger[pMe->m_CurProfile].ringType = OEMNV_MID_RINGER;
            }
            ICONFIG_SetItem(pMe->m_pConfig,CFGI_PROFILE_SMS_RINGER_ID,(void*)nNewConfigRinger,sizeof(nNewConfigRinger));
            //从ISoundMenu_SoundList接口进入声音设置菜单，
            //或在已启动的情景模式中编辑，直接激活用户设置
            if (pMe->m_bNaturalStart == SOUNDLIST_ENTRY || pMe->m_active == pMe->m_CurProfile)
            {
                pMe->m_ScenemodesubType = SET_ACTIVATE;
                SoundMenu_SceneModeActivate(pMe);
            }
            CLOSE_DIALOG(DLGRET_MESSAGE)
            return TRUE;
        }


        default:
            break;
    }
    return FALSE;
} // HandleSmsRingDialogEvent
#endif

/*==============================================================================
函数：
       HandleVolumeDialogEvent
说明：
       IDD_VOLUME_MENU对话框事件处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleVolumeDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_VOLUME);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    SOUND_ERR("%x,%x,%x,HandleVolumeDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    //SoundMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSOUNDMENU_RES_FILE,                                
                        IDS_VOLUME_TITLE,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            Sound_App_Add_Menu(pMenu,IDS_RINGER_VOLUME_TITLE);
            Sound_App_Add_Menu(pMenu,IDS_KEYBEEP_VOLUME_TITLE);
            return TRUE;

        case EVT_DIALOG_START:
			#ifdef FEATURE_VERSION_K212
			{
				nv_item_type	SimChoice;
				SimChoice.sim_select =1;
				(void)OEMNV_Put(NV_SIM_SELECT_I,&SimChoice);
			}
            #endif
            // 给菜单各菜单项加数字编号图标
            SoundMenu_SetItemNumIcon(pMenu);

            // 返回此菜单的选择项
            IMENUCTL_SetSel(pMenu, pMe->m_fSubDlgId);

            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY|MP_ACTIVE_NO_REDRAW);
            IMENUCTL_SetOemProperties( pMenu , OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
            IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_APP_SOUNDMENU,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);
            return TRUE;

        case EVT_USER_REDRAW:
            //(void)IMENUCTL_Redraw(pMenu);   //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
		#ifdef FEATURE_VERSION_K212
			{
				nv_item_type	SimChoice;
				SimChoice.sim_select =2;
				(void)OEMNV_Put(NV_SIM_SELECT_I,&SimChoice);
			}
        #endif
            return TRUE;

        case EVT_KEY:
            switch(wParam)
            {
                case AVK_CLR:
                    CLOSE_DIALOG(DLGRET_CANCELED)
                    return TRUE;

                default:
                    break;
            }
            return TRUE;

        case EVT_COMMAND:
            //pMe->m_fSubDlgId = IMENUCTL_GetSel(pMenu);
            pMe->m_fSubDlgId = wParam;
            switch (wParam)
            {
                case IDS_RINGER_VOLUME_TITLE:     //铃声
                    pMe->m_VolType = SET_RINGER_VOL;
                    CLOSE_DIALOG(DLGRET_VOLUMESUB)
                    break;

                case IDS_EARPIECE_VOLUME_TITLE:   //耳机
                    pMe->m_VolType = SET_EARPIECE_VOL;
                    CLOSE_DIALOG(DLGRET_VOLUMESUB)
                    break;

                case IDS_KEYBEEP_VOLUME_TITLE:    //键盘
                    pMe->m_VolType = SET_KEYBEEP_VOL;
                    CLOSE_DIALOG(DLGRET_VOLUMESUB)
                    break;

                default:
                    ASSERT_NOT_REACHABLE;;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // HandleVolumeDialogEvent
/*==============================================================================
函数：
       HandleFMModeDialogEvent
说明：
       IDD_FMMODE_MENU对话框事件处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/

static boolean  HandleFMModeDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
	PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_MENU_FM);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            Sound_App_Add_Menu(pMenu,IDS_FMRADIO_PLAY_HANDSET);
            Sound_App_Add_Menu(pMenu,IDS_RADIO_PLAY_SPEAKER);
            return TRUE;

        case EVT_DIALOG_START:
            {
                boolean FMPlaymode = FALSE;
                uint16 ui16_return = IDS_FMRADIO_PLAY_HANDSET;
               	OEM_GetConfig(CFGI_FM_PLAY_MODE,&FMPlaymode, sizeof(FMPlaymode));
               	if(FMPlaymode)
               	{
               		ui16_return = IDS_RADIO_PLAY_SPEAKER;
               	}
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, ui16_return, TRUE);
                IMENUCTL_SetSel(pMenu, ui16_return);
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
                IMENUCTL_SetOemProperties( pMenu , OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                #if 0
                (void)IMENUCTL_SetTitle(pMenu,
                                        AEE_APPSSOUNDMENU_RES_FILE,
                                        pMe->m_wResID,
                                        NULL);
				#else
			    {
			  		AECHAR WTitle[40] = {0};
					(void)ISHELL_LoadResString(pMe->m_pShell,
	                        AEE_APPSSOUNDMENU_RES_FILE,                                
	                        pMe->m_wResID,
	                        WTitle,
	                        sizeof(WTitle));
					IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
			    }
			    #endif
                (void) ISHELL_PostEvent( pMe->m_pShell,
                                         AEECLSID_APP_SOUNDMENU,
                                         EVT_USER_REDRAW,
                                         0,
                                         0);
            }
            return TRUE;

        case EVT_USER_REDRAW:
            //(void)IMENUCTL_Redraw(pMenu);   //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
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
                
                boolean FMPlaymode = FALSE;

                switch (wParam)
                {
                    case IDS_FMRADIO_PLAY_HANDSET:      //关
                         FMPlaymode = FALSE;
                         break;

                    case IDS_RADIO_PLAY_SPEAKER:      //开
                         FMPlaymode = TRUE;
                         break;

                    default:
                        ASSERT_NOT_REACHABLE;
                }
                OEM_SetConfig(CFGI_FM_PLAY_MODE,&FMPlaymode, sizeof(FMPlaymode));
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wParam, TRUE);
                CLOSE_DIALOG(DLGRET_MESSAGE)

            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
}

/*==============================================================================
函数：
       HandleOtherSelDialogEvent
说明：
       IDD_OTHERSEL_MENU对话框事件处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleOtherSelDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_OTHERSEL);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    SOUND_ERR("%x,%x,%x,HandleOtherSelDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    //SoundMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            Sound_App_Add_Menu(pMenu,IDS_ITEM_ENABLE);
            Sound_App_Add_Menu(pMenu,IDS_ITEM_DISABLE);
            return TRUE;

        case EVT_DIALOG_START:
            {
                uint16 ui16_return = IDS_ITEM_ENABLE;
                byte   byte_return[PROFILENUMBER];
                switch(pMe->m_OtherVolType)
                {
                    case SET_MISSEDCALLALERT: //未接来电提醒
                        (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_PROFILE_MISSED_CALL_ALERT,
                                       byte_return,
                                       sizeof(byte_return));
                        pMe->m_wResID = IDS_MISSEDCALL_ALERT;
                        break;

                     case SET_POWERONOFFALERT: //开关机铃声提示
                        (void) ICONFIG_GetItem(pMe->m_pConfig,
                                       CFGI_PROFILE_POWERONOFF_ALERT,
                                       byte_return,
                                       sizeof(byte_return));
                        pMe->m_wResID = IDS_POWERONOFF_ALERT;
                        break;
                        
//                    case SET_STARTUP:     //开机音乐
//                        (void) ICONFIG_GetItem(pMe->m_pConfig,
//                                       CFGI_PROFILE_STARTUP_MUSIC,
//                                       byte_return,
//                                       sizeof(byte_return));
//                        pMe->m_wResID = IDS_STARTUP_MUSIC;
//                        break;
//
//                    case SET_SHUTDOWN:    //关机音乐
//                        (void) ICONFIG_GetItem(pMe->m_pConfig,
//                                       CFGI_PROFILE_SHUTDOWN_MUSIC,
//                                       byte_return,
//                                       sizeof(byte_return));
//                        pMe->m_wResID = IDS_SHUTDOWN_MUSIC;
//                        break;

                    default:
                        return FALSE;
                }
                switch(byte_return[pMe->m_CurProfile])
                {
                    case OEMNV_ALERT_DISABLE:           //关
                        ui16_return = IDS_ITEM_DISABLE;
                        break;

                    case OEMNV_ALERT_ENABLE:           //开
                    default:
                        ui16_return = IDS_ITEM_ENABLE;
                        break;
                }
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, ui16_return, TRUE);
                IMENUCTL_SetSel(pMenu, ui16_return);
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT|MP_ACTIVE_NO_REDRAW);
                IMENUCTL_SetOemProperties( pMenu , OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                #if 0
                (void)IMENUCTL_SetTitle(pMenu,
                                        AEE_APPSSOUNDMENU_RES_FILE,
                                        pMe->m_wResID,
                                        NULL);
				#else
			    {
			  		AECHAR WTitle[40] = {0};
					(void)ISHELL_LoadResString(pMe->m_pShell,
	                        AEE_APPSSOUNDMENU_RES_FILE,                                
	                        pMe->m_wResID,
	                        WTitle,
	                        sizeof(WTitle));
					IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
			    }
			    #endif
                (void) ISHELL_PostEvent( pMe->m_pShell,
                                         AEECLSID_APP_SOUNDMENU,
                                         EVT_USER_REDRAW,
                                         0,
                                         0);
            }
            return TRUE;

        case EVT_USER_REDRAW:
            //(void)IMENUCTL_Redraw(pMenu);    //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
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
                byte   btOtherVolSet[PROFILENUMBER] ;
                byte   byte_set[PROFILENUMBER] ;

                switch (wParam)
                {
                    case IDS_ITEM_DISABLE:      //关
                         btOtherVolSet[pMe->m_CurProfile] = OEMNV_ALERT_DISABLE;
                         break;

                    case IDS_ITEM_ENABLE:      //开
                         btOtherVolSet[pMe->m_CurProfile] = OEMNV_ALERT_ENABLE;
                         break;

                    default:
                        ASSERT_NOT_REACHABLE;
                }
                
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, wParam, TRUE);
                switch(pMe->m_OtherVolType)
                {
                    case SET_MISSEDCALLALERT: //未接来电提醒
                        (void) ICONFIG_GetItem(pMe->m_pConfig,
                                               CFGI_PROFILE_MISSED_CALL_ALERT,
                                               byte_set,
                                               sizeof(byte_set));
                         byte_set[pMe->m_CurProfile] = btOtherVolSet[pMe->m_CurProfile];
                         (void) ICONFIG_SetItem(pMe->m_pConfig,
                                               CFGI_PROFILE_MISSED_CALL_ALERT,
                                               byte_set,
                                               sizeof(byte_set));
                         break;

                     case SET_POWERONOFFALERT: //开关机铃声提示
                        (void) ICONFIG_GetItem(pMe->m_pConfig,
                                               CFGI_PROFILE_POWERONOFF_ALERT,
                                               byte_set,
                                               sizeof(byte_set));
                         byte_set[pMe->m_CurProfile] = btOtherVolSet[pMe->m_CurProfile];
                         (void) ICONFIG_SetItem(pMe->m_pConfig,
                                               CFGI_PROFILE_POWERONOFF_ALERT,
                                               byte_set,
                                               sizeof(byte_set));
                         break;

//                    case SET_STARTUP:       //开机音乐
//                         (void) ICONFIG_GetItem(pMe->m_pConfig,
//                                               CFGI_PROFILE_STARTUP_MUSIC,
//                                               byte_set,
//                                               sizeof(byte_set));
//                          byte_set[pMe->m_CurProfile] = btOtherVolSet[pMe->m_CurProfile];
//                         (void) ICONFIG_SetItem(pMe->m_pConfig,
//                                               CFGI_PROFILE_STARTUP_MUSIC,
//                                               byte_set,
//                                               sizeof(byte_set));
//                          break;
//
//                    case SET_SHUTDOWN:      //关机音乐
//                         (void) ICONFIG_GetItem(pMe->m_pConfig,
//                                               CFGI_PROFILE_SHUTDOWN_MUSIC,
//                                               byte_set,
//                                               sizeof(byte_set));
//                          byte_set[pMe->m_CurProfile] = btOtherVolSet[pMe->m_CurProfile];
//                         (void) ICONFIG_SetItem(pMe->m_pConfig,
//                                              CFGI_PROFILE_SHUTDOWN_MUSIC,
//                                              byte_set,
//                                              sizeof(byte_set));
//                         break;

                    default:
                         break;
                }
               //从ISoundMenu_SoundList接口进入声音设置菜单，或在已启动的情景模式中编辑，直接激活用户设置
               if (pMe->m_bNaturalStart == SOUNDLIST_ENTRY || pMe->m_active == pMe->m_CurProfile)
               {
                   pMe->m_ScenemodesubType = SET_ACTIVATE;
                   SoundMenu_SceneModeActivate(pMe);
               }
                CLOSE_DIALOG(DLGRET_MESSAGE)

            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // HandleOtherSelDialogEvent

/*==============================================================================
函数：
       HandleKeySndMenuDialogEvent
说明：
       IDD_KEYSND_MENU对话框事件处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleKeySndMenuDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    PARAM_NOT_REF(dwParam)

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                    IDC_KEYSND_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    SOUND_ERR("%x,%x,%x,HandleKeySndMenuDialogEvent",eCode,wParam,dwParam);
    //实现菜单循环滚动功能
    //SoundMenu_AutoScroll(pMenu,eCode,wParam);

    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			//add by yangdecai
			{
				AECHAR WTitle[40] = {0};
				(void)ISHELL_LoadResString(pMe->m_pShell,
                        AEE_APPSSOUNDMENU_RES_FILE,                                
                        IDS_COLORKEYSND,
                        WTitle,
                        sizeof(WTitle));
				IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
            }
            Sound_App_Add_Menu(pMenu,IDS_ITEM_TONE);
            Sound_App_Add_Menu(pMenu,IDS_ITEM_CLICK);
            //Sound_App_Add_Menu(pMenu,IDS_KEYSND_SILENT);
            return TRUE;

        case EVT_DIALOG_START:
            {
                uint16     ui16_return = IDS_ITEM_TONE;
                byte       KeySndType[PROFILENUMBER];

               (void) ICONFIG_GetItem(pMe->m_pConfig,
                                      CFGI_PROFILE_KEYSND_TYPE,
                                      KeySndType,
                                      sizeof(KeySndType));

                switch((snd_method_type)KeySndType[pMe->m_CurProfile])
                {
                    case SND_METHOD_KEY_BEEP:           //TONE
                        ui16_return = IDS_KEYSND_SILENT;
                        break;
                    default:
                        ui16_return = IDS_ITEM_TONE;
                        break;
#ifdef Temp_remove
                    case SND_METHOD_CLICK:             //CLICK
                        ui16_return = IDS_ITEM_CLICK;
                        break;
#endif/*Temp_remove*/
                }
                InitMenuIcons(pMenu);
                SetMenuIcon(pMenu, ui16_return, TRUE);
                IMENUCTL_SetSel(pMenu, ui16_return);
                IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_ACTIVE_NO_REDRAW);
                IMENUCTL_SetOemProperties( pMenu , OEMMP_USE_MENU_STYLE);
#ifdef FEATURE_CARRIER_CHINA_VERTU
                IMENUCTL_SetBackGround(pMenu, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
#endif
                IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
                (void) ISHELL_PostEvent( pMe->m_pShell,
                                         AEECLSID_APP_SOUNDMENU,
                                         EVT_USER_REDRAW,
                                         0,
                                         0);
            }
            return TRUE;

        case EVT_USER_REDRAW:
            //(void)IMENUCTL_Redraw(pMenu);   //dele by yangdecai
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_KEY:
            switch(wParam)
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
                byte byte_set[PROFILENUMBER];
                (void)ICONFIG_GetItem(pMe->m_pConfig,
                                      CFGI_PROFILE_KEYSND_TYPE,
                                      byte_set,
                                      sizeof(byte_set));

                switch (wParam)
                {
                    case IDS_ITEM_TONE:      //TONE
                        byte_set[pMe->m_CurProfile] = SND_METHOD_KEY_BEEP;
                        break;
#ifdef Temp_remove
                    case IDS_ITEM_CLICK:     //CLICK
                        byte_set[pMe->m_CurProfile] = SND_METHOD_CLICK;
                        break;
#endif/*Temp_remove*/

                    default:
                        ASSERT_NOT_REACHABLE;
                }

                (void)ICONFIG_SetItem(pMe->m_pConfig,
                                      CFGI_PROFILE_KEYSND_TYPE,
                                      byte_set,
                                      sizeof(byte_set));
                //从ISoundMenu_SoundList接口进入声音设置菜单，或在已启动的情景模式中编辑，直接激活用户设置
                if (pMe->m_bNaturalStart == SOUNDLIST_ENTRY || pMe->m_active == pMe->m_CurProfile)
                {
                    pMe->m_ScenemodesubType = SET_ACTIVATE;
                    SoundMenu_SceneModeActivate(pMe);
                }
                CLOSE_DIALOG(DLGRET_CANCELED)

            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} // HandleKeySndMenuDialogEvent

/*==============================================================================
函数：
       HandleWarningMessegeDialogEvent
说明：
       IDD_WARNING_MESSEGE对话框事件处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleWarningMessegeDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    //PARAM_NOT_REF(dwParam)
    static IStatic * pStatic = NULL;
    SOUND_ERR("%x,%x,%x,HandleWarningMessegeDialogEvent",eCode,wParam,dwParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
            if (NULL == pStatic)
            {
                AEERect rect = {0};
                if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_STATIC,(void **)&pStatic))

                {
                    SOUND_ERR("ISHELL_CreateInstance,AEECLSID_STATIC 2",0,0,0);
                    return FALSE;
                }
                ISTATIC_SetRect(pStatic, &rect);
            }
            return TRUE;

        case EVT_DIALOG_START:
            ISHELL_SetTimer(pMe->m_pShell,1000,SoundMenu_DialogTimeout,pMe);
            ISHELL_PostEvent( pMe->m_pShell,AEECLSID_APP_SOUNDMENU,EVT_USER_REDRAW,0,0);
            return TRUE;

        case EVT_USER_REDRAW:
        {
            PromptMsg_Param_type m_PromptMsg;
            MEMSET(&m_PromptMsg,0,sizeof(PromptMsg_Param_type));
            switch(pMe->m_sSubDlgId)
            {
                case IDS_ACTIVATING:
                case IDS_RESTORED:
                    m_PromptMsg.nMsgResID = pMe->m_sSubDlgId;
                    m_PromptMsg.ePMsgType = MESSAGE_WAITING;
                    m_PromptMsg.eBBarType = BTBAR_NONE;
                    SoundMenu_SceneModeActivate(pMe);
                    STRLCPY(m_PromptMsg.strMsgResFile, AEE_APPSSOUNDMENU_RES_FILE,MAX_FILE_NAME);
                    break;
/*                    
#ifdef FEATURE_VERSION_C316
                case IDS_WARNING_NO_BT_HEADSET:
                    m_PromptMsg.nMsgResID= pMe->m_sSubDlgId;
                    m_PromptMsg.ePMsgType = MESSAGE_WAITING;
                    m_PromptMsg.eBBarType = BTBAR_NONE;
                    STRLCPY(m_PromptMsg.strMsgResFile, AEE_APPSSOUNDMENU_RES_FILE,MAX_FILE_NAME);
                    break;
#endif
*/
                case IDS_CONFIRM_OFF_FM:
                case IDS_CONFIRM_OFF_MP:
                    ISHELL_CancelTimer(pMe->m_pShell, NULL,  pMe);
                    m_PromptMsg.nMsgResID = pMe->m_sSubDlgId;
                    m_PromptMsg.ePMsgType = MESSAGE_CONFIRM;
                    m_PromptMsg.eBBarType = BTBAR_OK_BACK;
                    STRLCPY(m_PromptMsg.strMsgResFile, AEE_APPSCOMMONRES_LANGFILE, MAX_FILE_NAME);
                    break;

                default:
                    m_PromptMsg.nMsgResID= IDS_WARNING_MESSEGE;
                    m_PromptMsg.ePMsgType = MESSAGE_INFORMATIVE;
                    m_PromptMsg.eBBarType = BTBAR_NONE;
                    STRLCPY(m_PromptMsg.strMsgResFile, AEE_APPSSOUNDMENU_RES_FILE,MAX_FILE_NAME);
                    break;
            }
            DrawPromptMessage(pMe->m_pDisplay,pStatic,&m_PromptMsg);
            IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
            return TRUE;
        }

        case EVT_DIALOG_END:
            ISTATIC_Release(pStatic);
            pStatic = NULL;
            ISHELL_CancelTimer(pMe->m_pShell, NULL,  pMe);
            return TRUE;

        case EVT_DISPLAYDIALOGTIMEOUT:
            if(pMe->m_sSubDlgId == IDS_ACTIVATING || pMe->m_sSubDlgId == IDS_RESTORED)
            {
                CLOSE_DIALOG(DLGRET_MSGBOX_OK)
            }
            else
            {
                CLOSE_DIALOG(DLGRET_OK)
            }
            return TRUE;

        case EVT_KEY:
            ISHELL_CancelTimer(pMe->m_pShell, NULL,  pMe);
            
            if(pMe->m_sSubDlgId == IDS_ACTIVATING || pMe->m_sSubDlgId == IDS_RESTORED)
            {
                CLOSE_DIALOG(DLGRET_MSGBOX_OK)
                return TRUE;
            }            
            
            switch(wParam)
            {
                case AVK_SELECT:
                    if(pMe->m_sSubDlgId == IDS_CONFIRM_OFF_FM || pMe->m_sSubDlgId == IDS_CONFIRM_OFF_MP)
                    {
                        CLOSE_DIALOG(DLGRET_MSGBOX_OK)
                    }
                    else
                    {
                        CLOSE_DIALOG(DLGRET_OK)
                    }
                    break;

                case AVK_CLR:
                    if(pMe->m_sSubDlgId == IDS_CONFIRM_OFF_FM || pMe->m_sSubDlgId == IDS_CONFIRM_OFF_MP)
                    {
                        CLOSE_DIALOG(DLGRET_MSGBOX_CANCEL)
                    }
                    else
                    {
                        CLOSE_DIALOG(DLGRET_OK)
                    }
                    break;                    
                    
                default:
                    CLOSE_DIALOG(DLGRET_OK)
                    break;
            }
            return TRUE;

        default:
            break;
    }

    return FALSE;
}//HandleWarningMessegeDialogEvent
#if 0
/*==============================================================================
函数：
       HandleMessageDialogEvent
说明：
       IDD_MESSAGE_DIALOG 对话框事件处理函数

参数：
       pMe [in]：指向SettingMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注：

==============================================================================*/
static boolean  HandleMessageDialogEvent(CSoundMenu *pMe,
    AEEEvent      eCode,
    uint16        wParam,
    uint32        dwParam
)
{
    //PARAM_NOT_REF(wParam)
    //PARAM_NOT_REF(dwParam)

    IStatic * pStatic = (IStatic *)IDIALOG_GetControl(pMe->m_pActiveDlg,
                                                      IDC_MESSAGE_STATIC);
    if (pStatic == NULL)
    {
        return FALSE;
    }
    SOUND_ERR("%x,%x,%x,HandleMessageDialogEvent",eCode,wParam,dwParam);
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
        {
            AEERect rect = {0};
            ISTATIC_SetRect(pStatic, &rect);
            return TRUE;
        }

        case EVT_DIALOG_START:
            ISHELL_SetTimer(pMe->m_pShell,2000,SoundMenu_DialogTimeout,pMe);
            ISHELL_PostEvent( pMe->m_pShell,AEECLSID_APP_SOUNDMENU,EVT_USER_REDRAW,0,0);
            return TRUE;

        case EVT_USER_REDRAW:
        {
            PromptMsg_Param_type m_PromptMsg;
            MEMSET(&m_PromptMsg,0,sizeof(PromptMsg_Param_type));
            if(pMe->m_ScenemodesubType == SET_ACTIVATE)
            {
                m_PromptMsg.nMsgResID= IDS_ACTIVATING;
            }
            else if(pMe->m_ScenemodesubType == SET_PROFILE_RESTORE)
            {
                m_PromptMsg.nMsgResID= IDS_RESTORED;
            }
            STRLCPY(m_PromptMsg.strMsgResFile, AEE_APPSSOUNDMENU_RES_FILE,MAX_FILE_NAME);
            m_PromptMsg.eBBarType = BTBAR_BACK;
            m_PromptMsg.ePMsgType = MESSAGE_WARNNING;
            DrawPromptMessage(pMe->m_pDisplay,pStatic,&m_PromptMsg);
            //启动情景模式
            SoundMenu_SceneModeActivate(pMe);
            IALERT_KeyBeep(pMe->m_pAlert, (AVKType) wParam, TRUE);
            IDISPLAY_UpdateEx(pMe->m_pDisplay,FALSE);
            return TRUE;
        }

        case EVT_DIALOG_END:
           (void)ISHELL_CancelTimer(pMe->m_pShell, NULL,  pMe);
           return TRUE;

        case EVT_DISPLAYDIALOGTIMEOUT:
            CLOSE_DIALOG(DLGRET_OK)
            return TRUE;

        case EVT_KEY:
           (void)ISHELL_CancelTimer(pMe->m_pShell, NULL,  pMe);
           CLOSE_DIALOG(DLGRET_OK)
            return TRUE;

        default:
            break;
    }

    return FALSE;
} // IDD_MESSAGE_DIALOG
#endif
/*==============================================================================
函数:
       SoundMenu_SceneModeActivate

说明:
       启动情景模式

参数:
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。

返回值:
       无。

备注:

==============================================================================*/
static void SoundMenu_SceneModeActivate(CSoundMenu *pMe)
{
     byte   return_alert_type[PROFILENUMBER],
            return_missed_call_alert[PROFILENUMBER],
            //return_headset_autoanswer[PROFILENUMBER],
            return_keysnd_type[PROFILENUMBER],
            /*return_keytone_length[PROFILENUMBER],*/
            return_ringer_level[PROFILENUMBER],
            return_handset_ear_level[PROFILENUMBER],
            return_beep_level[PROFILENUMBER], //;
            return_power_onoff[PROFILENUMBER];
    uint16 return_start_music[PROFILENUMBER];
    uint16 return_shutdown_music[PROFILENUMBER];

    ringID  return_call_ringer[PROFILENUMBER],
            return_alarm_ringer[PROFILENUMBER];

     byte   set_alert_type,
            set_missed_call_alert,
            //set_headset_autoanswer,
            set_keysnd_type,
            /*set_keytone_length,*/
            set_ringer_level,
            set_handset_ear_level,
            set_beep_level, //;
            set_power_onoff;

    ringID  set_call_ringer;
    ringID  set_alarm_ringer; 
    uint16  set_start_music,
            set_shutdown_music;
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
    byte    set_bt_sms_ringer, return_bt_sms_ringer[PROFILENUMBER];
    ringID  set_sms_ringer_id, return_sms_ringer_id[PROFILENUMBER];
#endif

//    byte    set_start_shutdown_music = 0x06;   //默认开机音乐，关机音乐为开
    if (NULL == pMe)
    {
        return;
    }
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                           CFGI_PROFILE_ALERT_TYPE,
                           return_alert_type,
                           sizeof(return_alert_type));

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                           CFGI_PROFILE_SMS_RINGER,
                           return_bt_sms_ringer,
                           sizeof(return_bt_sms_ringer));
#endif

    (void) ICONFIG_GetItem(pMe->m_pConfig,
                           CFGI_PROFILE_CALL_RINGER,
                           (void*)return_call_ringer,
                           sizeof(return_call_ringer));

     (void) ICONFIG_GetItem(pMe->m_pConfig,
                            CFGI_PROFILE_ALARM_RINGER,
                            (void*)return_alarm_ringer,
                            sizeof(return_alarm_ringer));

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
     (void) ICONFIG_GetItem(pMe->m_pConfig,
                            CFGI_PROFILE_SMS_RINGER_ID,
                            (void*)return_sms_ringer_id,
                            sizeof(return_sms_ringer_id));
#endif

     (void) ICONFIG_GetItem(pMe->m_pConfig,
                            CFGI_PROFILE_MISSED_CALL_ALERT,
                            return_missed_call_alert,
                            sizeof(return_missed_call_alert));

     (void) ICONFIG_GetItem(pMe->m_pConfig,
                            CFGI_PROFILE_STARTUP_MUSIC,
                            return_start_music,
                            sizeof(return_start_music));

     (void) ICONFIG_GetItem(pMe->m_pConfig,
                            CFGI_PROFILE_SHUTDOWN_MUSIC,
                            return_shutdown_music,
                            sizeof(return_shutdown_music));

     (void) ICONFIG_GetItem(pMe->m_pConfig,
                            CFGI_PROFILE_POWERONOFF_ALERT,
                            return_power_onoff,
                            sizeof(return_power_onoff));
#if 0
     (void) ICONFIG_GetItem(pMe->m_pConfig,
                            CFGI_PROFILE_HEADSET_AUTOANSWER,
                            return_headset_autoanswer,
                            sizeof(return_headset_autoanswer));
#endif
     (void) ICONFIG_GetItem(pMe->m_pConfig,
                            CFGI_PROFILE_KEYSND_TYPE,
                            return_keysnd_type,
                            sizeof(return_keysnd_type));
     //(void) ICONFIG_GetItem(pMe->m_pConfig,
     //                       CFGI_PROFILE_KEYTONE_LENGTH,
     //                       return_keytone_length,
     //                       sizeof(return_keytone_length));
     (void) ICONFIG_GetItem(pMe->m_pConfig,
                            CFGI_PROFILE_RINGER_VOL,
                            return_ringer_level,
                            sizeof(return_ringer_level));

     (void) ICONFIG_GetItem(pMe->m_pConfig,
                            CFGI_PROFILE_EAR_VOL,
                            return_handset_ear_level,
                            sizeof(return_handset_ear_level));

     (void) ICONFIG_GetItem(pMe->m_pConfig,
                            CFGI_PROFILE_BEEP_VOL,
                            return_beep_level,
                            sizeof(return_beep_level));

     if (pMe->m_ScenemodesubType == SET_ACTIVATE)
     {
          set_alert_type              =   return_alert_type[pMe->m_CurProfile];
          set_call_ringer             =   return_call_ringer[pMe->m_CurProfile];
          set_alarm_ringer            =   return_alarm_ringer[pMe->m_CurProfile];
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
          set_bt_sms_ringer           =   return_bt_sms_ringer[pMe->m_CurProfile];
          set_sms_ringer_id           =   return_sms_ringer_id[pMe->m_CurProfile];
#endif          
          set_missed_call_alert       =   return_missed_call_alert[pMe->m_CurProfile];
          set_power_onoff             =   return_power_onoff[pMe->m_CurProfile];
          set_start_music             =   return_start_music[pMe->m_CurProfile];
          set_shutdown_music          =   return_shutdown_music[pMe->m_CurProfile];
          //set_headset_autoanswer      =   return_headset_autoanswer[pMe->m_CurProfile];
          set_keysnd_type             =   return_keysnd_type[pMe->m_CurProfile];
          //set_keytone_length          =   return_keytone_length[pMe->m_CurProfile];
          set_ringer_level            =   return_ringer_level[pMe->m_CurProfile];
          set_handset_ear_level       =   return_handset_ear_level[pMe->m_CurProfile];
          set_beep_level              =   return_beep_level[pMe->m_CurProfile];


       (void) ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_PROFILE_CUR_NUMBER,
                              &pMe->m_CurProfile,
                              sizeof(byte));

        (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_ALERT_TYPE,
                              &set_alert_type,
                              sizeof(set_alert_type));
                              
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
        (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_SMS_RINGER,
                              &set_bt_sms_ringer,
                              sizeof(set_bt_sms_ringer));
#endif
        {
            uint16 alarm_id = 0;
            alarm_id = set_alarm_ringer.midID;
            
            (void)ICONFIG_SetItem(pMe->m_pConfig,
                                  CFGI_ALARM_RINGER,
                                  &alarm_id,
                                  sizeof(alarm_id));
        }

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
        {
            uint16 sms_id = 0;
            sms_id = set_sms_ringer_id.midID;
            
            (void)ICONFIG_SetItem(pMe->m_pConfig,
                                  CFGI_SMS_RINGER_ID,
                                  &sms_id,
                                  sizeof(sms_id));
        }
#endif

        (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_MISSED_CALL_ALERT,
                              &set_missed_call_alert,
                              sizeof(set_missed_call_alert));

        (void) ICONFIG_SetItem(pMe->m_pConfig,
                               CFGI_PROFILE_POWERONOFF_ALERT,
                               &set_power_onoff,
                               sizeof(set_power_onoff));

        (void) ICONFIG_SetItem(pMe->m_pConfig,
                               CFGI_PROFILE_POWERONOFF_ALERT,
                               &set_start_music,
                               sizeof(set_start_music));

        (void) ICONFIG_SetItem(pMe->m_pConfig,
                               CFGI_PROFILE_POWERONOFF_ALERT,
                               &set_shutdown_music,
                               sizeof(set_shutdown_music));
#if 0
        (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_HEADSET_AUTOANSWER,
                              &set_headset_autoanswer,
                              sizeof(set_headset_autoanswer));
#endif
        (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_KEYBEEP_SOUND,
                              &set_keysnd_type,
                              sizeof(set_keysnd_type));

        //(void)ICONFIG_SetItem(pMe->m_pConfig,
        //                      CFGI_KEYTONE_LENGTH,
        //                      &set_keytone_length,
        //                      sizeof(set_keytone_length));

        (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_RINGER_VOL,
                              &set_ringer_level,
                              sizeof(set_ringer_level));
        
/*
        (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_EAR_VOL,
                              &set_handset_ear_level,
                              sizeof(set_handset_ear_level));
*/

        (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_BEEP_VOL,
                              &set_beep_level,
                              sizeof(set_beep_level));

        (void) ICONFIG_GetItem(pMe->m_pConfig,
                              CFGI_PROFILE_CUR_NUMBER,
                              &pMe->m_active,
                              sizeof(pMe->m_active));

//        if(return_start_music[pMe->m_CurProfile] != 0)     //开
//        {
//                set_start_shutdown_music = set_start_shutdown_music|0x04;
//        }
//        else    //关
//        {
//                set_start_shutdown_music = set_start_shutdown_music&(~0x04);
//        }
//
//        if(return_shutdown_music[pMe->m_CurProfile] != 0)     //开
//        {
//                set_start_shutdown_music = set_start_shutdown_music|0x02;
//        }
//        else    //关
//        {
//                set_start_shutdown_music = set_start_shutdown_music&(~0x02);
//        }
//
//        (void) ICONFIG_SetItem(pMe->m_pConfig,
//                               CFGI_FLIP_SOUND,
//                               &set_start_shutdown_music,
//                               sizeof(set_start_shutdown_music));
     }

     if (pMe->m_ScenemodesubType == SET_PROFILE_RESTORE)
     {
          byte restore_alert_type[PROFILENUMBER]           =   OEMNV_ALERT_TYPE_INIT;
          ringID restore_call_ringer[PROFILENUMBER]        =   OEMNV_CALL_RINGER_INIT;
          ringID restore_alarm_ringer[PROFILENUMBER]       =   OEMNV_ALARM_RINGER_INIT;
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
          byte restore_bt_sms_ringer[PROFILENUMBER]        =   OEMNV_SMS_RINGER_INIT;
          ringID restore_sms_ringer_id[PROFILENUMBER]      =   OEMNV_SMS_RINGER_ID_INIT;
#endif          
          byte restore_missed_call_alert[PROFILENUMBER]    =   OEMNV_MISSED_CALL_ALERT_INIT;
          uint16 restore_start_music[PROFILENUMBER]          =   OEMNV_STARTUP_MUSIC_INIT;
          uint16 restore_shutdown_music[PROFILENUMBER]       =   OEMNV_SHUTDOWN_MUSIC_INIT;
          //byte restore_headset_autoanswer[PROFILENUMBER]   =   OEMNV_HEADSET_AUTOANSWER_INIT;
          byte restore_keysnd_type[PROFILENUMBER]          =   OEMNV_KEYSND_TYPE_INIT;
          //byte restore_keytone_length[PROFILENUMBER]       =   OEMNV_KEYTONE_LENGTH_INIT;
          byte restore_ringer_level[PROFILENUMBER]         =   OEMNV_RINGER_VOL_INIT;
          byte restore_handset_ear_level[PROFILENUMBER]    =   OEMNV_EAR_VOL_INIT;
          byte restore_beep_level[PROFILENUMBER]           =   OEMNV_BEEP_VOL_INIT;
          byte restore_power_onoff[PROFILENUMBER]           =   OEMNV_POWERONOFF_ALERT_INIT;
          return_alert_type[pMe->m_CurProfile]             =   restore_alert_type[pMe->m_CurProfile];
          return_call_ringer[pMe->m_CurProfile]            =   restore_call_ringer[pMe->m_CurProfile];
          return_alarm_ringer[pMe->m_CurProfile]           =   restore_alarm_ringer[pMe->m_CurProfile];
          
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
          return_bt_sms_ringer[pMe->m_CurProfile]          =   restore_bt_sms_ringer[pMe->m_CurProfile];
          return_sms_ringer_id[pMe->m_CurProfile]          =   restore_sms_ringer_id[pMe->m_CurProfile];
#endif
          
          return_missed_call_alert[pMe->m_CurProfile]      =   restore_missed_call_alert[pMe->m_CurProfile];
          return_power_onoff[pMe->m_CurProfile]            =   restore_power_onoff[pMe->m_CurProfile];
          return_start_music[pMe->m_CurProfile]            =   restore_start_music[pMe->m_CurProfile];
          return_shutdown_music[pMe->m_CurProfile]         =   restore_shutdown_music[pMe->m_CurProfile];
          //return_headset_autoanswer[pMe->m_CurProfile]     =   restore_headset_autoanswer[pMe->m_CurProfile];
          return_keysnd_type[pMe->m_CurProfile]            =   restore_keysnd_type[pMe->m_CurProfile];
          //return_keytone_length[pMe->m_CurProfile]         =   restore_keytone_length[pMe->m_CurProfile];
          return_ringer_level[pMe->m_CurProfile]           =   restore_ringer_level[pMe->m_CurProfile];
          return_handset_ear_level[pMe->m_CurProfile]      =   restore_handset_ear_level[pMe->m_CurProfile];
          return_beep_level[pMe->m_CurProfile]             =   restore_beep_level[pMe->m_CurProfile];

         (void)ICONFIG_SetItem(pMe->m_pConfig,
                               CFGI_PROFILE_ALERT_TYPE,
                               return_alert_type,
                               sizeof(return_alert_type));

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
         (void)ICONFIG_SetItem(pMe->m_pConfig,
                               CFGI_PROFILE_SMS_RINGER,
                               return_bt_sms_ringer,
                               sizeof(return_bt_sms_ringer));
#endif

         (void)ICONFIG_SetItem(pMe->m_pConfig,
                               CFGI_PROFILE_CALL_RINGER,
                               (void*)return_call_ringer,
                               sizeof(return_call_ringer));

         (void)ICONFIG_SetItem(pMe->m_pConfig,
                               CFGI_PROFILE_ALARM_RINGER,
                               (void*)return_alarm_ringer,
                               sizeof(return_alarm_ringer));

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
         (void)ICONFIG_SetItem(pMe->m_pConfig,
                               CFGI_PROFILE_SMS_RINGER_ID,
                               (void*)return_sms_ringer_id,
                               sizeof(return_sms_ringer_id));
#endif

         (void)ICONFIG_SetItem(pMe->m_pConfig,
                               CFGI_PROFILE_MISSED_CALL_ALERT,
                               return_missed_call_alert,
                               sizeof(return_missed_call_alert));

         (void)ICONFIG_SetItem(pMe->m_pConfig,
                               CFGI_PROFILE_STARTUP_MUSIC,
                               return_start_music,
                               sizeof(return_start_music));

         (void)ICONFIG_SetItem(pMe->m_pConfig,
                               CFGI_PROFILE_SHUTDOWN_MUSIC,
                               return_shutdown_music,
                               sizeof(return_shutdown_music));
#if 0
         (void)ICONFIG_SetItem(pMe->m_pConfig,
                               CFGI_PROFILE_HEADSET_AUTOANSWER,
                               return_headset_autoanswer,
                               sizeof(return_headset_autoanswer));
#endif
         (void)ICONFIG_SetItem(pMe->m_pConfig,
                               CFGI_PROFILE_KEYSND_TYPE,
                               return_keysnd_type,
                               sizeof(return_keysnd_type));
         //(void)ICONFIG_SetItem(pMe->m_pConfig,
         //                      CFGI_PROFILE_KEYTONE_LENGTH,
         //                      return_keytone_length,
         //                      sizeof(return_keytone_length));
         (void)ICONFIG_SetItem(pMe->m_pConfig,
                               CFGI_PROFILE_RINGER_VOL,
                               return_ringer_level,
                               sizeof(return_ringer_level));

         (void)ICONFIG_SetItem(pMe->m_pConfig,
                               CFGI_PROFILE_EAR_VOL,
                               return_handset_ear_level,
                               sizeof(return_handset_ear_level));

         (void)ICONFIG_SetItem(pMe->m_pConfig,
                               CFGI_PROFILE_BEEP_VOL,
                               return_beep_level,
                               sizeof(return_beep_level));
         (void)ICONFIG_SetItem(pMe->m_pConfig,
                               CFGI_PROFILE_POWERONOFF_ALERT,
                               return_power_onoff, // restore_power_onoff, 
                               sizeof(return_power_onoff)); // sizeof(restore_power_onoff));
         //在已启动的情景模式中恢复默认设置后，直接激活用户设置。
         if (pMe->m_active == pMe->m_CurProfile)
         {
             pMe->m_ScenemodesubType = SET_ACTIVATE;
             SoundMenu_SceneModeActivate(pMe);
         }
      }

#if 0
      {
          byte alertType;

          (void) ICONFIG_GetItem(pMe->m_pConfig,
                                         CFGI_ALERT_TYPE,
                                         &alertType,
                                         sizeof(alertType));
          switch(alertType)
          {
              case OEMNV_ALERTTYPE_OFF :
                  //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_SILENT, ANNUN_STATE_ON);
                  IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_SILENT);
                  break;

              case OEMNV_ALERTTYPE_RINGER :
                  //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RING, ANNUN_STATE_ON);
                  IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_ALERT);
                  break;

              case OEMNV_ALERTTYPE_VIB :
                  //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_VIBRATE, ANNUN_STATE_ON);
                  IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_VIBRATOR);
                  break;

              case OEMNV_ALERTTYPE_VIBRINGER :
                  //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_ALERT, ANNUN_STATE_ON);
                  IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_VIBRING);
                  break;
                  
              case OEMNV_ALERTTYPE_VIBANDRINGER :
                  //IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_ALERT, ANNUN_STATE_ON);
                  IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_VIBRING);
                  break;
              default :
                  break;
          }
      }
#endif    
		
      switch(pMe->m_active)
      {
          case OEMNV_PROFILE_NORMALMODE:          //正常模式
              IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_ALERT);
              break;
  
          case OEMNV_PROFILE_QUIETMODE:           //安静模式
              IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_SILENT);
              break;
          case OEMNV_PROFILE_MEETING:             //会议模式
              IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_VIBRATOR);
              break;
          case OEMNV_PROFILE_NOISEMODE:           //户外模式
              IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_VIBRING);
              break;
  
          case OEMNV_PROFILE_CARMODE:             //车载模式
              IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_VIBRING);
              break;
/*
#ifdef FEATURE_VERSION_C316
          case OEMNV_PROFILE_BLUETOOTH:             //蓝牙模式
              IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_BLUETOOTH);
        break;
#endif
*/
          default:
              break;
      }

	/*
	//Add By zzg 2010_08_31
	{		
	    byte      byte_set[PROFILENUMBER];
	    

	    (void) ICONFIG_GetItem(pMe->m_pConfig,
	                           CFGI_PROFILE_ALERT_TYPE,		//CFGI_PROFILE_SMS_RINGER
	                           byte_set,
	                           sizeof(byte_set));	
        
	    switch(byte_set[pMe->m_CurProfile])     
	    {
			case OEMNV_SMS_OFF:
			{
				IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_SILENT);
				break;
			}
			case OEMNV_SMS_RING:
			{
				IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_ALERT);
				break;
			}
			case OEMNV_SMS_VIBONLY:
			{
				IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_VIBRATOR);
				break;
			}
			case OEMNV_SMS_RINGVIB:
			{
				IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_VIBRING);
				break;
			}
			case OEMNV_ALERTTYPE_VIBANDRINGER:
			{
				IANNUNCIATOR_SetField (pMe->m_pIAnn, ANNUN_FIELD_RINGTONE, ANNUN_STATE_RINGTONE_VIBRING);
				break;
			}
			default:
			{
				break;
			}
	    }
	}                                       
	//Add End
	*/
      
 #ifdef FEATURE_APP_MUSICPLAYER
    ProfileNotifyMP3PlayerAlertEvent(pMe,FALSE);
 #endif
    {
        boolean b_FMBackground = FALSE;
        byte mute = OEMSOUND_MUTE_VOL;        
        OEM_GetConfig(CFGI_FM_BACKGROUND,&b_FMBackground, sizeof(b_FMBackground));
        if(TRUE == b_FMBackground)
        {
            OEM_SetConfig( CFGI_BEEP_VOL, &mute, sizeof(byte));
        }
    }
}

/*==============================================================================
函数:
       SoundMenu_DialogTimeout

说明:
       定时器回调函数。主要用于自动关闭消息对话框。

参数:
       pme [in]：void *类型指针，暗指CSoundMenu结构指针。

返回值:
       无。

备注:

==============================================================================*/
static void SoundMenu_DialogTimeout(void *pme)
{
    CSoundMenu *pMe = (CSoundMenu *)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent(pMe->m_pShell,
                            AEECLSID_APP_SOUNDMENU,
                            EVT_DISPLAYDIALOGTIMEOUT,
                            0,
                            0);
}


/*==============================================================================
函数：
       RingerPreview

说明：
       播放铃声

参数：
       pMe [in]：指向 Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       无。

备注：:

==============================================================================*/
static void RingerPreview(void *pUser)
{
    CSoundMenu *pMe = (CSoundMenu*)pUser;
    (void) ICONFIG_GetItem(pMe->m_pConfig,
                                            CFGI_PROFILE_RINGER_VOL,
                                            pMe->m_RingCurVol,
                                            sizeof(pMe->m_RingCurVol));
    (void)ICONFIG_SetItem(pMe->m_pConfig,
                                            CFGI_RINGER_VOL,
                                            &pMe->m_RingCurVol[pMe->m_CurProfile],
                                            sizeof(pMe->m_RingCurVol[pMe->m_CurProfile]));
	//MODI BY YANGDECAI 09-27
	MSG_FATAL("pMe->m_slecet_id ::::::::::%d",pMe->m_slecet_id ,0,0);
	if(pMe->m_slecet_id == POWERONRINGID || pMe->m_slecet_id == POWEROFFRINGID)
	{
		IALERT_StartRingerAlert_Ex(pMe->m_pAlert,pMe->m_lastRingerPlayed);
	}
	else
	{
    	IALERT_StartRingerAlert_Ex(pMe->m_pAlert,pMe->m_lastRingerPlayed);
	}
}

/*==============================================================================
函数：
       VolumePreview

说明：
       播放铃声

参数：
       pMe [in]：指向 Applet对象结构的指针。该结构包含小程序的特定信息。

返回值：
       无。

备注：:

==============================================================================*/
static void VolumePreview(void *pUser)
{	
    CSoundMenu *pMe = (CSoundMenu*)pUser;
    ringID        ringerID[PROFILENUMBER];

	MSG_FATAL("***zzg VolumePreview***", 0, 0, 0);

	MSG_FATAL("***zzg VolumePreview ringType=%x***", ringerID[pMe->m_CurProfile].ringType, 0, 0);

#ifdef FEATURE_VERSION_W317A
	if(AEE_SUCCESS != ICONFIG_GetItem(pMe->m_pConfig,  CFGI_PROFILE_CALL_RINGER, (void*)ringerID, sizeof(ringerID)))
    {
        ringerID[pMe->m_CurProfile].ringType = OEMNV_MP3_RINGER;
        pMe->m_RingerID[pMe->m_CurProfile].midID = OEMNV_DEFAULTRINGER;
    }

	IALERT_StartMp3Preview(pMe->m_pAlert, pMe->m_RingerID[pMe->m_CurProfile].szMusicname);
#else
	if(AEE_SUCCESS != ICONFIG_GetItem(pMe->m_pConfig,  CFGI_PROFILE_CALL_RINGER, (void*)ringerID, sizeof(ringerID)))
    {
        ringerID[pMe->m_CurProfile].ringType = OEMNV_MID_RINGER;
        pMe->m_RingerID[pMe->m_CurProfile].midID = OEMNV_DEFAULTRINGER;
    }
	
	if(ringerID[pMe->m_CurProfile].ringType == OEMNV_MID_RINGER)
    {
#if defined( FEATURE_VERSION_C316) || defined( FEATURE_VERSION_C310)
        pMe->m_RingerID[pMe->m_CurProfile].midID = 1;
#else   
        if(pMe->m_RingerID[pMe->m_CurProfile].midID == 0)
        {
            pMe->m_RingerID[pMe->m_CurProfile].midID = OEMNV_DEFAULTRINGER;
        }
#endif
		MSG_FATAL("***zzg VolumePreview OEMNV_MID_RINGER m_RingerID=%x***", pMe->m_RingerID[pMe->m_CurProfile].midID, 0, 0);
		
		MSG_FATAL("***zzg VolumePreview IALERT_StartRingerPreview***", 0, 0, 0);
		#ifdef FEATURE_VERSION_K202_LM129C
		IALERT_StartRingerAlert(pMe->m_pAlert, OEMNV_DEFAULTRINGER);
		#else
        IALERT_StartRingerPreview(pMe->m_pAlert,pMe->m_RingerID[pMe->m_CurProfile].midID);
		#endif
    }
    else
    {
    	MSG_FATAL("***zzg VolumePreview IALERT_StartMp3Preview***", 0, 0, 0);
        IALERT_StartMp3Preview(pMe->m_pAlert, pMe->m_RingerID[pMe->m_CurProfile].szMusicname);
    }
#endif   
}

static void SoundMenu_SetItemNumIcon(IMenuCtl   *pMenu)
{
}

//static void SoundMenu_StartRingerPreview(void *pUser,uint16 ring_id)
//{
//    CSoundMenu *pMe = (CSoundMenu*)pUser;
//    IRINGERMGR_Play(pMe->m_pRingerMgr, ring_id, 0);
//}
static void Sound_App_Add_Menu(IMenuCtl *pMenu,uint16 id)
{
    IMENUCTL_AddItem(pMenu, AEE_APPSSOUNDMENU_RES_FILE, id, id, NULL, 0);
}

static void notifyFMRadioAlertEvent( CSoundMenu *pMe, boolean toStartAlert)
{
        ISHELL_SendEvent( pMe->m_pShell,
                          AEECLSID_APP_FMRADIO,
                          EVT_ALARM,
                          toStartAlert,
                          0
                        );
    }
static void ProfileNotifyMP3PlayerAlertEvent(CSoundMenu *pMe, boolean toStartAlert)
{
	MSG_FATAL("***zzg ProfileNotifyMP3PlayerAlertEvent m_bSuspending=%x***", pMe->m_bSuspending, 0, 0);
	
 #ifdef FEATURE_APP_MUSICPLAYER
    if(pMe->m_bSuspending)
    {
        //当为挂起状态时，只有MP3在后台播放，且toStartAlert为FALSE时
        //才可以发送EVT_ALARM，这是为了避免来电话挂起且toStartAlert为TRUE时
        //，在接电话的同时播放MP3
        if((GetMp3PlayerStatus() == MP3STATUS_RUNONBACKGROUND) && !toStartAlert)
        {
        	MSG_FATAL("***zzg ProfileNotifyMP3PlayerAlertEvent ISHELL_SendEvent EVT_ALARM***", 0, 0, 0);
            ISHELL_SendEvent(pMe->m_pShell,
                             AEECLSID_APP_MUSICPLAYER,
                             EVT_ALARM,
                             toStartAlert,
                             TRUE);
        }            
        return;
    }
    if(GetMp3PlayerStatus() == MP3STATUS_RUNONBACKGROUND)
    {
    	MSG_FATAL("***zzg ProfileNotifyMP3PlayerAlertEvent MP3STATUS_RUNONBACKGROUND EVT_ALARM***", 0, 0, 0);
        ISHELL_SendEvent(pMe->m_pShell,
                         AEECLSID_APP_MUSICPLAYER,
                         EVT_ALARM,
                         toStartAlert,
                         TRUE);
    }
 #endif
}
/*==============================================================================
函数：
       HandleVolumeSubDialogEvent
说明：
       IDD_VOLUME_SUB对话框事件处理函数

参数：
       pMe [in]：指向SoundMenu Applet对象结构的指针。该结构包含小程序的特定信息。
       eCode [in]：事件代码。
       wParam：事件相关数据。
       dwParam：事件相关数据。

返回值：
       TRUE：传入事件被处理。
       FALSE：传入事件被忽略。

备注： 用图片显示音量级别

==============================================================================*/
static boolean  HandleVolumeSubDialogEvent(CSoundMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
    SOUND_ERR("eCode %x,  wParam %x, dwParam %x",eCode,wParam,dwParam);
	
    switch(eCode)
    {
        case EVT_DIALOG_INIT:
            notifyFMRadioAlertEvent( pMe, TRUE);
            pMe->m_sSubDlgId = 0;
            return TRUE;

        case EVT_DIALOG_START:
            {
                byte   byte_set[PROFILENUMBER];
                
                //ICONFIG_GetItem(pMe->m_pConfig,CFGI_RINGER_VOL,&pMe->m_RingVol,sizeof(pMe->m_RingVol));

                ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_CALL_RINGER,(void*)pMe->m_RingerID,sizeof(pMe->m_RingerID));

                ICONFIG_GetItem(pMe->m_pConfig,CFGI_BEEP_VOL,&pMe->m_btTep,sizeof(pMe->m_btTep));                

                if (pMe->m_RingerID[pMe->m_CurProfile].midID == 0)
                {
                    pMe->m_RingerID[pMe->m_CurProfile].midID = OEMNV_DEFAULTRINGER;
                }
                switch(pMe->m_VolType)
                {
                    case SET_RINGER_VOL:      //铃声音量
                        pMe->m_wResID = IDS_RINGER_VOLUME_TITLE;
                        ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_RINGER_VOL,byte_set,sizeof(byte_set));
                        ICONFIG_SetItem(pMe->m_pConfig,CFGI_RINGER_VOL,&byte_set[pMe->m_CurProfile],sizeof(byte));
                        break;

                    case SET_KEYBEEP_VOL:      //键盘音量
                        pMe->m_wResID = IDS_KEYBEEP_VOLUME_TITLE;
                        ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_BEEP_VOL, byte_set,sizeof(byte_set));
                        
                        ICONFIG_SetItem(pMe->m_pConfig,CFGI_BEEP_VOL, &byte_set[pMe->m_CurProfile],sizeof(byte));

                        break;

                    default:
                        return FALSE;
                }

				MSG_FATAL("***zzg SoundMenuDialog m_VolType=%x***", pMe->m_VolType, 0, 0);
				
                ProfileNotifyMP3PlayerAlertEvent(pMe, TRUE);
                ISHELL_PostEvent( pMe->m_pShell,AEECLSID_APP_SOUNDMENU,EVT_USER_REDRAW,0,0);
                return TRUE;
            }

        case EVT_USER_REDRAW:
            {
                //uint16    ui16_return = IDI_ICON_AUDIO_VOL1;
                int imageIndex = 0;
                IImage*   pRingLevel = NULL;
                AEERect   rect = {0};
                AEEImageInfo ImageSize;
                byte        byte_return;

				MSG_FATAL("***zzg SoundMenuDialogHandler EVT_USER_REDRAW***", 0, 0, 0);
                
                switch(pMe->m_VolType)
                {
                    case SET_RINGER_VOL:
                        ICONFIG_GetItem(pMe->m_pConfig,CFGI_RINGER_VOL,&byte_return,sizeof(byte_return));
                        break;

                    case SET_KEYBEEP_VOL:
                        ICONFIG_GetItem(pMe->m_pConfig,CFGI_BEEP_VOL,&byte_return,sizeof(byte_return));
                        break;

                    default:
                        return FALSE;
                }
                
                switch(byte_return)
                {
                    case OEMSOUND_1ST_VOL:      //音量为“1级”
                        //ui16_return = IDI_ICON_AUDIO_VOL0;
                        imageIndex = 1;
                        break;
                
                    case OEMSOUND_2ND_VOL:      //音量为“2级”
                    default:
                        //ui16_return = IDI_ICON_AUDIO_VOL1;
                        imageIndex = 2;
                        break;
                
                    case OEMSOUND_3RD_VOL:      //音量为“3级”
                        //ui16_return = IDI_ICON_AUDIO_VOL2;
                        imageIndex = 3;
                        break;
                
                    case OEMSOUND_4TH_VOL:      //音量为“4级”
                        //ui16_return = IDI_ICON_AUDIO_VOL3;
                        imageIndex = 4;
                        break;
                
                    case OEMSOUND_5TH_VOL:      //音量为“5级”
                        //ui16_return = IDI_ICON_AUDIO_VOL4;
                        imageIndex = 5;
                        break;
                
                    case OEMSOUND_MUTE_VOL:      //音量为“关闭”
                        //ui16_return = IDI_ICON_AUDIO_VOL_OFF;
                        imageIndex = 0;
                        break;
                }             
                pRingLevel = ISHELL_LoadImage(pMe->m_pShell, SOUNDMENU_VOLUME_IMAGE);

                SETAEERECT(&rect, 
                            0, 
                            0, 
                            pMe->m_rc.dx,
                            pMe->m_rc.dy  - BOTTOMBAR_HEIGHT);
                
#ifdef FEATURE_CARRIER_CHINA_VERTU
                {
                    IImage *pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_SETTING_BACKGROUND);
                    
                    Appscommon_ResetBackground(pMe->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &rect, 0, 0);
                    if(pImageBg != NULL)
                    {
                        IImage_Release(pImageBg);
                    }
                }
#else
                Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &rect, TRUE);
#endif

                if(pRingLevel)
                {
                    IIMAGE_SetParm(pRingLevel, IPARM_NFRAMES, SOUNDMENU_VOLUME_LEVELS, 0);
                    IIMAGE_GetInfo(pRingLevel, &ImageSize);
                    IIMAGE_DrawFrame(pRingLevel, 
                                        imageIndex, 
                                        (rect.dx - ImageSize.cxFrame)/2, 
                                        rect.y + (rect.dy - ImageSize.cy)/2);
                    IIMAGE_Release(pRingLevel);
                    pRingLevel = NULL;
                }
                
                //draw title bar
                {
                    TitleBar_Param_type TitleBar;
                    
                    AECHAR wszTitle[16] = {0}; 
                    MEMSET(&TitleBar, NULL, sizeof(TitleBar_Param_type));
                    ISHELL_LoadResString(pMe->m_pShell,AEE_APPSSOUNDMENU_RES_FILE,pMe->m_wResID, wszTitle, sizeof(wszTitle));
                    TitleBar.dwAlignFlags  = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE;
                    TitleBar.pwszTitle = wszTitle;
					#if 0
                    DrawTitleBar(pMe->m_pDisplay, &TitleBar);
					#else
					IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,wszTitle);
					#endif
                }
                
                //draw bottom bar
                {
                    BottomBar_Param_type BottomBar;
                    
                    MEMSET(&BottomBar, NULL, sizeof(BottomBar_Param_type));
                    BottomBar.eBBarType = BTBAR_SELECT_BACK;
                    #ifdef FEATURE_VERSION_SKY
                    BottomBar.nImgResID = IDI_BOTTOMBARCONT;
                    #endif
                    DrawBottomBar(pMe->m_pDisplay, &BottomBar);
                }
                
                IDISPLAY_Update(pMe->m_pDisplay);

				MSG_FATAL("***zzg SoundMenuDialogHandler pMe->m_VolType=%x***", pMe->m_VolType, 0, 0);
				
                if (pMe->m_VolType == SET_RINGER_VOL /*&& byte_return != OEMSOUND_MUTE_VOL*/)
                {
                    IALERT_StopRingerAlert(pMe->m_pAlert);
                    IALERT_StopMp3Alert(pMe->m_pAlert);
                    ISHELL_SetTimer(pMe->m_pShell, 400, VolumePreview, pMe);
                }
                return TRUE;
            }

        case EVT_DIALOG_END:
            {
                byte btVolLevel[PROFILENUMBER];
                byte profilenum;

                ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_CUR_NUMBER, &profilenum, sizeof(profilenum));
                
                switch(pMe->m_VolType)
                {
                    case SET_RINGER_VOL:
                        ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_RINGER_VOL,btVolLevel,sizeof(btVolLevel));
                        ICONFIG_SetItem(pMe->m_pConfig,CFGI_RINGER_VOL,&btVolLevel[profilenum],sizeof(byte));
                        break;

                    case SET_KEYBEEP_VOL:
                        ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_BEEP_VOL,btVolLevel,sizeof(btVolLevel));
                        ICONFIG_SetItem(pMe->m_pConfig,CFGI_BEEP_VOL,&btVolLevel[profilenum],sizeof(byte));
                        break;

                    default:
                        return FALSE;
                }
            }

            ISHELL_CancelTimer(pMe->m_pShell, VolumePreview, pMe);
            IALERT_StopRingerAlert(pMe->m_pAlert);
            IALERT_StopMp3Alert(pMe->m_pAlert);
            notifyFMRadioAlertEvent( pMe, FALSE);
            ProfileNotifyMP3PlayerAlertEvent(pMe, FALSE);

            return TRUE;

        case EVT_KEY:
            {
                byte byte_change;
                byte   btVolLevel[PROFILENUMBER];
                
                if (pMe->m_VolType == SET_RINGER_VOL)
                {
                    IALERT_StopAlerting(pMe->m_pAlert);
                }
                switch(wParam)
                {
                    
                    case AVK_CLR: 
                        CLOSE_DIALOG(DLGRET_CANCELED)
                        return TRUE;

                    case AVK_SELECT:
                        
                        switch(pMe->m_VolType)
                        {
                            case SET_RINGER_VOL:
                                ICONFIG_GetItem(pMe->m_pConfig,CFGI_RINGER_VOL,&byte_change,sizeof(byte_change));
                                ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_RINGER_VOL,btVolLevel,sizeof(btVolLevel));
                                btVolLevel[pMe->m_CurProfile] = byte_change;
                                ICONFIG_SetItem(pMe->m_pConfig,CFGI_PROFILE_RINGER_VOL,&btVolLevel,sizeof(btVolLevel));
                                break;
                        
                            case SET_KEYBEEP_VOL:
                                ICONFIG_GetItem(pMe->m_pConfig,CFGI_BEEP_VOL,&byte_change,sizeof(byte_change));
                                ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_BEEP_VOL,btVolLevel,sizeof(btVolLevel));
                                btVolLevel[pMe->m_CurProfile] = byte_change;
                                ICONFIG_SetItem(pMe->m_pConfig,CFGI_PROFILE_BEEP_VOL,&btVolLevel,sizeof(btVolLevel));                                
                                break;
                        
                            default:
                                return FALSE;
                        }

                        ISHELL_CancelTimer(pMe->m_pShell, VolumePreview, pMe);
                        CLOSE_DIALOG(DLGRET_MESSAGE)
                        return TRUE;

                    case AVK_LEFT:
                    case AVK_DOWN:
					case AVK_VOLUME_DOWN:
                        switch(pMe->m_VolType)
                        {
                            case SET_RINGER_VOL:
                                ICONFIG_GetItem(pMe->m_pConfig, CFGI_RINGER_VOL, &byte_change, sizeof(byte_change));
                                if(byte_change != OEMSOUND_MUTE_VOL)
                                {
                                    byte_change --;
                                    ICONFIG_SetItem(pMe->m_pConfig,CFGI_RINGER_VOL,&byte_change,sizeof(byte_change));
                                }
                                break;

                            case SET_KEYBEEP_VOL:
                                ICONFIG_GetItem(pMe->m_pConfig, CFGI_BEEP_VOL, &byte_change, sizeof(byte_change));
                                if(byte_change != OEMSOUND_MUTE_VOL)
                                {
                                    byte_change --;
                                    ICONFIG_SetItem(pMe->m_pConfig,CFGI_BEEP_VOL,&byte_change,sizeof(byte_change));
                                }
                                break;
                        }
                        break;

                    case AVK_UP:
                    case AVK_RIGHT:
					case AVK_VOLUME_UP:
                        switch(pMe->m_VolType)
                        {
                            case SET_RINGER_VOL:
                                ICONFIG_GetItem(pMe->m_pConfig, CFGI_RINGER_VOL, &byte_change, sizeof(byte_change));
                                if(byte_change != OEMSOUND_5TH_VOL)
                                {
                                    byte_change ++;
                                    ICONFIG_SetItem(pMe->m_pConfig,CFGI_RINGER_VOL,&byte_change,sizeof(byte_change));
                                }
                                break;

                            case SET_KEYBEEP_VOL:
                                ICONFIG_GetItem(pMe->m_pConfig, CFGI_BEEP_VOL, &byte_change, sizeof(byte_change));
                                if(byte_change != OEMSOUND_5TH_VOL)
                                {
                                    byte_change ++;
                                    ICONFIG_SetItem(pMe->m_pConfig,CFGI_BEEP_VOL,&byte_change,sizeof(byte_change));
                                }
                                break;
                        }
                        break;

                         
                    default:
                        break;
                }
                ISHELL_PostEvent( pMe->m_pShell,AEECLSID_APP_SOUNDMENU,EVT_USER_REDRAW,0,0);
                return TRUE;
            }
        
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

				if(SOUNDMENU_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_SOUNDMENU,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//左
					{
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_SOUNDMENU,EVT_USER,AVK_INFO,0);
						 return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//左
					{						
						 boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_APP_SOUNDMENU,EVT_USER,AVK_CLR,0);
						 return rt;
					}
				}

			}
			break;
#endif //FEATURE_LCD_TOUCH_ENABLE
        default:
            break;
    }
    return FALSE;
}
int SoundMenu_SetMp3ring(void* pv, FileNamesBuf pBuf, uint32 nBufSize)
{
    CSoundMenu *pMe = (CSoundMenu*)pv;
    ringID nNewConfigRinger[PROFILENUMBER];

	MSG_FATAL("***zzg SoundMenu_SetMp3Ring m_RingerType=%x***", pMe->m_RingerType, 0, 0);
    
    if(NULL == pMe)
    {
        return EBADPARM;
    }
        
    if(pMe->m_RingerType == SET_RINGER)               // 来电铃声
    {
        ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_CALL_RINGER,(void*)nNewConfigRinger,sizeof(nNewConfigRinger));
        STRCPY(nNewConfigRinger[pMe->m_CurProfile].szMusicname, (char*)pBuf); 
        nNewConfigRinger[pMe->m_CurProfile].ringType = OEMNV_MP3_RINGER; 
        ICONFIG_SetItem(pMe->m_pConfig,CFGI_PROFILE_CALL_RINGER,(void*)nNewConfigRinger,sizeof(nNewConfigRinger));
    }
    else if(pMe->m_RingerType == SET_ALARMRING)       // 闹钟铃声
    {
        ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_ALARM_RINGER, (void*)nNewConfigRinger,sizeof(nNewConfigRinger));
        STRCPY(nNewConfigRinger[pMe->m_CurProfile].szMusicname, (char*)pBuf); 
        nNewConfigRinger[pMe->m_CurProfile].ringType = OEMNV_MP3_RINGER; 
        ICONFIG_SetItem(pMe->m_pConfig, CFGI_PROFILE_ALARM_RINGER,(void*)nNewConfigRinger,sizeof(nNewConfigRinger));
    }
    else if(pMe->m_RingerType == SET_SMSTONE)          // 短信铃声
    {
        ICONFIG_GetItem(pMe->m_pConfig,CFGI_PROFILE_SMS_RINGER_ID, (void*)nNewConfigRinger,sizeof(nNewConfigRinger));
        STRCPY(nNewConfigRinger[pMe->m_CurProfile].szMusicname, (char*)pBuf); 
        nNewConfigRinger[pMe->m_CurProfile].ringType = OEMNV_MP3_RINGER; 
        ICONFIG_SetItem(pMe->m_pConfig, CFGI_PROFILE_SMS_RINGER_ID,(void*)nNewConfigRinger,sizeof(nNewConfigRinger));        
    }
    else if(pMe->m_RingerType == SET_ANOTHER)
    {
        AECHAR    filename[MAX_FILE_NAME] = {0};
        STRTOWSTR((char*)pBuf, filename, MAX_FILE_NAME*sizeof(AECHAR));
        
        WSTRCPY(pMe->m_enter_address, filename); 
    }

    if(pMe->m_active == pMe->m_CurProfile)
    {
        pMe->m_ScenemodesubType = SET_ACTIVATE;
        SoundMenu_SceneModeActivate(pMe);        
    }
    pMe->m_eDlgRet = DLGRET_MESSAGE;
    return SUCCESS;
   
}

