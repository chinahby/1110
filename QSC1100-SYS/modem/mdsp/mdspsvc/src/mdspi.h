#ifndef MDSPI_H
#define MDSPI_H
/*===========================================================================

                   M D S P   S E R V I C E S   M O D U L E
                   I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions internal to the MDSP task.

REFERENCES
  None

Copyright (c) 2002 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //depot/asic/qsc1100/services/mdspsvc/mdspi.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/08   ah      Branched from MSMSHARED_MDSP_SVCS.00.01.25
06/19/06   ah      Fixed Lint errors.
02/17/06   gfr     Added mDSP logging support
02/18/03    sg     Modified to support module based interfaces.
12/12/02    sg     Added support for mdsp_reset() command.
11/20/02    sg     Removed command types for sleep/wakeup, since they now use
                   synchronous interfaces.
11/15/02    sg     Added command types to allow applications to command the MDSP
                   to sleep/wakeup after acquiring the MDSP.
08/20/02    sg     Removed some function declarations which were not
                   accessed outside mdsptask.c. Removed unnecessary
                   externalization of some functions.
04/02/02    sg     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.                */
#include "customer.h"   /* Customer specific definitions                   */
#include "mdsp.h"       /* MDSP Services Interface Definitions             */
#include "queue.h"      /* Definitions for queue services                  */
#include "target.h"     /* Target specific definitions                     */

/* <EJECT> */
/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/* Generate the clock control bit mask for the given application. */
#define MDSP_APP_CLK_MASK(app_id) (uint16)( 1 << (app_id) )


/* --------------------------------------------------------------------------
** MDSP Services task signals
** --------------------------------------------------------------------------
*/

#ifdef FEATURE_MDSPSVC_LOG
#define MDSP_LOG_NV_SIG            0x0040 /* NV read timer                    */
#define MDSP_LOG_TIMER_SIG         0x0020 /* Log timer                        */
#endif
#define MDSP_DSP_CMD_TIMEOUT_SIG   0x0010 /* Timed out on the DSP cmd reponse */
#define MDSP_CHECK_STATE_SIG       0x0008 /* Re-evaluate the module states    */
#define MDSP_IMAGE_RESET_SIG       0x0004 /* Image reset signal               */
#define MDSP_CMD_Q_SIG             0x0002 /* Command Queue Signal             */
#define MDSP_RPT_TIMER_SIG         0x0001 /* Watchdog report timer            */


/* --------------------------------------------------------------------------
** MDSP Services task commands
** --------------------------------------------------------------------------
*/

/* Command types */
typedef enum 
{
  MDSP_CMD_REGISTER_APP,               /* Register application             */
  MDSP_CMD_REGISTER_ISR,               /* Register ISR                     */
  MDSP_CMD_ENABLE,                     /* Enable MDSP module               */
  MDSP_CMD_DISABLE,                    /* Disable MDSP module              */
  MDSP_CMD_DISABLE_EVENT_RSP,          /* Application allowed disabling    */
  MDSP_CMD_RESET_AND_ENABLE,           /* Reset the DSP before enabling    */
#ifdef FEATURE_MDSPSVC_LOG
  MDSP_CMD_LOG_SET_MODE,               /* Set the application log mode     */
  MDSP_CMD_LOG_SEND,                   /* Trigger a log packet to be sent  */
  MDSP_CMD_LOG_SAVE,                   /* Save log buffer to EFS           */
  MDSP_CMD_MEMORY_SAVE,                /* Save memory to EFS               */
#endif
  MDSP_CMD_MAX,
  MDSP_CMD_INVALID = MDSP_CMD_MAX
} mdsp_cmd_type;

/* Command packet header */
typedef struct 
{
  q_link_type          link;                    /* Command queue link      */
  mdsp_cmd_type        cmd;                     /* MDSP command to perform */
} mdsp_hdr_type;

/* Register Application command packet */  
typedef struct 
{
  mdsp_hdr_type        hdr;                     /* Command packet header     */
  mdsp_app_type        app_id;                  /* Application identifier    */
  mdsp_isr_func_type   *isrs_ptr;               /* Table of ISRs for the App */
  mdsp_event_cb_type   cb_ptr;                  /* Event callback pointer    */
  uint16               event_mask;              /* Event mask                */
} mdsp_register_app_packet_type;

#ifdef MDSP_INTERRUPTS_USED
/* Register ISR command packet */  
typedef struct 
{
  mdsp_hdr_type        hdr;                     /* Command packet header   */
  mdsp_app_type        app_id;                  /* Application identifier  */
  mdsp_isr_func_type   isr_ptr;                 /* MDSP-to-ARM ISR         */
  mdsp_isr_type        isr_type;                /* The type of interrupt   */
} mdsp_register_isr_packet_type;
#endif

/* Enable command packet */
typedef struct 
{
  mdsp_hdr_type        hdr;                     /* Command packet header    */
  mdsp_app_type        app_id;                  /* Application identifier   */
  mdsp_module_type     module;                  /* Selected firmware module */
} mdsp_enable_packet_type;

/* Disable command packet */
typedef struct 
{
  mdsp_hdr_type        hdr;                     /* Command packet header   */
  mdsp_app_type        app_id;                  /* Application identifier  */
} mdsp_disable_packet_type;

/* Reset_and_enable command packet */
typedef struct 
{
  mdsp_hdr_type        hdr;                     /* Command packet header   */
  mdsp_app_type        app_id;                  /* Application identifier  */
  mdsp_module_type     module;                  /* Selected firmware module */
} mdsp_reset_and_enable_packet_type;

/* Disable event response command packet */
typedef struct 
{
  mdsp_hdr_type        hdr;                     /* Command packet header   */
  mdsp_app_type        app_id;                  /* Application identifier  */
} mdsp_disable_event_rsp_packet_type;

#ifdef FEATURE_MDSPSVC_LOG
/* Set log mode command packet */
typedef struct 
{
  mdsp_hdr_type        hdr;                     /* Command packet header    */
  mdsp_app_type        app_id;                  /* Application identifier   */
  mdsp_log_mode_type   log_mode;                /* Log mode                 */
  uint16               log_period;              /* Freq to read log from mDSP */
} mdsp_log_set_mode_packet_type;

/* Send log packet command packet */
typedef struct 
{
  mdsp_hdr_type        hdr;                     /* Command packet header    */
  mdsp_app_type        app_id;                  /* Application identifier   */
} mdsp_log_send_packet_type;

/* Save log packet command packet */
typedef struct 
{
  mdsp_hdr_type        hdr;                     /* Command packet header    */
  mdsp_app_type        app_id;                  /* Application identifier   */
  uint32               tag;                     /* Optional tag to save     */
} mdsp_log_save_packet_type;

/* Save memory packet command packet */
typedef struct 
{
  mdsp_hdr_type        hdr;                     /* Command packet header    */
  mdsp_app_type        app_id;                  /* Application identifier   */
  uint32               tag;                     /* Optional tag to save     */
} mdsp_memory_save_packet_type;
#endif

/* Command packet union */
typedef union 
{
  /* Command packet header */
  mdsp_hdr_type                          hdr;

  /* Register app. command */
  mdsp_register_app_packet_type          register_app_cmd;

#ifdef MDSP_INTERRUPTS_USED
  /* Register ISR command  */
  mdsp_register_isr_packet_type          register_isr_cmd;
#endif

  /* Enable App. command   */
  mdsp_enable_packet_type                enable_cmd;

  /* Disable App. command  */
  mdsp_disable_packet_type               disable_cmd;

  /* Reset DSP and enable module */
  mdsp_reset_and_enable_packet_type      reset_and_enable_cmd;

  /* Disable event response command */
  mdsp_disable_event_rsp_packet_type     disable_event_rsp_cmd;

#ifdef FEATURE_MDSPSVC_LOG
  /* Change log mode for an App. */
  mdsp_log_set_mode_packet_type          log_set_mode_cmd;

  /* Send a log packet to diag */
  mdsp_log_send_packet_type              log_send_cmd;

  /* Save the log to EFS */
  mdsp_log_save_packet_type              log_save_cmd;

  /* Save memory to EFS */
  mdsp_memory_save_packet_type           memory_save_cmd;
#endif

} mdsp_packet_type;


/* --------------------------------------------------------------------------
** Miscellaneous declarations
** --------------------------------------------------------------------------
*/
/* Internal states of MDSP Services */
typedef enum
{
  MDSP_STATE_IDLE,
  MDSP_STATE_WAITING_FOR_APP_RESET,
  MDSP_STATE_WAITING_FOR_MODULE_SWITCH,
  MDSP_STATE_MODULE_SWITCH_SUCCESSFUL,
  MDSP_STATE_MAX
} mdsp_services_state_type;

/* Current image loaded in the MDSP */
extern mdsp_image_type mdsp_current_image;
  

/* --------------------------------------------------------------------------
** Application data types
** --------------------------------------------------------------------------
*/

/* States of the MDSP client applications */
typedef enum
{
  MDSP_APP_STATE_DISABLED,
  MDSP_APP_STATE_ENABLE_REQUESTED,
  MDSP_APP_STATE_ENABLING,
  MDSP_APP_STATE_ENABLED,
  MDSP_APP_STATE_PREEMPTING,
  MDSP_APP_STATE_MAX
} mdsp_app_state_type;

/* Application data */
typedef struct
{
  boolean             registered;               /* is application registered */
  mdsp_app_state_type state;                    /* State of the application  */
  mdsp_module_type    module;                   /* Module desired by app.    */

#ifdef MDSP_INTERRUPTS_USED
  mdsp_isr_func_type  isrs[MDSP_ISR_MAX];       /* MDSP-to-ARM ISRs          */
#endif

  mdsp_event_cb_type  cb_ptr;                   /* Event callback function   */
  uint16              event_mask;               /* Event mask                */

#ifdef FEATURE_MDSPSVC_LOG
  mdsp_log_mode_type  log_mode;                 /* Logging mode              */
  uint16              log_period;               /* Logging period in ms      */
#endif
} mdsp_app_table_type;

extern mdsp_app_table_type mdsp_app_table[];


/* <EJECT> */
/*===========================================================================

       P U B L I C   F U N C T I O N S
  
===========================================================================*/

/*===========================================================================

FUNCTION MDSP_CLOCKS_ENABLE

DESCRIPTION
  Enables the clock regimes associated with the MDSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  mdsp_clock_mask
  Interrupts are locked briefly

===========================================================================*/
extern void mdsp_clocks_enable ( 
  /* Application ID */
  mdsp_app_type app_id
);

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_CLOCKS_DISABLE

DESCRIPTION
  Disables the clock regimes associated with the MDSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  mdsp_clock_mask
  Interrupts are locked briefly

===========================================================================*/
extern void mdsp_clocks_disable ( 
  /* Application ID */
  mdsp_app_type app_id
);

/* <EJECT> */
/*===========================================================================

FUNCTION mdsp_cmd

DESCRIPTION
  Queue a command for processing by the parent task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  mdsp_cmd_q

===========================================================================*/
extern void mdsp_cmd (
  /* The command packet */
  mdsp_packet_type* mdsp_cmd_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION mdsp_get_packet

DESCRIPTION
  Get a MDSP command packet.  Used by callers of mdsp_cmd().
  
DEPENDENCIES
  mdsp_task_init() needs to have initialized the queues.

RETURN VALUE
  A pointer to an available mdsp_packet_type or NULL if no packets are
  available.  (Note if NULL is returned, ERR() will have ALREADY been
  called to log the problem.)

SIDE EFFECTS
  mdsp_free_q

===========================================================================*/
extern mdsp_packet_type* mdsp_get_packet( void );


/* <EJECT> */
/*===========================================================================

       P R I V A T E   I N T E R N A L   F U N C T I O N S
  
===========================================================================*/

/*===========================================================================

FUNCTION mdsp_send_cmd_event

DESCRIPTION
  Sends a MDSP_EV_COMMAND event to the specified application.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_send_cmd_event (
  /* Application ID */
  mdsp_app_type            app_id,
  /* Command status */
  mdsp_command_status_type status
);

#ifdef FEATURE_MDSPSVC_LOG
/*===========================================================================

FUNCTION mdsp_log_set_mode_processing

DESCRIPTION
  Sets the logging mode for the given application.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  May start the logging timer to begin reading mDSP log packets if the log
  mode is not disabled.

===========================================================================*/
void mdsp_log_set_mode_processing
(
  /* Command packet */
  mdsp_packet_type *packet
);

/*===========================================================================

FUNCTION mdsp_log_send_processing

DESCRIPTION
  Requests a log packet be sent to diag.  If logging is not enabled then
  this will do nothing.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_log_send_processing
(
  /* Command packet */
  mdsp_packet_type *packet
);

/*===========================================================================

FUNCTION mdsp_log_save_processing

DESCRIPTION
  Requests that the current set of chunks be saved to EFS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_log_save_processing
(
  /* Command packet */
  mdsp_packet_type *packet
);

/*===========================================================================

FUNCTION mdsp_memory_save_processing

DESCRIPTION
  Requests that mDSP memory be saved to EFS.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Takes a long time to execute (150 ms), stalling other mDSP services
  requests.

===========================================================================*/
void mdsp_memory_save_processing
(
  /* Command packet */
  mdsp_packet_type *packet
);

/*===========================================================================

FUNCTION mdsp_recover_from_dsp_failure

DESCRIPTION
  Attempts to reset the MDSP and restore normal operation if the
  Master Controller stops responding.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void mdsp_recover_from_dsp_failure(void);

/*===========================================================================

FUNCTION mdsp_log_process_app_state_change

DESCRIPTION
  Called when an app changes state to ENABLED or DISABLED, goes to sleep,
  or wakes-up to handle activating or deactivating logging if necessary.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_log_process_app_state_change
(
  /* Application id */
  mdsp_app_type app_id
);

/*===========================================================================

FUNCTION mdsp_log_process_timer

DESCRIPTION
  MDSP Services log code timer expiry processing.  Reads out any new
  chunks from mDSP memory.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_log_process_timer (void);

/*===========================================================================

FUNCTION mdsp_log_init

DESCRIPTION
  MDSP Services log code initialization function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_log_init (void);

/*===========================================================================

FUNCTION mdsp_log_dsp_start

DESCRIPTION
  This function is called after the firmware image has been loaded but
  before it starts running.  It sets up any shared variables that need
  to be set before the kernel starts.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_log_dsp_start (void);

/*===========================================================================

FUNCTION mdsp_log_dsp_failure

DESCRIPTION
  Called if an mDSP failure was detected and recovery is about to begin.
  Will cause log and memory to be saved to EFS if the appropriate flag is
  set.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_log_dsp_failure (void);

#endif /* FEATURE_MDSPSVC_LOG */

#endif /* MDSPI_H */

