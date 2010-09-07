/*
FILE:  OEMRTC.c

SERVICES:  OEM functions for supporting RTC Interface

GENERAL DESCRIPTION:
  This file provides the prorotypes of the functions that OEMs must
  provide in order to support the RTC interface.

PUBLIC CLASSES AND STATIC FUNCTIONS:

        Copyright ?1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*=============================================================================
            EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Src/OEMRTC.c#13 $
$DateTime: 2008/01/30 22:24:12 $
$Change: 339355 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
=============================================================================*/

/*===========================================================================

INCLUDE FILES FOR MODULE

===========================================================================*/
#include "Pm.h"
#include "Pmapp_rtc.h"
#include "OEMRTC.h"
#include "AEEStdLib.h"
#include "OEMCFGI.h"
#include "OEMClassIDs.h"
#include "OEMConfig.h"
#include "OEMHeap.h"
#include "AEE_OEMDispatch.h"
#include "AEECM.h"
#include "ui.h"
#include "Task.h"
/*===========================================================================

                       TYPE DECLARATIONs

===========================================================================*/

typedef struct  _time_table
{
    struct _time_table *next;
    uint32             set_time;
    byte               sig;
    time_list_cb       function;
    void *p_data;
}time_table;

typedef struct _time_table_list
{
    time_table *phead;
    int16      count;
}time_table_list;
//#define RTC_DEBUG
//#ifdef RTC_DEBUG
//#define RTC_ERR( str, a, b, c ) ERR( str, a, b, c )
//#else
//#define RTC_ERR( str, a, b, c )
//#endif /*RTC_DEBUG*/
#ifdef FEATURE_OEM_DEBUG
#define RTC_PRINTF  DBGPRINTF
#define RTC_ERR( str, a, b, c ) ERR( str, a, b, c )
#else
#define RTC_PRINTF(format, ...) //ERR(format, code1, code2, code3)
#define RTC_ERR( str, a, b, c )
#endif

/*===========================================================================

FUNCTION PROTOTYPES

===========================================================================*/
static time_table * OEMRTC_Creat_Time_Node(void);
static boolean      OEMRTC_Insert_Node_To_List(time_table *p_node);
static boolean      OEMRTC_Process_Timer_List(void);
static void         OEMRTC_Print_List(void);
static void         OEMRTC_hs_bsp_test_rtc_clr_alarm(word alarm,boolean b_set_2018);
static void         OEMRTC_hs_bsp_test_pmic_rtc_alarm_isr_cb( void);
static void         OEMRTC_hs_bsp_test_rtc_set_alarm_time(void *m_time );
/*static*/ void         OEMRTC_hs_bsp_test_rtc_get_alarm_time(void * m_time);
//#ifdef FEATRUE_AUTO_POWER
//static void         OEMRTC_Process_Auto_Power_Down_Cb(void *p_data);
//#endif //#ifdef FEATRUE_AUTO_POWER
#if 0
static boolean      OEMRTC_Compare_Time(uint32 set_time);
#endif 
static boolean      OEMRTC_Modify_Time_Node(uint32 time ,byte timer_sig, time_list_cb cb, void * p_data);
static void         OEMRTC_hs_bsp_test_rtc_set(void);
/*===========================================================================

LOCAL/STATIC DATA

===========================================================================*/
static time_table_list timer_list = {NULL,0};
static pm_rtc_julian_type pm_rtc_time;
/*==============================================================================

                                 函数定义

==============================================================================*/

#ifdef FEATRUE_AUTO_POWER
//static void OEMRTC_Process_Auto_Power_Down_Cb(void *p_data)
//{
//    IShell *pShell = NULL;
//    pShell = (IShell*)p_data;
//    RTC_ERR("OEMRTC_Process_Auto_Power_Down_Cb",0,0,0);
//    if(pShell)
//    {
//        ISHELL_SendEvent(pShell,AEECLSID_CORE_APP,EVT_AUTO_POWERDOWN,0,0);
//    }
//    return ;
//}
boolean OEMRTC_Process_Auto_Power_Down(IShell *pShell)
{
    Auto_Power_Cfg   m_Cfg;
    uint32 set_time = 60;
    OEM_GetConfig(CFGI_AUTO_POWER_OFF, (void*)&m_Cfg, sizeof(Auto_Power_Cfg));
    if((boolean)m_Cfg.bStateOn)
    {
        uint32 current_time = 0;
        JulianType julian_current_time;
        JulianType julian_set_time;

        if(OEMRTC_Get_Current_Time(&current_time))
        {
            set_time = current_time;
            if (set_time > 0) 
            {
                set_time %= 60;
                set_time = (60 - set_time);
            }             
            if (set_time == 0) 
            {
                set_time = 60;
            }
        GETJULIANDATE(current_time, &julian_current_time);
        /*julian_current_time.wYear = 0;
        julian_current_time.wMonth = 0;
        julian_current_time.wDay = 0;
        julian_current_time.wSecond  = 0;*/
        RTC_ERR("julian_current_time %d %d %d",current_time,julian_current_time.wHour,julian_current_time.wMinute);
        m_Cfg.dwWATime       = m_Cfg.dwWATime /1000;//m_Cfg.dwWATime is ms for 00:00
        /*julian_set_time.wYear = 0;
        julian_set_time.wMonth = 0;
        julian_set_time.wDay = 0;*/
        julian_set_time.wHour    = (uint16)(m_Cfg.dwWATime /3600);//hour
        julian_set_time.wMinute  = (uint16)((m_Cfg.dwWATime - julian_set_time.wHour *3600)/60);
        julian_set_time.wSecond  = 0;
        RTC_ERR("julian_set_time %d %d %d",m_Cfg.dwWATime,julian_set_time.wHour,julian_set_time.wMinute);
        if(julian_current_time.wHour == julian_set_time.wHour 
            && julian_current_time.wMinute == julian_set_time.wMinute )
        {
            ISHELL_SendEvent(pShell,AEECLSID_CORE_APP,EVT_AUTO_POWERDOWN,0,0);
        }
        }
    }    
    RTC_ERR("set time = %d",set_time,0,0);
    AEE_SetSysTimer(set_time * 1000, (PFNNOTIFY)OEMRTC_Process_Auto_Power_Down,(void *)pShell);
    return TRUE;
}

boolean OEMRTC_Process_Auto_Power_On(void)
{
    Auto_Power_Cfg   m_Cfg;

    OEM_GetConfig(CFGI_AUTO_POWER_ON, (void*)&m_Cfg, sizeof(Auto_Power_Cfg));
    OEMRTC_hs_bsp_test_rtc_set();
    if((boolean)m_Cfg.bStateOn)
    {
        uint32 current_time = 0;
        JulianType julian_time;
        uint32 set_time     = 0;
        //OEMRTC_hs_bsp_test_rtc_set();
        if(!OEMRTC_Get_Current_Time(&current_time))
        {
            return FALSE;
        }

        GETJULIANDATE(current_time, &julian_time);
        m_Cfg.dwWATime      = m_Cfg.dwWATime /1000;

        julian_time.wHour   = (uint16)(m_Cfg.dwWATime /3600);//hour
        julian_time.wMinute = (uint16)((m_Cfg.dwWATime - julian_time.wHour *3600)/60);
        julian_time.wSecond = 0;
        set_time            = JULIANTOSECONDS(&julian_time);

        if(set_time < current_time)
        {
            julian_time.wDay ++;
            RTC_ERR("set %d day %d:%d power on",julian_time.wDay,julian_time.wHour,julian_time.wMinute);
            set_time = set_time + 86400;//set to tomorrow
        }

        OEMRTC_Add_Time_To_Timer_List(set_time,OEMRTC_SET_POWER_ON,NULL,NULL);
    }
    else
    {
#if 0
        if(timer_list.phead != NULL)
        {
            if(timer_list.phead->sig == OEMRTC_SET_POWER_ON)// current alarm is auto power on,need clear it
            {
                OEMRTC_hs_bsp_test_rtc_clr_alarm(PM_RTC_ALARM_1);
            }
        }
        OEMRTC_hs_bsp_test_rtc_clr_alarm(PM_RTC_ALARM_1,TRUE);
#endif
        OEMRTC_Del_Time_Node(OEMRTC_SET_POWER_ON);
    }

    OEMRTC_Process_Timer_List();	
    return TRUE;
}
#endif //#ifdef FEATRUE_AUTO_POWER
//jiagng 20080213 add
boolean OEMRTC_Process_PowerDown_Alarm(void)
{
    
    PowerDown_Alarm_Cfg   m_Cfg;
    
    OEM_GetConfig(CFGI_POWERDOWN_ALARM, (void*)&m_Cfg, sizeof(PowerDown_Alarm_Cfg));    
    
    RTC_PRINTF( ";----------------------------------------------",0);
    RTC_PRINTF( ";alarm state: %d", m_Cfg.bStateOn);
	
    if((boolean)m_Cfg.bStateOn)
    {
        uint32 current_time = 0;

        RTC_PRINTF( ";to invoke OEMRTC_hs_bsp_test_rtc_set",0);		
        OEMRTC_hs_bsp_test_rtc_set();
        if(!OEMRTC_Get_Current_Time(&current_time))
        {
            RTC_PRINTF( ";OEMRTC_Get_Current_Time, time is invalid, return",0);
            return FALSE;
        }
        
        {
            JulianType julian_time1;
            GETJULIANDATE(m_Cfg.dwWATime, &julian_time1);
            RTC_ERR("year %d/%d/%d alarm on",julian_time1.wYear,julian_time1.wMonth,julian_time1.wDay);
            RTC_ERR("set %d hour %d:%d alarm on",julian_time1.wHour,julian_time1.wMinute,julian_time1.wSecond);
        }

        if(m_Cfg.dwWATime < current_time)
        {
            m_Cfg.dwWATime = m_Cfg.dwWATime + 86400;//推迟到明天
        }

        OEMRTC_Add_Time_To_Timer_List(m_Cfg.dwWATime,OEMRTC_SET_ALARM_1,NULL,NULL);
    }
    else
    {

        RTC_PRINTF( ";alarm is off, to delete it from rtc list",0);

#if 0
        if(timer_list.phead != NULL)
        {
            if(timer_list.phead->sig == OEMRTC_SET_ALARM_1)// current alarm is auto power on,need clear it
            {
                OEMRTC_hs_bsp_test_rtc_clr_alarm(PM_RTC_ALARM_1,TRUE);
            }
        }
#endif

        OEMRTC_Del_Time_Node(OEMRTC_SET_ALARM_1);
    }

    OEMRTC_Process_Timer_List();	
    return TRUE;    
}
boolean OEMRTC_Add_Time_To_Timer_List(uint32 time ,byte timer_sig, time_list_cb cb, void * p_data)
{
    time_table *node = NULL;
    uint32 current_time = 0;

    if(OEMRTC_Modify_Time_Node( time, timer_sig,cb, p_data))
    {
        RTC_ERR("Found the same node in list,not add",0,0,0);
        return TRUE;
    }

    if(!OEMRTC_Get_Current_Time(&current_time))
    {
        RTC_ERR( "get current time failed", 0, 0, 0);
        return FALSE;
    }
    RTC_ERR("Set_time= %d current_time= %d sig= %d Add_Time_To_List",time,current_time,timer_sig);
    if(time < current_time)
    {
        RTC_ERR( "time %d less than current_time %d", time, current_time, 0);
        return FALSE;
    }

    OEMRTC_Del_Time_Node(timer_sig);
    node = OEMRTC_Creat_Time_Node();

    if(node)
    {
        node->set_time    = time;
        node->sig         = timer_sig;
        node->function    = cb;
        node->p_data      = p_data;
        node->next        = NULL;
        OEMRTC_Insert_Node_To_List(node);
        return TRUE;
    }
    return FALSE;
}

static boolean OEMRTC_Process_Timer_List(void)
{

#ifdef RTC_DEBUG
    uint32 time = 0;
    JulianType current_time;
#endif

	if( timer_list.phead == NULL)
	{
            RTC_PRINTF( ";-------------------------",0);
            RTC_PRINTF( ";OEMRTC_Process_Timer_List",0);
            RTC_PRINTF( ";NULL list, to clear PM_RTC_ALARM_1",0);
		
		OEMRTC_hs_bsp_test_rtc_clr_alarm(PM_RTC_ALARM_1,FALSE);
		return FALSE;
	}
	
    RTC_ERR("sig = %d OEMRTC_Process_Timer_List",timer_list.phead->sig,0,0);

#if 0
    if(OEMRTC_Compare_Time(timer_list.phead->set_time))
    {
        /*The set_time have been set to RTC,so do not process*/
		RTC_ERR( "The set_time have been set to RTC,so do not process", 0, 0, 0);
        return FALSE;
    }
#endif

#ifdef RTC_DEBUG
    if(!OEMRTC_Get_Current_Time(&time))
    {
        RTC_ERR( "get current time failed", 0, 0, 0);
        return FALSE;
    }
    //OEMRTC_Print_List();
    GETJULIANDATE(time, &current_time);
#endif

	RTC_ERR( "list count %d", timer_list.count, 0, 0);
    //process the list
    if(timer_list.count)
    {
        JulianType list_time;
        boolean flag = FALSE;
        if(timer_list.phead->sig == OEMRTC_SET_ALARM_1)
        {
            flag = TRUE;
        }
        OEM_SetConfig(CFGI_ALARM_FLAG, (void*)&flag, sizeof(flag));
        GETJULIANDATE(timer_list.phead->set_time, &list_time);

        RTC_ERR("%d day %d:%d Set_time",list_time.wDay,list_time.wHour,list_time.wMinute);
#ifdef RTC_DEBUG
        RTC_ERR("%d day %d:%d current_time",current_time.wDay,current_time.wHour,current_time.wMinute);
#endif
        OEMRTC_hs_bsp_test_rtc_clr_alarm(PM_RTC_ALARM_1,FALSE);
        //pm_reset_rtc_alarm(PM_RTC_ALARM_1);
        //OEMRTC_hs_bsp_test_rtc_set((void*)&current_time);
        OEMRTC_hs_bsp_test_rtc_set_alarm_time((void*)&list_time);
        OEMRTC_Print_List();
        return TRUE;
    }
    else
    {
        return FALSE;
    }

}

static time_table * OEMRTC_Creat_Time_Node(void)
{
    time_table * temp = NULL;
    temp = (time_table*) sys_malloc(sizeof(time_table));
    if(temp)
    {
        temp->next      = NULL;
        temp->set_time  = 0;
        temp->p_data    = NULL;
        temp->function  = NULL;
        temp->sig       = 0;
    }
    return temp;
}

boolean OEMRTC_Del_Time_Node(byte del_sig)
{
    time_table * p1 = NULL;
    time_table * p2 = NULL;
    RTC_ERR("OEMRTC_Del_Time_Node sig= %d",del_sig,0,0);
    if(timer_list.phead == NULL)
    {
        RTC_ERR("timer_list.phead == NULL",0,0,0);
        return FALSE;
    }

    p1 = timer_list.phead;

    while(p1->sig != del_sig && p1->next != NULL)
    {
        p2 = p1;
        p1 = p1->next;
    }

    if(p1->sig == del_sig) //Ok,find it
    {
        if(p1 == timer_list.phead)
        {
            timer_list.phead = p1->next;
        }
        else
        {
            p2->next = p1->next;
        }
        sys_free(p1);
        timer_list.count --;
        OEMRTC_Print_List();
        return TRUE;
    }
    else
    {
        RTC_ERR("Not find %d Node",del_sig,0,0);
        return FALSE;
    }

}
static void  OEMRTC_hs_bsp_test_rtc_set(void)
{
    static boolean b_set_time = FALSE;
    if(b_set_time == FALSE)
    {
        pm_err_flag_type pmic_err_flag = PM_ERR_FLAG__SUCCESS;
        //pm_rtc_julian_type * time = (pm_rtc_julian_type*)m_time;
        
        uint32 current_time = 0;
		
        current_time = GETTIMESECONDS();
        GETJULIANDATE( current_time, (JulianType*)&pm_rtc_time);

	    RTC_PRINTF( ";-------------------------------------",0);
		RTC_PRINTF( ";OEMRTC_hs_bsp_test_rtc_set",0);
		RTC_PRINTF( ";get time by GETTIMESECONDS: %d.%02d.%02d %02d:%d:%d", pm_rtc_time.year, pm_rtc_time.month, pm_rtc_time.day, pm_rtc_time.hour, pm_rtc_time.minute, pm_rtc_time.second);
		
        if( pm_rtc_time.year < 2008)
        {
			RTC_PRINTF( "; invalid time got by GETTIMESECONDS, to get time from rtc",0);
            OEMRTC_hs_bsp_test_rtc_get();
  			RTC_PRINTF( ";get time from rtc: %d.%02d.%02d %02d:%02d:%02d", pm_rtc_time.year, pm_rtc_time.month, pm_rtc_time.day, pm_rtc_time.hour, pm_rtc_time.minute, pm_rtc_time.second);
			return;
        }

		
        /* Write RTC time back to PMIC to get around RTC-stuck issue */
        pmic_err_flag = pm_rtc_rw_cmd(PM_RTC_SET_CMD, (pm_rtc_julian_type*)(&pm_rtc_time));
	    RTC_PRINTF( ";write time to rtc: %d.%02d.%02d %02d:%02d:%02d", pm_rtc_time.year, pm_rtc_time.month, pm_rtc_time.day, pm_rtc_time.hour, pm_rtc_time.minute, pm_rtc_time.second);
  
        if(pmic_err_flag == PM_ERR_FLAG__SUCCESS)
        {
            b_set_time = TRUE;
        }
    }
    //if (pmic_err_flag != PM_ERR_FLAG__SUCCESS)
    //{
    //    RTC_ERR("%d OEMRTC_hs_bsp_test_rtc_set",pmic_err_flag,0,0);
    //}
    //RTC_ERR("%d OEMRTC_hs_bsp_test_rtc_set",pmic_err_flag,0,0);
    //return pmic_err_flag;
}

//get rtc current time
void * OEMRTC_hs_bsp_test_rtc_get(void)
{
    pm_err_flag_type pmic_err_flag = PM_ERR_FLAG__SUCCESS;
    /* Force display mode to 24 hours */
    pm_set_rtc_display_mode(PM_RTC_24HR_MODE);

    /* Write RTC time back to PMIC to get around RTC-stuck issue */
    pmic_err_flag = pm_rtc_rw_cmd(PM_RTC_GET_CMD, (pm_rtc_julian_type*)(&pm_rtc_time));

    if (pmic_err_flag != PM_ERR_FLAG__SUCCESS)
    {
        /* Get time from RTC before initialization */
        pmic_err_flag |= pm_rtc_rw_cmd(PM_RTC_GET_CMD, (pm_rtc_julian_type*)(&pm_rtc_time));
    }
    return (void*)&pm_rtc_time;
}

static void OEMRTC_hs_bsp_test_rtc_clr_alarm(word alarm,boolean b_set_2018)
{
  pm_err_flag_type pmic_err_flag = PM_ERR_FLAG__SUCCESS;
  pmic_err_flag  |= pm_reset_rtc_alarm((pm_rtc_alarm_type) alarm);
  pmic_err_flag |= pm_clear_irq( PM_RTC_ALRM_IRQ_HDL);
  pmic_err_flag |= pm_set_irq_handle( PM_RTC_ALRM_IRQ_HDL, NULL);
    if(TRUE == b_set_2018)
    {
        pm_rtc_julian_type list_time;
        list_time.day_of_week = 4; /* TGIF!...day of week not really necessary */
        list_time.month       = 01;
        list_time.day         = 01;
        list_time.year        = 2018;
        list_time.hour        = 0;
        list_time.minute      = 0;
        list_time.second      = 0;
        OEMRTC_hs_bsp_test_rtc_set_alarm_time((void*)&list_time);
    }

  //RTC_ERR("%d OEMRTC_hs_bsp_test_rtc_clr_alarm",pmic_err_flag,0,0);
  //return pmic_err_flag;
}
static void OEMRTC_hs_bsp_test_pmic_rtc_alarm_isr_cb( void)
{
    pm_err_flag_type   err_flag;

    //-->TODO read alarm status to find which alarm actually triggered.  Reset
    //appropriate alarm.  Ok for now since only have one alarm in use

    //rex_set_sigs(&ui_tcb, UI_RTC_SIG);
    err_flag  = pm_reset_rtc_alarm( PM_RTC_ALARM_1);
    err_flag |= pm_clear_irq( PM_RTC_ALRM_IRQ_HDL);
    err_flag |= pm_set_irq_handle( PM_RTC_ALRM_IRQ_HDL, NULL);
    //RTC_ERR("%d OEMRTC_hs_bsp_test_pmic_rtc_alarm_isr_cb",err_flag,0,0);
    RTC_PRINTF( ";---------------------------------------",0);
	RTC_PRINTF( ";OEMRTC_hs_bsp_test_pmic_rtc_alarm_isr_cb",0);
	RTC_PRINTF( ";rex_set_sigs, UI_RTC_SIG",0);
    rex_set_sigs(&ui_tcb, UI_RTC_SIG);
}

static void OEMRTC_hs_bsp_test_rtc_set_alarm_time(void *m_time )
{
    pm_rtc_alarm_type alarm;
    pm_rtc_julian_type *time = (pm_rtc_julian_type*)m_time;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    //if(!time)
    //{
    //    time.day_of_week = 4; /* TGIF!...day of week not really necessary */
    //    time.month       = 12;
    //    time.day         = 13;
    //    time.year        = 2007;
    //    time.hour        = 14;
    //    time.minute      = 41;
    //    time.second      = 0;
    //}
    alarm = PM_RTC_ALARM_1;

    err_flag  = pm_clear_irq( PM_RTC_ALRM_IRQ_HDL);
    err_flag |= pm_set_irq_handle( PM_RTC_ALRM_IRQ_HDL, OEMRTC_hs_bsp_test_pmic_rtc_alarm_isr_cb);
    err_flag |= pm_rtc_alarm_rw_cmd( PM_RTC_SET_CMD, alarm, time);
	
    if (err_flag != PM_ERR_FLAG__SUCCESS)
    {
        err_flag |= pm_rtc_alarm_rw_cmd( PM_RTC_SET_CMD, alarm, time);
        //RTC_ERR("%d OEMRTC_hs_bsp_test_rtc_set_alarm_time",err_flag,0,0);
    }
	RTC_PRINTF( ";----------------------------------",0);
    RTC_PRINTF( ";%d, OEMRTC_hs_bsp_test_rtc_set_alarm_time",err_flag);
	RTC_PRINTF( ";%d%02d%02d %02d:%02d:%02d", time->year, time->month, time->day, time->hour, time->minute, time->second);
	RTC_PRINTF( ";set irq handler = OEMRTC_hs_bsp_test_pmic_rtc_alarm_isr_cb",0);
    //return pm_rtc_alarm_type;
}

/*static*/ void OEMRTC_hs_bsp_test_rtc_get_alarm_time(void * m_time)
{
    pm_rtc_alarm_type alarm;
    pm_rtc_julian_type *time = (pm_rtc_julian_type*)m_time;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    alarm = PM_RTC_ALARM_1;

    err_flag |= pm_rtc_alarm_rw_cmd( PM_RTC_GET_CMD, alarm, time);
    if (err_flag != PM_ERR_FLAG__SUCCESS)
    {
        err_flag |= pm_rtc_alarm_rw_cmd( PM_RTC_GET_CMD, alarm, time);
    }
    //RTC_ERR("%d OEMRTC_hs_bsp_test_rtc_get_alarm_time",err_flag,0,0);
}

static boolean OEMRTC_Insert_Node_To_List(time_table *p_node)
{
    if(p_node == NULL)
    {
        return FALSE;
    }
    RTC_ERR("OEMRTC_Insert_Node_To_List",0,0,0);
    if(timer_list.phead == NULL) //add to first
    {
        timer_list.phead = p_node;
        p_node->next = NULL;
    }
    else
    {
        time_table * p0 = NULL;
        time_table * p1 = NULL;
        time_table * p2 = NULL;

        p1 = timer_list.phead;
        p0 = p_node;

        while( (p0->set_time >p1->set_time) && (p1->next != NULL))
        {
            p2= p1;
            p1 = p1->next;
        }

#if 1
		if( p0->set_time < p1->set_time ||
			( p0->set_time == p1->set_time && p0->sig >= OEMRTC_SET_ALARM_1)
		)
#else
        if(p0->set_time <= p1->set_time)
#endif
        {
            if(timer_list.phead == p1)
            {
                timer_list.phead = p0;
                p0->next = p1;
            }
            else
            {
                p2->next = p0;
                p0->next = p1;
            }
        }
        else
        {
            p1->next = p0;
            p0->next = NULL;

        }
    }
    timer_list.count ++;
    OEMRTC_Print_List();
    return TRUE;
}

boolean OEMRTC_Get_Current_Time(uint32 *get_time)
{
    uint32 time = 0;
    JulianType current_time;
    //RTC_ERR("OEMRTC_Get_Current_Time",0,0,0);
    current_time.wYear = 0;
    time = GETTIMESECONDS();

    if(time)
    {
        GETJULIANDATE(time, &current_time);
    }

    if(current_time.wYear < 2008)
    {
        pm_rtc_julian_type *get_rtc_time = NULL;
        get_rtc_time = (pm_rtc_julian_type*)OEMRTC_hs_bsp_test_rtc_get();
 		RTC_PRINTF( ";get current time, %d.%02d.%02d %02d:%02d:%02d", get_rtc_time->year, get_rtc_time->month, get_rtc_time->day, get_rtc_time->hour, get_rtc_time->minute, get_rtc_time->second);
        if(get_rtc_time->year < 2008)
        {
            return FALSE;
        }
        else
        {
            time = JULIANTOSECONDS((JulianType*)get_rtc_time);
        }
    }
    *get_time = time;
    return TRUE;
}

static void OEMRTC_Print_List(void)
{
#ifdef RTC_DEBUG
    int i = 0;
    time_table * temp = timer_list.phead;
	JulianType julian = {0};
    while(temp)
    {
        RTC_ERR("ID = %d time = %d sig = %d",i,temp->set_time,temp->sig);
    	GETJULIANDATE( temp->set_time, &julian);
        temp = temp->next;
        i ++;
    }
#endif /*RTC_DEBUG*/
}

void OEMRTC_Run_Call_Back_Function(void)
{
    time_table temp;
    RTC_ERR("sig= %d err_flag= %d set_time= %d Call_Back_Function",timer_list.phead->sig,timer_list.phead->function,timer_list.phead->set_time);
    temp.next     = NULL;
    temp.function = timer_list.phead->function;
    temp.set_time = timer_list.phead->set_time + 86400;//set to tomorrow
    temp.sig      = timer_list.phead->sig;
    temp.p_data   = timer_list.phead->p_data;

    if(timer_list.phead->function!= NULL)
    {
        (timer_list.phead->function)((void *)timer_list.phead->p_data);
    }
    OEMRTC_Del_Time_Node(timer_list.phead->sig);
	if( temp.sig <= OEMRTC_SET_POWER_ON)
	{
        OEMRTC_Add_Time_To_Timer_List(temp.set_time, temp.sig, temp.function , temp.p_data);
	}
    OEMRTC_Process_Timer_List();
}

//void OEMRTC_Process_User_Power_Off(void)
//{
//    RTC_ERR("OEMRTC_Process_User_Power_Off",0,0,0);
//    if(timer_list.phead)
//    {
//        if(timer_list.phead->sig == OEMRTC_SET_POWER_DOWN)
//        {
//            OEMRTC_hs_bsp_test_rtc_clr_alarm(PM_RTC_ALARM_1);
//            OEMRTC_Del_Time_Node(OEMRTC_SET_POWER_DOWN);
//            if(timer_list.phead)
//            {
//                OEMRTC_Process_Timer_List();
//            }
//        }
//    }
//}

#if 0
static boolean OEMRTC_Compare_Time(uint32 set_time)
{
    pm_rtc_julian_type alarmTime;
    uint32       get_time = 0;
    alarmTime.year = 0;
    OEMRTC_hs_bsp_test_rtc_get_alarm_time((void*)&alarmTime);
	RTC_PRINTF( ";set alarm time is: %d.%02d.%02d %02d:%02d:%02d", alarmTime.year, alarmTime.month, alarmTime.day, alarmTime.hour, alarmTime.minute, alarmTime.second);

    get_time = JULIANTOSECONDS( (JulianType*)&alarmTime);
    RTC_ERR("set_time= %d get_time= %d Compare_Time",set_time,get_time,0);
    if(set_time == get_time)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
#endif

static boolean OEMRTC_Modify_Time_Node(uint32 time ,byte timer_sig, time_list_cb cb, void * p_data)
{
    time_table * p1 = NULL;
    time_table * p2 = NULL;
    if(timer_list.phead == NULL)
    {
        RTC_ERR("timer_list.phead == NULL",0,0,0);
        return FALSE;
    }

    p1 = timer_list.phead;

    while(p1->sig != timer_sig && p1->next != NULL)
    {
        p2 = p1;
        p1 = p1->next;
    }

    if(p1->sig == timer_sig) //Ok,find it
    {
        if(p1->set_time == time && p1->function == cb && p1->p_data == p_data)
        {
            //RTC_ERR("List have the same node,not add it",0,0,0);
            OEMRTC_Print_List();
            return TRUE;
        }
    }
    return FALSE;
}
void OEMRTC_Free_All_Node(void)
{
    time_table * p1 = NULL;
    time_table * p2 = NULL;
    p1 = timer_list.phead;
    while(p1)
    {
        p2 = p1->next;
        sys_free(p1);
        p1 = p2;
    }
    timer_list.phead = NULL;
    timer_list.count = 0;
}

void OEMRTC_Phone_Reset(void *ppp)
{
    ICM *pIcm = NULL;
    IShell *pShell =(IShell*) ppp;
    uint32 time = 0;
    boolean flag = FALSE;
    if(pShell == NULL)
    {
        return;
    }
    if(ISHELL_CreateInstance(pShell ,AEECLSID_CM,(void **) &pIcm) != SUCCESS)
    {
        RTC_ERR("Unable to create ICM interface", 0, 0, 0);
        return;
    }
    OEM_SetConfig(CFGI_ALARM_FLAG, (void*)&flag, sizeof(flag));
    ICM_SetOperatingMode(pIcm, AEECM_OPRT_MODE_PWROFF);
    ICM_Release(pIcm);

    OEMRTC_hs_bsp_test_rtc_set();
    OEMRTC_hs_bsp_test_rtc_get();
    time = JULIANTOSECONDS((JulianType*)&pm_rtc_time);
    RTC_ERR("%d %d %d",pm_rtc_time.hour, pm_rtc_time.minute, pm_rtc_time.second);
    time = time + 7;
    GETJULIANDATE(time,(JulianType*)&pm_rtc_time);
    RTC_ERR("%d %d %d",pm_rtc_time.hour, pm_rtc_time.minute, pm_rtc_time.second);
    //pm_rtc_time.second = pm_rtc_time.second + 5;
    OEMRTC_hs_bsp_test_rtc_clr_alarm(PM_RTC_ALARM_1,FALSE);
    OEMRTC_hs_bsp_test_rtc_set_alarm_time((void*)&pm_rtc_time);
}
