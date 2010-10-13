
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D I S P L A Y     T A S K

GENERAL DESCRIPTION
  This module contains display task software.

REFERENCES

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2001 - 2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/display/mdp1/disp_task/main/latest/src/disp_task.c#4 $ $DateTime: 2008/06/11 02:21:08 $ $Author: rkashyap $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/10/08   rk      Defined FEATURE_MDP_VSYNC_RESYNC_TIMER, to help run the s/w vsync logic.
11/07/07   dw      Add support for dynamic allocation of DISP buffers
05/16/07   dw      Replace header file for including updated BREW offsets
03/21/07   cjr     Changed mc.h to tmc.h for both UMTS and 1X targets
03/01/07   trg     Add compile flag to remove timetick_get_ms() for WM.
02/16/07   cjr     Added a support for WinMobile INTLOCK()/INTFREE()
01/25/07   cjr     Fixed packet free logic under DISP_TASK_BUFFER_SHARING
11/14/06   dw      Remove high lint warning/errors
09/19/06   cjr     Added power managment to control MDP clock source
08/18/06   cjr     Added vsync-resync timer and update time debug measurement
08/03/06   dw      Removed all critical/high/medium lint warnings and errors
07/27/06   cjr     Added MDP power off timer signal
07/17/06   dw      Remove FEATURE_DISP_TASK
07/06/06   cjr     Added support for power collpase and complete packet queue
05/10/06   cjr     Added display API queue
04/03/06   cjr     Added MDP direct callback
03/13/06   cjr     Added support for disp queue sharing for MDP1 Async feature
10/13/05   cjr     Added support for MDPOP_QUEUE_EXPAND 
06/13/05   cjr     Initial creation of file.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"             /* Target specific definitions             */
#include "customer.h"           /* Customer specific definitions           */
#include "comdef.h"             /* Definitions for byte, word, etc.        */

#include "rex.h"                /* definition of REX data types            */
#include "task.h"               /* Task definitions and prototypes         */
#include "msg.h"                /* Message logging/reporting services      */
#include "err.h"                /* Error logging/reporting services        */
  
#include "dog.h"                /* Watchdog signal and time definitions    */
#ifndef FEATURE_WINCE
  #ifdef WINCE_TODO
#error code not present
  #endif /* WINCE_TODO */

  #include "tmc.h"                 /* Need for MC signals and prototypes      */  
  
#endif /* NOT FEATURE_WINCE */

#include "sleep.h"              /* Sleep interfaces                        */
  
#include <memory.h>             /* Memory manipulation routines            */
#include <stdlib.h>             /* Dynamic memory allocation routines      */
 
#define DISP_TASK_C
#include "mdp_drv.h"
#include "mdp_api_proc.h"
#include "disp_task.h"
#include "drvLib.h"

#ifdef FEATURE_BREW
  #include "OEMFeatures.h"
#endif
  
#ifdef FEATURE_PMEM
  #include "pmem.h"
#endif

#if defined( FEATURE_MDP2 ) && defined( FEATURE_DUAL_PROC_MODEM_DISPLAY )
#error code not present
#endif /* FEATURE_MDP2 && FEATURE_DUAL_PROC_MODEM_DISPLAY */

extern rex_tcb_type * mdp_last_tcb;      //mdp_api_proc.c
extern rex_tcb_type * mdp_last_tcb_API;  //mdp_api_proc.c

// mdp driver fd
extern int mdp_fd;  

#ifdef FEATURE_MDP2
#error code not present
#endif

#define FEATURE_MDP_VSYNC_RESYNC_TIMER
#ifdef FEATURE_MDP_VSYNC_RESYNC_TIMER
// MDP driver detect LCD and provide this timer.
extern int mdp_vsync_resync_time;
// This flag is to call mdp_set_vsync() API
boolean disp_mdp_resync_vsync_flag=TRUE;
// This flag is to clear timer if the timer is set previously. 
// Otherwise, do not call to clear the timer.
boolean disp_vsync_resync_timer_set=FALSE;
#endif

extern void disp_lcd_switch_control ( boolean enable );
#define DISP_MALLOC(size)            malloc(size)

/*---------------------------------------------------------------------------
** Display Command Queue, Free Queue and Available Packets
**---------------------------------------------------------------------------
*/
LOCAL disp_packet_type_API ** dispQueueHPtr_API;
LOCAL disp_packet_type * dispQueueHPtr;
LOCAL disp_packet_type * dispQueueTPtr;
LOCAL disp_packet_type * dispImgListTailPtr;
LOCAL disp_packet_type * dispCompleteQueue;
LOCAL uint32 disp_imglist_total;
// Available Packets 
// int DISP_NUM_PACKETS = 5;
LOCAL disp_packet_type   dispImgList [DISP_NUM_PACKETS];
LOCAL rex_timer_type     disp_rpt_timer;  /* Watch Dog Timer              */

#ifdef FEATURE_MDP_VSYNC_RESYNC_TIMER
LOCAL rex_timer_type     disp_vsync_resync_timer;  /* mdp vsync resync timer */
#endif

LOCAL disp_packet_type_API * dispImgListTailPtr_API;
LOCAL uint32 disp_imglist_total_API;
LOCAL disp_packet_type_API   dispImgList_API [DISP_NUM_PACKETS_API];

LOCAL boolean disp_task_initialized = FALSE;
LOCAL rex_crit_sect_type   disp_task_crit_sect;
LOCAL timetick_type disp_task_timetick[DISP_TASK_TIMETICK_MAX_INDEX];
LOCAL uint32 disp_task_calling_index = 0;
LOCAL uint32 disp_task_finish_index = 0;

#ifdef FEATURE_MDP_POWER_OFF_TIMER      
#error code not present
#endif

/*===========================================================================

                DECLARATIONS FOR MODULE

===========================================================================*/
#ifndef FEATURE_WINCE
  #ifdef WINCE_TODO
#error code not present
  #endif /* WINCE_TODO */

/*===========================================================================

FUNCTION disp_dem_handoff_cb()

DESCRIPTION
  Callback function to handle the power collapse for both Modem and App side

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
  #if defined( FEATURE_MDP2 ) && defined( FEATURE_DUAL_PROC_MODEM_DISPLAY )
#error code not present
  #endif /* FEATURE_MDP2 && FEATURE_DUAL_PROC_MODEM_DISPLAY */
#endif /* NOT FEATURE_WINCE */

/*===========================================================================

FUNCTION disp_task

DESCRIPTION
  This procedure is the entrance procedure for Display Task
  It contains the main processing loop for the Display Task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  disp_tcb.

===========================================================================*/
static unsigned int disp_task_cnt = 0;

void disp_task
(
dword ignored
)
{
  rex_sigs_type sigs;


  /* Initialize DISP Task
  */
  disp_task_init ();

#ifndef FEATURE_WINCE
  #ifdef FEATURE_MULTIMODE_ARCH
    /*  Process task start procedure from task controller. */
    (void) task_start (DISP_RPT_TIMER_SIG, DOG_DISP_RPT, &disp_rpt_timer);
  #else /* FEATURE_MULTIMODE_ARCH */
    /* Wait for start signal from Main Control task
    */
    mc_task_start (DISP_RPT_TIMER_SIG, DOG_DISP_RPT, &disp_rpt_timer);
  #endif /* FEATURE_MULTIMODE_ARCH */

  /* Clear start signal
  */
  (void) rex_clr_sigs (&disp_tcb, TASK_START_SIG);

  /* Set the watchdog report timer signal so that we'll
  ** reset the watchdog as soon as we start checking for events
  */
  (void) rex_set_sigs (&disp_tcb, DISP_RPT_TIMER_SIG);
#endif /* NOT FEATURE_WINCE */

  /* Infinite loop -- process events forever
  */
  for (;;)
  {

    /* Wait for something to happen
    */
    sigs = rex_wait( DISP_RPT_TIMER_SIG      |   /* Watchdog report timer  */
                     DISP_CMD_Q_SIG          |   /* Command queue signal   */
                     DISP_CMD_MDP_CB         |   /* MDP callback handle    */
                     DISP_MDP_POWER_OFF_TIMER_SIG | /* MDP power off hanlde */                     
                     TASK_STOP_SIG           |   /* Task stop from MC      */
                     TASK_OFFLINE_SIG);          /* Task offline from MC   */

    //lint -e551  Suppress warning on non-accessed variable used for debug
    disp_task_cnt++; //job cnt

    #ifndef FEATURE_WINCE 
      #ifdef WINCE_TODO
#error code not present
      #endif /* WINCE_TODO */

    /* If watchdog report timer expired, report and restart the timer
    */
    if ((sigs & DISP_RPT_TIMER_SIG) != 0)
    {
      // we need to call dog_report() right before rex_wait() inside mdp_disp_update_proc()
      // Otherwise, it'll cause the watchdog timeout
      // Or define FEATURE_ENHACNED_REX_TASK feature
      // FEATURE_ENHACNED_REX_TASK feature will disable watchdog while rex_wait()
      dog_report (DOG_DISP_RPT);
      (void) rex_set_timer (&disp_rpt_timer, DOG_DISP_RPT_TIME);
    }
    #endif /* NOT FEATURE_WINCE */

    /*---------------------------------------------------------------------*/
    /* Process command complete callback
    */
    if ((sigs & DISP_CMD_MDP_CB) != 0 )
    {
      (void) rex_clr_sigs (&disp_tcb, DISP_CMD_MDP_CB);    
      disp_task_cb_clean_up();
    }

    /*---------------------------------------------------------------------*/
    /* Process the queued commands
    */
    if ((sigs & DISP_CMD_Q_SIG) != 0 )
    {
      /* Clear the 'queued commands' signal.
      */
      (void) rex_clr_sigs (&disp_tcb, DISP_CMD_Q_SIG);
      disp_cmd_parser();
    }

#ifdef FEATURE_MDP_POWER_OFF_TIMER    
#error code not present
#endif

    /*---------------------------------------------------------------------*/
    /* We're supposed to shutdown.
    */
    if ((sigs & TASK_STOP_SIG) != 0)
    {
      disp_task_shutdown ();
    }

    /*---------------------------------------------------------------------*/

    /* We're supposed to go offline.
    */
    if ((sigs & TASK_OFFLINE_SIG) != 0)
    {
      disp_task_offline ();
    }

    /*---------------------------------------------------------------------*/

  } /* end of infinite loop */
} /* end disp_task */


/*===========================================================================

FUNCTION disp_vsync_resync_timer_cb()

DESCRIPTION
  This is a timer callback function to set vsync-resync flag.

DEPENDENCIES
  mdp_drv_init() is called to setup MDP HW block and initialize the
  associated driver structures and interrupt controllers.

RETURN VALUE
  None
  
SIDE EFFECTS
  Resets MDP HW&SW
===========================================================================*/
#ifdef FEATURE_MDP_VSYNC_RESYNC_TIMER
void disp_vsync_resync_timer_cb(timer_cb_data_type data)
{
  disp_mdp_resync_vsync_flag = TRUE;
}
#endif

/*===========================================================================

FUNCTION disp_task_init

DESCRIPTION
  This procedure initializes the disp task.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL void disp_task_init (void)
{
  if (disp_task_initialized == TRUE)
    return;

#ifdef IMAGE_MODEM_PROC 
  disp_init();
#endif

#ifdef FEATURE_WINCE
#error code not present
#endif

  rex_init_crit_sect(&disp_task_crit_sect); 
  /* Initialize timers
  */
  rex_def_timer (&disp_rpt_timer, &disp_tcb, DISP_RPT_TIMER_SIG);  

#ifdef FEATURE_MDP_POWER_OFF_TIMER      
#error code not present
#endif
#ifdef FEATURE_MDP_VSYNC_RESYNC_TIMER
  #ifdef FEATURE_WINCE
#error code not present
  #else /* NOT FEATURE_WINCE */
    timer_def((timer_ptr_type)&disp_vsync_resync_timer,NULL/*group*/,&disp_tcb,0,disp_vsync_resync_timer_cb,0/*arg*/);
  #endif /* FEATURE_WINCE */
#endif

  disp_task_initialized = TRUE;

  // OK..disp_task is initialized. Let's initialize disp task list
  disp_imglist_init();

#ifndef FEATURE_WINCE
  #ifdef WINCE_TODO
#error code not present
  #endif /* WINCE_TODO */

  #if defined( FEATURE_MDP2 ) && defined( FEATURE_DUAL_PROC_MODEM_DISPLAY )
#error code not present
  #endif
#endif /* NOT FEATURE_WINCE */

  MSG_LOW("DISP task initialized",0,0,0);

} /* end disp_task_init */


/*===========================================================================

FUNCTION disp_q_get_complete()

DESCRIPTION
  This function is to get the next job from display queue processed by display
  driver

DEPENDENCIES
  disp_drv_init() is called to setup disp task module and initialize the
  associated task structures.

RETURN VALUE
  the next job node

SIDE EFFECTS
  None

===========================================================================*/
LOCAL disp_packet_type * disp_q_get_complete(void)
{
  disp_packet_type * node;

  DISP_INTLOCK();
  node = dispCompleteQueue;

  if (node != NULL)
  {
    dispCompleteQueue = node->next;

    if (node->next != NULL)
      node->next->prev = NULL;

    node->next = node->prev = NULL;
  }
  DISP_INTFREE();

  return node;
}

/*===========================================================================

FUNCTION disp_q_put_complete()

DESCRIPTION
  This function is to put the next job into display queue processed by display
  driver

DEPENDENCIES
  disp_drv_init() is called to setup disp task module and initialize the
  associated task structures.

RETURN VALUE
  the next job node

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void disp_q_put_complete(disp_packet_type *node)
{
  if (NULL == node)
    return;

  DISP_INTLOCK();
  if (dispCompleteQueue == NULL)
  {
    dispCompleteQueue = node;
    node->prev = node->next = NULL;
  }
  else
  {
    dispCompleteQueue->next = node;
    node->prev = dispCompleteQueue;
    node->next = NULL;        
  }  
  DISP_INTFREE();
}

/*===========================================================================

FUNCTION disp_task_cb_clean_up

DESCRIPTION
  This is function is to purge cb list

DEPENDENCIES
  disp_task_init must have been called to init the command queue.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void disp_task_cb_clean_up(void)
{
  disp_packet_type * node;

  while ((node = disp_q_get_complete()) != NULL)
  {
    disp_task_default_cb((void *)node, MDP_SUCCESSFUL);  
  }
}

/*===========================================================================

FUNCTION disp_task_default_cb

DESCRIPTION
  This is the callback routine to be called by the driver when the update is 
  complete.  From display task callback, the user callback will get called.

DEPENDENCIES
  disp_task_init must have been called to init the command queue.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void disp_task_default_cb(void *arg, MDP_STATUS status)
{
  disp_packet_type * cmd_ptr = (disp_packet_type *)arg;
  disp_cb_type cb;

// Remove timetick_get_ms() for specific WinMob builds.
#ifndef uldr
  if (disp_task_dbg)
  {
    if (disp_task_finish_index != disp_task_calling_index)
    {
      disp_task_timetick[disp_task_finish_index] = timetick_get_ms() - 
                                                   disp_task_timetick[disp_task_finish_index];
      DISP_MSG("finished - mdp_disp_update_proc(%d) - %d msec elapsed ",
               disp_task_finish_index, disp_task_timetick[disp_task_finish_index],0);     

      if (++disp_task_finish_index >= DISP_TASK_TIMETICK_MAX_INDEX)
        disp_task_finish_index = 0;
    }
  }
#endif // #ifndef uldr

#ifdef DISP_TASK_BUFFER_SHARING
  // if it's being registered, do not free it
  cmd_ptr->done = TRUE;
  if (!cmd_ptr->registered)
  {
#endif // DISP_TASK_BUFFER_SHARING                        
    // releasing disp packet asap
    cb = cmd_ptr->cb;
    disp_imglist_free(cmd_ptr);

    // user callback function
    if (NULL != cb.cb_func)
    {
      cb.cb_func(cb.arg, status);
    }
#ifdef DISP_TASK_BUFFER_SHARING          
  }
#endif    
}


/*===========================================================================

FUNCTION disp_cmd_parser

DESCRIPTION
  This procedure processes requests received by disp_cmd(). See disp_task.h for
  a description of the actual CMX packets.

DEPENDENCIES
  disp_task_init must have been called to init the command queue.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void disp_cmd_parser (void)
{
  disp_packet_type *cmd_ptr;
  disp_cb_type cb;
  disp_cb_type disp_cb;
  MDP_STATUS result;

  /* While there are commands to process, process each command.
  */
  while ((cmd_ptr = (disp_packet_type *) disp_q_get ()) != NULL)
  {

    /* Process each command type.
    */
    switch (cmd_ptr->cmd)
    {
      case DISP_CMD_DISP_UPDATE:

#ifdef FEATURE_MDP_POWER_OFF_TIMER      
#error code not present
#endif

#ifdef FEATURE_MDP_VSYNC_RESYNC_TIMER                         
        if (disp_mdp_resync_vsync_flag)
        {
          disp_mdp_resync_vsync_flag = FALSE;                            
          mdp_set_vsync();
          // timer base vsync-resync
          if (0 != mdp_vsync_resync_time)
          {
            if (disp_vsync_resync_timer_set)
              rex_clr_timer(&disp_vsync_resync_timer);

            // clears the signal and starts the timer
            rex_set_timer(&disp_vsync_resync_timer, mdp_vsync_resync_time);
            disp_vsync_resync_timer_set = TRUE;
          }
          else
          {
            disp_mdp_resync_vsync_flag = TRUE;                            
          }
        }
#endif

        // if the previous update is complete....
        // no need for INTLOCK since we will receive the complete signal 
        // even if we miss here
        if (NULL != dispCompleteQueue)
          disp_task_cb_clean_up();

// Remove timetick_get_ms() for specific WinMob builds.
#ifndef uldr
        // MDP start timestamp
        if (disp_task_dbg)
        {
          DISP_MSG("calling  - mdp_disp_update_proc(%d) ",disp_task_calling_index,0,0);         
          disp_task_timetick[disp_task_calling_index] = timetick_get_ms();     
          if (++disp_task_calling_index >= DISP_TASK_TIMETICK_MAX_INDEX)
            disp_task_calling_index = 0;
        }
#endif // #ifndef uldr

        // disp task callback handler
        disp_cb.cb_func = disp_task_default_cb;
        disp_cb.arg = (void *)cmd_ptr;

        // kick off update
        result = mdp_disp_update_proc(&cmd_ptr->mdpImg, &disp_cb);

        // if mdp fails to update and do a callback...
        if (NULL != disp_cb.cb_func)
          disp_cb.cb_func(disp_cb.arg, result);

        break;

      case DISP_CMD_API_UPDATE:
        //lint -e740 Suppress warning for incompatible pointer cast
        result = (unsigned int) 
                 mdp_api_ioctl((MDP_API_IOCTL)((disp_packet_type_API *)cmd_ptr)->api_id,
                               ((disp_packet_type_API *)cmd_ptr)->arg);

        cb = cmd_ptr->cb;
        disp_imglist_free_API((disp_packet_type_API *)cmd_ptr);

        // user callback function
        if (NULL != cb.cb_func)
        {
          cb.cb_func(cb.arg, result);
        }
        break;

      case DISP_CMD_INVALID:
      default:
        MSG_ERROR ("Error: Illegal vs command %x", cmd_ptr->cmd, 0, 0);
    }
  }  

#ifdef FEATURE_MDP_POWER_OFF_TIMER      
#error code not present
#endif  
} /* end disp_cmd_parser */

/*===========================================================================

FUNCTION disp_task_offline

DESCRIPTION
  This procedure performs the transition to "offline" for the disp task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void disp_task_offline (void)
{

#ifndef FEATURE_WINCE
  #ifdef FEATURE_MULTIMODE_ARCH
    /* Process task offline procedure from task controller.
    */
    (void) task_offline();
  #else /* FEATURE_MULTIMODE_ARCH */
    /* Acknowledge the offline signal
    */
    (void) rex_set_sigs (&mc_tcb, MC_ACK_SIG);
  #endif /* FEATURE_MULTIMODE_ARCH */
#endif /* NOT FEATURE_WINCE */

  /* Clear the offline signal, and return
  */
  (void) rex_clr_sigs (&disp_tcb, TASK_OFFLINE_SIG);

} /* end disp_task_offline */

/* <EJECT> */
/*===========================================================================

FUNCTION disp_task_shutdown

DESCRIPTION
  This procedure performs shutdown processing for the disp task.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void disp_task_shutdown (void)
{
#ifndef FEATURE_WINCE
  #ifdef FEATURE_MULTIMODE_ARCH
    /* Process task stop procedure from task controller.
    */
    (void) task_stop();
  #else /* FEATURE_MULTIMODE_ARCH */
    /* Acknowledge the stop signal
    */
    (void) rex_set_sigs (&mc_tcb, MC_ACK_SIG);
  #endif /* FEATURE_MULTIMODE_ARCH */
#endif /* NOT FEATURE_WINCE */

  /* Clear the stop signal, and return
  */
  (void) rex_clr_sigs (&disp_tcb, TASK_STOP_SIG);

} /* end disp_task_shutdown */


/*===========================================================================

FUNCTION disp_q_get()

DESCRIPTION
  This function is to get the next job from the display queue 

DEPENDENCIES
  disp_drv_init() is called to setup disp task module and initialize the
  associated task structures.

RETURN VALUE
  the next job node

SIDE EFFECTS
  None

===========================================================================*/
LOCAL disp_packet_type * disp_q_get(void)
{
  disp_packet_type * node;

  if (disp_task_initialized == FALSE)
    return NULL;

  rex_enter_crit_sect(&disp_task_crit_sect);
  node = dispQueueHPtr;

#ifndef FEATURE_MDP2
  // for MDP1/MSM6K platform, we're executing API immediately
  // we should evaluate to remove this block to execute API in order
  while (NULL != node)
  {
    if (DISP_CMD_API_UPDATE == node->cmd)
      break;
    else
      node = node->next;
  }
#endif

  if (NULL == node)
    node = dispQueueHPtr;

  if (NULL != node)
    disp_imglist_remove(node);

  rex_leave_crit_sect(&disp_task_crit_sect);  

  return node;
}

/*===========================================================================

FUNCTION disp_clean_overlap()

DESCRIPTION
  This function is to optimize the queue list by removing any update window 
  that is covered by this node's update window 

DEPENDENCIES
  disp_drv_init() is called to setup disp task module and initialize the
  associated task structures.

RETURN VALUE
  the next job node

SIDE EFFECTS
  None

===========================================================================*/

LOCAL MDPIMG_LIST disp_mdp_list;

LOCAL void disp_clean_overlap(disp_packet_type *node)
{  
  disp_packet_type *this_node, *next_node;
  disp_cb_type cb;

  disp_mdp_list.img = node->mdpImg;

  // adjusting to BREW Screen Offset
#ifdef FEATURE_BREW
  disp_mdp_list.img.lcd_x += BREW_OFFSET_X;
  disp_mdp_list.img.lcd_y += BREW_OFFSET_Y;  
#endif  

  mdp_touch_img(&disp_mdp_list);

  node->real_cwin = disp_mdp_list.img.cwin;

  this_node = dispQueueHPtr;

  while (this_node != NULL)
  {
    next_node = this_node->next;

    if ( (this_node->cmd == DISP_CMD_DISP_UPDATE) &&         
         ((this_node->mdpImg.mdpOp & MDPOP_FAST_QUEUE) ||
          (this_node->mdpImg.layer != node->mdpImg.layer)))
    {
      if ((node->real_cwin.x1 <= this_node->real_cwin.x1) &&
          (node->real_cwin.x2 >= this_node->real_cwin.x2) &&
          (node->real_cwin.y1 <= this_node->real_cwin.y1) &&
          (node->real_cwin.y2 >= this_node->real_cwin.y2))
      {
        disp_imglist_remove(this_node);

#ifdef DISP_TASK_BUFFER_SHARING
/* for DISP_TASK_BUFFER_SHARING, we need to remove it from the queue
   but can't free it if it's still being owned by the app */
        // if it's being registered, do not free it
        this_node->done = TRUE;
        if (!this_node->registered)
        {
#endif // DISP_TASK_BUFFER_SHARING                

          cb = this_node->cb;
          disp_imglist_free(this_node);       

          if (NULL != cb.cb_func)
          {
            cb.cb_func(cb.arg, MDP_SUCCESSFUL);
          }
#ifdef DISP_TASK_BUFFER_SHARING      
        }
#endif //DISP_TASK_BUFFER_SHARING    
      }
    }

    this_node = next_node;
  }
}


/*===========================================================================

FUNCTION disp_node_already_in_queue()

DESCRIPTION
  This function is to check to see if the node is already in queue or not 

DEPENDENCIES
  disp_drv_init() is called to setup disp task module and initialize the
  associated task structures.

RETURN VALUE
  the next job node

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean disp_node_already_in_queue(disp_packet_type * this_node)
{
  disp_packet_type * node;
  boolean result;
  
  if (this_node == NULL)
    return FALSE;
    
  rex_enter_crit_sect(&disp_task_crit_sect);

  node = dispQueueHPtr;
  result = FALSE;
  while (node != NULL)
  {
    if (this_node == node)
    {
      result = TRUE;
      break;
    }
    node = node->next;
  }

  rex_leave_crit_sect(&disp_task_crit_sect);  

  return result;
}


/*===========================================================================

FUNCTION disp_q_put()

DESCRIPTION
  This function is to put the next job to the end of the display queue 

DEPENDENCIES
  disp_drv_init() is called to setup disp task module and initialize the
  associated task structures.

RETURN VALUE
  the next job node

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void disp_q_put(disp_packet_type *node)
{
  if (node == NULL)
    return;
    
  if (disp_task_initialized == FALSE)
    return;

  rex_enter_crit_sect(&disp_task_crit_sect);

  // if this node is already in queue
  if (disp_node_already_in_queue(node))
  {
    rex_leave_crit_sect(&disp_task_crit_sect);    
    return;
  }    
  
  // clean the queue list first
  if ((dispQueueTPtr != NULL) &&
      (node->cmd == DISP_CMD_DISP_UPDATE))
  {
    disp_clean_overlap(node);
  }
  
#ifdef DISP_TASK_BUFFER_SHARING      
  if (node->cmd == DISP_CMD_DISP_UPDATE)
  {
    // node is being pushed into queue and
    // it hasn't been executed by disp task
    node->done = FALSE;      
  }
#endif      

  if (dispQueueTPtr == NULL)
  {
    dispQueueHPtr = node;
    dispQueueTPtr = node;    
    node->prev = node->next = NULL;    
  }
  else
  {
    if (dispQueueTPtr != node)
    {
      dispQueueTPtr->next = node;
      node->prev = dispQueueTPtr;
      node->next = NULL;        
      dispQueueTPtr = node;
    }
  }  
  rex_leave_crit_sect(&disp_task_crit_sect);  
}

/*===========================================================================

FUNCTION disp_q_put_API()

DESCRIPTION
  This function is to put the next job to the end of the display queue for
  API processing

DEPENDENCIES
  disp_drv_init() is called to setup disp task module and initialize the
  associated task structures.

RETURN VALUE
  the next job node

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void disp_q_put_API(disp_packet_type_API *node)
{
  /* After casting disp_packet_type_API* to disp_packet_type* , 
  ** only the first 4 elements of node are valid, the rest should 
  ** not be used to prevent invalid memory access.
  */
  disp_q_put((disp_packet_type *)node); 
}

/*===========================================================================

FUNCTION disp_imglist_remove()

DESCRIPTION
  This function is to remove a disp list node

DEPENDENCIES
  disp_drv_init() is called to setup disp task module and initialize the
  associated task structures.

RETURN VALUE
  None

SIDE EFFECTS
  Previous and next nodes are linked together

===========================================================================*/
LOCAL void disp_imglist_remove(disp_packet_type *node)
{
  if (disp_task_initialized == FALSE)
    return;

  rex_enter_crit_sect(&disp_task_crit_sect);

  if (node != NULL)
  {
    if (dispQueueHPtr == node)
      dispQueueHPtr = node->next;

    if (dispQueueTPtr == node)
      dispQueueTPtr = node->prev;

    if (node->prev != NULL)
    {
      node->prev->next = node->next;
    }
    if (node->next != NULL)
    {
      node->next->prev = node->prev;
    }

    node->next = node->prev = NULL;
  }

  rex_leave_crit_sect(&disp_task_crit_sect);  
}

/*===========================================================================

FUNCTION disp_imglist_remove()

DESCRIPTION
  This function is to remove a disp list node for API processing

DEPENDENCIES
  disp_drv_init() is called to setup disp task module and initialize the
  associated task structures.

RETURN VALUE
  None

SIDE EFFECTS
  Previous and next nodes are linked together

===========================================================================*/
LOCAL void disp_imglist_remove_API(disp_packet_type_API *node)
{
  /* After casting disp_packet_type_API* to disp_packet_type* , 
  ** only the first 4 elements of node are valid, the rest should 
  ** not be used to prevent invalid memory access.
  */
  disp_imglist_remove((disp_packet_type *)node); 
}

/*===========================================================================

FUNCTION disp_imglist_get()

DESCRIPTION
  This function is to request one display queue for data processing

DEPENDENCIES
  disp_drv_init() is called to setup disp task module and initialize the
  associated task structures.

RETURN VALUE
  disp packet node if successful otherwise NULL

SIDE EFFECTS
  None

===========================================================================*/
LOCAL disp_packet_type * disp_imglist_get(MDPIMG *mdpImg, MDP_CWIN *old_cwin)
{
  disp_packet_type *node;
  disp_cb_type cb;

  if (mdpImg == NULL)
    return NULL;
    
  if (disp_task_initialized == FALSE)
    return NULL;

  rex_enter_crit_sect(&disp_task_crit_sect);

  node = NULL;
  if ((NULL != dispImgListTailPtr) && (disp_imglist_total > 0))
  {
    node = dispImgListTailPtr;

    if (dispImgListTailPtr->prev != NULL)
    {
      dispImgListTailPtr->prev->next = NULL;
    }
    dispImgListTailPtr = dispImgListTailPtr->prev;

    if (node != NULL)
    {
      disp_imglist_total--;
      DISP_MSG("disp_imglist_total = %d ",disp_imglist_total,0,0);
      node->prev = NULL;
      node->next = NULL;
    }
  }
  else if ( (mdpImg->mdpOp & MDPOP_QUEUE_EXPAND) && (mdpImg->mdpOp & MDPOP_QUEUE_COPY) )
  {
    node = dispQueueHPtr;

    while ((node != NULL) && (node->next != NULL))
      node = node->next;

    while (node != NULL)
    {
      if ((node->orig_bmy_addr == mdpImg->bmy_addr) &&
          (node->mdpImg.mdpOp == mdpImg->mdpOp) &&
          (node->mdpImg.layer == mdpImg->layer))
      {
        *old_cwin = node->mdpImg.cwin;

        disp_imglist_remove(node);
#ifdef DISP_TASK_BUFFER_SHARING
/* for DISP_TASK_BUFFER_SHARING, we need to remove it from the queue
   but can't free it if it's still being owned by the app */
        // if it's being registered, do not free it
        node->done = TRUE;
        if (!node->registered)
        {
#endif // DISP_TASK_BUFFER_SHARING                

          cb = node->cb;
          disp_imglist_free(node);       

          if (NULL != cb.cb_func)
          {
            cb.cb_func(cb.arg, MDP_SUCCESSFUL);
          }
          node = disp_imglist_get(mdpImg,old_cwin);        
#ifdef DISP_TASK_BUFFER_SHARING      
        }
#endif //DISP_TASK_BUFFER_SHARING    

        break;
      }
      node = node->prev;
    }    
  }

  rex_leave_crit_sect(&disp_task_crit_sect);

  return node;
}


/*===========================================================================

FUNCTION disp_imglist_get_API()

DESCRIPTION
  This function is to request one display queue for API processing
  
DEPENDENCIES
  disp_drv_init() is called to setup disp task module and initialize the
  associated task structures.

RETURN VALUE
  disp pakcet API node if successful otherwise NULL

SIDE EFFECTS
  None

===========================================================================*/
LOCAL disp_packet_type_API * disp_imglist_get_API(void)
{
  disp_packet_type_API *node;

  if (disp_task_initialized == FALSE)
    return NULL;

  rex_enter_crit_sect(&disp_task_crit_sect);

  node = NULL;
  if ((NULL != dispImgListTailPtr_API) && (disp_imglist_total_API > 0))
  {
    node = dispImgListTailPtr_API;

    if (dispImgListTailPtr_API->prev != NULL)
    {
      dispImgListTailPtr_API->prev->next = NULL;
    }
    dispImgListTailPtr_API = dispImgListTailPtr_API->prev;

    if (node != NULL)
    {
      disp_imglist_total_API--;
      DISP_MSG("disp_imglist_total_API = %d ",disp_imglist_total_API,0,0);
      node->prev = NULL;
      node->next = NULL;
    }
  }

  rex_leave_crit_sect(&disp_task_crit_sect);

  return node;
}

/*===========================================================================

FUNCTION disp_imglist_free()

DESCRIPTION
  This function is to return a disp packet node
  
DEPENDENCIES
  disp_drv_init() is called to setup disp task module and initialize the
  associated task structures.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void disp_imglist_free(disp_packet_type *node)
{
  if (disp_task_initialized == FALSE)
    return;

  if ((node == dispImgListTailPtr) || (node == NULL))
    return;

  rex_enter_crit_sect(&disp_task_crit_sect);

  disp_imglist_total++;

#ifdef DISP_TASK_BUFFER_SHARING
  node->registered = FALSE;
  node->done = TRUE;
#endif
  node->queue_copy_done = FALSE;

  if (dispImgListTailPtr == NULL)
  {
    dispImgListTailPtr = node;
    dispImgListTailPtr->prev = NULL;
    dispImgListTailPtr->next = NULL;
  }
  else
  {
    node->prev = dispImgListTailPtr;
    node->next = NULL;
    dispImgListTailPtr->next = node;
    dispImgListTailPtr = node;
  }

  // mdp_last_tcb to signal the task waiting for free disp_task queue
  if (mdp_last_tcb != NULL)
  {
    (void) rex_set_sigs(mdp_last_tcb, MDP_COMPLETE_SIG);
    mdp_last_tcb = NULL;
  }

  rex_leave_crit_sect(&disp_task_crit_sect);    
}

/*===========================================================================

FUNCTION disp_imglist_free_API()

DESCRIPTION
  This function is to return a disp packet node for API processing

DEPENDENCIES
  disp_drv_init() is called to setup disp task module and initialize the
  associated task structures.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void disp_imglist_free_API(disp_packet_type_API *node)
{
  if (disp_task_initialized == FALSE)
    return;

  if ((node == dispImgListTailPtr_API) || (node == NULL))
    return;

  rex_enter_crit_sect(&disp_task_crit_sect);

  disp_imglist_total_API++;

  if (dispImgListTailPtr_API == NULL)
  {
    dispImgListTailPtr_API = node;
    dispImgListTailPtr_API->prev = NULL;
    dispImgListTailPtr_API->next = NULL;
  }
  else
  {
    node->prev = dispImgListTailPtr_API;
    node->next = NULL;
    dispImgListTailPtr_API->next = node;
    dispImgListTailPtr_API = node;
  }

  // mdp_last_tcb to signal the task waiting for free disp_task queue
  if (mdp_last_tcb_API != NULL)
  {
    (void) rex_set_sigs(mdp_last_tcb_API, MDP_COMPLETE_SIG);
    mdp_last_tcb_API = NULL;
  }

  rex_leave_crit_sect(&disp_task_crit_sect);    
}

/*===========================================================================

FUNCTION disp_imglist_init()

DESCRIPTION
  This function is to initialize the disp image list

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void disp_imglist_init(void)
{
  int i;
  disp_packet_type *node;
  disp_packet_type_API *node_API;

  dispQueueHPtr = NULL;
  dispQueueTPtr = NULL;
  dispCompleteQueue = NULL;

  //lint -e552  Suppress warning on non-accessed variable used for debug
  dispQueueHPtr_API = (disp_packet_type_API **)(&dispQueueHPtr);

  /////////////////////////////////////////
  // disp packet data queue initialization
  /////////////////////////////////////////
  dispImgListTailPtr = NULL;
  disp_imglist_total = 0;

  for (i=0;i<DISP_NUM_PACKETS;i++)
  {
    node = &dispImgList[i];

#ifndef FEATURE_MDP2
    node->disp_frame = mdp_create_largest_frame(DISP_16BPP);

    if (node->disp_frame == NULL)
      break;
#endif //#ifndef FEATURE_MDP2

    node->prev = NULL;
    node->next = NULL;

    disp_imglist_free(node);
  }

  /////////////////////////////////////////
  // disp packet API cmd queue initialization
  /////////////////////////////////////////
  dispImgListTailPtr_API = NULL;
  disp_imglist_total_API = 0;

  for (i=0;i<DISP_NUM_PACKETS_API;i++)
  {
    node_API = &dispImgList_API[i];

    node_API->prev = NULL;
    node_API->next = NULL;

    disp_imglist_free_API(node_API);
  }
}
