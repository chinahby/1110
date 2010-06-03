#ifndef TCXOMGR_TASK_H
#define TCXOMGR_TASK_H

/*===========================================================================

                   TCXO Manager Task Header File

DESCRIPTION
  This file contains all the exported definitions for the TCXO Manager task.

Copyright (c) 2007 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/


/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mcs/tcxomgr/main/latest/inc/tcxomgr_task.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/16/07   sst     Added tcxomgr_wait() function
                   Add UPDATE_XO_TRIM_NV_SIG
10/10/07   va      Add voting mechanism for sleep
10/02/07   va      Added TCXO Manager command queue
09/25/07   cab     Move field cal to tcxomgr task
07/06/07   va      Replace FEATURE_XO with FEATURE_XO_TASK
06/18/07   va      Initial release.

===========================================================================*/

#include "comdef.h"    /* Definition for basic types and macros */
#include "target.h"    /* Target configuration file */
#include "customer.h"  /* Customer configuration file */


#ifdef FEATURE_XO_TASK
#include "rex.h"
#include "timer.h"
#include "tcxomgr.h"
#include "queue.h"
#include "sleep.h"

/* TCXO Manager's OK to sleep handle */
extern sleep_okts_handle tcxomgr_sleep_okts_handle;


/******************************************************************************
                 Signals used by TCXO Manager task
******************************************************************************/


/* Generic blocking timer signal for TCXOMGR task. */
#define TCXOMGR_SLOPE_UPDATE_TIMER_SIG           0x00100000
#define TCXOMGR_FIELD_CAL_SIG                    0x00200000
#define TCXOMGR_CMD_SIG                          0x00400000
#define TCXOMGR_UPDATE_XO_TRIM_NV_SIG            0x00800000

#define TCXOMGR_TASK_RPT_TIMER_SIG    0x1000           /* WDOG timer signal */
#define TCXOMGR_TASK_OFFLINE_SIG      TASK_OFFLINE_SIG /* 0x2000 */
#define TCXOMGR_TASK_STOP_SIG         TASK_STOP_SIG    /* 0x4000 */
#define TCXOMGR_TASK_START_SIG        TASK_START_SIG   /* 0x8000 */
#define TCXOMGR_TASK_SIG_MASK         (TCXOMGR_TASK_RPT_TIMER_SIG | \
                                       TCXOMGR_TASK_OFFLINE_SIG   | \
                                       TCXOMGR_TASK_STOP_SIG)

#define TCXOMGR_ALL_SIGS 0xBFFFFFFF

#define TCXOMGR_SIGS   ( TCXOMGR_SLOPE_UPDATE_TIMER_SIG        | \
                         TCXOMGR_FIELD_CAL_SIG                 | \
                         TCXOMGR_CMD_SIG                       | \
                         TCXOMGR_UPDATE_XO_TRIM_NV_SIG         | \
                         (TCXOMGR_TASK_SIG_MASK))


extern timer_type  tcxomgr_slope_timer;  /* Slope Update timer */

/* TCXO Manager command codes */
typedef enum
{
  TCXOMGR_CMD_NOTIFY,
  TCXOMGR_CMD_RPUSH,
  TCXOMGR_CMD_MAX
}
tcxomgr_cmd_code_type;

/* Structure containing parameters specific to the TCXOMGR_NOTIFY_CMD command.
 */
typedef struct
{
  tcxomgr_restriction_type restriction;
}tcxomgr_notify_cmd_type;

/* Structure containing parameters specific to the TCXOMGR_RPUSH_CMD command.
 */
typedef struct
{
  tcxomgr_client_id_type     source_id;
  tcxomgr_vco_type           vco_value;
  tcxomgr_rotator_type       average_rotator_value;
  uint32                     number_of_samples;
  tcxomgr_client_state_type  client_state;
  timetick_type              sclk_time_first_sample;
  timetick_type              sclk_time_last_sample;
  int32                      temperature;
}tcxomgr_rpush_cmd_type;

/* TCXO Manager's generic command struct */
typedef struct
{
  /* Common fields. */
  q_link_type                   link;
  tcxomgr_cmd_code_type         cmd_id;

  /* Union of command structures. */
  union
  {
    tcxomgr_notify_cmd_type     notify;
    tcxomgr_rpush_cmd_type      rpush;
  } cmd;
}
tcxomgr_cmd_type;

/******************************************************************************
                 TCXO Manager Task prototypes.
******************************************************************************/

/*===========================================================================

FUNCTION TCXOMGR_SET_SUBSIGS

DESCRIPTION    This function sets the signals for TCXO Manager task.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void tcxomgr_set_sigs ( rex_sigs_type sigs );


/*===========================================================================

FUNCTION TCXOMGR_CLR_SIGS

DESCRIPTION    This function clears the TCXO Manager task signals.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
extern void tcxomgr_clr_sigs ( rex_sigs_type sigs );


/*===========================================================================

FUNCTION TCXOMGR_WAIT

DESCRIPTION
  This function just performs a REX wait as specified by the caller.

RETURN VALUE
  Returns the value returned by rex_wait

DEPENDENCIES

===========================================================================*/
extern rex_sigs_type tcxomgr_wait ( rex_sigs_type  sigs );

/*===========================================================================

FUNCTION TCXOMGR_TASK

DESCRIPTION
  The TCXO Manager task main function. It performs task
  initialization then continuously handles TCXO Manager signals.

DEPENDENCIES
  REX is running.

RETURN VALUE
  Never exits.

SIDE EFFECTS

===========================================================================*/

extern void tcxomgr_task (dword dummy );

/*===========================================================================

FUNCTION      TCXOMGR_GET_CMD_BUF

DESCRIPTION

  Returns a pointer to a newly allocated command buffer.

DEPENDENCIES

  The buffer must eventually be returned to the free queue using
  tcxomgr_free_cmd_buf().

RETURN VALUE

  The newly allocated buffer. Null if there are no free buffers left.

SIDE EFFECTS

===========================================================================*/
tcxomgr_cmd_type* tcxomgr_get_cmd_buf(void);

/*===========================================================================

FUNCTION      TCXOMGR_SEND_CMD

DESCRIPTION

  Writes a command to the TCXOMGR command queue then
  raises the TCXOMGR_CMD_SIG signal.

DEPENDENCIES
  The pointer passed to the function must have been allocated using
  tcxomgr_get_cmd_buf.

RETURN VALUE
  TRUE if the command is successfully created and queued, otherwise FALSE.

SIDE EFFECTS

===========================================================================*/

extern void tcxomgr_send_cmd (tcxomgr_cmd_type *cmd_ptr );

#endif /* FEATURE_XO_TASK */
#endif /* TCXOMGR_TASK_H */
