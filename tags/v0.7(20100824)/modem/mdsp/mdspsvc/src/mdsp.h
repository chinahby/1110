#ifndef MDSP_H
#define MDSP_H
/*===========================================================================

           M D S P    S E R V I C E S    H E A D E R    F I L E

DESCRIPTION
  This file contains declarations associated with the MDSP Services module.

REFERENCES
  None

Copyright (c) 2002 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/services/mdspsvc/mdsp.h#1 $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/08   ah      Branched from MSMSHARED_MDSP_SVCS.00.01.25
11/15/06   cs      Reinstate the mdsp_peek_reg and mdsp_poke_reg commands
10/25/06   scm     Add new mdsp_app_type MDSP_APP_W2G. W2G is a small module
                   handling GSM measurements while in WCDMA.
06/19/06   ah      Fixed Lint errors.
02/17/06   gfr     Added mDSP logging support
01/27/06   adm     Added GSM_FTM Application Id.
11/30/04    sg     Defined MDSP_CLIENT_DOWNLOAD_TIMEOUT. Added support for
                   specifying reason for disabling a client.
09/21/03    bt     Added SRCH app.
05/08/03    sg     Removed superflous commas in enum declarations.
02/18/03    sg     Modified to support module based interfaces.
01/24/03    sg     Added mdsp_memcpy() and mdsp_memcpy32() functions.
12/12/02    sg     Added mdsp_reset(). Rename mdsp_sleep()/mdsp_wakeup() as
                   mdsp_dsp_sleep()/mdsp_dsp_wakeup().
11/15/02    sg     Added interfaces to allow applications to command the MDSP
                   to sleep/wakeup after acquiring the MDSP.
08/20/02    sg     Added GPS and BT applications.
                   Added MDSP_IMAGE_DWNLD_FAILED image_status_type to inform
                   the clients that a download attempt failed.
07/29/02    sm     Added CDMA application.
04/02/02    sg     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"              /* Target specific definitions            */
#include "comdef.h"              /* Definitions for byte, word, etc.       */
#include "mdspext.h"             /* MDSP firmware images and access        */

/* <EJECT> */
/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed to interface with the MDSP Services module.

===========================================================================*/

#define MDSP_CLIENT_DOWNLOAD_TIMEOUT  3000

/* Application identifiers */
typedef enum
{
#ifdef FEATURE_GPSONE
#error code not present
#endif

#ifdef FEATURE_HDR
#error code not present
#endif

  MDSP_APP_CDMA,       /* CDMA                                             */
  MDSP_APP_1X = MDSP_APP_CDMA,

  MDSP_APP_SRCH,       /* SRCH */

#ifdef FEATURE_WCDMA
#error code not present
#endif

#ifdef FEATURE_APP_W2G
  MDSP_APP_W2G,        /* GSM measurement support during WCDMA mode        */
#endif

#ifdef FEATURE_GSM
#error code not present
#endif

#ifdef FEATURE_GSM
#error code not present
#endif

#ifdef FEATURE_BT
  MDSP_APP_BT,
#endif

  MDSP_APP_MAX,

  /* For Internal MDSP Services Use Only! It is used to represent MDSPS
   * when setting the clock mask.
   */
  MDSP_APP_SELF,

  /* DO NOT USE: Force this enum to be a 32bit type to improve speed       */
  MDSP_APP_32BIT_DUMMY = 0X10000
} mdsp_app_type;


/* --------------------------------------------------------------------------
                         MDSP Event Definitions
----------------------------------------------------------------------------*/

/* MDSP events - used as a mask so each event must be a single bit */
typedef enum
{
  MDSP_EV_COMMAND = 1, /* Command status */
  MDSP_EV_MODULE  = 2, /* Module status  */
  MDSP_EV_LOG     = 4  /* Log status     */
} mdsp_event_type;

/* Command status values */
typedef enum
{
  MDSP_CMD_ACCEPT = 0, /* Command was accepted */
  MDSP_CMD_FAIL        /* Command failed       */
} mdsp_command_status_type;

/* Command Status Event info */
typedef struct
{
  mdsp_event_type          event;
  mdsp_app_type            app_id;
  mdsp_command_status_type cmd_status;
} mdsp_ev_cmd_type;

/* Module status values */
typedef enum
{
  MDSP_MOD_READY,          /* Module is running and ready */
  MDSP_MOD_DISABLE,        /* Module is not available     */
  MDSP_MOD_ENABLE_FAILED   /* Module could not be enabled */
} mdsp_mod_status_type;

/* Reason for disabling a module */
typedef enum
{
  MDSP_DISABLE_REASON_PREEMPTING,    /* Client is being preempted             */
  MDSP_DISABLE_REASON_FIRMWARE_ERROR /* DSP will be reset due firmware errors */
} mdsp_disable_reason_type;

/* Used to provide additional information about a module event */
typedef union
{
  /* MOD_READY info */
  struct
  {
    mdsp_image_type image; /* Image that is used to enable the module */
  } ready;

  /* MOD_DISABLE info */
  struct
  {
    mdsp_disable_reason_type reason;
  } disable;

  /* MOD_ENABLE_FAILED info */
  struct
  {
    mdsp_image_type image; /* The image download that caused the failure */
  } enable_failed;

} mdsp_ev_mod_info_type;

/* Module Event info */
typedef struct
{
  mdsp_event_type       event;
  mdsp_app_type         app_id;
  mdsp_mod_status_type  mod_status;
  mdsp_ev_mod_info_type info;
} mdsp_ev_mod_type;

#ifdef FEATURE_MDSPSVC_LOG
/* Log status type */
typedef enum
{
  MDSP_LOG_SAVE_COMPLETE,  /* Save of log/memory is complete */
  MDSP_LOG_SAVE_FAILED     /* Save of log/memory failed */
} mdsp_log_status_type;

/* Log Event info */
typedef struct
{
  mdsp_event_type       event;
  mdsp_app_type         app_id;
  mdsp_log_status_type  log_status;
} mdsp_ev_log_type;
#endif

/* MDSP event data type */
typedef union
{
  mdsp_event_type     event;
  mdsp_ev_cmd_type    cmd;
  mdsp_ev_mod_type    mod;
#ifdef FEATURE_MDSPSVC_LOG
  mdsp_ev_log_type    log;
#endif
} mdsp_event_data_type;

/* Event callback function */
typedef void (*mdsp_event_cb_type) (mdsp_event_data_type* event_ptr);


#ifdef FEATURE_MDSPSVC_LOG
/* --------------------------------------------------------------------------
                         MDSP Log Definitions
----------------------------------------------------------------------------*/

/* Log mode types */
typedef enum
{
   MDSP_LOG_DISABLED,    /* No logging */
   MDSP_LOG_ON_REQUEST,  /* Send log packets only when requested */
   MDSP_LOG_CONTINUOUS,  /* Send log packets as they are generated */
   MDSP_LOG_MAX          /* Invalid value for range check */
} mdsp_log_mode_type;
#endif


/* <EJECT> */
/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION MDSP_PEEK

DESCRIPTION
  Reads a block of data from the MDSP/ARM shared memory area.  This function
  will activate the clocks if they are not active so it can be called at any
  time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mdsp_peek (
  /* The start address of the MDSP memory block to read */
  void* src_ptr,
  /* The address of the ARM destination buffer */
  void* dst_ptr,
  /* The length in bytes of the memory block */
  uint32 byte_length
);

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_POKE

DESCRIPTION
  Writes a block of data to the MDSP/ARM shared memory area.  This function
  will activate the MDSP clocks if they are not active so it can be called 
  at any time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mdsp_poke (
  /* The address of the source buffer to write to the MDSP */
  void* src_ptr,
  /* The start address of the MDSP memory block to write */
  void* dst_ptr,
  /* The length in bytes of the memory block */
  uint32 byte_length
);

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_POKE_REG

DESCRIPTION
  Pokes an mDSP register.  Will activate the MDSP clocks if they are not
  active so it can be called at any time.

DEPENDENCIES
  None

RETURN VALUE
  FALSE if the poke failed for some reason.

SIDE EFFECTS
  Issues interrupt to mDSP firmware and waits for a response.  Will wake
  up mDSP if it is asleep.

===========================================================================*/
extern boolean mdsp_poke_reg (
  uint16 reg,  /* The register address to poke */
  uint32 data  /* The data to write */
);

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_PEEK_REG

DESCRIPTION
  Peeks an mDSP register.  Will activate the MDSP clocks if they are not
  active so it can be called at any time.

DEPENDENCIES
  None

RETURN VALUE
  FALSE if the peek failed for some reason.  Data is returned in parameter.

SIDE EFFECTS
  Issues interrupt to mDSP firmware and waits for a response.  Will wake
  up mDSP if it is asleep.

===========================================================================*/
extern boolean mdsp_peek_reg (
  uint16  reg,   /* The register address to peek */
  uint32 *data   /* Where to store the retrieved data */
);

/*===========================================================================

FUNCTION MDSP_MEMCPY

DESCRIPTION
  Copies a block of 16-bit values to or from the MDSP/ARM shared memory area.

DEPENDENCIES
  Applications must call mdsp_enable() before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mdsp_memcpy (
  uint16 *dest,  /* Destination address                         */
  uint16 *src,   /* Source address                              */
  uint32 wlen,   /* Length of data to copy, in words            */
  boolean swap   /* Boolean to swap when copying to destination */
);

/*===========================================================================

FUNCTION MDSP_MEMCPY32

DESCRIPTION
  Copies a block of 32-bit values to or from the MDSP/ARM shared memory area.

DEPENDENCIES
  Applications must call mdsp_enable() before using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mdsp_memcpy32 (
  uint32 *dest,  /* Destination address                         */
  uint32 *src,   /* Source address                              */
  uint32 len,    /* Length of data to copy, in dwords           */
  boolean swap   /* Boolean to swap when copying to destination */
);

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_REGISTER_APP

DESCRIPTION
  Applications use this function to register interrupt handler functions,
  event callback function and event mask. Once registered, applications will
  begin receiving all events in their event mask. The application's
  interrupt handlers will only be called when the application has enabled the
  MDSP by calling mdsp_enable().

  Note on registering ISRs:
    If the client does not wish to register for any DSP interrupts, then
       the isrs_ptr parameter must be set to NULL.
    If interrupts are registered, they must be passed using 
       mdsp_isr_func_type pointers in an array of size MDSP_ISR_MAX.
     
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A MDSP_EV_COMMAND event will issued to the calling application once the
  command has completed.

===========================================================================*/
extern void mdsp_register_app (
  mdsp_app_type      app_id,    /* Application ID                            */
  mdsp_isr_func_type *isrs_ptr, /* Pointer to a table of ISRs for the App    */
  mdsp_event_cb_type cb_ptr,    /* Application's event callback function     */
  uint16 event_mask /* Mask of events that the application needs to receive  */
);

#ifdef MDSP_INTERRUPTS_USED
/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_REGISTER_ISR

DESCRIPTION
  Applications use this function to register an interrupt handler function
  for the specified DSP interrupt. The application's interrupt handler will
  only be called when the application has enabled the MDSP by calling
  mdsp_enable(). This function may be used to change the client's registered
  ISRs even after the client is enabled using mdsp_enable().

DEPENDENCIES
  The client must have been previously registered using mdsp_register_app().

RETURN VALUE
  None

SIDE EFFECTS
  A MDSP_EV_COMMAND event will issued to the calling application once the
  command has completed.

===========================================================================*/
extern void mdsp_register_isr (
  mdsp_app_type            app_id,  /* Application ID                         */
  mdsp_isr_func_type       isr_ptr, /* Function to call when interrupt occurs */
  mdsp_isr_type            isr_type /* Interrupt identifier                   */
);
#endif /* MDSP_INTERRUPTS_USED */

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_ENABLE

DESCRIPTION
  Applications use this function to indicate which MDSP module should be
  enabled. When the calling application's priority is high enough,
  MDSP Services will issue a callback to the application, indicating that it
  has been enabled. When the "MOD_READY" event is received, the MDSP clocks are
  on, the current firmware image is running and the requested module is ready
  for use on the MDSP.

  When MDSP-to-ARM interrupts occur, MDSP Services will call the registered
  interrupt handlers (highest priority first) of each application is currently
  enabled.

DEPENDENCIES
  It is important that applications call mdsp_disable() when they finish using
  the MDSP.  If the MDSP is left enabled, excessive power comsumption will
  result.

RETURN VALUE
  None

SIDE EFFECTS
  A MDSP_EV_COMMAND event will issued to the calling application immediately
  after the command is accepted. Note however, that this does not mean that
  the requested module has been enabled. The client application _must_ wait
  to receive the MDSP_MOD_READY event.

===========================================================================*/
extern void mdsp_enable (
  /* Application ID */
  mdsp_app_type app_id,
  /* The module to enable */
  mdsp_module_type module
);

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_DISABLE

DESCRIPTION
  Applications use this function to disable the MDSP.  MDSP Services disables
  the MDSP when no application require it to be enabled.  When the MDSP is
  disabled, its clocks are off and the current firmware image is not running.
  The MDSP draws minimum current when it is disabled.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A MDSP_EV_COMMAND event will issued to the calling application once the
  command has completed.

===========================================================================*/
extern void mdsp_disable (
  /* Application ID */
  mdsp_app_type app_id
);

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_RESET_AND_ENABLE

DESCRIPTION
  Applications use this function to reset the DSP and enable a module.
  This is useful when the client wants to ensure that MDSP services actually
  does a reset and redownload of the image, before it is enabled. In the
  normal scenario, the DSP is not reset between successive calls to
  mdsp_enable() and mdsp_disable(), if the image has not changed.

  IMPORTANT: This interface must used only as a debugging aid, since resetting
  the DSP may cause other MDSP Services clients to react adversely.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A MDSP_EV_COMMAND event will issued to the calling application once the
  command has completed.

===========================================================================*/
extern void mdsp_reset_and_enable (
  /* Application ID */
  mdsp_app_type app_id,
  /* The module to enable */
  mdsp_module_type module
);

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_DISABLE_EVENT_RSP

DESCRIPTION
  Applications should call this function in response to a MDSP_EV_MODULE event
  with MDSP_MOD_DISABLE status. MDSP Services issues this event whenever the
  client application's use of the MDSP will be preempted temporarily. Once the
  client receives this event, it may possibly save any context information and
  stop using the DSP. It should then call this function to indicate to MDSP
  Services that it is ready to be disabled.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mdsp_disable_event_rsp (
  /* Application ID */
  mdsp_app_type app_id
);

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_DSP_SLEEP

DESCRIPTION
  Applications use this function to notify MDSP Services that they want to
  go to sleep. This is used by MDSP Services to _potentially_ turn off
  MDSP clocks.

DEPENDENCIES
  The calling Application must have called mdsp_enable() prior to using this
  function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_dsp_sleep (
  /* Application ID */
  mdsp_app_type app_id
);

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_DSP_WAKEUP

DESCRIPTION
  Applications use this function to notify MDSP Services that the MDSP must
  be woken up (brought out of sleep). MDSP Services uses this as an indication
  that the clocks must be turned on.

DEPENDENCIES
  The calling Application must have called mdsp_enable() prior to using this
  function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_dsp_wakeup (
  /* Application ID */
  mdsp_app_type app_id
);

#ifdef FEATURE_MDSPSVC_LOG
/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_LOG_SET_MODE

DESCRIPTION
  Applications use this function to set the log mode while they are active.

DEPENDENCIES
  The calling Application must have registered prior to using this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_log_set_mode
(
   /* Application id */
   mdsp_app_type       app_id,
   /* Log mode to set */
   mdsp_log_mode_type  log_mode,
   /* Period at which to read chunks from the mDSP */
   uint16              log_period
);

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_LOG_SEND

DESCRIPTION
  Triggers an mDSP log packet to be sent.

DEPENDENCIES
  The calling Application must have registered prior to using this function
  and enabled logging using mdsp_log_set_mode.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_log_send
(
   /* Application id */
   mdsp_app_type       app_id
);

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_LOG_SAVE

DESCRIPTION
  Causes the entire saved mDSP log buffer to be saved to EFS.

DEPENDENCIES
  The calling Application must have registered prior to using this function
  and enabled logging using mdsp_log_set_mode.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_log_save
(
   /* Application id */
   mdsp_app_type       app_id,
   /* Optional tag to store with the file for tracking */
   uint32              tag
);

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_MEMORY_SAVE

DESCRIPTION
  Causes the entire mDSP memory to be saved to EFS.

DEPENDENCIES
  The calling Application must have registered prior to using this function.

RETURN VALUE
  None

SIDE EFFECTS
  mDSP services will not process any further requests until the save is
  complete, which takes ~250 milliseconds.

===========================================================================*/
void mdsp_memory_save
(
   /* Application id */
   mdsp_app_type       app_id,
   /* Optional tag to store with the file for tracking */
   uint32              tag
);
#endif /* FEATURE_MDSPSVC_LOG */

/* <EJECT> */
/*===========================================================================

FUNCTION mdsp_task

DESCRIPTION
  MDSP Services task processing function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void mdsp_task (
  /* Parameter received from Main Control task - ignored */
  dword ignored
);
#endif /* MDSP_H */

