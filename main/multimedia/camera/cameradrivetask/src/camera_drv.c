/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      C A M E R A     D R I V E R     T A S K

GENERAL DESCRIPTION
  This module contains camera driver software.

REFERENCES

EXTERNALIZED FUNCTIONS

  camera_drv_task
    This function is the entrance procedure for the camera driver
    Task. It contains the main processing loop for the camera driver task.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright(c) 2005 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/camera/6k/cameradrivetask/main/latest/src/camera_drv.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/21/06   jn      AF lens move is now done in camera_drv_task.
12/21/05   dle     Initial release.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "camera_drv.h"
#ifdef FEATURE_CAMERA_DRV_TASK
#include "rex.h"
#include "task.h"
#include "queue.h"
#include "msg.h"
#include "err.h"
#include "dog.h"
#include "mc.h"
#include "camera_drvi.h"
#include "camera_af.h"

/*===========================================================================

                DECLARATIONS FOR MODULE

===========================================================================*/

typedef enum {
  CAMERA_DRV_CMD_FOCUS,
  CAMERA_DRV_CMD_RESET_LENS,
  CAMERA_DRV_CMD_INVALID
} camera_drv_cmd_type;

/* Header for camera drive commands
*/
typedef struct
{
  q_link_type                     link;   /* Command queue link */
  camera_drv_cmd_type             cmd;    /* camera drive command to perform */
} camera_drv_hdr_type;                    /* Graphic header */

typedef struct {
  camera_drv_hdr_type             hdr;    /* Command Header */
  void                            *callback;
  uint32                          client_data;
  uint32                          info1;
  uint32                          info2;
  uint32                          info3;
  uint32                          info4;
} camera_focus_packet_type;

typedef union {
  camera_drv_hdr_type        hdr;
  camera_focus_packet_type   camera_focus;
} camera_drv_packet_type; 

#define CAMERA_DRV_NUM_PACKETS 20
static q_type                  camera_drv_cmd_q;
static q_type                  camera_drv_free_q;
static camera_drv_packet_type  camera_drv_free_packet [CAMERA_DRV_NUM_PACKETS];
static rex_timer_type          camera_drv_rpt_timer;  /* Watch Dog Timer */


/*===========================================================================

                FORWARD DECLARATIONS

===========================================================================*/

static void camera_drv_task_init (void);
static void camera_drv_task_offline (void);
static void camera_drv_task_shutdown (void);
static void camera_drv_queue_cmd
(
  camera_drv_packet_type *camera_drv_cmd_ptr
    /* pointer to camera driver command */
);
static camera_drv_packet_type *camera_drv_cmd_get_pkt (void);
static void camera_drv_cmd_parser (void);


/*===========================================================================

FUNCTION camera_drv_task

DESCRIPTION
  This procedure is the entrance procedure for the CAMERA_DRV
  Task.  It contains the main processing loop for the CAMERA_DRV Task.

DEPENDENCIES
  FEATURE_CAMERA_DRV_TASK.

RETURN VALUE
  None

SIDE EFFECTS
  camera_drv_tcb.

===========================================================================*/
void camera_drv_task
(
  dword ignored
    /* Parameter received from Main Control task - ignored */
    /*lint -esym(715,ignored)
    ** Have lint not complain about the ignored parameter 'ignored' which is
    ** specified to make this routine match the template for rex_def_task().
    */
)
{
  rex_sigs_type sigs;

  /* Initialize Camera Driver Task
  */
  camera_drv_task_init ();

#ifdef FEATURE_MULTIMODE_ARCH
  /*  Process task start procedure from task controller. */
  task_start (CAMERA_DRV_RPT_TIMER_SIG, DOG_CAMERA_DRV_RPT, &camera_drv_rpt_timer);
#else /* FEATURE_MULTIMODE_ARCH */
  /* Wait for start signal from Main Control task
  */
  mc_task_start (CAMERA_DRV_RPT_TIMER_SIG, DOG_CAMERA_DRV_RPT, &camera_drv_rpt_timer);
#endif /* FEATURE_MULTIMODE_ARCH */

  /* Clear start signal
  */
  (void) rex_clr_sigs (&camera_drv_tcb, TASK_START_SIG);

  /* Set the watchdog report timer signal so that we'll
  ** reset the watchdog as soon as we start checking for events
  */
  (void) rex_set_sigs (&camera_drv_tcb, CAMERA_DRV_RPT_TIMER_SIG);

  /* Infinite loop -- process events forever
  */
  for (;;) {

    /* Wait for something to happen
    */
    sigs = rex_wait( CAMERA_DRV_RPT_TIMER_SIG |
                     CAMERA_DRV_CMD_Q_SIG         |
                     TASK_STOP_SIG            |
                     TASK_OFFLINE_SIG);

    /* Process the queued commands
    */
    if ((sigs & CAMERA_DRV_CMD_Q_SIG) != 0 )
    {
      /* Clear the 'queued commands' signal.
      */
      (void) rex_clr_sigs (&camera_drv_tcb, CAMERA_DRV_CMD_Q_SIG);
      camera_drv_cmd_parser ();
    }

    /* If watchdog report timer expired, report and restart the timer
    */
    if ((sigs & CAMERA_DRV_RPT_TIMER_SIG) != 0)
    {
      dog_report (DOG_CAMERA_DRV_RPT);
      (void) rex_set_timer (&camera_drv_rpt_timer, DOG_CAMERA_DRV_RPT_TIME);
    }

    /*---------------------------------------------------------------------*/


    /*---------------------------------------------------------------------*/
    /* We're supposed to shutdown.
    */
    if ((sigs & TASK_STOP_SIG) != 0)
    {
      camera_drv_task_shutdown ();
    }

    /*---------------------------------------------------------------------*/

    /* We're supposed to go offline.
    */
    if ((sigs & TASK_OFFLINE_SIG) != 0)
    {
      camera_drv_task_offline ();
    }

    /*---------------------------------------------------------------------*/

  } /* end of infinite loop */
} /* end camera_drv_task */

/*===========================================================================

FUNCTION camera_drv_task_init

DESCRIPTION
  This procedure initializes the camera driver task.

DEPENDENCIES
  FEATURE_CAMERA_DRV_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  camera_drv_tcb.

===========================================================================*/
static void camera_drv_task_init (void)
{
  int i;

  /* Initialize timers
  */
  rex_def_timer (&camera_drv_rpt_timer, &camera_drv_tcb, CAMERA_DRV_RPT_TIMER_SIG);

  /* Initialize the command queues.
  */
  (void) q_init (&camera_drv_cmd_q);
  (void) q_init (&camera_drv_free_q);

  /* Add items to camera_drv_free_q.
  */
  for (i = 0; i < CAMERA_DRV_NUM_PACKETS; i++)
  {
    q_link_type *link_item;
    link_item = q_link( &camera_drv_free_packet[i],
                        &camera_drv_free_packet[i].hdr.link );
    q_put( &camera_drv_free_q, link_item );
  }

  MSG_LOW("Camera Driver task initialized",0,0,0);

} /* end camera_drv_task_init */

/*===========================================================================

FUNCTION camera_drv_task_offline

DESCRIPTION
  This procedure performs the transition to "offline" for the camera driver task.

DEPENDENCIES
  FEATURE_CAMERA_DRV_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  camera_drv_tcb.

===========================================================================*/
static void camera_drv_task_offline (void)
{

#ifdef FEATURE_MULTIMODE_ARCH
  /* Process task offline procedure from task controller.
  */
  task_offline();
#else /* FEATURE_MULTIMODE_ARCH */
  /* Acknowledge the offline signal
  */
  (void) rex_set_sigs (&mc_tcb, MC_ACK_SIG);
#endif /* FEATURE_MULTIMODE_ARCH */

  /* Clear the offline signal, and return
  */
  (void) rex_clr_sigs (&camera_drv_tcb, TASK_OFFLINE_SIG);

} /* end camera_drv_task_offline */

/*===========================================================================

FUNCTION camera_drv_task_shutdown

DESCRIPTION
  This procedure performs shutdown processing for the camera driver task.

DEPENDENCIES
  FEATURE_CAMERA_DRV_TASK.

RETURN VALUE
  None.

SIDE EFFECTS
  camera_drv_tcb.

===========================================================================*/
static void camera_drv_task_shutdown (void)
{

#ifdef FEATURE_MULTIMODE_ARCH
  /* Process task stop procedure from task controller.
  */
  task_stop();
#else /* FEATURE_MULTIMODE_ARCH */
  /* Acknowledge the stop signal
  */
  (void) rex_set_sigs (&mc_tcb, MC_ACK_SIG);
#endif /* FEATURE_MULTIMODE_ARCH */

  /* Clear the stop signal, and return
  */
  (void) rex_clr_sigs (&camera_drv_tcb, TASK_STOP_SIG);

} /* end camera_drv_task_shutdown */

/*===========================================================================

FUNCTION camera_drv_queue_cmd

DESCRIPTION
  Queue a command for processing by the camera drive task.

DEPENDENCIES
  FEATURE_CAMERA_DRV_TASK.
  camera_drv_task_init() needs to have initialized the queues.

RETURN VALUE
  None.

SIDE EFFECTS
  camera_drv_cmd_q.
  camera_drv_tcb.

===========================================================================*/
static void camera_drv_queue_cmd
(
  camera_drv_packet_type *camera_drv_cmd_ptr
    /* pointer to camera driver command */
)
{
  /* Init link field     */
  (void) q_link (camera_drv_cmd_ptr, &camera_drv_cmd_ptr->hdr.link);
  /* Put on command queue*/
  q_put (&camera_drv_cmd_q, &camera_drv_cmd_ptr->hdr.link);
  /* Signal a queue event*/
  (void) rex_set_sigs(&camera_drv_tcb, CAMERA_DRV_CMD_Q_SIG);
} /* end camera_drv_cmd */

/*===========================================================================

FUNCTION camera_drv_cmd_get_pkt

DESCRIPTION
  Get a camera driver Command Packet.  Use by callers of camera_drv_cmd().

DEPENDENCIES
  FEATURE_CAMERA_DRV_TASK.
  camera_drv_task_init() needs to have initialized the queues.

RETURN VALUE
  A pointer to an available camera_drv_packet_type or NULL if no packets are
  available.  (Note if NULL is returned, ERR() will have ALREADY been
  called to log the problem.)

SIDE EFFECTS
  camera_drv_free_q (and the packet fetched off camera_drv_free_q).

===========================================================================*/
static camera_drv_packet_type *camera_drv_cmd_get_pkt (void)
{
  camera_drv_packet_type *packet;

  /* Get a packet and then either report an error, or clear the packet out
  ** so the previous user's garbage doesn't do something unexpected.
  */
  packet = (camera_drv_packet_type *) (q_get (&camera_drv_free_q));

  if (packet == NULL)
  {
    ERR ("Ran Out of camera driver Packets!", 0, 0, 0);
  }
  else
  {
    /* Fill the entire camera driver packet with 0's to get default behavior on
    ** un-initialized fields.
    */
    (void) memset (packet, 0, sizeof (camera_drv_packet_type));
    packet->hdr.cmd = CAMERA_DRV_CMD_INVALID;     /* Invalid Command Value   */
  }

  return (packet);

} /* end camera_drv_cmd_get_pkt */

/*===========================================================================

FUNCTION camera_drv_queue_focus

DESCRIPTION
  This function queues camera focus request

DEPENDENCIES
  FEATURE_CAMERA_DRV_TASK

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void camera_drv_queue_focus
(
  void *callback,
  uint32 client_data,
  uint32 info1,
  uint32 info2,
  uint32 info3,
  uint32 info4
)
{
  camera_drv_packet_type   *packet;

  if ((packet = camera_drv_cmd_get_pkt()) != NULL) {
    packet->camera_focus.hdr.cmd = CAMERA_DRV_CMD_FOCUS;
    packet->camera_focus.callback = callback;
    packet->camera_focus.client_data = client_data;
    packet->camera_focus.info1 = info1;
    packet->camera_focus.info2 = info2;
    packet->camera_focus.info3 = info3;
    packet->camera_focus.info4 = info4;
    camera_drv_queue_cmd(packet);
  }
} /* camera_drv_queue_camqdsp_msg */

/*===========================================================================

FUNCTION camera_drv_queue_reset_lens

DESCRIPTION
  This function queues camera reset lens request

DEPENDENCIES
  FEATURE_CAMERA_DRV_TASK

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void camera_drv_queue_reset_lens
(
  void *callback,
  uint32 client_data,
  uint32 info1,
  uint32 info2,
  uint32 info3,
  uint32 info4
)
{
  camera_drv_packet_type   *packet;

  if ((packet = camera_drv_cmd_get_pkt()) != NULL) {
    packet->camera_focus.hdr.cmd = CAMERA_DRV_CMD_RESET_LENS;
    packet->camera_focus.callback = callback;
    packet->camera_focus.client_data = client_data;
    packet->camera_focus.info1 = info1;
    packet->camera_focus.info2 = info2;
    packet->camera_focus.info3 = info3;
    packet->camera_focus.info4 = info4;
    camera_drv_queue_cmd(packet);
  }
} /* camera_drv_queue_reset_lens */

/*===========================================================================

FUNCTION camera_drv_cmd_parser

DESCRIPTION
  This procedure processes requests received by camera_drv_cmd(). See camera_drvi.h for
  a description of the actual CMX packets.

DEPENDENCIES
  FEATURE_CAMERA_DRV_TASK.
  camera_drv_task_init must have been called to init the command queue.

RETURN VALUE
  None.

SIDE EFFECTS
  camera_drv_cmd_q.

===========================================================================*/
static void camera_drv_cmd_parser (void)
{
  camera_drv_packet_type *cmd_ptr;

  /* If there are commands to process, process the first command.
  */
  cmd_ptr = (camera_drv_packet_type *) q_get (&camera_drv_cmd_q);
  if (cmd_ptr != NULL)
  {
#ifndef CAMERA_USES_SOFTDSP
    switch (cmd_ptr->hdr.cmd)
    {
      case CAMERA_DRV_CMD_FOCUS:
        camera_af_do_safe_move(
          (camsensor_move_focus_enum_type)(cmd_ptr->camera_focus.info1),
          (int32)cmd_ptr->camera_focus.info2);
        break;

      case CAMERA_DRV_CMD_RESET_LENS:
        camera_af_do_reset_lens();
        break;

      /* Illegal command, does not return.
      */
      default:
        ERR_FATAL ("Illegal vs command %x", cmd_ptr->hdr.cmd, 0, 0);
    }
#endif
    /* Return camera driver packet to free q
    */
    q_put (&camera_drv_free_q, &cmd_ptr->hdr.link);

  } /* end of while "commands to process" */

  if ( q_check( &camera_drv_cmd_q ) != NULL)
  {
    (void) rex_set_sigs (&camera_drv_tcb, CAMERA_DRV_CMD_Q_SIG);
  }
} /* end camera_drv_cmd_parser */

#endif /* FEATURE_CAMERA_DRV_TASK */
