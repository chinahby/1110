#ifndef _SPORTSAPP_DIALOG_GPROT_C_
#define _SPORTSAPP_DIALOG_GPROT_C_
/*********************************************************************************

模块名       : <SportApplication>
文件名       : <SportSAppDialogGprot.c>
相关文件     : 
文件实现功能 : Sports Application
作者         : 
版本         : 1.0
--------------------------------------------------------------------------------
备注         : 
--------------------------------------------------------------------------------
修改记录 : 

******************************************************************************/
#include "SportSAppDialogGprot.h"
#include "SportSAppOperateData.h"
/*081209*/
#include "Scheduleapp.h"
/*081209*/
#ifdef FEATURE_SPORTS_APP
extern Sport_Appointment pSportAppt;
#endif
#include "Appscommon_color.brh"


/*=============================================================================
FUNCTION: InitPedoMeter

DESCRIPTION:画出秒表界面

PARAMETERS:
   *pme:

RETURN VALUE:
   boolean

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean InitPedoMeter(CSportsMenu *pMe)
{
	uint16 profile_index;
	
	pMe->m_pSportsAppMenu.m_nPedometerState = PM_IDLE;
	pMe->m_pSportsAppMenu.m_nTempSelProFile = USER_NONE;
	pMe->m_pSportsAppMenu.m_bTempEntryView = FALSE;
        /*entry from schedule */
        pMe->FromScheduleFlag = FALSE;
    
	/*Init Animation */
	SportSApp_ReleaseAnimation(pMe);
	/*Init SportsAppMenu*/
	MEMSET(pMe->m_pSportsAppMenu.m_nSportSAppData,0,sizeof(pMe->m_pSportsAppMenu.m_nSportSAppData));
	/*Init Profile WalkStep and RunStep*/
	for(profile_index = 0;profile_index < SPORT_USER_NUM;profile_index++)
	{
		pMe->m_pSportsAppMenu.m_nSportSAppData[profile_index].m_nSportUintData.m_nWalkstep = 80;
		pMe->m_pSportsAppMenu.m_nSportSAppData[profile_index].m_nSportUintData.m_nRunstep = 80;
	}
	return TRUE;
}

/*=============================================================================
FUNCTION: Pedometer_Redraw

DESCRIPTION:画出Pedometer Screen

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void Pedometer_Redraw(CSportsMenu *pMe)
{
	AEEMenuColors color;		//List Menu 的文本及背景颜色
	TitleBar_Param_type title;
	AECHAR		wszTitle[32];	 
	AECHAR		wszState[16];	 
	AEERect rc;
	int16 nLineHeight;

/****************************/
	int titleBarHeight;
	AEERect rect;
/****************************/	


	IImage *bg_ResImg = NULL;
	ProFileUser	active_user; 
	

//Redraw BottomBar
	BottomBar_e_Type BottomDisplayStyle[] = { 
						BTBAR_OPTION_BACK //IDLE
#ifdef FEATURE_SPORTS_APP 						
						,BTBAR_PAUSE_STOP //STOPED
						,BTBAR_RESUME_STOP  //RESUME
#endif						
						};

	nLineHeight = IDISPLAY_GetFontMetrics(pMe->m_pDisplay,AEE_FONT_NORMAL,NULL,NULL);
	
    	IMENUCTL_AddItem(pMe->m_pSportsAppMenu.m_pSAppMenu, AEE_APPSSPORTSMENU_RES_FILE, 
				IDS_PED_WALK, IDS_PED_WALK, NULL, 0);
    	IMENUCTL_AddItem(pMe->m_pSportsAppMenu.m_pSAppMenu, AEE_APPSSPORTSMENU_RES_FILE, 
				IDS_PED_RUN, IDS_PED_RUN, NULL, 0);
	
    	pMe->m_pSportsAppMenu.m_nCtlID = IDC_PEDOMETER_STATE;

	//You should Save This Data and Read It When You want to used this
	active_user = SportsApp_GetCurrentActiveUser(pMe);
	MEMSET(wszTitle,0,sizeof(wszTitle));
	MEMSET(wszState,0,sizeof(wszState));
	
    	IMENUCTL_SetOemProperties(pMe->m_pSportsAppMenu.m_pSAppMenu, OEMMP_SWITCHNAVIGATIONKEY|OEMMP_IDF_ALIGN_CENTER);

	if(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_bPedRunOrWalkState == FALSE)
	{
	    	IMENUCTL_SetSel(pMe->m_pSportsAppMenu.m_pSAppMenu, IDS_PED_WALK);
		if(pMe->m_pSportsAppMenu.m_nPedometerState == PM_RUNNING)
		{
			pMe->m_pSportsAppMenu.animatedIndex = 0;	
			SportSApp_drawWalkAnimation(pMe);	
		}

	}
	else
	{
	    	IMENUCTL_SetSel(pMe->m_pSportsAppMenu.m_pSAppMenu, IDS_PED_RUN);
		if(pMe->m_pSportsAppMenu.m_nPedometerState == PM_RUNNING)
		{
			pMe->m_pSportsAppMenu.animatedIndex = 0;	
			SportSApp_drawRunAnimation(pMe);	
		}

	}

	/**************************************************************************************/
	titleBarHeight = GetTitleBarHeight(pMe->m_pDisplay);
	ITIMECTL_SetFont(pMe->m_pSportsAppMenu.m_pSAppTime, AEE_FONT_BOLD, AEE_FONT_BOLD);

	//Set Time Control Rect
    	SETAEERECT( &rect, 22, 106, pMe->m_rc.dx - 44, titleBarHeight - 8);//Coordinate Time Control display
   	ITIMECTL_SetRect(pMe->m_pSportsAppMenu.m_pSAppTime, &rect);	

	if(pMe->m_pSportsAppMenu.m_nPedometerState == PM_IDLE)
	{
		(void) ITIMECTL_SetTimeEx(pMe->m_pSportsAppMenu.m_pSAppTime,
					pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nSportendtime, TRUE);
	}
	/*****************************************************************************************/	
	//Set Bg Color and so on
    	color.wMask = MC_SEL_TEXT | MC_SEL_BACK | MC_TEXT | MC_BACK;
    	color.cBack = RGB_BLACK;
    	color.cText =  RGB_WHITE;
    	color.cSelBack = RGB_BLACK;
    	color.cSelText = RGB_WHITE;	

	IDISPLAY_FillRect(pMe->m_pDisplay,	&pMe->m_rc,  RGB_BLACK);
	//Set Menu Area
	SETAEERECT(&rc,  22, 36,pMe->m_rc.dx - 44,18);
	IDISPLAY_EraseRect(pMe->m_pDisplay,&rc);
	SETAEERECT(&rc,  22, 36,pMe->m_rc.dx - 44,18);
							
	IMENUCTL_SetRect(pMe->m_pSportsAppMenu.m_pSAppMenu, &rc);
	IDISPLAY_FillRect(pMe->m_pDisplay, &rc,  RGB_WHITE);
	//End Set Area And Fill Color

	(void) ISHELL_LoadResString(pMe->m_pShell,
									AEE_APPSSPORTSMENU_RES_FILE,
									IDS_SPORTS_PEDOMETER,
									wszTitle,
									sizeof(wszTitle));
	//Set Main Title Coordinate
	rc.x = 0;
	rc.dx = pMe->m_rc.dx;
	rc.y = 0;
	rc.dy = nLineHeight;
	//Set end
	MEMSET(&title,0,sizeof(TitleBar_Param_type));
	title.dwAlignFlags = IDF_TEXT_TRANSPARENT | IDF_ALIGN_CENTER;
	title.pwszTitle = wszTitle;
	DrawTitleBar(pMe->m_pDisplay,&title);

	IMENUCTL_SetColors(pMe->m_pSportsAppMenu.m_pSAppMenu, &color);
	//Set Bottom Bar Option and Back
	SPORTS_MENU_DRAW_BOTTOMBAR(BottomDisplayStyle[pMe->m_pSportsAppMenu.m_nPedometerState]);
	
	//Set Bg Image
	bg_ResImg = ISHELL_LoadResImage(pMe->m_pShell,PEDOMETER_RES_FILE,IDI_PEDOMETER_BG);
	
	if(bg_ResImg  != NULL)
	{
		IIMAGE_Draw(bg_ResImg, 13,36);
		IIMAGE_Release(bg_ResImg);
		bg_ResImg = NULL;

	}
	{
		AECHAR text[30] = {0};
		char fmt_str[30] = {0};
		char text_str[] = "0.0";
		char text_distance[] = "0.0";
		if(pMe->m_pSportsAppMenu.m_nPedometerState == PM_IDLE)
		{
			STRTOWSTR(text_str,pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nSpeed,
				sizeof(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nSpeed));

		STRTOWSTR(text_distance,pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nDistance,
			sizeof(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nDistance));

		}
		IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
	        (void)IDISPLAY_DrawText(pMe->m_pDisplay, AEE_FONT_NORMAL,
	              pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nDistance,
	              -1,18,65,NULL,IDF_TEXT_TRANSPARENT);
		

		IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
	        (void)IDISPLAY_DrawText(pMe->m_pDisplay, AEE_FONT_NORMAL,
	              pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nSpeed,
	              -1,95,65,NULL,IDF_TEXT_TRANSPARENT);

		/*Display  Step*/
        {
    	    AECHAR m_wszTitle[20] = {0};
        	(void) ISHELL_LoadResString(pMe->m_pShell,
    				AEE_APPSSPORTSMENU_RES_FILE,
    				IDS_STEP,
    				m_wszTitle,
    				sizeof(m_wszTitle));        
            		SNPRINTF(fmt_str,sizeof(fmt_str),"%d  ",
			pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nStep);
                    
		STRTOWSTR(fmt_str,text,sizeof(text));
            WSTRCAT(text,m_wszTitle);
            
        }
		
		IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
			(void)IDISPLAY_DrawText(pMe->m_pDisplay, AEE_FONT_NORMAL,
						  text,-1,93,146,NULL,IDF_TEXT_TRANSPARENT);
		IDISPLAY_Update(pMe->m_pDisplay);

	}
	//Set end Image
	if(pMe->m_pSportsAppMenu.m_nPedometerState == PM_IDLE)
	{
		IMENUCTL_SetActive(pMe->m_pSportsAppMenu.m_pSAppMenu, TRUE);
	}
	else
	{
		IMENUCTL_SetActive(pMe->m_pSportsAppMenu.m_pSAppMenu, FALSE);
	}
	//Time and Menu Control Redraw
	ITIMECTL_Redraw(pMe->m_pSportsAppMenu.m_pSAppTime);
	IMENUCTL_Redraw(pMe->m_pSportsAppMenu.m_pSAppMenu);

}

/*=============================================================================
FUNCTION: PedometerFree

DESCRIPTION:Free Pedometer Control Handler

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void PedometerFree(CSportsMenu *pMe)
{
    if (pMe->m_pSportsAppMenu.m_pSAppMenu != NULL)
	{
       IMENUCTL_Release(pMe->m_pSportsAppMenu.m_pSAppMenu);
       pMe->m_pSportsAppMenu.m_pSAppMenu = NULL;
    }

    if (pMe->m_pSportsAppMenu.m_pSAppTime != NULL) {
       ITIMECTL_Release(pMe->m_pSportsAppMenu.m_pSAppTime);
       pMe->m_pSportsAppMenu.m_pSAppTime = NULL;
    }

    ISHELL_CancelTimer(pMe->m_pShell,NULL,pMe);

	
	
}


/*=============================================================================
FUNCTION: Pedometer_Gettime

DESCRIPTION:画出当前要刷新的时间

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void Pedometer_Gettime(CSportsMenu *pMe,uint32 nTicks)
{

	ProFileUser	active_user; 
	active_user = SportsApp_GetCurrentActiveUser(pMe);

    	if((nTicks - pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nTicks) > MSECS_PER_DAY)
	{
       	pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nNumDays++;
    	}

	pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nSportendtime += 
		(nTicks - pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nTicks) + (pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nNumDays * MSECS_PER_DAY);

	pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nTicks = 
		ISHELL_GetTimeMS(pMe->m_pShell);
		
	ITIMECTL_SetProperties(pMe->m_pSportsAppMenu.m_pSAppTime,0);

	if(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_suspending != TRUE)
	{
		if((pMe->runOnBackgrounds == TRUE)&&(pMe->GobackIdleFlag == TRUE))
		{
		}
		else
		{
   		(void) ITIMECTL_SetTimeEx(pMe->m_pSportsAppMenu.m_pSAppTime,
			pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nSportendtime,TRUE);
	}
	}

}

/*=============================================================================
FUNCTION: Pedometer_Running

DESCRIPTION: 秒表开始跑

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void Pedometer_Running(CSportsMenu *pMe)
{
	uint32   nMS;

	nMS = ISHELL_GetTimeMS(pMe->m_pShell);

	Pedometer_Gettime(pMe,nMS);

	nMS = (RUNNING_UPDATE_MSECS - (ISHELL_GetTimeMS(pMe->m_pShell) - nMS)) + 1;

	ISHELL_SetTimer(pMe->m_pShell,(int32)nMS,(PFNNOTIFY)Pedometer_Running,pMe);

}

/*=============================================================================
FUNCTION: Pedometer_Stop

DESCRIPTION: 秒表停止

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void Pedometer_Stop(CSportsMenu *pMe)
{
	ProFileUser active_user; 
	active_user = SportsApp_GetCurrentActiveUser(pMe);
	pMe->m_pSportsAppMenu.m_nPedometerState = PM_STOPED;
	
    	ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY)Pedometer_Running, pMe);
	if(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_bPedRunOrWalkState == FALSE)
	{
		ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY)SportSApp_drawWalkAnimation, pMe);
	}
	else
	{
		ISHELL_CancelTimer(pMe->m_pShell, (PFNNOTIFY)SportSApp_drawRunAnimation, pMe);	
	}
    	pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nSportendtime += 
								ISHELL_GetTimeMS(pMe->m_pShell) 
								- pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nTicks;
	
    	ITIMECTL_SetProperties(pMe->m_pSportsAppMenu.m_pSAppTime,0);
    	ITIMECTL_SetTimeEx(pMe->m_pSportsAppMenu.m_pSAppTime,
				pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nSportendtime, TRUE);
	
	Pedometer_Redraw(pMe);
}

/*=============================================================================
FUNCTION: StopWatch_Continue

DESCRIPTION: 秒表暂停后继续执行

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void Pedometer_Resume(CSportsMenu *pMe)
{
	ProFileUser active_user; 
	active_user = SportsApp_GetCurrentActiveUser(pMe);
	pMe->m_pSportsAppMenu.m_nPedometerState = PM_RUNNING;

	pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nTicks = 
		ISHELL_GetTimeMS( pMe->m_pShell);
	
	Pedometer_Running(pMe);


}

/*=============================================================================
FUNCTION: Pedometer_Reset

DESCRIPTION: 秒表置零

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void Pedometer_Reset(CSportsMenu *pMe)
{
	ProFileUser active_user;
	active_user = SportsApp_GetCurrentActiveUser(pMe);
	pMe->m_pSportsAppMenu.m_nPedometerState = PM_IDLE;
	/*Save All Active Data*/
	SportsApp_SaveAllCurrentActiveData(pMe);
    	/*Rest Pedometer Data*/
	MEMSET(&pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData,
			0,sizeof(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData));

    	ITIMECTL_SetProperties(pMe->m_pSportsAppMenu.m_pSAppTime,0);
    	ITIMECTL_SetTimeEx(pMe->m_pSportsAppMenu.m_pSAppTime,
					pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nSportendtime, TRUE);


}

/*=============================================================================
FUNCTION: Pedometer_ResetConfirm

DESCRIPTION: 秒表置零确定对话

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void Pedometer_ResetConfirm(CSportsMenu *pMe)
{
	if(pMe->m_pSportsAppMenu.pStatic == NULL)
	{
		if( SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_STATIC, (void **)&pMe->m_pSportsAppMenu.pStatic))
		{
			return;
		}
	}

	{
		PromptMsg_Param_type	parm;
		AECHAR	text[16];

		MEMSET( &parm, 0, sizeof( parm));
		parm.ePMsgType	= MESSAGE_CONFIRM;
		parm.eBBarType	= BTBAR_OK_BACK;	   

//Set State
		pMe->m_pSportsAppMenu.m_nPedometerState = PM_REST_CONFIRM;
		
		ISHELL_LoadResString(pMe->m_pShell, AEE_APPSSPORTSMENU_RES_FILE, IDS_STOP_CONFIRM, text, 32);
		parm.pwszMsg = text;

		DrawPromptMessage( pMe->m_pDisplay,pMe->m_pSportsAppMenu.pStatic,&parm);
		
	}
}


/*=============================================================================
FUNCTION: Pedometer_ResetConfirm

DESCRIPTION: 秒表置零确定对话

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void Pedometer_RecordRestConfirm(CSportsMenu *pMe)
{
	PromptMsg_Param_type	parm;
	AECHAR	text[16];

	MEMSET( &parm, 0, sizeof( parm));
	parm.ePMsgType	= MESSAGE_CONFIRM;	   
	parm.eBBarType	= BTBAR_OK_BACK;	  

	if(pMe->m_pSportsAppMenu.pStatic == NULL)
	{
		if( SUCCESS != ISHELL_CreateInstance(pMe->m_pShell, AEECLSID_STATIC, (void **)&pMe->m_pSportsAppMenu.pStatic))
		{
			return;
		}
	}

	ISHELL_LoadResString(pMe->m_pShell, AEE_APPSSPORTSMENU_RES_FILE, IDS_REST_CONFIRM, text, 32);
	parm.pwszMsg = text;
	DrawPromptMessage( pMe->m_pDisplay,pMe->m_pSportsAppMenu.pStatic,&parm);
		
}

/*=============================================================================
FUNCTION: Pedometer_SetPedSpeedData

DESCRIPTION: 秒表清空数据

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void Pedometer_SetPedSpeedData(CSportsMenu *pMe)
{
	

}

/*=============================================================================
FUNCTION: Pedometer_ClearData

DESCRIPTION: 秒表清空数据

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void Pedometer_ClearData(CSportsMenu *pMe)
{


}

/*=============================================================================
FUNCTION: SportSApp_GetInputMode

DESCRIPTION: 获取输入模式

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
SportUnitData SportSApp_GetInputMode(SportUnitData Input_Id)
{
    switch(Input_Id)
    {
        case UNIT_DATA_NAME:
            return AEE_TM_CURRENT;
            
        case UNIT_DATA_HEIGHT:
        case UNIT_DATA_WEIGHT:
        case UNIT_DATA_WALK_STEP:
        case UNIT_DATA_RUN_STEP:
	  case UINT_DATA_BRITHDAY:
            return AEE_TM_NUMBERS;
            
        default:
            return AEE_TM_NUMBERS;
    }
}// CContApp_GetFldInputMode

/*=============================================================================

FUNCTION:CContApp_GetFldInputProp

DESCRIPTION:
    获得对应 Field 的输入法属性
    
PARAMETERS:
    nFldID  : 字段类型
    
RETURN VALUE:
    The property of the text control
    
COMMENTS:
    
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 SportSApp_GetInputProp(SportUnitData Input_Id)
{
    switch(Input_Id)
    {
        	case UNIT_DATA_NAME:
			return (TP_MULTILINE | TP_FRAME | TP_FIXSETRECT | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT );
		case UNIT_DATA_HEIGHT:
		case UNIT_DATA_WEIGHT:
		case UNIT_DATA_WALK_STEP:
		case UNIT_DATA_RUN_STEP:
		case UINT_DATA_BRITHDAY:
			return (TP_FRAME | TP_FIXSETRECT |TP_EDITNUMBER_PTSTRING);
            		

        	default:
            		return 0;
    }
}// CContApp_GetFldInputProp


/*=============================================================================

FUNCTION:SportSApp_SetSavePrometer

DESCRIPTION:
    获得对应 Field 的输入法属性
    
PARAMETERS:
    nFldID  : 字段类型
    
RETURN VALUE:
    The property of the text control
    
COMMENTS:
    
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SportSApp_SetSavePrometer(CSportsMenu *pMe)
{
	
}

/*=============================================================================
FUNCTION:  SportSApp_drawAnimation

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SportSApp_drawWalkAnimation(CSportsMenu *pMe)
{
	ProFileUser	active_user; 
	active_user = SportsApp_GetCurrentActiveUser(pMe);
	
    	if( pMe->m_pSportsAppMenu.imageIconAnimated[pMe->m_pSportsAppMenu.animatedIndex] == NULL)
    	{
        	pMe->m_pSportsAppMenu.imageIconAnimated[pMe->m_pSportsAppMenu.animatedIndex] = 
				ISHELL_LoadResImage(pMe->m_pShell,
				AEE_APPSSPORT_WALK_RES_FILE,
                			IDI_WALK_ANI_1 + pMe->m_pSportsAppMenu.animatedIndex);
    	}

    	if( pMe->m_pSportsAppMenu.imageIconAnimated[pMe->m_pSportsAppMenu.animatedIndex] != NULL)
    	{
        	IIMAGE_Draw( pMe->m_pSportsAppMenu.imageIconAnimated[pMe->m_pSportsAppMenu.animatedIndex],
			  /*Coordinate X*/
                     13,
                     /*Coordinate Y*/
                     146);
		IIMAGE_Release(pMe->m_pSportsAppMenu.imageIconAnimated[pMe->m_pSportsAppMenu.animatedIndex]);
		pMe->m_pSportsAppMenu.imageIconAnimated[pMe->m_pSportsAppMenu.animatedIndex] = NULL;
    	}
    	IDISPLAY_UpdateEx( pMe->m_pDisplay, TRUE);

    	if( pMe->m_pSportsAppMenu.animatedIndex < 4)
    	{
        	ISHELL_SetTimer (pMe->m_pShell,
                        	120,
                        	( PFNNOTIFY)SportSApp_drawWalkAnimation,
                        	( void*)pMe);

    }
	if(pMe->m_pSportsAppMenu.animatedIndex < 3)
	{
        	++ pMe->m_pSportsAppMenu.animatedIndex;
	}
	else
	{
		pMe->m_pSportsAppMenu.animatedIndex = 0;
	}

}

/*=============================================================================
FUNCTION:  SportSApp_drawRunAnimation

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SportSApp_drawRunAnimation(CSportsMenu *pMe)
{
	ProFileUser	active_user; 
	active_user = SportsApp_GetCurrentActiveUser(pMe);
	
    	if( pMe->m_pSportsAppMenu.imageIconAnimated[pMe->m_pSportsAppMenu.animatedIndex] == NULL)
    	{
        	pMe->m_pSportsAppMenu.imageIconAnimated[pMe->m_pSportsAppMenu.animatedIndex] = 
				ISHELL_LoadResImage(pMe->m_pShell,
				AEE_APPSSPORT_RUN_RES_FILE,
                			IDI_RUN_ANI_1 + pMe->m_pSportsAppMenu.animatedIndex);
    	}

    	if( pMe->m_pSportsAppMenu.imageIconAnimated[pMe->m_pSportsAppMenu.animatedIndex] != NULL)
    	{
        	IIMAGE_Draw( pMe->m_pSportsAppMenu.imageIconAnimated[pMe->m_pSportsAppMenu.animatedIndex],
			  /*Coordinate X*/
                     13,
                     /*Coordinate Y*/
                     146);
		IIMAGE_Release(pMe->m_pSportsAppMenu.imageIconAnimated[pMe->m_pSportsAppMenu.animatedIndex]);
		pMe->m_pSportsAppMenu.imageIconAnimated[pMe->m_pSportsAppMenu.animatedIndex] = NULL;
    	}
    	IDISPLAY_UpdateEx( pMe->m_pDisplay, TRUE);

    	if( pMe->m_pSportsAppMenu.animatedIndex < 6)
    	{
        	ISHELL_SetTimer (pMe->m_pShell,
                        	120,
                        	( PFNNOTIFY)SportSApp_drawRunAnimation,
                        	( void*)pMe);

    	}
		
	if(pMe->m_pSportsAppMenu.animatedIndex < 5)
	{
        	++ pMe->m_pSportsAppMenu.animatedIndex;
	}
	else
	{
		pMe->m_pSportsAppMenu.animatedIndex = 0;
	}

}

/*=============================================================================
FUNCTION:  SportSApp_DisplayArrow

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SportSApp_DisplayArrow(CSportsMenu *pMe)
{
	IImage		*pR_ResImg = NULL;
	IImage		*pL_ResImg = NULL;
	pL_ResImg  = ISHELL_LoadResImage(pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDB_LEFTARROW);
	pR_ResImg  = ISHELL_LoadResImage(pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDB_RIGHTARROW);
	
	if(pR_ResImg != NULL)
	{
		IIMAGE_Draw(pR_ResImg, 150, 165);
		IIMAGE_Draw(pR_ResImg, 150 ,165);
	}

	if(pL_ResImg != NULL)
	{
		IIMAGE_Draw(pL_ResImg, 70,165);
		IIMAGE_Draw(pL_ResImg, 70,165);
	}
	IIMAGE_Release(pR_ResImg);
	pR_ResImg = NULL;
	IIMAGE_Release(pL_ResImg);
	pL_ResImg = NULL;
	IDISPLAY_UpdateEx( pMe->m_pDisplay, TRUE);
}

/*=============================================================================
FUNCTION:  SportSApp_ReleaseAnimation

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SportSApp_ReleaseAnimation(CSportsMenu *pMe)
{
    int j = 0;


        for( j = 0; j < 4; j ++)
        {
            if( pMe->m_pSportsAppMenu.imageIconAnimated[j] != NULL)
            {
                //IIMAGE_Release(pMe->m_pSportsAppMenu.imageIconAnimated[i][i]);
                pMe->m_pSportsAppMenu.imageIconAnimated[j] = NULL;
            }

    	}

}

/*=============================================================================
FUNCTION:  SportsApp_ShowEditItem

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SportsApp_ShowEditItem(CSportsMenu  *pMe, IMenuCtl  *pMenuCtl,ITextCtl *pText)
{
	CtlAddItem	menuItem;
	AEERect 	rect;
	ProFileUser active_user; 
	AECHAR text[16] = {0};
	char fmt_str[16] = {0};
		active_user = SportsApp_GetCurrentActiveUser(pMe);
	if( IMENUCTL_GetItem( pMenuCtl, IMENUCTL_GetSel( pMenuCtl), &menuItem))
	{
		switch(pMe->m_pSportsAppMenu.m_nUnitDataFlag)
		{
			case UNIT_DATA_HEIGHT:
				{
						SNPRINTF(fmt_str,sizeof(fmt_str),"%d",
							pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nHeight);
						STRTOWSTR(fmt_str, text, sizeof(text));
						(void)ITEXTCTL_SetText(pText, text,-1);
				}
				break;
			case UNIT_DATA_WEIGHT:
				{
					SNPRINTF(fmt_str,sizeof(fmt_str),"%d",
						pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nWeight);
					STRTOWSTR(fmt_str, text, sizeof(text));
					(void)ITEXTCTL_SetText(pText, text,-1);

				}					
				break;
			case UNIT_DATA_WALK_STEP:
				{
					SNPRINTF(fmt_str,sizeof(fmt_str),"%d",
						pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nWalkstep);
					STRTOWSTR(fmt_str, text, sizeof(text));
					(void)ITEXTCTL_SetText(pText, text,-1);

				}					
				break;
			case UNIT_DATA_RUN_STEP:
				{
					SNPRINTF(fmt_str,sizeof(fmt_str),"%d",
						pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nRunstep);
					STRTOWSTR(fmt_str, text, sizeof(text));
					(void)ITEXTCTL_SetText(pText, text,-1);

				}						
				break;
			case UINT_DATA_BRITHDAY:
				{
					SNPRINTF(fmt_str,sizeof(fmt_str),"%d",
						pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nBrithday);
					STRTOWSTR(fmt_str, text, sizeof(text));
					(void)ITEXTCTL_SetText(pText, text,-1);

				}				
				break;
			default:
				
				break;
						
		}


	}

/*Set Menu Text Area*/
	IMENUCTL_GetSelItemRect( pMenuCtl, &rect);
	rect.x	= 20;
	rect.dx = pMe->m_rc.dx - 20;
        rect.dy -= 1;
	ITEXTCTL_SetRect( pText, &rect);
	ITEXTCTL_SetInputMode(pText, AEE_TM_NUMBERS);
/*Set End Menu Text Area*/
	if ( WSTRLEN(ITEXTCTL_GetTextPtr(pText)) >= 0)
	{
		SPORTS_MENU_DRAW_BOTTOMBAR(BTBAR_SAVE_DELETE);
	}
	else
	{
		SPORTS_MENU_DRAW_BOTTOMBAR(BTBAR_OK_BACK);			  
	}

        #ifndef FEATURE_CARRIER_CHINA_VERTU
	ITEXTCTL_SetProperties(pText, TP_FIXSETRECT|TP_FIXOEM|TP_FOCUS_NOSEL);
        #else
        ITEXTCTL_SetProperties(pText, TP_FIXSETRECT|TP_FIXOEM|TP_FOCUS_NOSEL|TP_GRAPHIC_BG);
        #endif
	ITEXTCTL_SetMaxSize(pText, 4);
	IDISPLAY_SetColor( pMe->m_pDisplay, CLR_USER_TEXT, RGB_BLACK);
	IMENUCTL_SetActive(pMenuCtl, FALSE);
	ITEXTCTL_SetActive(pText, TRUE);
	IDIALOG_SetFocus(pMe->m_pActiveDlg, IDC_TEXT_PERS);
	ITEXTCTL_Redraw(pText);
	IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
}

/*=============================================================================
FUNCTION:  SportSapp_SaveCurrentItemText

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SportSapp_SaveCurrentItemText(CSportsMenu  *pMe,  ITextCtl  *pText)
{
	ProFileUser active_user; 
	AECHAR *pBuffer = NULL;
	char fmt_str[32] = {0};
	
	active_user = SportsApp_GetCurrentActiveUser(pMe);
	
        FREEIF(pBuffer);
	switch(pMe->m_pSportsAppMenu.m_nUnitDataFlag)
	{
		case UNIT_DATA_NAME:	
			break;
		case UNIT_DATA_HEIGHT:
				pBuffer = WSTRDUP(ITEXTCTL_GetTextPtr(pText));
				MEMSET(fmt_str,0,sizeof(fmt_str));
                          if(WSTRLEN(pBuffer) != 0)
                          {
				WSTRTOSTR(pBuffer,fmt_str,sizeof(fmt_str));
				pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nHeight =
					ATOI(fmt_str);
					
                          }	
			break;
		case UNIT_DATA_WEIGHT:
				pBuffer = WSTRDUP(ITEXTCTL_GetTextPtr(pText));
				MEMSET(fmt_str,0,sizeof(fmt_str));
                            if(WSTRLEN(pBuffer) != 0)
                            {
				WSTRTOSTR(pBuffer,fmt_str,sizeof(fmt_str));
				pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nWeight =
					ATOI(fmt_str);	
                            
                            }
			break;
		case UNIT_DATA_WALK_STEP:	
				pBuffer = WSTRDUP(ITEXTCTL_GetTextPtr(pText));
				MEMSET(fmt_str,0,sizeof(fmt_str));
                          if(WSTRLEN(pBuffer) != 0)
                          {
				WSTRTOSTR(pBuffer,fmt_str,sizeof(fmt_str));
				pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nWalkstep =
					ATOI(fmt_str);			
                         }
			break;
		case UNIT_DATA_RUN_STEP:
				pBuffer = WSTRDUP(ITEXTCTL_GetTextPtr(pText));
				MEMSET(fmt_str,0,sizeof(fmt_str));
                         if(WSTRLEN(pBuffer) != 0)
                         {
				WSTRTOSTR(pBuffer,fmt_str,sizeof(fmt_str));
				pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nRunstep =
					ATOI(fmt_str);						
                         }
			break;
		case UINT_DATA_BRITHDAY:
				pBuffer = WSTRDUP(ITEXTCTL_GetTextPtr(pText));
				MEMSET(fmt_str,0,sizeof(fmt_str));
                        if(WSTRLEN(pBuffer) != 0)
                            {
				WSTRTOSTR(pBuffer,fmt_str,sizeof(fmt_str));
				pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_nBrithday =
					ATOI(fmt_str);						
                            }
			break;			
		default:
			
			break;
		
	}

}

/*==============================================================================
函数:
    SportsApp_SportsPlan

说明:
    本函数进入到 日程表模块

参数:


返回值:
    无。

备注:
    调用Schedule Applet，Sports applet被挂起。
        
==============================================================================*/
void SportSApp_SportsPlan(CSportsMenu *pMe)
{

    IScheduleApp *pSports = NULL;

    if (SUCCESS == ISHELL_CreateInstance(pMe->m_pShell,
                                    AEECLSID_SCHEDULEAPP,
                                    (void**)&pSports))
    {
        if (pSports == NULL)
        {
            return;
        }
        
        
        ISCHEDULEAPP_SportsPlan(pSports);
        ISCHEDULEAPP_Release(pSports);

    } 

}

/*=============================================================================
FUNCTION:  SportSapp_SetMenuText

DESCRIPTION: 

PARAMETERS:

RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SportSapp_SetMenuText(CSportsMenu *pMe,IMenuCtl *pMenu,uint16 nSelMenu)
{
    	uint16 buffer_len;
      ProFileUser active_user; 
	AECHAR *pBuffer = NULL;
	AECHAR text[30] = {0};
	char fmt_str[30] = {0};
	AECHAR text_str[30] = {0};

      active_user = SportsApp_GetCurrentActiveUser(pMe);
    switch(nSelMenu)
    {
        case IDS_EDIT_HIGHT:
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
                            IMENUCTL_SetItemText(pMenu, IDS_EDIT_HIGHT, NULL, 0,
                                text);
                }
            break;
        case IDS_EDIT_WEIGHT:
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
                            IMENUCTL_SetItemText(pMenu, IDS_EDIT_WEIGHT, NULL, 0,
                                text);
                }

           
            break;
         case IDS_WALK_STEP:
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


            
            break;
         case IDS_RUN_STEP:
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
                             IMENUCTL_SetItemText(pMenu, IDS_RUN_STEP, NULL, 0,
                                 text);
             
                 }

            break;
         case IDS_EDIT_BRITHDAY:
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
            break;
            
         default:
            break;
         
  
    }
       
}


/*=============================================================================

FUNCTION:SportSApp_BuildGroupList

DESCRIPTION:
    向选项菜单中添加内容
    
PARAMETERS:
    pMe     :
    pMenuCtl: 指向Menu Control的指针
    bAll    : 是否添加全部项目
RETURN VALUE:
    SUCCESS: 
    EFAILED: If the number of current cache is 0
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
int SportSApp_BuildGroupList(CSportsMenu  *pMe, IMenuCtl  *groupList)
{
    if( NULL == groupList)
    {
        return EFAILED;
    }

    // Clear items
    (void)IMENUCTL_DeleteAll(groupList);

    IMENUCTL_AddItem(groupList, AEE_APPSSPORTSMENU_RES_FILE, IDS_GENDER_MALE, IDS_GENDER_MALE, NULL, 0);
    IMENUCTL_AddItem(groupList, AEE_APPSSPORTSMENU_RES_FILE, IDS_EDIT_FEMALE, IDS_EDIT_FEMALE, NULL, 0);   
    

return SUCCESS;
}


/*=============================================================================

FUNCTION:SportSApp_BuildGroupList

DESCRIPTION:
    向选项菜单中添加内容
    
PARAMETERS:
    pMe     :
    pMenuCtl: 指向Menu Control的指针
    bAll    : 是否添加全部项目
RETURN VALUE:
    SUCCESS: 
    EFAILED: If the number of current cache is 0
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/

void SportSapp_ShowGroupSelect(CSportsMenu  *pMe, IMenuCtl  *pMenuCtl, IMenuCtl  *groupList)
{
    AEERect         rect;
    IImage          *left_arrow;
    IImage          *right_arrow;
    ProFileUser active_user; 
    AEEImageInfo    info = {0};
    int16           curGroup;

    IMENUCTL_GetSelItemRect( pMenuCtl, &rect);
    rect.x  = 20;//20是大概的一个数字,一个偏移值，应该是群组那张图片的宽度
    rect.dx = pMe->m_rc.dx - 20;
    
    if( NULL == (left_arrow  = ISHELL_LoadResImage(pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDB_LEFTARROW) ) )
    {
        ERR("error in load image left arrow", 0,0,0);
        return;
    }
    if( NULL == (right_arrow  = ISHELL_LoadResImage(pMe->m_pShell,AEE_APPSCOMMONRES_IMAGESFILE,IDB_RIGHTARROW) ) )
    {
        ERR("error in load image right arrow", 0,0,0);
        return;
    }

    IIMAGE_GetInfo(right_arrow, &info);

    IDISPLAY_EraseRect(pMe->m_pDisplay, &rect);
    IIMAGE_Draw( left_arrow, rect.x + 2, rect.y + 4);
    IIMAGE_Draw( right_arrow, rect.dx + 20 - info.cx - 2 , rect.y + 4);

    rect.x += (info.cx + 4);
    rect.dx -= (info.cx + 4) * 2;
    IMENUCTL_SetRect(groupList, &rect);

    //AECHAR pGroup[MAX_INPUT_NAME_EN + 1];
    //pGroup[0] = ONEDIAL_NULL;
    active_user = SportsApp_GetCurrentActiveUser(pMe);
    if(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_bGender == FALSE)
    {
        DBGPRINTF("SportSapp_ShowGroupSelect ");
        curGroup = IDS_GENDER_MALE;
    }
    else
    {
        DBGPRINTF("SportSapp_ShowGroupSelect else ");
        curGroup = IDS_EDIT_FEMALE;
    }
    
    
    IMENUCTL_SetSel( groupList, curGroup);
    //IMENUCTL_SetBottomBarType(groupList, BTBAR_SELECT_BACK);
    //CONTAPP_DRAW_BOTTOMBAR(BTBAR_SELECT_BACK);
    IMENUCTL_SetOemProperties(groupList, OEMMP_SWITCHNAVIGATIONKEY | OEMMP_IDF_ALIGN_CENTER | OEMMP_DISTINGUISH_INFOKEY_SELECTKEY); 
    IMENUCTL_Redraw(groupList);
    IIMAGE_Release(left_arrow);
    IIMAGE_Release(right_arrow);
    left_arrow = NULL;
    right_arrow = NULL;
    
}

/*=============================================================================

FUNCTION:SportSapp_BuildAddMenuMenu

DESCRIPTION:
    向选项菜单中添加内容
    
PARAMETERS:
    pMe     :
    pMenuCtl: 指向Menu Control的指针
    bAll    : 是否添加全部项目
RETURN VALUE:
    SUCCESS: 
    EFAILED: If the number of current cache is 0
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
int CSportSapp_BuildAddMenuMenu(CSportsMenu *pMe, IMenuCtl *pMenuCtl)
{
    CtlAddItem ai;
    
    ASSERT(pMe != NULL); 
    
     // Check Parameter
    if(!pMenuCtl)
    {
        return EBADPARM;
    }
    
    // Clear items
    (void)IMENUCTL_DeleteAll(pMenuCtl);
    
    // Init items
    MEMSET(&ai, 0, sizeof(ai));
    ai.pszResText = AEE_APPSSPORTSMENU_RES_FILE;
    ai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;


    ai.wText     = IDS_EDIT_NAME;
    ai.wItemID   = IDS_EDIT_NAME;
    ai.wImage    = IDB_SPORT_RACE;
    
    if(FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
    {
       return EFAILED;
    }    

    ai.wText     = IDS_EDIT_HIGHT;
    ai.wItemID   = IDS_EDIT_HIGHT;
    ai.wImage    = IDB_SPORT_RACE;
            
    if(FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
    {
       return EFAILED;
    }    

    ai.wText     = IDS_EDIT_WEIGHT;
    ai.wItemID   = IDS_EDIT_WEIGHT;
    ai.wImage    = IDB_SPORT_RACE;
          
    if(FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
    {
        return EFAILED;
    }        

    ai.wText     = IDS_WALK_STEP;
    ai.wItemID   = IDS_WALK_STEP;
    ai.wImage    = IDB_SPORT_RACE;
            
    if(FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
    {
        return EFAILED;
    }        

    ai.wText     = IDS_RUN_STEP;
    ai.wItemID   = IDS_RUN_STEP;
    ai.wImage    = IDB_SPORT_RACE;    
    if(FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
    {
        return EFAILED;
    }   

    ai.wText     = IDS_EDIT_BRITHDAY;
    ai.wItemID   = IDS_EDIT_BRITHDAY;
    ai.wImage    = IDB_SPORT_RACE;
            
    if(FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
    {
        return EFAILED;
    } 
 

    ai.wText       = IDS_EDIT_GENDER;
    ai.wItemID   = IDS_EDIT_GENDER;
    ai.wImage    = IDB_SPORT_RACE;
            
    if(FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
    {
        return EFAILED;
    }  
      
    return SUCCESS;
}// CContApp_BuildAddMenuMenu


#endif
