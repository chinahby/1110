/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     C A M C O R D E R   U T I L I T I E S

GENERAL DESCRIPTION
  This module contains utility funcitons used by all camcorder subsystems.

REFERENCES
  none

EXTERNALIZED FUNCTIONS
  Memory heap tracking system.

Copyright(c) 2004-2005 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/main/latest/src/camcorder_util.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/14/06   dhaval  Added null checking for input parameter in 
                   camcorderutil_llmo_destructor
07/14/06   jdas    Added message related global variables for FEATURE_VIDEO_CLIP_INFO
06/07/06   pavan   Revert Link error cleanup due to regressions uncovered
                   during video editing & cut and merge testing
03/29/06   jdas    Removed Lint error 
10/17/05   gm      Automated script to modify the MSG macro's
07/29/05   wyh     llmo routines will take malloc() and free() func pointer
03/16/05   wyh     Prevent it from crashing if context is NULL.
03/16/05   wyh     Added the llmo (linked list memory object)
03/02/05   wyh     Fixed bug in realloc. Added malloc_verify()
02/09/05   wyh     Improved malloc debug. Implement fence and source tracking
01/31/05   wyh     Fix local_q_linear_delete to work for last q
12/29/04   wyh     Created file.

===========================================================================*/
/* <EJECT> */
/*===========================================================================

                        INCLUDE FILES FOR MODULE

===========================================================================*/
#include "camcorder_util.h"
#include "camcorder_msg.h" 
#include "rex.h"

#include "assert.h"
/* <EJECT> */
/*===========================================================================

                          DECLARATIONS FOR MODULE

===========================================================================*/
#ifdef FEATURE_VIDEO_CLIP_INFO
/* The following are global variables for error/fatal 
** message logging into the clip
*/
video_fmt_msg_list_type *msg_head = NULL;
video_fmt_msg_list_type *msg_curr = NULL;
boolean                 log_msg = TRUE;
#endif //FEATURE_VIDEO_CLIP_INFO

#ifdef FEATURE_CAMCORDER_MALLOC



/* a packet error emssage will be issued during a free or realloc error.
** The first CAMCORDER_MSG_ERROR() will be sent from the location of the free() call.
** The second one indicates where that packet was allocated.
*/
static void  camcorderutil_packet_error_msg(
  msg_const_type *m1,
  uint32 linenum, char *module,
  camcorderutil_malloc_queue_element_type *elem,
  char *format, 
  uint32 arg1,uint32 arg2, uint32 arg3)
{
  static char *allocated_from_msg = "Location where memory was allocated from";
  msg_const_type *m2;

  m1->desc.line = linenum;
  m1->desc.ss_id = MSG_SSID_DFLT;
  m1->desc.ss_mask = MSG_LEGACY_ERROR;
  m1->fname     = module;
  m1->fmt       = format;
  msg_send_3(m1,arg1,arg2,arg3);

  m2 = elem->msg;
  m2->fname     = elem->module;
  m2->fmt       = allocated_from_msg;
  msg_send_3(m2,0,0,0);

}

/* This has to do with context errors where we don't know if a packet
** is valid or not.
** The first CAMCORDER_MSG_ERROR() is from where the malloc()/free()/realloc()
** call originated. The second CAMCORDER_MSG_ERROR() tells you where the context
** was initialized.
*/
static void  camcorderutil_context_error_msg(
  msg_const_type *m1,
  uint32 linenum, const char *module,
  camcorderutil_malloc_context_type *context,
  char *format,
  uint32 arg1, uint32 arg2, uint32 arg3
)
{
  static char *allocated_from_msg = "Location where memory was initialized";
  msg_const_type *m2;


  m1->desc.line = linenum;
  m1->desc.ss_id = MSG_SSID_DFLT;
  m1->desc.ss_mask = MSG_LEGACY_ERROR;
  m1->fname     = module;
  m1->fmt       = format;
  msg_send_3(m1,arg1,arg2,arg3);

  if(context)
  {
    m2 = context->msg;
    m2->fname     = context->module;
    m2->fmt       = allocated_from_msg;
    msg_send_3(m2,0,0,0);  
  }

}


/* ==========================================================================
FUNCTION camcorderutil_malloc_init
DESCRIPTION
   Initializes a malloc context structure.

   The module parameter should be called with __MODULE__ or __func__
   The linenum parameter should be called with __LINE__.
   
DEPENDENCIES
   None.
RETURN VALUE
   None.
SIDE EFFECTS
   None.
=============================================================================*/
camcorderutil_malloc_context_type *camcorderutil_malloc_init(
  char *module, uint32 linenum,msg_const_type *m1
  )
{
  camcorderutil_malloc_context_type *context;

  context = malloc(sizeof(camcorderutil_malloc_context_type));

  if (!context ) {
    return context;
  }

  m1->desc.line  = linenum;
  m1->desc.ss_id = MSG_SSID_DFLT;
  m1->desc.ss_mask = MSG_LEGACY_ERROR;

  /* set up the context 
  */
  context->msg            = m1;
  context->state          = CAMCORDERUTIL_MALLOC_STATE_IDLE;
  context->tot_allocated  = 0;
  context->max_heap_usage = 0;
  context->nmallocs       = 0;
  context->linenum        = linenum;
  strncpy(context->module,module, CAMCORDER_UTIL_MALLOC_MODNAMESIZE);
  if (strlen(module) <= CAMCORDER_UTIL_MALLOC_MODNAMESIZE) {
     context->module[CAMCORDER_UTIL_MALLOC_MODNAMESIZE] = '\0';
  }

  q_init(&context->allocated_q);

  context->state = CAMCORDERUTIL_MALLOC_STATE_ACTIVE;

  CAMCORDER_MSG_HIGH("camcorderutil_malloc initialized",0,0,0);

  return context;
}

/* ==========================================================================
FUNCTION camcorderutil_malloc_terminate
DESCRIPTION
   Terminates a malloc context structure and prints out heap usages and
   memory leak information.

   The module parameter should be called with __MODULE__ or __func__
   The linenum parameter should be called with __LINE__.
   
DEPENDENCIES
   None.
RETURN VALUE
   None.
SIDE EFFECTS
   None.
=============================================================================*/
camcorderutil_malloc_ret_type camcorderutil_malloc_terminate(
  camcorderutil_malloc_context_type *context,
  uint32 linenum, char *module,msg_const_type *m1
  )
{
  uint32 tot_leaked_bytes = 0;
  camcorderutil_malloc_queue_element_type *pmalloc_element;

  if ( !context ||
       context->state != CAMCORDERUTIL_MALLOC_STATE_ACTIVE) {
    return CAMUTIL_MALLOC_FAILURE;
  }

  while(q_cnt(&context->allocated_q))
  {
    pmalloc_element = (camcorderutil_malloc_queue_element_type *)
      q_get(&context->allocated_q);
    if (pmalloc_element->nbytes) {
      tot_leaked_bytes += pmalloc_element->nbytes;
      camcorderutil_packet_error_msg(m1,linenum,module,pmalloc_element,
           "CAMUTIL_malloc: detected %d bytes leak",pmalloc_element->nbytes,0,0);
      free(pmalloc_element->fence_ptr);
    }
    free(pmalloc_element);
  }

  if (tot_leaked_bytes) {
    camcorderutil_context_error_msg(m1,linenum,module,context,
                  "CAMUTIL_malloc memory leak %d bytes",
                   tot_leaked_bytes,0,0);
  }

  camcorderutil_context_error_msg(m1,linenum,module,context,
          "CAMUTIL_malloc terminate, max heap= %d nmallocs=%d",
          context->max_heap_usage,context->nmallocs,0);

  context->state = CAMCORDERUTIL_MALLOC_STATE_IDLE;

  free(context);
  return CAMUTIL_MALLOC_SUCCESS;
}

/* ==========================================================================
FUNCTION camcorderutil_malloc
DESCRIPTION
   Allocate memory in a certain context.

   The module parameter should be called with __MODULE__ or __func__
   The linenum parameter should be called with __LINE__.
   
DEPENDENCIES
   None.
RETURN VALUE
   None.
SIDE EFFECTS
   None.
=============================================================================*/
void * camcorderutil_malloc(camcorderutil_malloc_context_type *context,
                      uint32           nbytes_requested,
                      char            *module_name,
                      uint32           linenum

                     )
{
  void *retval;
  int i;
  camcorderutil_malloc_queue_element_type *pmalloc_element; 
  msg_const_type *m1;

  m1 = (msg_const_type *) malloc(sizeof(msg_const_type));

  if (!context ||
      context->state != CAMCORDERUTIL_MALLOC_STATE_ACTIVE ) {
    camcorderutil_context_error_msg(m1,linenum,module_name,context,
      "CAMCORDERUTIL_MALLOC invalid context",0,0,0);
    return NULL;
  }

  pmalloc_element = (camcorderutil_malloc_queue_element_type *)
      malloc(sizeof(camcorderutil_malloc_queue_element_type));

  if ( !pmalloc_element ) 
  {
    retval = NULL;
  }
  else
  {
    retval = pmalloc_element->fence_ptr = (uint8 *)
      malloc(nbytes_requested + 2*CAMCORDER_UTIL_MALLOC_FENCE_SIZE);
  
    if (retval) {
      q_link_type *link_item;

      /* all malloc() operations are successful ...
      ** add it as a queue element
      */

      pmalloc_element->nbytes = nbytes_requested;
      retval = pmalloc_element->ptr = 
        &pmalloc_element->fence_ptr[CAMCORDER_UTIL_MALLOC_FENCE_SIZE];

      /* fill the fence in front and back with magic number */
      for(i=0; i<CAMCORDER_UTIL_MALLOC_FENCE_SIZE; i++)
      {
        pmalloc_element->fence_ptr[i] = CAMCORDER_UTIL_MALLOC_FENCE_MAGIC;
        pmalloc_element->fence_ptr[
          CAMCORDER_UTIL_MALLOC_FENCE_SIZE + nbytes_requested + i
          ] = CAMCORDER_UTIL_MALLOC_FENCE_MAGIC;
      }
      strncpy(pmalloc_element->module,module_name,
              CAMCORDER_UTIL_MALLOC_MODNAMESIZE
              );
      if (strlen(module_name) <= CAMCORDER_UTIL_MALLOC_MODNAMESIZE) {
        pmalloc_element->module[CAMCORDER_UTIL_MALLOC_MODNAMESIZE] = '\0';
      }

      pmalloc_element->linenum = linenum;

      m1->desc.line    = linenum;
      m1->desc.ss_id   = MSG_SSID_DFLT;
      m1->desc.ss_mask = MSG_LEGACY_ERROR;

      pmalloc_element->msg = m1;

      TASKLOCK();
      /* prepare the new buffer to go into a queue */
      link_item = q_link( pmalloc_element, &pmalloc_element->link );
  
      q_put(&context->allocated_q,link_item);
  
      context->tot_allocated += nbytes_requested;
      if ( context->tot_allocated > context->max_heap_usage ) {
        context->max_heap_usage = context->tot_allocated;
      }

      context->nmallocs++;
      TASKFREE();
    }
    else
    {
      free(pmalloc_element);
    }  
  }

  return retval;
}


/* variables used by the queue sub-functions
** ... must be called within TASKLOC()'s
*/

static void * target_free_ptr = NULL; /* the pointer we wish to free/realloc */
static q_type  *current_q;            /* current queue we're working with    */
/* return value ... realloc() and free() must be done outside tasklock()'s   */
static camcorderutil_malloc_queue_element_type *requested_free_elem;

/* ==========================================================================
FUNCTION compare_func_for_free_and_realloc
DESCRIPTION
   Compare function required by q_linear_delete() and q_linear_search()
DEPENDENCIES
   None.
RETURN VALUE
   None.
SIDE EFFECTS
   None.
=============================================================================*/
static int compare_func_for_free_and_realloc(void  *generic_qelem, void *dummy_not_used)
{
  camcorderutil_malloc_queue_element_type *qelem;

  qelem = (camcorderutil_malloc_queue_element_type *) generic_qelem;

  return (qelem->ptr == target_free_ptr);
}

/* ==========================================================================
FUNCTION action_func_for_free
DESCRIPTION
   Action function required by q_linear_delete().
DEPENDENCIES
   None.
RETURN VALUE
   None.
SIDE EFFECTS
   None.
=============================================================================*/
static void action_func_for_free( void *generic_qelem,void *param)
{ 
  requested_free_elem = 
    (camcorderutil_malloc_queue_element_type *) generic_qelem;
  /* this should be done by q_linear_delete() but its not
  */
}


/* ==========================================================================
FUNCTION camcorderutil_free
DESCRIPTION
   Free memory allocated in a certain context.

   The module parameter should be called with __MODULE__ or __func__
   The linenum parameter should be called with __LINE__.

DEPENDENCIES
   None.
RETURN VALUE
   None.
SIDE EFFECTS
   None.
=============================================================================*/
void camcorderutil_free(camcorderutil_malloc_context_type *context,
                        uint32 linenum, char *module,msg_const_type *m1,
                    void *ptr
                   )
{
  int i;
  camcorderutil_malloc_queue_element_type *qelem;

  /* see below why we have to define this here */
  void local_q_linear_delete(
         q_type             *q_ptr,  
         q_compare_func_type compare_func,
         void               *param,
         q_action_func_type  action_func
  );


  if (!context ||
      context->state != CAMCORDERUTIL_MALLOC_STATE_ACTIVE ) {
    camcorderutil_context_error_msg(m1,linenum,module,context,
        "CAMCORDERUTIL_free invalid context",0,0,0);
    return;
  }

  TASKLOCK();

  target_free_ptr = ptr;
  requested_free_elem = NULL;
  current_q = &context->allocated_q;

  local_q_linear_delete(&context->allocated_q,
                   compare_func_for_free_and_realloc,
                   NULL,
                   action_func_for_free
                 );

  /* copy free request to local variable so that the actual
  ** free()'ing can be done outside TASKLOC()'s
  */
  qelem = requested_free_elem;

  if (qelem) {
    context->tot_allocated -= qelem->nbytes;
  }

  TASKFREE();

  if ( qelem ) {

    /* test if the fence was violated */
    for (i=0;i<CAMCORDER_UTIL_MALLOC_FENCE_SIZE; i++) {
      /* violation of the front part of the fence */
      if (qelem->fence_ptr[i] != CAMCORDER_UTIL_MALLOC_FENCE_MAGIC)
      {
        camcorderutil_packet_error_msg(m1,linenum,module,qelem,
          "camutil_free: front fence violation!",0,0,0);
      }
      if(qelem->fence_ptr[CAMCORDER_UTIL_MALLOC_FENCE_SIZE
                          + qelem->nbytes + i]
         != CAMCORDER_UTIL_MALLOC_FENCE_MAGIC
         )
      {
        camcorderutil_packet_error_msg(m1,linenum,module,qelem,
          "camutil_free: rear fence violation!",0,0,0);
      }
    }

    /* perform the requested free operation */
    free(qelem->fence_ptr);
    // let it leak memory
    //free(qelem->msg);
    free(qelem);
  }
  else
  {
    /* freed pointer was not CAMUTIL_MALLOC'ed with the same context
    ** we'll free it but we must compmlain first.
    */
    camcorderutil_context_error_msg(m1,linenum,module,context,
          "CAMUTIL FREEing invalid pointer",0,0,0);
    free(ptr); /* not sure what we're freeing but free it anyway*/
  }
}


camcorderutil_malloc_ret_type camcorderutil_malloc_verify(
  camcorderutil_malloc_context_type *context,
  uint32 linenum, char *module,msg_const_type *m1
  )
{
  uint32 i;
  camcorderutil_malloc_queue_element_type *qelem;

  if ( !context ||
       context->state != CAMCORDERUTIL_MALLOC_STATE_ACTIVE) {
    return CAMUTIL_MALLOC_FAILURE;
  }

  
  qelem = (camcorderutil_malloc_queue_element_type *)
      q_check(&context->allocated_q);
  while(qelem != NULL)
  {

    /* test if the fence was violated */
    for (i=0;i<CAMCORDER_UTIL_MALLOC_FENCE_SIZE; i++) {
      /* violation of the front part of the fence */
      if (qelem->fence_ptr[i] != CAMCORDER_UTIL_MALLOC_FENCE_MAGIC)
      {
        camcorderutil_packet_error_msg(m1,linenum,module,qelem,
          "malloc_verify: front fence violation!",0,0,0);
        return CAMUTIL_MALLOC_FAILURE;
      }
      if(qelem->fence_ptr[CAMCORDER_UTIL_MALLOC_FENCE_SIZE
                          + qelem->nbytes + i]
         != CAMCORDER_UTIL_MALLOC_FENCE_MAGIC
         )
      {
        camcorderutil_packet_error_msg(m1,linenum,module,qelem,
          "malloc_verify: rear fence violation!",0,0,0);
        return CAMUTIL_MALLOC_FAILURE;
      }

    }
    qelem = q_next(&context->allocated_q,&qelem->link);
  }

  return CAMUTIL_MALLOC_SUCCESS;
}

/* ==========================================================================
FUNCTION camcorderutil_realloc
DESCRIPTION
   Resize memory allocated in a certain context.
DEPENDENCIES
   None.
RETURN VALUE
   None.
SIDE EFFECTS
   None.
=============================================================================*/
void * camcorderutil_realloc (camcorderutil_malloc_context_type *context,
                              uint32 linenum, char *module,
                              void *ptr,
                              uint32 nbytes
                   )
{
  void *retval;
  camcorderutil_malloc_queue_element_type *elem;

  if (!context ||
      context->state != CAMCORDERUTIL_MALLOC_STATE_ACTIVE ) {
    msg_const_type *m1;
    m1 = (msg_const_type *) malloc(sizeof(msg_const_type));
    camcorderutil_context_error_msg(m1,linenum,module,context,
         "CAMCORDERUTIL_realloc invalid context",0,0,0);
    return NULL;
  }

  target_free_ptr = ptr;

  TASKLOCK();
  elem = (camcorderutil_malloc_queue_element_type *)
    q_linear_search(&context->allocated_q,
                    compare_func_for_free_and_realloc,
                    NULL);

  TASKFREE();

  if (elem == NULL) {
    msg_const_type *m1;
    m1 = (msg_const_type *) malloc(sizeof(msg_const_type));
    camcorderutil_context_error_msg(m1,linenum,module,context,
      "CAMUTIL REALLOOC an invalid pointer",0,0,0);
    /* this pointer was not allocated in the same context
    ** but we'll realloc it anyway
    */
    retval = realloc(ptr,nbytes);
  }
  else {
    /* check for fence violations */
    {
      int i;
      msg_const_type m1;
      for(i=0; i<CAMCORDER_UTIL_MALLOC_FENCE_SIZE; i++) {
        if(elem->fence_ptr[i] != CAMCORDER_UTIL_MALLOC_FENCE_MAGIC)
          camcorderutil_packet_error_msg(&m1,linenum,module,elem,
             "camcorderult_realloc: front fence violation!",0,0,0);
        if(elem->fence_ptr[elem->nbytes+i+
                           CAMCORDER_UTIL_MALLOC_FENCE_SIZE]
           != CAMCORDER_UTIL_MALLOC_FENCE_MAGIC
          )
          camcorderutil_packet_error_msg(&m1,linenum,module,elem,
             "camcorderult_realloc: front fence violation!",0,0,0);
        /* destroy the real fence before reallocing */
        elem->fence_ptr[elem->nbytes+i+CAMCORDER_UTIL_MALLOC_FENCE_SIZE]
          = 0;
      }
    }
    elem->fence_ptr = realloc(elem->fence_ptr,nbytes+
                           2*CAMCORDER_UTIL_MALLOC_FENCE_SIZE);
    TASKLOCK();
    context->tot_allocated -= elem->nbytes;
    context->tot_allocated += nbytes;
    elem->nbytes = nbytes;
    if(context->tot_allocated > context->max_heap_usage )
      context->max_heap_usage = context->tot_allocated;
    TASKFREE();
    elem->ptr = &elem->fence_ptr[CAMCORDER_UTIL_MALLOC_FENCE_SIZE];
    retval = elem->ptr;

    {
      int i;
      /* recontstruct the rear fence */
      for(i=0; i<CAMCORDER_UTIL_MALLOC_FENCE_SIZE; i++)
      {
        elem->fence_ptr[i+CAMCORDER_UTIL_MALLOC_FENCE_SIZE+nbytes]
          = CAMCORDER_UTIL_MALLOC_FENCE_MAGIC;
      }
    }
  }

  return retval;

}

/*  OK THIS IS ANNOYING
**
** there is a bug in q_linear_delete() where if the queue element is not
** at the head it will not decrement count.
**
** rajeevp and davidb have been notified to fixe this. Until they check
** in their fix we'll have to replicate that function here with the fix
** and change the name to local_q_linear_delete()
**
** Please remove this after they check in their fix.
*/

   #define q_lock( q ) INTLOCK( )
   #define q_free( q ) INTFREE( )
/*===========================================================================
FUNCTION Q_LINEAR_DELETE

DESCRIPTION
  Given a comparison function, this function traverses the elements in
  a queue, calls the compare function, and returns a pointer to the
  current element being compared if the user passed compare function
  returns non zero.  In addition, the item will be removed from the queue.

  The user compare function should return 0 if the current element is
  not the element in which the compare function is interested.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to q_init.

  The user's queue elements must have q_link_type as the first element
  of the queued structure.  

  The user's compare function will be passed NULL for the compare value.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void local_q_linear_delete(
   q_type             *q_ptr,  
   q_compare_func_type compare_func,
   void               *param,
   q_action_func_type  action_func
)
{
   q_generic_item_type *item_ptr = NULL;
      /* Used in the traversal to point to the current item
      */
   q_generic_item_type *prev_ptr = NULL;
      /* Used in the traversal to point to the item previous to
      ** the current item.  This makes removing the current item
      ** a constant time operation
      */

   /* User must provide a compare function, otherwise, this is
   ** meaningless.
   */
   if( compare_func == NULL )
   {
      return;
   }

   q_lock( q_ptr );

   /* item_ptr points to the first item on the list
   */
   item_ptr = (q_generic_item_type*)q_check( q_ptr );
   prev_ptr = NULL;
   
   while( item_ptr != NULL )
   {
      if( compare_func( item_ptr, NULL ) != 0 )
      {
         /* Remove the item
         */
         if( prev_ptr != NULL )
         {
            /* Remove from the middle or tail
            */
            prev_ptr->link.next_ptr = item_ptr->link.next_ptr;
            item_ptr->link.next_ptr = NULL;
            if ( (void *)prev_ptr->link.next_ptr == (void *)q_ptr ) {
              q_ptr->link.prev_ptr = (void *) prev_ptr;
            }

            /*  ______________________________________
            ** | THIS IS THE REQUIRED FIX RIGHT HERE! |
            ** |______________________________________|
            */

            q_ptr->cnt--;
         }
         else
         {
            /* Remove from the head
            */
            q_get( q_ptr );
         }

         /* Call the action function if there is one
         */
         if( action_func )
         {
            action_func( item_ptr, param );
         }
         break;
      }

      /* Move on to the next item
      */
      prev_ptr = item_ptr;
      item_ptr = (q_generic_item_type*)q_next( q_ptr, &item_ptr->link );
   } /* END while traversing the queue */

   q_free( q_ptr );
   return;

} /* END q_linear_delete */


#endif  // ifdef FEATURE_CAMCORDER_MALLOC


/* a linked list memory object
*/

/* Memory allocator and deallocator passed into the decoder
** must be of these types.
*/

camcorderutil_llmo_type *
camcorderutil_llmo_constructor(uint32 nBytes,CCD_MALLOC_FN mlc,CCD_FREE_FN fr)
{
  camcorderutil_llmo_type *retval;

  retval = (camcorderutil_llmo_type *) mlc(sizeof(camcorderutil_llmo_type));

  if(retval == NULL )
  {
    CAMCORDER_MSG_ERROR("malloc error",0,0,0);
  }
  else 
  {
    retval->data = (uint8 *) mlc(nBytes);

    if(retval->data == NULL )
    {
      fr(retval);
      retval = NULL;
    }
    else 
    {
      retval->header.mlc  = mlc;
      retval->header.fr   = fr;
      retval->header.next = retval->header.previous = NULL;
      retval->header.nBytes = nBytes;
  
      retval->header.read_idx =
        retval->header.write_idx = 
        retval->header.max_write = 0;
    } 
  }

  return retval;
}


void camcorderutil_llmo_destructor(
  camcorderutil_llmo_type *in)
{
  void *check;

  /* If in pointer is NULL then return */
  if(in == NULL)
  {
    return;
  }

  /* traverse to the head packet */
  while(in->header.previous )
  {
    check = (void *) in;
    in = (camcorderutil_llmo_type *) in->header.previous;
    ASSERT(check == in->header.next );
  }

  do {
    check = in->header.next;
    in->header.fr(in->data);
    in->header.fr(in);
    in = (camcorderutil_llmo_type *) check;
  } while ( check != NULL );

}

/*
** To write in place in memory, call
**    camcorderutil_llmo_write_inplace_step1()
**    to get target memory pointer and target memory size;
** Then call camcorderutil_llmo_write_inplace_step2()
**    to finalize the write
*/

/*  returns memory pointer
*/
uint8 *camcorderutil_llmo_write_inplace_step1(
  camcorderutil_llmo_type **inout,  
  int32 *nBytes  /* input=desired size, output=actual size */
  )
{
  camcorderutil_llmo_type *in_ptr,*new_packet;
  int32 max_nBytes;

  in_ptr = *inout;

  /* must retain byte alignment */
  if(1 == (in_ptr->header.write_idx & 1))
  {
    in_ptr->header.write_idx++;
  }
  if(0 != (in_ptr->header.write_idx & 3 ))
    in_ptr->header.write_idx += 2;

  while (  
    (max_nBytes =  ((int32)in_ptr->header.nBytes - in_ptr->header.write_idx)
                 - (int32)sizeof(camcorderutil_llmo_data_header_type) )
      < *nBytes )
  {
    in_ptr->header.write_idx = -1;  /* close this off */

    /* allocate a new container using the same size the client used
    ** for the first one
    */
    new_packet =
      camcorderutil_llmo_constructor(MAX((uint32)*nBytes,in_ptr->header.nBytes),
              in_ptr->header.mlc,in_ptr->header.fr);
    if(new_packet == NULL )
      return NULL;
    in_ptr->header.next = *inout = new_packet;
    new_packet->header.previous = in_ptr;
    in_ptr = *inout;
  }

  *nBytes = max_nBytes;
  return &in_ptr->data[in_ptr->header.write_idx + 
                       (int32)sizeof(camcorderutil_llmo_data_header_type)
                      ];
}


/* this will expand the size by adding another packet to
** the linked list.
**
** in and nBytes are input/output
*/
void camcorderutil_llmo_write_inplace_step2(
  camcorderutil_llmo_type *in,
  int32  nBytes_written,
  const camcorderutil_llmo_data_header_type *inp_header
  )
{
  camcorderutil_llmo_data_header_type *head;

  /*lint -e{826} suspicious pointer to pointer conversion */
  head = (camcorderutil_llmo_data_header_type *) 
     &in->data[in->header.write_idx];

  memcpy(head,inp_header,sizeof(camcorderutil_llmo_data_header_type));
  head->size = nBytes_written;

  in->header.write_idx += nBytes_written
             + (int32)sizeof(camcorderutil_llmo_data_header_type);
  in->header.max_write = in->header.write_idx;
}

/* return 0 for success, 1 for failure */
int
camcorderutil_llmo_write(
  camcorderutil_llmo_type **in,
  int32 nBytes,
  const uint8 *data,
  const camcorderutil_llmo_data_header_type *inp_header
  )
{
  int32 nBytes_available = nBytes;
  uint8 *space;
  space = 
    camcorderutil_llmo_write_inplace_step1(in,&nBytes_available);
  if(space == NULL )
    return 1;

  memcpy(space,data,(uint32)nBytes);

  camcorderutil_llmo_write_inplace_step2(*in,nBytes,inp_header);

  return 0;

}

uint8 * 
camcorderutil_llmo_read(
  camcorderutil_llmo_type **in,
  int32 *nBytes,
  camcorderutil_llmo_data_header_type *out_header
  )
{
  camcorderutil_llmo_data_header_type *head;
  uint8 *retval;
  camcorderutil_llmo_type *inptr;
  int32 maxread;

  inptr = *in;

  /* must retain byte alignment */
  if(1 == (inptr->header.read_idx & 1 ))
  {
    inptr->header.read_idx++;
  }
  if(0 != (inptr->header.read_idx & 3))
    inptr->header.read_idx += 2;

  maxread = (inptr->header.max_write - inptr->header.read_idx)
         - (int32)sizeof(camcorderutil_llmo_data_header_type);
  if(maxread <= 0 )
    return NULL;

  /*lint -e{826} suspicious pointer to pointer conversion */
  head = (camcorderutil_llmo_data_header_type *) 
     &inptr->data[inptr->header.read_idx];
  if(head->size > maxread )
    return NULL;

  memcpy(out_header,head,sizeof(camcorderutil_llmo_data_header_type));

  *nBytes = head->size;

  inptr->header.read_idx += 
    (int32)sizeof(camcorderutil_llmo_data_header_type);

  retval = &inptr->data[inptr->header.read_idx];

  inptr->header.read_idx += *nBytes;

  if(inptr->header.read_idx >= inptr->header.max_write &&
     inptr->header.write_idx == -1 )
  {
    *in = (camcorderutil_llmo_type *) inptr->header.next;
  }

  return retval;
}
