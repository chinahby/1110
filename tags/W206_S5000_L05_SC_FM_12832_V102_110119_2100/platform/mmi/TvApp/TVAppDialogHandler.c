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
#define TV_FIRST_REGION  IDS_USA         

#define MMI_SUCCESS(result) (result == SUCCESS? TRUE:FALSE)
#define MMI_FAILURE(result) (result == SUCCESS? FALSE:TRUE)
boolean MMI_Utf8ToWstr(char* pUtf8Str, AECHAR* pUcs2Str, int Ucs2BufSize);
boolean UTF8TOWSTR(const byte *pSrc,int nLen, AECHAR *pDst, int nSize);
boolean strlight=FALSE;
boolean VIEWBAR=FALSE;


/*
struct ICameraEx
{
    VTBL(ICamera)    *pvt;
    int32             m_nRefs;
    OEMCamera        *m_pOEMCamera;
};
*/
/*==============================================================================
                                 函数声明
==============================================================================*/
// 对话框IDD_CMAINMENU事件处理函数
static boolean  TVApp_MainMenuHandleEvent(CTVApp *pMe, 
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

#if 0
// 设置菜单项选中图标
static void TVApp_SetCFGMenuIcon(IMenuCtl *pm, 
                                     uint16 listId, 
                                   boolean bSel);
#endif



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

/*TV_AUTOSCAN菜单事件处理函数*/
static boolean TV_AUTOSCAN_HandleEvent(CTVApp *pMe,
                                              AEEEvent eCode,
                                              uint16 wParam,
                                              uint32 dwParam);
/*TV_BOOKMARK菜单事件处理函数*/
static boolean TV_BOOKMARK_HandleEvent(CTVApp *pMe,
                                              AEEEvent eCode,
                                              uint16 wParam,
                                              uint32 dwParam);
/*TVWAITING菜单事件处理函数*/

static boolean TV_TVWAITING_HandleEvent(CTVApp * pMe,AEEEvent eCode,uint16 wParam,uint32 dwParam);


//TV主菜单
static void TV_Build_MainOpts_Menu(CTVApp *pMe,IMenuCtl *pMenuCtl);



// 快门声处理回调函数
//static void TVApp_MediaNotify(void *pUser, 
//                                  AEEMediaCmdNotify *pCmdNotify);

//void TVApp_AlphaDisplay(CTVApp *pMe, IMenuCtl *pMenu);

// 设置弹出菜单矩形
static void TVApp_SetPopMenuRect(CTVApp *pMe, 
                                     IMenuCtl *popMenu, 
                                     int menuItemSum);

// 画TopBar的函数
static void TVApp_DrawTopBar(CTVApp *pMe);

//static void TVApp_DrawpopMenuBg(CTVApp *pMe, IMenuCtl *popMenu);

// 画Bottombar上的TV/video图标函数
//static void TVApp_DrawMidPic(CTVApp *pMe);


// 找T卡的函数
//static boolean TVApp_FindMemoryCardExist(CTVApp *pMe);

// 找Sensor的函数
//static boolean TVApp_FindSensorExist(CTVApp *pMe);

// Dialog的定时处理函数
//static void TVApp_DialogTimeout(void *pme);


// 画bottom bar上的文本函数
static void TVApp_DrawBottomBarText(CTVApp *pMe, 
                                        BottomBar_e_Type eBarType);

#if 0
// 设置拍照文件名
static boolean TVApp_SetDateForRecordFileName(CTVApp *pMe);

// 获取当前时间
static boolean TVApp_GetDateForRecordFileName(CTVApp *pMe,
                                                  char *pszDest, 
                                                  unsigned int fileType);
#endif


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
/*TV自动搜台*/
static int TVAPP_AutoScan(CTVApp *pMe);
static void CTvUtil_StopSearchAnimation(CTVApp * pMe);
static void TVAPP_SET_BAR(CTVApp *pMe,uint32 bar);

/*TV bar*/
static void TvApp_DrawBar(void * pme);

/*搜索进度条*/
static void TvApp_DrawAUTOSCANBar(void * pme,uint16 nowchannel,uint16 AllTotal );


/*画图处理*/
static int TV_UpdateInit(CTVApp *pMe);
static int TV_Update(CTVApp *pMe);
static void TV_StartPreview(CTVApp *pMe);
//static void TV_StopSearchAnimation(CTVApp *pITvUtil);

// Dialog的定时处理函数
static void TvApp_DialogTimeout(void *pme);

void CTvUtil_BookmarkOperator_InsertAt(CTVApp *pMe,char* in_channel, char *in_title, uint16 *out_index);

void TVApp_InitTVCheck(void *po);

//static void TVApp_DrawImage(CTVApp *pMe, uint16 ResID, int x, int y);


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
            (void)IDISPLAY_SetPrefs(pMe->m_pDisplay, "a:0", STRLEN("a:0"));
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
    MSG_FATAL("TVApp_RouteDialogEvent",0,0,0);
    if((NULL == pMe) || (NULL == pMe->m_pActiveDlg))
    {
        MSG_FATAL("pMe->m_pActiveDlgID---------------------------------is NULL",0,0,0);
        return FALSE;
    }
    MSG_FATAL("pMe->m_pActiveDlgID---------------------------------%d",pMe->m_pActiveDlgID,0,0);
    switch(pMe->m_pActiveDlgID)
    {
        case IDD_CMAINMENU:
            return TVApp_MainMenuHandleEvent(pMe, eCode, wParam, dwParam);
            
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
		case IDD_AUTOSCAN:
			MSG_FATAL("IDD_AUTOSCAN-----------",0,0,0);
		    return  TV_AUTOSCAN_HandleEvent(pMe, eCode,wParam, dwParam);

		case IDD_BOOKMARK:
			MSG_FATAL("TV_BOOKMARK_HandleEvent-----------",0,0,0);
		    return  TV_BOOKMARK_HandleEvent(pMe, eCode,wParam, dwParam);
        case IDD_TVWAITING:
            return TV_TVWAITING_HandleEvent(pMe, eCode,wParam, dwParam);
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
static boolean TVApp_MainMenuHandleEvent(CTVApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    PARAM_NOT_REF(dwParam)
        
	//AECHAR WTitle[40] = {0};
    MSG_FATAL("TVApp_MainMenuHandleEvent Start",0,0,0);
    MSG_FATAL("TVApp_MainMenuHandleEvent eCode=%d, wParam=%d, dwParam=%d",eCode,wParam,dwParam);
    switch(eCode) 
    {
        case EVT_DIALOG_INIT:
           MSG_FATAL("TVApp_MainMenuHandleEvent EVT_DIALOG_INIT",0,0,0);
		   IDISPLAY_SetClipRect(pMe->m_pDisplay, NULL); 
           pMe->m_bIsPreview = FALSE;
           pMe->m_nTVState = TV_START;
           strlight=FALSE;
           VIEWBAR=FALSE;
          // pMe->m_wMsgID = IDS_MSG_WAITING;
          // pMe->m_nMsgTimeout = TIMEOUT_MS_MSGBOX;
          
		 {
			int result = SUCCESS;
							   
			result = ISHELL_CreateInstance(pMe->m_pShell, MMI_CLSID_IMMITV, (void**)&pMe->pIMMITv);
			if(pMe->pIMMITv == NULL)
			 {
			  return FALSE;
			 }
			(void)ICONFIG_GetItem(pMe->m_pConfig,
                                  CFGI_TV_SETCHANNL,
                                  pMe->pTvSetting,
                                  sizeof(CFG_TvSetting));
			MSG_FATAL("pMe->pTvSetting->region=%d-------------------------",pMe->pTvSetting->region,0,0);

			result = IMMITv_CreateTv(pMe->pIMMITv, &pMe->myICBMMITv,pMe->pTvSetting->region);
			if(MMI_SUCCESS(result))
			 {
				 MSG_FATAL("IMMITv_CreateTv---------------------success",0,0,0);
			 }
			result = IMMITv_StartPreview(pMe->pIMMITv);
			if(MMI_SUCCESS(result))
			{
				MSG_FATAL("IMMITv_StartPreview----------------------success",0,0,0);
			}
			MSG_FATAL("IMMITv_StartPreview----------------------will--end",0,0,0);
            
            IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_SOUND, pMe->pTvSetting->SoundStep);
            if(pMe->m_bAUTOSCAN)
			{
                 MSG_FATAL("TVApp_MainMenuHandleEvent m_bAUTOSCAN",0,0,0);
                TVAPP_AutoScan(pMe);
            }
            else
            {
                pMe->currentlyChannel=pMe->pTvSetting->CurrentChannel;//(int)WSTRTOFLOAT(pMe->pTvSetting->Bookmark[pMe->currentlyChannel].channel);
                MSG_FATAL("pMe->currentlyChannel=%d----------",pMe->currentlyChannel,0,0);
                //TLGAPP_SetChannel(pMe->currentlyChannel);
                IMMITv_SetTvChannel(pMe->pIMMITv, pMe->currentlyChannel,FALSE); 

            }
		 }
         
            IDIALOG_SetProperties((IDialog *)dwParam, DLG_NOT_REDRAW_AFTER_START);	
            return TRUE;
        	
        case EVT_DIALOG_START:
            MSG_FATAL("TVApp_MainMenuHandleEvent EVT_DIALOG_START",0,0,0);
            (void)ISHELL_PostEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL);            
            return TRUE;

        case EVT_DIALOG_END:
            IMMITv_StopPreview(pMe->pIMMITv);
            VIEWBAR=FALSE;
            strlight=FALSE;
            pMe->m_bAUTOSTOP=FALSE;
            pMe->m_bAUTOSCAN=FALSE;
            snd_set_device(NULL, SND_MUTE_UNMUTED, SND_MUTE_UNMUTED, NULL, NULL);
            return TRUE;
        case EVT_HEADSET:
             if(HS_HEADSET_ON())
               {
                 snd_set_device(SND_DEVICE_HEADSET_FM, SND_MUTE_UNMUTED, SND_MUTE_UNMUTED, NULL, NULL);
               }
               else
               {
                 snd_set_device(SND_DEVICE_SPEAKER_FM, SND_MUTE_UNMUTED, SND_MUTE_UNMUTED, NULL, NULL);
               }
             return TRUE;
        case EVT_USER_REDRAW: 

			{
            
			 AEERect	 rc = pMe->m_rc;
			 AECHAR channelStr[32];
			// WSTRCPY(strchannel,(AECHAR *)(pMe->currentlyChannel)); (AECHAR *)pMe->currentlyChannel
			 MSG_FATAL("EVT_USER_REDRAW--pMe->currentlyChannel=%d----------",pMe->currentlyChannel,0,0);
            TV_UpdateInit(pMe);
             if(pMe->m_bAUTOSCAN)
             {
                   int AllTotal;
                   pMe->m_bAUTOSCAN=FALSE;
                   AllTotal=IMMITv_getChannelTotal(pMe->pIMMITv);
                   //ITV_GetChnCount(pMe->pITv, &AllTotal);
                   MSG_FATAL("TV_mainMenu_HandleEvent--------------m_bAUTOSCAN=%d",AllTotal,0,0);
                   TvApp_DrawAUTOSCANBar(pMe,pMe->m_curChnIdx,AllTotal);
                   IMMITv_AutoScanTV(pMe->pIMMITv); 
              }
             
              if(pMe->m_bAUTOSTOP)
              {
                   CTvUtil_StopSearchAnimation(pMe);  
                   pMe->m_bAUTOSTOP=FALSE;
              }
              
              if((pMe->m_bAUTOSCAN==FALSE) && (pMe->m_bAUTOSTOP==FALSE))
              {
              WSPRINTF(channelStr, sizeof(channelStr), (AECHAR*)L"%d", pMe->currentlyChannel);
              DrawTextWithProfile(pMe->m_pShell, 
                                pMe->m_pDisplay, 
                                RGB_WHITE, 
                                AEE_FONT_NORMAL, 
                                channelStr, 
                                -1, 
                                5, 
                                5, 
                                &rc, 
                               IDF_TEXT_TRANSPARENT);
             }
             
             TVApp_DrawBottomBarText(pMe, BTBAR_OPTION_BACK); 
             if(VIEWBAR)
             {
                 TvApp_DrawBar(pMe);
             }
           
               
			 TV_Update(pMe);
			} 
            if(HS_HEADSET_ON())
            {
                snd_set_device(SND_DEVICE_HEADSET_FM, SND_MUTE_UNMUTED, SND_MUTE_UNMUTED, NULL, NULL);
            }
            else
            {
                snd_set_device(SND_DEVICE_SPEAKER_FM, SND_MUTE_UNMUTED, SND_MUTE_UNMUTED, NULL, NULL);
            }
            
            
            return TRUE;

        case EVT_ATV_AUTOSCANFINISH:
             MSG_FATAL("TV_mainMenu_HandleEvent--------------EVT_ATV_AUTOSCANFINISH",0,0,0);
             pMe->m_bAUTOSTOP=TRUE;
             pMe->m_bAUTOSCAN=FALSE;
			 ISHELL_SendEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL);
			 return TRUE;

        case  EVT_ATV_AUTOSCAN:
              MSG_FATAL("TV_mainMenu_HandleEvent--------------EVT_ATV_AUTOSCAN",0,0,0);
              pMe->m_bAUTOSTOP=FALSE;
              pMe->m_bAUTOSCAN=TRUE;
              pMe->m_curChnIdx++;
              ISHELL_SendEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL);
    		  return TRUE;
               
        case EVT_COMMAND:
            return TRUE; 
		case EVT_NO_CLOSEBACKLIGHT:
			return TRUE;
			
        case EVT_KEY:
            {
            switch(wParam)
                {
                    case AVK_SELECT:
                        IMMITv_StopPreview(pMe->pIMMITv);
                        VIEWBAR=FALSE;
                        CLOSE_DIALOG(DLGRET_POPMSG);//STATE_CPOPMSG
                        break;
                    case AVK_CLR:
                        if(VIEWBAR)
                        {
                          VIEWBAR=FALSE;
                        }else
                        {
                            IMMITv_StopPreview(pMe->pIMMITv);
                            CLOSE_DIALOG(DLGRET_CANCELED);
                        }
                        
                        break;
                    case AVK_LEFT:
                        if(VIEWBAR)
                        {
                             if(pMe->m_nTVCFG == TVSETChannel)
                	            {
                	                pMe->m_nTVCFG = TVSETSound;
                	            }
                	            else
                	            {
                	                pMe->m_nTVCFG--;
                	            }
                                TVAPP_SET_BAR(pMe,pMe->m_nTVCFG);
                        }
                        else
                        {
                           VIEWBAR=TRUE;
                           //CLOSE_DIALOG(DLGRET_DRAWTOPBAR);
                        }
                        break;
                    case AVK_RIGHT:
                        if(VIEWBAR)
                           {
                                if(pMe->m_nTVCFG == TVSETSound)
                     	         {
                     				    pMe->m_nTVCFG = TVSETChannel;
                     	          }
                     	         else
                     	          {
                     	                pMe->m_nTVCFG++;
                     	          }
                                  TVAPP_SET_BAR(pMe,pMe->m_nTVCFG);
                           }

                        break;
                    case AVK_UP:
                        if(VIEWBAR==FALSE)
                           {
                             AECHAR* channel = NULL;
   
                             (void)ICONFIG_GetItem(pMe->m_pConfig,
                                                     CFGI_TV_SETCHANNL,
                                                     pMe->pTvSetting,
                                                     sizeof(CFG_TvSetting));
                             
       						 if(pMe->currentlyChannelIndex < pMe->pTvSetting->ChannelCountAble-1)
       						   {
       							   pMe->currentlyChannelIndex++;
       						   }
       						   else
       						   {
       							   pMe->currentlyChannelIndex = 0;
       						   }
                                  MSG_FATAL("currentlyChannelIndex=%d----",pMe->currentlyChannelIndex,0,0);
       						   channel = pMe->pTvSetting->Bookmark[pMe->currentlyChannelIndex].channel; 
       						   if (channel != NULL)
       						   {
       							   int index = (int)WSTRTOFLOAT(channel);
       							   IMMITv_SetTvChannel(pMe->pIMMITv, index,FALSE); 
       							   pMe->pTvSetting->CurrentChannel = index;
       							   pMe->currentlyChannel = index;
       		
       						   }
                               MSG_FATAL("currentlyChannel=%d----",pMe->currentlyChannel,0,0);
                              (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                           }
                          else
                          {
                            switch(pMe->m_nTVCFG)
                 					{
                 					case TVSETChannel:
                 						{
                 						   AECHAR* channel = NULL;
                                           // strlight=FALSE;
                 						   MSG_FATAL("ToolBar_OnKeyPress channelcount=%d",pMe->pTvSetting->ChannelCountAble ,0,0);
                 		
                 						   
                 						   if(pMe->currentlyChannelIndex < pMe->pTvSetting->ChannelCountAble-1)
                 						   {
                 							   pMe->currentlyChannelIndex++;
                 						   }
                 						   else
                 						   {
                 							   pMe->currentlyChannelIndex = 0;
                 						   }
                 						   channel = pMe->pTvSetting->Bookmark[pMe->currentlyChannelIndex].channel; 
                 						   if (channel != NULL)
                 						   {
                 							   int index = (int)WSTRTOFLOAT(channel);
                 							   IMMITv_SetTvChannel(pMe->pIMMITv, index,FALSE); 
                 							   pMe->pTvSetting->CurrentChannel = index;
                 							   pMe->currentlyChannel = index;
                 		
                 						   }	
                                            (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                 						 }
                 					        break;
                 					case TVSETBrightness:
                                         MSG_FATAL("TVSETBrightness-------------------",0,0,0);
                                        
                                         if(pMe->pTvSetting->BrightnessStep < TV_SETTING_BRIGHTNESS_MAXVALUE)
                                           {
                                           
                                            pMe->m_barW++;
                                            pMe->pTvSetting->BrightnessStep++;
                                            
                                           }
                                          IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_BRIGHT, pMe->pTvSetting->BrightnessStep);
                                         (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                                         break;
                                     case TVSETContrast:
                                         MSG_FATAL("TVSETBrightness-------------------",0,0,0);
                                        
                                         if(pMe->pTvSetting->ContrastStep< TV_SETTING_CONTRAST_MAXVALUE)
                                           {
                                           
                                            pMe->m_barW++;
                                            pMe->pTvSetting->ContrastStep ++;
                                            
                                           }
                                         IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_CONTRAST, pMe->pTvSetting->ContrastStep);
                                         (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                                         break;
                                      case TVSETDefinition:
                                         MSG_FATAL("TVSETBrightness-------------------",0,0,0);
                                        
                                         if(pMe->pTvSetting->DefinitionStep< TV_SETTING_DEFINITION_MAXVALUE)
                                           {
                                          
                                            pMe->m_barW++;
                                            pMe->pTvSetting->DefinitionStep ++;
                                            
                                           }
                                         IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_DEFINITION, pMe->pTvSetting->DefinitionStep);
                                         (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                                         break;
                                       case TVSETSaturation:
                                         MSG_FATAL("TVSETBrightness-------------------",0,0,0);
                                        
                                         if(pMe->pTvSetting->SaturationStep< TV_SETTING_SATURATION_MAXVALUE)
                                           {
                                          
                                            pMe->m_barW=pMe->m_barW+(TV_SETTING_SATURATION_MAXVALUE/50);
                                            pMe->pTvSetting->SaturationStep = pMe->pTvSetting->SaturationStep+(TV_SETTING_SATURATION_MAXVALUE/50);
                                            MSG_FATAL("pMe->pTvSetting->SaturationStep-------%d",pMe->m_barW,0,0);
                                            
                                           }
                                         IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_SATURATION, pMe->pTvSetting->SaturationStep);
                                         (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                                         break;
                                     case TVSETSound:
                                         MSG_FATAL("TVSETBrightness-------------------",0,0,0);
                                        
                                         if(pMe->pTvSetting->SoundStep< TV_SETTING_SOUND_MAXVALUE)
                                           {
                                          
                                            pMe->m_barW++;
                                            pMe->pTvSetting->SoundStep ++;
                                            
                                           }
                                         IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_SOUND, pMe->pTvSetting->SoundStep);
                                         (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                                         break;
     
                 					    default:
                 							return TRUE; 
                                    }
                          }
                        break;
                    case AVK_DOWN:
                        if(VIEWBAR==FALSE)
						 {
                            AECHAR* channel = NULL;
  
                            (void)ICONFIG_GetItem(pMe->m_pConfig,
                                                    CFGI_TV_SETCHANNL,
                                                    pMe->pTvSetting,
                                                    sizeof(CFG_TvSetting));
                                 
       						 if(pMe->currentlyChannelIndex > 0)
       						   {
       							   pMe->currentlyChannelIndex--;
       						   }
       						   else
       						   {
       							   pMe->currentlyChannelIndex = pMe->pTvSetting->ChannelCountAble-1;
       						   }
                                  MSG_FATAL("currentlyChannelIndex=%d----",pMe->currentlyChannelIndex,0,0);
       						   channel = pMe->pTvSetting->Bookmark[pMe->currentlyChannelIndex].channel; 
       						   if (channel != NULL)
       						   {
       							   int index = (int)WSTRTOFLOAT(channel);
       							   IMMITv_SetTvChannel(pMe->pIMMITv, index,FALSE); 
       							   pMe->pTvSetting->CurrentChannel = index;
       							   pMe->currentlyChannel = index;
       		
       						   }
                              MSG_FATAL("currentlyChannel=%d----",pMe->currentlyChannel,0,0);
                             (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                        }
                        else
                        {
                           switch(pMe->m_nTVCFG)
            					{
            					case TVSETChannel:
                                      {
                    		            AECHAR* channel = NULL;
                    					MSG_FATAL("ToolBar_OnKeyPress channelcount=%d",pMe->pTvSetting->ChannelCountAble ,0,0);

                    		            
                    		            if(pMe->currentlyChannelIndex > 0)
                    		            {
                    		                pMe->currentlyChannelIndex--;
                    		            }
                    		            else
                    		            {
                    		                pMe->currentlyChannelIndex = pMe->pTvSetting->ChannelCountAble;
                    		            }
                    		            channel = pMe->pTvSetting->Bookmark[pMe->currentlyChannelIndex].channel; 
                    		            if (channel != NULL)
                    		            {
                    		                int index = (int)WSTRTOFLOAT(channel);
                    		                IMMITv_SetTvChannel(pMe->pIMMITv, index,FALSE); 
                    		                pMe->pTvSetting->CurrentChannel = index;
                    		                pMe->currentlyChannel = index;
                    				    }
                                      }
                                    break;
                                case TVSETBrightness:
                                    if(pMe->pTvSetting->BrightnessStep > 0)
                                      {
                                      // pMe->m_barMAXW=TV_SETTING_BRIGHTNESS_MAXVALUE;
                                       pMe->m_barW--;
                                       pMe->pTvSetting->BrightnessStep--;
                                       
                                      }
                                    IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_BRIGHT, pMe->pTvSetting->BrightnessStep);
                                    (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                                    break;
                                case TVSETContrast:
                                    MSG_FATAL("TVSETContrastdown-----------%d",pMe->m_barW,0,0);
                                    if(pMe->pTvSetting->ContrastStep > 0)
                                      {
                                     // pMe->m_barMAXW=TV_SETTING_CONTRAST_MAXVALUE;
                                       pMe->m_barW--;
                                       pMe->pTvSetting->ContrastStep--;
                                       
                                      }
                                    IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_CONTRAST, pMe->pTvSetting->ContrastStep);
                                    (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                                    break;
                                 case TVSETDefinition:
                                    MSG_FATAL("TVSETDefinitiondown-----------%d",pMe->m_barW,0,0);
                                    if(pMe->pTvSetting->DefinitionStep > 0)
                                      {
                                     // pMe->m_barMAXW=TV_SETTING_DEFINITION_MAXVALUE;
                                       pMe->m_barW--;
                                       pMe->pTvSetting->DefinitionStep--;
                                       
                                      }
                                     IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_DEFINITION, pMe->pTvSetting->DefinitionStep);
                                    (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                                    break;
                                  case TVSETSaturation:
                                    MSG_FATAL("TVSETSaturationdown-----------%d",pMe->m_barW,0,0);
                                    if(pMe->pTvSetting->SaturationStep > 0)
                                      {
                                     // pMe->m_barMAXW=TV_SETTING_SATURATION_MAXVALUE;
                                       pMe->m_barW=pMe->m_barW-(TV_SETTING_SATURATION_MAXVALUE/50);
                                       pMe->pTvSetting->SaturationStep= pMe->pTvSetting->SaturationStep - (TV_SETTING_SATURATION_MAXVALUE/50);
                                       
                                      }
                                    IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_SATURATION, pMe->pTvSetting->SaturationStep);
                                    (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                                    break;
                                case TVSETSound:
                                    if(pMe->pTvSetting->SoundStep > 0)
                                      {
                                     // pMe->m_barMAXW=TV_SETTING_SOUND_MAXVALUE;
                                       pMe->m_barW--;
                                       pMe->pTvSetting->SoundStep--;
                                       
                                      }
                                    IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_SOUND, pMe->pTvSetting->SoundStep);
                                    (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                                    break;

            					default:
            							return TRUE; 
                                }
                        }
                        
                        break;
                    default:         
                        break; 
                    
                }

                ISHELL_SendEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL);
                return TRUE;
            }
       default:
            break;  
    }
    MSG_FATAL("TVApp_MainMenuHandleEvent End",0,0,0);
    return FALSE;
}

static void TvApp_DrawBar(void * pme)
{
    
        CTVApp *pMe = (CTVApp *)pme;
        IImage *pTopBarImage2 = NULL;
        AEEImageInfo myInfo;
        AEERect  rc = pMe->m_rc;
        AECHAR    channelStr[32];
        AECHAR    m_barWStr[15];
        AECHAR    strTitle[15]; 
        AEERect clip;
        AEERect oldClip;
         if(strlight)
         {
             IImage *IMGbarblank=NULL;
             IImage *image12=NULL;
             AEEImageInfo IMGbarblankInfo;
             IMGbarblank=ISHELL_LoadResImage(pMe->m_pShell,TVAPP_IMAGE_RES_FILE,IDI_SCHEDULEBAR_BLANK);
             IIMAGE_Draw(IMGbarblank, TV_SETTING_BAR_X, TV_SETTING_BAR_Y);
             IIMAGE_GetInfo(IMGbarblank, &IMGbarblankInfo);
             image12 = ISHELL_LoadResImage( pMe->m_pShell,TVAPP_IMAGE_RES_FILE,IDI_SCHEDULEBAR_FULL);
            if( image12 != NULL)
            {
                SETAEERECT( &clip, TV_SETTING_BAR_X,TV_SETTING_BAR_Y, ((IMGbarblankInfo.cx* pMe->m_barW)/pMe->m_barMAXW),TV_SETTING_BAR_H);
                MSG_FATAL("m_barMAXW---=%d----m_barW----=%d-------------img.cx=%d-",pMe->m_barMAXW,pMe->m_barW,IMGbarblankInfo.cx);
                IDISPLAY_GetClipRect( pMe->m_pDisplay, &oldClip);
                IDISPLAY_SetClipRect( pMe->m_pDisplay, &clip);

                IIMAGE_Draw(image12, TV_SETTING_BAR_X,TV_SETTING_BAR_Y);
                IDISPLAY_SetClipRect( pMe->m_pDisplay, &oldClip);
                MSG_FATAL("image12 address = %x", image12, 0, 0);
                MSG_FATAL("IMGbarblank address = %x", IMGbarblank, 0, 0);
                IIMAGE_Release(image12);
                IIMAGE_Release(IMGbarblank);
                image12 = NULL;
                IMGbarblank = NULL;

                WSPRINTF(m_barWStr, sizeof(m_barWStr), (AECHAR*)L"%d", pMe->m_barW);
                DrawTextWithProfile(pMe->m_pShell, 
                                    pMe->m_pDisplay, 
                                    RGB_WHITE, 
                                    AEE_FONT_NORMAL, 
                                    m_barWStr, 
                                    -1, 
                                    TV_SETTING_BAR_X+IMGbarblankInfo.cx, 
                                    TV_SETTING_BAR_Y-5, 
                                    &rc, 
                                    IDF_TEXT_TRANSPARENT);
                (void) ISHELL_LoadResString(pMe->m_pShell,
                                    AEE_APPSTVAPP_RES_FILE,
                                    pMe->m_barTitle,
                                    strTitle,
                                    sizeof(strTitle));
                DrawTextWithProfile(pMe->m_pShell, pMe->m_pDisplay, RGB_WHITE, AEE_FONT_NORMAL,strTitle,-1, TV_TITLE_BAR_X, TV_TITLE_BAR_Y,&rc, IDF_TEXT_TRANSPARENT);
               
            } 
             
         }
           
        pTopBarImage2 = ISHELL_LoadResImage(pMe->m_pShell, 
                                       TVAPP_IMAGE_RES_FILE, 
                                       IDI_PNG_TV_TOOLBAR); 
        if(pTopBarImage2)
        {
        
        
        IIMAGE_GetInfo(pTopBarImage2, &myInfo);
        MSG_FATAL("m_cxWidth--%d------m_cyHeight--%d-----myInfo----%d",pMe->m_cxWidth,pMe->m_cyHeight,myInfo.cy);
        IIMAGE_Draw(pTopBarImage2, (pMe->m_cxWidth - myInfo.cx)/2, pMe->m_cyHeight-(myInfo.cy*2)-5);    
        TVApp_DrawTopBar(pMe);
        TVApp_DrawBottomBarText(pMe, BTBAR_SELECT_BACK);
        MSG_FATAL("pTopBarImage2 address = %x", pTopBarImage2, 0, 0);
        IIMAGE_Release(pTopBarImage2);
        pTopBarImage2 = NULL;
        
        }
        
        WSPRINTF(channelStr, sizeof(channelStr), (AECHAR*)L"%d", pMe->currentlyChannel);
         DrawTextWithProfile(pMe->m_pShell, 
                    pMe->m_pDisplay, 
                    RGB_WHITE, 
                    AEE_FONT_NORMAL, 
                    channelStr, 
                    -1, 
                    5, 
                    5, 
                    &rc, 
                   IDF_TEXT_TRANSPARENT);
             
}
static void TvApp_DrawAUTOSCANBar(void * pme,uint16 nowchannel,uint16 AllTotal )
{
    
        CTVApp *pMe = (CTVApp *)pme;
        AEERect  rc = pMe->m_rc; 
        AEERect clip;
        AEERect oldClip;
        IImage *IMGbarblank=NULL;
        IImage *image12=NULL;
        AEEImageInfo IMGbarblankInfo;
        AECHAR channeltotal[32];
        IMGbarblank=ISHELL_LoadResImage(pMe->m_pShell,TVAPP_IMAGE_RES_FILE,IDI_SCHEDULEBAR_BLANK);
        IIMAGE_Draw(IMGbarblank, TV_AUTOSCAN_BAR_X, TV_AUTOSCAN_BAR_Y);
        IIMAGE_GetInfo(IMGbarblank, &IMGbarblankInfo);
        image12 = ISHELL_LoadResImage( pMe->m_pShell,TVAPP_IMAGE_RES_FILE,IDI_SCHEDULEBAR_FULL);
        MSG_FATAL("AllTotal---=%d----nowchannel----=%d-------------img.cx=%d-",AllTotal,nowchannel,IMGbarblankInfo.cx);
        
        if( image12 != NULL)
        {
            SETAEERECT( &clip, TV_AUTOSCAN_BAR_X,TV_AUTOSCAN_BAR_Y, ((IMGbarblankInfo.cx* nowchannel)/AllTotal),TV_AUTOSCAN_BAR_H);
            
            IDISPLAY_GetClipRect( pMe->m_pDisplay, &oldClip);
            IDISPLAY_SetClipRect( pMe->m_pDisplay, &clip);

            IIMAGE_Draw(image12, TV_AUTOSCAN_BAR_X,TV_AUTOSCAN_BAR_Y);
            IDISPLAY_SetClipRect( pMe->m_pDisplay, &oldClip);
            MSG_FATAL("image12 address = %x", image12, 0, 0);
            IIMAGE_Release(image12);
            IIMAGE_Release(IMGbarblank);
            image12 = NULL;
            IMGbarblank = NULL;

            WSPRINTF(channeltotal, sizeof(channeltotal), (AECHAR*)L"%d", pMe->m_curChnIdx);
     		DrawTextWithProfile(pMe->m_pShell, 
                                     pMe->m_pDisplay, 
                                     RGB_WHITE, 
                                     AEE_FONT_NORMAL, 
                                     channeltotal, 
                                     -1, 
                                     TV_SETTING_BAR_X+IMGbarblankInfo.cx, 
                                     TV_AUTOSCAN_BAR_Y-10, 
                                     &rc, 
                                    IDF_TEXT_TRANSPARENT);
         } 
         
}


#if 0
static void TV_StopSearchAnimation(CTVApp *pMe)
{  
    int result = SUCCESS;
    int i = 0;
    char title[3];
    char channel[4];
    //CTVApp* pThis = (CTVApp*)pITvUtil;
    uint16 *ableChannelArray = IMMITv_getAbleChannelArray(pMe->pIMMITv);
    int channelCount = IMMITv_getChannelCountAble(pMe->pIMMITv);
    

  //  MMI_DEBUG(ATV,("CTvUtil_StopSearchAnimation Start"));

    if(channelCount > 0)
    {
            {
              int num = pMe->pTvSetting->ChannelCountAble;
              int i = 0;
              int nSize = 0;   
              MSG_FATAL("CTvUtil_BookmarkOperator_RemoveAll Start",0,0,0); 
              (void)ICONFIG_GetItem(pMe->m_pConfig,
                                     CFGI_TV_SETCHANNL,
                                     (void*)pMe->pTvSetting,
                                     sizeof(CFG_TvSetting));
              for(; i < num; i++)
              {
                  nSize = WSTRLEN(pMe->pTvSetting->Bookmark[i].name); 
                  MEMSET (pMe->pTvSetting->Bookmark[i].name, '\0', sizeof(AECHAR) * nSize); 
            
                  nSize = WSTRLEN(pMe->pTvSetting->Bookmark[i].channel); 
                  MEMSET (pMe->pTvSetting->Bookmark[i].channel, '\0', sizeof(AECHAR) * nSize); 
              }
              pMe->pTvSetting->Bookmarktotal = 0;
              pMe->pTvSetting->ChannelCountAble = 0;
              (void)ICONFIG_SetItem(pMe->m_pConfig,
                                    CFGI_TV_SETCHANNL,
                                    (void*)pMe->pTvSetting,
                                    sizeof(CFG_TvSetting));
             }
    
            pMe->pTvSetting->ChannelCountAble = channelCount;
            pMe->pTvSetting->CurrentChannel = ableChannelArray[0];//自动搜索完成后，会跳到索引为0的频道来
            pMe->currentlyChannel = ableChannelArray[0];
            pMe->currentlyChannelIndex = 0;
            result = IMMITv_SetTvChannel(pMe->pIMMITv, ableChannelArray[0],FALSE); 
            if (pMe->pIMMITv != NULL)
            {
                IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_SOUND, pMe->pTvSetting->SoundStep);
                //IMMIAudioDevice_GetOutputVolumn(pThis->pIMMIAudioDevice,AUDIOVOLUME_LEVEL0);
            }

            for(; i < channelCount; i++)
            {
                SPRINTF(title, "%d", i+1);
                SPRINTF(channel, "%d",  ableChannelArray[i]);
               // CTvUtil_BookmarkOperator_InsertAt(channel, title, &pThis->BM_ListFocusIndex);             
            }
            IMMITv_ResetScanTag(pMe->pIMMITv);
            // CTvUtil_UI_ShowCurrentChannel(pThis, TRUE);                     
   }

}
#endif
static void TV_StartPreview(CTVApp * pMe)
{
        int result=SUCCESS;
	    result = IMMITv_CreateTv(pMe->pIMMITv, &pMe->myICBMMITv,pMe->pTvSetting->region);
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
    AEERect clip;
    AEERect oldClip;
    (void)ICONFIG_GetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
    switch(eCode) 
    {
        case EVT_DIALOG_INIT:
		        strlight=FALSE;
            return TRUE;
        	
        case EVT_DIALOG_START:

            (void)ISHELL_PostEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL);            
            return TRUE;

        case EVT_DIALOG_END:
			IMMITv_StopPreview(pMe->pIMMITv);
            return TRUE;

        case EVT_USER_REDRAW:
			TV_UpdateInit(pMe);

            //IMENUCTL_Redraw(pMenu);
			
				{
				IImage *pTopBarImage2 = NULL;
				AEEImageInfo myInfo;
				AEERect	 rc = pMe->m_rc;
				AECHAR    channelStr[32];
				AECHAR    m_barWStr[15];
                AECHAR    strTitle[15];   
                 if(strlight)
                 {
                     IImage *IMGbarblank=NULL;
                     IImage *image12=NULL;
                     AEEImageInfo IMGbarblankInfo;
                     IMGbarblank=ISHELL_LoadResImage(pMe->m_pShell,TVAPP_IMAGE_RES_FILE,IDI_SCHEDULEBAR_BLANK);
                     IIMAGE_Draw(IMGbarblank, TV_SETTING_BAR_X, TV_SETTING_BAR_Y);
                     IIMAGE_GetInfo(IMGbarblank, &IMGbarblankInfo);
                     image12 = ISHELL_LoadResImage( pMe->m_pShell,TVAPP_IMAGE_RES_FILE,IDI_SCHEDULEBAR_FULL);
                    if( image12 != NULL)
                    {
            			SETAEERECT( &clip, TV_SETTING_BAR_X,TV_SETTING_BAR_Y, ((IMGbarblankInfo.cx* pMe->m_barW)/pMe->m_barMAXW),TV_SETTING_BAR_H);
                        MSG_FATAL("m_barMAXW---=%d----m_barW----=%d-------------img.cx=%d-",pMe->m_barMAXW,pMe->m_barW,IMGbarblankInfo.cx);
                        IDISPLAY_GetClipRect( pMe->m_pDisplay, &oldClip);
                        IDISPLAY_SetClipRect( pMe->m_pDisplay, &clip);

                        IIMAGE_Draw(image12, TV_SETTING_BAR_X,TV_SETTING_BAR_Y);
                        IDISPLAY_SetClipRect( pMe->m_pDisplay, &oldClip);
                        IIMAGE_Release(image12);
                        IIMAGE_Release(IMGbarblank);
                        image12 = NULL;
                        IMGbarblank = NULL;

    					WSPRINTF(m_barWStr, sizeof(m_barWStr), (AECHAR*)L"%d", pMe->m_barW);
    					DrawTextWithProfile(pMe->m_pShell, 
		    	                            pMe->m_pDisplay, 
		    	                            RGB_WHITE, 
		    	                            AEE_FONT_NORMAL, 
		    	                            m_barWStr, 
		    	                            -1, 
		    	                            TV_SETTING_BAR_X+IMGbarblankInfo.cx, 
		    	                            TV_SETTING_BAR_Y-5, 
		    	                            &rc, 
		    	                            IDF_TEXT_TRANSPARENT);
					    (void) ISHELL_LoadResString(pMe->m_pShell,
			                                AEE_APPSTVAPP_RES_FILE,
			                                pMe->m_barTitle,
			                                strTitle,
			                                sizeof(strTitle));
						DrawTextWithProfile(pMe->m_pShell, pMe->m_pDisplay, RGB_WHITE, AEE_FONT_NORMAL,strTitle,-1, TV_TITLE_BAR_X, TV_TITLE_BAR_Y,&rc, IDF_TEXT_TRANSPARENT);
					   
                    } 
                    #if 0
                     MSG_FATAL("TVSETBrightness1-------------------1",0,0,0);
                     (void)IGRAPHICS_SetFillMode(pMe->pGraphics, TRUE);
                     (void)IGRAPHICS_SetColor(pMe->pGraphics,0,0,0,0);
                     (void)IGRAPHICS_SetFillColor(pMe->pGraphics,0,0,0,0);
                     //(void)IGRAPHICS_SetPointSize(pMe->pGraphics,10);
                     line.x = 20;
                     line.y = 50;
                     line.dx = 130;
                     line.dy = 10;
                     //(void)IGRAPHICS_DrawLine(pMe->pGraphics, &line); 
                     (void)IGRAPHICS_DrawRect(pMe->pGraphics, &line);
                     #endif
                     
                 }
                   
			    pTopBarImage2 = ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_TOOLBAR); 
				if(pTopBarImage2)
	            {
	            
			    
	            IIMAGE_GetInfo(pTopBarImage2, &myInfo);
				MSG_FATAL("m_cxWidth--%d------m_cyHeight--%d-----myInfo----%d",pMe->m_cxWidth,pMe->m_cyHeight,myInfo.cy);
	            IIMAGE_Draw(pTopBarImage2, (pMe->m_cxWidth - myInfo.cx)/2, pMe->m_cyHeight-(myInfo.cy*2)-5);	
				TVApp_DrawTopBar(pMe);
	            TVApp_DrawBottomBarText(pMe, BTBAR_SELECT_BACK);
	            IIMAGE_Release(pTopBarImage2);
	            pTopBarImage2 = NULL;
				
	            }
				
			    WSPRINTF(channelStr, sizeof(channelStr), (AECHAR*)L"%d", pMe->currentlyChannel);
				 DrawTextWithProfile(pMe->m_pShell, 
                            pMe->m_pDisplay, 
                            RGB_WHITE, 
                            AEE_FONT_NORMAL, 
                            channelStr, 
                            -1, 
                            5, 
                            5, 
                            &rc, 
                           IDF_TEXT_TRANSPARENT);
             
             }
			
			
			TV_Update(pMe);
			//IDISPLAY_Update(pMe->m_pDisplay); //刷屏
            return TRUE;
            
        case EVT_COMMAND:
            return TRUE;  
			
        case EVT_KEY:
	      switch(wParam)
            {
            
            case AVK_CLR: 
               // IMMITv_StopPreview(pMe->pIMMITv);
                CLOSE_DIALOG(DLGRET_CANCELED);
             return TRUE;

			case AVK_LEFT:   
	            if(pMe->m_nTVCFG == TVSETChannel)
	            {
	                pMe->m_nTVCFG = TVSETSound;
	            }
	            else
	            {
	                pMe->m_nTVCFG--;
	            }
                TVAPP_SET_BAR(pMe,pMe->m_nTVCFG);
	            break;

	        case AVK_RIGHT:
	            if(pMe->m_nTVCFG == TVSETSound)
	            {
				    pMe->m_nTVCFG = TVSETChannel;
	            }
	            else
	            {
	                pMe->m_nTVCFG++;
	            }
                TVAPP_SET_BAR(pMe,pMe->m_nTVCFG);
				break;
			case AVK_UP:
				switch(pMe->m_nTVCFG)
					{
					case TVSETChannel:
						{
						   AECHAR* channel = NULL;
                          // strlight=FALSE;
						   MSG_FATAL("ToolBar_OnKeyPress channelcount=%d",pMe->pTvSetting->ChannelCountAble ,0,0);
		
						   
						   if(pMe->currentlyChannelIndex < pMe->pTvSetting->ChannelCountAble-1)
						   {
							   pMe->currentlyChannelIndex++;
						   }
						   else
						   {
							   pMe->currentlyChannelIndex = 0;
						   }
						   channel = pMe->pTvSetting->Bookmark[pMe->currentlyChannelIndex].channel; 
						   if (channel != NULL)
						   {
							   int index = (int)WSTRTOFLOAT(channel);
							   IMMITv_SetTvChannel(pMe->pIMMITv, index,FALSE); 
							   pMe->pTvSetting->CurrentChannel = index;
							   pMe->currentlyChannel = index;
		
						   }	
                           (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
						 }
					        break;
					case TVSETBrightness:
                        MSG_FATAL("TVSETBrightness-------------------",0,0,0);
                       
                        if(pMe->pTvSetting->BrightnessStep < TV_SETTING_BRIGHTNESS_MAXVALUE)
                          {
                          
                           pMe->m_barW++;
                           pMe->pTvSetting->BrightnessStep++;
                           
                          }
                         IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_BRIGHT, pMe->pTvSetting->BrightnessStep);
                        (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                        break;
                    case TVSETContrast:
                        MSG_FATAL("TVSETBrightness-------------------",0,0,0);
                       
                        if(pMe->pTvSetting->ContrastStep< TV_SETTING_CONTRAST_MAXVALUE)
                          {
                          
                           pMe->m_barW++;
                           pMe->pTvSetting->ContrastStep ++;
                           
                          }
                        IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_CONTRAST, pMe->pTvSetting->ContrastStep);
                        (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                        break;
                     case TVSETDefinition:
                        MSG_FATAL("TVSETBrightness-------------------",0,0,0);
                       
                        if(pMe->pTvSetting->DefinitionStep< TV_SETTING_DEFINITION_MAXVALUE)
                          {
                         
                           pMe->m_barW++;
                           pMe->pTvSetting->DefinitionStep ++;
                           
                          }
                        IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_DEFINITION, pMe->pTvSetting->DefinitionStep);
                        (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                        break;
                      case TVSETSaturation:
                        MSG_FATAL("TVSETBrightness-------------------",0,0,0);
                       
                        if(pMe->pTvSetting->SaturationStep< TV_SETTING_SATURATION_MAXVALUE)
                          {
                         
                           pMe->m_barW=pMe->m_barW+(TV_SETTING_SATURATION_MAXVALUE/50);
                           pMe->pTvSetting->SaturationStep = pMe->pTvSetting->SaturationStep+(TV_SETTING_SATURATION_MAXVALUE/50);
                           MSG_FATAL("pMe->pTvSetting->SaturationStep-------%d",pMe->m_barW,0,0);
                           
                          }
                        IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_SATURATION, pMe->pTvSetting->SaturationStep);
                        (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                        break;
                    case TVSETSound:
                        MSG_FATAL("TVSETBrightness-------------------",0,0,0);
                       
                        if(pMe->pTvSetting->SoundStep< TV_SETTING_SOUND_MAXVALUE)
                          {
                         
                           pMe->m_barW++;
                           pMe->pTvSetting->SoundStep ++;
                           
                          }
                        IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_SOUND, pMe->pTvSetting->SoundStep);
                        (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                        break;

					default:
							return TRUE; 

				    }
				 
				 break;
			 case AVK_DOWN:
                switch(pMe->m_nTVCFG)
					{
					case TVSETChannel:
                          {
        		            AECHAR* channel = NULL;
        					MSG_FATAL("ToolBar_OnKeyPress channelcount=%d",pMe->pTvSetting->ChannelCountAble ,0,0);

        		            
        		            if(pMe->currentlyChannelIndex > 0)
        		            {
        		                pMe->currentlyChannelIndex--;
        		            }
        		            else
        		            {
        		                pMe->currentlyChannelIndex = pMe->pTvSetting->ChannelCountAble;
        		            }
        		            channel = pMe->pTvSetting->Bookmark[pMe->currentlyChannelIndex].channel; 
        		            if (channel != NULL)
        		            {
        		                int index = (int)WSTRTOFLOAT(channel);
        		                IMMITv_SetTvChannel(pMe->pIMMITv, index,FALSE); 
        		                pMe->pTvSetting->CurrentChannel = index;
        		                pMe->currentlyChannel = index;
        				    }
                          }
                        break;
                    case TVSETBrightness:
                        if(pMe->pTvSetting->BrightnessStep > 0)
                          {
                          // pMe->m_barMAXW=TV_SETTING_BRIGHTNESS_MAXVALUE;
                           pMe->m_barW--;
                           pMe->pTvSetting->BrightnessStep--;
                           
                          }
                        IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_BRIGHT, pMe->pTvSetting->BrightnessStep);
                        (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                        break;
                    case TVSETContrast:
                        MSG_FATAL("TVSETContrastdown-----------%d",pMe->m_barW,0,0);
                        if(pMe->pTvSetting->ContrastStep > 0)
                          {
                         // pMe->m_barMAXW=TV_SETTING_CONTRAST_MAXVALUE;
                           pMe->m_barW--;
                           pMe->pTvSetting->ContrastStep--;
                           
                          }
                        IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_CONTRAST, pMe->pTvSetting->ContrastStep);
                        (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                        break;
                     case TVSETDefinition:
                        MSG_FATAL("TVSETDefinitiondown-----------%d",pMe->m_barW,0,0);
                        if(pMe->pTvSetting->DefinitionStep > 0)
                          {
                         // pMe->m_barMAXW=TV_SETTING_DEFINITION_MAXVALUE;
                           pMe->m_barW--;
                           pMe->pTvSetting->DefinitionStep--;
                           
                          }
                         IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_DEFINITION, pMe->pTvSetting->DefinitionStep);
                        (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                        break;
                      case TVSETSaturation:
                        MSG_FATAL("TVSETSaturationdown-----------%d",pMe->m_barW,0,0);
                        if(pMe->pTvSetting->SaturationStep > 0)
                          {
                         // pMe->m_barMAXW=TV_SETTING_SATURATION_MAXVALUE;
                           pMe->m_barW=pMe->m_barW-(TV_SETTING_SATURATION_MAXVALUE/50);
                           pMe->pTvSetting->SaturationStep= pMe->pTvSetting->SaturationStep - (TV_SETTING_SATURATION_MAXVALUE/50);
                           
                          }
                        IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_SATURATION, pMe->pTvSetting->SaturationStep);
                        (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                        break;
                    case TVSETSound:
                        if(pMe->pTvSetting->SoundStep > 0)
                          {
                         // pMe->m_barMAXW=TV_SETTING_SOUND_MAXVALUE;
                           pMe->m_barW--;
                           pMe->pTvSetting->SoundStep--;
                           
                          }
                        IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_SOUND, pMe->pTvSetting->SoundStep);
                        (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));
                        break;

					default:
							return TRUE; 
                    }
				
				 break;
			default:
                   return TRUE;	
			}

	        ISHELL_SendEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL);
            return TRUE;

   
    }
    return FALSE;
}

static void TVAPP_SET_BAR(CTVApp *pMe,uint32 wParam)
{
  (void)ICONFIG_GetItem(pMe->m_pConfig,CFGI_TV_SETCHANNL,pMe->pTvSetting,sizeof(CFG_TvSetting));

  switch(wParam)
         {
           case TVSETBrightness: 
               strlight=TRUE;
                pMe->m_barMAXW=TV_SETTING_BRIGHTNESS_MAXVALUE;
                pMe->m_barW=pMe->pTvSetting->BrightnessStep;
				pMe->m_barTitle=IDS_TITLE_BRIGHTNESS; 
           break;
           case TVSETContrast: 
               strlight=TRUE;
               pMe->m_barMAXW=TV_SETTING_CONTRAST_MAXVALUE;
                pMe->m_barW=pMe->pTvSetting->ContrastStep;
				pMe->m_barTitle=IDS_TITLE_CONTRAST; 
           break;
           case TVSETDefinition: 
               strlight=TRUE;
                pMe->m_barMAXW=TV_SETTING_DEFINITION_MAXVALUE;
                {
                 pMe->m_barW=pMe->pTvSetting->DefinitionStep;
				 pMe->m_barTitle=IDS_TITLE_DEFINITION; 
                }
           break;
           case TVSETSaturation: 
               strlight=TRUE;
                pMe->m_barMAXW=TV_SETTING_SATURATION_MAXVALUE;
                {
                  pMe->m_barW=pMe->pTvSetting->SaturationStep;
                  pMe->m_barTitle=IDS_TITLE_SATURATION;
                }
           break;
           case TVSETSound: 
               strlight=TRUE;
                pMe->m_barMAXW=TV_SETTING_SOUND_MAXVALUE;
                {
                 pMe->m_barW=pMe->pTvSetting->SoundStep;
				 pMe->m_barTitle=IDS_TITLE_SOUND;
                }
           break;
           default:
                 strlight=FALSE;
            break;
        }  

}


static void TVApp_DrawTopBar(CTVApp *pMe)
{
  
    IImage *Imgtoolbar = NULL;       
    AEEImageInfo myInfo;
	 Imgtoolbar = ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_TOOLBAR); 
	 
    IIMAGE_GetInfo(Imgtoolbar, &myInfo);
    {
	    IImage *pTVCFGChooseIcon = ISHELL_LoadResImage(pMe->m_pShell, TVAPP_IMAGE_RES_FILE, IDI_PNG_TV_SELECT);
	    
	    if (pTVCFGChooseIcon)
	    {  
			//IIMAGE_SetDrawSize(pCameraCFGChooseIcon, TOPBAR_ICON_WIDTH, CFGBAR_TEXT_HEIGHT);
			IIMAGE_Draw(pTVCFGChooseIcon, TVBAR_ICON_WIDTH*(pMe->m_nTVCFG)-(pMe->m_nTVCFG+1)+(pMe->m_cxWidth - myInfo.cx)/2, pMe->m_cyHeight-(myInfo.cy*2)-5);	
			MSG_FATAL("pTVCFGChooseIcon address = %x", pTVCFGChooseIcon, 0, 0);
	        IIMAGE_Release(pTVCFGChooseIcon);
	        pTVCFGChooseIcon = NULL;
	    }
	}
}


/*==============================================================================
函数：
       TV_AUTOSCAN_HandleEvent
说明：
       IDD_AUTOSCAN对话框事件处理函数
       
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
static boolean TV_AUTOSCAN_HandleEvent(CTVApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
   MSG_FATAL("TV_AUTOSCAN_HandleEvent is start--ecode=0x%x 0x%x",eCode,EVT_ATV_AUTOSCANFINISH,0);
   switch(eCode) 
    {
		
        case EVT_DIALOG_INIT:
		{
		    int result = SUCCESS;
							  
		    result = ISHELL_CreateInstance(pMe->m_pShell, MMI_CLSID_IMMITV, (void**)&pMe->pIMMITv);
		    if(pMe->pIMMITv == NULL)
			{
			    return FALSE;
			}
					   
		    (void)ICONFIG_GetItem(pMe->m_pConfig,
                      CFGI_TV_SETCHANNL,
                      pMe->pTvSetting,
                      sizeof(CFG_TvSetting));		   
		    result = IMMITv_CreateTv(pMe->pIMMITv, &pMe->myICBMMITv,pMe->pTvSetting->region);
		    if(MMI_SUCCESS(result))
			{
				MSG_FATAL("IMMITv_CreateTv---------------------success",0,0,0);
			}

		    if(pMe->pIMMITv == NULL)
			{
			  MSG_FATAL("TV_AUTOSCAN_HandleEvent---------------pIMMITv is null",0,0,0);
			}
		    IMMITv_StartPreview(pMe->pIMMITv);
		    MSG_FATAL("TV_AUTOSCAN_HandleEvent--------------IMMITv_StartPreview",0,0,0);
		    TVAPP_AutoScan(pMe);
		   
		    MSG_FATAL("TV_AUTOSCAN_HandleEvent--------------TVAPP_AutoScan",0,0,0);
            return TRUE;
        }
        case EVT_DIALOG_START:
            (void)ISHELL_PostEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL);            
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_USER_REDRAW:
			TV_UpdateInit(pMe);
			// IMENUCTL_Redraw(pMenu);
				{
				IImage *searchImg = NULL;
				AEEImageInfo myInfo;
			    searchImg = ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_AUTOSCAN); 
				 
				if(searchImg)
	            {
	            IIMAGE_GetInfo(searchImg, &myInfo);
				MSG_FATAL("m_cxWidth--%d------m_cyHeight--%d-----",pMe->m_cxWidth,pMe->m_cyHeight,0);
				MSG_FATAL("myInfoWidth--%d------myInfoHeight--%d-----",myInfo.cx,myInfo.cy,0);
				IIMAGE_Draw(searchImg,( pMe->m_cxWidth-myInfo.cx)/2,(pMe->m_cyHeight-myInfo.cy)/2);	
				TVApp_DrawBottomBarText(pMe, BTBAR_SELECT_BACK);
				IIMAGE_Release(searchImg);
	            searchImg = NULL;
	            }
            	}
			TV_Update(pMe);
            return TRUE;
            
        case EVT_COMMAND:          
            switch(wParam) 
            {
                default:         
                    break;   
            }
            return TRUE;        
        case EVT_ATV_AUTOSCANFINISH:
             MSG_FATAL("TV_AUTOSCAN_HandleEvent--------------EVT_ATV_AUTOSCANFINISH",0,0,0);
			 CTvUtil_StopSearchAnimation(pMe);
             CLOSE_DIALOG(DLGRET_MAINMENU);
			 break;
	    case EVT_NO_CLOSEBACKLIGHT:
			return TRUE;
			
        case EVT_KEY:
                {
            switch(wParam)
                    {
                        case AVK_SELECT:
                            break;
                        case AVK_CLR:
                            IMMITv_StopPreview(pMe->pIMMITv);
                            CLOSE_DIALOG(DLGRET_POPMSG);
                            return TRUE;
                        case AVK_LEFT:
                           // CLOSE_DIALOG(DLGRET_DRAWTOPBAR);
                            break;
                        case AVK_UP:
                            break;
                        case AVK_DOWN:
                            break;
                        default:         
                            break; 
                        
                    }

                   // ISHELL_SendEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL);
                    return TRUE;
                  }

        case  EVT_ATV_AUTOSCAN:
               MSG_FATAL("TV_AUTOSCAN_HandleEvent--------------EVT_ATV_AUTOSCAN",0,0,0);
               IMMITv_AutoScanTV(pMe->pIMMITv); 
    		   return TRUE;
		     // break;
		
        default:
            break;
    }
	MSG_FATAL("TV_AUTOSCAN_HandleEvent is END--------ecode=0x%x",eCode,0,0);
    return FALSE;
}




/*==============================================================================
函数：
       TV_BOOKMARK_HandleEvent
说明：
       IDD_BOOKMARK对话框事件处理函数
       
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
static boolean TV_BOOKMARK_HandleEvent(CTVApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	int i =0;
	int sNum = 0;

    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_BOOKMARK);
    MSG_FATAL("TV_BOOKMARK_HandleEvent--------------start",0,0,0);
    if(pMenu == NULL)
    {
        return FALSE;
    }
    switch(eCode) 
    {
        case EVT_DIALOG_INIT:
		  {
							  
		   MSG_FATAL("TV_BOOKMARK_HandleEvent--------------EVT_DIALOG_INIT",0,0,0);
		   MEMSET(pMe->pTvSetting,0,sizeof(CFG_TvSetting));
		  (void)ICONFIG_GetItem(pMe->m_pConfig,
                      CFGI_TV_SETCHANNL,
                      pMe->pTvSetting,
                      sizeof(CFG_TvSetting));
		   sNum=pMe->pTvSetting->ChannelCountAble;
		   for (i = sNum ; i > 0; i--)
	       {
	         // WSTRCPY(pMe->pTvSetting->Bookmark[0].name,L"21");
			 // WSTRCPY(pMe->pTvSetting->Bookmark[0].channel,L"21");(int)WSTRTOFLOAT(channel)
	          IMENUCTL_AddItem((pMenu), NULL, NULL, (int)WSTRTOFLOAT(pMe->pTvSetting->Bookmark[i-1].channel), pMe->pTvSetting->Bookmark[i-1].name, 0);
              MSG_FATAL("TV_BOOKMARK_HandleEvent--------snum=%d----------chanel=%d-------name=%d",sNum,(int)WSTRTOFLOAT(pMe->pTvSetting->Bookmark[i-1].channel),(int)WSTRTOFLOAT(pMe->pTvSetting->Bookmark[i-1].name));
			  
           }
		   (void)ICONFIG_SetItem(pMe->m_pConfig,
                                  CFGI_TV_SETCHANNL,
                                  (void*)pMe->pTvSetting,
                                  sizeof(CFG_TvSetting));
		
          }
		
            return TRUE;
        	
        case EVT_DIALOG_START:
            (void)ISHELL_PostEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL);            
            return TRUE;

        case EVT_DIALOG_END:
            return TRUE;

        case EVT_USER_REDRAW:
			TV_UpdateInit(pMe);
			IMENUCTL_Redraw(pMenu);
			#if 0
			// IMENUCTL_Redraw(pMenu);
				{
				IImage *searchImg = NULL;
				AEEImageInfo myInfo;
			    searchImg = ISHELL_LoadResImage(pMe->m_pShell, 
	                                           TVAPP_IMAGE_RES_FILE, 
	                                           IDI_PNG_TV_AUTOSCAN); 
				 
				if(searchImg)
	            {
	            IIMAGE_GetInfo(searchImg, &myInfo);
				MSG_FATAL("m_cxWidth--%d------m_cyHeight--%d-----",pMe->m_cxWidth,pMe->m_cyHeight,0);
				MSG_FATAL("myInfoWidth--%d------myInfoHeight--%d-----",myInfo.cx,myInfo.cy,0);
				IIMAGE_Draw(searchImg,( pMe->m_cxWidth-myInfo.cx)/2,(pMe->m_cyHeight-myInfo.cy)/2);	
				TVApp_DrawBottomBarText(pMe, BTBAR_SELECT_BACK);
				IIMAGE_Release(searchImg);
	            searchImg = NULL;
	            }
            	}
			#endif
			TVApp_DrawBottomBarText(pMe, BTBAR_SELECT_BACK);
			TV_Update(pMe);
            return TRUE;
            
        case EVT_COMMAND:
            return TRUE;        
             
        case EVT_KEY:
            #if 0
            if(wParam == AVK_SELECT)
            {
                int num = 6;
            	int i = 0;
            	int nSize = 0;	 
                MSG_FATAL("CTvUtil_BookmarkOperator_RemoveAll Start",0,0,0); 
            	for(; i < num; i++)
            	{
            		nSize = WSTRLEN(pMe->pTvSetting->Bookmark[i].name); 
            		MEMSET (pMe->pTvSetting->Bookmark[i].name, '\0', sizeof(AECHAR) * nSize); 

            		nSize = WSTRLEN(pMe->pTvSetting->Bookmark[i].channel); 
            		MEMSET (pMe->pTvSetting->Bookmark[i].channel, '\0', sizeof(AECHAR) * nSize); 
            	}
            	pMe->pTvSetting->Bookmarktotal = 0;
            	pMe->pTvSetting->ChannelCountAble = 0;
                 (void)ICONFIG_SetItem(pMe->m_pConfig,
                                  CFGI_TV_SETCHANNL,
                                  (void*)pMe->pTvSetting,
                                  sizeof(CFG_TvSetting));
            // CLOSE_DIALOG(DLGRET_POPMSG);

			}
            #endif
			if(wParam == AVK_CLR)
            {
                IMMITv_StopPreview(pMe->pIMMITv);
                CLOSE_DIALOG(DLGRET_POPMSG);
            }
			
            return TRUE;

    }
    return FALSE;
}

/*==============================================================================
函数：
       IDD_TVWAITING_HandleEvent
说明：
       IDD_TVWAITING对话框事件处理函数
       
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
static boolean TV_TVWAITING_HandleEvent(CTVApp *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
    
    static IStatic *pStatic = NULL;       
    pMe->m_nMsgTimeout = TIMEOUT_MS_MSGBOX;
    MSG_FATAL("TV_TVWAITING_HandleEvent ----- eCode=%d",eCode,0,0);
    switch(eCode) 
    {
        case EVT_DIALOG_INIT:
    	    {
                AECHAR WTitle[40] = {0};
                MSG_FATAL("TV_TVWAITING_HandleEvent ----- EVT_DIALOG_INIT",0,0,0);
                (void)ISHELL_LoadResString(pMe->m_pShell,
                                            AEE_APPSTVAPP_RES_FILE,                                
                                            IDS_ITEM_TV,
                                            WTitle,
                                            sizeof(WTitle));
                
    			
    		    if(pMe->m_pIAnn != NULL)
            	{
    		    	IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
    			}
                    
                IDISPLAY_SetClipRect(pMe->m_pDisplay, NULL); 
    			IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,TRUE);
            }
            
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
        	
        case EVT_DIALOG_START:
              MSG_FATAL("TV_TVWAITING_HandleEvent ----- EVT_DIALOG_START",0,0,0);
              (void)ISHELL_SetTimer(pMe->m_pShell,
                                      pMe->m_nMsgTimeout,
                                      TvApp_DialogTimeout,
                                      pMe);
              
            (void)ISHELL_PostEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL); 
            return TRUE;

        case EVT_DIALOG_END:
            MSG_FATAL("TV_TVWAITING_HandleEvent ----- EVT_DIALOG_END",0,0,0);
            if(pStatic != NULL)
            {
                MSG_FATAL("pStatic address = %x", pStatic, 0, 0);
                ISTATIC_Release(pStatic); 
                pStatic = NULL;
            }
            return TRUE;
            
        case EVT_USER_REDRAW:
			if(pStatic)
            {             
                PromptMsg_Param_type PromptMsg={0};  
                AECHAR  wstrText[40]={0};   
                MSG_FATAL("TV_TVWAITING_HandleEvent ----- EVT_USER_REDRAW",0,0,0);
                (void)ISHELL_LoadResString(pMe->m_pShell,
                                               AEE_APPSTVAPP_RES_FILE,
                                               IDS_MSG_WAITING,
                                               wstrText,
                                               sizeof(wstrText)); 
                PromptMsg.ePMsgType = MESSAGE_WAITING;
                PromptMsg.eBBarType = BTBAR_NONE;// BTBAR_OK_BACK;
               
                PromptMsg.pwszMsg = wstrText;
                
                DrawPromptMessage(pMe->m_pDisplay, pStatic, &PromptMsg); 
                MSG_FATAL("TV_TVWAITING_HandleEvent ----- EVT_USER_REDRAW----end",0,0,0);
            }
            return TRUE;
            
        case EVT_APP_DIALOG_TIMEOUT:
            MSG_FATAL("TV_TVWAITING_HandleEvent ----- EVT_APP_DIALOG_TIMEOUT",0,0,0);
       
           (void)ISHELL_CancelTimer(pMe->m_pShell,
                                     TvApp_DialogTimeout,
                                     pMe);
            CLOSE_DIALOG(DLGRET_MAINMENU);
           
            return TRUE;
            
        case EVT_NO_CLOSEBACKLIGHT:
			return TRUE;
			
        case EVT_COMMAND:
            return TRUE;        
             
        case EVT_KEY:
			
            return TRUE;

        default:
            break;

    }
    return FALSE;
}

static void TvApp_DialogTimeout(void * pme)
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
 // AECHAR WTitle[40] = {0};
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
			#if 0
		   	(void)ISHELL_LoadResString(pMe->m_pShell,
                            AEE_APPSTVAPP_RES_FILE,                                
                            IDS_ITEM_TV,
                            WTitle,
                            sizeof(WTitle));
			
		    if(pMe->m_pIAnn != NULL)
        	{
		    	IANNUNCIATOR_SetFieldText(pMe->m_pIAnn,WTitle);
			}
                
            IDISPLAY_SetClipRect(pMe->m_pDisplay, NULL); 
			

            IANNUNCIATOR_EnableAnnunciatorBar(pMe->m_pIAnn,AEECLSID_DISPLAY1,TRUE);
			#endif
		  TV_Build_MainOpts_Menu(pMe,pMenuCtl);
		 IMENUCTL_SetPopMenuRect(pMenuCtl);
		 IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
	   
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
            //IMENUCTL_SetProperties(pMenuCtl, MP_UNDERLINE_TITLE |MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
			
			IMENUCTL_Redraw(pMenuCtl);
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
                    CLOSE_DIALOG(DLGRET_MAINMENU);//DLGRET_CANCELED
                    return TRUE;
                }
                case AVK_BGPLAY:
				
                    return TRUE;
                    
                default:
                    break;                    
            }
            return TRUE; 
		case EVT_COMMAND:          
            switch(wParam) 
            {
            case IDS_REGION: 
				MSG_FATAL("IDS_REGION----------------------------",0,0,0);
				 CLOSE_DIALOG(DLGRET_REGION);
				break; 
			
            case IDS_AUTO_SCAN:
                 pMe->m_bAUTOSCAN=TRUE;
                 pMe->m_curChnIdx=0;
				 CLOSE_DIALOG(DLGRET_MAINMENU); //DLGRET_MAINMENU DLGRET_AUTOSCAN
                 break;	
			case IDS_TV_FAVORITE:
				MSG_FATAL("IDS_TV_FAVORITE-----------------------------",0,0,0);
				 CLOSE_DIALOG(DLGRET_BOOKMARK); //
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
    	case EVT_COMMAND:          
	        
	         if(((wParam-TV_FIRST_REGION) > TLG_REGION_START) && ((wParam-TV_FIRST_REGION) < TLG_REGION_TOTAL))
	         {
	         MSG_FATAL("------------pTvSetting.region=%d",wParam,0,0);
		    IMMITv_SetRegion(pMe->pIMMITv, wParam-TV_FIRST_REGION);  //设置区域
		    pMe->pTvSetting->region = (TLG_REGION_CODE)(wParam-TV_FIRST_REGION);
            MSG_FATAL("pMe->pTvSetting->region ::::%d",pMe->pTvSetting->region,0,0);
			 (void)ICONFIG_SetItem(pMe->m_pConfig,
                                  CFGI_TV_SETCHANNL,
                                  (void*)pMe->pTvSetting,
                                  sizeof(CFG_TvSetting));
			 CLOSE_DIALOG(DLGRET_MAINMENU);

			 }
		return TRUE;     
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
            return TRUE;
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
        case EVT_COMMAND:          
	        
	         if(((wParam-TV_FIRST_REGION) > TLG_REGION_START) && ((wParam-TV_FIRST_REGION) < TLG_REGION_TOTAL))
	         {
	         MSG_FATAL("------------pTvSetting.region=%d",wParam,0,0);
		    IMMITv_SetRegion(pMe->pIMMITv, wParam-TV_FIRST_REGION);  //设置区域
		    pMe->pTvSetting->region = (TLG_REGION_CODE)(wParam-TV_FIRST_REGION);
            MSG_FATAL("pMe->pTvSetting->region ::::%d",pMe->pTvSetting->region,0,0);
			 (void)ICONFIG_SetItem(pMe->m_pConfig,
                                  CFGI_TV_SETCHANNL,
                                  (void*)pMe->pTvSetting,
                                  sizeof(CFG_TvSetting));
			 CLOSE_DIALOG(DLGRET_MAINMENU);

			 }
		return TRUE;    
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
    	case EVT_COMMAND:          
        
         if(((wParam-TV_FIRST_REGION) > TLG_REGION_START) && ((wParam-TV_FIRST_REGION) < TLG_REGION_TOTAL))
         {
         MSG_FATAL("------------pTvSetting.region=%d",wParam,0,0);
	    IMMITv_SetRegion(pMe->pIMMITv, wParam-TV_FIRST_REGION);  //设置区域
	    pMe->pTvSetting->region = (TLG_REGION_CODE)(wParam-TV_FIRST_REGION);
        MSG_FATAL("pMe->pTvSetting->region ::::%d",pMe->pTvSetting->region,0,0);
		 (void)ICONFIG_SetItem(pMe->m_pConfig,
                              CFGI_TV_SETCHANNL,
                              (void*)pMe->pTvSetting,
                              sizeof(CFG_TvSetting));
		 CLOSE_DIALOG(DLGRET_MAINMENU);

		 }
		return TRUE;    
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
    	case EVT_COMMAND:          
        
         if(((wParam-TV_FIRST_REGION) > TLG_REGION_START) && ((wParam-TV_FIRST_REGION) < TLG_REGION_TOTAL))
         {
	         MSG_FATAL("------------pTvSetting.region=%d",wParam,0,0);
		    IMMITv_SetRegion(pMe->pIMMITv, wParam-TV_FIRST_REGION);  //设置区域
		    pMe->pTvSetting->region = (TLG_REGION_CODE)(wParam-TV_FIRST_REGION);
	        MSG_FATAL("pMe->pTvSetting->region ::::%d",pMe->pTvSetting->region,0,0);
			 (void)ICONFIG_SetItem(pMe->m_pConfig,
	                              CFGI_TV_SETCHANNL,
	                              (void*)pMe->pTvSetting,
	                              sizeof(CFG_TvSetting));
			 CLOSE_DIALOG(DLGRET_MAINMENU);

		 }
		return TRUE;    
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
    	case EVT_COMMAND:          
        
         if(((wParam-TV_FIRST_REGION) > TLG_REGION_START) && ((wParam-TV_FIRST_REGION) < TLG_REGION_TOTAL))
         {
	         MSG_FATAL("------------pTvSetting.region=%d",wParam,0,0);
		    IMMITv_SetRegion(pMe->pIMMITv, wParam-TV_FIRST_REGION);  //设置区域
		    pMe->pTvSetting->region = (TLG_REGION_CODE)(wParam-TV_FIRST_REGION);
	        MSG_FATAL("pMe->pTvSetting->region ::::%d",pMe->pTvSetting->region,0,0);
			 (void)ICONFIG_SetItem(pMe->m_pConfig,
	                              CFGI_TV_SETCHANNL,
	                              (void*)pMe->pTvSetting,
	                              sizeof(CFG_TvSetting));
			 CLOSE_DIALOG(DLGRET_MAINMENU);

		 }
		return TRUE;    
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

#if 0
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

#endif
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
            
           // TVApp_InitpopMenu(pMe, popMenu);
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
               // TVApp_CPreviewStart(pMe);
                pMe->m_bRePreview = FALSE;
            }
                  
            IDISPLAY_SetClipRect(pMe->m_pDisplay, NULL);

            TVApp_DrawBottomBarText(pMe, BTBAR_SELECT_BACK);
            TVApp_DrawTopBar(pMe);   
           // TVApp_DrawCFGPromptText(pMe);                        
            
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
                  //  ICAMERA_Stop(pMe->m_pTV);
                    pMe->m_nTVState = TV_STOP;
                    return FALSE;
                    
                default:
                    return TRUE;
            }
            
            //左右键切换不同的设置菜单，需要重新初始化菜单项
           // TVApp_InitpopMenu(pMe, popMenu);

            return TRUE;//ISHELL_SendEvent(pMe->m_pShell, AEECLSID_TVAPP, EVT_USER_REDRAW, NULL, NULL);
            
       // case EVT_COMMAND:
            //return TVApp_RoutePopMenuCommandEvent(pMe, wParam);
  
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
          //  TVApp_HandleSnapshotPic(pMe);
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
            
           // pMe->m_wMsgID = IDS_DONE;
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

/*添加OPT主菜单*/
static void TV_Build_MainOpts_Menu(CTVApp *pMe,IMenuCtl *pMenuCtl)
{
    TVApp_SetPopMenuRect(pMe, pMenuCtl, 5);
    TVMENU_ADDITEM(pMenuCtl,IDS_TV_FAVORITE);
    TVMENU_ADDITEM(pMenuCtl,IDS_REGION);
    TVMENU_ADDITEM(pMenuCtl,IDS_AUTO_SCAN);
    //TVMENU_ADDITEM(pMenuCtl,IDS_LANDSCAPE);
	//TVMENU_ADDITEM(pMenuCtl,IDS_PORTRAIT);
         
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

static int TVAPP_AutoScan(CTVApp *pMe)
{
   MSG_FATAL("TVAPP_AutoScan Start",0,0,0);

   IMMITv_AutoScanTV(pMe->pIMMITv);   
   MSG_FATAL("TVAPP_AutoScan END",0,0,0);
#if 0
   if(channelCount > 0)
    {
        pMe->pTvSetting->ChannelCountAble = channelCount;
        pMe->pTvSetting->CurrentChannel = ableChannelArray[0];//自动搜索完成后，会跳到索引为0的频道来
        pMe->currentlyChannel = ableChannelArray[0];
        pMe->currentlyChannelIndex = 0;
        result = IMMITv_SetTvChannel(pMe->pIMMITv, ableChannelArray[0],FALSE); 
        if (pMe->pIMMITv != NULL)
        {
           // IMMITv_SetProperty(pThis->pIMMITv, TV_PROPERTY_SOUND, pTvSetting->SoundStep);
            //IMMIAudioDevice_GetOutputVolumn(pThis->pIMMIAudioDevice,AUDIOVOLUME_LEVEL0);
        }
		MSG_FATAL("CTvUtil_BookmarkOperator_InsertAt-----------1",0,0,0);
        for(; i < channelCount; i++)
        {
        MSG_FATAL("CTvUtil_BookmarkOperator_InsertAt-----------i=%d",i,0,0);
  /*          SPRINTF(title, "%d", i+1);
			 // SPRINTF(channel, "%d",  ableChannelArray[i]);
            SPRINTF(channel, "%d",  i+20);
            CTvUtil_BookmarkOperator_InsertAt(pMe,channel, title, 0); 
*/

			WSPRINTF(pMe->pTvSetting->Bookmark[i].name, 32*sizeof(AECHAR), L"%d", i+1);
			WSPRINTF(pMe->pTvSetting->Bookmark[i].channel, 32*sizeof(AECHAR), L"%d", ableChannelArray[i]);
        }
		MSG_FATAL("CTvUtil_BookmarkOperator_InsertAt-----------10",0,0,0);
        IMMITv_ResetScanTag(pMe->pIMMITv);
		(void)ICONFIG_SetItem(pMe->m_pConfig,
                                  CFGI_TV_SETCHANNL,
                                  (void*)pMe->pTvSetting,
                                  sizeof(CFG_TvSetting));
        //CTvUtil_UI_ShowCurrentChannel(pThis, TRUE);  
        //CLOSE_DIALOG(DLGRET_MAINMENU);
   }  
   #endif
    return SUCCESS;
}

static void CTvUtil_StopSearchAnimation(CTVApp * pMe)
{  
    int result = SUCCESS;
    int i = 0;
    int a = 0;
    //char title[32];
   // char channel[32];
    AECHAR Aname[32] = {0};
    AECHAR AChannel[32] = {0};
   //CTVApp* pThis = (CTVApp*)pITvUtil->pData;
    uint16 *ableChannelArray = IMMITv_getAbleChannelArray(pMe->pIMMITv);
    int channelCount = IMMITv_getChannelCountAble(pMe->pIMMITv);
    (void)ICONFIG_GetItem(pMe->m_pConfig,
                            CFGI_TV_SETCHANNL,
                            (void*)pMe->pTvSetting,
                            sizeof(CFG_TvSetting));

    MSG_FATAL("CTvUtil_StopSearchAnimation Start  ----channelCount=%d",channelCount,0,0);

    if(channelCount > 0)
    {
    {
          int num = pMe->pTvSetting->ChannelCountAble;
          int b = 0;
          int nSize = 0;   
          MSG_FATAL("CTvUtil_BookmarkOperator_RemoveAll Start",0,0,0); 
          (void)ICONFIG_GetItem(pMe->m_pConfig,
                                 CFGI_TV_SETCHANNL,
                                 (void*)pMe->pTvSetting,
                                 sizeof(CFG_TvSetting));
          for(; b <= num; b++)
          {
              nSize = WSTRLEN(pMe->pTvSetting->Bookmark[b].name); 
              MEMSET (pMe->pTvSetting->Bookmark[b].name, '\0', sizeof(AECHAR) * nSize); 
        
              nSize = WSTRLEN(pMe->pTvSetting->Bookmark[b].channel); 
              MEMSET (pMe->pTvSetting->Bookmark[b].channel, '\0', sizeof(AECHAR) * nSize); 
          }
          pMe->pTvSetting->Bookmarktotal = 0;
          pMe->pTvSetting->ChannelCountAble = 0;
          (void)ICONFIG_SetItem(pMe->m_pConfig,
                                CFGI_TV_SETCHANNL,
                                (void*)pMe->pTvSetting,
                                sizeof(CFG_TvSetting));
         }
    
        pMe->pTvSetting->ChannelCountAble = channelCount;
       
        pMe->pTvSetting->CurrentChannel = ableChannelArray[0];//自动搜索完成后，会跳到索引为0的频道来
        pMe->currentlyChannel = ableChannelArray[0];
        pMe->currentlyChannelIndex = 0;
        result = IMMITv_SetTvChannel(pMe->pIMMITv, ableChannelArray[0],FALSE); 
       MSG_FATAL("search finish，have %d channel",channelCount,0,0);
        if (pMe->pIMMITv != NULL)
        {
           MSG_FATAL("TV_PROPERTY_SOUND------------",0,0,0);
           IMMITv_SetProperty(pMe->pIMMITv, TV_PROPERTY_SOUND, pMe->pTvSetting->SoundStep);
            //IMMIAudioDevice_GetOutputVolumn(pThis->pIMMIAudioDevice,AUDIOVOLUME_LEVEL0);
        }
        for(; i < channelCount; i++)
        {
            MSG_FATAL("channelCount------------channelCount=%d",channelCount,0,0);
           // SPRINTF(title, "%d", i+1);
           // SPRINTF(channel, "%d",  ableChannelArray[i]);

            WSPRINTF(Aname,sizeof(Aname),(AECHAR*)L"%d(%d)",i+1,ableChannelArray[i]);
            WSPRINTF(AChannel,sizeof(AChannel),(AECHAR*)L"%d",ableChannelArray[i]);
            WSTRCPY(pMe->pTvSetting->Bookmark[a].name,Aname);
            WSTRCPY(pMe->pTvSetting->Bookmark[a].channel,AChannel);
            
            //WSTRCPY(pMe->pTvSetting->Bookmark[a].name,(AECHAR *)(title));
            //WSTRCPY(pMe->pTvSetting->Bookmark[a].channel,(AECHAR *)(channel));
            MSG_FATAL("Bookmark-i=%d---------channel=%d",a, ableChannelArray[i],0);
            a++;
            //STRTOWSTR(emerg_tab.emerg_num[i].num_buf, wstrNum, sizeof(wstrNum));
           
           // int16 nTime=127;
            

               // WSTRLCPY(wFormat,L"%02d/%02d",63);

            //WSPRINTF(wszDate, sizeof(wszDate), wFormat, jDate.wMonth, jDate.wDay, jDate.wYear);
        }
        (void)ICONFIG_SetItem(pMe->m_pConfig,
                            CFGI_TV_SETCHANNL,
                            (void*)pMe->pTvSetting,
                            sizeof(CFG_TvSetting));
        IMMITv_ResetScanTag(pMe->pIMMITv);
        
       // CTvUtil_UI_ShowCurrentChannel(pThis, TRUE);  

       
   }
}


void CTvUtil_BookmarkOperator_InsertAt(CTVApp *pMe,char* in_channel, char *in_title, uint16 *out_index)
{
    int nSize = 0;   
    nSize = STRLEN(in_title) + 1;
	UTF8TOWSTR((byte*)in_title,nSize,pMe->pTvSetting->Bookmark[pMe->pTvSetting->Bookmarktotal].name, sizeof(AECHAR) * nSize);
   // MMI_Utf8ToWstr((in_title), pMe->pTvSetting->Bookmark[pMe->pTvSetting->Bookmarktotal].name, sizeof(AECHAR) * nSize);
    nSize = STRLEN(in_channel) + 1;
   UTF8TOWSTR((byte*)in_channel,nSize, pMe->pTvSetting->Bookmark[pMe->pTvSetting->Bookmarktotal].channel, sizeof(AECHAR) * nSize); 
    //MMI_Utf8ToWstr((in_channel), pMe->pTvSetting->Bookmark[pMe->pTvSetting->Bookmarktotal].channel, sizeof(AECHAR) * nSize);  
    pMe->pTvSetting->Bookmarktotal++;
	pMe->pTvSetting->ChannelCountAble++;
}

#if 0

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
#endif
#if 0
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
#endif
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
#if 0
static void TVApp_DrawMidPic(CTVApp *pMe)
{
   // IImage *pImage = NULL;
   // AEEImageInfo myInfo;
    
    switch(pMe->m_pActiveDlgID)
    {
        case IDD_CPREVIEW:
          /*  pImage = ISHELL_LoadResImage(pMe->m_pShell,
                                         TVAPP_IMAGE_RES_FILE,
                                         IDI_MID_CAMERA);
            if(pImage != NULL)
            {
                IIMAGE_GetInfo(pImage, &myInfo);
                IIMAGE_Draw(pImage, (pMe->m_cxWidth - myInfo.cx)/2, pMe->m_cyHeight - myInfo.cy -1 );
                IIMAGE_Release(pImage);
                pImage = NULL;
            } */
            break;
            
        default:
            break;
    }
}
#endif
#if 0
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
#endif
#if 0
static boolean TVApp_FindMemoryCardExist(CTVApp *pMe)
{   
    return (IFILEMGR_Test(pMe->m_pFileMgr, AEEFS_CARD0_DIR)==SUCCESS)?TRUE:FALSE;	
}
#endif
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
#if 0
static boolean TVApp_SetDateForRecordFileName(CTVApp *pMe)
{
    AEEMediaData   md;
    
    //Fill media data
    md.clsData = MMD_FILE_NAME;
    md.pData = (void *)pMe->m_sCurrentFileName;
    md.dwSize = 0;

    (void)ICAMERA_SetMediaData(pMe->m_pTV, &md, 0);

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
#endif
static void TVApp_EventNotify(CTVApp *pMe, AEECameraNotify *pcn)
{
    if (!pMe || !pcn || !pMe->m_pTV)
        return;
    
    switch (pcn->nCmd){
    case CAM_CMD_START:
        switch (pcn->nStatus){
        case CAM_STATUS_FRAME:
            //It is the image from the viewfinder.
           // TVApp_UpdateFrame(pMe);
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
 
    if(pMe->m_pTV == NULL)
    {
        ISHELL_CreateInstance(pMe->m_pShell, 
                              AEECLSID_CAMERA, 
                              (void **)&pMe->m_pTV);
    }
    if(pMe->m_pTV != NULL)
    {
        MSG_FATAL("pMe->m_pTV != NULL",0,0,0);
        ICAMERA_GetParm(pMe->m_pTV, CAM_PARM_SENSOR_MODEL, &(pMe->m_sensor_model), NULL);
        MSG_FATAL("m_sensor_model=%d",pMe->m_sensor_model,0,0);            
    }
    else
    {
        MSG_FATAL("pMe->m_pTV == NULL",0,0,0);
    }
    if(pMe->m_pTV)
    {
        ICAMERA_RegisterNotify(pMe->m_pTV,(PFNCAMERANOTIFY)TVApp_EventNotify,po);
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
                //TVApp_RecordSnapShot(pMe);
            }
            else if(pMe->m_nTVState == TV_SAVE)
            {
                //TVApp_SavePhoto(pMe);
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
            //TVApp_PlayShutterSound(pMe);
#else
            if(pMe->m_nTVTone == OEMNV_CAMERA_SHUTTER_TONE_ENABLE)
            {
                //TVApp_PlayShutterSound(pMe);
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


static int TV_UpdateInit(CTVApp *pMe)
{
    IBitmap* pbmp = NULL;
    IDISPLAY_GetDeviceBitmap(pMe->m_pDisplay, &pbmp);
	if(pbmp)
	{
        IBITMAP_FillRect(pbmp, &pMe->m_rc, TRANS_COLOR, AEE_RO_COPY);
        MSG_FATAL("pbmp address = %x", pbmp, 0, 0);
		IBITMAP_Release(pbmp);  
        MSG_FATAL("TV_UpdateInit1-------x%d---------y%d",pMe->m_rc.dx,pMe->m_rc.dy,0);
        MSG_FATAL("TV_UpdateInit2-------x%d---------y%d",pMe->m_rc.x,pMe->m_rc.y,0);
	}
    return SUCCESS;
}
static int TV_Update(CTVApp *pMe)
{
   
        IBitmap* pbmp = NULL;
        IDIB* pdib = NULL;
        
        IDISPLAY_UpdateEx(pMe->m_pDisplay, TRUE); 
        IDISPLAY_GetDeviceBitmap(pMe->m_pDisplay, &pbmp);           
        IBITMAP_QueryInterface(pbmp, AEECLSID_DIB, (void**)&pdib);
		MSG_FATAL("IMMITv_updateimg--------------------------start",0,0,0);
        IMMITv_updateimg(pMe->pIMMITv, (uint32)pdib->pBmp);
		
        MSG_FATAL("pbmp address = %x", pbmp, 0, 0);
        IBITMAP_Release(pbmp);  
        IDIB_Release(pdib);
        MSG_FATAL("IMMITv_updateimg--------------------------end",0,0,0);
        return SUCCESS;
    
}
