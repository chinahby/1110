#ifndef _SPORTSAPP_OPERATE_DATA_C_
#define _SPORTSAPP_OPERATE_DATA_C_
/*********************************************************************************

模块名       : <SportApplication>
文件名       : <SportSAppOperateData.c>
相关文件     : 
文件实现功能 : Sports Application
作者         : 
版本         : 1.0
--------------------------------------------------------------------------------
备注         : 
--------------------------------------------------------------------------------
修改记录 : 

******************************************************************************/
#include "SportSApp_priv.h"
#include "SportSAppOperateData.h"
#include "MusicPlayer.h"

#ifdef FEATURE_SPORTS_APP
extern Sport_Appointment pSportAppt;
#endif
/*=============================================================================
FUNCTION: Sports_SaveCurrentActiveUser

DESCRIPTION:保存当前激活的用户

PARAMETERS:
   *pme:

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SportsApp_SaveCurrentActiveUser(CSportsMenu *pMe)
{
	/*Save User Info*/
}

/*=============================================================================
FUNCTION: Sports_GetCurrentActiveUser

DESCRIPTION:获取当前用户

PARAMETERS:
   *pme:

RETURN VALUE:
   ProFileUser

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
ProFileUser SportsApp_GetCurrentActiveUser(CSportsMenu *pMe)
{
	/*Get User Info*/
	if(pMe->m_pSportsAppMenu.m_bTempEntryView == TRUE)
	{
		return  pMe->m_pSportsAppMenu.m_nTempSelProFile;

	}
	else
	{
		return  pMe->m_pSportsAppMenu.m_nSelProFile;
	}


}

/*=============================================================================
FUNCTION: SportsApp_GetAllCurrentActiveData

DESCRIPTION:保存所有用户的信息

PARAMETERS:
   *pme:

RETURN VALUE:
   ProFileUser

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/

CSportSAppData SportsApp_GetAllCurrentActiveData(CSportsMenu *pMe)
{
	ProFileUser active_user_num;
	active_user_num = SportsApp_GetCurrentActiveUser(pMe);
	
	return pMe->m_pSportsAppMenu.m_nSportSAppData[active_user_num];

}

/*=============================================================================
FUNCTION: SportsApp_SaveAllCurrentActiveData

DESCRIPTION:保存所有用户的信息

PARAMETERS:
   *pme:

RETURN VALUE:
   ProFileUser

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SportsApp_SaveAllCurrentActiveData(CSportsMenu *pMe)
{
	ProFileUser active_user;
        double lastCal = 0.0;
        double accCal = 0.0;
	active_user = SportsApp_GetCurrentActiveUser(pMe);

	pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nSportendtime = 
		pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nSportendtime;

	ITIMECTL_GetTimeString(pMe->m_pSportsAppMenu.m_pSAppTime, 
		pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nSportendtime,
		pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_dwDispTime,
		sizeof(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_dwDispTime),
		GTS_MSECS);
	
	SportsApp_SetPedometerCalorie(pMe);
	
    if(pMe->FromScheduleFlag == TRUE)
    {
        AECHAR m_pPlan[SPORT_EDIT_MAX_LEN];
#ifdef FEATURE_SPORTS_APP
        if(pSportAppt.runOrwalk == FALSE)
        {
            
        	(void) ISHELL_LoadResString(pMe->m_pShell,
        					AEE_APPSSPORTSMENU_RES_FILE,
        					IDS_PED_WALK,
        					m_pPlan,
        					sizeof(m_pPlan));
            WSTRCPY(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_pPlan,m_pPlan);
        }
        else
#endif
        {

        	(void) ISHELL_LoadResString(pMe->m_pShell,
        					AEE_APPSSPORTSMENU_RES_FILE,
        					IDS_PED_RUN,
        					m_pPlan,
        					sizeof(m_pPlan));
            WSTRCPY(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_pPlan,m_pPlan);            
        }
    }
//Compare Last Record and Accumulate Record

      /*modify accumulate Record data justify*/
     accCal = WSTRTOFLOAT(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_nCalorie);   
     lastCal = WSTRTOFLOAT(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nCalorie); 

       if(accCal <= lastCal)
	//if(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_nSportendtime <= 
	//	pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nSportendtime)
	{
		pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_nSportendtime = 
			pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nSportendtime;

        WSTRCPY(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_pPlan,
            pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_pPlan);
		WSTRCPY(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_nDistance ,
			pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nDistance);

		WSTRCPY(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_nSpeed ,
			pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nSpeed);
		
		pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_nStep = 
			pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nStep;

		/*Calorie*/
		WSTRCPY(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_nCalorie ,
			pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nCalorie);

		ITIMECTL_GetTimeString(pMe->m_pSportsAppMenu.m_pSAppTime, 
			pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_nSportendtime,
			pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_dwDispTime,
			sizeof(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_dwDispTime),
			GTS_MSECS);
		
	}
//End Compare Last Record and Accumulate Record
                //Save Data
                /*Save Sport Data form File System*/
		SportSApp_SaveSportData(pMe);
                

}

/*=============================================================================
FUNCTION: SportsApp_ClearAllCurrentActiveData

DESCRIPTION:清除被激活用户的纪录并保存

PARAMETERS:
   *pme:

RETURN VALUE:
   ProFileUser

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SportsApp_ClearAllCurrentActiveData(CSportsMenu *pMe)
{
	ProFileUser active_user;
      if(pMe->m_pSportsAppMenu.m_nEntryDialogState == ENTRY_PERS)
        {
            if(pMe->m_pSportsAppMenu.m_nTempSelProFile != USER_NONE)
            {
            	pMe->m_pSportsAppMenu.m_bTempEntryView = TRUE;
            }
            else
            {
            	pMe->m_pSportsAppMenu.m_bTempEntryView = FALSE;
            } 
        }
	active_user = SportsApp_GetCurrentActiveUser(pMe);
	/*Clear Pedometer Last Record Data */
	MEMSET(&pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData,0,
		sizeof(CSportSPedoMeterData));
	/*Clear Pedometer Accumulate Record Data */
	MEMSET(&pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord,0,
		sizeof(CSportSPedoMeterData));
	/*Save Data */
        /*Save Sport Data from File System*/
	SportSApp_SaveSportData(pMe);
	
    if(pMe->m_pSportsAppMenu.m_nEntryDialogState == ENTRY_PERS)
    {
	pMe->m_pSportsAppMenu.m_bTempEntryView = FALSE;
    }

}

/*=============================================================================
FUNCTION: SportsApp_GetPedometerData

DESCRIPTION:保存所有用户的信息

PARAMETERS:
   *pme:

RETURN VALUE:
   CSportSPedoMeterData

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
CSportSPedoMeterData SportsApp_GetPedometerData(CSportsMenu *pMe)
{
	CSportSAppData m_nSportAppData;
	m_nSportAppData = SportsApp_GetAllCurrentActiveData(pMe);

	return m_nSportAppData.m_nSportMeterData;
}

/*=============================================================================
FUNCTION: SportsApp_GetUnitData

DESCRIPTION:保存所有用户的信息

PARAMETERS:
   *pme:

RETURN VALUE:
   CSportSAppUintData

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
CSportSAppUintData SportsApp_GetUnitData(CSportsMenu *pMe)
{
	CSportSAppData m_nSportAppData;
	m_nSportAppData = SportsApp_GetAllCurrentActiveData(pMe);

	return m_nSportAppData.m_nSportUintData;
}

/*=============================================================================
FUNCTION: SportsApp_GetAccRecordData

DESCRIPTION:保存所有用户的信息

PARAMETERS:
   *pme:

RETURN VALUE:
   CSportSPedoMeterData

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
CSportSPedoMeterData SportsApp_GetAccRecordData(CSportsMenu *pMe)
{
	CSportSAppData m_nSportAppData;
	m_nSportAppData = SportsApp_GetAllCurrentActiveData(pMe);

	return m_nSportAppData.m_nAccRecord;	
}

/*=============================================================================
FUNCTION: SportsApp_DisplayAccRecordMenu

DESCRIPTION:显示最佳纪录菜单项

PARAMETERS:
   *pme:

RETURN VALUE:
   CSportSPedoMeterData

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SportsApp_DisplayAccRecordMenu(CSportsMenu *pMe,IMenuCtl *pActiveMenu)
{
	ProFileUser active_user; 
	uint16 buffer_len;
    	AECHAR text[20] = {0};
	char fmt_str[30] = {0};		
	char name_str[30] = {0};
	AECHAR name_wstr[20] = {0};
	AECHAR wszTitle[20] = {0};
	char *PedViewData[] = {
					"Plan:            ",
					"Time:           ",
					"Distance(m):      ",
					"Speed(km/h):     ",
					"Step:              ",
					"Calorie(kcal):      ",
					"None"
					};

	if(pMe->m_pSportsAppMenu.m_nTempSelProFile != USER_NONE)
	{
		pMe->m_pSportsAppMenu.m_bTempEntryView = TRUE;
	}
	else
	{
		pMe->m_pSportsAppMenu.m_bTempEntryView = FALSE;
	}

	active_user = SportsApp_GetCurrentActiveUser(pMe);
	SPORTS_ERR("%x, %x ,%x,HandleMainDialogEvent",eCode,wParam,dwParam);
        /*Get SportData from File System*/
        SportSApp_LoadSportData(pMe);
    /*menu item 1 Plan*/
    (void) ISHELL_LoadResString(pMe->m_pShell,
                    AEE_APPSSPORTSMENU_RES_FILE,
                    IDS_REC_PLAN,
                    wszTitle,
                    sizeof(wszTitle));  
    buffer_len = WSTRLEN(wszTitle);

	if(WSTRLEN(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_pPlan) == 0)
	{
	    AECHAR m_wszTitle[20] = {0};
    	(void) ISHELL_LoadResString(pMe->m_pShell,
				AEE_APPSSPORTSMENU_RES_FILE,
				IDS_REC_CONT,
				m_wszTitle,
				sizeof(m_wszTitle));

		SNPRINTF(name_str,sizeof(name_str), "          ");
		STRTOWSTR(name_str, name_wstr, sizeof(name_wstr));
       
	    WSTRCAT(wszTitle + buffer_len ,name_wstr);
        WSTRCAT(wszTitle,m_wszTitle);

	}
    else
    {
        SNPRINTF(name_str,sizeof(name_str), "          ");
        STRTOWSTR(name_str, name_wstr, sizeof(name_wstr));
        WSTRCAT(wszTitle + buffer_len,name_wstr);
    	WSTRCAT(wszTitle,pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_pPlan);
	}
	IMENUCTL_AddItem( pActiveMenu, 0, 0, 0, wszTitle, 0);
	
	/*menu item 2 Time*/
	MEMSET(wszTitle,0,sizeof(wszTitle));
	(void) ISHELL_LoadResString(pMe->m_pShell,
					AEE_APPSSPORTSMENU_RES_FILE,
					IDS_REC_TIME,
					wszTitle,
					sizeof(wszTitle));	
	
        SNPRINTF(name_str,sizeof(name_str), "      ");
        STRTOWSTR(name_str, name_wstr, sizeof(name_wstr));
        buffer_len = WSTRLEN(name_wstr);
		if(WSTRLEN(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_dwDispTime) == 0)
		{
			AECHAR aedata[16] = {'0'};
			WSTRCAT(name_wstr + buffer_len,aedata);
		}
		else
		{
        WSTRCAT(name_wstr + buffer_len,pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_dwDispTime);
		}

	buffer_len = WSTRLEN(wszTitle);
	 WSTRCAT(wszTitle + buffer_len,name_wstr);
	IMENUCTL_AddItem(pActiveMenu, 0, 0, 1,wszTitle, 0);

	/*menu item 3 Distance*/
	MEMSET(wszTitle,0,sizeof(wszTitle));

	(void) ISHELL_LoadResString(pMe->m_pShell,
					AEE_APPSSPORTSMENU_RES_FILE,
					IDS_REC_DISTANCE,
					wszTitle,
					sizeof(wszTitle));	
    
       SNPRINTF(name_str,sizeof(name_str), "   ");
       STRTOWSTR(name_str, name_wstr, sizeof(name_wstr));
       buffer_len = WSTRLEN(name_wstr);
		if(WSTRLEN(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_nDistance) == 0)
		{
			AECHAR aedata[16] = {'0'};
			WSTRCAT(name_wstr + buffer_len,aedata);
		}
		else
		{
       WSTRCAT(name_wstr + buffer_len,pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_nDistance);
		}
	buffer_len = WSTRLEN(wszTitle);
       WSTRCAT(wszTitle + buffer_len,name_wstr);

	IMENUCTL_AddItem( pActiveMenu, 0, 0, 2, wszTitle, 0);

	/*menu item 4 Speed*/
	MEMSET(wszTitle,0,sizeof(wszTitle));
        MEMSET(name_str,0,sizeof(name_str));
        MEMSET(name_wstr,0,sizeof(name_wstr));
	(void) ISHELL_LoadResString(pMe->m_pShell,
					AEE_APPSSPORTSMENU_RES_FILE,
					IDS_REC_SPEED,
					wszTitle,
					sizeof(wszTitle));	
    
        SNPRINTF(name_str,sizeof(name_str), "   ");
        STRTOWSTR(name_str, name_wstr, sizeof(name_wstr));
        buffer_len = WSTRLEN(name_wstr);
		if(WSTRLEN(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_nSpeed) == 0)
		{
			AECHAR aedata[16] = {'0'};
			WSTRCAT(name_wstr + buffer_len,aedata);
		}
		else
		{
        WSTRCAT(name_wstr + buffer_len,pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_nSpeed);
		}
	buffer_len = WSTRLEN(wszTitle);
        WSTRCAT(wszTitle + buffer_len,name_wstr);

	IMENUCTL_AddItem( pActiveMenu, 0, 0, 3, wszTitle, 0);
	
	/*menu item 5 step*/
	MEMSET(fmt_str,0,sizeof(fmt_str));
	MEMSET(wszTitle,0,sizeof(wszTitle));
	(void) ISHELL_LoadResString(pMe->m_pShell,
					AEE_APPSSPORTSMENU_RES_FILE,
					IDS_REC_STEP,
					wszTitle,
					sizeof(wszTitle));		
	SNPRINTF(fmt_str,sizeof(fmt_str), "    %d",
		pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_nStep);
	STRTOWSTR(fmt_str, text, sizeof(text));
	buffer_len = WSTRLEN(wszTitle);
	WSTRCAT(wszTitle + buffer_len,text);
	IMENUCTL_AddItem( pActiveMenu, 0, 0, 4, wszTitle, 0);

	/*menu item 6 Calorie*/
	MEMSET(wszTitle,0,sizeof(wszTitle));

	(void) ISHELL_LoadResString(pMe->m_pShell,
					AEE_APPSSPORTSMENU_RES_FILE,
					IDS_REC_CAL,
					wszTitle,
					sizeof(wszTitle));	

        SNPRINTF(name_str,sizeof(name_str), "   ");
        STRTOWSTR(name_str, name_wstr, sizeof(name_wstr));
        buffer_len = WSTRLEN(name_wstr);
		if(WSTRLEN(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_nCalorie) == 0)
		{
			AECHAR aedata[16] = {'0'};
			WSTRCAT(name_wstr + buffer_len,aedata);
		}
		else
		{
        WSTRCAT(name_wstr + buffer_len,pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_nCalorie);
		}	
	buffer_len = WSTRLEN(wszTitle);
        WSTRCAT(wszTitle + buffer_len,name_wstr);

	IMENUCTL_AddItem( pActiveMenu, 0, 0, 5, wszTitle, 0);

	/*menu item Property and Bottom bar*/
    IMENUCTL_SetOemProperties(pActiveMenu,OEMMP_USE_MENU_STYLE);
        IMENUCTL_SetProperties(pActiveMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
	IMENUCTL_SetBottomBarType( pActiveMenu, BTBAR_BACK);
	/*Rest Record Data*/
	if(pMe->m_pSportsAppMenu.m_nTempSelProFile != USER_NONE)
	{
		pMe->m_pSportsAppMenu.m_bTempEntryView = FALSE;
		pMe->m_pSportsAppMenu.m_nTempSelProFile = USER_NONE;
	}


}

/*=============================================================================
FUNCTION: SportsApp_DisplayLastRecordMenu

DESCRIPTION:显示最佳纪录菜单项

PARAMETERS:
   *pme:

RETURN VALUE:
   CSportSPedoMeterData

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SportsApp_DisplayLastRecordMenu(CSportsMenu *pMe,IMenuCtl *pActiveMenu)
{
	ProFileUser active_user; 
	uint16 buffer_len;
	AECHAR text[20] = {0};
	char fmt_str[30] = {0}; 	
	char name_str[30] = {0};
	AECHAR name_wstr[20] = {0};
	AECHAR wszTitle[20] = {0};
	char *PedViewData[] = {
					"Plan:            ",
					"Time:           ",
					"Distance(m):      ",
					"Speed(km/h):     ",
					"Step:              ",
					"Calorie(kcal):      ",
					"None"
					};

	if(pMe->m_pSportsAppMenu.m_nTempSelProFile != USER_NONE)
	{
		pMe->m_pSportsAppMenu.m_bTempEntryView = TRUE;
	}
	else
	{
		pMe->m_pSportsAppMenu.m_bTempEntryView = FALSE;
	}

	active_user = SportsApp_GetCurrentActiveUser(pMe);

	//Read SportSAppData
	SportSApp_LoadSportData(pMe);

	/*menu item 1 Plan*/
	(void) ISHELL_LoadResString(pMe->m_pShell,
					AEE_APPSSPORTSMENU_RES_FILE,
					IDS_REC_PLAN,
					wszTitle,
					sizeof(wszTitle));	
	buffer_len = WSTRLEN(wszTitle);
    
	if(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_pPlan[0] == 0)
	{
	    AECHAR m_wszTitle[20] = {0};
    	(void) ISHELL_LoadResString(pMe->m_pShell,
				AEE_APPSSPORTSMENU_RES_FILE,
				IDS_REC_CONT,
				m_wszTitle,
				sizeof(m_wszTitle));
		SNPRINTF(name_str,sizeof(name_str), "          ");
		STRTOWSTR(name_str, name_wstr, sizeof(name_wstr));   
	    WSTRCAT(wszTitle + buffer_len ,name_wstr);
        WSTRCAT(wszTitle,m_wszTitle);
	}
    else
    {
        SNPRINTF(name_str,sizeof(name_str), "          ");
        STRTOWSTR(name_str, name_wstr, sizeof(name_wstr));
        WSTRCAT(wszTitle + buffer_len,name_wstr);
    	WSTRCAT(wszTitle,pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_pPlan);
    }
	IMENUCTL_AddItem( pActiveMenu, 0, 0, 0, wszTitle, 0);
	
	/*menu item 2 Time*/
	MEMSET(wszTitle,0,sizeof(wszTitle));
	(void) ISHELL_LoadResString(pMe->m_pShell,
					AEE_APPSSPORTSMENU_RES_FILE,
					IDS_REC_TIME,
					wszTitle,
					sizeof(wszTitle));	
	
        SNPRINTF(name_str,sizeof(name_str), "      ");
        STRTOWSTR(name_str, name_wstr, sizeof(name_wstr));
        buffer_len = WSTRLEN(name_wstr);
		if(WSTRLEN(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_dwDispTime) == 0)
		{
			AECHAR aedata[16] = {'0'};
			WSTRCAT(name_wstr + buffer_len,aedata);
		}
		else
		{
        WSTRCAT(name_wstr + buffer_len,pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_dwDispTime);
		}
	buffer_len = WSTRLEN(wszTitle);
	 WSTRCAT(wszTitle + buffer_len,name_wstr);
	IMENUCTL_AddItem(pActiveMenu, 0, 0, 1,wszTitle, 0);


	/*menu item 3 Distance*/
	MEMSET(wszTitle,0,sizeof(wszTitle));
	
	(void) ISHELL_LoadResString(pMe->m_pShell,
					AEE_APPSSPORTSMENU_RES_FILE,
					IDS_REC_DISTANCE,
					wszTitle,
					sizeof(wszTitle));	
    
       SNPRINTF(name_str,sizeof(name_str), "   ");
       STRTOWSTR(name_str, name_wstr, sizeof(name_wstr));
       buffer_len = WSTRLEN(name_wstr);
		if(WSTRLEN(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_nDistance) == 0)
		{
			AECHAR aedata[16] = {'0'};
			WSTRCAT(name_wstr + buffer_len,aedata);
		}
		else
		{
       WSTRCAT(name_wstr + buffer_len,pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nDistance);
		}
	buffer_len = WSTRLEN(wszTitle);
       WSTRCAT(wszTitle + buffer_len,name_wstr);

	IMENUCTL_AddItem( pActiveMenu, 0, 0, 2, wszTitle, 0);
		/*menu item 4 Speed*/
	MEMSET(wszTitle,0,sizeof(wszTitle));
        MEMSET(name_str,0,sizeof(name_str));
        MEMSET(name_wstr,0,sizeof(name_wstr));
	(void) ISHELL_LoadResString(pMe->m_pShell,
					AEE_APPSSPORTSMENU_RES_FILE,
					IDS_REC_SPEED,
					wszTitle,
					sizeof(wszTitle));	
    
        SNPRINTF(name_str,sizeof(name_str), "   ");
        STRTOWSTR(name_str, name_wstr, sizeof(name_wstr));
        buffer_len = WSTRLEN(name_wstr);
		if(WSTRLEN(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_nSpeed) == 0)
		{
			AECHAR aedata[16] = {'0'};
			WSTRCAT(name_wstr + buffer_len,aedata);
		}
		else
		{
        WSTRCAT(name_wstr + buffer_len,pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nSpeed);
		}
	buffer_len = WSTRLEN(wszTitle);
        WSTRCAT(wszTitle + buffer_len,name_wstr);

	IMENUCTL_AddItem( pActiveMenu, 0, 0, 3, wszTitle, 0);

	
	/*menu item 5 step*/

	MEMSET(fmt_str,0,sizeof(fmt_str));
	MEMSET(wszTitle,0,sizeof(wszTitle));
	(void) ISHELL_LoadResString(pMe->m_pShell,
					AEE_APPSSPORTSMENU_RES_FILE,
					IDS_REC_STEP,
					wszTitle,
					sizeof(wszTitle));		
	SNPRINTF(fmt_str,sizeof(fmt_str), "    %d",
		pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nStep);
	STRTOWSTR(fmt_str, text, sizeof(text));
	buffer_len = WSTRLEN(wszTitle);
	WSTRCAT(wszTitle + buffer_len,text);
	IMENUCTL_AddItem( pActiveMenu, 0, 0, 4, wszTitle, 0);

	/*menu item 6 Calorie*/
	MEMSET(wszTitle,0,sizeof(wszTitle));

	(void) ISHELL_LoadResString(pMe->m_pShell,
					AEE_APPSSPORTSMENU_RES_FILE,
					IDS_REC_CAL,
					wszTitle,
					sizeof(wszTitle));	
    
        SNPRINTF(name_str,sizeof(name_str), "   ");
        STRTOWSTR(name_str, name_wstr, sizeof(name_wstr));
        buffer_len = WSTRLEN(name_wstr);
		if(WSTRLEN(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nAccRecord.m_nCalorie) == 0)
		{
			AECHAR aedata[16] = {'0'};
			WSTRCAT(name_wstr + buffer_len,aedata);
		}
		else
		{
        WSTRCAT(name_wstr + buffer_len,pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nCalorie);
		}
	buffer_len = WSTRLEN(wszTitle);
        WSTRCAT(wszTitle + buffer_len,name_wstr);

	IMENUCTL_AddItem( pActiveMenu, 0, 0, 5, wszTitle, 0);

	/*menu item Property and Bottom bar*/
    IMENUCTL_SetOemProperties(pActiveMenu,OEMMP_USE_MENU_STYLE);
	IMENUCTL_SetProperties(pActiveMenu, MP_UNDERLINE_TITLE|MP_WRAPSCROLL|MP_BIND_ITEM_TO_NUMBER_KEY);
	IMENUCTL_SetBottomBarType( pActiveMenu, BTBAR_BACK);
	
	if(pMe->m_pSportsAppMenu.m_nTempSelProFile != USER_NONE)
	{
		pMe->m_pSportsAppMenu.m_bTempEntryView = FALSE;
		pMe->m_pSportsAppMenu.m_nTempSelProFile = USER_NONE;
	}


}


/*=============================================================================
FUNCTION: SportSApp_FloatToWStr

DESCRIPTION:将浮点型数转换成宽字符

PARAMETERS:
   *pme:

RETURN VALUE:
   CSportSPedoMeterData

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean SportSApp_FloatToWStr(double v, AECHAR * pdest, int nSize)
{
	char szBuff[32];
	AECHAR ch, *psz, *pszLastZero, *pszSrc, *pszDst;
	boolean bDot;

	if (!pdest || nSize <= 0)
		return FALSE;

	MEMSET( pdest, 0, nSize);
	if( v == 0)
	{
		pdest[0] = '0';
		return FALSE;
	}


	(void)sprintf( szBuff, "%12.10f", v);
	(void)STRTOWSTR(szBuff, pdest, nSize);

	// Clip leading spaces...
	for (pszDst = pszSrc = pdest; ( ch = *pszSrc) == ' '; pszSrc++) 
		;

	if (pszSrc != pszDst)
	{
		while (1)
		{ //lint !e716
			ch = *pszSrc;
			*pszDst = ch;
			if (!ch)
			{
				break;
			}
			pszDst++;
			pszSrc++;
		}
	}

	// Clip trailing zeros...
	for (bDot = FALSE, pszLastZero = NULL, psz = pdest; (ch = *psz) != 0; psz++)
	{

		if (ch == (AECHAR)'.')
		{
			bDot = TRUE;
		}
		else
		{
			if (bDot)
			{
				if (ch == (AECHAR)'0')
				{
					if (!pszLastZero)
					{
						pszLastZero = psz;
					}
				}
				else
					pszLastZero = NULL;
			}
		}
	}

	// If the trailing zero followed the decimal point, clip them both...
	if (pszLastZero)
	{
		if (pszLastZero[-1] == (AECHAR)'.')
		{
			pszLastZero--;
		}
		*pszLastZero = 0;
	}

	return (TRUE);
}

/*=============================================================================
FUNCTION: SportsApp_SetPedometerSpeeds

DESCRIPTION:显示最佳纪录菜单项

PARAMETERS:
   *pme:

RETURN VALUE:
   CSportSPedoMeterData

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SportsApp_SetPedometerSpeeds(CSportsMenu *pMe,int32 nStep,uint32 active_index)
{
	double cur_speed ;
	int32 total_time = 0; 
	AECHAR m_nSpeed[5] = {0};
	int16 m_StepValue;
	
	if(pMe->m_pSportsAppMenu.m_nSportSAppData[active_index].m_nSportMeterData.m_bPedRunOrWalkState == FALSE)
	{
		m_StepValue = pMe->m_pSportsAppMenu.m_nSportSAppData[active_index].m_nSportUintData.m_nWalkstep;
	}
	else
	{
		m_StepValue = pMe->m_pSportsAppMenu.m_nSportSAppData[active_index].m_nSportUintData.m_nRunstep;
	}

	total_time = ITIMECTL_GetTime(pMe->m_pSportsAppMenu.m_pSAppTime);
	cur_speed = (double)(nStep*m_StepValue*10)/((double)total_time);
	
	/*float to wstr*/
	SportSApp_FloatToWStr(cur_speed,m_nSpeed,sizeof(m_nSpeed));
	WSTRCPY(pMe->m_pSportsAppMenu.m_nSportSAppData[active_index].m_nSportMeterData.m_nSpeed,m_nSpeed);
}

/*=============================================================================
FUNCTION: SportsApp_SetPedometerDistance

DESCRIPTION:显示最佳纪录菜单项

PARAMETERS:
   *pme:

RETURN VALUE:
   CSportSPedoMeterData

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SportsApp_SetPedometerDistance(CSportsMenu *pMe,int32 nStep,uint32 active_index)
{
	int16 m_StepValue;
	AECHAR m_nDistance[10] = {0};
	double total_distance = 0.0;
	
	if(pMe->m_pSportsAppMenu.m_nSportSAppData[active_index].m_nSportMeterData.m_bPedRunOrWalkState == FALSE)
	{
		m_StepValue = pMe->m_pSportsAppMenu.m_nSportSAppData[active_index].m_nSportUintData.m_nWalkstep;
	}
	else
	{
		m_StepValue = pMe->m_pSportsAppMenu.m_nSportSAppData[active_index].m_nSportUintData.m_nRunstep;
	}
	
	total_distance = (double)(nStep*m_StepValue)/100;
	SportSApp_FloatToWStr(total_distance,m_nDistance,sizeof(m_nDistance));
	WSTRCPY(pMe->m_pSportsAppMenu.m_nSportSAppData[active_index].m_nSportMeterData.m_nDistance,m_nDistance);

}

/*=============================================================================
FUNCTION: SportsApp_SetPedometerCalorie

DESCRIPTION:显示最佳纪录菜单项

PARAMETERS:
   *pme:

RETURN VALUE:
   CSportSPedoMeterData

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SportsApp_SetPedometerCalorie(CSportsMenu *pMe)
{
	ProFileUser active_user; 
	double time;
	double speed;
	double curr_speed;
	double totalcalorie;
	double modulus;
	AECHAR Wstr_data[10] = {0};
	active_user = SportsApp_GetCurrentActiveUser(pMe);

	time = pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nSportendtime;
	
	speed = WSTRTOFLOAT(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nSpeed);
	
	curr_speed = (double)(speed*60);

	if(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportUintData.m_bGender == FALSE)
	{
		modulus = 1;
	}
	else
	{
		modulus =  0.85;
	}
	DBGPRINTF("speed %d",time);
	DBGPRINTF("Calorie speed %s",pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nSpeed);
	/*Current Speed */
	if(0 < curr_speed <= 20)
	{
		totalcalorie = (double)(SPORT_WALK_CALORIE*time)*modulus/3600000;
		SportSApp_FloatToWStr(totalcalorie,
			Wstr_data,
			sizeof(Wstr_data));
	}
	else if(20<curr_speed<= 70)
	{
		totalcalorie = (double)(SPORT_LOW_STEP_CALORIE*time)*modulus/3600000;
		SportSApp_FloatToWStr(totalcalorie,
			Wstr_data,
			sizeof(Wstr_data));		
	}
	else if(70<curr_speed<= 110)
	{
		totalcalorie = (double)(SPORT_MID_STEP_CALORIE*time)*modulus/3600000;
		SportSApp_FloatToWStr(totalcalorie,
			Wstr_data,
			sizeof(Wstr_data));		
	}
	else if(110<curr_speed<= 130)
	{
		totalcalorie = (double)(SPORT_QUICK_STEP_CALOIRE*time)*modulus/3600000;
		SportSApp_FloatToWStr(totalcalorie,
			Wstr_data,
			sizeof(Wstr_data));		
	}
	else if(130<curr_speed<= 165)
	{
		totalcalorie = (double)(SPORT_LOW_RUN_CALORIE*time)*modulus/3600000;
		SportSApp_FloatToWStr(totalcalorie,
			Wstr_data,
			sizeof(Wstr_data));		
	}
	else if(165<curr_speed)
	{
		totalcalorie = (double)(SPORT_QUICK_RUN_CALORIE*time)*modulus/3600000;
		SportSApp_FloatToWStr(totalcalorie,
			Wstr_data,
			sizeof(Wstr_data));	
	}
	
	SportsApp_ExtraDecimal(pMe->m_pSportsAppMenu.m_nSportSAppData[active_user].m_nSportMeterData.m_nCalorie
						,Wstr_data);
	
}

/*=============================================================================
FUNCTION: SportsApp_SetPersonalItemName

DESCRIPTION:设置Personal 中用户的姓名

PARAMETERS:
   *pme:

RETURN VALUE:
   ProFileUser

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SportsApp_SetPersonalItemName(CSportsMenu *pMe,IMenuCtl *pMenu)
{
	
	int Itemindex;
        AECHAR *pBuffer = NULL;
	uint16 m_aItemName[]={
				IDS_PERS_ONE,
				IDS_PERS_TWO,
				IDS_PERS_THREE
			};
	
	for(Itemindex = 0;Itemindex < 3;Itemindex++)
	{
		if(WSTRLEN(pMe->m_pSportsAppMenu.m_nSportSAppData[Itemindex].m_nSportUintData.m_pName) != 0)
		{
			pBuffer = pMe->m_pSportsAppMenu.m_nSportSAppData[Itemindex].m_nSportUintData.m_pName;
	        	IMENUCTL_SetItemText(pMenu, m_aItemName[Itemindex], NULL, 0,pBuffer);
		}	
	}

}

/*=============================================================================
FUNCTION: SportSApp_LoadSportData

DESCRIPTION:Load Sport data From to file System

PARAMETERS:
   *pme:

RETURN VALUE:
   ProFileUser

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int SportSApp_LoadSportData(CSportsMenu *pMe)
{
	IFile     *pFile;
	int32	   bytesRead;

	pFile = IFILEMGR_OpenFile(pMe->m_pSportsAppMenu.m_pFileMgr, SPORT_EFS_FILE, _OFM_READ);

	if(pFile != NULL)
	{
		if(SUCCESS != IFILE_Seek(pFile, _SEEK_START, 0))
		{
		    (void)IFILE_Release(pFile);
		    return EFAILED;
		}
		bytesRead = IFILE_Read(pFile, (void*)&pMe->m_pSportsAppMenu.m_nSportSAppData, 
						sizeof(pMe->m_pSportsAppMenu.m_nSportSAppData));

            if (bytesRead != (int32)sizeof(pMe->m_pSportsAppMenu.m_nSportSAppData))
            {
			IFILE_Release(pFile);		
                	return EFAILED;
            }	
		IFILE_Release(pFile);
		return SUCCESS;
	}
    return EFAILED;
}

/*=============================================================================
FUNCTION: SportSApp_SaveSportData

DESCRIPTION:Save Sport data to File System

PARAMETERS:
   *pme:

RETURN VALUE:
   ProFileUser

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int SportSApp_SaveSportData(CSportsMenu *pMe)
{
	IFile  *pFile;
	uint32  bytesWritten;

	pFile = IFILEMGR_OpenFile(pMe->m_pSportsAppMenu.m_pFileMgr, 
	                     SPORT_EFS_FILE, 
	                     _OFM_READWRITE);

	if (NULL == pFile)
	{
		pFile = IFILEMGR_OpenFile(pMe->m_pSportsAppMenu.m_pFileMgr,
		                    SPORT_EFS_FILE,
		                    _OFM_CREATE);
	}

	(void) IFILE_Truncate(pFile, 0);
	if(SUCCESS != IFILE_Seek(pFile, _SEEK_START, 0))
	{
		(void)IFILE_Release(pFile);
		return EFAILED;
	}
	
	bytesWritten = IFILE_Write(pFile, pMe->m_pSportsAppMenu.m_nSportSAppData,
						sizeof(pMe->m_pSportsAppMenu.m_nSportSAppData));
	
	if (bytesWritten != sizeof(pMe->m_pSportsAppMenu.m_nSportSAppData)) 
	{ 
		IFILE_Release(pFile);
		return EFAILED;
	}
	IFILE_Release(pFile);
	return SUCCESS;
	
	}

/*=============================================================================
FUNCTION: SportSApp_SaveSportData

DESCRIPTION:

PARAMETERS:
   *pme:

RETURN VALUE:
   ProFileUser

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean SportsApp_ExtraDecimal(AECHAR *OutWdata,AECHAR *inWData)
{
	int16 index = 0;
	int16 num = 0;
	int16 wstr_len = 0;
	wstr_len = WSTRLEN(inWData);
	for(index = 0;index < wstr_len;index++)
	{
		if(inWData[index] == 46)
		{
			index = wstr_len - 3;
			OutWdata[num] = inWData[num];
			num++;
		}
		else
		{
			OutWdata[num] = inWData[num];
			num++;			
		}
	}
	return TRUE;
	
}

/*=============================================================================
FUNCTION: SportSApp_Set_Shake_Disable

DESCRIPTION:关掉shake 

PARAMETERS:
   *pme:

RETURN VALUE:
   ProFileUser

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean SportSApp_Set_Shake_Disable(CSportsMenu *pMe)
{	
	dword shake;
	boolean isShakeOn = FALSE;
	(void)ICONFIG_GetItem(pMe->m_pConfig,CFGI_GSENSOR,&shake,sizeof(shake));
	if(shake)
	{
		
		if(shake & OEMNV_SHAKE_MUSIC_MASK)
		{
			isShakeOn = FALSE;										
            /*Set Shake Reset*/
			mmi_g_sensor_process(G_SENSOR_SHAKE_DISABLE);

            mmi_g_sensor_process(G_SENSOR_VIDEO_CIRC_DISABLE); 
		}
        
		else if(shake & OEMNV_SHAKE_FM_RADIO_MASK)
		{
			isShakeOn = FALSE;										
			mmi_g_sensor_process(G_SENSOR_SHAKE_DISABLE);
                        mmi_g_sensor_process(G_SENSOR_VIDEO_CIRC_DISABLE);
		}
		else if(shake & OEMNV_SHAKE_WALLPAPER_MASK)
		{
			isShakeOn = FALSE;										
			mmi_g_sensor_process(G_SENSOR_SHAKE_DISABLE);
                        mmi_g_sensor_process(G_SENSOR_VIDEO_CIRC_DISABLE);			
		
		}
		else if(shake & OEMNV_SHAKE_SNOOZE_ALARM_MASK)
		{
			isShakeOn = FALSE;										
			mmi_g_sensor_process(G_SENSOR_SHAKE_DISABLE);
                        mmi_g_sensor_process(G_SENSOR_VIDEO_CIRC_DISABLE);				
		}
		else if(shake & OEMNV_SHAKE_VIDEO_MASK)
		{
			isShakeOn = FALSE;		
			mmi_g_sensor_process(G_SENSOR_VIDEO_CIRC_DISABLE);
                        mmi_g_sensor_process(G_SENSOR_SHAKE_DISABLE);
			
		}

            (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_SHAKE_MUSIC_CHECK,&isShakeOn,
                            sizeof(isShakeOn));
            (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_SHAKE_FM_RADIO_CHECK,&isShakeOn,
                                    sizeof(isShakeOn));
            (void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_SHAKE_WALLPAPER_CHECK,&isShakeOn,
                                    sizeof(isShakeOn));
            
			(void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_SHAKE_SNOOZE_ALARM_CHECK,&isShakeOn,
									sizeof(isShakeOn));

			(void)ICONFIG_SetItem(pMe->m_pConfig,CFGI_SHAKE_VIDEO_CHECK,&isShakeOn,
									sizeof(isShakeOn)); 

			return TRUE;			
	
	}
	else
	{
		return FALSE;
	}
		

	
}

/*=============================================================================
FUNCTION: SportSApp_Get_Shake_OnOrOff

DESCRIPTION:判断是否已经开了shake

PARAMETERS:
   *pme:

RETURN VALUE:
   ProFileUser

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean SportSApp_Get_Shake_OnOrOff(CSportsMenu *pMe)
{
	dword shake;
	(void)ICONFIG_GetItem(pMe->m_pConfig,CFGI_GSENSOR,&shake,sizeof(shake));

	if(shake)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

/*=============================================================================
FUNCTION: SportSApp_PlayMusicWithSport

DESCRIPTION:判断是否已经开了shake

PARAMETERS:
   *pme:

RETURN VALUE:
   ProFileUser

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SportSApp_PlayMusicWithSport(CSportsMenu *pMe)
{
	IMusicPlayer *pMusicPlayer=NULL;
	
	ISHELL_CreateInstance(pMe->m_pShell,AEECLSID_APP_MUSICPLAYER,
					(void**)&pMusicPlayer);

	IMUSICPLAYER_PedometerCallPlayer(pMusicPlayer);
	(void)IMUSICPLAYER_Release(pMusicPlayer);
	
}

/*=============================================================================
FUNCTION: SportSApp_ClosePlayMusic

DESCRIPTION:当sport停止的时候music 也要关闭

PARAMETERS:
   *pme:

RETURN VALUE:
   ProFileUser

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SportSApp_ClosePlayMusic(CSportsMenu *pMe)
{
	(void) ISHELL_SendEvent( pMe->m_pShell,AEECLSID_APP_MUSICPLAYER,
								EVT_CLOSEAPP,
								0,
								0);
	
}

/*=============================================================================
FUNCTION: SportSApp_PlayMusicOnOrOff

DESCRIPTION:当sport停止的时候music 也要关闭

PARAMETERS:
   *pme:

RETURN VALUE:
   ProFileUser

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean SportSApp_GetPlayMusicOnOrOff(void)
{
	if(GetMp3PlayerStatus() == MP3STATUS_RUNONBACKGROUND)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*=============================================================================
FUNCTION: SportSApp_StartFmRadio

DESCRIPTION:
            start fmRadio
PARAMETERS:
   *pme:

RETURN VALUE:
   ProFileUser

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean SportSApp_StartFmRadio(CSportsMenu *pMe)
{

}

/*=============================================================================
FUNCTION: SportSApp_StopFmRadio

DESCRIPTION:
            start fmRadio
PARAMETERS:
   *pme:

RETURN VALUE:
   ProFileUser

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean SportSApp_StopFmRadio(CSportsMenu *pMe)
{
    IFmRadio *fmRadio = NULL;
    boolean connect = FALSE;

    return ISHELL_SendEvent( pMe->m_pShell, AEECLSID_APP_FMRADIO, EVT_CLOSEAPP, connect, 0);

}

/*=============================================================================
FUNCTION: SportSApp_IsRunOnGround

DESCRIPTION:
            start fmRadio
PARAMETERS:
   *pme:

RETURN VALUE:
   ProFileUser

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean SportSApp_IsRunOnGround(CSportsMenu *pMe)
{
    boolean  isRunOnGround;
    
    ICONFIG_GetItem( pMe->m_pConfig, CFGI_FM_BACKGROUND, &isRunOnGround, sizeof(isRunOnGround));
    
    return isRunOnGround;
}

/*=============================================================================
FUNCTION: Games_isFmBg

DESCRIPTION:
            start fmRadio
PARAMETERS:
   *pme:

RETURN VALUE:
   ProFileUser

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean Games_isFmBg(void)
{
    boolean b_FMBackground = FALSE;
    
    OEM_GetConfig(CFGI_FM_BACKGROUND,&b_FMBackground, sizeof(b_FMBackground));

    if(b_FMBackground)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*=============================================================================
FUNCTION: SportSApp_StartFmRadioEx

DESCRIPTION:
            start fmRadio
PARAMETERS:
   *pme:

RETURN VALUE:
   ProFileUser

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean SportSApp_StartFmRadioEx(CSportsMenu *pMe)
{
    char args[2] = {0};
    int rnt;
    
    args[0] = 'e'; // 'e' meams extra
    
    rnt = ISHELL_StartAppletArgs(pMe->m_pShell, AEECLSID_APP_FMRADIO, args);

    if(SUCCESS != rnt)
    {
        DBGPRINTF("ISHELL_StartAppletArgs AEECLSID_APP_FMRADIO failed",0,0,0);
        return FALSE;
    }

    return TRUE;
    
}
/*=============================================================================
FUNCTION: SportSapp_SwitchFmAndMusic

DESCRIPTION:
            start fmRadio
PARAMETERS:
   *pme:

RETURN VALUE:
   ProFileUser

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean SportSapp_SwitchFmAndMusic(CSportsMenu *pMe)
{
    /*sportsapp set small title icon*/
    IANNUNCIATOR_SetField(pMe->m_pIAnn,ANNUN_FIELD_SPORTS, ANNUN_STATE_SPORTS_OFF);
    if(pMe->m_pSportsAppMenu.m_nMsgTypeFlag == SPORT_RUN_WITH_MUSIC)
    {
        SportSApp_ClosePlayMusic(pMe);

        if(SportSApp_IsRunOnGround(pMe) == TRUE)
        {
            if(SportSApp_StartFmRadioEx(pMe) == FALSE)
            {
                return TRUE;
            }
        }        
        return TRUE;
    }
    else if(pMe->m_pSportsAppMenu.m_nMsgTypeFlag == SPORT_RUN_WITH_FM)
    {
        
        SportSApp_StopFmRadio(pMe);

        /*Music Background Resume*/
        //SportSapp_ResumeMusic(pMe);

        return TRUE;
    }
    else
    {

    }
    
    return FALSE;

}

/*=============================================================================
FUNCTION: SportSapp_PauseMusic

DESCRIPTION:
            start fmRadio
PARAMETERS:
   *pme:

RETURN VALUE:
   ProFileUser

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SportSapp_PauseMusic(CSportsMenu *pMe)
{ 
       if(SportSApp_GetPlayMusicOnOrOff() == TRUE)
       {
                (void) ISHELL_SendEvent(pMe->m_pShell,
                                AEECLSID_APP_MUSICPLAYER,
                                EVT_ALARM,
                                TRUE,
                                0);
       }
}

/*=============================================================================
FUNCTION: SportSapp_ResumeMusic

DESCRIPTION:
            start fmRadio
PARAMETERS:
   *pme:

RETURN VALUE:
   ProFileUser

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SportSapp_ResumeMusic(CSportsMenu *pMe)
{
        if(SportSApp_GetPlayMusicOnOrOff() == TRUE)
        {
                (void) ISHELL_SendEvent(pMe->m_pShell,
                                AEECLSID_APP_MUSICPLAYER,
                                EVT_ALARM,
                                FALSE,
                                0);
        }   
}

#endif
