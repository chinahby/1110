#ifndef QDSPI_H
#define QDSPI_H
/*===========================================================================

                   Q D S P   S E R V I C E S   M O D U L E
                   I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This header file contains all the definitions internal to the QDSP task.

REFERENCES
  None

Copyright (c) 2000 - 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
   
$Header: //source/qcom/qct/multimedia/core/qdspservices/6k/ver1/qdspsvcs/main/latest/src/qdspi.h#4 $ $DateTime: 2008/11/19 04:18:21 $ $Author: shilpaj $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/16/08   knm     Added new API to collect valid ADSP dump prior to AMSS Reset
01/08/07    sg     Added support to keep track of pending and allocated
                   modules requests for an app separately.
11/19/04    sg     Merged module-based QDSP services from 00.00.13.01. Added
                   support for concurrent images.
12/03/02    st     Added num_apps parameter to qdsp_send_image_event()
                   prototype; added qdsp_perform_image_ready_event() prototype.
04/27/02    sg     Modified to allow the specification of zero interrupts in the
                   qdspext configuration file.
02/21/02    sm     Added qdsp_reset() function.
01/25/02    sm     Removed BASE_ADDRESS, VALID_START, VALID_END defines.
12/05/00    ro     memcpy prototype removed - in qdspmem.h.
10/04/00    sm     Added qdsp_memcpy().
08/28/00    ro     Unit integration.
08/17/00   spf     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.                */
#include "customer.h"   /* Customer specific definitions                   */
#include "qdsp.h"       /* QDSP Services Interface Definitions             */
#include "queue.h"      /* Definitions for queue services                  */
#include "target.h"     /* Target specific definitions                     */

/* <EJECT> */
/*===========================================================================

                         DATA DECLARATIONS

===========================================================================*/

/* --------------------------------------------------------------------------
** QDSP Services task signals
** --------------------------------------------------------------------------
*/

#define QDSP_DSP_CMD_TIMEOUT_SIG   0x0010 /* Downloader CMD response timer    */
#define QDSP_CHECK_STATE_SIG       0x0008 /* Re-evaluate the module states    */
#define QDSP_IMAGE_RESET_SIG       0x0004 /* Image reset signal               */
#define QDSP_CMD_Q_SIG             0x0002 /* Command Queue Signal             */
#define QDSP_RPT_TIMER_SIG         0x0001 /* Watchdog report timer            */


/* --------------------------------------------------------------------------
** QDSP Services task commands
** --------------------------------------------------------------------------
*/

/* Command types */
typedef enum 
{
  QDSP_CMD_REGISTER_APP,               /* Register application             */
  QDSP_CMD_REGISTER_ISR,               /* Register ISR                     */
  QDSP_CMD_ENABLE,                     /* Enable QDSP module               */
  QDSP_CMD_DISABLE,                    /* Disable QDSP module              */
  QDSP_CMD_DISABLE_EVENT_RSP,          /* Application allowed disabling    */
  QDSP_CMD_IMAGE_CHG_EVENT_RSP,        /* Application acked image change   */
  QDSP_CMD_RESET_AND_ENABLE,           /* Reset the DSP before enabling    */
  QDSP_CMD_MAX,
  QDSP_CMD_INVALID = QDSP_CMD_MAX
} qdsp_cmd_type;

/* Command packet header */
typedef struct 
{
  q_link_type          link;                    /* Command queue link      */
  qdsp_cmd_type        cmd;                     /* QDSP command to perform */
} qdsp_hdr_type;

/* Register Application command packet */  
typedef struct 
{
  qdsp_hdr_type        hdr;                     /* Command packet header     */
  qdsp_app_type        app_id;                  /* Application identifier    */
  qdsp_isr_func_type   isrs[QDSP_ISR_MAX];      /* Table of ISRs for the App */
  qdsp_event_cb_type   cb_ptr;                  /* Event callback pointer    */
  uint16               event_mask;              /* Event mask                */
} qdsp_register_app_packet_type;

/* Register ISR command packet */  
typedef struct 
{
  qdsp_hdr_type        hdr;                     /* Command packet header   */
  qdsp_app_type        app_id;                  /* Application identifier  */
  qdsp_isr_func_type   isr_ptr;                 /* QDSP-to-ARM ISR         */
  qdsp_isr_type        isr_type;                /* The type of interrupt   */
} qdsp_register_isr_packet_type;

/* Enable command packet */
typedef struct 
{
  qdsp_hdr_type        hdr;                     /* Command packet header    */
  qdsp_app_type        app_id;                  /* Application identifier   */
  qdsp_module_type     module;                  /* Selected firmware module */
} qdsp_enable_packet_type;

/* Disable command packet */
typedef struct 
{
  qdsp_hdr_type        hdr;                     /* Command packet header   */
  qdsp_app_type        app_id;                  /* Application identifier  */
} qdsp_disable_packet_type;

/* Reset_and_enable command packet */
typedef struct 
{
  qdsp_hdr_type        hdr;                     /* Command packet header   */
  qdsp_app_type        app_id;                  /* Application identifier  */
  qdsp_module_type     module;                  /* Selected firmware module */
} qdsp_reset_and_enable_packet_type;

/* Disable event response command packet */
typedef struct 
{
  qdsp_hdr_type        hdr;                     /* Command packet header   */
  qdsp_app_type        app_id;                  /* Application identifier  */
} qdsp_disable_event_rsp_packet_type;

/* Image change event response command packet */
typedef struct 
{
  qdsp_hdr_type        hdr;                     /* Command packet header   */
  qdsp_app_type        app_id;                  /* Application identifier  */
} qdsp_image_chg_event_rsp_packet_type;

/* Command packet union */
typedef union 
{
  /* Command packet header */
  qdsp_hdr_type                          hdr;

  /* Register app. command */
  qdsp_register_app_packet_type          register_app_cmd;

  /* Register ISR command  */
  qdsp_register_isr_packet_type          register_isr_cmd;

  /* Enable App. command   */
  qdsp_enable_packet_type                enable_cmd;

  /* Disable App. command  */
  qdsp_disable_packet_type               disable_cmd;

  /* Reset DSP and enable module */
  qdsp_reset_and_enable_packet_type      reset_and_enable_cmd;

  /* Disable event response command */
  qdsp_disable_event_rsp_packet_type     disable_event_rsp_cmd;

  /* Image change event response command */
  qdsp_image_chg_event_rsp_packet_type   image_chg_event_rsp_cmd;

} qdsp_packet_type;


/* --------------------------------------------------------------------------
** Miscellaneous declarations
** --------------------------------------------------------------------------
*/
/* Internal states of QDSP Services */
typedef enum
{
  QDSP_SERVICES_STATE_IDLE,
  QDSP_SERVICES_STATE_WAITING_FOR_APP_RESET,
  QDSP_SERVICES_STATE_WAITING_FOR_MODULE_SWITCH,
  QDSP_SERVICES_STATE_MODULE_SWITCH_SUCCESSFUL,
  QDSP_SERVICES_STATE_MAX
} qdsp_services_state_type;

/* Current image loaded in the QDSP */
extern qdsp_image_type qdsp_current_image;

/* --------------------------------------------------------------------------
** Application data types
** --------------------------------------------------------------------------
*/

/* States of the QDSP client applications */
typedef enum
{
  QDSP_APP_STATE_DISABLED,
  QDSP_APP_STATE_ENABLE_REQUESTED,
  QDSP_APP_STATE_ENABLING,
  QDSP_APP_STATE_ENABLED,
  QDSP_APP_STATE_PREEMPTING,
  QDSP_APP_STATE_MAX
} qdsp_app_state_type;

/* Application data */
typedef struct
{
  boolean             registered;         /* is application registered       */
  qdsp_app_state_type state;              /* State of the application        */
  qdsp_module_type    module_requested;   /* New module required by app.     */
  qdsp_module_type    module_enabled;     /* Current module allocated to app.*/
  qdsp_isr_func_type  isrs[QDSP_ISR_MAX]; /* QDSP-to-ARM ISRs                */
  qdsp_event_cb_type  cb_ptr;             /* Event callback function         */
  uint16              event_mask;         /* Event mask                      */
  boolean             nested_enable;
  boolean             reset;              /* Reset the DSP before enabling   */
} qdsp_app_table_type;

extern qdsp_app_table_type qdsp_app_table[];

typedef struct
{
  uint16              count;
  qdsp_image_type     image;
  qdsp_module_type    *mode;
}
qdsp_mod_search_type;

#ifdef FEATURE_QDSP_ERR_LOGS_STORE
#error code not present
#endif /* FEATURE_QDSP_ERR_LOGS_STORE */
/* <EJECT> */
/*===========================================================================

       P U B L I C   F U N C T I O N S
  
===========================================================================*/

/*===========================================================================

FUNCTION QDSP_CLOCKS_ENABLE

DESCRIPTION
  Enables the clock regimes associated with the QDSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  qdsp_clock_mask
  Interrupts are locked briefly

===========================================================================*/
extern void qdsp_clocks_enable ( 
  /* Application ID */
  qdsp_app_type app_id
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CLOCKS_DISABLE

DESCRIPTION
  Disables the clock regimes associated with the QDSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  qdsp_clock_mask
  Interrupts are locked briefly

===========================================================================*/
extern void qdsp_clocks_disable ( 
  /* Application ID */
  qdsp_app_type app_id
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd

DESCRIPTION
  Queue a command for processing by the parent task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  qdsp_cmd_q

===========================================================================*/
extern void qdsp_cmd (
  /* The command packet */
  qdsp_packet_type* qdsp_cmd_ptr
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_get_packet

DESCRIPTION
  Get a QDSP command packet.  Used by callers of qdsp_cmd().
  
DEPENDENCIES
  qdsp_task_init() needs to have initialized the queues.

RETURN VALUE
  A pointer to an available qdsp_packet_type or NULL if no packets are
  available.  (Note if NULL is returned, ERR() will have ALREADY been
  called to log the problem.)

SIDE EFFECTS
  qdsp_free_q

===========================================================================*/
extern qdsp_packet_type* qdsp_get_packet( void );


/* <EJECT> */
/*===========================================================================

       P R I V A T E   I N T E R N A L   F U N C T I O N S
  
===========================================================================*/

/*===========================================================================

FUNCTION qdsp_send_cmd_event

DESCRIPTION
  Sends a QDSP_EV_COMMAND event to the specified application.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void qdsp_send_cmd_event (
  /* Application ID */
  qdsp_app_type            app_id,
  /* Command status */
  qdsp_command_status_type status
);

#endif /* QDSPI_H */

