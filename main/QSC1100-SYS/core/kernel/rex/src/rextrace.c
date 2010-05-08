/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             R E X  T R A C E

EXTERNALIZED FUNCTIONS
rex_trace_isr_start
rex_trace_task_switch
rex_trace_task_enter
rex_trace_task_leave

Copyright (c) 2005 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                         EDIT HISTORY FOR MODULE

 $Header: //source/qcom/qct/core/kernel/rex/main/latest/rextrace.c#1 $

 when who what, where, why
 -------- --- ----------------------------------------------------------
 08/29/05 gfr Created

===========================================================================*/

/*===========================================================================

                       INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_REX_TRACE

#include "rex.h"
#include "task.h"
#include "nv.h"
#include "tmc.h"

/*===========================================================================

                     TYPES AND DEFINITIONS FOR MODULE

===========================================================================*/

/* Current and best task pointers from rex.c */
extern rex_tcb_type *rex_curr_task;
//extern rex_tcb_type *rex_best_task;


/* Trace events type */
typedef enum
{
   REX_TRACE_TASK_LEAVE,
   REX_TRACE_TASK_ENTER,
   REX_TRACE_ISR_START
} rex_trace_event_type;


/* Data stored for a task trace event */
typedef struct
{
   rex_tcb_type *tcb_ptr;
   uint32 pc;
} rex_trace_task_type;


/* Data stored for an ISR trace event */
typedef struct
{
   uint32 vector;
   void *handler;
   rex_tcb_type *curr_task;
} rex_trace_isr_type;


/* A single trace will either be for a task or ISR event, so we make a union
of the two data structures to save memory */
typedef union
{
   rex_trace_task_type task;
   rex_trace_isr_type isr;
} rex_trace_data_type;


/* Stored event trace item */
typedef struct
{
   rex_trace_event_type event;
   timetick_type time;
   rex_trace_data_type data;
   int rex_int_status;
} rex_trace_type;


/* Maintain a list of the past trace events */
#define REX_TRACE_LENGTH 1000
/*lint -e715 arg not referenced */
static rex_trace_type rex_trace[REX_TRACE_LENGTH];
static uint32 rex_trace_index = 0;

/* Specifies if the rex trace module is enabled */
boolean rex_trace_enabled;

/* Macro to go to the next trace index */
#define NEXT_TRACE() \
INTLOCK(); \
if (++rex_trace_index >= REX_TRACE_LENGTH) rex_trace_index = 0; \
INTFREE()


/*===========================================================================

                  FUNCTION DEFINITIONS FOR MODULE

===========================================================================*/

/*===========================================================================

FUNCTION rex_trace_init

DESCRIPTION
Intializes the rex trace module.

===========================================================================*/
void rex_trace_init(void)
{
   nv_cmd_type 	    ncmd; 
   
   ncmd.cmd        = NV_READ_F;
   ncmd.data_ptr   = (nv_item_type *) &rex_trace_enabled;
   ncmd.item       = (nv_items_enum_type)NV_REX_TRACE_I;
   ncmd.tcb_ptr    = rex_self();
   ncmd.sigs       = TMC_NV_CMD_SIG;
   ncmd.done_q_ptr = NULL;
   
   (void) rex_clr_sigs( rex_self(), TMC_NV_CMD_SIG );
   nv_cmd( &ncmd); /* read the data*/
   (void) rex_wait( (rex_sigs_type) TMC_NV_CMD_SIG );
   (void) rex_clr_sigs( rex_self(), TMC_NV_CMD_SIG);

   if ( ncmd.status != NV_DONE_S )
     rex_trace_enabled = FALSE;
}


/*===========================================================================

FUNCTION rex_trace_task_leave

DESCRIPTION
Traces a task leaving (rex_wait, or pre-emption).

===========================================================================*/
void rex_trace_task_leave (uint32 pc)
{
   rex_trace[rex_trace_index].event = REX_TRACE_TASK_LEAVE;
   rex_trace[rex_trace_index].time = timetick_get();
   rex_trace[rex_trace_index].data.task.tcb_ptr = rex_curr_task;
   rex_trace[rex_trace_index].data.task.pc = pc;
   rex_trace[rex_trace_index].rex_int_status = rex_ints_are_locked();
   
   NEXT_TRACE();
}



/*===========================================================================

FUNCTION rex_trace_task_enter

DESCRIPTION
Traces the start of a task running.

===========================================================================*/
void rex_trace_task_enter (uint32 pc)
{
   rex_trace[rex_trace_index].event = REX_TRACE_TASK_ENTER;
   rex_trace[rex_trace_index].time = timetick_get();
   rex_trace[rex_trace_index].data.task.tcb_ptr = rex_curr_task;
   rex_trace[rex_trace_index].data.task.pc = pc;
   rex_trace[rex_trace_index].rex_int_status = rex_ints_are_locked();
   
   NEXT_TRACE();
}



/*===========================================================================

FUNCTION rex_trace_isr_start

DESCRIPTION
Traces the start of an ISR.

===========================================================================*/
void rex_trace_isr_start (uint32 vector, void *fptr)
{
   rex_trace[rex_trace_index].event = REX_TRACE_ISR_START;
   rex_trace[rex_trace_index].time = timetick_get();
   rex_trace[rex_trace_index].data.isr.curr_task = rex_curr_task;
   rex_trace[rex_trace_index].data.isr.vector = vector;
   rex_trace[rex_trace_index].data.isr.handler = fptr;
   rex_trace[rex_trace_index].rex_int_status = rex_ints_are_locked();
   
   NEXT_TRACE();
}


#endif /* FEATURE_REX_TRACE */

