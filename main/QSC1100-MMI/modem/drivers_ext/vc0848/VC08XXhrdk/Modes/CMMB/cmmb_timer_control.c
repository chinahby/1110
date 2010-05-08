/*======================================================================
*                                                                       
*                Copyright (C) 2008 Vimicro CO.,LTD     		 
* File Name: 	cmmb_timer_control.c 
* User for : 	820
* Creat By : 	anton liu                                                                   
* CrateDate: 	2008/10/13
* Last modify : anton liu   2008-xx-xx     
*
* version 0.1: 
*
* release history:
* 		version 0.1 release by anton liu
=======================================================================*/

#include "cmmb_timer_control.h"

#ifdef CMMB_ENABLE	


void cmmbTimer_defaut_process(void)
{
	;
}

MMD_U32 cmmbTimer_set_default(cmmbTimer_routine_t* routine)
{
	routine->enable = 0;
	routine->id[0] = routine->id[1] = routine->id[2] = routine->id[3] = 'd';
	routine->interval = 100;
	routine->intr_cnt = 0;
	routine->timer_process = cmmbTimer_defaut_process;
	return CMMB_TIMER_NOERR;
}

cmmbTimer_node_list* cmmbTimer_list_create(void) 
{
  	cmmbTimer_node_list *list	= NULL;

  	list = (cmmbTimer_node_list*) MMD_Malloc(sizeof(cmmbTimer_node_list));
	if(list==NULL)
	{
		CMMB_PRINTF("Malloc cmmbTimer_node_list fail!\n");
		return NULL;
	}
	else
	{
		//initlize object list each member
		list->counter = 0;
  		list->first=NULL;
  		list->last =NULL;
  		list->cur  =NULL;
  	}
  	return list;
}


MMD_U32 cmmbTimer_list_append_content (cmmbTimer_node_list *l, void *content) 
{
  	cmmbTimer_node_t *node	= NULL;
 
  	node = (cmmbTimer_node_t *) MMD_Malloc(sizeof(cmmbTimer_node_t));
  	node->content = content;
	if (l->last) 
 	{
    		node->next = NULL;
    		node->prev = l->last;
    		l->last->next = node;
    		l->last = node;
    		l->cur = node;
    		l->counter++;
  	} 
  	else 
  	{
    		l->first = l->last = l->cur = node;
    		node->prev = node->next = NULL;
    		l->counter++;
  	}
  	 return CMMB_TIMER_NOERR;
}


MMD_U32  cmmbTimer_list_free(cmmbTimer_node_list *l) 
{
  	cmmbTimer_node_t *node	= NULL;
  
  	if (!l) 
  	{
 		return CMMB_TIMER_NOERR;
  	}
 
  	if(!l->counter) 
  	{
  		MMD_Free(l);
		l = NULL;
    	return CMMB_TIMER_NOERR;
  	}

  	node = l->first;
	while(node) 
	{
		cmmbTimer_node_t *n = node;
		node = n->next;
		MMD_Free(n->content); 
		n->content = NULL;	 
		MMD_Free(n);
		n = NULL;
		l->counter--;
	}
 	MMD_Free(l);
	l = NULL;
	
	return CMMB_TIMER_NOERR;
}

MMD_U32 cmmbTimer_check_id(cmmbTimer_routine_t* routine, MMD_S08* id)
{
	if((routine->id[0] == id[0]) && (routine->id[1] == id[1])
		&& (routine->id[2] == id[2]) && (routine->id[3] == id[3]))
	{
		return 1;
	}
	else
		return 0;
}


MMD_U32	cmmbTimer_enable(cmmbTimer_node_list* list, MMD_S08* timer_id)
{
	cmmbTimer_node_t *node	= NULL;
	cmmbTimer_routine_t	*routine	= NULL;

	node = list->first;
	while(node)
	{
		routine = (cmmbTimer_routine_t*)node->content;
		if(cmmbTimer_check_id(routine, timer_id))
		{
			routine->enable = 1;
			return CMMB_TIMER_NOERR;
		}
		node = node->next;
	}
	return CMMB_TIMER_UNSUPPORT;
}

MMD_U32	cmmbTimer_disable(cmmbTimer_node_list* list, MMD_S08* timer_id)
{
	cmmbTimer_node_t *node	= NULL;
	cmmbTimer_routine_t	*routine	= NULL;

	node = list->first;
	while(node)
	{
		routine = (cmmbTimer_routine_t*)node->content;
		if(cmmbTimer_check_id(routine, timer_id))
		{
			routine->enable = 0;
			return CMMB_TIMER_NOERR;
		}
		node = node->next;
	}
	return CMMB_TIMER_UNSUPPORT;
}

#endif
