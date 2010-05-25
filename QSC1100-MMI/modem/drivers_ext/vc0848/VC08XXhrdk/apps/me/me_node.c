/*======================================================================
*                                                                       
*                Copyright (C) 2006 Vimicro CO.,LTD     		 
* File Name: 	me_node.c 
* User for : 	820
* Creat By : 	Amanda Deng()                                                                   
* CrateDate: 	2006/12/31
* Last modify : Amanda Deng 2007-xx-xx     
*
* release history:
* 		version 0.1 release by Amanda Deng (Deng Naili --Zhang Zhongliang AE)
=======================================================================*/

#include "me_node.h"

/*******************************************************************************************
Description:
	creat a object list, and initialize all content to NULL
Parameters:
       No.
Return:	
      
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
me_node_list	*me_list_new(ME_VOID) 
{
  	me_node_list *list	= NULL;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_list_new()"));
	#endif
	//new a object list
  	list = (me_node_list *) me_malloc(sizeof(me_node_list));
	if(list==NULL)
	{
		ME_ERROR(("ME_ERR_MALLOC_MEMORY_ERROR"));
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
/*******************************************************************************************
Description:
	free a object list, and free all content memory
Parameters:
       me_node_list *l: object list point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
ME_U32 me_list_free(me_node_list *l) 
{
  	me_node_t *node	= NULL;
  	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_list_free()"));
	#endif  
	
	if (!l) 
  	{
    		ME_MESSAGE(("->no list"));
 		return ME_SUCCESS;
  	}

  	if(!l->counter) 
  	{
		#if(!ME_KM_OBJ_STACK)  
  			me_free(l);
			l = NULL;
  		#endif  
    		return ME_FAILE;
  	}

  	node = l->first;
    	while(node) 
    	{
    		me_node_t *n = node;
      		/* TODO: support for content destructors */
    		node = n->next;
		#if(!ME_KM_OBJ_STACK)
			me_free(n->content); 
			n->content = NULL;	 
		#endif	
    		me_free(n);
		n = NULL;
		l->counter--;
  	}
	#if(!ME_KM_OBJ_STACK)  
  		me_free(l);
		l = NULL;
	#endif  

  return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	get a object list first content 
Parameters:
       me_node_list *l: object list point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
ME_VOID	*me_list_first_content (me_node_list *l) 
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_list_first_content()"));
	#endif
	
  	l->cur = l->first;
	if (l->first)
  	{
    		return l->first->content;
  	}
  	else
  	{
  		return NULL;
  	}
}
/*******************************************************************************************
Description:
	get a object list next content 
Parameters:
       me_node_list *l: object list point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
ME_VOID	*me_list_next_content (me_node_list *l) 
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_list_next_content()"));
	#endif
	
  	if (l->cur) 
  	{
   		 if (l->cur->next) 
   		 {
     			 l->cur = l->cur->next;
      			return l->cur->content;
    		} 
    		else
    		{
      			return NULL;
    		}
  	} 
  	else 
  	{
		return NULL;
  	}    
}
/*******************************************************************************************
Description:
	judge a object list is empty or not
Parameters:
       me_node_list *l: object list point
Return:	
      ME_TRUE:		list is empty
      ME_FALSE: 	list is not empty
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
ME_U32 me_list_is_empty (me_node_list *l) 
{
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_list_is_empty()"));
	#endif

  	if (l == NULL)
  	{
		ME_MESSAGE(("->list is NULL"));
		return ME_SUCCESS;
  	}
  
  	return (l->first == NULL)?(ME_TRUE):(ME_FALSE);
}
/*******************************************************************************************
Description:
	add a content node to node list, the node with priority
Parameters:
       me_node_list *l: object list point
       ME_VOID *content:  content node
       ME_S32 priority:   node's priority
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
ME_U32 me_list_append_priority_content (me_node_list *l, ME_VOID *content, ME_S32 priority) 
{
  	me_node_t *node	= NULL;
	#if ME_KM_FUNNAME
		ME_MESSAGE(("->me_list_append_priority_content()"));
	#endif
	
	ME_ASSERT_PARAM_RETURN((l!=NULL));
	ME_ASSERT_PARAM_RETURN((content!=NULL));
	
  	node = (me_node_t *) me_malloc(sizeof(me_node_t));
  	node->content = content;
  	node->priority = priority;

  	if (l->counter) 
  	{
    		me_node_t *cur;
    		cur = l->first;
		while(1) 
    		{
      			if( priority > cur->priority ) 
      			{
        			node->next = cur;
        			node->prev = cur->prev;
        			if( node->prev )
         			 	node->prev->next = node;
        			else
          				l->first = node;
        			cur->prev = node;

       		 	l->cur = node;
        			break;
      			}
      			if( !cur->next ) 
      			{
        			node->next = NULL;
        			node->prev = cur;
        			cur->next = node;

        			l->cur = node;
        			l->last = node;
        			break;
      			}
      			cur = cur->next;
  		}
		l->counter++;
  	} 
 	else 
 	{
    		l->first = l->last = l->cur = node;
    		l->counter++;
    		node->prev = node->next = NULL;
 	}
 	  return ME_SUCCESS;
}
/*******************************************************************************************
Description:
	add a content node to node list
Parameters:
       me_node_list *l: object list point
       ME_VOID *content:  content node
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2006.12.31		create first version
********************************************************************************************/
ME_U32 me_list_append_content (me_node_list *l, ME_VOID *content) 
{
  	me_node_t *node	= NULL;
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_list_append_content()"));
	#endif  

  	node = (me_node_t *) me_malloc(sizeof(me_node_t));
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
  	 return ME_SUCCESS;
}

/*******************************************************************************************
Description:
	free a object list, and free all content memory
Parameters:
       me_node_list *l: object list point
Return:	
      ME_SUCCESS:	no error;
      Other Value: error information;
Modify History:
	Version			Modifyby				DataTime			modified
	   0.1			Amanda Deng			2007.03.20		create first version
********************************************************************************************/
ME_U32 me_effectprclist_free(me_node_list *l) 
{
  	me_node_t *node	= NULL;
  	
	#if ME_KM_FUNNAME
	ME_MESSAGE(("->me_effectprclist_free()"));
	#endif  

  	if (!l) 
  	{
    		ME_MESSAGE(("->no list"));
 		return ME_SUCCESS;
  	}
 
  	if(!l->counter) 
  	{
  		me_free(l);
		l = NULL;
    		return ME_SUCCESS;
  	}

  	node = l->first;
    	while(node) 
    	{
    		me_node_t *n = node;
    		node = n->next;
		me_free(n->content); 
		n->content = NULL;	 
    		me_free(n);
		n = NULL;
		l->counter--;
  	}
 	me_free(l);
	l = NULL;
	
	 return ME_SUCCESS;
}
