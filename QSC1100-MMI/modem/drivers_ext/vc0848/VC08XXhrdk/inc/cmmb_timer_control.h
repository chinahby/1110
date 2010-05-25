/*======================================================================
*                                                                       
*                Copyright (C) 2008 Vimicro CO.,LTD     		 
* File Name: 	cmmb_timer_control.h 
* User for : 	820
* Creat By : 	anton liu                                                               
* CrateDate: 	2008/10/29
* Last modify : anton liu 2008-xx-xx     
*
* release history:
* 		version 0.1 release by anton liu
=======================================================================*/

#ifndef _CMMB_TIMER_CONTROL_
#define _CMMB_TIMER_CONTROL_

#include "mmp_rdk.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CMMB_TIMER_INTERVAL 10

enum {
	CMMB_TIMER_NOERR 				= 0, 	/* operation successful*/
	CMMB_TIMER_ERR_MALLOC,
	CMMB_TIMER_UNSUPPORT
};



typedef struct _cmmbTimer_routine_s_  cmmbTimer_routine_t;
struct _cmmbTimer_routine_s_{
	MMD_S08		id[4]; 
	MMD_U32		enable;
	MMD_U32		interval;
	MMD_U32		intr_cnt;
	void		(*timer_process)(void);
};


/*==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-
						macro and structor define for cmmb timer list and note
-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==-==*/
typedef struct _cmmbTimer_node_s_ cmmbTimer_node_t;
struct _cmmbTimer_node_s_{
	cmmbTimer_node_t	*next;
	cmmbTimer_node_t	*prev;
	void				*content;
};

typedef struct _cmmbTimer_node_list_{
	MMD_U32				counter;
 	cmmbTimer_node_t	*first;
	cmmbTimer_node_t	*last;
	cmmbTimer_node_t	*cur;
}cmmbTimer_node_list;

//: CMMB timer function declarations
cmmbTimer_node_list* cmmbTimer_list_create(void);
MMD_U32 cmmbTimer_list_append_content (cmmbTimer_node_list *l, void *content);
MMD_U32  cmmbTimer_list_free(cmmbTimer_node_list *l); 
MMD_U32	cmmbTimer_enable(cmmbTimer_node_list* list, MMD_S08* timer_id);
MMD_U32	cmmbTimer_disable(cmmbTimer_node_list* list, MMD_S08* timer_id);

#ifdef __cplusplus
}
#endif

#endif 

