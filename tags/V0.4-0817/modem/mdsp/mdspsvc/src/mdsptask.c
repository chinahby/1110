/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   M D S P   S E R V I C E S   M O D U L E
                         T A S K   F U N C T I O N S
                     
GENERAL DESCRIPTION
  This task controls the Modem DSP (MDSP) embedded Digital Signal Processor.

EXTERNALIZED FUNCTIONS
  mdsp_task
    This is the entrance procedure for the MDSP Services Task.
    It contains the main processing loop for the task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright(c) 2002 - 2008 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/services/mdspsvc/mdsptask.c#2 $ 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/10/08   ah      Based on harshak request pulled in 
                   //depot/asic/qsc6055/services/mdspsvc/mdsptask.c#4, needed
                   for clock related issues.
02/19/08   ah      Branched from MSMSHARED_MDSP_SVCS.00.01.25
02/01/07   rs      Moved setting and completing the MDSP clock frequency
                   function calls outside of the SUSPEND/RESUME window to 
                   minimize the SUSPEND time.
01/17/07   ah      Cleaned FEATURE_MULTIMODE_ARCH definitions, as it's present 
                   on all targets
12/05/06   cc      Fixed function mdsp_config_dsp_clock_for_modules early return
                   without proper TASKFREE(). 
11/24/06   cs      Reinstate the FEATURE_DCVS_LPM to compile as clkrgm not matching
11/15/06   cs      Reinstate the mdsp_peek_reg and mdsp_poke_reg commands
09/07/06   dcf     Removed FEATURE_DCVS_LPM from 
                   mdsp_set_specific_mdsp_clk_frequency() function.
06/19/06   ah      Fixed Lint errors.
05/06/06   adm     Added mdsp_set_specific_mdsp_frequency function
02/17/06   gfr     Added mDSP logging support
08/11/05    sg     Added support for MIPS utilization based clock rate control.
08/03/06    gfr    Support for mdsp_peek_reg and mdsp_poke_reg.  Use exported
                   defines for MC commands.
08/03/05    sg     Fixed bug in mdsp_clocks_enable where the bitmask was only
                   being set for the first client that enabled the clock.
07/29/05    sg     Modified to forcibly set MDSP_APP_SELF in mdsp_clock_mask
                   when we go into error recovery mode, to prevent unnecessary
                   RESUME commands from being sent to the DSP after a redownload
                   which was done as a consequence of an error recovery
                   operation. This is ok since the clock had to be ON for MDSP
                   Services to detect the error situation in the first place.
03/29/05    sg     Fixed bug where mdsp_current_bitmask was not being cleared
                   before a new image is downloaded.
11/30/04    sg     Issue Suspend/Resume commands before disabling/enabling MDSP
                   clock. Cleaned up error recovery code. Added preemption
                   reason to DISABLE callback.
10/05/04    sg     The MDSP clock may be quickly disabled and then enabled if
                   mdsp_disable() is called while all other clients have voted
                   to disable the clock. This may cause DSP failures (CR50689).
                   Modified to minimize this rapid switching.
09/22/04    sg     Merged dynamic download support from mdspsvc.00.01.09.01.00.
05/31/04    sg     Modified to discard interrupts if the mdsp_clock_mask is 0,
                   since this implies that all clients voted to sleep or are
                   currently disabled.
04/08/04    sg     Modified to reduce the amount time interrupts are locked
                   out when an image is downloaded (for non-master controller
                   based images).
10/28/03    sg     Fixed bug where modules were not being enabled if they were
                   not in the preferred mode.
10/20/03    sg     Stripped out QUASAR stuff. Changed feature-ized to
                   accommodate new ARM clock control scheme.
09/29/03   dna     Get T_QUASAR to compile and work again for MSM6500
09/24/03    sg     Increased timeout for DSP to become active to 500
                   microseconds (for MSM6300 GSM+BT image).
09/02/03    sg     INTLOCK the code used to issue mode switch commands since
                   the MC doesn't have a dedicated interrupt in M6500. Lint
                   changes.
08/22/03    sg     Use access function to get bitmask values for modules.
05/08/03    sg     Moved ISR definition from mdspext.c to this file. MOD_READY
                   is now always be sent before any interrupts are passed to
                   the client (only in images with no Master Controller).
04/14/03    sg     Modified so that interrupts will not be lost after the image
                   is started (in the case of images with no MC).
03/25/03    sg     Bumps up the ARM clock rate to reduce the time taken to
                   switch the MDSP clock rate. Now manually checks the status of
                   the MC to handle the case where the 2nd MDSP interrupt is
                   missed while we are handling the first interrupt.
                   Added an error recovery mechanism that will attempt to reset
                   the MDSP if the MC stops responding.
03/13/03    sg     Updated to support two interrupts for each command to the
                   master controller interface - now waits for a command ack
                   interrupt and checks semaphores. Clock switching polls the
                   MDSP interrupt for quicker response. When switching modules,
                   first disables modules that require disabling before
                   enabling any new modules.
02/25/03    sg     Fixed race condition that would cause the CMD timer to be
                   set after the DSP command was processed.
02/18/03    sg     Modified to support module based interfaces.
12/12/02    sg     Added support for mdsp_reset() command.
11/20/02    sg     Removed processing functions sleep/wakeup, since they now use
                   synchronous interfaces.
11/15/02    sg     Added interfaces to allow applications to command the MDSP
                   to sleep/wakeup after acquiring the MDSP.
09/06/02    st     Added support for multimode architecture.
08/21/02    sg     Increased the time out value for application to respond to a
                   reset request to 2000ms. If after the timeout period, some
                   application has not responded to the reset request, a
                   ERR_FATAL message is issued. If image download fails,
                   MDSP_IMAGE_DWNLD_FAILED event is sent to clients.
07/29/02    sm     Removed old debug code.
04/02/02    sg     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"             /* Target specific definitions            */
#include "assert.h"             /* Definitions for ASSERT                 */
#include "clkregim.h"           /* For clk_regime_enable/disable.         */
#include "comdef.h"             /* Definitions for byte, word, etc.       */
#include "dog.h"                /* Watchdog signal and time definitions   */
#include "err.h"                /* Error logging macro                    */
#include "mc.h"                 /* Need for MC signals and prototypes     */
#include "msg.h"                /* Message logging/reporting services     */
#include "mdspext.h"            /* MDSP firmware images and access        */
#include "mdspi.h"              /* Internal MDSP Services definitions     */
#include <string.h>             /* Interface to string library            */
#include "task.h"               /* Task definitions and prototypes        */
#include "tramp.h"              /* Interrupt Trampoline Definitions       */

/* <EJECT> */

/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed to interface with the MDSP Services module.

===========================================================================*/
#ifdef FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING
  #include "clkregim.h"
  #ifdef FEATURE_MODEM_CLOCK_HALT
    extern void clk_regime_modem_test_and_halt(void);
    extern void clk_regime_modem_test_and_unhalt(void);
  #else
    #define clk_regime_modem_test_and_halt()
    #define clk_regime_modem_test_and_unhalt()
  #endif
#else
  #define clk_regime_modem_test_and_halt()
  #define clk_regime_modem_test_and_unhalt()
#endif

#ifndef FEATURE_MDSPSVC_LOG
/* Generate the clock control bit mask for the given application. */
#define MDSP_APP_CLK_MASK(app_id) (uint16)( 1 << (app_id) )
#endif

/* DSP commands */
#define MDSP_CMD_MODULE_CFG    1
#define MDSP_CMD_DSP_SUSPEND   4
#define MDSP_CMD_DSP_RESUME    5

/*-------------------------------------------------------------------------
** Timers
**-----------------------------------------------------------------------*/
LOCAL rex_timer_type   mdsp_rpt_timer;         /* Watchdog timer         */

/* This is the maximum time that MDSP Services will wait for all enabled
** applications to allow the image to be reset. The unit is milliseconds.
*/
#define MDSP_IMAGE_RESET_TIME  2000
LOCAL rex_timer_type   mdsp_image_reset_timer; /* Image reset timer      */

/* This is the maximum time that MDSP Services will wait, to allow the DSP
** to respond to a command. The unit is milliseconds.
*/
#define MDSP_DSP_CMD_TIMEOUT_VAL 1000

/* Check the MC command status every 25ms so that even if we miss the 2nd
 * MDSP interrupt, we can still recognize the status switch */
#define MDSP_MC_STATUS_CHK_VAL  25
LOCAL rex_timer_type   mdsp_dsp_cmd_rsp_timer; /* DSP Cmd. Rsp. Timer    */

LOCAL uint16 mdsp_mc_status_chk_time = 0;

/*-------------------------------------------------------------------------
** State variables
**-----------------------------------------------------------------------*/

/* Current image loaded in the MDSP */
mdsp_image_type mdsp_current_image = MDSP_IMAGE_MAX;

/* Image that was loaded when the MDSP Reset request was made. If this variable
 * is set to anything other than MAX, there is a pending reset request.
*/
LOCAL mdsp_image_type mdsp_reset_image = MDSP_IMAGE_MAX;

/* Application registration table */
mdsp_app_table_type mdsp_app_table[MDSP_APP_MAX];

/* Mask to track application clock usage */
uint16 mdsp_clock_mask = 0;

/* Current state of MDSP Services. */
mdsp_services_state_type mdsp_services_current_state = MDSP_STATE_IDLE;

/* TRUE = there is a pending client request. */
LOCAL boolean mdsp_pending_app_request = FALSE;

/* MDSP mode */
LOCAL mdsp_mode_data_type* mdsp_mode_required = NULL;

/* If set to TRUE then the module is selected for enabling */
LOCAL boolean  mdsp_mod_selection[MDSP_MODULE_MAX];

/* Copy of the bitmask that was used to configure the MDSP modules. */
LOCAL mdsp_module_bitmask_type  mdsp_current_bitmask = 0;

#ifdef MDSP_SET_MODULE_HW_RESOURCES
LOCAL  mdsp_clock_rate_type mdsp_previous_clock_rate = MDSP_CLOCK_RATE_MAX;
#endif

boolean mdsp_two_step_module_switch = FALSE;

/* Saved Interrupt mask */
LOCAL uint32 mdsp_saved_intlock_mask = 0;

/* Definitions for MDSP failure recovery */
#ifdef FEATURE_MDSP_ATTEMPT_RECOVERY

#ifndef MDSP_MASTER_CONTROLLER_USED
#error Feature not available if Master Controller interface is not present
#endif

#define MDSP_ATTEMPT_RECOVERY_LIMIT 100

/* No. of recovery attempts made */
LOCAL byte mdsp_attempt_recovery_cnt = 0;
#endif /* FEATURE_MDSP_ATTEMPT_RECOVERY */

/* Max time the MC will be polled for a command response */
#define MDSP_MC_POLL_TIME_MAX   1000 /* Microseconds */

/* No. of microseconds between poll attempts */
#define MDSP_MC_POLL_INCREMENT  25   /* Microseconds */

/* The max. value the counter should reach which doing a busy
   loop waiting for a for a command response */
#define MDSP_POLL_COUNTER_MAX  10000 

/*-------------------------------------------------------------------------
** MDSP Services Task Command Queue, Free Queue and Available Packets.
**-----------------------------------------------------------------------*/
#define MDSP_NUM_PACKETS 15              /* # of free packets allocated  */
q_type           mdsp_cmd_q;             /* Request packets queue        */
q_type           mdsp_free_q;            /* Available packets queue      */
mdsp_packet_type mdsp_free_packet[MDSP_NUM_PACKETS];

/* <EJECT> */
/*===========================================================================

                             FUNCTION DEFINITIONS

===========================================================================*/

/* <EJECT> */
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
)
{
  mdsp_ev_cmd_type cmd_event;

  if (app_id < MDSP_APP_MAX) 
  {
    if ((mdsp_app_table[app_id].cb_ptr != NULL) &&
        ((mdsp_app_table[app_id].event_mask & (uint16)MDSP_EV_COMMAND) != 0))
    {
      cmd_event.event      = MDSP_EV_COMMAND;
      cmd_event.app_id     = app_id;
      cmd_event.cmd_status = status;

      (mdsp_app_table[app_id].cb_ptr)((mdsp_event_data_type*)&cmd_event);
    }
  }
  else
  {
      MSG_HIGH("MDSP command event not sent for app = %d", app_id,0,0);
  }
  
} /* mdsp_send_cmd_event */

/* <EJECT> */
/*===========================================================================

FUNCTION mdsp_send_module_event

DESCRIPTION
  Sends a MDSP_EV_MODULE event to the specified application.

DEPENDENCIES
  mdsp_app_table

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_send_module_event (
  /* Application ID */
  mdsp_app_type app_id,
  /* Module status */
  mdsp_mod_status_type status,
  /* Other event specific info */
  void *data
)
{
  mdsp_ev_mod_type mod_event;

  /* Populate any related data before sending the Event. */
  if ((mdsp_app_table[app_id].cb_ptr != NULL) &&
      ((mdsp_app_table[app_id].event_mask & (uint16)MDSP_EV_MODULE) != 0))
  {
    mod_event.event      = MDSP_EV_MODULE;
    mod_event.app_id     = app_id;
    mod_event.mod_status = status;

    switch (status)
    {
      case MDSP_MOD_READY:
        mod_event.info.ready.image = *(mdsp_image_type*)data;
        break;

      case MDSP_MOD_DISABLE:
        mod_event.info.disable.reason = *(mdsp_disable_reason_type*)data;
        break;

      case MDSP_MOD_ENABLE_FAILED:
        mod_event.info.enable_failed.image = *(mdsp_image_type*)data;
        break;

      default:
        ERR_FATAL("Illegal status type", 0, 0, 0);
    }
    (mdsp_app_table[app_id].cb_ptr)((mdsp_event_data_type*)&mod_event);
  }

} /* mdsp_send_module_event */

/*===========================================================================

FUNCTION mdsp_disable_app

DESCRIPTION
  Informs the specified app. that it will be disabled.

DEPENDENCIES
  mdsp_app_table

RETURN VALUE
  None.

SIDE EFFECTS
  Updates the state of the application.
  
===========================================================================*/
void mdsp_disable_app(
  mdsp_app_type            app,
  mdsp_disable_reason_type reason
)
{
  if (mdsp_app_table[app].state == MDSP_APP_STATE_ENABLED)
  {
    mdsp_app_table[app].state = MDSP_APP_STATE_PREEMPTING;
    MSG_ERROR("Disabling app=%d, reason=%d", app, (int)reason, 0);
    mdsp_send_module_event(app, MDSP_MOD_DISABLE, &reason);
    #ifdef FEATURE_MDSPSVC_LOG
    mdsp_log_process_app_state_change(app);
    #endif
  }
}

#ifdef MDSP_MASTER_CONTROLLER_USED
/* <EJECT> */
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
void mdsp_recover_from_dsp_failure(void)
{
#ifndef FEATURE_MDSP_ATTEMPT_RECOVERY
  ERR_FATAL("MDSP Master Controller not responding!", 0, 0, 0);
#else

  mdsp_app_type app;
  mdsp_app_table_type *app_ptr;
  boolean apps_disabled = FALSE;

  MSG_FATAL("MDSP Master Controller not responding!", 0, 0, 0);

  if (mdsp_attempt_recovery_cnt >= MDSP_ATTEMPT_RECOVERY_LIMIT) {
    ERR_FATAL("MDSP failure recovery limit reached", 0, 0, 0);
  }

  #ifdef FEATURE_MDSPSVC_LOG
   /* Log the failure - do this before sending notification to any enabled
      apps to ensure that the save commands are in the queue before any
      disable response since we want to save before reloading! */
  mdsp_log_dsp_failure();
  #endif

  mdsp_attempt_recovery_cnt++;
  for (app = (mdsp_app_type)0; app < MDSP_APP_MAX; app++)
  {
    app_ptr = &mdsp_app_table[app];

    switch (app_ptr->state)
    {
      case MDSP_APP_STATE_ENABLING:
        mdsp_app_table[app].state = MDSP_APP_STATE_ENABLE_REQUESTED;
        break;

      case MDSP_APP_STATE_ENABLED:
        mdsp_disable_app(app, MDSP_DISABLE_REASON_FIRMWARE_ERROR);
        apps_disabled = TRUE;
        break;

      case MDSP_APP_STATE_PREEMPTING:
        ERR_FATAL("Unexpected App state", 0,0,0);
        break;

      default:
        ERR_FATAL("Unknown Application state!", 0, 0, 0);
        /* Fall Through */

      case MDSP_APP_STATE_DISABLED:
      case MDSP_APP_STATE_ENABLE_REQUESTED:
        break;
    }
  }
  /* Reset the MDSP and force re-download of image */
  mdsp_reset_image = mdsp_current_image;
  mdsp_pending_app_request = TRUE;

  /* Wait to receive all the ACKs to the disable requests. */
  mdsp_services_current_state = MDSP_STATE_WAITING_FOR_APP_RESET;

  /* Force at least one bit to be enabled to ensure that the mask is non-zero.
   * This will prevent an unnecessary RESUME command from being sent after the
   * image is downloaded. This is ok since the clock must have be on already
   * when the error was detected. If the recovery goes thru correctly, we'll
   * eventually clear the APP_SELF bit in mdsp_state_module_switch_complete().
   */
  mdsp_clock_mask |= MDSP_APP_CLK_MASK( (uint16)MDSP_APP_SELF );

  if (apps_disabled == FALSE)
  {
    (void) rex_set_sigs( &mdsp_tcb, MDSP_CHECK_STATE_SIG );
  }
#endif /* FEATURE_MDSP_ATTEMPT_RECOVERY */
}
#endif /* MDSP_MASTER_CONTROLLER_USED */

/*===========================================================================

FUNCTION mdsp_download_dsp_image

DESCRIPTION
  Downloads the specified firmware image, if not already downloaded.

DEPENDENCIES
  mdsp_current_image
  mdsp_previous_image
  mdsp_reset_image

RETURN VALUE
  None

SIDE EFFECTS
  mdsp_current_image
  mdsp_reset_image

===========================================================================*/
void mdsp_download_dsp_image(mdsp_image_type pending_image)
{
  /*  Download the new image, if necessary
  */
  if ( mdsp_current_image != pending_image ||
       (mdsp_reset_image != MDSP_IMAGE_MAX &&
        mdsp_reset_image == mdsp_current_image) )
  {
    /* Clear any pending reset request. */
    mdsp_reset_image = MDSP_IMAGE_MAX;

    mdsp_current_bitmask = 0;

    /* Download the firmware image to the MDSP */
    if (mdsp_download_image(pending_image) == MDSP_DOWNLOAD_SUCCESS)
    {
      /* Update the current image.
      */
      mdsp_current_image = pending_image;

      #ifdef FEATURE_MDSPSVC_LOG
      /* Let the log module know we are about to start the image */
      mdsp_log_dsp_start();
      #endif

      /* Lock interrupts to guarantee that MOD_READY events are sent out before
         any MDSP interrupts are issued to the clients */
      #ifdef MDSP_MASTER_CONTROLLER_USED
      if (mdsp_modules_info[mdsp_current_image].bitmasks == NULL)
      #endif
      {
        INTLOCK_SAV(mdsp_saved_intlock_mask);
      }

      /* Start the MDSP firmware.
      */
      mdsp_start_image();
      
      #ifdef MDSP_MASTER_CONTROLLER_USED
      /* Allow the MDSP Controller to initialize. */
      if (mdsp_modules_info[mdsp_current_image].bitmasks != NULL)
      {
        clk_busy_wait(500);
        if (mdsp_read(MDSP_dspActive) == 0)
        {
          MSG_FATAL("MDSP did not initialize!", 0, 0, 0);
          mdsp_recover_from_dsp_failure();
        }
      }
      #endif
      MSG_HIGH("MDSP image loaded = %d", mdsp_current_image,0,0);
    }
    else
    {
      ERR_FATAL("Download Failed: Image = %d", mdsp_current_image, 0, 0);
    }
  }
  /* else the required image was already downloaded. */

  return;
} /* mdsp_download_dsp_image */

#ifdef MDSP_MASTER_CONTROLLER_USED
/*===========================================================================

FUNCTION mdsp_master_controller_isr

DESCRIPTION
  Interrupt service routine which handles all the interrupts sent by the MDSP
  Master Controller.

DEPENDENCIES
  mdsp_current_image
  mdsp_reset_image

RETURN VALUE
  None

SIDE EFFECTS
  mdsp_services_current_state

===========================================================================*/
void mdsp_master_controller_isr(void)
{
  if (mdsp_services_current_state == MDSP_STATE_WAITING_FOR_MODULE_SWITCH)
  {
    /* Check if the requested modules could be enabled. */
    if (mdsp_read(MDSP_microMCCmd) == 0)
    {
      if (mdsp_read(MDSP_mdspAppEnable) == mdsp_current_bitmask)
      {
        (void) rex_clr_timer( &mdsp_dsp_cmd_rsp_timer );
        (void) rex_clr_sigs( &mdsp_tcb, MDSP_DSP_CMD_TIMEOUT_SIG );

        mdsp_services_current_state = MDSP_STATE_MODULE_SWITCH_SUCCESSFUL;
#ifdef FEATURE_MDSP_ATTEMPT_RECOVERY
        mdsp_attempt_recovery_cnt = 0;
#endif
        (void) rex_set_sigs( &mdsp_tcb, MDSP_CHECK_STATE_SIG );
      }
    }
  }
}
#endif /* MDSP_MASTER_CONTROLLER_USED */

/*===========================================================================

FUNCTION mdsp_services_isr

DESCRIPTION
  Interrupt service routine which handles all the interrupts sent by the MDSP.
  It routes the interrupts to all the clients who have registered for the
  received interrupt (if they are currently enabled).

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void mdsp_services_isr(
  /* MDSP Hardware interrupt line */
  mdsp_isr_type interrupt_line
)
{
  mdsp_app_type i;
  mdsp_app_table_type *app_ptr;
#ifdef MDSP_MASTER_CONTROLLER_USED
    uint32  error_state;
#endif

  if ((mdsp_current_image < MDSP_IMAGE_MAX) && mdsp_clock_mask)
  {
    clk_regime_modem_test_and_unhalt();
#ifdef MDSP_MASTER_CONTROLLER_USED
    if ((mdsp_modules_info[mdsp_current_image].bitmasks != NULL) &&
        ((error_state = mdsp_read(MDSP_dspHalt)) != 0))
    {
      MSG_FATAL("MDSP halt (0x%x) detected!", error_state, 0, 0);
      mdsp_recover_from_dsp_failure();
      clk_regime_modem_test_and_halt();
      return;
    }

    if (mdsp_controller_ints[mdsp_current_image] == interrupt_line)
    {
      mdsp_master_controller_isr();
    }
#endif

    /* Call ISR for all enabled applications; highest priority first */
    for (i = (mdsp_app_type)0; i < MDSP_APP_MAX; i++)
    {
      app_ptr = &mdsp_app_table[i];
      if (app_ptr->isrs[interrupt_line] != NULL)
      {
        switch (app_ptr->state)
        {
          case MDSP_APP_STATE_ENABLING:
            if (mdsp_services_current_state !=
                  MDSP_STATE_MODULE_SWITCH_SUCCESSFUL)
            {
              break;
            }
            /* Fall Through */

          case MDSP_APP_STATE_ENABLED:
          case MDSP_APP_STATE_PREEMPTING:
            (app_ptr->isrs[interrupt_line])();
            break;

          default:
            break;
        }
      }
    }
    clk_regime_modem_test_and_halt();
  }
}

/*===========================================================================

FUNCTION mdsp_config_dme_mode_for_modules

DESCRIPTION
  Enables DME hardware if at least one module requires DME. Disables otherwise.

DEPENDENCIES
  mdsp_current_image

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void mdsp_config_dme_mode_for_modules(
  /* Modules which will be enabled */
  boolean   *mod_selection
)
{
#ifdef MDSP_SET_MODULE_HW_RESOURCES
  mdsp_module_type     module;
  clkrgm_dme_mode_type *dme_mode_ptr = (clkrgm_dme_mode_type*)
                  mdsp_modules_info[mdsp_current_image].dme_modes;

  static clkrgm_dme_mode_type previous_dme_mode = CLKRGM_DME_NOT_REQUIRED;
  clkrgm_dme_mode_type new_dme_mode = CLKRGM_DME_NOT_REQUIRED;

  if (dme_mode_ptr != NULL)
  {
    /* Determine if any module requires DME */
    for (module=0; module<MDSP_MODULE_MAX; module++)
    {
      if (mod_selection[module] == TRUE)
      {
        if (dme_mode_ptr[module] == CLKRGM_DME_REQUIRED)
        {
          new_dme_mode = CLKRGM_DME_REQUIRED;
          break;
        }
      }
    }
    if (previous_dme_mode != new_dme_mode)
    {
      INTLOCK();
      clk_regime_set_mdsp_dme_mode(new_dme_mode);
      previous_dme_mode = new_dme_mode;
      INTFREE();
    }
  }
#endif /* MDSP_SET_MODULE_HW_RESOURCES */
}

/*===========================================================================

FUNCTION mdsp_config_dsp_clock_for_modules

DESCRIPTION
  Determines the highest clock rate required to support all the modules that
  will be enabled, and switches the clock rate if required.

DEPENDENCIES
  mdsp_current_image

RETURN VALUE
  None.

SIDE EFFECTS
  mdsp_services_current_state

===========================================================================*/
void mdsp_config_dsp_clock_for_modules(
  /* Modules which will be enabled */
  boolean   *mod_selection
)
{
#if defined(MDSP_SET_MODULE_HW_RESOURCES) && \
    defined(MDSP_MASTER_CONTROLLER_USED)
#ifndef MDSP_SERVICES_MIPS_BASED_CLOCK_CTRL
  uint32               saved_int_mask = 0;
  uint32               curr_poll_ms = 0;
  mdsp_clock_rate_type new_clock_rate = (mdsp_clock_rate_type)0;
  mdsp_module_type     module;
  mdsp_clock_rate_type *clock_rates_ptr = (mdsp_clock_rate_type*)
                            mdsp_modules_info[mdsp_current_image].clock_rates;

  /* Determine the highest clock rate required */
  if (clock_rates_ptr != NULL)
  {
    for (module=0; module<MDSP_MODULE_MAX; module++)
    {
      if (mod_selection[module] == TRUE)
      {
        if (clock_rates_ptr[module] > new_clock_rate)
        {
          new_clock_rate = clock_rates_ptr[module];
        }
      }
    }

    /* Switch clock rate if necessary */
    if (mdsp_previous_clock_rate != new_clock_rate)
    {
      INTLOCK_SAV(saved_int_mask);
      if (mdsp_read(MDSP_microMCCmd) == 0)
      {
        mdsp_previous_clock_rate = new_clock_rate;

        /* Prepare to switch the clock rate */
        mdsp_prepare_for_clock_rate_switch(new_clock_rate);

        /* Bump up the ARM clock so that we can switch the DSP clock quickly */
        clk_regime_register_for_cpu_resource(CLKRGM_MDSP_SERVICES);

        /* Initiate DSP WAIT cycles */
        mdsp_write(MDSP_microMCCmd, MDSP_CMD_DSP_SUSPEND);
        mdsp_gen_interrupt(mdsp_modules_info[mdsp_current_image].mc_irq);

        /* The interrupt may belong to some other DSP task. Check the status
        ** to confirm that the DSP has acknowledged the suspend command */
        while (curr_poll_ms < MDSP_MC_POLL_TIME_MAX)
        {
          if (mdsp_read(MDSP_microMCCmd) == 0)
          {
            break;
          }
          clk_busy_wait(MDSP_MC_POLL_INCREMENT);
          curr_poll_ms += MDSP_MC_POLL_INCREMENT;
        }
        if (curr_poll_ms >= MDSP_MC_POLL_TIME_MAX)
        {
          INTFREE_SAV(saved_int_mask);
          MSG_FATAL("MC didn't respond to suspend cmd", 0, 0, 0);
          mdsp_recover_from_dsp_failure();
          return;
        }

        /* Switch clock rate */
        clk_regime_switch_mdsp_clk();
        clk_regime_complete_mdsp_clk_switch();

        /* Inform DSP */
        mdsp_write(MDSP_microMCCmd, MDSP_CMD_DSP_RESUME);
        mdsp_gen_interrupt(mdsp_modules_info[mdsp_current_image].mc_irq);

        /* Allow ARM clock rate to be reduced */
        clk_regime_deregister_for_cpu_resource(CLKRGM_MDSP_SERVICES);

        /* Wait for the DSP to acknowledge the resume command */
        while (curr_poll_ms < MDSP_MC_POLL_TIME_MAX)
        {
          if (mdsp_read(MDSP_microMCCmd) == 0)
          {
            break;
          }
          clk_busy_wait(MDSP_MC_POLL_INCREMENT);
          curr_poll_ms += MDSP_MC_POLL_INCREMENT;
        }

        INTFREE_SAV(saved_int_mask);
        if (curr_poll_ms >= MDSP_MC_POLL_TIME_MAX)
        {
          MSG_FATAL("MC didn't respond to suspend cmd", 0, 0, 0);
          mdsp_recover_from_dsp_failure();
        }
      }
      else
      {
        INTFREE_SAV(saved_int_mask);
        MSG_FATAL("MDSP clock rate switch failed!", 0, 0, 0);
        mdsp_recover_from_dsp_failure();
      }
    }
  }
#else /* MDSP_SERVICES_MIPS_BASED_CLOCK_CTRL */

  uint32               saved_int_mask = 0;
  uint32               curr_poll_ms = 0;
  mdsp_clock_rate_type clock_rate = (mdsp_clock_rate_type)0;
  mdsp_clock_rate_type new_clock_rate = MDSP_CLOCK_RATE_MAX - 1;
  mdsp_module_type     module;
  uint32               total_mips = 0;
  uint16 *mips_reqd_ptr = (uint16*)
                            mdsp_modules_info[mdsp_current_image].mips_reqd;

  /* Determine the highest clock rate required */
  if (mips_reqd_ptr != NULL)
  {
    /* Sum up the total MIPs required for the module that are to be enabled */
    for (module=0; module<MDSP_MODULE_MAX; module++)
    {
      if (mod_selection[module] == TRUE)
      {
        total_mips += mips_reqd_ptr[module];
      }
    }

    /* Pick the clock rate that can satisfy the MIPS requirement.
     * NOTE: Assumes that mdsp_clock_rate_to_mips is in ascending order.
     */
    for (clock_rate = (mdsp_clock_rate_type)0;
         clock_rate < MDSP_CLOCK_RATE_MAX; clock_rate++)
    {
      if (mdsp_get_mips_for_clock_rate(clock_rate) >= total_mips) {
        new_clock_rate = clock_rate;
        break;
      }
    }

    if (mdsp_previous_clock_rate != new_clock_rate)
    {
      MSG_HIGH("Switching clock from rate %d to %d",
               (int)mdsp_previous_clock_rate, (int)new_clock_rate, 0);

      /* Prevent any context switching to other interrupts/tasks since we have 
       * to switch the clock in as little time as possible.
       */
      INTLOCK();
      TASKLOCK();
      mdsp_previous_clock_rate = new_clock_rate;

      /* Bump up the ARM clock so that we can switch the DSP clock quickly */
      clk_regime_register_for_cpu_resource(CLKRGM_MDSP_SERVICES);

      /* Initiate DSP WAIT cycles */
      mdsp_write(MDSP_microMCCmd, MDSP_CMD_DSP_SUSPEND);
      mdsp_gen_interrupt(mdsp_modules_info[mdsp_current_image].mc_irq);

      /* The interrupt may belong to some other DSP task. Check the status
      ** to confirm that the DSP has acknowledged the suspend command */
      while (curr_poll_ms < MDSP_MC_POLL_TIME_MAX)
      {
        if (mdsp_read(MDSP_microMCCmd) == 0)
        {
          break;
        }
        clk_busy_wait(MDSP_MC_POLL_INCREMENT);
        curr_poll_ms += MDSP_MC_POLL_INCREMENT;
      }
      if (curr_poll_ms >= MDSP_MC_POLL_TIME_MAX)
      {
        INTFREE_SAV(saved_int_mask);
        TASKFREE_SAV();
        MSG_FATAL("MC didn't respond to suspend cmd", 0, 0, 0);
        mdsp_recover_from_dsp_failure();
        return;
      }

      mdsp_prepare_for_clock_rate_switch(new_clock_rate);
      clk_regime_switch_mdsp_clk();
      clk_regime_complete_mdsp_clk_switch();

      mdsp_write(MDSP_microMCCmd, MDSP_CMD_DSP_RESUME);
      mdsp_gen_interrupt(mdsp_modules_info[mdsp_current_image].mc_irq);

      /* Allow ARM clock rate to be reduced */
      clk_regime_deregister_for_cpu_resource(CLKRGM_MDSP_SERVICES);

      /* Wait for the DSP to acknowledge the resume command */
      while (curr_poll_ms < MDSP_MC_POLL_TIME_MAX)
      {
        if (mdsp_read(MDSP_microMCCmd) == 0)
        {
          break;
        }
        clk_busy_wait(MDSP_MC_POLL_INCREMENT);
        curr_poll_ms += MDSP_MC_POLL_INCREMENT;
      }

      INTFREE_SAV(saved_int_mask);
      if (curr_poll_ms >= MDSP_MC_POLL_TIME_MAX)
      {
        MSG_FATAL("MC didn't respond to suspend cmd", 0, 0, 0);
        mdsp_recover_from_dsp_failure();
      }
      
      TASKFREE();
      INTFREE();
    }
  }
#endif /* !MDSP_SERVICES_MIPS_BASED_CLOCK_CTRL */

#endif /* MDSP_SET_MODULE_HW_RESOURCES */
}

#ifdef MDSP_USES_OVERLAYED_IMAGES
/*===========================================================================

FUNCTION mdsp_download_new_modules

DESCRIPTION
  Downloads dynamically managed modules to the DSP.

DEPENDENCIES
  mdsp_app_table

RETURN VALUE
  TRUE if modules are ready for immediate use.
  FALSE if the caller must wait for the configuration operation to complete
    on the DSP.

SIDE EFFECTS
  mdsp_services_current_state
  mdsp_current_bitmask

===========================================================================*/
void mdsp_download_new_modules(void)
{
  mdsp_app_type app_id;
  for (app_id = (mdsp_app_type)0; app_id < MDSP_APP_MAX; app_id++)
  {
    if (mdsp_app_table[app_id].state == MDSP_APP_STATE_ENABLING)
    {
      mdsp_download_dsp_module(mdsp_current_image,
                               mdsp_app_table[app_id].module);
    }
  }
}
#endif /* MDSP_USES_OVERLAYED_IMAGES */

/*===========================================================================

FUNCTION mdsp_start_modules_on_dsp

DESCRIPTION
  Enables/disables the specified modules on the MDSP. It also configures the
  MDSP into the appropriate memory mode.

DEPENDENCIES
  mdsp_services_current_state
  mdsp_current_bitmask

RETURN VALUE
  TRUE if modules are ready for immediate use.
  FALSE if the caller must wait for the configuration operation to complete
    on the DSP.

SIDE EFFECTS
  mdsp_services_current_state
  mdsp_current_bitmask

===========================================================================*/
boolean mdsp_start_modules_on_dsp(
  /* Modules which should be enabled */
  boolean   *mod_selection
)
{
  boolean                   modules_ready = FALSE;
#ifdef MDSP_MASTER_CONTROLLER_USED
  mdsp_app_type             app_id;
  mdsp_module_bitmask_type  *bitmasks_ptr;
  mdsp_module_bitmask_type  bitmask = 0;
  mdsp_module_bitmask_type  tmp_bitmask;
  mdsp_module_type          module;
#endif

  if (mdsp_current_image < MDSP_IMAGE_MAX)
  {
#ifdef MDSP_MASTER_CONTROLLER_USED
    bitmasks_ptr = (mdsp_module_bitmask_type*)
                    mdsp_modules_info[mdsp_current_image].bitmasks;

    /* Issue the DSP command to enable/disable modules if the image supports
     * the MDSP Master Controller interface. */
    if (bitmasks_ptr != NULL)
    {
      /* Form the bitmask that will be used to enable/disable the modules. */
      for (module=(mdsp_module_type)0; module<MDSP_MODULE_MAX; module++)
      {
        if (mod_selection[module] == TRUE)
        {
          bitmask |= mdsp_get_module_bitmask(mdsp_current_image, module);
        }
      }

      /* Set the new bitmask only if the image or the bitmask has changed. */
      if (mdsp_current_bitmask != bitmask)
      {
        /* Turn on the clocks before we start issuing any commands */
        mdsp_clocks_enable(MDSP_APP_SELF);

        /* Turn off the disabled modules first */
        tmp_bitmask = ~(mdsp_current_bitmask ^ bitmask);
        tmp_bitmask &= mdsp_current_bitmask;

        if (tmp_bitmask != mdsp_current_bitmask)
        {
          for (app_id=(mdsp_app_type)0; app_id<MDSP_APP_MAX; app_id++)
          {
            if (mdsp_app_table[app_id].state == MDSP_APP_STATE_ENABLING)
            {
              mdsp_app_table[app_id].state = MDSP_APP_STATE_ENABLE_REQUESTED;
            }
          }
          bitmask = tmp_bitmask;
          mdsp_two_step_module_switch = TRUE;
        }
        else
        {
          mdsp_two_step_module_switch = FALSE;

          mdsp_config_dme_mode_for_modules(mod_selection);

          /* Switch clock rates if necessary */
          mdsp_config_dsp_clock_for_modules(mod_selection);

          /* Set the appropriate DSP memory mode. */
          mdsp_memshare_ctrl_func(mdsp_mode_required->memory_config);

#ifdef MDSP_USES_OVERLAYED_IMAGES
          mdsp_download_new_modules();
#endif /* MDSP_USES_OVERLAYED_IMAGES */
        }

        INTLOCK();
        /* Check if the DSP is ready to receive the module config command. */
        if (mdsp_read(MDSP_microMCCmd) == 0)
        {
          mdsp_services_current_state = MDSP_STATE_WAITING_FOR_MODULE_SWITCH;
          mdsp_current_bitmask = bitmask;

          /* Set up a timer for this command's response. We'll manually check
           * the response every MDSP_MC_STATUS_CHK_VAL to handle the case
           * where the command response interrupt and the command completion
           * interrupt happen very close to each other */
          (void) rex_set_timer(&mdsp_dsp_cmd_rsp_timer, MDSP_MC_STATUS_CHK_VAL);

          mdsp_mc_status_chk_time = 0;

          /* Issue the command */
          mdsp_write(MDSP_mdspAppEnableCfg, bitmask);
          mdsp_write(MDSP_microMCCmd, MDSP_CMD_MODULE_CFG);
          mdsp_gen_interrupt(mdsp_modules_info[mdsp_current_image].mc_irq);
        }
        else
        {
          MSG_FATAL("MDSP MC is unexpectedly busy!", 0, 0, 0);
          mdsp_recover_from_dsp_failure();
        }
        INTFREE();
      }
      else
      {
        modules_ready = TRUE;
        mdsp_services_current_state = MDSP_STATE_MODULE_SWITCH_SUCCESSFUL;
      }
    }
    else
#endif /* MDSP_MASTER_CONTROLLER_USED */
    {
      mdsp_config_dme_mode_for_modules(mod_selection);

      modules_ready = TRUE;
      mdsp_services_current_state = MDSP_STATE_MODULE_SWITCH_SUCCESSFUL;
    }
  }
  else
  {
    ERR_FATAL("Invalid image type", 0, 0, 0);
  }
  return modules_ready;
}

/*===========================================================================

FUNCTION mdsp_image_supports_module

DESCRIPTION
  Checks if the specified module can be supported by the given image.

DEPENDENCIES
  mdsp_modules_info

RETURN VALUE
  TRUE if the module can be supported, false otherwise.

SIDE EFFECTS
  None
  
===========================================================================*/
boolean mdsp_image_supports_module(
  /* Image to check against */
  mdsp_image_type  image,
  /* Module required */
  mdsp_module_type module
)
{
  mdsp_module_type *modules_supported;
  byte             mod_index = 0;

  if (image < MDSP_IMAGE_MAX)
  {
    modules_supported = (mdsp_module_type*)mdsp_modules_info[image].modules;

    while (modules_supported[mod_index] != MDSP_MODULE_MAX)
    {
      if (modules_supported[mod_index] == module)
      {
        return TRUE;
      }
      mod_index++;
    }
  }
  return FALSE;
}

/*===========================================================================

FUNCTION mdsp_get_mode

DESCRIPTION
  Get the DSP mode required to support the specified module. This is based on
  the condition that all the previously selected modules in mod_selection can
  still be supported after the addition of the new module.

DEPENDENCIES
  mdsp_modules_info
  This function assumes that the modules in all the data structures in
  mdsp_modules_info always appear in ascending order (by value).

RETURN VALUE
  TRUE if the module can be supported, false otherwise.

SIDE EFFECTS
  Updates current_mode.
  
===========================================================================*/
boolean mdsp_get_mode(
  /* Image to check against */
  mdsp_image_type     image,
  /* Module required */
  mdsp_module_type    module_reqd,
  /* Currently selected mode (preferred mode) */
  mdsp_mode_data_type **preferred_mode,
  /* Modules that are currently enabled */
  boolean             *mod_selection
)
{
  byte                 module_index = 0;
  byte                 mod_enab_index;
  byte                 mode_index;
  mdsp_mode_data_type  *new_mode;
  byte                 num_modes;

  if ( module_reqd < MDSP_MODULE_MAX && image < MDSP_IMAGE_MAX )
  {
    if ( *preferred_mode != NULL )
    {
      /* Give preference to the mode specified by the caller of this function.*/
      while ( (*preferred_mode)->mode[module_index] != MDSP_MODULE_MAX )
      {
        if ( (*preferred_mode)->mode[module_index] == module_reqd )
        {
          return TRUE;
        }
        module_index++;
      }
    }

    /* The current mode can't support the requested module. Check if there
     * is another mode that can support it, _and_ all the other modes that have
     * already been selected for enabling. */

    num_modes = mdsp_modules_info[image].mode_info.num_modes;
    
    for (mode_index=0; mode_index<num_modes; mode_index++)
    {
      new_mode = (mdsp_mode_data_type*)
                 &mdsp_modules_info[image].mode_info.modes[mode_index];

      if (*preferred_mode != new_mode)
      {
        mod_enab_index = 0;
        module_index = 0;
        while ( mod_enab_index < (byte)MDSP_MODULE_MAX &&
                new_mode->mode[module_index] != MDSP_MODULE_MAX )
        {
          if (module_reqd != (mdsp_module_type)mod_enab_index &&
              mod_selection[mod_enab_index] == FALSE)
          {
            mod_enab_index++;
          }
          else
          {
            if ((mdsp_module_type)mod_enab_index < new_mode->mode[module_index])
            {
              /* Failed. Try the next mode (if any). */
              break;
            }
            else if ((mdsp_module_type)mod_enab_index == new_mode->mode[module_index])
            {
              /* Match found for module. */
              if (module_reqd == (mdsp_module_type)mod_enab_index)
              {
                /* Found a mode that fits all the higher priority apps,
                 * including the newly requested module. */
                *preferred_mode = new_mode;
                return TRUE;
              }

              mod_enab_index++;
              module_index++;
            }
            else
            {
              /* Skip - we don't want this module. */
              module_index++;
            }
          }
        }
      }
    }
  }
  return FALSE;
}

/*===========================================================================

FUNCTION mdsp_select_image

DESCRIPTION
  Determines which image is required to support the specified module. Also
  determines the mode required to support the module.

DEPENDENCIES
  None

RETURN VALUE
  Image which can support the module.

SIDE EFFECTS
  Updates current_mode.
  
===========================================================================*/
mdsp_image_type mdsp_select_image(
  mdsp_image_type     current_image,  /* Image that is currently selected */
  mdsp_module_type    module_reqd,    /* Module required                  */
  mdsp_mode_data_type **current_mode, /* Current mode                     */
  boolean             *mod_selection  /* Mods. that are currently enabled */
)
{
  mdsp_image_type new_image = MDSP_IMAGE_MAX;

  /* Check if this is the first time we are trying to select an image */
  if (current_image == MDSP_IMAGE_MAX)
  {
    current_image = (mdsp_image_type) 0;
  }

  if ( module_reqd < MDSP_MODULE_MAX )
  {
    /* Give preference to the current image if it can support
     * the requested mode. */
    if ( mdsp_image_supports_module(current_image, module_reqd) == TRUE &&
         mdsp_get_mode(current_image, module_reqd,
                       current_mode, mod_selection) == TRUE )
    {
      new_image = current_image;
    }
    else
    {
      /* The current image cannot support the required mode.
       * Invalidate the current mode, and try the other images. */
      *current_mode = NULL;
      mdsp_current_bitmask = 0;
#ifdef MDSP_SET_MODULE_HW_RESOURCES
      mdsp_previous_clock_rate = MDSP_CLOCK_RATE_MAX;
#endif

      for (new_image=(mdsp_image_type)0; new_image<MDSP_IMAGE_MAX; new_image++)
      {
        if ( new_image != current_image )
        {
          if ( mdsp_image_supports_module(new_image, module_reqd) == TRUE &&
               mdsp_get_mode(new_image, module_reqd,
                             current_mode, mod_selection) == TRUE )
          {
            break;
          }
        }
      }
    }
  }
  return new_image;
}

/* <EJECT> */
/*===========================================================================

FUNCTION mdsp_inform_enabled_apps

DESCRIPTION
  Sends a MDSP_MOD_READY event to all the newly enabled applications.

DEPENDENCIES
  mdsp_app_table

RETURN VALUE
  None

SIDE EFFECTS
  Updates the state of the enabled apps.

===========================================================================*/
void mdsp_inform_enabled_apps(void)
{
  mdsp_app_type app_id;

  for (app_id=(mdsp_app_type)0; app_id<MDSP_APP_MAX; app_id++)
  {
    if (mdsp_app_table[app_id].state == MDSP_APP_STATE_ENABLING)
    {
      mdsp_app_table[app_id].state = MDSP_APP_STATE_ENABLED;
      mdsp_send_module_event(app_id, MDSP_MOD_READY, &mdsp_current_image);
      #ifdef FEATURE_MDSPSVC_LOG
      mdsp_log_process_app_state_change(app_id);
      #endif
    }
  }
}

/*===========================================================================

FUNCTION mdsp_all_acks_recvd

DESCRIPTION
  Checks if there are any application that have not yet responded to a
  MOD_DISABLE event.

DEPENDENCIES
  mdsp_app_table

RETURN VALUE
  FALSE if at least one application has not yet responded.
  TRUE otherwise.

SIDE EFFECTS
  None.
  
===========================================================================*/
boolean mdsp_all_acks_recvd(
  mdsp_app_type *offending_app /* Application that did not respond */
)
{
  boolean             all_acks_received = TRUE;
  mdsp_app_type app;
  mdsp_app_table_type *app_ptr;

  *offending_app = MDSP_APP_MAX;

  for (app = (mdsp_app_type)0; app < MDSP_APP_MAX; app++)
  {
    app_ptr = &mdsp_app_table[app];

    if (app_ptr->state == MDSP_APP_STATE_PREEMPTING)
    {
      all_acks_received = FALSE;
      *offending_app = app;
      break;
    }
  }
  return all_acks_received;
}

/*===========================================================================

FUNCTION mdsp_state_module_switch_complete

DESCRIPTION
  If no applications are enabled, it disables the MDSP. Else it informs all
  the newly enabled apps. that the DSP is ready for use.

DEPENDENCIES
  mdsp_clock_mask

RETURN VALUE
  TRUE if at least one application has not yet responded.

SIDE EFFECTS
  mdsp_services_current_state
  
===========================================================================*/
void mdsp_state_module_switch_complete(
  boolean   *mod_selection
)
{
  mdsp_app_type app_id;
  boolean       config_reqd = TRUE;

  if (mdsp_two_step_module_switch == TRUE)
  {
    mdsp_two_step_module_switch = FALSE;
    for (app_id=(mdsp_app_type)0; app_id<MDSP_APP_MAX; app_id++)
    {
      if (mdsp_app_table[app_id].state == MDSP_APP_STATE_ENABLE_REQUESTED)
      {
        mdsp_pending_app_request = TRUE;
        config_reqd = FALSE;
        break;
      }
    }

    if (config_reqd == TRUE) {
      /* Ensure that we switch the hardware to the correct modes for the
      ** the modules that remain enabled.
      */
      /* Switch the DME mode */
      mdsp_config_dme_mode_for_modules(mod_selection);

      /* Switch clock rates if necessary */
      mdsp_config_dsp_clock_for_modules(mod_selection);

      /* Set the appropriate DSP memory mode. */
      mdsp_memshare_ctrl_func(mdsp_mode_required->memory_config);
    }
  }
#ifdef MDSP_MASTER_CONTROLLER_USED
  mdsp_clocks_disable(MDSP_APP_SELF);
#endif
  mdsp_inform_enabled_apps();
  mdsp_services_current_state = MDSP_STATE_IDLE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_ENABLE_MODULES

DESCRIPTION
  Determines which applications to enable/disable, and selects the appropriate
  image and DSP mode.

DEPENDENCIES
  mdsp_services_current_state
  mdsp_app_table
  mdsp_current_image
  mdsp_reset_image

RETURN VALUE
  None.

SIDE EFFECTS
  mdsp_pending_app_request

===========================================================================*/
void mdsp_enable_modules(void)
{
  mdsp_app_type     app;
  mdsp_module_type  module;
  mdsp_app_type     not_ready_app_id;
  mdsp_app_type     high_pri_app   = MDSP_APP_MAX;
  mdsp_image_type   image_required = mdsp_current_image;

  /* Select new set of modules only if IDLE */
  if (mdsp_services_current_state != MDSP_STATE_IDLE)
  {
    return;
  }

  mdsp_pending_app_request = FALSE;

  for (module=(mdsp_module_type)0; module<MDSP_MODULE_MAX; module++)
  {
    mdsp_mod_selection[module] = FALSE;
  }
  
  /* Enable modules based on the application priority, the image and mode
   * selection. */
  for (app = (mdsp_app_type)0; app < MDSP_APP_MAX; app++)
  {
    module = mdsp_app_table[app].module;

    switch (mdsp_app_table[app].state)
    {
      case MDSP_APP_STATE_PREEMPTING:
        /* Should never get here if we are not busy */
        ERR_FATAL("Internal state error", 0, 0, 0);

      case MDSP_APP_STATE_ENABLING:
      case MDSP_APP_STATE_ENABLE_REQUESTED:
      case MDSP_APP_STATE_ENABLED:

        if (high_pri_app == MDSP_APP_MAX)
        {
          high_pri_app = app;
          mdsp_mod_selection[module] = TRUE;
          image_required = mdsp_select_image(mdsp_current_image, module,
                                      &mdsp_mode_required, mdsp_mod_selection);

          if (image_required >= MDSP_IMAGE_MAX)
          {
            ERR_FATAL("Unable to find a valid image", 0, 0, 0);
          }

          if (mdsp_app_table[app].state != MDSP_APP_STATE_ENABLED)
          {
            mdsp_app_table[app].state = MDSP_APP_STATE_ENABLING;
          }

          if (mdsp_reset_image != MDSP_IMAGE_MAX &&
              mdsp_reset_image != image_required)
          {
            /* Ignore any reset request because the image is about to change
             * anyway, which will cause the DSP to be reset. */
            mdsp_reset_image = MDSP_IMAGE_MAX;
          }
        }
        else
        {
          if (mdsp_image_supports_module(image_required, module) == TRUE)
          {
            if (mdsp_get_mode(image_required, module,
                              &mdsp_mode_required, mdsp_mod_selection) == TRUE)
            {
              mdsp_mod_selection[module] = TRUE;

              if (mdsp_app_table[app].state == MDSP_APP_STATE_ENABLED)
              {
                if (mdsp_current_image != MDSP_IMAGE_MAX &&
                    image_required != mdsp_current_image)
                {
                  mdsp_disable_app(app, MDSP_DISABLE_REASON_PREEMPTING);
                }
              }
              else
              {
                mdsp_app_table[app].state = MDSP_APP_STATE_ENABLING;
              }
            }
            else
            {
              /* Can't support with this image */
              mdsp_disable_app(app, MDSP_DISABLE_REASON_PREEMPTING);
            }
          }
          else
          {
            /* Can't support with this image */
            mdsp_disable_app(app, MDSP_DISABLE_REASON_PREEMPTING);
          }
        }

        /* If there is a pending Reset request, disable the application if it
         * is already active, to allow a safe Reset operation. */
        if (mdsp_reset_image != MDSP_IMAGE_MAX &&
            mdsp_reset_image == image_required &&
            mdsp_app_table[app].state == MDSP_APP_STATE_ENABLED)
        {
          mdsp_disable_app(app, MDSP_DISABLE_REASON_PREEMPTING);
        }
        break;

      default:
        ERR_FATAL("Invalid state for App %d", app, 0, 0);
        /* Fall through */

      case MDSP_APP_STATE_DISABLED:
        break;
    }
  }

  /* At least one app. may have gotten disabled.
   * Check if all of them have responded. */
  if (mdsp_all_acks_recvd(&not_ready_app_id) == FALSE)
  {
    /* Wait to receive all the ACKs. */
    mdsp_services_current_state = MDSP_STATE_WAITING_FOR_APP_RESET;
    (void) rex_set_timer(&mdsp_image_reset_timer, MDSP_IMAGE_RESET_TIME);
  }
  else
  {
    /* Some images which don't have a master controller generate interrupts
       immediately after they are reset. We'll guarantee that the MOD_READY
       is received by the clients before any interrupts are passed to them */
    if (mdsp_modules_info[image_required].bitmasks == NULL)
    {
      mdsp_download_dsp_image(image_required);
      if (mdsp_start_modules_on_dsp(mdsp_mod_selection) == TRUE)
      {
        mdsp_state_module_switch_complete(mdsp_mod_selection);
      }
      else
      {
        /* Should always succeed if there is no master controller */
        ERR_FATAL("Internal state error", 0,0,0);
      }
      /* Restore Interrupts saved in mdsp_download_dsp_image() */
      INTFREE_SAV(mdsp_saved_intlock_mask);
    }
    /* In the case where a master controller is present, the DSP will not
       generate any interrupts for clients until their modules are enabled.
       Also, we cannot depend on the master controller to switch modules
       instantaneously - so, we can't use any INTLOCKs like we did in the 
       above case */
    else
    {
      mdsp_download_dsp_image(image_required);
      if (mdsp_start_modules_on_dsp(mdsp_mod_selection) == TRUE)
      {
        mdsp_state_module_switch_complete(mdsp_mod_selection);
      }
      /* else wait for the modules to start on the DSP. */
    }
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION mdsp_check_all_apps_state

DESCRIPTION
  If all apps. has responded to any pending events, it clears the RESET_SIG
  timer.

DEPENDENCIES

RETURN VALUE
  TRUE if all apps has responded.

SIDE EFFECTS
  None.

===========================================================================*/
boolean mdsp_check_all_apps_state(void)
{
  boolean        all_apps_responded = TRUE;
  mdsp_app_type  not_ready_app_id;

  all_apps_responded = mdsp_all_acks_recvd(&not_ready_app_id);
  if (all_apps_responded == TRUE)
  {
    /* Clear the image reset timer
    */
    (void) rex_clr_timer( &mdsp_image_reset_timer );
    (void) rex_clr_sigs( &mdsp_tcb, MDSP_IMAGE_RESET_SIG );
  }

  return all_apps_responded;
}

/* <EJECT> */
/*===========================================================================

FUNCTION mdsp_check_state

DESCRIPTION
  Checks the current state of MDSP Services makes the appropriate state
  transitions.

DEPENDENCIES
  mdsp_services_current_state
  mdsp_pending_app_request

RETURN VALUE
  None.

SIDE EFFECTS
  mdsp_services_current_state
  mdsp_pending_app_request

===========================================================================*/
void mdsp_check_state(void)
{
  switch (mdsp_services_current_state)
  {
    case MDSP_STATE_IDLE:
      break;

    case MDSP_STATE_WAITING_FOR_APP_RESET:
      if (mdsp_check_all_apps_state() == TRUE)
      {
        mdsp_services_current_state = MDSP_STATE_IDLE;
        mdsp_pending_app_request = TRUE;
      }
      else
      {
        /* Do nothing while waiting for the Apps to respond. */
        return;
      }
      break;

    case MDSP_STATE_WAITING_FOR_MODULE_SWITCH:
      /* Do nothing while waiting for the MDSP response interrupt. */
      return;

    case MDSP_STATE_MODULE_SWITCH_SUCCESSFUL:
      mdsp_state_module_switch_complete(mdsp_mod_selection);
      break;

    default:
      ERR_FATAL("Invalid internal state", 0, 0, 0);
      break;
  }

  (void) rex_clr_sigs( &mdsp_tcb, MDSP_CHECK_STATE_SIG );

  if (mdsp_pending_app_request == TRUE)
  {
    mdsp_enable_modules();
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION mdsp_process_pending_request

DESCRIPTION
  Causes any outstanding application requests to be processed.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  mdsp_pending_app_request

===========================================================================*/
void mdsp_process_pending_request(void)
{
  mdsp_pending_app_request = TRUE;
  mdsp_check_state();
}

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_REGISTER_APP_PROCESSING

DESCRIPTION
  Register interrupt handler functions, event callback function and event mask.
  Once registered, applications will begin receiving all events in their
  event mask.  The application's interrupt handlers will only be called
  when the application has enabled the MDSP by calling mdsp_enable().

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
  command has been accepted.

===========================================================================*/
void mdsp_register_app_processing (
  /* The register command packet */
  mdsp_packet_type* packet
)
{
  mdsp_app_type app_id;

  ASSERT( packet != NULL );

  /* app_id already verified in mdsp_register_app() */
  app_id = packet->register_app_cmd.app_id;

  /* Register the application */
  mdsp_app_table[app_id].registered = TRUE;
  mdsp_app_table[app_id].cb_ptr     = packet->register_app_cmd.cb_ptr;
  mdsp_app_table[app_id].event_mask = packet->register_app_cmd.event_mask;

#ifdef MDSP_INTERRUPTS_USED
  if (packet->register_app_cmd.isrs_ptr != NULL)
  {
    memcpy(mdsp_app_table[app_id].isrs, packet->register_app_cmd.isrs_ptr,
           sizeof(mdsp_app_table[app_id].isrs));
  }
  else
  {
    /* Make sure that we clear any previously registered ISRs */
    memset(mdsp_app_table[app_id].isrs, NULL,
           sizeof(mdsp_app_table[app_id].isrs));
  }
#endif

  /* Notify application that command was accepted */
  mdsp_send_cmd_event( app_id, MDSP_CMD_ACCEPT ); 

} /* mdsp_register_app_processing */

#ifdef MDSP_INTERRUPTS_USED
/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_REGISTER_ISR_PROCESSING

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
  command has been accepted.

===========================================================================*/
void mdsp_register_isr_processing (
  /* The register command packet */
  mdsp_packet_type* packet
)
{
  mdsp_app_type app_id;
  mdsp_command_status_type cmd_status = MDSP_CMD_ACCEPT;

  ASSERT( packet != NULL );

  /* app_id already verified in mdsp_register_isr() */
  app_id = packet->register_isr_cmd.app_id;

  if (!mdsp_app_table[app_id].registered)
  {
    MSG_ERROR("mdsp_register_isr failed: Unregistered app_id = %d",app_id,0,0);
  }
  else
  {
    if (packet->register_isr_cmd.isr_type >= MDSP_ISR_MAX)
    {
      MSG_ERROR("Invalid ISR type = %d", packet->register_isr_cmd.isr_type,0,0);
      cmd_status = MDSP_CMD_FAIL;
    }
    else
    {
      /* Register the ISR */
      mdsp_app_table[app_id].isrs[packet->register_isr_cmd.isr_type] =
                                              packet->register_isr_cmd.isr_ptr;
    }
    mdsp_send_cmd_event( app_id, cmd_status ); 
  }

} /* mdsp_register_isr_processing */
#endif /* MDSP_INTERRUPTS_USED */

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_ENABLE_PROCESSING

DESCRIPTION
  Applications use this function to indicate which MDSP module should be
  enabled. When the calling application's priority is high enough,
  MDSP Services will issue a callback to the application, indicating that it
  has been enabled. When the "MOD_READY" event is received, the MDSP clocks are
  on, the current firmware image is running and the requested module is ready
  for use on the MDSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A MDSP_EV_COMMAND event will issued to the calling application once the
  command has been accepted.

===========================================================================*/
void mdsp_enable_processing (
  /* The enable command packet */
  mdsp_packet_type* packet
)
{
  mdsp_app_type   app_id;

  ASSERT( packet != NULL );

  /* app_id already verified in mdsp_enable() */
  app_id = packet->enable_cmd.app_id;

  if ( mdsp_app_table[app_id].registered == FALSE )
  {
    MSG_ERROR( "mdsp_enable failed: Unregistered app_id = %d", app_id, 0, 0 );
  }
  else
  {
    switch (mdsp_app_table[app_id].state)
    {
      case MDSP_APP_STATE_ENABLED:
        if (mdsp_app_table[app_id].module == packet->enable_cmd.module)
        {
          mdsp_clocks_enable( app_id );
          mdsp_send_cmd_event(app_id, MDSP_CMD_ACCEPT); 
          mdsp_send_module_event(app_id, MDSP_MOD_READY, &mdsp_current_image);
          break;
        }
        /* else fall through */

      case MDSP_APP_STATE_DISABLED:
      case MDSP_APP_STATE_ENABLE_REQUESTED:
      case MDSP_APP_STATE_ENABLING:
        mdsp_app_table[app_id].state  = MDSP_APP_STATE_ENABLE_REQUESTED;
        mdsp_app_table[app_id].module = packet->enable_cmd.module;

        /* Send the CMD_ACCEPT event before we start processing the request. */
        mdsp_send_cmd_event( app_id, MDSP_CMD_ACCEPT ); 

        #ifdef FEATURE_MDSPSVC_LOG
        /* Process state change since we may have been enabled before */
        mdsp_log_process_app_state_change(app_id);
        #endif

        mdsp_clocks_enable( app_id );
        mdsp_process_pending_request();
        break;

      case MDSP_APP_STATE_PREEMPTING:
        MSG_ERROR("mdsp_enable() called before responding to MOD_DISABLE",0,0,0);
        mdsp_send_cmd_event( app_id, MDSP_CMD_FAIL ); 
        break;

      default:
        ERR_FATAL("Unknown Application state!", 0, 0, 0);
        break;
    }
  }

} /* mdsp_enable_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_DISABLE_PROCESSING

DESCRIPTION
  Disables the MDSP when no applications require it to be enabled.  When the
  MDSP is disabled, its clocks are off and the current firmware image is not
  running.  The MDSP draws minimum current when it is disabled.

DEPENDENCIES
  mdsp_qpp_table

RETURN VALUE
  None

SIDE EFFECTS
  A MDSP_EV_COMMAND event will issued to the calling application once the
  command has been accepted.

===========================================================================*/
void mdsp_disable_processing (
  /* The disable command packet */
  mdsp_packet_type* packet
)
{
  mdsp_app_type app_id;
  mdsp_app_state_type      saved_app_state;

  ASSERT( packet != NULL );

  /* app_id already verified in mdsp_disable() */
  app_id = packet->disable_cmd.app_id;

  if ( mdsp_app_table[app_id].registered == FALSE )
  {
    MSG_ERROR( "mdsp_disable failed: Unregistered app_id = %d", app_id, 0, 0 );
  }
  else
  {
    saved_app_state = mdsp_app_table[app_id].state;
    mdsp_app_table[app_id].state = MDSP_APP_STATE_DISABLED;
    mdsp_send_cmd_event( app_id, MDSP_CMD_ACCEPT ); 

    switch (saved_app_state)
    {
      case MDSP_APP_STATE_DISABLED:
        /* NOP */
        break;

      case MDSP_APP_STATE_ENABLE_REQUESTED:
        mdsp_clocks_disable( app_id );
        break;

      #ifdef FEATURE_MDSPSVC_LOG
      case MDSP_APP_STATE_ENABLED:
        mdsp_log_process_app_state_change(app_id);
        /* fall through */
      #endif

      default:
        #ifdef MDSP_USES_OVERLAYED_IMAGES
        mdsp_unload_dsp_module(mdsp_current_image,
                               mdsp_app_table[app_id].module);
        #endif
        mdsp_process_pending_request();
        mdsp_clocks_disable( app_id );
        break;
    }
  }

} /* mdsp_disable_processing */


/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_RESET_AND_ENABLE_PROCESSING

DESCRIPTION
  Applications use this function to reset the DSP and enable a module.
  This is useful when the client wants to ensure that MDSP services actually
  does a reset and redownload of the image, before it is enabled. In the
  normal scenario, the DSP is not reset between successive calls to
  mdsp_enable() and mdsp_disable(), if the image has not changed.

  NOTE: This interface must used only as a debugging aid, since resetting the
  DSP may cause other MDSP Services clients to react adversely.

DEPENDENCIES
  mdsp_app_table

RETURN VALUE
  None

SIDE EFFECTS
  A MDSP_EV_COMMAND event will issued to the calling application once the
  command has been accepted.

===========================================================================*/
void mdsp_reset_and_enable_processing (
  /* The disable command packet */
  mdsp_packet_type* packet
)
{
  mdsp_app_type app_id;

  ASSERT( packet != NULL );

  /* app_id already verified in mdsp_reset() */
  app_id = packet->reset_and_enable_cmd.app_id;

  if ( mdsp_app_table[app_id].registered == FALSE )
  {
    MSG_ERROR( "Unregistered app_id = %d", app_id, 0, 0 );
    mdsp_send_cmd_event( app_id, MDSP_CMD_FAIL ); 
  }
  else
  {
    switch (mdsp_app_table[app_id].state)
    {
      case MDSP_APP_STATE_ENABLED:
      case MDSP_APP_STATE_DISABLED:
      case MDSP_APP_STATE_ENABLE_REQUESTED:
      case MDSP_APP_STATE_ENABLING:
        mdsp_app_table[app_id].state  = MDSP_APP_STATE_ENABLE_REQUESTED;
        mdsp_app_table[app_id].module = packet->reset_and_enable_cmd.module;

        /* Send the CMD_ACCEPT event before we start processing the request. */
        mdsp_send_cmd_event( app_id, MDSP_CMD_ACCEPT ); 

        #ifdef FEATURE_MDSPSVC_LOG
        /* Process state change since we may have been enabled before */
        mdsp_log_process_app_state_change(app_id);
        #endif

        mdsp_clocks_enable( app_id );

        /* Reset the MDSP and force re-download of image */
        mdsp_reset_image = mdsp_current_image;

        mdsp_process_pending_request();
        break;

      case MDSP_APP_STATE_PREEMPTING:
        MSG_ERROR("mdsp_reset_and_enable() called before responding to MOD_DISABLE", 0, 0, 0);
        mdsp_send_cmd_event( app_id, MDSP_CMD_FAIL ); 
        break;

      default:
        ERR_FATAL("Unknown Application state!", 0, 0, 0);
        break;
    }
  }
} /* mdsp_reset_and_enable_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_DISABLE_EVENT_RSP_PROCESSING

DESCRIPTION
  Processes the clients' response to a previously sent MOD_DISABLE event.

DEPENDENCIES
  mdsp_app_table

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void mdsp_disable_event_rsp_processing (
  /* The disable_event_rsp command packet */
  mdsp_packet_type* packet
)
{
  mdsp_app_type app_id;

  ASSERT( packet != NULL );

  /* app_id already verified in mdsp_disable_event_rsp() */
  app_id = packet->disable_event_rsp_cmd.app_id;

  if (mdsp_app_table[app_id].registered == FALSE)
  {
    MSG_ERROR("Unregistered app_id = %d", app_id,0,0);
  }
  else
  {
    switch (mdsp_app_table[app_id].state)
    {
      case MDSP_APP_STATE_PREEMPTING:
        mdsp_app_table[app_id].state  = MDSP_APP_STATE_ENABLE_REQUESTED;
#ifdef MDSP_USES_OVERLAYED_IMAGES
        mdsp_unload_dsp_module(mdsp_current_image,
                               mdsp_app_table[app_id].module);
#endif
        mdsp_send_cmd_event( app_id, MDSP_CMD_ACCEPT ); 
        mdsp_process_pending_request();
        return;

      case MDSP_APP_STATE_DISABLED:
      case MDSP_APP_STATE_ENABLE_REQUESTED:
      case MDSP_APP_STATE_ENABLING:
      case MDSP_APP_STATE_ENABLED:
        MSG_ERROR("Unexpected disable_event_rsp from app %d", app_id, 0, 0);
        break;

      default:
        ERR_FATAL("Unknown Application state!", 0, 0, 0);
        break;
    }
    mdsp_send_cmd_event( app_id, MDSP_CMD_FAIL ); 
  }

} /* mdsp_disable_event_rsp_processing */

#ifdef MDSP_MASTER_CONTROLLER_USED
/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_CHECK_MC_CMD_STATUS

DESCRIPTION
  Checks if Master Controller has executed the previously issued command
  until MDSP_DSP_CMD_TIMEOUT_VAL time has expired.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void mdsp_check_mc_cmd_status(void)
{
  uint32 error_state;

  (void) rex_clr_sigs( &mdsp_tcb, MDSP_DSP_CMD_TIMEOUT_SIG );

  if (mdsp_modules_info[mdsp_current_image].bitmasks != NULL)
  {
    if ((error_state = mdsp_read(MDSP_dspHalt)) != 0)
    {
      MSG_FATAL("MDSP halt (0x%x)", error_state, 0, 0);
      mdsp_recover_from_dsp_failure();
    }
    else
    {
      mdsp_mc_status_chk_time += MDSP_MC_STATUS_CHK_VAL;
      if (mdsp_mc_status_chk_time <= MDSP_DSP_CMD_TIMEOUT_VAL)
      {
        MSG_ERROR("Checking MC status manually", 0,0,0);

        INTLOCK();
        mdsp_master_controller_isr();
        if (mdsp_services_current_state != MDSP_STATE_MODULE_SWITCH_SUCCESSFUL)
        {
          (void) rex_set_timer(&mdsp_dsp_cmd_rsp_timer, MDSP_MC_STATUS_CHK_VAL);
        }
        INTFREE();
      }
      else if (mdsp_services_current_state !=
                                          MDSP_STATE_MODULE_SWITCH_SUCCESSFUL)
      {
        mdsp_recover_from_dsp_failure();
      }
    }
  }
  /* else ignore unexpected signal */
}
#endif /* MDSP_MASTER_CONTROLLER_USED */

/* <EJECT> */
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
void mdsp_clocks_enable ( 
  /* Application ID */
  mdsp_app_type app_id
)
{
  uint32               saved_int_mask = 0;
  uint16               old_clock_mask = 0;
  #ifdef MDSP_MASTER_CONTROLLER_USED
  uint32               curr_poll_ms = 0;
  #endif /* MDSP_MASTER_CONTROLLER_USED */

  if (app_id <= MDSP_APP_MAX || app_id == MDSP_APP_SELF)
  {
    /* Critical section - enable clocks, if not already */ 
    INTLOCK_SAV(saved_int_mask);

    old_clock_mask = mdsp_clock_mask;

    mdsp_clock_mask |= MDSP_APP_CLK_MASK( (uint16)app_id );

    if (old_clock_mask == 0)
    {
      clk_regime_enable_client( CLK_RGM_MODEM_M, CLKRGM_MDSP );
      clk_regime_enable_client( CLK_RGM_OFFLINE_M, CLKRGM_MDSP );
      clk_regime_enable_client( CLK_RGM_CHIPX8_M, CLKRGM_MDSP );
      clk_regime_rxf_request( CLKRGM_RXF_MDSP, CLKRGM_RXF_ENABLE);

      #ifdef MDSP_MASTER_CONTROLLER_USED
      if (mdsp_current_image < MDSP_IMAGE_MAX &&
          mdsp_modules_info[mdsp_current_image].bitmasks != NULL)
      {
        /* Tell the DSP to resume if we have previously suspended it */
        if (mdsp_read(MDSP_microMCCmd) == 0)
        {
          mdsp_write(MDSP_microMCCmd, MDSP_CMD_DSP_RESUME);
          mdsp_gen_interrupt(mdsp_modules_info[mdsp_current_image].mc_irq);

          /* Wait for the DSP to acknowledge the resume command */
          while (curr_poll_ms < MDSP_MC_POLL_TIME_MAX)
          {
            if (mdsp_read(MDSP_microMCCmd) == 0)
            {
              break;
            }
            clk_busy_wait(MDSP_MC_POLL_INCREMENT);
            curr_poll_ms += MDSP_MC_POLL_INCREMENT;
          }

          if (curr_poll_ms >= MDSP_MC_POLL_TIME_MAX)
          {
            INTFREE_SAV(saved_int_mask);
            MSG_FATAL("MC didn't respond to resume cmd", 0, 0, 0);
            mdsp_recover_from_dsp_failure();
            return;
          }
        }
        else
        {
          INTFREE_SAV(saved_int_mask);
          MSG_FATAL("MDSP MC is unexpectedly busy!", 0, 0, 0);
          mdsp_recover_from_dsp_failure();
          return;
        }
      }
      #endif /* MDSP_MASTER_CONTROLLER_USED */
    }

    #ifdef FEATURE_MDSPSVC_LOG
    if (mdsp_clock_mask != old_clock_mask)
    {
       mdsp_log_process_app_state_change( app_id );
    }
    #endif

    INTFREE_SAV(saved_int_mask);
  }
  else
  {
    ERR_FATAL("mdsp_clocks_enable failed: Invalid app_id = %d", app_id, 0, 0);
  }

} /* mdsp_clocks_enable */

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
void mdsp_clocks_disable ( 
  /* Application ID */
  mdsp_app_type app_id
)
{
  uint16 prev_clock_mask = 0;
  uint32 saved_int_mask  = 0;
  #ifdef MDSP_MASTER_CONTROLLER_USED
  uint32 curr_poll_ms    = 0;
  #endif

  if (app_id <= MDSP_APP_MAX || app_id == MDSP_APP_SELF)
  {
    /* Critical section - disable clocks, if necessary */ 
    INTLOCK_SAV(saved_int_mask);

    prev_clock_mask = mdsp_clock_mask;

    mdsp_clock_mask &= ~MDSP_APP_CLK_MASK( (uint16)app_id );

    #ifdef FEATURE_MDSPSVC_LOG
    if (mdsp_clock_mask != prev_clock_mask)
    {
       mdsp_log_process_app_state_change( app_id );
    }
    #endif

    if ( mdsp_clock_mask == 0 )
    {
      #ifdef MDSP_MASTER_CONTROLLER_USED
      if (prev_clock_mask &&
          mdsp_current_image < MDSP_IMAGE_MAX &&
          mdsp_modules_info[mdsp_current_image].bitmasks != NULL)
      {
        /* If this image uses a Master Controller, make sure that it goes to a
         * quiescent state before we turn off the clocks */
        if (mdsp_read(MDSP_microMCCmd) == 0)
        {
          /* Initiate DSP WAIT cycles */
          mdsp_write(MDSP_microMCCmd, MDSP_CMD_DSP_SUSPEND);
          mdsp_gen_interrupt(mdsp_modules_info[mdsp_current_image].mc_irq);

          /* Check the status to confirm that the DSP has acknowledged the
           * suspend command */
          while (curr_poll_ms < MDSP_MC_POLL_TIME_MAX)
          {
            if (mdsp_read(MDSP_microMCCmd) == 0)
            {
              break;
            }
            clk_busy_wait(MDSP_MC_POLL_INCREMENT);
            curr_poll_ms += MDSP_MC_POLL_INCREMENT;
          }

          if (curr_poll_ms >= MDSP_MC_POLL_TIME_MAX)
          {
            INTFREE_SAV(saved_int_mask);
            MSG_FATAL("MC didn't respond to suspend cmd", 0, 0, 0);
            mdsp_recover_from_dsp_failure();
            return;
          }
        }
        else
        {
          INTFREE_SAV(saved_int_mask);
          MSG_FATAL("MDSP MC is unexpectedly busy!", 0, 0, 0);
          mdsp_recover_from_dsp_failure();
          return;
        }
      }
      #endif /* MDSP_MASTER_CONTROLLER_USED */

      mdsp_config_hw_resources_for_sleep_mode();
      clk_regime_rxf_request( CLKRGM_RXF_MDSP, CLKRGM_RXF_DISABLE);
      clk_regime_disable_client( CLK_RGM_MODEM_M, CLKRGM_MDSP );
      clk_regime_disable_client( CLK_RGM_OFFLINE_M, CLKRGM_MDSP );
      clk_regime_disable_client( CLK_RGM_CHIPX8_M, CLKRGM_MDSP );
    }

    INTFREE_SAV(saved_int_mask);
  }
  else
  {
    ERR_FATAL("mdsp_clocks_disable failed: Invalid app_id = %d", app_id, 0, 0);
  }

} /* mdsp_clocks_disable */
    
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
boolean mdsp_poke_reg (
  uint16 reg,  /* The register address to poke */
  uint32 data  /* The data to write */
)
{
  #ifdef MDSP_MASTER_CONTROLLER_USED
  uint32 curr_poll_ms;
  boolean success = FALSE;
  
  /* Critical section.  Do not want MDSP task to perform any memory 
  ** accesses while poke operation is in progress.
  */
  INTLOCK();

  /* We cannot poke if we already have an outstanding microMC command or
     no image is running. */
  if (mdsp_services_current_state == MDSP_STATE_WAITING_FOR_MODULE_SWITCH ||
      mdsp_current_image == MDSP_IMAGE_MAX ||
      mdsp_modules_info[mdsp_current_image].bitmasks == NULL ||
      mdsp_read(MDSP_microMCCmd) != 0)
  {
     success = FALSE;
  }
  else
  {
    /* Enable MDSP clocks */
    mdsp_clocks_enable( MDSP_APP_MAX );

    /* Set the address and data values, then send the poke command */
    mdsp_write(MDSP_regAddress, reg);
    mdsp_write(MDSP_regValue, data);
    mdsp_write(MDSP_microMCCmd, MDSP_MC_CMD_XMEMC_WRITE);
    mdsp_gen_interrupt(mdsp_modules_info[mdsp_current_image].mc_irq);

    /* Wait for the DSP to acknowledge the resume command */
    curr_poll_ms = 0;
    while (curr_poll_ms < MDSP_MC_POLL_TIME_MAX)
    {
      if (mdsp_read(MDSP_microMCCmd) == 0)
      {
        success = TRUE;
        break;
      }
      clk_busy_wait(MDSP_MC_POLL_INCREMENT);
      curr_poll_ms += MDSP_MC_POLL_INCREMENT;
    }

    if (curr_poll_ms >= MDSP_MC_POLL_TIME_MAX)
    {
      success = FALSE;
      MSG_ERROR("MC did not respond to poke cmd (reg=%d, data=%d)", reg, data, 0);
    }

    /* Disable MDSP clocks */
    mdsp_clocks_disable( MDSP_APP_MAX );
  }

  INTFREE();

  return success;

  #else
  
  /* This is not supported if there is no master controller */
  return FALSE;

  #endif /* MDSP_MASTER_CONTROLLER_USED */

} /* mdsp_poke_reg */

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
boolean mdsp_peek_reg (
  uint16  reg,  /* The register address to peek */
  uint32 *data  /* Where to store the retrieved data */
)
{
  #ifdef MDSP_MASTER_CONTROLLER_USED
  uint32 curr_poll_ms;
  boolean success = FALSE;
  
  /* Critical section.  Do not want MDSP task to perform any memory 
  ** accesses while poke operation is in progress.
  */
  INTLOCK();

  /* We cannot poke if we already have an outstanding microMC command or
     no image is running. */
  if (mdsp_services_current_state == MDSP_STATE_WAITING_FOR_MODULE_SWITCH ||
      mdsp_current_image == MDSP_IMAGE_MAX ||
      mdsp_modules_info[mdsp_current_image].bitmasks == NULL ||
      mdsp_read(MDSP_microMCCmd) != 0 ||
      data == NULL)
  {
     success = FALSE;
  }
  else
  {
    /* Enable MDSP clocks */
    mdsp_clocks_enable( MDSP_APP_MAX );

    /* Set the address, then send the peek command - set the value to some
       flag to help catch bad reads. */
    mdsp_write(MDSP_regAddress, reg);
    mdsp_write(MDSP_regValue, 0xDEADBEEF);
    mdsp_write(MDSP_microMCCmd, MDSP_MC_CMD_XMEMC_READ);
    mdsp_gen_interrupt(mdsp_modules_info[mdsp_current_image].mc_irq);

    /* Wait for the DSP to acknowledge the resume command */
    curr_poll_ms = 0;
    while (curr_poll_ms < MDSP_MC_POLL_TIME_MAX)
    {
      if (mdsp_read(MDSP_microMCCmd) == 0)
      {
        success = TRUE;
        *data = mdsp_read(MDSP_regValue);
        break;
      }
      clk_busy_wait(MDSP_MC_POLL_INCREMENT);
      curr_poll_ms += MDSP_MC_POLL_INCREMENT;
    }

    if (curr_poll_ms >= MDSP_MC_POLL_TIME_MAX)
    {
      success = FALSE;
      MSG_ERROR("MC did not respond to peek cmd (reg=%d)", reg, 0, 0);
    }

    /* Disable MDSP clocks */
    mdsp_clocks_disable( MDSP_APP_MAX );
  }

  INTFREE();

  return success;

  #else
  
  /* This is not supported if there is no master controller */
  return FALSE;

  #endif /* MDSP_MASTER_CONTROLLER_USED */

} /* mdsp_poke_reg */

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
void mdsp_cmd (
  /* The command packet */
  mdsp_packet_type* mdsp_cmd_ptr
)
{
  ASSERT( mdsp_cmd_ptr != NULL );

  /* Put the command packet in the queue */
  (void) q_link( mdsp_cmd_ptr, &mdsp_cmd_ptr->hdr.link );
  q_put( &mdsp_cmd_q, &mdsp_cmd_ptr->hdr.link);

  /* Signal task to process command queue */
  (void) rex_set_sigs( &mdsp_tcb, MDSP_CMD_Q_SIG );

} /* mdsp_cmd */

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
  mdsp_free_q (and the packet fetched off mdsp_free_q)

===========================================================================*/
mdsp_packet_type* mdsp_get_packet( void )
{
  mdsp_packet_type* packet;

  /* Get a packet and then either report an error, or clear the packet out
  ** so the previous user's garbage doesn't do something unexpected.
  */
  packet = (mdsp_packet_type*) q_get( &mdsp_free_q );

  if (packet == NULL) 
  {
    ERR( "Ran out of MDSP packets", 0,0,0 );
  } 
  else 
  {
    /* Fill the entire MDSP packet with 0's to get
    ** default behavior on un-initialized fields.
    */
    (void) memset( packet, 0, sizeof( mdsp_packet_type ) );
    packet->hdr.cmd = MDSP_CMD_INVALID;
  }

  return( packet );

} /* mdsp_get_packet */

/* <EJECT> */
/*===========================================================================

FUNCTION mdsp_cmd_done

DESCRIPTION
  Dispose of a command which is done being processed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void mdsp_cmd_done (
  /* The command packet */ 
  mdsp_packet_type* mdsp_cmd_ptr
)
{
  q_put( &mdsp_free_q, &mdsp_cmd_ptr->hdr.link );

} /* mdsp_cmd_done */

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_CMD_PROCESSOR

DESCRIPTION
  Processes all of the commands in the MDSP Services command queue.  It must
  be called in response to the MDSP_EV_CMD_Q event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_cmd_processor( void )
{
  mdsp_packet_type *cmd_ptr;
  
  /* Clear the 'queued commands' signal prior to emptying the queue.
  ** This is done in order to avoid race conditions where an item is
  ** added to the queue just as we are done servicing the queue.
  */
  (void) rex_clr_sigs( &mdsp_tcb, MDSP_CMD_Q_SIG );

  /* While there are commands to process... */
  while( (cmd_ptr = (mdsp_packet_type *) q_get( &mdsp_cmd_q )) != NULL )
  {
    /* Process each command type */
    switch( cmd_ptr->hdr.cmd )
    {
      /* Register an application */
      case MDSP_CMD_REGISTER_APP:
        mdsp_register_app_processing( cmd_ptr );
        break;

#ifdef MDSP_INTERRUPTS_USED
      /* Register an ISR */
      case MDSP_CMD_REGISTER_ISR:
        mdsp_register_isr_processing( cmd_ptr );
        break;
#endif

      /* Enable the MDSP */
      case MDSP_CMD_ENABLE:
        mdsp_enable_processing( cmd_ptr );
        break;
      
      /* Disable the MDSP */
      case MDSP_CMD_DISABLE:
        mdsp_disable_processing( cmd_ptr );
        break;

      /* Reset the MDSP */
      case MDSP_CMD_RESET_AND_ENABLE:
        mdsp_reset_and_enable_processing( cmd_ptr );
        break;

      /* Application ready for disabling */
      case MDSP_CMD_DISABLE_EVENT_RSP:
        mdsp_disable_event_rsp_processing( cmd_ptr );
        break;

      #ifdef FEATURE_MDSPSVC_LOG
      /* Set the application log mode */
      case MDSP_CMD_LOG_SET_MODE:
        mdsp_log_set_mode_processing( cmd_ptr );
        break;

      /* Send a log packet */
      case MDSP_CMD_LOG_SEND:
        mdsp_log_send_processing( cmd_ptr );
        break;

      /* Save full log to a file */
      case MDSP_CMD_LOG_SAVE:
        mdsp_log_save_processing( cmd_ptr );
        break;

      /* Save mDSP memory to a file */
      case MDSP_CMD_MEMORY_SAVE:
        mdsp_memory_save_processing( cmd_ptr );
        break;
      #endif

      default:
        ERR_FATAL( "Illegal command: %x", cmd_ptr->hdr.cmd, 0, 0 );
        /* Does not return */
    }

    mdsp_cmd_done( cmd_ptr );
  }

} /* mdsp_cmd_processor */

/* <EJECT> */
/*===========================================================================

FUNCTION mdsp_task_init

DESCRIPTION
  Initializes the MDSP task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  mdsp_free_q

===========================================================================*/
void mdsp_task_init( void )
{
  uint16 i;

  /* Initialize timers */
  rex_def_timer( &mdsp_rpt_timer, &mdsp_tcb, MDSP_RPT_TIMER_SIG );
  rex_def_timer( &mdsp_image_reset_timer, &mdsp_tcb, MDSP_IMAGE_RESET_SIG );
  rex_def_timer( &mdsp_dsp_cmd_rsp_timer, &mdsp_tcb, MDSP_DSP_CMD_TIMEOUT_SIG);
 
  /* Initialize the command queues */
  (void) q_init( &mdsp_cmd_q );
  (void) q_init( &mdsp_free_q );

  /* Add packets to the free command queue */
  for( i = 0; i < MDSP_NUM_PACKETS; i++ ) 
  {
    q_link_type *link_item;
    link_item = q_link( &mdsp_free_packet[i], &mdsp_free_packet[i].hdr.link );
    q_put( &mdsp_free_q, link_item );
  }

#ifdef MDSP_INTERRUPTS_USED
  /* Install the MDSP-to-ARM ISR's */
  mdsp_register_dsp_services_isrs();
#endif
  
  /* Initialize the application table */
  for( i = 0; i < (uint16)MDSP_APP_MAX; i++ )
  {
    mdsp_app_table[i].registered     = FALSE;
    mdsp_app_table[i].state          = MDSP_APP_STATE_DISABLED;
    mdsp_app_table[i].module         = MDSP_MODULE_MAX;
    mdsp_app_table[i].cb_ptr         = NULL;
    mdsp_app_table[i].event_mask     = 0;
    #ifdef FEATURE_MDSPSVC_LOG
    mdsp_app_table[i].log_mode       = MDSP_LOG_DISABLED;
    mdsp_app_table[i].log_period     = 0;
    #endif
  }

  MSG_LOW( "MDSP task initialized", 0, 0, 0 );

} /* mdsp_task_init */

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
void mdsp_task (
  /* Parameter received from Main Control task - ignored */
  dword ignored
  /*lint -esym(715,ignored)
  ** Have lint not complain about the ignored parameter 'ignored' which is
  ** specified to make this routine match the template for rex_def_task().
  */
)
{
  rex_sigs_type sigs;
  mdsp_app_type app;

  /* Initialize MDSP task */
  mdsp_task_init();

  /* Wait for start signal from Main Control task */
  #ifdef FEATURE_MULTIMODE_ARCH
  task_start( MDSP_RPT_TIMER_SIG, DOG_MDSP_RPT, &mdsp_rpt_timer );
  #else
  mc_task_start( MDSP_RPT_TIMER_SIG, DOG_MDSP_RPT, &mdsp_rpt_timer );
  #endif /* FEATURE_MULTIMODE_ARCH */

  /* Clear start signal */
  (void) rex_clr_sigs( &mdsp_tcb, TASK_START_SIG );
  
  #ifdef FEATURE_MDSPSVC_LOG
  /* Log init, must be after the task_start function since it talks to NV */
  mdsp_log_init();
  #endif

  /* Set the watchdog report timer signal so that we'll
  ** reset the watchdog as soon as we start checking for events
  */
  (void) rex_set_sigs( &mdsp_tcb, MDSP_RPT_TIMER_SIG );

  /* Infinite loop -- process events forever */
  for( ;; )
  {
    /* Wait for something to happen */
    sigs = rex_wait( MDSP_CMD_Q_SIG            |  /* Command Queue Signal   */
                     MDSP_IMAGE_RESET_SIG      |  /* Image reset signal     */
                     MDSP_CHECK_STATE_SIG      |  /* Update module states   */
                     MDSP_DSP_CMD_TIMEOUT_SIG  |  /* DSP cmd rsp. timed out */
                     MDSP_RPT_TIMER_SIG        |  /* Watchdog report timer  */
                     #ifdef FEATURE_MDSPSVC_LOG
                     MDSP_LOG_TIMER_SIG        |  /* Log timer              */
                     #endif
                     TASK_STOP_SIG             |  /* Task stop from MC      */
                     TASK_OFFLINE_SIG );          /* Task offline from MC   */

    /* If watchdog report timer expired, report and restart the timer */
    if ( ( sigs & MDSP_RPT_TIMER_SIG ) != 0 )
    {
      dog_report( DOG_MDSP_RPT );
      (void) rex_set_timer( &mdsp_rpt_timer, DOG_MDSP_RPT_TIME );
    }

    clk_regime_modem_test_and_unhalt();

    #ifdef FEATURE_MDSPSVC_LOG
    /* Handle log timer signal - do this before processing commands in case
       a pending command disables logging, which will otherwise result in
       an unexpected timer expiry when we process the timer after. */
    if ( ( sigs & MDSP_LOG_TIMER_SIG) != 0 )
    {
      mdsp_log_process_timer();
      (void) rex_clr_sigs( &mdsp_tcb, MDSP_LOG_TIMER_SIG );
    }
    #endif

    #ifdef MDSP_MASTER_CONTROLLER_USED
    /* The DSP Master Controller did not respond to a command. */
    if ( ( sigs & MDSP_DSP_CMD_TIMEOUT_SIG ) != 0 )
    {
      mdsp_check_mc_cmd_status();
    }
    #endif /* MDSP_MASTER_CONTROLLER_USED */

    /* Process the command queue */
    if ( ( sigs & MDSP_CMD_Q_SIG ) != 0 )
    {
      mdsp_cmd_processor();
    }

    /* Process the image reset signal */
    if ( ( sigs & MDSP_IMAGE_RESET_SIG ) != 0 )
    {
      /* If this signal occurs, one or more applications has not
      ** called mdsp_allow_reset within the allotted time.
      */
      ERR("MDSP Image Reset Timer expired",0,0,0);
      
      /* Find the offending application!
      */
      if (mdsp_all_acks_recvd(&app) == FALSE)
      {
        /* Rogue application is not releasing the DSP */
        ERR_FATAL("MDSP App %d did not allow reset",app,0,0);
      }

      /* All apps. responded to the disable request.
       * We can enable the new set of modules.
       */
      (void) rex_clr_sigs( &mdsp_tcb, MDSP_IMAGE_RESET_SIG );
      (void) rex_clr_sigs( &mdsp_tcb, MDSP_CHECK_STATE_SIG );
      mdsp_process_pending_request();
    }
    
    /* Re-evaluate the application and module states */
    if ( ( sigs & MDSP_CHECK_STATE_SIG ) != 0 )
    {
      mdsp_check_state();
    }

    clk_regime_modem_test_and_halt();

    /* Handle task offline signal from MC */
    if ( ( sigs & TASK_OFFLINE_SIG ) != 0 )
    {
      #ifdef FEATURE_MULTIMODE_ARCH
      task_offline();
      #else
      (void) rex_set_sigs( &mc_tcb, MC_ACK_SIG );
      #endif /* FEATURE_MULTIMODE_ARCH */
      (void) rex_clr_sigs( &mdsp_tcb, TASK_OFFLINE_SIG );
    }

    /* Handle task stop signal from MC */
    if ( ( sigs & TASK_STOP_SIG ) != 0 )
    {
      #ifdef FEATURE_MULTIMODE_ARCH
      task_stop();
      #else
      (void) rex_set_sigs( &mc_tcb, MC_ACK_SIG );
      #endif /* FEATURE_MULTIMODE_ARCH */
      (void) rex_clr_sigs( &mdsp_tcb, TASK_STOP_SIG );
    }
  }
} /* mdsp_task */

#ifdef FEATURE_DCVS_LPM
/*===========================================================================

FUNCTION mdsp_set_specific_mdsp_frequency

DESCRIPTION

DEPENDENCIES
  mdsp_current_image

RETURN VALUE
  None.

SIDE EFFECTS
  mdsp_services_current_state

===========================================================================*/
void mdsp_set_specific_mdsp_frequency
(
  clkrgm_mdsp_clk_frequency_type new_frequency
)
{

  uint32               saved_int_mask = 0;
  uint32               curr_poll_counter = 0;

  INTLOCK_SAV(saved_int_mask);

  clk_regime_set_mdsp_clk_frequency(new_frequency);

  /* Initiate DSP WAIT cycles */
  mdsp_write(MDSP_microMCCmd, MDSP_CMD_DSP_SUSPEND);
  mdsp_gen_interrupt(mdsp_modules_info[mdsp_current_image].mc_irq);

  /* The interrupt may belong to some other DSP task. Check the status
  ** to confirm that the DSP has acknowledged the suspend command */
  while ( curr_poll_counter < MDSP_POLL_COUNTER_MAX )
  {
    if ( mdsp_read(MDSP_microMCCmd) == 0 )
    {
      break;
    }
    curr_poll_counter++;
  }
  if ( curr_poll_counter >= MDSP_POLL_COUNTER_MAX )
  {
    INTFREE_SAV(saved_int_mask);
    MSG_FATAL("MC didn't respond to suspend cmd", 0, 0, 0);
    mdsp_recover_from_dsp_failure();
    return;
  }

  clk_regime_switch_mdsp_clk();

  mdsp_write(MDSP_microMCCmd, MDSP_CMD_DSP_RESUME);
  mdsp_gen_interrupt(mdsp_modules_info[mdsp_current_image].mc_irq);

  /* Wait for the DSP to acknowledge the resume command */
  while ( curr_poll_counter < MDSP_POLL_COUNTER_MAX )
  {
    if ( mdsp_read(MDSP_microMCCmd) == 0 )
    {
      break;
    }
    curr_poll_counter++;
  }

  if ( curr_poll_counter >= MDSP_POLL_COUNTER_MAX )
  {
    MSG_FATAL("MC didn't respond to resume cmd", 0, 0, 0);
    mdsp_recover_from_dsp_failure();
  }

  clk_regime_complete_mdsp_clk_switch();

  INTFREE_SAV(saved_int_mask);

} /* end of mdsp_set_specific_mdsp_frequency */
#endif

