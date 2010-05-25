/*======================================================================
*                                                                       
*                Copyright (C) 2008 Vimicro CO.,LTD     		 
* File Name: 	user_cmmb_control.h 
* User for : 	820
* Creat By : 	anton liu                                                               
* CrateDate: 	2008/10/13
* Last modify : anton liu 2008-xx-xx     
*
* release history:
* 		version 0.1 release by anton liu
=======================================================================*/
#ifdef CMMB_ENABLE	

#ifndef _USER_CMMB_CONTROL_
#define _USER_CMMB_CONTROL_

#include "mmp_rdk.h"

#ifdef __cplusplus
extern "C" {
#endif

extern cmmbTimer_node_list* gp_cmmb_timer_list;

enum {
	USER_CMMB_NO_ERROR 				= 0, 	/* operation successful*/
	USER_CMMB_ERR_MALLOC			= 1,	/* malloc error*/
	USER_CMMB_SEARCH_FAIL			= 2,
	USER_CMMB_ESG_BUF_ERR			= 3,
	USER_CMMB_MMD_FAIL				= 4,
	USER_CMMB_ESG_SAVEBUF_FULL
};



typedef struct
{
	MMD_U32 channel_no;
	tCMCT ch_info;
	MMD_U32 search_try;
	MMD_U32 time_out;
} CMMB_SEARCH_INFO;

typedef struct 
{
	MMD_U32 service_num;
	MMD_U32 fill_over;	//	Check this value for Get ESG over
	CMMB_ServiceElement service_element[USER_CMMB_MAX_SERVICE_ELEMENT];
} USER_CMMB_SERVICE_LIST;

typedef struct 
{
	MMD_U32 schedule_num;
	MMD_U32 fill_over;	//	Check this value for Get ESG over
	//CMMB_ServiceSchedule service_schedule[USER_CMMB_MAX_SERVICE_SCHEDULE];
} USER_CMMB_SCHEDULE_LIST;

typedef struct
{
	MMD_U32 channel_no;
	MMD_U32 start_slot;
	MMD_U32 slot_count;
	MMD_U32 demod;
	
	MMD_U32 service_id;		//	Service ID
	CMMB_ServiceElement*	service_info;	//	pointer to ESG service info 
} CMMB_Play_Info;


typedef struct 
{
	MMD_U32 play_num;
	CMMB_Play_Info play_info[USER_CMMB_MAX_SERVICE_ELEMENT];
} USER_CMMB_PLAY_LIST;

typedef void (*ESG_SAVE_CALLBACK)(MMD_S08* esg_buf, MMD_U32 esg_len, MMD_U32 esg_element_num, MMD_U32 esg_type, MMD_U32* result);

//: API declearation
MMD_U32 USER_CMMB_INIT(CMMB_INFO*  pcmmb_info, MMD_U32 fw_option);
MMD_U32 USER_CMMB_SEARCH(CMMB_INFO*  pcmmb_info, MMD_U32 try_times, USER_CMMB_PLAY_LIST* pplay_list);

MMD_U32 USER_CMMB_SET_DISPLAY_PARA(CMMB_LCD_INFO* lcd_info);

MMD_U32 USER_CMMB_PLAY(CMMB_Play_Info* play_info);
MMD_U32 USER_CMMB_STOP(void);
MMD_U32 USER_CMMB_QUIT(void);
MMD_U32 USER_CMMB_GetService(CMMB_INFO*  pcmmb_info, USER_CMMB_SERVICE_LIST* pservice_list, USER_CMMB_PLAY_LIST* pplay_list);
MMD_U32 USER_CMMB_GetSchedule(CMMB_INFO*  pcmmb_info, USER_CMMB_SCHEDULE_LIST* pschedule_list);


MMD_U32 USER_CMMB_GET_SYS_STATUS(CMMB_SysStatus* cmmb_status);
MMD_U32 USER_CMMB_SET_VOLUME(MMD_U32 volume);
MMD_U32 USER_CMMB_ESG_STOP(void);
MMD_U32 USER_CMMB_Timer_Register(MMD_S08* id, TimerISR pTimerISR, MMD_U32 interval);
MMD_U32 USER_CMMB_Timer_Start(MMD_S08* id);
MMD_U32 USER_CMMB_Timer_Stop(MMD_S08* id);
void  USER_CMMB_SET_ESG_SAVE_CALLBACK(ESG_SAVE_CALLBACK callback);

///:~
#endif
#ifdef __cplusplus
}
#endif

#endif 

