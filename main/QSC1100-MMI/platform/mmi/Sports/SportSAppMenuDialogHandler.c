#ifndef _SPORTSAPPMENU_DIALOG_HANDLER_C_
#define _SPORTSAPPMENU_DIALOG_HANDLER_C_
/*********************************************************************************

模块名       : <SportApplication>
文件名       : <SportSAppMenuDialogHandler.c>
相关文件     : 
文件实现功能 : Sports Application
作者         : 
版本         : 1.0
--------------------------------------------------------------------------------
备注         : 
--------------------------------------------------------------------------------
修改记录 : 

******************************************************************************/



/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "SportSAppDialogGprot.h"
#include "SportSAppOperateData.h"

#include "AEECM.h"
#ifdef FEATURE_SID_LOCK
#include "OEMNV.h"
//#include "OEMFeatures.h"
#endif
#include "OEMRTC.h"
#ifdef FEATURE_COLOR_DISPLAY
#include "appscommon_color.brh"
#else
#include "appscommon_momo.brh"
#endif
#ifdef FEATURE_LCD_TOUCH_ENABLE   //add by ydc
#define DISPLAYMENU_PT_IN_RECT(a,b,rct)      (boolean)( ((a) >= (rct).x && (a) <= ((rct).x + (rct).dx)) && ((b) >= (rct).y && (b) <= ((rct).y + (rct).dy)) )

#endif
#include "appscommon.h" 

#ifdef FEATURE_APP_MUSICPLAYER
#include "MusicPlayer.h"
#endif
/*==============================================================================
                                 宏定义和常数
==============================================================================*/
// 关闭对话框宏
#define CLOSE_DIALOG(DlgRet)  {    \
                                     pMe->m_eDlgRet = DlgRet; \
                                     (void) ISHELL_EndDialog(pMe->m_pShell);  \
                                 }

boolean g_sportsapp_entry_pedometer_flag = FALSE;
boolean g_sportsapp_pedometer_bground_flag = FALSE;
boolean g_sportsapp_entry_view_flag = FALSE;
/*==============================================================================
                                 类型定义
==============================================================================*/


/*==============================================================================
                                 函数声明
==============================================================================*/
// 对话框 IDD_MAIN_MENU 事件处理函数
static boolean  HandleMainDialogEvent(CSportsMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_MAIN_PEDOMETER 事件处理函数
static boolean  HandlePedometerDialogEvent(CSportsMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_MAIN_PEDOMETER_OPTION 事件处理函数
static boolean  HandlePedometerOptionDialogEvent(CSportsMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);


// 对话框 IDD_MAIN_PEDOMETER_RECORD 事件处理函数
static boolean  HandlePedometerOptRecordDialogEvent(CSportsMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_MAIN_PEDOMETER_RECORD_VIEW 事件处理函数
static boolean  HandlePedometerRecViewDialogEvent(CSportsMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_MAIN_PERSONAL 事件处理函数
static boolean  HandlePersonalDialogEvent(CSportsMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_MAIN_PERSONAL_OPTION 事件处理函数
static boolean  HandlePersonalOptionDialogEvent(CSportsMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_MAIN_PERSONAL_EDIT 事件处理函数
static boolean  HandlePersonalEditDialogEvent(CSportsMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

static boolean	HandlePersonalInputDialogEvent(CSportsMenu *pMe,
	AEEEvent eCode,
	uint16 wParam,
	uint32 dwParam
	);

// 对话框 IDD_MESSEGE_BOX 事件处理函数
static boolean  HandleMessegeBoxDialogEvent(CSportsMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
);

// 对话框 IDD_APP_BGROUND 事件处理函数
static boolean	HandleAppConfirmBgRunEvent(CSportsMenu *pMe,
	AEEEvent eCode,
	uint16 wParam,
	uint32 dwParam
	);

static void SportSApp_DialogTimeout(void *pme);

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
       SportSApp_ShowDialog
说明:
       函数由状态处理函数用来创建相应对话框资源ID的对话框。

参数:
       pMe [In]: 指向CSportsMenu Applet对象结构的指针,该结构包含小程序的特定信息.
       dlgResId [in]：对话框资源ID，函数根据该ID创建对话框。

返回值:
       无

备注:


==============================================================================*/
void SportSApp_ShowDialog(CSportsMenu *pMe,uint16  dlgResId)
{
    int nRet;
    //SETTING_ERR("next show %d dialog", dlgResId, 0, 0);
    // At most one dialog open at once
    if (ISHELL_GetActiveDialog(pMe->m_pShell) != NULL)
    {
        // Looks like there is one dialog already opened.
        // Flag an error an return without doing anything.
        SPORTS_ERR("Trying to create a dialog without closing the previous one",0,0,0);
        return;
    }

    nRet = ISHELL_CreateDialog(pMe->m_pShell,AEE_APPSSPORTSMENU_RES_FILE,dlgResId,NULL);

    if (nRet != SUCCESS)
    {
        SPORTS_ERR("Failed to create the dialog in the SettingMenu applet",0,0,0);
    }
}

/*==============================================================================
函数:
    SportSApp_ShowMsgBox

说明:
    函数由状态处理函数用来唤起消息对话框，显示消息提示用户。

参数:
    pMe [in]: 指向 Contact Applet 对象结构的指针。该结构包含小程序的特定信息。
    wTextResId [in]: 消息文本资源 ID。

返回值:
    none

备注:

==============================================================================*/
int SportSApp_ShowMsgBox(CSportsMenu *pMe, uint16 wTextResId)
{
    pMe->m_pSportsAppMenu.m_wMsgResID = wTextResId;
    
    SportSApp_ShowDialog(pMe, IDD_MSGBOX);
    return 0;
}

/*==============================================================================
函数:
       SportSApp_RouteDialogEvent
说明:
       函数将BREW事件路由给当前活动对话框的事件处理函数。

参数:
       pMe [In]: 指向SportSApp Applet对象结构的指针,该结构包含小程序的特定信息.
       eCode [in]：事件代码。
       wParam [in]：与事件eCode关联的数据。
       dwParam [in]：与事件eCode关联的数据。

返回值:
       TRUE：传入事件被处理。
       FALSE：传入事件没被处理。

备注:

==============================================================================*/
boolean SportSApp_RouteDialogEvent(CSportsMenu *pMe,
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
    SPORTS_ERR("%d SettingMenu_RouteDialogEvent", pMe->m_pActiveDlgID, 0, 0);
    switch (pMe->m_pActiveDlgID)
    {
        case IDD_DIALOG_MAIN:
            return HandleMainDialogEvent(pMe,eCode,wParam,dwParam);
			
		case IDD_DIALOG_PEDOMETER:
			return HandlePedometerDialogEvent(pMe,eCode,wParam,dwParam);
			
		case IDD_DIALOG_PEDOMETER_OPTION:
			return HandlePedometerOptionDialogEvent(pMe,eCode,wParam,dwParam);
			
		case IDD_DIALOG_PED_RECORD:
			return HandlePedometerOptRecordDialogEvent(pMe,eCode,wParam,dwParam);
			
		case IDD_DIALOG_PED_RECORD_VIEW:
			return HandlePedometerRecViewDialogEvent(pMe,eCode,wParam,dwParam);
		/*Personal Dialog Event Handler*/	
		case IDD_DIALOG_PERSONAL:
			return HandlePersonalDialogEvent(pMe,eCode,wParam,dwParam);
			
		case IDD_DIALOG_PER_OPTION:
			return HandlePersonalOptionDialogEvent(pMe,eCode,wParam,dwParam);
		/*Personal edit Dialog Event Handler*/	
		case IDD_DIALOG_PER_EDIT:
			return HandlePersonalEditDialogEvent(pMe,eCode,wParam,dwParam);
			
		case IDD_DIALOG_INPUT:
			return HandlePersonalInputDialogEvent(pMe,eCode,wParam,dwParam);
			
		case IDD_MSGBOX:
			return HandleMessegeBoxDialogEvent(pMe,eCode,wParam,dwParam);
			
		case IDD_APP_BGROUND:
			return HandleAppConfirmBgRunEvent(pMe,eCode,wParam,dwParam);
        	default:
            	return FALSE;
    }
}

/*==============================================================================
函数：
       HandleMainDialogEvent
说明：
       IDD_DIALOG_MAIN对话框事件处理函数

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
static boolean  HandleMainDialogEvent(CSportsMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
	uint16 bytNewData;
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_MENU);
    if (pMenu == NULL)
    {
        return FALSE;
    }
    SPORTS_ERR("%x, %x ,%x,HandleMainDialogEvent",eCode,wParam,dwParam);
 
    switch (eCode)
    {
        case EVT_DIALOG_INIT:
			(void)ICONFIG_GetItem(pMe->m_pConfig,
							  CFGI_SPORT_FLAG,
							  &bytNewData, sizeof(bytNewData));
			
		if(bytNewData >= 3)
		{
			pMe->m_pSportsAppMenu.m_nSelProFile = 0;
		}
		else
		{
			pMe->m_pSportsAppMenu.m_nSelProFile = bytNewData;
		}
                /*Get Sport Data From File System*/
		SportSApp_LoadSportData(pMe);		

		IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_SPORTS_PEDOMETER, IDS_SPORTS_PEDOMETER, NULL, 0);
        #ifdef FEATURE_APP_CALENDAR
            IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_SPORTS_PLAN, IDS_SPORTS_PLAN, NULL, 0);
        #endif /*FEATURE_APP_CALENDAR*/
            IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_SPORTS_PERSONAL, IDS_SPORTS_PERSONAL, NULL, 0);	
            return TRUE;

        case EVT_DIALOG_START:
			
            IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);
            IMENUCTL_SetOemProperties(pMenu,OEMMP_USE_MENU_STYLE);
            IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
            IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);

            (void) ISHELL_PostEvent( pMe->m_pShell,
                                     AEECLSID_SPORTSAPP,
                                     EVT_USER_REDRAW,
                                     0,
                                     0);

            return TRUE;

        case EVT_USER_REDRAW:
            (void)IMENUCTL_Redraw(pMenu);
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
            //pMe->m_currDlgId = IMENUCTL_GetSel(pMenu);
            pMe->m_currDlgId = wParam;
            switch (wParam)
            {
                	case IDS_SPORTS_PEDOMETER:      //petometer
                		CLOSE_DIALOG(DLGRET_PEDOMETER)
                    	break;

               	case IDS_SPORTS_PLAN:  //plan
                    SportSApp_SportsPlan(pMe);
                    	break;

                case IDS_SPORTS_PERSONAL:      //personal
				CLOSE_DIALOG(DLGRET_PERSONAL)		
                    	break;

                default:
                    	ASSERT_NOT_REACHABLE;
            }
            return TRUE;

        default:
            break;
    }
    return FALSE;
} 


/*==============================================================================
函数：
       HandleMainDialogEvent
说明：
       IDD_DIALOG_MAIN对话框事件处理函数

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
#ifdef FEATURE_SPORTS_APP
extern Sport_Appointment pSportAppt;
#endif

static boolean  HandlePedometerDialogEvent(CSportsMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
	ProFileUser active_user; 
	active_user = SportsApp_GetCurrentActiveUser(pMe);

	SPORTS_ERR("%x, %x ,%x,HandleMainDialogEvent",eCode,wParam,dwParam);
#ifdef FEATURE_LCD_TOUCH_ENABLE//wlh add for LCD touch

	if ((eCode == EVT_PEN_UP))
	{
			int i;
			int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
			int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
			int xy[][4] = {
				{12,36,22,50},      // left
				{150,36,160,50},    // right
				{0,184,59,202},     //AVK_SELECT
				{60,184,118,202},   //AVK_INFO
				{119,184,176,202}   //AVK_CLR
			};
			for( i = 0; i < 5; i ++)
			{
				if( wXPos >= xy[i][0] &&
					wXPos <= xy[i][2] &&
					wYPos >= xy[i][1] &&
					wYPos <= xy[i][3]
				)
				{
					if (i == 0)
					{
						eCode = EVT_KEY;
						wParam = AVK_LEFT;
					}
					else if (i == 1)
					{
						eCode = EVT_KEY;
						wParam = AVK_RIGHT;
					}
					else if (i == 2)
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_SPORTSAPP,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if (i == 3)
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_SPORTSAPP,EVT_USER,AVK_INFO,0);
						return rt;
					}
					else if (i == 4)
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_SPORTSAPP,EVT_USER,AVK_CLR,0);
						return rt;
					}
				}
		    }
	}

#endif    
	switch (eCode)
	{
		case EVT_DIALOG_INIT:
			
			pMe->GobackIdleFlag = FALSE;
			pMe->m_pSportsAppMenu.m_PopMsgBoxBackFlag = SPORT_STATE_PETOMETER;
			pMe->m_pSportsAppMenu.m_nEntryDialogState = ENTRY_PED; 
				/*Rest Record Data */
            /*Entry From Schedule*/
                #ifdef FEATURE_SPORTS_APP
            if(pMe->FromScheduleFlag == TRUE)
            {
				#ifdef FEATRUE_SUPPORT_G_SENSOR
				mmi_g_sensor_process(G_SENSOR_ENABLE | G_SENSOR_PEDOMETER);
				#endif
                DBGPRINTF("Entry From Schedule");
				pMe->m_pSportsAppMenu.m_nPedometerState = PM_RUNNING;
                IANNUNCIATOR_SetField(pMe->m_pIAnn,ANNUN_FIELD_SPORTS, ANNUN_STATE_SPORTS_ON);
                pMe->m_pSportsAppMenu.m_nPedometerState = PM_RUNNING;

#ifdef FEATURE_SPORTS_APP

                MEMSET(&pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData,
                    0,sizeof(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData));

#endif

                DBGPRINTF("PedometerDialogEvent runOrwalk = %d",pSportAppt.runOrwalk);
                if(pSportAppt.runOrwalk == FALSE)
                {
                    pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_bPedRunOrWalkState = FALSE;
                }
                else
                {
                    pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_bPedRunOrWalkState = TRUE;
                }
                 
            }
            #endif
            /*End Entry From Schedule */
			if(pMe->m_pSportsAppMenu.m_nPedometerState == PM_IDLE)
			{
				MEMSET(&pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData,
					0,sizeof(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData));
			}

                    if((pMe->m_pSportsAppMenu.m_nPedometerState == PM_RUNNING)
                        ||(pMe->m_pSportsAppMenu.m_nPedometerState == PM_STOPED))
                        {
                                g_sportsapp_entry_pedometer_flag = TRUE;
                                g_sportsapp_pedometer_bground_flag = FALSE;
                        }

			return TRUE;
		case EVT_DIALOG_START:
		(void) ISHELL_PostEvent( pMe->m_pShell,
				 AEECLSID_SPORTSAPP,
				 EVT_USER_REDRAW,
				 0,
				 0);
			return TRUE;

		case EVT_USER_REDRAW:
			if(pMe->m_pSportsAppMenu.m_nPedometerState == PM_RUNNING)
			{

				pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nTicks = 
					ISHELL_GetTimeMS( pMe->m_pShell);
				Pedometer_Running(pMe);
			}

			if((pMe->runOnBackgrounds == TRUE))
			{
				{	
					int32 total_step = 0;
					pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nStep = ReadPedoCounter();
					total_step = pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nStep;
					SportsApp_SetPedometerSpeeds(pMe,total_step,active_user);
					SportsApp_SetPedometerDistance(pMe,total_step,active_user);
					Pedometer_Redraw(pMe);
				}			
			}
			Pedometer_Redraw(pMe);
			return TRUE;

		case EVT_DIALOG_END:
			g_sportsapp_entry_pedometer_flag = FALSE;
			return TRUE;

		case EVT_KEY:

			if(pMe->m_pSportsAppMenu.m_nPedometerState == PM_IDLE)
			{
				IMENUCTL_SetActive(pMe->m_pSportsAppMenu.m_pSAppMenu, TRUE);
			}
			else
			{
				IMENUCTL_SetActive(pMe->m_pSportsAppMenu.m_pSAppMenu, FALSE);
			}
			
			switch(wParam)
			{
				case AVK_BGRUN:
					if((pMe->runOnBackgrounds == FALSE)
                                    &&((pMe->m_pSportsAppMenu.m_nPedometerState == PM_RUNNING)||
                                        (pMe->m_pSportsAppMenu.m_nPedometerState == PM_STOPED)))
					{
						pMe->runOnBackgrounds = TRUE;
					}


                                if((pMe->m_pSportsAppMenu.m_nPedometerState == PM_RUNNING)
                                    ||(pMe->m_pSportsAppMenu.m_nPedometerState == PM_STOPED))
                                {
                                    /*bground timer show flag*/
					pMe->GobackIdleFlag = TRUE;
					g_sportsapp_pedometer_bground_flag = TRUE;
					ISHELL_CloseApplet(pMe->m_pShell, TRUE);	
#ifdef FEATURE_APP_MUSICPLAYER
                    if(MP3STATUS_RUNONBACKGROUND == GetMp3PlayerStatus())
                    {
                        (void)ISHELL_PostEvent(pMe->m_pShell,
                                               AEECLSID_APP_MUSICPLAYER,
                                               EVT_DRAWMUSICNAME,
                                               0,
                                               0);
                    }
#endif
                                }
                                else
                                {
                                    g_sportsapp_pedometer_bground_flag = FALSE;
                                }
					return TRUE;
				case AVK_CLR:
					switch(pMe->m_pSportsAppMenu.m_nPedometerState)
					{
						case PM_RUNNING:
						case PM_STOPED:
							Pedometer_Stop(pMe);
							#ifdef FEATRUE_SUPPORT_G_SENSOR
							mmi_g_sensor_process(G_SENSOR_PEDOMETER_PAUSE);
							#endif
							Pedometer_ResetConfirm(pMe);
							break;
						
						case PM_IDLE:
							CLOSE_DIALOG(DLGRET_SPORTS)
							return TRUE;
						case PM_REST_CONFIRM:
							pMe->m_pSportsAppMenu.m_nPedometerState = PM_RUNNING;

							#ifdef FEATRUE_SUPPORT_G_SENSOR
							mmi_g_sensor_process(G_SENSOR_PEDOMETER);
							#endif
                                                        /*bground run flag*/
		                                        pMe->GobackIdleFlag = FALSE;
		
							Pedometer_Resume(pMe);
							Pedometer_Redraw(pMe);

						default:
							break;
					}
					break;
					
				case AVK_INFO:
					switch(pMe->m_pSportsAppMenu.m_nPedometerState)
					{
						case PM_IDLE:
							//CLOSE_DIALOG(DLGRET_PEDOMETER_OPTION);
                            pMe->m_pSportsAppMenu.m_nMsgTypeFlag = SPORT_SIG_RUN;
        					if(SportSApp_Get_Shake_OnOrOff(pMe) == FALSE)
        					{
        					#ifdef FEATRUE_SUPPORT_G_SENSOR
        					mmi_g_sensor_process(G_SENSOR_ENABLE | G_SENSOR_PEDOMETER);
        					#endif
        					pMe->m_pSportsAppMenu.m_nPedometerState = PM_RUNNING;
                            IANNUNCIATOR_SetField(pMe->m_pIAnn,ANNUN_FIELD_SPORTS, ANNUN_STATE_SPORTS_ON);
        					CLOSE_DIALOG(DLGRET_PEDOMETER)
        					}
        					else
        					{
        						pMe->m_pSportsAppMenu.m_wMsgResID = IDS_WARNNING_CLOSE;
        						CLOSE_DIALOG(DLGRET_MSGBOX);
        					}							
							return TRUE;
							//break;
						case PM_RUNNING:
							#ifdef FEATRUE_SUPPORT_G_SENSOR
							mmi_g_sensor_process(G_SENSOR_PEDOMETER_PAUSE);
							#endif
							Pedometer_Stop(pMe);
							break;
			
						case PM_STOPED:
							#ifdef FEATRUE_SUPPORT_G_SENSOR
							mmi_g_sensor_process(G_SENSOR_PEDOMETER);
							#endif
							Pedometer_Resume(pMe);
							Pedometer_Redraw(pMe);
							break;
							
						case PM_REST_CONFIRM:
							/*Save PedoMeter Data*/
							#ifdef FEATRUE_SUPPORT_G_SENSOR
							mmi_g_sensor_process(G_SENSOR_PEDOMETER_DISABLE);
							#endif
							Pedometer_Reset(pMe);
							pMe->m_pSportsAppMenu.m_nEntryRecordState = REC_LAST;
							pMe->runOnBackgrounds = FALSE;
							g_sportsapp_pedometer_bground_flag =  FALSE;

                                             g_sportsapp_entry_view_flag =TRUE;
                                                        /*Switch Fm or Music stop*/
                                                        SportSapp_SwitchFmAndMusic(pMe); 
                                                        /*entry From Schedule False flag*/
                                                        pMe->FromScheduleFlag = FALSE;                  
							CLOSE_DIALOG(DLGRET_PEDOMETER_RECORD_VIEW);
							return TRUE;
							
							default:
								break;
					}   
                                        break;
				case AVK_SELECT:
					switch(pMe->m_pSportsAppMenu.m_nPedometerState)
					{
						case PM_IDLE:
							CLOSE_DIALOG(DLGRET_PEDOMETER_OPTION);
							return TRUE;
							//break;
						case PM_RUNNING:
							#ifdef FEATRUE_SUPPORT_G_SENSOR
							mmi_g_sensor_process(G_SENSOR_PEDOMETER_PAUSE);
							#endif
							Pedometer_Stop(pMe);
							break;
			
						case PM_STOPED:
							#ifdef FEATRUE_SUPPORT_G_SENSOR
							mmi_g_sensor_process(G_SENSOR_PEDOMETER);
							#endif
							Pedometer_Resume(pMe);
							Pedometer_Redraw(pMe);
							break;
							
						case PM_REST_CONFIRM:
							/*Save PedoMeter Data*/
							#ifdef FEATRUE_SUPPORT_G_SENSOR
							mmi_g_sensor_process(G_SENSOR_PEDOMETER_DISABLE);
							#endif
							Pedometer_Reset(pMe);
							pMe->m_pSportsAppMenu.m_nEntryRecordState = REC_LAST;
							pMe->runOnBackgrounds = FALSE;
							g_sportsapp_pedometer_bground_flag =  FALSE;

                                            g_sportsapp_entry_view_flag = TRUE;
                                                        /*Switch Fm or Music stop*/
                                                        SportSapp_SwitchFmAndMusic(pMe); 
                                                        /*entry From Schedule False flag*/
                                                        pMe->FromScheduleFlag = FALSE;                  
							CLOSE_DIALOG(DLGRET_PEDOMETER_RECORD_VIEW);
							return TRUE;
							
							default:
								break;
					}
					break;
#ifndef WIN32
					#ifdef FEATRUE_SUPPORT_G_SENSOR
					case AVK_GSENSOR_STEP:
					if(pMe->m_pSportsAppMenu.m_nPedometerState == PM_RUNNING)
					{
							
							int32 total_step = 0;
							pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nStep = ReadPedoCounter();
							total_step = pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nStep;
							DBGPRINTF("PedometerDialog AVK_GSENSOR_STEP total_step = %d",total_step);
							SportsApp_SetPedometerSpeeds(pMe,total_step,active_user);
							SportsApp_SetPedometerDistance(pMe,total_step,active_user);

                            #ifdef FEATURE_SPORTS_APP
                            if(pMe->FromScheduleFlag == TRUE)
                            {
                                int16 data_dist = 0;
                                int16 data_sche_dist = 0;
                                data_dist = (int16)WSTRTOFLOAT(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nDistance);
                                data_sche_dist = WSTRTOFLOAT(pSportAppt.subject);
                                DBGPRINTF("data_dist = %d,data_sche_dist = %d",data_dist,data_sche_dist);
                                
                                if(data_sche_dist <= data_dist)
                                {
                                    /*Save PedoMeter Data*/
                                    Pedometer_Stop(pMe);
                                    #ifdef FEATRUE_SUPPORT_G_SENSOR
                                    mmi_g_sensor_process(G_SENSOR_PEDOMETER_PAUSE);
                                    #endif
                                    Pedometer_ResetConfirm(pMe);
                                    
                                    #ifdef FEATRUE_SUPPORT_G_SENSOR
                                    mmi_g_sensor_process(G_SENSOR_PEDOMETER_DISABLE);
                                    #endif
                                    Pedometer_Reset(pMe);
                                    pMe->m_pSportsAppMenu.m_nEntryRecordState = REC_LAST;
                                    pMe->runOnBackgrounds = FALSE;
                                    g_sportsapp_pedometer_bground_flag =  FALSE;
                            
                                    /*Switch Fm or Music stop*/
                                    SportSapp_SwitchFmAndMusic(pMe); 
                                    /**/
                                    pMe->FromScheduleFlag = FALSE;                  
                                    CLOSE_DIALOG(DLGRET_PEDOMETER_RECORD_VIEW);
                                    return TRUE;                            
                                }
                            }
                            #endif

							Pedometer_Redraw(pMe);
					}
					/**************************************************/
					break;
					#endif
#endif//WIN32
				default:
					break;
			}
			if (IMENUCTL_IsActive(pMe->m_pSportsAppMenu.m_pSAppMenu))
			{
			    	if(IMENUCTL_HandleEvent(pMe->m_pSportsAppMenu.m_pSAppMenu, eCode, wParam, dwParam))
	            		{
					if(IMENUCTL_GetSel(pMe->m_pSportsAppMenu.m_pSAppMenu) == IDS_PED_WALK)
					{
						pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_bPedRunOrWalkState = FALSE;
					}
					else
					{
						pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_bPedRunOrWalkState = TRUE;
					}

	              		return TRUE;
	            		}
			}
			return TRUE;

		case EVT_COMMAND:
			/*Save Pedometer Data*/
			return TRUE;
		default:
			break;
	}
	return FALSE;
}

/*==============================================================================
函数：
       HandlePedometerOptionDialogEvent
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
extern void SportSapp_FmRadioentryPedometer(CSportsMenu *pMe);

void SportSapp_FmRadioentryPedometer(CSportsMenu *pMe)
{
    ISHELL_CancelTimer(pMe->m_pShell,(PFNNOTIFY)SportSapp_FmRadioentryPedometer,(void*)pMe);

    CLOSE_DIALOG(DLGRET_PEDOMETER) 
}
static boolean	HandlePedometerOptionDialogEvent(CSportsMenu *pMe,
	AEEEvent eCode,
	uint16 wParam,
	uint32 dwParam
	)
{
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_PED_OPTION);
    if (pMenu == NULL)
    {
        return FALSE;
    }

	SPORTS_ERR("%x, %x ,%x,HandleMainDialogEvent",eCode,wParam,dwParam);
 
	switch (eCode)
	{
		case EVT_DIALOG_INIT:
			pMe->m_pSportsAppMenu.m_PopMsgBoxBackFlag = SPORT_STATE_PETOMETER_OPTION;
		  IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_PEDOMETER_START, IDS_PEDOMETER_START, NULL, 0);
                  if(FALSE == SportSApp_GetPlayMusicOnOrOff())
                  {
	             IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_PEDOMETER_START_WITH_MUSIC, IDS_PEDOMETER_START_WITH_MUSIC, NULL, 0);
                  }

                  if( FALSE == SportSApp_IsRunOnGround(pMe))
                  {
	             IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_PEDOMETER_START_WITH_FM, IDS_PEDOMETER_START_WITH_FM, NULL, 0);
                  }
	          IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_PEDOMETER_RECORD, IDS_PEDOMETER_RECORD, NULL, 0);
			return TRUE;

		case EVT_DIALOG_START:
			IMENUCTL_SetPopMenuRect(pMenu);
            IMENUCTL_SetOemProperties(pMenu,OEMMP_USE_MENU_STYLE);
            		IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
			IMENUCTL_SetBottomBarType( pMenu, BTBAR_SELECT_BACK);

			(void) ISHELL_PostEvent( pMe->m_pShell,
									 AEECLSID_SPORTSAPP,
									 EVT_USER_REDRAW,
									 0,
									 0);

			return TRUE;

		case EVT_USER_REDRAW:
			(void)IMENUCTL_Redraw(pMenu);
			return TRUE;

		case EVT_DIALOG_END:
			return TRUE;

		case EVT_KEY:
			
			switch(wParam)
			{
				ISHELL_CancelTimer(pMe->m_pShell,(PFNNOTIFY)SportSapp_FmRadioentryPedometer,(void*)pMe);
				case AVK_CLR:
					CLOSE_DIALOG(DLGRET_PEDOMETER)
					return TRUE;
				case AVK_SELECT:
				case AVK_INFO:

				default:
					break;
			}
			return TRUE;

		case EVT_COMMAND:
			//pMe->m_currDlgId = IMENUCTL_GetSel(pMenu);
			pMe->m_currDlgId = wParam;
			switch (wParam)
			{
				case IDS_PEDOMETER_START:
          pMe->m_pSportsAppMenu.m_nMsgTypeFlag = SPORT_SIG_RUN;
					if(SportSApp_Get_Shake_OnOrOff(pMe) == FALSE)
					{
					#ifdef FEATRUE_SUPPORT_G_SENSOR
					mmi_g_sensor_process(G_SENSOR_ENABLE | G_SENSOR_PEDOMETER);
					#endif
							pMe->m_pSportsAppMenu.m_nPedometerState = PM_RUNNING;
                                        IANNUNCIATOR_SetField(pMe->m_pIAnn,ANNUN_FIELD_SPORTS, ANNUN_STATE_SPORTS_ON);
					CLOSE_DIALOG(DLGRET_PEDOMETER)
					}
					else
					{
						pMe->m_pSportsAppMenu.m_wMsgResID = IDS_WARNNING_CLOSE;
						CLOSE_DIALOG(DLGRET_MSGBOX);
					}
					return TRUE;					

				case IDS_PEDOMETER_START_WITH_MUSIC:
                                pMe->m_pSportsAppMenu.m_nMsgTypeFlag = SPORT_RUN_WITH_MUSIC;
					if(SportSApp_Get_Shake_OnOrOff(pMe) == FALSE)
					{
                                            if(SportSApp_IsRunOnGround(pMe) == TRUE)
                                            {
                                                if(SportSApp_StopFmRadio(pMe) == FALSE)
                                                {
                                                    return TRUE;
                                                }
                                            }					
                                            #ifdef FEATRUE_SUPPORT_G_SENSOR
							mmi_g_sensor_process(G_SENSOR_ENABLE | G_SENSOR_PEDOMETER);
                                            #endif
							pMe->m_pSportsAppMenu.m_nPedometerState = PM_RUNNING;
                                            SportSApp_PlayMusicWithSport(pMe);
                                            IANNUNCIATOR_SetField(pMe->m_pIAnn,ANNUN_FIELD_SPORTS, ANNUN_STATE_SPORTS_ON);
							CLOSE_DIALOG(DLGRET_PEDOMETER)						
					}
					else
					{
						pMe->m_pSportsAppMenu.m_wMsgResID = IDS_WARNNING_CLOSE;
						CLOSE_DIALOG(DLGRET_MSGBOX);
					}
					return TRUE;		
					
				case IDS_PEDOMETER_START_WITH_FM:
                            pMe->m_pSportsAppMenu.m_nMsgTypeFlag = SPORT_RUN_WITH_FM;

                            if(SportSApp_Get_Shake_OnOrOff(pMe) == FALSE)
                            {
                                    boolean fmPowerOnorOff = FALSE;
                                OEM_GetConfig(CFGI_HEADSET_PRESENT, &fmPowerOnorOff, sizeof(fmPowerOnorOff));
                                    if(fmPowerOnorOff == FALSE)
                                    {
                                        pMe->m_pSportsAppMenu.m_wMsgResID = IDS_HEADSET_OFF;
                                        CLOSE_DIALOG(DLGRET_MSGBOX)
                                        return TRUE;
                                    }                                    
                                    /*Music Background Pause*/
                                SportSApp_ClosePlayMusic(pMe);
                                    
                                    #ifdef FEATRUE_SUPPORT_G_SENSOR
                                    mmi_g_sensor_process(G_SENSOR_ENABLE | G_SENSOR_PEDOMETER);
                                    #endif
                                    pMe->m_pSportsAppMenu.m_nPedometerState = PM_RUNNING;
                                    /*Stop Music Player*/

                                    /*Start Sport FmRadio */
                                    SportSApp_StartFmRadioEx(pMe);
                                    IANNUNCIATOR_SetField(pMe->m_pIAnn,ANNUN_FIELD_SPORTS, ANNUN_STATE_SPORTS_ON);
                                    ISHELL_SetTimer(pMe->m_pShell,300,(PFNNOTIFY)SportSapp_FmRadioentryPedometer,(void*)pMe);
                            }
                            else
                            {
                                pMe->m_pSportsAppMenu.m_wMsgResID = IDS_WARNNING_CLOSE;
                                CLOSE_DIALOG(DLGRET_MSGBOX);
                            }
                            return TRUE;    
					break;
					
				case IDS_PEDOMETER_RECORD:
					CLOSE_DIALOG(DLGRET_PEDOMETER_RECORD)
					return TRUE;

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
							int16 wXPos = (int16)AEE_GET_X((const char *)dwParam);
							int16 wYPos = (int16)AEE_GET_Y((const char *)dwParam);
							nBarH = GetBottomBarHeight(pMe->m_pDisplay);
							MEMSET(&devinfo, 0, sizeof(devinfo));
							ISHELL_GetDeviceInfo(pMe->m_pShell, &devinfo);
							SETAEERECT(&rc, 0, devinfo.cyScreen-nBarH, devinfo.cxScreen, devinfo.cyScreen);
							if ((wXPos>0)&&(wXPos<devinfo.cxScreen/3)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
							{
								uint16	nEvtCommand = IMENUCTL_GetSel(pMenu);
								boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_SPORTSAPP,EVT_USER,AVK_SELECT,nEvtCommand);
								return rt;
							}
							else if ((wXPos>devinfo.cxScreen/3)&&(wXPos<(devinfo.cxScreen/3)*2)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
							{
								boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_SPORTSAPP,EVT_USER,AVK_INFO,0);
								return rt;
							}
							else if ((wXPos>(devinfo.cxScreen/3)*2)&&(wXPos<devinfo.cxScreen)&&(wYPos>rc.y)&&(wYPos<devinfo.cyScreen))
							{
								boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_SPORTSAPP,EVT_USER,AVK_CLR,0);
								return rt;
							}
						}
						return TRUE;
#endif

		default:
			break;
	}
	return FALSE;
}


/*==============================================================================
函数：
       HandlePedometerOptRecordDialogEvent
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
static boolean	HandlePedometerOptRecordDialogEvent(CSportsMenu *pMe,
	AEEEvent eCode,
	uint16 wParam,
	uint32 dwParam
	)
{
    	PARAM_NOT_REF(dwParam)
    	IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_MENU_REC);
    	if (pMenu == NULL)
    	{
        	return FALSE;
    	}

	SPORTS_ERR("%x, %x ,%x,HandleMainDialogEvent",eCode,wParam,dwParam);
 
	switch (eCode)
	{
		case EVT_DIALOG_INIT:
			    pMe->m_pSportsAppMenu.m_PopMsgBoxBackFlag = SPORT_STATE_PEDOMETER_RECORD;
		    	IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_PED_OPT_LASTREC, IDS_PED_OPT_LASTREC, NULL, 0);
	        	IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_PED_OPT_ACCREC, IDS_PED_OPT_ACCREC, NULL, 0);
	        	IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_PED_OPT_RESET, IDS_PED_OPT_RESET, NULL, 0);
			return TRUE;

		case EVT_DIALOG_START:
			IMENUCTL_SetSel(pMenu, pMe->m_currDlgId);
			IMENUCTL_SetActive(pMenu,TRUE);
            IMENUCTL_SetOemProperties(pMenu,OEMMP_USE_MENU_STYLE);
            		IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
			IMENUCTL_SetBottomBarType( pMenu, BTBAR_SELECT_BACK);

			(void) ISHELL_PostEvent( pMe->m_pShell,
									 AEECLSID_SPORTSAPP,
									 EVT_USER_REDRAW,
									 0,
									 0);

			return TRUE;

		case EVT_USER_REDRAW:
			(void)IMENUCTL_Redraw(pMenu);
			return TRUE;

		case EVT_DIALOG_END:
			return TRUE;

		case EVT_KEY:
			switch(wParam)
			{
				case AVK_CLR:
					switch(pMe->m_pSportsAppMenu.m_nEntryDialogState)
					{
						case ENTRY_PED:
                                            if(pMe->m_pSportsAppMenu.m_nEntryRecordState == REC_REST)
                                            {
                                                pMe->m_pSportsAppMenu.m_nEntryRecordState =REC_NULL;
                                                CLOSE_DIALOG(DLGRET_PEDOMETER_RECORD);
                                            }
                                            else
                                            {
							CLOSE_DIALOG(DLGRET_PEDOMETER);
                                            }
							break;
						case ENTRY_PERS:
                                            if(pMe->m_pSportsAppMenu.m_nEntryRecordState == REC_REST)
                                            {
                                                pMe->m_pSportsAppMenu.m_nEntryRecordState =REC_NULL;
                                                CLOSE_DIALOG(DLGRET_PEDOMETER_RECORD);
                                            }
                                            else
                                            {
							CLOSE_DIALOG(DLGRET_PERSONAL);
                                            }
							break;
						default:
							break;
					}
					return TRUE;
				case AVK_SELECT:
				case AVK_INFO:
					//Clear Record Data Here
					switch(pMe->m_pSportsAppMenu.m_nEntryDialogState)
					{
						case ENTRY_PED:
					                SportsApp_ClearAllCurrentActiveData(pMe);
                            pMe->m_pSportsAppMenu.m_wMsgResID = IDS_DONE;
				            CLOSE_DIALOG(DLGRET_MSGBOX);
							break;
						case ENTRY_PERS:
							SportsApp_ClearAllCurrentActiveData(pMe);
                            pMe->m_pSportsAppMenu.m_wMsgResID = IDS_DONE;
				            CLOSE_DIALOG(DLGRET_MSGBOX);                            
							break;
						default:
							break;
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
				/*Last Record*/
				case IDS_PED_OPT_LASTREC:
					pMe->m_pSportsAppMenu.m_nEntryRecordState = REC_LAST;
					CLOSE_DIALOG(DLGRET_PEDOMETER_RECORD_VIEW);
					return TRUE;					

				/*accumulated record*/	
				case IDS_PED_OPT_ACCREC:

					pMe->m_pSportsAppMenu.m_nEntryRecordState = REC_ACC;
					CLOSE_DIALOG(DLGRET_PEDOMETER_RECORD_VIEW);
					break;
				/*Clear Record Data*/
				case IDS_PED_OPT_RESET:
                                pMe->m_pSportsAppMenu.m_nEntryRecordState = REC_REST;
					IMENUCTL_SetActive(pMenu,FALSE);
					Pedometer_RecordRestConfirm(pMe);
					break;
				default:
                    ASSERT_NOT_REACHABLE;
						
			}
			return TRUE;
		default:
			break;
	}
	return FALSE;
}

/*==============================================================================
函数：
       HandlePedometerRecViewDialogEvent
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
static boolean	HandlePedometerRecViewDialogEvent(CSportsMenu *pMe,
	AEEEvent eCode,
	uint16 wParam,
	uint32 dwParam
	)
{

    	PARAM_NOT_REF(dwParam)
	
    	IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_MENU_REC_VIEW);
	
    	if (pMenu == NULL)
    	{
        	return FALSE;
    	}
		
	switch (eCode)
	{
		case EVT_DIALOG_INIT:
			
			if(pMe->m_pSportsAppMenu.m_nEntryRecordState == REC_LAST)
			{
				SportsApp_DisplayLastRecordMenu(pMe,pMenu);
			}
			else
			{
				SportsApp_DisplayAccRecordMenu(pMe,pMenu);
			}
			return TRUE;

		case EVT_DIALOG_START:
			(void) ISHELL_PostEvent( pMe->m_pShell,
									 AEECLSID_SPORTSAPP,
									 EVT_USER_REDRAW,
									 0,
									 0);

			return TRUE;

		case EVT_USER_REDRAW:
			(void)IMENUCTL_Redraw(pMenu);
			return TRUE;

		case EVT_DIALOG_END:

			return TRUE;

		case EVT_KEY:
			
			switch(wParam)
			{
				case AVK_CLR:
					switch(pMe->m_pSportsAppMenu.m_nEntryDialogState)
					{
						case ENTRY_PED:
                                            if(g_sportsapp_entry_view_flag == TRUE)
                                            {
                                                g_sportsapp_entry_view_flag = FALSE;
                                                CLOSE_DIALOG(DLGRET_PEDOMETER)

                                            }
                                            else
                                            {
							CLOSE_DIALOG(DLGRET_PEDOMETER_RECORD)
                                            }
							
							return TRUE;
						case ENTRY_PERS:

							CLOSE_DIALOG(DLGRET_PEDOMETER_RECORD)
							return TRUE;
						default:
							
							break;				
					}
					return TRUE;
					break;
				case AVK_SELECT:
				case AVK_INFO:
					CLOSE_DIALOG(DLGRET_PEDOMETER)
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
函数：
       HandlePersonalDialogEvent
说明：
       IDD_DIALOG_MAIN对话框事件处理函数

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
static boolean  HandlePersonalDialogEvent(CSportsMenu *pMe,
    AEEEvent eCode,
    uint16 wParam,
    uint32 dwParam
)
{
	//ProFileUser active_user;
	uint16 m_wItemID;
        uint16 bytNewData;
	PARAM_NOT_REF(dwParam)
	IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_MENU_PERSONAL);
	
	//active_user = SportsApp_GetCurrentActiveUser(pMe);
	if (pMenu == NULL)
	{
	    return FALSE;
	}
	SPORTS_ERR("%x, %x ,%x,HandleMainDialogEvent",eCode,wParam,dwParam);
	
	pMe->m_pSportsAppMenu.m_nEntryDialogState = ENTRY_PERS; //Init Entry_PERS
 	pMe->m_pSportsAppMenu.m_nUnitDataEditFlag = IDS_EDIT_NAME;

	switch (eCode)
	{
		case EVT_DIALOG_INIT:
			SportSApp_LoadSportData(pMe);	
			IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_PERS_ONE, IDS_PERS_ONE, NULL, 0);
	      		IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_PERS_TWO, IDS_PERS_TWO, NULL, 0);
	      		IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_PERS_THREE, IDS_PERS_THREE, NULL, 0);	
	        	return TRUE;

	    case EVT_DIALOG_START:
			
	        	IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_TEXT_ALIGN_LEFT_ICON_ALIGN_RIGHT);
	        	IMENUCTL_SetBottomBarType(pMenu,BTBAR_OPTION_BACK);
                        IMENUCTL_SetOemProperties( pMenu , OEMMP_DISTINGUISH_INFOKEY_SELECTKEY|OEMMP_USE_MENU_STYLE);
			/*Set Radio Image and Sel it*/
			switch(pMe->m_pSportsAppMenu.m_nSelProFile)
			{
				case USER_ONE:
					m_wItemID = IDS_PERS_ONE;
					break;
					
				case USER_TWO:
					m_wItemID = IDS_PERS_TWO;
					break;
					
				case USER_THREE:
					m_wItemID = IDS_PERS_THREE;
					break;
					
				default:
					
					break;
			}
				SportsApp_SetPersonalItemName(pMe,pMenu);
				
			      	InitMenuIcons(pMenu);
                		SetMenuIcon(pMenu, m_wItemID, TRUE);
				IMENUCTL_SetSel(pMenu, m_wItemID);	

	        	(void) ISHELL_PostEvent( pMe->m_pShell,
	                                 AEECLSID_SPORTSAPP,
	                                 EVT_USER_REDRAW,
	                                 0,
	                                 0);

	        return TRUE;

	    case EVT_USER_REDRAW:

	        (void)IMENUCTL_Redraw(pMenu);
	        return TRUE;

	    case EVT_DIALOG_END:
	        return TRUE;

	    case EVT_KEY:
	        switch(wParam)
	        {
	            case AVK_CLR:
	                CLOSE_DIALOG(DLGRET_SPORTS)  
	                return TRUE;
                case AVK_INFO:
                    pMe->m_currDlgId = IMENUCTL_GetSel(pMenu);
                    pMe->m_pSportsAppMenu.m_nTempSelProFile = pMe->m_currDlgId - IDS_PERS_ONE;
					pMe->m_pSportsAppMenu.m_nSelProFile = pMe->m_pSportsAppMenu.m_nTempSelProFile;
					bytNewData = pMe->m_pSportsAppMenu.m_nSelProFile;
				        (void)ICONFIG_SetItem(pMe->m_pConfig,
                          					CFGI_SPORT_FLAG,
                          					&bytNewData, sizeof(bytNewData));

					CLOSE_DIALOG(DLGRET_PERSONAL);                    
                    return TRUE;
	            default:
	                break;
	        }
	        return TRUE;

	    case EVT_COMMAND:
	        pMe->m_currDlgId = IMENUCTL_GetSel(pMenu);
	        switch (wParam)
	        {
	            	case IDS_PERS_ONE: 
				pMe->m_pSportsAppMenu.m_nTempSelProFile = USER_ONE;
				CLOSE_DIALOG(DLGRET_PERSONAL_OPTION)
	                	return TRUE;

	           	case IDS_PERS_TWO: 
				pMe->m_pSportsAppMenu.m_nTempSelProFile = USER_TWO;
				CLOSE_DIALOG(DLGRET_PERSONAL_OPTION)
	                	return TRUE;

	            	case IDS_PERS_THREE:      
				pMe->m_pSportsAppMenu.m_nTempSelProFile = USER_THREE;
				CLOSE_DIALOG(DLGRET_PERSONAL_OPTION)
	                	return TRUE;

	            	default:
	                	ASSERT_NOT_REACHABLE;
                        break;
	        }
					
	        return TRUE;

	    default:
	        break;
	}
    return FALSE;
} // HandleMainDialogEvent

/*==============================================================================
函数：
       HandlePersonalOptionDialogEvent
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
static boolean	HandlePersonalOptionDialogEvent(CSportsMenu *pMe,
	AEEEvent eCode,
	uint16 wParam,
	uint32 dwParam
	)
{
	uint16 bytNewData;
    PARAM_NOT_REF(dwParam)
    IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_PERS_OPTION);
    if (pMenu == NULL)
    {
        return FALSE;
    }

	SPORTS_ERR("%x, %x ,%x,HandleMainDialogEvent",eCode,wParam,dwParam);
 
	switch (eCode)
	{
		case EVT_DIALOG_INIT:
                        IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_PERS_OPT_SELECT, IDS_PERS_OPT_SELECT, NULL, 0);
                        IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_PERS_OPT_EDIT, IDS_PERS_OPT_EDIT, NULL, 0);
	        	IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_PERS_OPT_RECORD, IDS_PERS_OPT_RECORD, NULL, 0);
			return TRUE;

		case EVT_DIALOG_START:
			IMENUCTL_SetPopMenuRect(pMenu);
            IMENUCTL_SetOemProperties(pMenu,OEMMP_USE_MENU_STYLE);
            		IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
			IMENUCTL_SetBottomBarType( pMenu, BTBAR_SELECT_BACK);

			(void) ISHELL_PostEvent( pMe->m_pShell,
									 AEECLSID_SPORTSAPP,
									 EVT_USER_REDRAW,
									 0,
									 0);

			return TRUE;

		case EVT_USER_REDRAW:
			(void)IMENUCTL_Redraw(pMenu);
			return TRUE;

		case EVT_DIALOG_END:
			return TRUE;

		case EVT_KEY:
			
			switch(wParam)
			{
				case AVK_CLR:
					CLOSE_DIALOG(DLGRET_PERSONAL)
					return TRUE;
				case AVK_SELECT:
				case AVK_INFO:

				default:
					break;
			}
			return TRUE;

		case EVT_COMMAND:
			//pMe->m_currDlgId = IMENUCTL_GetSel(pMenu);
			pMe->m_currDlgId = wParam;
			switch (wParam)
			{
				if(pMe->m_pSportsAppMenu.m_nSportSAppData[pMe->m_pSportsAppMenu.m_nTempSelProFile].m_nSportUintData.m_nWalkstep == 0)
				{
					pMe->m_pSportsAppMenu.m_nSportSAppData[pMe->m_pSportsAppMenu.m_nTempSelProFile].m_nSportUintData.m_nWalkstep = 80;
					
				}

				if(pMe->m_pSportsAppMenu.m_nSportSAppData[pMe->m_pSportsAppMenu.m_nTempSelProFile].m_nSportUintData.m_nRunstep == 0)
				{
					pMe->m_pSportsAppMenu.m_nSportSAppData[pMe->m_pSportsAppMenu.m_nTempSelProFile].m_nSportUintData.m_nRunstep = 80;
				}
				case IDS_PERS_OPT_EDIT:
					CLOSE_DIALOG(DLGRET_PERSONAL_EDIT);
					return TRUE;
					
				case IDS_PERS_OPT_SELECT:
					pMe->m_pSportsAppMenu.m_nSelProFile = pMe->m_pSportsAppMenu.m_nTempSelProFile;
					bytNewData = pMe->m_pSportsAppMenu.m_nSelProFile;
				        (void)ICONFIG_SetItem(pMe->m_pConfig,
                          					CFGI_SPORT_FLAG,
                          					&bytNewData, sizeof(bytNewData));
					
					CLOSE_DIALOG(DLGRET_PERSONAL);
					break;
					
				case IDS_PERS_OPT_RECORD:
					CLOSE_DIALOG(DLGRET_PEDOMETER_RECORD);
					break;
					
				default:
                    		ASSERT_NOT_REACHABLE;
						
			}
			return TRUE;
		default:
			break;
	}
	return FALSE;
}

/*==============================================================================
函数：
       HandlePedometerRecViewDialogEvent
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
static boolean	HandlePersonalEditDialogEvent(CSportsMenu *pMe,
	AEEEvent eCode,
	uint16 wParam,
	uint32 dwParam
	)
{
	uint32   dwMask;
	ProFileUser active_user; 
	uint16 buffer_len;
        IMenuCtl *gRoupList; 
	AECHAR *pBuffer = NULL;
	AECHAR text[20] = {0};
        AECHAR text_temp[20] = {0};
    
	char fmt_str[30] = {0};
        AECHAR fmt_aestr_temp[20] = {0};
	AECHAR text_str[20] = {0};
        AECHAR text_str_gen[20] = {0};
	
    	PARAM_NOT_REF(dwParam)
	
    	IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_MENU_EDIT);
/*add text control */
	ITextCtl  *pText = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_TEXT_PERS); 
/*add Genor control*/
        gRoupList = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg, IDC_LIST_EDIT);
/*add end Genor control*/
	if(pMe->m_pSportsAppMenu.m_nTempSelProFile != USER_NONE)
	{
		pMe->m_pSportsAppMenu.m_bTempEntryView = TRUE;
	}
	else
	{
		pMe->m_pSportsAppMenu.m_bTempEntryView = FALSE;
	}

	/*********************************/
	active_user = SportsApp_GetCurrentActiveUser(pMe);
	
    	if (pMenu == NULL)
    	{
        	return FALSE;
    	}
		
	switch (eCode)
	{
		case EVT_DIALOG_INIT:
			/*Show Text Menu*/
	            	IMENUCTL_SetActive(pMenu,TRUE);
	            	ITEXTCTL_SetActive(pText, FALSE);
	            	dwMask = IDIALOG_GetProperties(pMe->m_pActiveDlg);
	            	dwMask |= DLG_NOT_SET_FOCUS_AUTO;
	            	IDIALOG_SetProperties(pMe->m_pActiveDlg, dwMask);
	            	IDIALOG_SetFocus(pMe->m_pActiveDlg, IDC_MENU_EDIT);
			pMe->m_pSportsAppMenu.m_nInputMode = EDIT_MENU_MODE;
			pMe->m_pSportsAppMenu.m_nUnitDataEditFlag = IDS_EDIT_NAME;
			pMe->m_pSportsAppMenu.m_PopMsgBoxBackFlag = SPORT_STATE_PERSONAL_EDIT;
            
#if 0            
			IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_EDIT_NAME, IDS_EDIT_NAME, NULL, 0);
	      		IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_EDIT_HIGHT, IDS_EDIT_HIGHT, NULL, 0);
	      		IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_EDIT_WEIGHT, IDS_EDIT_WEIGHT, NULL, 0);
	      		IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_WALK_STEP, IDS_WALK_STEP, NULL, 0);
	      		IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_RUN_STEP, IDS_RUN_STEP, NULL, 0);

			IMENUCTL_AddItem(pMenu,AEE_APPSSPORTSMENU_RES_FILE,IDS_EDIT_BRITHDAY,IDS_EDIT_BRITHDAY,NULL,0);
			IMENUCTL_AddItem(pMenu,AEE_APPSSPORTSMENU_RES_FILE,IDS_EDIT_GENDER,IDS_EDIT_GENDER,NULL,0);

#endif
            CSportSapp_BuildAddMenuMenu(pMe,pMenu);

			IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL);
			IMENUCTL_SetOemProperties(pMenu, OEMMP_DISTINGUISH_INFOKEY_SELECTKEY|OEMMP_USE_MENU_STYLE);
			IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_OK_BACK);

			if(WSTRLEN(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_pName) != 0)
			{
				pBuffer = pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_pName;
                		IMENUCTL_SetItemText(pMenu, IDS_EDIT_NAME, NULL, 0,pBuffer);
			}

			{
				MEMSET(text,0,sizeof(text));
				MEMSET(fmt_str,0,sizeof(fmt_str));
				MEMSET(text_str,0,sizeof(text_str));
				ISHELL_LoadResString(pMe->m_pShell,AEE_APPSSPORTSMENU_RES_FILE,IDS_EDIT_HIGHT,
					text,sizeof(text));
				SNPRINTF(fmt_str,sizeof(fmt_str),":        %d",
					pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nHeight);
				STRTOWSTR(fmt_str,text_str,sizeof(text_str));
				buffer_len = WSTRLEN(text);
				WSTRCAT(text + buffer_len,text_str);
                		IMENUCTL_SetItemText(pMenu, IDS_EDIT_HIGHT, NULL, 0,text);
			}

			{	
				MEMSET(text,0,sizeof(text));
				MEMSET(fmt_str,0,sizeof(fmt_str));
				MEMSET(text_str,0,sizeof(text_str));
				ISHELL_LoadResString(pMe->m_pShell,AEE_APPSSPORTSMENU_RES_FILE,IDS_EDIT_WEIGHT,
					text,sizeof(text));
				SNPRINTF(fmt_str,sizeof(fmt_str),":        %d",
					pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nWeight);
				STRTOWSTR(fmt_str,text_str,sizeof(text_str));
				buffer_len = WSTRLEN(text);
				WSTRCAT(text + buffer_len,text_str);
                		IMENUCTL_SetItemText(pMenu, IDS_EDIT_WEIGHT, NULL, 0,text);
			}

			{	
				MEMSET(text,0,sizeof(text));
				MEMSET(fmt_str,0,sizeof(fmt_str));
				MEMSET(text_str,0,sizeof(text_str));

				ISHELL_LoadResString(pMe->m_pShell,AEE_APPSSPORTSMENU_RES_FILE,IDS_WALK_STEP,
					text,sizeof(text));
				SNPRINTF(fmt_str,sizeof(fmt_str),":   %d",
					pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nWalkstep);
				STRTOWSTR(fmt_str,text_str,sizeof(text_str));
				buffer_len = WSTRLEN(text);
				WSTRCAT(text + buffer_len,text_str);
                		IMENUCTL_SetItemText(pMenu, IDS_WALK_STEP, NULL, 0,text);
			}

			{	
				MEMSET(text,0,sizeof(text));
				MEMSET(fmt_str,0,sizeof(fmt_str));
				MEMSET(text_str,0,sizeof(text_str));

				ISHELL_LoadResString(pMe->m_pShell,AEE_APPSSPORTSMENU_RES_FILE,IDS_RUN_STEP,
					text,sizeof(text));
				SNPRINTF(fmt_str,sizeof(fmt_str),":     %d",
					pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nRunstep);
				STRTOWSTR(fmt_str,text_str,sizeof(text_str));
				buffer_len = WSTRLEN(text);
				WSTRCAT(text + buffer_len,text_str);
                		IMENUCTL_SetItemText(pMenu, IDS_RUN_STEP, NULL, 0,text);

			}

			{	
				MEMSET(text,0,sizeof(text));
				MEMSET(fmt_str,0,sizeof(fmt_str));
				MEMSET(text_str,0,sizeof(text_str));

				ISHELL_LoadResString(pMe->m_pShell,AEE_APPSSPORTSMENU_RES_FILE,IDS_EDIT_BRITHDAY,
					text,sizeof(text));
				SNPRINTF(fmt_str,sizeof(fmt_str),":            %d",
					pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nBrithday);
				STRTOWSTR(fmt_str,text_str,sizeof(text_str));
				buffer_len = WSTRLEN(text);
				WSTRCAT(text + buffer_len,text_str);
			    	IMENUCTL_SetItemText(pMenu, IDS_EDIT_BRITHDAY, NULL, 0,text);

			}

			{	
				MEMSET(text,0,sizeof(text));
                                MEMSET(text_temp,0,sizeof(text_temp));
                                MEMSET(fmt_aestr_temp,0,sizeof(fmt_aestr_temp));
				MEMSET(fmt_str,0,sizeof(fmt_str));
				MEMSET(text_str,0,sizeof(text_str));
                         MEMSET(text_str_gen,0,sizeof(text_str_gen));
				ISHELL_LoadResString(pMe->m_pShell,AEE_APPSSPORTSMENU_RES_FILE,IDS_EDIT_GENDER,text,sizeof(text));
                
				if(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_bGender == FALSE)
				{
				    ISHELL_LoadResString(pMe->m_pShell,AEE_APPSSPORTSMENU_RES_FILE,IDS_EDIT_MALE,text_temp,sizeof(text_temp));
					//SNPRINTF(fmt_str,sizeof(fmt_str),":      %S","Male");
                    
				}
				else
				{
				    ISHELL_LoadResString(pMe->m_pShell,AEE_APPSSPORTSMENU_RES_FILE,IDS_EDIT_FEMALE,text_temp,sizeof(text_temp));					
				}

				
				SNPRINTF(fmt_str,sizeof(fmt_str),":      ");
                                STRTOWSTR(fmt_str,fmt_aestr_temp,sizeof(fmt_aestr_temp));
                                WSTRCAT(text_str,fmt_aestr_temp);
				WSTRCAT(text_str,text_temp);
				buffer_len = WSTRLEN(text);
				WSTRCAT(text + buffer_len,text_str);
                
					IMENUCTL_SetItemText(pMenu, IDS_EDIT_GENDER, NULL, 0,text);
			}

			return TRUE;

		case EVT_DIALOG_START:
			IMENUCTL_SetSel(pMenu, pMe->m_pSportsAppMenu.m_nUnitDataEditFlag);
			(void) ISHELL_PostEvent( pMe->m_pShell,
						 AEECLSID_SPORTSAPP,
						 EVT_USER_REDRAW,
						 0,
						 0);

			return TRUE;

		case EVT_USER_REDRAW:
			(void)IMENUCTL_Redraw(pMenu);
			IDISPLAY_Update(pMe->m_pDisplay);  
			return TRUE;

		case EVT_DIALOG_END:
			return TRUE;

		case EVT_KEY:
			
			switch(wParam)
			{
				case AVK_CLR:
					/*********************************/
					if(pMe->m_pSportsAppMenu.m_nTempSelProFile != USER_NONE)
					{
						pMe->m_pSportsAppMenu.m_bTempEntryView = FALSE;
						pMe->m_pSportsAppMenu.m_nTempSelProFile = USER_NONE;
					}
					/*********************************/
					CLOSE_DIALOG(DLGRET_PERSONAL);
					return TRUE;
				case AVK_SELECT:
                    
                                 if( LOCAL_NUMBER_INPUT == pMe->m_pSportsAppMenu.m_nInputMode)
                                 {
                                      SportSapp_SaveCurrentItemText(pMe,pText);
                                      SportSapp_SetMenuText(pMe,pMenu,pMe->m_pSportsAppMenu.m_nUnitDataEditFlag);
                                      ITEXTCTL_SetActive(pText, FALSE);
                                      IMENUCTL_SetActive(pMenu, TRUE);
                                      IMENUCTL_SetSel(pMenu,pMe->m_pSportsAppMenu.m_nUnitDataEditFlag);
                                 }
					return TRUE;
				case AVK_INFO:
					/*Save UintData*/
                                SportSapp_SaveCurrentItemText(pMe,pText);
                                
					if (WSTRLEN(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_pName) == 0)
					{

						pMe->m_pSportsAppMenu.m_wMsgResID = IDS_NEEDNAME;
						CLOSE_DIALOG(DLGRET_MSGBOX);

						return TRUE;
					}
					else if ((pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nWeight > 250)
						||(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nWeight < 40))
					{
						pMe->m_pSportsAppMenu.m_wMsgResID = IDS_OUTOFWEIGHT;
						CLOSE_DIALOG(DLGRET_MSGBOX);

						return TRUE;
					}
					else if (pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nHeight >250
						||pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nHeight < 120)
					{
						pMe->m_pSportsAppMenu.m_wMsgResID = IDS_OUTOFHEIGHT;

						CLOSE_DIALOG(DLGRET_MSGBOX);

						return TRUE;
					}
					else if (pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nWalkstep >100
						||pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nWalkstep < 50)
					{
						pMe->m_pSportsAppMenu.m_wMsgResID = IDS_OUTOFWALK;

						CLOSE_DIALOG(DLGRET_MSGBOX);

						return TRUE;
					}
					 else if (pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nRunstep >150
						||pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nRunstep < 50)
					{
						pMe->m_pSportsAppMenu.m_wMsgResID = IDS_OUTOFRUN;
						CLOSE_DIALOG(DLGRET_MSGBOX);

						return TRUE;
					}
					  else if (pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nBrithday >90
						 ||pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nBrithday < 1)
					 {
						 pMe->m_pSportsAppMenu.m_wMsgResID = IDS_WARNNING_AGE_NULL;
						 CLOSE_DIALOG(DLGRET_MSGBOX);
					 
						 return TRUE;
					 }
                                 else
                                 {
                                        SportSApp_SaveSportData(pMe);	
                            pMe->m_pSportsAppMenu.m_wMsgResID = IDS_DONE;
		                    //CLOSE_DIALOG(DLGRET_PERSONAL);
		                    CLOSE_DIALOG(DLGRET_MSGBOX);
                                        return TRUE;
                                 }
					return TRUE;

				default:
					break;
			}
			return TRUE;

		case EVT_COMMAND:
			pMe->m_pSportsAppMenu.m_nUnitDataEditFlag = wParam;
			switch(wParam)
			{
				case IDS_EDIT_NAME:
					pMe->m_pSportsAppMenu.m_nUnitDataFlag = UNIT_DATA_NAME;
                                pMe->m_pSportsAppMenu.m_nInputMode = OPT_TEXT_INPUT;
					CLOSE_DIALOG(DLGRET_EDIT);
					break;		
				case IDS_EDIT_HIGHT:
					pMe->m_pSportsAppMenu.m_nUnitDataFlag = UNIT_DATA_HEIGHT;
					pMe->m_pSportsAppMenu.m_nInputMode = LOCAL_NUMBER_INPUT;
					SportsApp_ShowEditItem(pMe,pMenu,pText);
					break;
					
				case IDS_EDIT_WEIGHT:
					pMe->m_pSportsAppMenu.m_nUnitDataFlag = UNIT_DATA_WEIGHT;
					pMe->m_pSportsAppMenu.m_nInputMode = LOCAL_NUMBER_INPUT;
					SportsApp_ShowEditItem(pMe,pMenu,pText);
					break;
					
				case IDS_WALK_STEP:
					pMe->m_pSportsAppMenu.m_nUnitDataFlag = UNIT_DATA_WALK_STEP;
					pMe->m_pSportsAppMenu.m_nInputMode = LOCAL_NUMBER_INPUT;
					SportsApp_ShowEditItem(pMe,pMenu,pText);			
					break;
					
				case IDS_RUN_STEP:
					pMe->m_pSportsAppMenu.m_nUnitDataFlag = UNIT_DATA_RUN_STEP;
					pMe->m_pSportsAppMenu.m_nInputMode = LOCAL_NUMBER_INPUT;
					SportsApp_ShowEditItem(pMe,pMenu,pText);
					break;					
				case IDS_EDIT_BRITHDAY:
					pMe->m_pSportsAppMenu.m_nUnitDataFlag = UINT_DATA_BRITHDAY;
					pMe->m_pSportsAppMenu.m_nInputMode = LOCAL_NUMBER_INPUT;
					SportsApp_ShowEditItem(pMe,pMenu,pText);
					break;
				case IDS_EDIT_GENDER:
                                pMe->m_pSportsAppMenu.m_nInputMode = EDIT_GROUP;
                                IMENUCTL_SetActive(gRoupList, TRUE);
                                IMENUCTL_SetActive(pMenu, FALSE);
                                SportSApp_BuildGroupList(pMe,gRoupList);
                                SportSapp_ShowGroupSelect(pMe,pMenu,gRoupList);				
					break;
                    
                          case IDS_GENDER_MALE:

                          case IDS_EDIT_FEMALE:
                                 if(pMe->m_pSportsAppMenu.m_nInputMode == EDIT_GROUP)
                                 {
                                      {   
                                              MEMSET(text,0,sizeof(text));
                                              MEMSET(text_temp,0,sizeof(text_temp));
                                              MEMSET(fmt_aestr_temp,0,sizeof(fmt_aestr_temp));
                                              MEMSET(fmt_str,0,sizeof(fmt_str));
                                              MEMSET(text_str,0,sizeof(text_str));

                                              ISHELL_LoadResString(pMe->m_pShell,AEE_APPSSPORTSMENU_RES_FILE,IDS_EDIT_GENDER,text,sizeof(text));
                                              
                                              if(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_bGender == FALSE)
                                              {
                                                  ISHELL_LoadResString(pMe->m_pShell,AEE_APPSSPORTSMENU_RES_FILE,IDS_EDIT_MALE,text_temp,sizeof(text_temp));

                                              }
                                              else
                                              {
                                                  ISHELL_LoadResString(pMe->m_pShell,AEE_APPSSPORTSMENU_RES_FILE,IDS_EDIT_FEMALE,text_temp,sizeof(text_temp));                                     
                                              }

                          
                          SNPRINTF(fmt_str,sizeof(fmt_str),":      ");
                          STRTOWSTR(fmt_str,fmt_aestr_temp,sizeof(fmt_aestr_temp));
                          WSTRCAT(text_str,fmt_aestr_temp);
                          WSTRCAT(text_str,text_temp);
                                              buffer_len = WSTRLEN(text);
                                              WSTRCAT(text + buffer_len,text_str);
                                  
                                                  IMENUCTL_SetItemText(pMenu, IDS_EDIT_GENDER, NULL, 0,text);
                                     }
                                        IMENUCTL_SetActive(gRoupList, FALSE);
                                        IMENUCTL_SetActive(pMenu, TRUE);
                                 }                                
                                break;
					
			}
                    default:
                        break;                    
			return TRUE;
		case EVT_CTL_SEL_CHANGED :
                          pMe->m_pSportsAppMenu.m_nUnitDataEditPrivFlag = pMe->m_pSportsAppMenu.m_nUnitDataEditFlag;
				 pMe->m_pSportsAppMenu.m_nUnitDataEditFlag = wParam;

                          switch(wParam)
                          {
                          case IDS_EDIT_NAME:
                            pMe->m_pSportsAppMenu.m_nInputMode = OPT_TEXT_INPUT;
                            pMe->m_pSportsAppMenu.m_nUnitDataFlag = UNIT_DATA_NAME;
                            break;
                            
				case IDS_EDIT_HIGHT:
					pMe->m_pSportsAppMenu.m_nUnitDataFlag = UNIT_DATA_HEIGHT;
					pMe->m_pSportsAppMenu.m_nInputMode = LOCAL_NUMBER_INPUT;
					SportsApp_ShowEditItem(pMe,pMenu,pText);
					break;
					
				case IDS_EDIT_WEIGHT:
					pMe->m_pSportsAppMenu.m_nUnitDataFlag = UNIT_DATA_WEIGHT;
					pMe->m_pSportsAppMenu.m_nInputMode = LOCAL_NUMBER_INPUT;
					SportsApp_ShowEditItem(pMe,pMenu,pText);
					break;
					
				case IDS_WALK_STEP:
					pMe->m_pSportsAppMenu.m_nUnitDataFlag = UNIT_DATA_WALK_STEP;
					pMe->m_pSportsAppMenu.m_nInputMode = LOCAL_NUMBER_INPUT;
					SportsApp_ShowEditItem(pMe,pMenu,pText);			
					break;
					
				case IDS_RUN_STEP:
					pMe->m_pSportsAppMenu.m_nUnitDataFlag = UNIT_DATA_RUN_STEP;
					pMe->m_pSportsAppMenu.m_nInputMode = LOCAL_NUMBER_INPUT;
					SportsApp_ShowEditItem(pMe,pMenu,pText);
					break;
                    
				case IDS_EDIT_BRITHDAY:
					pMe->m_pSportsAppMenu.m_nUnitDataFlag = UINT_DATA_BRITHDAY;
					pMe->m_pSportsAppMenu.m_nInputMode = LOCAL_NUMBER_INPUT;
					SportsApp_ShowEditItem(pMe,pMenu,pText);
					break;
                    
                          case IDS_EDIT_GENDER:
                                pMe->m_pSportsAppMenu.m_nInputMode = EDIT_GROUP;
                                IMENUCTL_SetActive(gRoupList, TRUE);
                                IMENUCTL_SetActive(pMenu, FALSE);
                                SportSApp_BuildGroupList(pMe,gRoupList);
                                SportSapp_ShowGroupSelect(pMe,pMenu,gRoupList);
                                break;
                                
                           default:
                            
                                break;
			}
            			return TRUE;

		case EVT_KEY_RELEASE:
			if(pMe->m_pSportsAppMenu.m_nInputMode == LOCAL_NUMBER_INPUT)
			{
				if ( WSTRLEN(ITEXTCTL_GetTextPtr(pText)) >= 0)
				{
					SPORTS_MENU_DRAW_BOTTOMBAR(BTBAR_SAVE_DELETE);
				}
				else
				{
					SPORTS_MENU_DRAW_BOTTOMBAR(BTBAR_OK_BACK);			  
				}
			}
                   else if(pMe->m_pSportsAppMenu.m_nInputMode == EDIT_GROUP)
                   {
                        SPORTS_MENU_DRAW_BOTTOMBAR(BTBAR_SELECT_OK_BACK);
                   }
                   else
                   {
                        SPORTS_MENU_DRAW_BOTTOMBAR(BTBAR_SELECT_OK_BACK);
                   }
			break;
		case EVT_KEY_PRESS:
			switch(wParam)
			{
				case AVK_UP:
                          if(pMe->m_pSportsAppMenu.m_nInputMode == EDIT_GROUP)
                          {
                              {   
                                              MEMSET(text,0,sizeof(text));
                                              MEMSET(text_temp,0,sizeof(text_temp));
                                              MEMSET(fmt_aestr_temp,0,sizeof(fmt_aestr_temp));
                                              MEMSET(fmt_str,0,sizeof(fmt_str));
                                              MEMSET(text_str,0,sizeof(text_str));
                                                       MEMSET(text_str_gen,0,sizeof(text_str_gen));
                                              ISHELL_LoadResString(pMe->m_pShell,AEE_APPSSPORTSMENU_RES_FILE,IDS_EDIT_GENDER,text,sizeof(text));
                                              
                                              if(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_bGender == FALSE)
                                              {
                                                  ISHELL_LoadResString(pMe->m_pShell,AEE_APPSSPORTSMENU_RES_FILE,IDS_EDIT_MALE,text_temp,sizeof(text_temp));
                              
                                              }
                                              else
                                              {
                                                  ISHELL_LoadResString(pMe->m_pShell,AEE_APPSSPORTSMENU_RES_FILE,IDS_EDIT_FEMALE,text_temp,sizeof(text_temp));                        
                                              }
                                              
                                              SNPRINTF(fmt_str,sizeof(fmt_str),":      ");
                                              STRTOWSTR(fmt_str,fmt_aestr_temp,sizeof(fmt_aestr_temp));
                                              WSTRCAT(text_str,fmt_aestr_temp);
                                              WSTRCAT(text_str,text_temp);
                                              buffer_len = WSTRLEN(text);
                                              WSTRCAT(text + buffer_len,text_str);
                                                  IMENUCTL_SetItemText(pMenu, IDS_EDIT_GENDER, NULL, 0,text);
                                          }
                    
                                IMENUCTL_SetActive(gRoupList, FALSE);
                                IMENUCTL_SetActive(pMenu, TRUE);   
                                pMe->m_pSportsAppMenu.m_nInputMode = LOCAL_NUMBER_INPUT;
                          }
				else if((pMe->m_pSportsAppMenu.m_nInputMode == LOCAL_NUMBER_INPUT)
                    &&(pMe->m_pSportsAppMenu.m_nUnitDataEditFlag != IDS_EDIT_HIGHT))
				{
					IMENUCTL_SetActive(pMenu, TRUE);
	                    	ITEXTCTL_SetActive(pText, FALSE);
	                    	IDIALOG_SetFocus(pMe->m_pActiveDlg, IDC_MENU_EDIT);
					SportSapp_SaveCurrentItemText(pMe,pText);
                                SportSapp_SetMenuText(pMe,pMenu,pMe->m_pSportsAppMenu.m_nUnitDataEditFlag);                      
				}
				else
				{
				       IMENUCTL_SetActive(pMenu, TRUE);
	                    	ITEXTCTL_SetActive(pText, FALSE);
	                    	IDIALOG_SetFocus(pMe->m_pActiveDlg, IDC_MENU_EDIT);
				}  
                        (void)IMENUCTL_Redraw(pMenu);
                          break;
                          
				case AVK_DOWN:
				/*Changed By Show Text  */
                        if(pMe->m_pSportsAppMenu.m_nInputMode == EDIT_GROUP)
                        {
                            {   
                            MEMSET(text,0,sizeof(text));
                            MEMSET(fmt_str,0,sizeof(fmt_str));
                            MEMSET(text_str,0,sizeof(text_str));
                            MEMSET(text_temp,0,sizeof(text_temp));
                            MEMSET(fmt_aestr_temp,0,sizeof(fmt_aestr_temp));
                                     MEMSET(text_str_gen,0,sizeof(text_str_gen));
                            ISHELL_LoadResString(pMe->m_pShell,AEE_APPSSPORTSMENU_RES_FILE,IDS_EDIT_GENDER,text,sizeof(text));

                            if(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_bGender == FALSE)
                            {
                                ISHELL_LoadResString(pMe->m_pShell,AEE_APPSSPORTSMENU_RES_FILE,IDS_EDIT_MALE,text_temp,sizeof(text_temp));                               
                            }
                            else
                            {
                                ISHELL_LoadResString(pMe->m_pShell,AEE_APPSSPORTSMENU_RES_FILE,IDS_EDIT_FEMALE,text_temp,sizeof(text_temp));                                
                            }

                            
                            SNPRINTF(fmt_str,sizeof(fmt_str),":      ");
                            STRTOWSTR(fmt_str,fmt_aestr_temp,sizeof(fmt_aestr_temp));
                            WSTRCAT(text_str,fmt_aestr_temp);
                            WSTRCAT(text_str,text_temp);
                            buffer_len = WSTRLEN(text);
                            WSTRCAT(text + buffer_len,text_str);
                            
                                IMENUCTL_SetItemText(pMenu, IDS_EDIT_GENDER, NULL, 0,text);
                            }
                        
                                  IMENUCTL_SetActive(gRoupList, FALSE);
                                  IMENUCTL_SetActive(pMenu, TRUE);   
                                  pMe->m_pSportsAppMenu.m_nInputMode = LOCAL_NUMBER_INPUT;
                        }
                        else if((pMe->m_pSportsAppMenu.m_nInputMode == LOCAL_NUMBER_INPUT)
                                    ||(pMe->m_pSportsAppMenu.m_nUnitDataEditFlag == IDS_EDIT_NAME))
                        {

                        	IMENUCTL_SetActive(pMenu, TRUE);
                                	ITEXTCTL_SetActive(pText, FALSE);
                                	IDIALOG_SetFocus(pMe->m_pActiveDlg, IDC_MENU_EDIT);
                        	SportSapp_SaveCurrentItemText(pMe,pText);
                                        SportSapp_SetMenuText(pMe,pMenu,pMe->m_pSportsAppMenu.m_nUnitDataEditFlag);                                
                        }
                        else
                        {

                        	IMENUCTL_SetActive(pMenu, TRUE);
                                	ITEXTCTL_SetActive(pText, FALSE);
                                	IDIALOG_SetFocus(pMe->m_pActiveDlg, IDC_MENU_EDIT);

                        }
                        (void)IMENUCTL_Redraw(pMenu);
					break;
                        case AVK_LEFT:
                        case AVK_RIGHT:
                        if(IMENUCTL_GetSel(pMenu) == IDS_EDIT_GENDER)
                        {
                            if(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_bGender == FALSE)
                            {

                            	pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_bGender = TRUE;
                            }
                            else
                            {
                            	pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_bGender = FALSE;
                            }
                        }
				return TRUE;	
                
                            default:
                            break;
            
                        }
			return TRUE;
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
static boolean	HandlePersonalInputDialogEvent(CSportsMenu *pMe,
	AEEEvent eCode,
	uint16 wParam,
	uint32 dwParam
	)
{
	
	ProFileUser active_user; 
	AEERect rc;
	uint16 buffer_len;
	AECHAR *pBuffer = NULL;
	AECHAR text[30] = {0};
	char fmt_str[30] = {0};
    	PARAM_NOT_REF(dwParam)
	AECHAR EmptyTitle[2]={' ','\0'};
    	ITextCtl *pText = (ITextCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_TEXT_INPUT);
			
    	if (pText == NULL)
    	{
        	return FALSE;
    	}
	active_user = SportsApp_GetCurrentActiveUser(pMe);	
	switch (eCode)
	{
		case EVT_DIALOG_INIT:
			rc = pMe->m_rc;
			rc.dy -= GetBottomBarHeight(pMe->m_pDisplay);
			ITEXTCTL_SetRect(pText, &rc);
			
#ifdef FEATURE_CARRIER_CHINA_VERTU
			ITEXTCTL_SetProperties( pText,SportSApp_GetInputProp(pMe->m_pSportsAppMenu.m_nUnitDataFlag)|TP_FOCUS_NOSEL|TP_GRAPHIC_BG);
#else
            ITEXTCTL_SetProperties( pText,SportSApp_GetInputProp(pMe->m_pSportsAppMenu.m_nUnitDataFlag)|TP_FOCUS_NOSEL);
#endif
			(void)ITEXTCTL_SetInputMode(pText,SportSApp_GetInputMode(pMe->m_pSportsAppMenu.m_nUnitDataFlag));
			ITEXTCTL_SetMaxSize( pText,16);
			
			(void)ITEXTCTL_SetTitle( pText,
									 NULL,
									 NULL,
									 EmptyTitle);
				
			switch(pMe->m_pSportsAppMenu.m_nUnitDataFlag)
			{
				case UNIT_DATA_NAME:
					
		            		if(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_pName[0])
			            {
			            		pBuffer = pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_pName;
			                	(void)ITEXTCTL_SetText(pText, pBuffer,-1);
			            }					
					break;
				default:
					
					break;
				
			}

			return TRUE;

		case EVT_DIALOG_START:
			(void) ISHELL_PostEvent( pMe->m_pShell,
						 AEECLSID_SPORTSAPP,
						 EVT_USER_REDRAW,
						 0,
						 0);

			return TRUE;

		case EVT_USER_REDRAW:
			if ( WSTRLEN(ITEXTCTL_GetTextPtr(pText)) > 0)
			{
				SPORTS_MENU_DRAW_BOTTOMBAR(BTBAR_OK_DELETE);
			}
			else
			{
				SPORTS_MENU_DRAW_BOTTOMBAR(BTBAR_OK_BACK);			  
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
					CLOSE_DIALOG(DLGRET_PERSONAL_EDIT);
					return TRUE;
					break;
				case AVK_SELECT:
				case AVK_INFO:
					switch(pMe->m_pSportsAppMenu.m_nUnitDataFlag)
					{
						case UNIT_DATA_NAME:
								FREEIF(pBuffer); 
								pBuffer = WSTRDUP(ITEXTCTL_GetTextPtr(pText));
								buffer_len = WSTRLEN(pBuffer);
								WSTRCPY(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_pName,
									pBuffer);
								
							break;
						case UNIT_DATA_HEIGHT:
                                                    FREEIF(pBuffer);
								pBuffer = WSTRDUP(ITEXTCTL_GetTextPtr(pText));
								MEMSET(fmt_str,0,sizeof(fmt_str));
								WSTRTOSTR(pBuffer,fmt_str,sizeof(fmt_str));
								pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nHeight =
									ATOI(fmt_str);
									
							break;
						case UNIT_DATA_WEIGHT:
                                                    FREEIF(pBuffer);
								pBuffer = WSTRDUP(ITEXTCTL_GetTextPtr(pText));
								MEMSET(fmt_str,0,sizeof(fmt_str));
								WSTRTOSTR(pBuffer,fmt_str,sizeof(fmt_str));
								pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nWeight =
									ATOI(fmt_str);	
							break;
						case UNIT_DATA_WALK_STEP:	
                                                    FREEIF(pBuffer);
								pBuffer = WSTRDUP(ITEXTCTL_GetTextPtr(pText));
								MEMSET(fmt_str,0,sizeof(fmt_str));
								WSTRTOSTR(pBuffer,fmt_str,sizeof(fmt_str));
								pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nWalkstep =
									ATOI(fmt_str);			
							break;
						case UNIT_DATA_RUN_STEP:
                                                    FREEIF(pBuffer);
								pBuffer = WSTRDUP(ITEXTCTL_GetTextPtr(pText));
								MEMSET(fmt_str,0,sizeof(fmt_str));
								WSTRTOSTR(pBuffer,fmt_str,sizeof(fmt_str));
								pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nRunstep =
									ATOI(fmt_str);						
							break;
							case UINT_DATA_BRITHDAY:
                                                        FREEIF(pBuffer);
									pBuffer = WSTRDUP(ITEXTCTL_GetTextPtr(pText));
									MEMSET(fmt_str,0,sizeof(fmt_str));
									WSTRTOSTR(pBuffer,fmt_str,sizeof(fmt_str));
									pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nBrithday =
										ATOI(fmt_str);						
								break;

						default:
							
							break;
						
					}
					
					CLOSE_DIALOG(DLGRET_PERSONAL_EDIT);
					return TRUE;
				default:
					break;
			}
			return TRUE;

		case EVT_COMMAND:
			return TRUE;

#ifdef FEATURE_LCD_TOUCH_ENABLE//ydc add for LCD touch
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

				if(DISPLAYMENU_PT_IN_RECT(wXPos,wYPos,rc))
				{
					if(wXPos >= rc.x && wXPos < rc.x + (rc.dx/3) )//左
					{
						boolean rt =  ISHELL_PostEvent(pMe->m_pShell,AEECLSID_SPORTSAPP,EVT_USER,AVK_SELECT,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)   && wXPos < rc.x + (rc.dx/3)*2 )//中
					{
						boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_SPORTSAPP,EVT_USER,AVK_INFO,0);
						return rt;
					}
					else if(wXPos >= rc.x + (rc.dx/3)*2 && wXPos < rc.x + (rc.dx/3)*3 )//右
					{						
						boolean rt = ISHELL_PostEvent(pMe->m_pShell,AEECLSID_SPORTSAPP,EVT_USER,AVK_CLR,0);
						return rt;
					}
				}

			}
			return TRUE;
#endif		
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
static void SportSApp_DialogTimeout(void *pme)
{
    CSportsMenu *pMe = (CSportsMenu *)pme;

    if (NULL == pMe)
    {
        return;
    }

    (void) ISHELL_PostEvent(pMe->m_pShell,
                            AEECLSID_SPORTSAPP,
                            EVT_DISPLAYDIALOGTIMEOUT,
                            0,
                            0);
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
 static boolean  HandleMessegeBoxDialogEvent(CSportsMenu *pMe,
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
	        (void) ISHELL_PostEventEx(pMe->m_pShell, 
	                                EVTFLG_ASYNC,
	                                AEECLSID_SPORTSAPP,
	                                EVT_USER_REDRAW,
	                                0, 
	                                0);

	        return TRUE;

	    case EVT_USER_REDRAW:
	        {
	            PromptMsg_Param_type  Msg_Param={0};
	            AECHAR  wstrText[MSGBOX_MAXTEXTLEN] = {(AECHAR)'\0'};

	            (void)ISHELL_LoadResString(pMe->m_pShell,
	                            AEE_APPSSPORTSMENU_RES_FILE,                                
	                            pMe->m_pSportsAppMenu.m_wMsgResID,
	                            wstrText,
	                            sizeof(wstrText));
	                            
	            Msg_Param.ePMsgType = MESSAGE_INFORMATION;
	            Msg_Param.pwszMsg = wstrText;

			switch(pMe->m_pSportsAppMenu.m_PopMsgBoxBackFlag)
			{
				case SPORT_STATE_PETOMETER:
                                Msg_Param.ePMsgType = MESSAGE_CONFIRM;
					Msg_Param.eBBarType =  BTBAR_OK_CANCEL;
					break;
				case SPORT_STATE_PETOMETER_OPTION:
                    if(pMe->m_pSportsAppMenu.m_wMsgResID == IDS_HEADSET_OFF)
                    {
                        Msg_Param.ePMsgType = MESSAGE_WARNNING;
                        Msg_Param.eBBarType = BTBAR_NONE;
                    }
                    else
                    {
                        Msg_Param.ePMsgType = MESSAGE_CONFIRM;
					Msg_Param.eBBarType =  BTBAR_OK_CANCEL;
                    }
					break;

                case SPORT_STATE_PERSONAL_EDIT:
                    if(pMe->m_pSportsAppMenu.m_wMsgResID == IDS_DONE)
                    {
                        Msg_Param.ePMsgType = MESSAGE_INFORMATION;
                        Msg_Param.eBBarType = BTBAR_NONE;
                    }
                    else
                    {
                        Msg_Param.ePMsgType = MESSAGE_WARNNING;
                        Msg_Param.eBBarType = BTBAR_NONE;
                    }

					break;
				default:
					Msg_Param.eBBarType = BTBAR_NONE;
					break;
			}
	            
	            DrawPromptMessage(pMe->m_pDisplay, pStatic, &Msg_Param);
	        }

	        IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);

			switch(pMe->m_pSportsAppMenu.m_PopMsgBoxBackFlag)
			{
				case SPORT_STATE_PETOMETER:
					break;
				case SPORT_STATE_PETOMETER_OPTION:
                    if(pMe->m_pSportsAppMenu.m_wMsgResID == IDS_HEADSET_OFF)
                    {
    					(void) ISHELL_SetTimer(pMe->m_pShell,
    	                            PROMPTMSG_TIMER,
    	                            SportSApp_DialogTimeout,
    	                            pMe);                    
                    }
					break;
				default:
					(void) ISHELL_SetTimer(pMe->m_pShell,
	                            PROMPTMSG_TIMER,
	                            SportSApp_DialogTimeout,
	                            pMe);
					break;
			}	

	        
	        return TRUE;

	    case EVT_DIALOG_END:
			switch(pMe->m_pSportsAppMenu.m_PopMsgBoxBackFlag)
			{
				case SPORT_STATE_PETOMETER:
					break;
				case SPORT_STATE_PETOMETER_OPTION:
                    if(pMe->m_pSportsAppMenu.m_wMsgResID == IDS_HEADSET_OFF)
                    {
                        (void) ISHELL_CancelTimer(pMe->m_pShell,SportSApp_DialogTimeout, pMe);
                    }
                    
					break;
				default:
					 (void) ISHELL_CancelTimer(pMe->m_pShell,SportSApp_DialogTimeout, pMe);
					break;
			}
	        ISTATIC_Release(pStatic);
	        pStatic = NULL;
	        
	        return TRUE;

	    case EVT_KEY:
			switch(wParam)
			{
				case AVK_CLR:
					switch(pMe->m_pSportsAppMenu.m_PopMsgBoxBackFlag)
					{
						case SPORT_STATE_PETOMETER:
                                                      if(pMe->m_pSportsAppMenu.m_nPedometerState == PM_STOPED)
                                                      {
							pMe->m_pSportsAppMenu.m_nPedometerState = PM_RUNNING;
							ITIMECTL_SetActive(pMe->m_pSportsAppMenu.m_pSAppTime,TRUE);
							SportSApp_drawWalkAnimation(pMe);
							CLOSE_DIALOG(DLGRET_PEDOMETER)
                                                       }
                                                       else
                                                       {
                                                          CLOSE_DIALOG(DLGRET_PEDOMETER)
                                                       }

							break;
							
						case SPORT_STATE_PETOMETER_OPTION:
							CLOSE_DIALOG(DLGRET_PEDOMETER)
							break;
						default:
							break;
				
					}
					break;
				case AVK_SELECT:
					switch(pMe->m_pSportsAppMenu.m_PopMsgBoxBackFlag)
					{
					    case SPORT_STATE_PETOMETER:
							SportSApp_Set_Shake_Disable(pMe);
							/*Set State And Entry to Pedometer Dialog*/
							#ifdef FEATRUE_SUPPORT_G_SENSOR
							mmi_g_sensor_process(G_SENSOR_ENABLE | G_SENSOR_PEDOMETER);
							#endif
                            
							pMe->m_pSportsAppMenu.m_nPedometerState = PM_RUNNING;                            
                                                        IANNUNCIATOR_SetField(pMe->m_pIAnn,ANNUN_FIELD_SPORTS, ANNUN_STATE_SPORTS_ON);
							CLOSE_DIALOG(DLGRET_PEDOMETER)                            
                                                break;
						case SPORT_STATE_PETOMETER_OPTION:
							/*Disable Shake And Sport Run*/
							SportSApp_Set_Shake_Disable(pMe);
							/*Set State And Entry to Pedometer Dialog*/
							#ifdef FEATRUE_SUPPORT_G_SENSOR
							mmi_g_sensor_process(G_SENSOR_ENABLE | G_SENSOR_PEDOMETER);
							#endif
							pMe->m_pSportsAppMenu.m_nPedometerState = PM_RUNNING;
                            
                                            if(pMe->m_pSportsAppMenu.m_nMsgTypeFlag == SPORT_RUN_WITH_MUSIC)
                                            {
                                                        /*stop Fm Radio*/
                                                        if(SportSApp_IsRunOnGround(pMe) == TRUE)
                                                        {
                                                            if(SportSApp_StopFmRadio(pMe) == FALSE)
                                                            {
                                                                return TRUE;
                                                            }
                                                        }
                                                        
                                                        /*Start  Music*/
                                                        SportSApp_PlayMusicWithSport(pMe);
                                             }
                                            else if(pMe->m_pSportsAppMenu.m_nMsgTypeFlag == SPORT_RUN_WITH_FM)
                                            {
                                                /*stop Music player*/
                                                SportSapp_PauseMusic(pMe);
                                                /*Start Fm radio*/
                                                if(TRUE != SportSApp_StartFmRadioEx(pMe))
                                                {
                                                    return TRUE;
                                                }
                                            }
                                            else
                                            {
                                                
                                            }
                                            IANNUNCIATOR_SetField(pMe->m_pIAnn,ANNUN_FIELD_SPORTS, ANNUN_STATE_SPORTS_ON);
							CLOSE_DIALOG(DLGRET_PEDOMETER)
							break;
					
						default:
							break;
					}					
					break;
			
			}			
	        return TRUE;
	    
	    case EVT_DISPLAYDIALOGTIMEOUT:
			switch(pMe->m_pSportsAppMenu.m_PopMsgBoxBackFlag)
			{
				case SPORT_STATE_PERSONAL_EDIT:
                    if(pMe->m_pSportsAppMenu.m_wMsgResID == IDS_DONE)
                    {
                        CLOSE_DIALOG(DLGRET_PERSONAL)
                    }
                    else
                    {
					CLOSE_DIALOG(DLGRET_PERSONAL_EDIT)
                    }
					break;
                case SPORT_STATE_PETOMETER_OPTION:
                    if(pMe->m_pSportsAppMenu.m_wMsgResID == IDS_HEADSET_OFF)
                    {
                        CLOSE_DIALOG(DLGRET_PEDOMETER)
                    }
                    break;
                case SPORT_STATE_PEDOMETER_RECORD:
					switch(pMe->m_pSportsAppMenu.m_nEntryDialogState)
                    {
						case ENTRY_PED:
                                            pMe->m_pSportsAppMenu.m_nEntryRecordState =REC_NULL;
							CLOSE_DIALOG(DLGRET_PEDOMETER_RECORD);
							break;
						case ENTRY_PERS:
                                            pMe->m_pSportsAppMenu.m_nEntryRecordState =REC_NULL;
							CLOSE_DIALOG(DLGRET_PEDOMETER_RECORD);
							break;
						default:
							break;
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

 /*==============================================================================
函数：
       HandleAppConfirmBgounrdEvent
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
 static boolean  HandleAppConfirmBgRunEvent(CSportsMenu *pMe,
	 AEEEvent eCode,
	 uint16 wParam,
	 uint32 dwParam
	 )
 {
	 PARAM_NOT_REF(dwParam)
	 IMenuCtl *pMenu = (IMenuCtl*)IDIALOG_GetControl(pMe->m_pActiveDlg,IDC_APP_BG_RUN);
	 if (pMenu == NULL)
	 {
		 return FALSE;
	 }
	 SPORTS_ERR("%x, %x ,%x,HandleMainDialogEvent",eCode,wParam,dwParam);
  
	 switch (eCode)
	 {
		 case EVT_DIALOG_INIT:
			IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_RUN_BG, IDS_RUN_BG, NULL, 0);
			IMENUCTL_AddItem(pMenu, AEE_APPSSPORTSMENU_RES_FILE, IDS_APP_EXIT, IDS_APP_EXIT, NULL, 0); 
			 return TRUE;
 
		 case EVT_DIALOG_START:
			 IMENUCTL_SetOemProperties(pMenu,OEMMP_USE_MENU_STYLE);
			 IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
			 IMENUCTL_SetBottomBarType(pMenu,BTBAR_SELECT_BACK);
 
			 (void) ISHELL_PostEvent( pMe->m_pShell,
									  AEECLSID_SPORTSAPP,
									  EVT_USER_REDRAW,
									  0,
									  0);
 
			 return TRUE;
 
		 case EVT_USER_REDRAW:
			 (void)IMENUCTL_Redraw(pMenu);
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
			 //pMe->m_currDlgId = IMENUCTL_GetSel(pMenu);
			 pMe->m_currDlgId = wParam;
			 switch (wParam)
			 {
				 case IDS_RUN_BG:
					 CLOSE_DIALOG(DLGRET_PEDOMETER)
					 break;
				 case IDS_APP_EXIT:
					 CLOSE_DIALOG(DLGRET_CANCELED)
					 break;
			 	default:
					ASSERT_NOT_REACHABLE;
			 }
			 return TRUE;
 
		 default:
			 break;
	 }
	 return FALSE;
 }
 
boolean is_g_sportsapp_pedometer_bground_flag()
{
    return g_sportsapp_pedometer_bground_flag;
}
//end of linjignmao 20090116

boolean is_g_sportsapp_pedometer_flag()
{
    //Note:if g_sportsapp_entry_pedometer_flag = 0 ,sports not use;
    return g_sportsapp_entry_pedometer_flag;
}
#endif

