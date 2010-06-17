/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Q D S P   S E R V I C E S   M O D U L E
                         T A S K   F U N C T I O N S

GENERAL DESCRIPTION
  This task controls the Application QDSP embedded Digital Signal Processor.

EXTERNALIZED FUNCTIONS
  qdsp_task
    This is the entrance procedure for the QDSP Services Task.
    It contains the main processing loop for the task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright(c) 2000 - 2007 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/core/qdspservices/6k/ver1/qdspsvcs/main/latest/src/qdsptask.c#8 $ $DateTime: 2008/12/22 22:09:09 $ $Author: shilpaj $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/25/08   knm     Ported the missing code from depot to source.
09/16/08   knm     Added new API to collect valid ADSP dump prior to AMSS Reset
06/26/08   knm     Fixed Klockwork errors and warnings
03/26/08   skr     change for not disabling ADSP clk during Secondary boot 
                   load phase.
03/05/08    wd     Correct typo in if-test. 
02/19/08   at/wd   CR 137694 - avoid changing nested_enable flag to False
                   when qdsp_enable() is called twice in a row in succession. 
12/20/07   act     Increased timeout for application response to DISABLE event
                   to 15s so avoid erroring due to stalled tasks.
06/12/07   act     Modified qdsp_check_state to use local copy of state
                   variable for switch-case since the state variable may be
                   changed in interrupt context.
06/08/07    sg     Modified to support DCS clock switching interface.
03/30/07    az     Modified maximum download time and maximum BIOS config.
                   time to 100ms; increased delay to compensate for lower DME
                   clock rate.
01/23/07   act     Fixed bug where clients were passed with newly requested 
                   module to disable instead of already enabled module for
                   QDSP_MOD_DISABLE event.
01/22/07    ay     Modified maximum download time and maximum BIOS config.
                   time to 50ms; increased delay to compensate for lower DME
                   clock rate.
                   Updated copyright.
01/09/07    sg     Fixed compilation error.
01/08/07    sg     Fixed bug where clients were not been asked to disable
                   modules prior to enabling another module from the same image
                   that uses a different bitmask (CR108280).
11/17/06   act     Modified qdsp_clocks_enable() to save the ADSP clock and DME
                   setting being set by external 
                   qdsp_config_hw_resources_for_image() call.
                   Added debug messages to aid in debugging.
08/22/06    sg     Modified to not hard code the images that support DSP clock
                   switching. Instead we can now use the clk_switch_int
                   to determine if a given images supports switching.
08/14/06    hz     Added dynamic clock switch for incall MP3.
05/30/06    ay     Fixed incorrect clock regime mapping in
                   qdsp_config_dsp_clock_for_modules().
05/24/06    hz     Added fixes for CR#92522, DSP reset failed due to
                   qdsp_cumulative_modules_enabled_cache missed setting.
03/09/06    hz     Added support for evrc incall mp3.
02/07/06    hz     Added voc_mp3 images handler for in-call mp3, and voc_e90,
                   voc_evrc_e90 handlers.
11/29/05    sg     Fixed bug where the
                   qdsp_check_if_any_module_requires_image_reset() is using the
                   wrong index to access the reset preference array.
11/19/05    hz     In qdsp_check_if_any_module_requires_image_reset return
                   FALSE if reset_to_load of the image is NULL.
11/15/05    sg     Deleted unused function qdsp_get_mode().
                   Added support for reloading the firmware if a certain module
                   requires it.
11/06/05    sg     Modified clk apis to support dual mode rgm.
09/19/05    sg     Modified to support variable length bitmasks for BIOS
                   commands (up to 4 16bit words).
09/08/05    sg     Re-featureized to support clock rate switching for modules
                   with no SVD dependency.
05/16/05    sg     Added additional debug messages.
03/17/05    sg     Added support for setting clock rate and dme modes for
                   modules. Feature-ized bios control code.
11/19/04    sg     Merged module-based QDSP services from 00.00.13.01. Added
                   support for concurrent images.
09/07/04    sg     Modified to not clear qdsp_current_image while waiting for
                   responses to an IMAGE_RESET event.
08/26/04    sg     Modified to check if selected image is avaliable in the build
                   before attempting to enable/download.
01/28/04    sg     Modified code to always send event callback for Vocoder apps.
                   featurerized under FEATURE_FORCE_VOCODER_EVENTS
10/21/03    sg     Uses reset mechanism similar to MDSP services 00.00.06.01.01
                   to prevent problems with lost events and unnecessary
                   downloads after qdsp_reset() is called.
10/09/03    sg     Modified to not send a RESET event with QDSP_IMAGE_MAX.
06/12/03    sg     Fixed qdsp_clocks_enable() so that DME will re-enabled
                   correctly if the image didn't change after we come back from
                   'sleep' (when no images are enabled).
06/01/03    sg     Moved image specific hw config to qdspext.c.
02/12/03    sg     Added support for clock rate and DME configuration.
12/03/02    st     Modified code to return the number of applications currently
                   using the DSP with the IMAGE_READY event.
09/06/02    st     Added support for multimode architecture.
04/27/02    sg     Modified to allow the specification of zero interrupts in the
                   qdspext configuration file. Moved ISR definitions to
                   qdspext.c.
03/19/02    sg     Fixed qdsp_reset() mechanism so that it can be called
                   before/after qdsp_enable()/qdsp_disable(), and in any order.
02/21/02    sm     Added qdsp_reset() function.
01/15/02   ymc     Removed tmc_task_start.
           ymc     Updated copyright.
11/16/01    ro     Fix interrupt handlers.
10/30/01    ro     Use parser to define interrupts.
10/18/01   ymc     Added GSM KOVU support
10/10/01   ymc     Used VDSP_INT0 for T_KOVU.
05/17/01   ymc     Used tmc_task_start for T_MSM5200.
05/17/01   ymc     Added KOVU and T_MSM5200 support.
02/05/01   vlc     Added MSM5100 support.
11/17/00    ro     Eliminated calls to qdsp_num_images.
11/06/00    ro     Supports new image parser.
10/13/00   ymc     Added MSM5105 support; T_MSM33 as T_MSM33 && !T_MSM5105;
                   !T_MSM33 as !T_MSM33 || T_MSM5105.
05/10/00    ro     Add app_id to all events.
09/21/00    ro     Send QDSP_IMAGE_* & QDSP_MOD_* events only to enabled apps.
09/19/00    ro     Increase QDSP_NUM_PACKETS from 10 to 15.
09/11/00    ro     Start QDSP firmware after setting current_image in
                   qdsp_download_image so first FW interuppt is not missed.
08/28/00    ro     Unit integration.
08/17/00   spf     Initial revision.

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
#include "qdspext.h"            /* QDSP firmware images and access        */
#include "qdspi.h"              /* Internal QDSP Services definitions     */
#include <string.h>             /* Interface to string library            */
#include "task.h"               /* Task definitions and prototypes        */
#include "msmaud.h"             /* Target specific ADSP definitions       */
#include "qdspmem.h"            /* qdsp mem operation definitions         */

/* <EJECT> */

/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed to interface with the QDSP Services module.

===========================================================================*/

/* Generate the clock control bit mask for the given application. */
#define QDSP_APP_CLK_MASK(app_id) ( 1 << (app_id) )

#ifdef QDSP_MASTER_CONTROLLER_USED
/* Max. command buffer size of bios cmd */
#define QDSP_BIOS_MAX_CMD_PARAMS_SIZE    4

/* Download command */
#define QDSP_BIOS_MODULE_CONFIG_CMD      0x100

/* Vocoder clock switch command */
#define QDSP_VOC_CLOCK_SWITCH_CMD        0xf

/* DCS clock switch command */
#define QDSP_DCS_CLOCK_SWITCH_CMD        0xf

/* Downloader states */
#define QDSP_DOWNLOADER_STATE_READY      0
#define QDSP_DOWNLOADER_STATE_DWNLD_DONE 1

/* Polling interval for BIOS startup (ms) */
#define QDSP_BIOS_STARTUP_POLL_PERIOD    5

/* Maximum startup time for BIOS interface */
#define QDSP_MAX_BIOS_STARTUP_TIME       100

#endif /* QDSP_MASTER_CONTROLLER_USED */

/*-------------------------------------------------------------------------
** Timers
**-----------------------------------------------------------------------*/
LOCAL rex_timer_type   qdsp_rpt_timer;         /* Watchdog timer         */

/* This is the maximum time that QDSP Services will wait for all enabled
** applications to allow the image to be reset. The unit is milliseconds.
*/
#define QDSP_IMAGE_RESET_TIME  15000
LOCAL rex_timer_type   qdsp_image_reset_timer; /* Image reset timer      */

#ifdef QDSP_MASTER_CONTROLLER_USED
/* This is the maximum time that QDSP Services will wait, to allow the DSP
** to respond to a command. The unit is milliseconds.
*/
#define QDSP_DSP_CMD_TIMEOUT_VAL 100

LOCAL rex_timer_type   qdsp_dsp_cmd_rsp_timer; /* DSP Cmd. Rsp. Timer    */

/* Flag for each module will be set to TRUE if was enabled at least once since
 * the image was loaded.
 */
LOCAL boolean qdsp_cumulative_modules_enabled_cache[QDSP_MODULE_MAX+1];
#endif /* QDSP_MASTER_CONTROLLER_USED */

/*-------------------------------------------------------------------------
** State variables
**-----------------------------------------------------------------------*/

/* Current image loaded in the QDSP */
qdsp_image_type qdsp_current_image = QDSP_IMAGE_MAX;

/* Application registration table */
qdsp_app_table_type qdsp_app_table[QDSP_APP_MAX];

/* Mask to track application clock usage */
uint32 qdsp_clock_mask = 0;

/* Current state of QDSP Services. */
qdsp_services_state_type qdsp_services_current_state = QDSP_SERVICES_STATE_IDLE;

/* TRUE = there is a pending client request. */
LOCAL boolean qdsp_pending_app_request = FALSE;

/* QDSP mode */
LOCAL qdsp_mode_data_type* qdsp_mode_required = NULL;

LOCAL qdsp_mod_search_type qdsp_image_supp_data[QDSP_IMAGE_MAX];

/* Copy of the bitmask that was used to configure the ADSP modules. */
LOCAL qdsp_module_bitmask_type  qdsp_current_bitmask = 0;

#ifdef QDSP_SET_MODULE_HW_RESOURCES
LOCAL  qdsp_clock_rate_type qdsp_previous_clock_rate = QDSP_CLOCK_RATE_MAX;
LOCAL  clkrgm_dme_mode_type qdsp_previous_dme_mode   = CLKRGM_DME_NOT_REQUIRED;
#endif

boolean qdsp_int_lock_used = FALSE;
uint32  qdsp_saved_interrupt_mask_setting = 0;

/* List of modules to be enabled */
LOCAL boolean qdsp_mods_to_be_enabled[QDSP_MODULE_MAX+1];

/*-------------------------------------------------------------------------
** QDSP Services Task Command Queue, Free Queue and Available Packets.
**-----------------------------------------------------------------------*/
#define QDSP_NUM_PACKETS 15              /* # of free packets allocated  */
q_type           qdsp_cmd_q;             /* Request packets queue        */
q_type           qdsp_free_q;            /* Available packets queue      */
qdsp_packet_type qdsp_free_packet[QDSP_NUM_PACKETS];

/* <EJECT> */
/*===========================================================================

                             FUNCTION DEFINITIONS

===========================================================================*/

/* <EJECT> */
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
  qdsp_app_type            app_id, /* Application ID */
  qdsp_command_status_type status  /* Command status */
)
{
  qdsp_ev_cmd_type cmd_event;

  if ( app_id >= QDSP_APP_MAX )
  {
    /* Can't issue failure callback, since the app_id is invalid. */
    ERR_FATAL("Invalid QDSP AppId %d", app_id, 0, 0);
  }

  if ((qdsp_app_table[app_id].cb_ptr != NULL) &&
      ((qdsp_app_table[app_id].event_mask & QDSP_EV_COMMAND) != 0))
  {
    cmd_event.event      = QDSP_EV_COMMAND;
    cmd_event.app_id     = app_id;
    cmd_event.cmd_status = status;

    (qdsp_app_table[app_id].cb_ptr)((qdsp_event_data_type*)&cmd_event);
  }

} /* qdsp_send_cmd_event */

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_send_module_event

DESCRIPTION
  Sends a QDSP_EV_MODULE event to the specified application.

DEPENDENCIES
  qdsp_app_table

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void qdsp_send_module_event (
  qdsp_app_type app_id,        /* Application ID */
  qdsp_mod_status_type status, /* Module status */
  void *data                   /* Other event specific info */
)
{
  qdsp_ev_mod_type mod_event;

  if ( app_id >= QDSP_APP_MAX )
  {
    /* Can't issue failure callback, since the app_id is invalid. */
    ERR_FATAL("Invalid QDSP AppId %d", app_id, 0, 0);
  }

  /* Populate any related data before sending the Event. */
  if ((qdsp_app_table[app_id].cb_ptr != NULL) &&
      ((qdsp_app_table[app_id].event_mask & QDSP_EV_MODULE) != 0))
  {
    mod_event.event      = QDSP_EV_MODULE;
    mod_event.app_id     = app_id;
    mod_event.mod_status = status;

    switch (status)
    {
      case QDSP_MOD_READY:
        mod_event.info.ready.image = *(qdsp_image_type*)data;
        mod_event.module = qdsp_app_table[app_id].module_enabled;
        MSG_HIGH("MOD_READY app=%d mod=%d img=%d", app_id, mod_event.module,
                 mod_event.info.ready.image);
        break;

      case QDSP_MOD_DISABLE:
        mod_event.module = qdsp_app_table[app_id].module_enabled;
        MSG_HIGH("MOD_DISABLE app=%d mod=%d", app_id, mod_event.module, 0);
        break;

      default:
        ERR_FATAL("Illegal status type", 0, 0, 0);
        return;
    }
    (qdsp_app_table[app_id].cb_ptr)((qdsp_event_data_type*)&mod_event);
  }

} /* qdsp_send_module_event */

#if defined(QDSP_MASTER_CONTROLLER_USED) || \
    defined(FEATURE_QDSP_MODULE_RESOURCE_MGMT)
/*===========================================================================

FUNCTION qdsp_fw_command

DESCRIPTION
  Issues commands to the BIOS command interfaces

DEPENDENCIES
  The current image must support the BIOS interface and the ADSP clock must be
  enabled.

RETURN VALUE
  TRUE if the command was issued; FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean qdsp_fw_command(
  const    uint16 command_name,
           uint16 *cmd_params_buf,
           uint16 cmd_params_length,
  volatile uint16 *dsp_cmd_reg,
  volatile uint16 *dsp_param_buf
)
{
  boolean  status = FALSE;
  uint16   semaphore;
  uint16   cnt = 0;

  INTLOCK();
  semaphore = qdsp_read(dsp_cmd_reg);
  if (semaphore == 0)
  {
    if (dsp_param_buf != NULL)
    {
      cmd_params_length++;
      /* Copy the params */
      while (--cmd_params_length)
      {
        MSG_HIGH("FW cmd buf[%d] = %d", cnt, *cmd_params_buf, 0);
        *dsp_param_buf++ = *cmd_params_buf++;
        cnt++;
      }
    }

    qdsp_write(dsp_cmd_reg, command_name);

    /* To generate microprocessor interrupt */
    qdsp_up_to_dsp_irq();

    MSG_HIGH("Issued FW reg=0x%x, cmd=0x%x", dsp_cmd_reg, command_name, 0);
    status = TRUE;
  }
  else
  {
    MSG_HIGH("ADSP BIOS Command buffer not ready %d", semaphore, 0, 0);
  }
  INTFREE();
  return status;
}
#endif /* QDSP_MASTER_CONTROLLER_USED || FEATURE_QDSP_MODULE_RESOURCE_MGMT */

#ifdef FEATURE_QDSP_SET_IMAGE_HW_RESOURCES
extern clkrgm_qdsp_clk_frequency_type qdsp_clock_rate_to_freq[];
#endif

#ifdef FEATURE_QDSP_MODULE_RESOURCE_MGMT
/*===========================================================================

FUNCTION QDSP_SWITCH_DSP_CLOCK

DESCRIPTION
  Interacts with the firmware to switch the ADSP clock rate.

DEPENDENCIES
  qdsp_app_table
  qdsp_previous_clock_rate

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void qdsp_switch_dsp_clock(
  volatile uint16  *int_ptr,
           boolean in_reset,
           uint16  cmd_id
)
{
  INTLOCK();

    #if defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING) || \
        defined(FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING)
    /* Bump up the ARM clock so that we can switch the DSP clock quickly */
    clk_regime_register_for_cpu_resource(CLKRGM_QDSP_SERVICES);
    #endif

    clk_regime_set_qdsp_clk_frequency(
      qdsp_clock_rate_to_freq[qdsp_previous_clock_rate]);

    if (!in_reset)
    {
      /* Wait for the command register to become ready */
      while ( qdsp_read(int_ptr) != 0 );

      /* Tell the DSP that we want to switch rates */
      if (!qdsp_fw_command(cmd_id, NULL, 0, int_ptr, NULL))
      {
        qdsp_fatal_error_core_dump();
        ERR_FATAL("ADSP Clock switching fw interface not responding!",0,0,0);
      }

      /* Wait for it to accept the command */
      while ( qdsp_read(int_ptr) != 0 );
     }

    /* Enable the new clock setting */
    clk_regime_switch_qdsp_clk();
    
    if (!in_reset)
    {
      /* Tell the DSP to resume */
      if (!qdsp_fw_command(cmd_id, NULL, 0, int_ptr, NULL))
      {
        qdsp_fatal_error_core_dump();
        ERR_FATAL("ADSP Clock switching fw interface not responding!",0,0,0);
      }
      while ( qdsp_read(int_ptr) != 0 );
    }

    /* Tell clk_rgm to complete house-keeping operations */
    clk_regime_complete_qdsp_clk_switch();

    #if defined(FEATURE_DYNAMIC_CLOCK_FREQUENCY_CHANGING) || \
        defined(FEATURE_AUTOMATIC_CLOCK_FREQUENCY_CHANGING)
    /* Allow ARM clock rate to be reduced */
    clk_regime_deregister_for_cpu_resource(CLKRGM_QDSP_SERVICES);
    #endif

  INTFREE();
}
#endif /* FEATURE_QDSP_MODULE_RESOURCE_MGMT */

/*===========================================================================

FUNCTION QDSP_CONFIG_DSP_CLOCK_FOR_MODULES

DESCRIPTION
  Sets the clock rate for the currently enabled modules

DEPENDENCIES
  qdsp_current_image
  qdsp_previous_image
  qdsp_reset_image

RETURN VALUE
  None

SIDE EFFECTS
  qdsp_current_image
  qdsp_reset_image

===========================================================================*/
void qdsp_config_dsp_clock_for_modules (boolean in_reset)
{
#if defined(FEATURE_QDSP_MODULE_RESOURCE_MGMT) &&  \
    (defined(QDSP_CLOCK_SWITCH_INT_DCS_DEFINED) || \
     defined(QDSP_CLOCK_SWITCH_INT_VOC_DEFINED))

  qdsp_clock_rate_type new_clock_rate = (qdsp_clock_rate_type)0;
  qdsp_app_type        app_id;
  uint16               mod_index;
  qdsp_clock_rate_type *clock_rates_ptr = (qdsp_clock_rate_type*)
                            qdsp_modules_info[qdsp_current_image].clock_rates;
  qdsp_module_type *modules_supported = (qdsp_module_type*)
                            qdsp_modules_info[qdsp_current_image].modules;

  MSG_HIGH("config_dsp_clock_for_modules:%d,0%x",in_reset,qdsp_clock_mask,0);

  if (!qdsp_clock_mask)
  {
    return;
  }

  /* Determine the highest clock rate required */
  if (clock_rates_ptr != NULL &&
      (
       #ifdef QDSP_CLOCK_SWITCH_INT_DCS_DEFINED
       (qdsp_modules_info[qdsp_current_image].clk_switch_int ==
                                       QDSP_CLOCK_SWITCH_INT_DCS)
       #endif /* QDSP_CLOCK_SWITCH_INT_DCS_DEFINED */
       #if defined(QDSP_CLOCK_SWITCH_INT_DCS_DEFINED) && \
           defined(QDSP_CLOCK_SWITCH_INT_VOC_DEFINED)
           ||
       #endif
       #ifdef QDSP_CLOCK_SWITCH_INT_VOC_DEFINED
       (qdsp_modules_info[qdsp_current_image].clk_switch_int ==
                                       QDSP_CLOCK_SWITCH_INT_VOC)
       #endif /* QDSP_CLOCK_SWITCH_INT_VOC_DEFINED */
       )
      )
  {
    for (app_id = (qdsp_app_type)0; app_id < QDSP_APP_MAX; app_id++)
    {
      if (((qdsp_app_table[app_id].state == QDSP_APP_STATE_ENABLING ||
            qdsp_app_table[app_id].state == QDSP_APP_STATE_ENABLE_REQUESTED) &&
           qdsp_mods_to_be_enabled[qdsp_app_table[app_id].module_requested]
                                                                    == TRUE) ||
          qdsp_app_table[app_id].state == QDSP_APP_STATE_ENABLED)
      {
        mod_index = 0;
        MSG_HIGH("app:%d, module:%d",
                 app_id,qdsp_app_table[app_id].module_requested,0);
        while (modules_supported[mod_index] < QDSP_MODULE_MAX)
        {
          if (modules_supported[mod_index] ==
              qdsp_app_table[app_id].module_requested)
          {
            MSG_HIGH("app:%d, module:%d, clk:%d", app_id,
                     qdsp_app_table[app_id].module_requested,
                     clock_rates_ptr[mod_index]);
            if (clock_rates_ptr[mod_index] > new_clock_rate)
            {
              new_clock_rate = clock_rates_ptr[mod_index];
            }
            break;
          }
          mod_index++;
        }
      }
    }

    /* Switch clock rate if necessary */
    if (qdsp_previous_clock_rate != new_clock_rate)
    {
      volatile uint16 *int_ptr = NULL;
      uint16 cmd_id = 0;

      MSG_HIGH("Setting ADSP clk: %d",new_clock_rate,0,0);
      qdsp_previous_clock_rate = new_clock_rate;

      #ifdef QDSP_CLOCK_SWITCH_INT_DCS_DEFINED
      if (qdsp_modules_info[qdsp_current_image].clk_switch_int ==
                                                    QDSP_CLOCK_SWITCH_INT_DCS)
      {
        int_ptr = qdsp_address(QDSP_dcsCommandReg, 0);
        cmd_id  = QDSP_DCS_CLOCK_SWITCH_CMD;
      }
      #endif /* QDSP_CLOCK_SWITCH_INT_DCS_DEFINED */

      #ifdef QDSP_CLOCK_SWITCH_INT_VOC_DEFINED
      if (qdsp_modules_info[qdsp_current_image].clk_switch_int ==
                                                    QDSP_CLOCK_SWITCH_INT_VOC)
      {
        int_ptr = qdsp_address(QDSP_uPCommandReg, 0);
        cmd_id  = QDSP_VOC_CLOCK_SWITCH_CMD;
      }
      #endif /* QDSP_CLOCK_SWITCH_INT_VOC_DEFINED */

      if (int_ptr != NULL)
      {
        qdsp_switch_dsp_clock(int_ptr, in_reset, cmd_id);
      }
    }
  }
#endif /* FEATURE_QDSP_MODULE_RESOURCE_MGMT */
}

/*===========================================================================

FUNCTION qdsp_config_dme_mode_for_modules

DESCRIPTION
  Enables DME hardware if at least one module requires DME. Disables otherwise.

DEPENDENCIES
  qdsp_current_image

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void qdsp_config_dme_mode_for_modules(void)
{
#ifdef QDSP_SET_MODULE_HW_RESOURCES
  qdsp_module_type     module;
  clkrgm_dme_mode_type new_dme_mode = CLKRGM_DME_NOT_REQUIRED;
  clkrgm_dme_mode_type curr_module_mode;

  /* Determine if any module requires DME */
  for (module=(qdsp_module_type)0; module<QDSP_MODULE_MAX; module++)
  {
    if (qdsp_mods_to_be_enabled[module] == TRUE)
    {
      curr_module_mode = qdsp_get_module_dme_mode(qdsp_current_image, module);
      if (curr_module_mode == CLKRGM_DME_REQUIRED)
      {
        new_dme_mode = CLKRGM_DME_REQUIRED;
        break;
      }
    }
  }
  INTLOCK();
  if (qdsp_previous_dme_mode != new_dme_mode)
  {
    qdsp_previous_dme_mode = new_dme_mode;
    clk_regime_set_qdsp_dme_mode(new_dme_mode);
  }
  INTFREE();
#endif /* QDSP_SET_MODULE_HW_RESOURCES */
}

/*===========================================================================

FUNCTION QDSP_DOWNLOAD_HIGH_IMAGE

DESCRIPTION
  Downloads the specified firmware image, if not already downloaded.

DEPENDENCIES
  qdsp_current_image
  qdsp_previous_image

RETURN VALUE
  None

SIDE EFFECTS
  qdsp_current_image

===========================================================================*/
void qdsp_download_dsp_image(
  qdsp_image_type pending_image,
  boolean         force_reset
)
{
  /*  Download the new image, if necessary
  */
  MSG_HIGH("download_dsp_image:%d %d",pending_image,force_reset,0);
  if ( qdsp_current_image != pending_image ||
       force_reset == TRUE )
  {
    qdsp_current_bitmask = 0;

#ifdef QDSP_MASTER_CONTROLLER_USED
    (void) memset(qdsp_cumulative_modules_enabled_cache, FALSE,
                  sizeof(qdsp_cumulative_modules_enabled_cache));
#endif /* QDSP_MASTER_CONTROLLER_USED */

    /* Download the firmware image to the QDSP */
    if (qdsp_download_image(pending_image) == QDSP_DOWNLOAD_SUCCESS)
    {
      /* Update the current image.
      */
      qdsp_current_image = pending_image;

#if defined(QDSP_SET_MODULE_HW_RESOURCES)
      qdsp_previous_clock_rate = qdsp_get_image_clock_rate(pending_image);
      MSG_HIGH("previous_clock_rate: %d",qdsp_previous_clock_rate,0,0);
      MSG_HIGH("download_dsp_image calling config_dsp_clock_for_modules",0,0,0);
      qdsp_config_dsp_clock_for_modules(TRUE);

      qdsp_previous_dme_mode = qdsp_get_image_dme_mode(pending_image);
      qdsp_config_dme_mode_for_modules();
#endif

#ifdef QDSP_MASTER_CONTROLLER_USED
      if (qdsp_modules_info[qdsp_current_image].bitmasks == NULL)
#endif
      {
        /* Lock interrupts to guarantee that MOD_READY events are sent out
           before any ADSP interrupts are issued to the clients */
        qdsp_int_lock_used = TRUE;
        INTLOCK_SAV(qdsp_saved_interrupt_mask_setting);
      }

      /* Start the QDSP firmware.
      */
      qdsp_start_image();

      MSG_HIGH("QDSP image loaded = %d", qdsp_current_image,0,0);

#ifdef QDSP_MASTER_CONTROLLER_USED
      /* Allow the QDSP downloader to initialize. */
      if (qdsp_modules_info[qdsp_current_image].bitmasks != NULL)
      {
        uint32 curr_poll_ms = 0;
        while (curr_poll_ms < QDSP_MAX_BIOS_STARTUP_TIME)
        {
          rex_sleep(QDSP_BIOS_STARTUP_POLL_PERIOD);
          if (qdsp_read(QDSP_biosStateReg) == QDSP_DOWNLOADER_STATE_READY)
          {
            break;
          }
          curr_poll_ms += QDSP_BIOS_STARTUP_POLL_PERIOD;
        }

        if (curr_poll_ms >= QDSP_MAX_BIOS_STARTUP_TIME)
        {
          qdsp_fatal_error_core_dump();
          ERR_FATAL("QDSP downloader did not initialize!", 0, 0, 0);
        }
      }
#endif
    }
    else
    {
      qdsp_fatal_error_core_dump();
      ERR_FATAL("Download Failed: Image = %d", qdsp_current_image, 0, 0);
    }
  }
  else {
    /* required image was already downloaded. */
#if defined(QDSP_SET_MODULE_HW_RESOURCES)
      MSG_HIGH("download_dsp_image calling config_dsp_clock_for_modules",0,0,0);
      qdsp_config_dsp_clock_for_modules(FALSE);
#endif
  }

  return;
} /* qdsp_download_high_image */

/*===========================================================================

FUNCTION qdsp_image_supports_module

DESCRIPTION
  Checks if the specified module can be supported by the given image.

DEPENDENCIES
  qdsp_modules_info

RETURN VALUE
  TRUE if the module can be supported, false otherwise.

SIDE EFFECTS
  None

===========================================================================*/
boolean qdsp_image_supports_module(
  qdsp_image_type  image, /* Image to check against */
  qdsp_module_type module /* Module required */
)
{
  qdsp_module_type *modules_supported;
  byte             mod_index = 0;

  if (image < QDSP_IMAGE_MAX && qdsp_is_supported_image(image))
  {
    modules_supported = (qdsp_module_type*)qdsp_modules_info[image].modules;

    if (modules_supported != NULL)
    {
      while (modules_supported[mod_index] != QDSP_MODULE_MAX)
      {
        if (modules_supported[mod_index] == module)
        {
          return TRUE;
        }
        mod_index++;
      }
    }
  }
  return FALSE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_module_currently_available

DESCRIPTION
  Checks if the specified module is supported by the current image.

DEPENDENCIES


RETURN VALUE
  TRUE if supported currently, FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/
boolean qdsp_module_currently_available (
  qdsp_module_type module
)
{
  boolean status = FALSE;

  if (module < QDSP_MODULE_MAX) {
    return qdsp_image_supports_module(qdsp_current_image, module);
  }
  return status;
}

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_inform_enabled_apps

DESCRIPTION
  Sends a QDSP_MOD_READY event to all the newly enabled applications.

DEPENDENCIES
  qdsp_app_table

RETURN VALUE
  None

SIDE EFFECTS
  Updates the state of the enabled apps.

===========================================================================*/
void qdsp_inform_enabled_apps(boolean *can_enable_mods)
{
  qdsp_app_type app_id;

  for (app_id=(qdsp_app_type)0; app_id<QDSP_APP_MAX; app_id++)
  {
    if ((qdsp_app_table[app_id].state == QDSP_APP_STATE_ENABLING ||
         qdsp_app_table[app_id].state == QDSP_APP_STATE_ENABLE_REQUESTED) &&
        (qdsp_app_table[app_id].module_requested < QDSP_MODULE_MAX &&
        can_enable_mods[ qdsp_app_table[app_id].module_requested ] == TRUE))
    {
      qdsp_app_table[app_id].state = QDSP_APP_STATE_ENABLED;
      qdsp_app_table[app_id].module_enabled =
                                      qdsp_app_table[app_id].module_requested;
      qdsp_app_table[app_id].nested_enable = FALSE;
      qdsp_app_table[app_id].reset = FALSE;
      qdsp_send_module_event(app_id, QDSP_MOD_READY, &qdsp_current_image);
    }
  }
  qdsp_services_current_state = QDSP_SERVICES_STATE_IDLE;
}

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_inform_apps_that_image_changed

DESCRIPTION
  Sends a QDSP_IMAGE_CHANGE event to all the clients who have registered for
  IMAGE_CHANGE events.

DEPENDENCIES
  qdsp_app_table

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void qdsp_inform_apps_that_image_changed(
  qdsp_image_type next_image,
  boolean         *can_enable_mods
)
{
  qdsp_app_type app_id;
  qdsp_ev_image_change_type img_event;
  uint32                    num_apps = 0;

  if (next_image < QDSP_IMAGE_MAX)
  {
    img_event.event = QDSP_EV_IMAGE_CHANGE;
    img_event.image = next_image;

    for (app_id=(qdsp_app_type)0; app_id<QDSP_APP_MAX; app_id++)
    {
      if (((qdsp_app_table[app_id].event_mask & QDSP_EV_IMAGE_CHANGE) != 0) &&
          (qdsp_app_table[app_id].cb_ptr != NULL))
      {
        MSG_MED("Sent IMAGE CHG for app=%d",app_id,0,0);
        img_event.app_id = app_id;
        img_event.num_higher_priority_apps = num_apps;
        (qdsp_app_table[app_id].cb_ptr)((qdsp_event_data_type*)&img_event);
      }

      if (((qdsp_app_table[app_id].state == QDSP_APP_STATE_ENABLING ||
            qdsp_app_table[app_id].state == QDSP_APP_STATE_ENABLE_REQUESTED) &&
           can_enable_mods[ qdsp_app_table[app_id].module_requested ] == TRUE)||
          qdsp_app_table[app_id].state == QDSP_APP_STATE_ENABLED )
      {
        num_apps++;
      }
    }
  }
}

/*===========================================================================

FUNCTION qdsp_disable_app

DESCRIPTION
  Informs the specified app. that it will be disabled.

DEPENDENCIES
  qdsp_app_table

RETURN VALUE
  None.

SIDE EFFECTS
  Updates the state of the application.

===========================================================================*/
void qdsp_disable_app(
  qdsp_app_type app
)
{
  if ( app >= QDSP_APP_MAX )
  {
    /* Can't issue failure callback, since the app_id is invalid. */
    ERR_FATAL("Invalid QDSP AppId %d", app, 0, 0);
  }

  if (qdsp_app_table[app].state == QDSP_APP_STATE_ENABLED ||
      qdsp_app_table[app].nested_enable == TRUE)
  {
    qdsp_app_table[app].nested_enable = FALSE;
    qdsp_app_table[app].state = QDSP_APP_STATE_PREEMPTING;
    qdsp_send_module_event(app, QDSP_MOD_DISABLE, NULL);
  }
}

/*===========================================================================

FUNCTION qdsp_all_acks_recvd

DESCRIPTION
  Checks if there are any application that have not yet responded to a
  MOD_DISABLE event.

DEPENDENCIES
  qdsp_app_table

RETURN VALUE
  FALSE if at least one application has not yet responded.
  TRUE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean qdsp_all_acks_recvd(
  qdsp_app_type *offending_app /* Application that did not respond */
)
{
  boolean             all_acks_received = TRUE;
  qdsp_app_type app;
  qdsp_app_table_type *app_ptr;

  *offending_app = QDSP_APP_MAX;

  for (app = (qdsp_app_type)0; app < QDSP_APP_MAX; app++)
  {
    app_ptr = &qdsp_app_table[app];

    if (app_ptr->state == QDSP_APP_STATE_PREEMPTING)
    {
      all_acks_received = FALSE;
      *offending_app = app;
      break;
    }
  }
  return all_acks_received;
}

/*===========================================================================

FUNCTION qdsp_modules_supported_by_image

DESCRIPTION
  Determines which of the required modules can be supported by the
  specified image and also ensures that the highest priority module
  is present in the selected mode.

DEPENDENCIES
  qdsp_app_table

RETURN VALUE
  TRUE if at least one module is supported
  FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean qdsp_modules_supported_by_image(
  qdsp_module_type      high_pri_mod,
  qdsp_image_type       image,
  boolean               *mods_to_be_enabled,
  qdsp_mod_search_type  *search_info
)
{
  boolean ret_val = FALSE;
  boolean mode_has_high_pri_mod;
  uint16  high_count = 0;
  uint16  enable_count = 0;
  byte    num_modes;
  byte    mode_index = 0;
  byte    most_mods_supported_by = 0;
  qdsp_module_type *mode;
  qdsp_module_type module_index;


  if (qdsp_is_supported_image(image) &&
      qdsp_image_supports_module(image, high_pri_mod) == TRUE)
  {
    num_modes = qdsp_modules_info[image].mode_info.num_modes;

    while (mode_index < num_modes)
	{
      mode = (qdsp_module_type*)
                    qdsp_modules_info[image].mode_info.modes[mode_index].mode;

      module_index = 0;
      enable_count = 0;
      mode_has_high_pri_mod = FALSE;

      //Checks whether the mode has highest priority module or not
      while (mode[module_index] < QDSP_MODULE_MAX)
      {
        if(mode[module_index] == high_pri_mod)
        {
          mode_has_high_pri_mod = TRUE;
          break;
        }
        module_index++;
      }

      if(mode_has_high_pri_mod)
      {
        module_index = 0;//resetting the index

        while (mode[module_index] < QDSP_MODULE_MAX)
		{
          if (mods_to_be_enabled[ mode[module_index] ] == TRUE)
          {
            enable_count++;
          }
          module_index++;
		}

        if (enable_count > high_count)
		{
          high_count = enable_count;
          most_mods_supported_by = mode_index;
		}
      }

      mode_index++;

    }

    if (high_count)
    {
      search_info->count = high_count;
      search_info->image = image;
      search_info->mode = (qdsp_module_type*)
         qdsp_modules_info[image].mode_info.modes[most_mods_supported_by].mode;
      ret_val = TRUE;
    }
  }
  return ret_val;
}

#ifdef QDSP_MASTER_CONTROLLER_USED
/*===========================================================================

FUNCTION qdsp_get_bitmask_for_modules

DESCRIPTION
  Gets the bitmask required to enable the currently selected modules.

DEPENDENCIES
  qdsp_mods_to_be_enabled

RETURN VALUE
  bitmask

SIDE EFFECTS
  None

===========================================================================*/
qdsp_module_bitmask_type qdsp_get_bitmask_for_modules(
  const qdsp_image_type image
)
{
  qdsp_module_bitmask_type  bitmask = 0;
  qdsp_module_bitmask_type  *bitmasks_ptr;
  qdsp_module_type          module;

  bitmasks_ptr = (qdsp_module_bitmask_type*)qdsp_modules_info[image].bitmasks;

  if (bitmasks_ptr != NULL)
  {
    /* Form the bitmask that will be used to enable/disable the modules. */
    for (module=(qdsp_module_type)0; module<QDSP_MODULE_MAX; module++)
    {
      if (qdsp_mods_to_be_enabled[module] == TRUE)
      {
        /* Whenever a module triggeres BIOS command, the corresponding cache
         * entry has to be set */
        qdsp_cumulative_modules_enabled_cache[module] = TRUE;

        bitmask |= qdsp_get_module_bitmask(image, module);
      }
    }
  }
  return bitmask;
}
#endif /* QDSP_MASTER_CONTROLLER_USED */

/*===========================================================================

FUNCTION qdsp_start_modules_on_dsp

DESCRIPTION
  Enables/disables the specified modules on the ADSP.

DEPENDENCIES
  qdsp_services_current_state
  qdsp_current_bitmask
  qdsp_current_image

RETURN VALUE
  TRUE if modules are ready for immediate use.
  FALSE if the caller must wait for the configuration operation to complete
    on the DSP.

SIDE EFFECTS
  qdsp_services_current_state
  qdsp_current_bitmask

===========================================================================*/
boolean qdsp_start_modules_on_dsp()
{
  boolean                   modules_ready = FALSE;
#ifdef QDSP_MASTER_CONTROLLER_USED
  qdsp_module_bitmask_type  *bitmasks_ptr;
  qdsp_module_bitmask_type  bitmask = 0;
  uint16                    dwnld_params[QDSP_BIOS_MAX_CMD_PARAMS_SIZE];
  uint8                     bmi;
#endif /* QDSP_MASTER_CONTROLLER_USED */

  MSG_HIGH("start_modules_on_dsp",0,0,0);

  if (qdsp_is_supported_image(qdsp_current_image))
  {
#ifdef QDSP_MASTER_CONTROLLER_USED
    bitmasks_ptr = (qdsp_module_bitmask_type*)
                  qdsp_modules_info[qdsp_current_image].bitmasks;

    /* Issue the DSP command to enable/disable modules if the image supports
     * the downloader interface. */
    if (bitmasks_ptr != NULL)
    {
      bitmask = qdsp_get_bitmask_for_modules(qdsp_current_image);

      /* Set the new bitmask only if the image or the bitmask has changed. */
      if (bitmask && qdsp_current_bitmask != bitmask)
      {
        /* Turn on the clocks before we start issuing any commands */
        
        MSG_HIGH("start_modules_on_dsp calling clocks_enable",0,0,0);
        qdsp_clocks_enable(QDSP_APP_SELF);
        MSG_HIGH("start_modules_on_dsp calling config_dsp_clock_for_modules",
                 0,0,0);
        qdsp_config_dsp_clock_for_modules(FALSE);
        qdsp_config_dme_mode_for_modules();

        INTLOCK();
        /* Check if the DSP is ready to receive the download command. */
        if (!qdsp_current_bitmask &&
            qdsp_read(QDSP_biosStateReg) != QDSP_DOWNLOADER_STATE_READY)
        {
          ERR_FATAL("DSP downloader is not in ready state",0,0,0);
        }
        qdsp_services_current_state =
                            QDSP_SERVICES_STATE_WAITING_FOR_MODULE_SWITCH;
        qdsp_current_bitmask = bitmask;

        for (bmi = 0;
             bmi < qdsp_modules_info[qdsp_current_image].bitmask_size; bmi++)
        {
           dwnld_params[bmi] = (uint16)(bitmask >> (16 *
              (qdsp_modules_info[qdsp_current_image].bitmask_size - bmi - 1)));
        }

        (void) rex_set_timer(&qdsp_dsp_cmd_rsp_timer, QDSP_DSP_CMD_TIMEOUT_VAL);

        /* Issue the command */
        if (!qdsp_fw_command(
               QDSP_BIOS_MODULE_CONFIG_CMD,
               dwnld_params,
               qdsp_modules_info[qdsp_current_image].bitmask_size,
               qdsp_address(QDSP_biosuPCommandReg, 0),
               qdsp_address(QDSP_biosuPCommandBuf, 0)))
        {
          qdsp_fatal_error_core_dump();
          ERR_FATAL("Download command failed! bitmask=%x", bitmask, 0, 0);
        }
        INTFREE();
      }
      else
      {
        modules_ready = TRUE;
        qdsp_services_current_state =
                                QDSP_SERVICES_STATE_MODULE_SWITCH_SUCCESSFUL;
      }
    }
    else
#endif /* QDSP_MASTER_CONTROLLER_USED */
    {
      MSG_HIGH("start_modules_on_dsp calling config_dsp_clock_for_modules",
               0,0,0);
      qdsp_config_dsp_clock_for_modules(FALSE);
      qdsp_config_dme_mode_for_modules();
      modules_ready = TRUE;
      qdsp_services_current_state =
                                QDSP_SERVICES_STATE_MODULE_SWITCH_SUCCESSFUL;
    }
  }
  else
  {
    ERR_FATAL("Invalid image type", 0, 0, 0);
  }
  return modules_ready;
}

#ifdef QDSP_MASTER_CONTROLLER_USED
/*===========================================================================

FUNCTION qdsp_check_if_any_module_requires_image_reset

DESCRIPTION
  Checks if any of the modules selected for enabling requires the firmware to
  be reloaded before it can be enabled.

DEPENDENCIES
  qdsp_modules_info
  qdsp_app_table
  qdsp_cumulative_modules_enabled_cache

  This function assumes that the modules in all the data structures in
  qdsp_modules_info always appear in ascending order (by value).

RETURN VALUE
  TRUE if the firmware must be reloaded

SIDE EFFECTS
  None.

===========================================================================*/
boolean qdsp_check_if_any_module_requires_image_reset (
  const qdsp_image_type image
)
{
  qdsp_module_type module;
  qdsp_module_type *modules_supported;
  uint32           index = 0;
  boolean          reset_dsp = FALSE;
  boolean          some_modules_already_enabled = FALSE;
  boolean          at_least_one_new_mod_requires_reset = FALSE;

  modules_supported = (qdsp_module_type*)qdsp_modules_info[image].modules;

  if (modules_supported == NULL)
  {
    ERR_FATAL("Null pointer!", 0, 0, 0);
  }

  if (qdsp_modules_info[image].reset_to_load == NULL)
  {
    return reset_dsp;
  }

  while (index < QDSP_MODULE_MAX &&
         modules_supported[index] < QDSP_MODULE_MAX)
  {
    module = modules_supported[index];

    if (qdsp_mods_to_be_enabled[module] &&
        qdsp_modules_info[image].reset_to_load[index] &&
        qdsp_cumulative_modules_enabled_cache[module] == FALSE)
    {
      at_least_one_new_mod_requires_reset = TRUE;
    }

    if (qdsp_cumulative_modules_enabled_cache[module])
    {
      some_modules_already_enabled = TRUE;
    }

    index++;
  }

  if (at_least_one_new_mod_requires_reset && some_modules_already_enabled)
  {
    reset_dsp = TRUE;
  }

  return reset_dsp;
}
#endif /* QDSP_MASTER_CONTROLLER_USED */

/*===========================================================================

FUNCTION qdsp_download_and_enable_apps

DESCRIPTION
  Downloads the indicated image if necessary and enables modules that have
  been selected in qdsp_mods_to_be_enabled.

DEPENDENCIES
  qdsp_mods_to_be_enabled
  qdsp_app_table

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/
void qdsp_download_and_enable_apps (
  qdsp_image_type   image_required,
  boolean           reset_dsp
)
{
  qdsp_download_dsp_image(image_required, reset_dsp);

  qdsp_inform_apps_that_image_changed(image_required,
                                      qdsp_mods_to_be_enabled);

  if (qdsp_start_modules_on_dsp() == TRUE)
  {
    MSG_HIGH("enable_modules calling clocks_disable for SELF",0,0,0);
    qdsp_clocks_disable(QDSP_APP_SELF);
    qdsp_inform_enabled_apps(qdsp_mods_to_be_enabled);
  }
  /* else wait for the modules to start on the DSP. */

  if (qdsp_int_lock_used)
  {
    qdsp_int_lock_used = FALSE;
    INTFREE_SAV(qdsp_saved_interrupt_mask_setting);
  }
}

/*===========================================================================

FUNCTION qdsp_enable_modules

DESCRIPTION
  Enables all modules that can be currently supported.

DEPENDENCIES
  qdsp_app_table

  This function assumes that the modules in all the data structures in
  qdsp_modules_info always appear in ascending order (by value).

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void qdsp_enable_modules(void)
{
  qdsp_app_type     app;
  qdsp_module_type  module;
  qdsp_app_type     high_pri_app = QDSP_APP_MAX;
  byte              total_enable_count = 0;
  qdsp_image_type   image_required = QDSP_IMAGE_MAX;
  qdsp_image_type   start_image;
  qdsp_image_type   next_image;
  byte              num_images_can_be_used = 0;
  byte              most_modules_supported_by = 0;
  qdsp_app_type     not_ready_app_id = QDSP_APP_MAX;
  boolean           force_preemption = FALSE;
  boolean           reset_dsp = FALSE;

  /* Select new set of modules only if IDLE */
  if (qdsp_services_current_state != QDSP_SERVICES_STATE_IDLE)
  {
    return;
  }
  qdsp_pending_app_request = FALSE;

  /* Reset the list of modules to enable */
  for (module=(qdsp_module_type)0; module<QDSP_MODULE_MAX; module++)
  {
    qdsp_mods_to_be_enabled[module] = FALSE;
  }

  /* Determine which modules need to be enabled */
  for (app = (qdsp_app_type)0; app < QDSP_APP_MAX; app++)
  {
    switch (qdsp_app_table[app].state)
    {
      case QDSP_APP_STATE_PREEMPTING:
        /* Should never get here if we are not busy */
        ERR_FATAL("Internal state error", 0, 0, 0);
        return;

      case QDSP_APP_STATE_ENABLING:
      case QDSP_APP_STATE_ENABLE_REQUESTED:
      case QDSP_APP_STATE_ENABLED:
        MSG_HIGH("app:%d, mod:%d",app,qdsp_app_table[app].module_requested,0);
        if (high_pri_app == QDSP_APP_MAX)
        {
          /* Make a note of which is the highest priority app */
          high_pri_app = app;
        }
        total_enable_count++;
        qdsp_mods_to_be_enabled[qdsp_app_table[app].module_requested] = TRUE;
        break;

      default:
        ERR_FATAL("Invalid state for App %d", app, 0, 0);
        /* Fall through */

      case QDSP_APP_STATE_DISABLED:
        break;
    }
  }

  MSG_MED("total enable count: %d",total_enable_count,0,0);

  if (total_enable_count == 0 ||
      high_pri_app >= QDSP_APP_MAX)
  {
    /* Nothing to enable */
    MSG_MED("No modules to enable. Exiting...",0,0,0);
    return;
  }

  if (total_enable_count == 1)
  {
    /* Try to use the preferred image */
    qdsp_get_preferred_image(
      qdsp_app_table[high_pri_app].module_requested,
      &image_required,
      &force_preemption
    );
    
    MSG_HIGH("Pref img:%d for mod:%d, force:%d", image_required,
             qdsp_app_table[high_pri_app].module_requested, force_preemption);

    /* Revert to current image if preemption is not in effect */
    if (image_required < QDSP_IMAGE_MAX)
    {
      if (force_preemption == FALSE &&
          qdsp_app_table[high_pri_app].state == QDSP_APP_STATE_ENABLED)
      {
        image_required = qdsp_current_image;
      }
    }

    /* Check how many modules can be supported by this image */
    if (image_required < QDSP_IMAGE_MAX)
    {
      (void)qdsp_modules_supported_by_image (
        qdsp_app_table[high_pri_app].module_requested,
        image_required,
        qdsp_mods_to_be_enabled,
        &qdsp_image_supp_data[num_images_can_be_used]
      );
      num_images_can_be_used++;
    }
  }

  if (image_required >= QDSP_IMAGE_MAX &&
      qdsp_current_image < QDSP_IMAGE_MAX)
  {
    /* Check how many modules can be supported by the current image */
    if (  qdsp_modules_supported_by_image (
            qdsp_app_table[high_pri_app].module_requested,
            qdsp_current_image,
            qdsp_mods_to_be_enabled,
            &qdsp_image_supp_data[num_images_can_be_used]
          ) == TRUE
       )
    {
      if (qdsp_image_supp_data[num_images_can_be_used].count)
      {
        num_images_can_be_used++;
        if (qdsp_image_supp_data[most_modules_supported_by].count >=
                                                           total_enable_count)
        {
          image_required = qdsp_current_image;
        }
      }
    }
  }

  /* No preferred image and the current image is not suitable. Try to find
  ** another image */
  if (image_required >= QDSP_IMAGE_MAX)
  {
    MSG_HIGH("crnt & pref img unacceptable, searching...",0,0,0);
    start_image = (qdsp_image_type)0;
    next_image = start_image;
    /* Walk thru the image list in a circular loop, starting from the next
       image that follows the current image */
    do
    {
      if (next_image != qdsp_current_image)
      {
        if (  qdsp_modules_supported_by_image (
                qdsp_app_table[high_pri_app].module_requested,
                next_image,
                qdsp_mods_to_be_enabled,
                &qdsp_image_supp_data[num_images_can_be_used]
              ) == TRUE
           )
        {
          if (qdsp_image_supp_data[num_images_can_be_used].count >
              qdsp_image_supp_data[most_modules_supported_by].count)
          {
            most_modules_supported_by = num_images_can_be_used;
          }

          num_images_can_be_used++;

          if (qdsp_image_supp_data[most_modules_supported_by].count >=
                                                           total_enable_count)
          {
            break;
          }
        }
      }

      next_image += 1;
      if (next_image >= QDSP_IMAGE_MAX)
      {
        next_image = 0;
      }
    } while (start_image != next_image);
  }


  /* Use the image that can support the max number of modules concurrently */
  if (num_images_can_be_used &&
      qdsp_image_supp_data[most_modules_supported_by].count)
  {
    boolean apps_disabled = FALSE;
    uint16 mode_index = 0;
    qdsp_module_type* new_mode =
                     qdsp_image_supp_data[most_modules_supported_by].mode;
    image_required = qdsp_image_supp_data[most_modules_supported_by].image;

    MSG_HIGH("chose img:%d",image_required,0,0);

    /* The following is based on the assumption that the modules in all the
     * data structures in qdsp_modules_info always appear in ascending order
     * (by value). The parser will always guarantee this. */
    for (module=(qdsp_module_type)0; module<QDSP_MODULE_MAX; module++)
    {
      if (new_mode[mode_index] != module)
      {
        /* Unselect this module, since it is not available in the selected
         * mode. */
        qdsp_mods_to_be_enabled[module] = FALSE;
      }
      else
      {
        mode_index++;
      }
    }

    if (image_required == qdsp_current_image)
    {
      /* Determine if a DSP reset has been requested by _any_ of the apps that
         will be enabled */
      for (app = (qdsp_app_type)0; app < QDSP_APP_MAX; app++)
      {
        if (qdsp_app_table[app].reset)
        {
          switch (qdsp_app_table[app].state)
          {
            case QDSP_APP_STATE_ENABLING:
            case QDSP_APP_STATE_ENABLE_REQUESTED:
            case QDSP_APP_STATE_ENABLED:
              if (qdsp_mods_to_be_enabled[
                    qdsp_app_table[app].module_requested ] == TRUE)
              {
                MSG_HIGH("Acting on reset request from app %d", app, 0, 0);
                reset_dsp = TRUE;
              }
              break;

            case QDSP_APP_STATE_PREEMPTING:
            case QDSP_APP_STATE_DISABLED:
            default:
              /* Should never get here */
              ERR_FATAL("Invalid state for App %d", app, 0, 0);
              break;
          }
        }

        if (reset_dsp == TRUE)
        {
          break;
        }
      }

#ifdef QDSP_MASTER_CONTROLLER_USED
      if (reset_dsp == FALSE &&
          qdsp_check_if_any_module_requires_image_reset(image_required))
      {
        MSG_HIGH("At least one new module requires reset to load", 0, 0, 0);
        reset_dsp = TRUE;
      }
#endif /* QDSP_MASTER_CONTROLLER_USED */
    }
    else
    {
      MSG_HIGH("Current image=%d, resetting to swap in new image=%d",
               qdsp_current_image, image_required, 0);
      reset_dsp = TRUE;
    }

    if (reset_dsp == TRUE)
    {
      MSG_HIGH("Disabling all apps, resetting!",0,0,0);
      /* Disable the currently enabled apps since the image has changed */
      for (app = (qdsp_app_type)0; app < QDSP_APP_MAX; app++)
      {
        if (qdsp_app_table[app].state == QDSP_APP_STATE_ENABLED ||
           (qdsp_app_table[app].state == QDSP_APP_STATE_ENABLE_REQUESTED &&
            qdsp_app_table[app].nested_enable == TRUE))
        {
          qdsp_disable_app(app);
          apps_disabled = TRUE;
        }
      }
    }
    else
    {
      /* Preempt any currently enabled apps if they cannot be supported in the
       * new mode;
       * Also selectively preempt apps whose new requests may need new BIOS
       * commands to be sent. */
      MSG_HIGH("Disabling apps that cannot be supported...",0,0,0);
      for (app = (qdsp_app_type)0; app < QDSP_APP_MAX; app++)
      {
        /* Check if this app cannot be supported in the new mode */
        if (qdsp_app_table[app].state == QDSP_APP_STATE_ENABLED ||
            (qdsp_app_table[app].state == QDSP_APP_STATE_ENABLE_REQUESTED &&
             qdsp_app_table[app].nested_enable == TRUE))
        {
          boolean disable_this_app = FALSE;

          if (qdsp_mods_to_be_enabled[
                qdsp_app_table[app].module_requested ] == FALSE)
          {
            MSG_HIGH("app:%d cannot be supported by img:%d, disabling",
                     app, image_required, 0);
            disable_this_app = TRUE;
          }
#ifdef QDSP_MASTER_CONTROLLER_USED
          /* Check if this app may require a new BIOS command to be sent
           * if its new request differs from the previously enabled module */
          else if (            
            ((qdsp_app_table[app].module_requested !=
              qdsp_app_table[app].module_enabled) &&
             (qdsp_get_module_bitmask(image_required,
                                      qdsp_app_table[app].module_requested) !=
              qdsp_get_module_bitmask(image_required,
                                      qdsp_app_table[app].module_enabled))))
          {
            MSG_HIGH("Need to disable app=%d to enable new module=%d, curr=%d",
                     app, qdsp_app_table[app].module_requested,
                     qdsp_app_table[app].module_enabled);

            disable_this_app = TRUE;
          }
#endif /* QDSP_MASTER_CONTROLLER_USED */

          if (disable_this_app == TRUE)
          {
            qdsp_disable_app(app);
            apps_disabled = TRUE;
          }
        }
      }
    }

    if (apps_disabled == TRUE)
    {
      /* At least one app. may have gotten disabled.
       * Check if all of them have responded. */
      if (qdsp_all_acks_recvd(&not_ready_app_id) == FALSE)
      {
        /* Wait to receive all the ACKs. */
        MSG_HIGH("Waiting for app disable ACKs...",0,0,0);
        qdsp_services_current_state = QDSP_SERVICES_STATE_WAITING_FOR_APP_RESET;
        (void) rex_set_timer(&qdsp_image_reset_timer, QDSP_IMAGE_RESET_TIME);
      }
      else
      {
        /* All preempted apps have already disabled themselves: can happen
         * if all preempted apps run in higher priority tasks compared to QDSP
         * Services. */
        qdsp_download_and_enable_apps(image_required, reset_dsp);
      }
    }
    else
    {
      /* Didn't preempt anybody; can start enabling */
      qdsp_download_and_enable_apps(image_required, reset_dsp);
    }
  }
  else
  {
    ERR_FATAL("Unable to find a valid image", 0, 0, 0);
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_check_all_apps_state

DESCRIPTION
  If all apps. has responded to any pending events, it clears the RESET_SIG
  timer.

DEPENDENCIES

RETURN VALUE
  TRUE if all apps has responded.

SIDE EFFECTS
  None.

===========================================================================*/
boolean qdsp_check_all_apps_state(void)
{
  boolean        all_apps_responded = TRUE;
  qdsp_app_type  not_ready_app_id;

  all_apps_responded = qdsp_all_acks_recvd(&not_ready_app_id);
  if (all_apps_responded == TRUE)
  {
    /* Clear the image reset timer
    */
    (void) rex_clr_timer( &qdsp_image_reset_timer );
    (void) rex_clr_sigs( &qdsp_tcb, QDSP_IMAGE_RESET_SIG );
  }

  return all_apps_responded;
}

void qdsp_services_isr(
  qdsp_isr_type interrupt_line /* Line that caused the interrupt */
);

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_check_state

DESCRIPTION
  Checks the current state of QDSP Services makes the appropriate state
  transitions.

DEPENDENCIES
  qdsp_services_current_state
  qdsp_pending_app_request

RETURN VALUE
  None.

SIDE EFFECTS
  qdsp_services_current_state
  qdsp_pending_app_request

===========================================================================*/
void qdsp_check_state(void)
{
  /* NOTE: qdsp_services_current_state can be changed from interrupt
     context! */
  qdsp_services_state_type current_state = qdsp_services_current_state;

  switch (current_state)
  {
    case QDSP_SERVICES_STATE_IDLE:
      break;

    case QDSP_SERVICES_STATE_WAITING_FOR_APP_RESET:
      if (qdsp_check_all_apps_state() == TRUE)
      {
        qdsp_services_current_state = QDSP_SERVICES_STATE_IDLE;
        qdsp_pending_app_request = TRUE;
      }
      else
      {
        /* Do nothing while waiting for the Apps to respond. */
        return;
      }
      break;

    case QDSP_SERVICES_STATE_WAITING_FOR_MODULE_SWITCH:
      /* Do nothing while waiting for the ADSP response interrupt. */
      return;

    case QDSP_SERVICES_STATE_MODULE_SWITCH_SUCCESSFUL:
      MSG_HIGH("check_state calling clocks_disable for SELF",0,0,0);
      qdsp_clocks_disable(QDSP_APP_SELF);
      qdsp_inform_enabled_apps(qdsp_mods_to_be_enabled);
      qdsp_services_isr(QDSP_ISR_INT0);
      break;

    default:
      ERR_FATAL("Invalid internal state", 0, 0, 0);
      break;
  }

  (void) rex_clr_sigs( &qdsp_tcb, QDSP_CHECK_STATE_SIG );

  if (qdsp_pending_app_request == TRUE)
  {
    qdsp_enable_modules();
  }
}

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_process_pending_request

DESCRIPTION
  Causes any outstanding application requests to be processed.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  qdsp_pending_app_request

===========================================================================*/
void qdsp_process_pending_request(void)
{
  qdsp_pending_app_request = TRUE;
  qdsp_check_state();
}

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_REGISTER_APP_PROCESSING

DESCRIPTION
  Register interrupt handler functions, event callback function and event mask.
  Once registered, applications will begin receiving all events in their
  event mask.  The application's interrupt handlers will only be called
  when the application has enabled the QDSP by calling qdsp_enable().

  Note on registering ISRs:
    If the client does not wish to register for any DSP interrupts, then
       the isrs_ptr parameter must be set to NULL.
    If interrupts are registered, they must be passed using
       qdsp_isr_func_type pointers in an array of size QDSP_ISR_MAX.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A QDSP_EV_COMMAND event will issued to the calling application once the
  command has been accepted.

===========================================================================*/
void qdsp_register_app_processing (
  qdsp_packet_type* packet /* The register command packet */
)
{
  qdsp_app_type app_id;

  ASSERT( packet != NULL );

  /* app_id already verified in qdsp_register_app() */
  app_id = packet->register_app_cmd.app_id;

  if ( app_id >= QDSP_APP_MAX )
  {
    /* Can't issue failure callback, since the app_id is invalid. */
    ERR_FATAL("Invalid QDSP AppId %d", app_id, 0, 0);
  }

  /* Register the application */
  qdsp_app_table[app_id].registered = TRUE;
  qdsp_app_table[app_id].cb_ptr     = packet->register_app_cmd.cb_ptr;
  qdsp_app_table[app_id].event_mask = packet->register_app_cmd.event_mask;

  memcpy(qdsp_app_table[app_id].isrs, packet->register_app_cmd.isrs,
         sizeof(qdsp_app_table[app_id].isrs));

  /* Notify application that command was accepted */
  qdsp_send_cmd_event( app_id, QDSP_CMD_ACCEPT );

} /* qdsp_register_app_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_REGISTER_ISR_PROCESSING

DESCRIPTION
  Applications use this function to register an interrupt handler function
  for the specified DSP interrupt. The application's interrupt handler will
  only be called when the application has enabled the QDSP by calling
  qdsp_enable(). This function may be used to change the client's registered
  ISRs even after the client is enabled using qdsp_enable().

DEPENDENCIES
  The client must have been previously registered using qdsp_register_app().

RETURN VALUE
  None

SIDE EFFECTS
  A QDSP_EV_COMMAND event will issued to the calling application once the
  command has been accepted.

===========================================================================*/
void qdsp_register_isr_processing (
  qdsp_packet_type* packet /* The register command packet */
)
{
  qdsp_app_type app_id;
  qdsp_command_status_type cmd_status = QDSP_CMD_ACCEPT;

  ASSERT( packet != NULL );

  /* app_id already verified in qdsp_register_isr() */
  app_id = packet->register_isr_cmd.app_id;

  if ( app_id >= QDSP_APP_MAX )
  {
    /* Can't issue failure callback, since the app_id is invalid. */
    ERR_FATAL("Invalid QDSP AppId %d", app_id, 0, 0);
  }

  if (!qdsp_app_table[app_id].registered)
  {
    MSG_ERROR("qdsp_register_isr failed: Unregistered app_id = %d",app_id,0,0);
  }
  else
  {
    if (packet->register_isr_cmd.isr_type >= QDSP_ISR_MAX)
    {
      MSG_ERROR("Invalid ISR type = %d", packet->register_isr_cmd.isr_type,0,0);
      cmd_status = QDSP_CMD_FAIL;
    }
    else
    {
      /* Register the ISR */
      qdsp_app_table[app_id].isrs[packet->register_isr_cmd.isr_type] =
                                              packet->register_isr_cmd.isr_ptr;
    }
    qdsp_send_cmd_event( app_id, cmd_status );
  }

} /* qdsp_register_isr_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_ENABLE_PROCESSING

DESCRIPTION
  Applications use this function to indicate which QDSP module should be
  enabled. When the calling application's priority is high enough,
  QDSP Services will issue a callback to the application, indicating that it
  has been enabled. When the "MOD_READY" event is received, the QDSP clocks are
  on, the current firmware image is running and the requested module is ready
  for use on the QDSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A QDSP_EV_COMMAND event will issued to the calling application once the
  command has been accepted.

===========================================================================*/
void qdsp_enable_processing (
  qdsp_packet_type* packet /* The enable command packet */
)
{
  qdsp_app_type   app_id;

  ASSERT( packet != NULL );

  /* app_id already verified in qdsp_enable() */
  app_id = packet->enable_cmd.app_id;

  if ( app_id >= QDSP_APP_MAX )
  {
    /* Can't issue failure callback, since the app_id is invalid. */
    ERR_FATAL("Invalid QDSP AppId %d", app_id, 0, 0);
  }

  MSG_HIGH("enable_processing:%d,%d,%d", app_id,
           packet->enable_cmd.module, qdsp_app_table[app_id].state);
  if ( qdsp_app_table[app_id].registered == FALSE )
  {
    MSG_ERROR("qdsp_enable failed: Unregistered app_id = %d", app_id, 0, 0 );
  }
  else
  {
    switch (qdsp_app_table[app_id].state)
    {
      case QDSP_APP_STATE_ENABLED:
        if (qdsp_app_table[app_id].module_enabled == packet->enable_cmd.module)
        {
          qdsp_send_cmd_event(app_id, QDSP_CMD_ACCEPT);
          qdsp_send_module_event(app_id, QDSP_MOD_READY, &qdsp_current_image);
          qdsp_app_table[app_id].nested_enable = FALSE;
          qdsp_app_table[app_id].reset = FALSE;
          break;
        }
        /* else fall through */

      case QDSP_APP_STATE_DISABLED:
      case QDSP_APP_STATE_ENABLE_REQUESTED:
      case QDSP_APP_STATE_ENABLING:
        if ( (qdsp_app_table[app_id].state == QDSP_APP_STATE_ENABLED) ||
             ( (qdsp_app_table[app_id].state == QDSP_APP_STATE_ENABLE_REQUESTED) &&
               (qdsp_app_table[app_id].nested_enable == TRUE) ) )
        {
          qdsp_app_table[app_id].nested_enable = TRUE;
        }
        else
        {
          qdsp_app_table[app_id].nested_enable = FALSE;
        }
        qdsp_app_table[app_id].state  = QDSP_APP_STATE_ENABLE_REQUESTED;
        qdsp_app_table[app_id].module_requested = packet->enable_cmd.module;
        qdsp_app_table[app_id].reset = FALSE;

        /* Send the CMD_ACCEPT event before we start processing the request. */
        qdsp_send_cmd_event( app_id, QDSP_CMD_ACCEPT );

        MSG_HIGH("enable_processing calling clocks_enable: %d",app_id,0,0);
        qdsp_clocks_enable( app_id );

        qdsp_process_pending_request();
        break;

      case QDSP_APP_STATE_PREEMPTING:
        MSG_ERROR("qdsp_enable called before responding to MOD_DISABLE",0,0,0);
        qdsp_send_cmd_event( app_id, QDSP_CMD_FAIL );
        break;

      default:
        ERR_FATAL("Unknown Application state!", 0, 0, 0);
        qdsp_app_table[app_id].state  = QDSP_APP_STATE_DISABLED;
        qdsp_send_cmd_event( app_id, QDSP_CMD_FAIL );
        break;
    }
  }

} /* qdsp_enable_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_DISABLE_PROCESSING

DESCRIPTION
  Disables the QDSP when no applications require it to be enabled.  When the
  QDSP is disabled, its clocks are off and the current firmware image is not
  running.  The QDSP draws minimum current when it is disabled.

DEPENDENCIES
  qdsp_qpp_table

RETURN VALUE
  None

SIDE EFFECTS
  A QDSP_EV_COMMAND event will issued to the calling application once the
  command has been accepted.

===========================================================================*/
void qdsp_disable_processing (
  qdsp_packet_type* packet /* The disable command packet */
)
{
  qdsp_app_type app_id;
  qdsp_app_state_type      saved_app_state;

  ASSERT( packet != NULL );

  /* app_id already verified in qdsp_disable() */
  app_id = packet->disable_cmd.app_id;

  if ( app_id >= QDSP_APP_MAX )
  {
    /* Can't issue failure callback, since the app_id is invalid. */
    ERR_FATAL("Invalid QDSP AppId %d", app_id, 0, 0);
  }

  MSG_HIGH("disable_processing:%d,%d",app_id,qdsp_app_table[app_id].state,0);
  if ( qdsp_app_table[app_id].registered == FALSE )
  {
    MSG_ERROR( "qdsp_disable failed: Unregistered app_id = %d", app_id, 0, 0 );
  }
  else
  {
    saved_app_state = qdsp_app_table[app_id].state;
    qdsp_app_table[app_id].state = QDSP_APP_STATE_DISABLED;
    qdsp_app_table[app_id].reset = FALSE;
    qdsp_app_table[app_id].nested_enable = FALSE;
    qdsp_app_table[app_id].module_requested = QDSP_MODULE_MAX;

    qdsp_send_cmd_event( app_id, QDSP_CMD_ACCEPT );

    switch (saved_app_state)
    {
      case QDSP_APP_STATE_DISABLED:
        /* NOP */
        break;

      case QDSP_APP_STATE_ENABLE_REQUESTED:
        MSG_HIGH("disable_processing calling clocks_disable: %d",app_id,0,0);
        qdsp_clocks_disable( app_id );
        break;

      default:
        MSG_HIGH("disable_processing calling clocks_disable: %d",app_id,0,0);
        qdsp_clocks_disable( app_id );
        qdsp_process_pending_request();
        break;
    }
  }

} /* qdsp_disable_processing */


/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_RESET_AND_ENABLE_PROCESSING

DESCRIPTION
  Applications use this function to reset the DSP and enable a module.
  This is useful when the client wants to ensure that QDSP services actually
  does a reset and redownload of the image, before it is enabled. In the
  normal scenario, the DSP is not reset between successive calls to
  qdsp_enable() and qdsp_disable(), if the image has not changed.

  NOTE: This interface must used only as a debugging aid, since resetting the
  DSP may cause other QDSP Services clients to react adversely.

DEPENDENCIES
  qdsp_app_table

RETURN VALUE
  None

SIDE EFFECTS
  A QDSP_EV_COMMAND event will issued to the calling application once the
  command has been accepted.

===========================================================================*/
void qdsp_reset_and_enable_processing (
  qdsp_packet_type* packet /* The disable command packet */
)
{
  qdsp_app_type app_id;

  ASSERT( packet != NULL );

  /* app_id already verified in qdsp_reset() */
  app_id = packet->reset_and_enable_cmd.app_id;

  if ( app_id >= QDSP_APP_MAX )
  {
    /* Can't issue failure callback, since the app_id is invalid. */
    ERR_FATAL("Invalid QDSP AppId %d", app_id, 0, 0);
  }

  MSG_HIGH("reset_and_enable_processing:%d,%d,%d",
           app_id, packet->reset_and_enable_cmd.module,
           qdsp_app_table[app_id].state);
  if ( qdsp_app_table[app_id].registered == FALSE )
  {
    MSG_ERROR( "Unregistered app_id = %d", app_id, 0, 0 );
    qdsp_send_cmd_event( app_id, QDSP_CMD_FAIL );
  }
  else
  {
    switch (qdsp_app_table[app_id].state)
    {
      case QDSP_APP_STATE_ENABLED:
      case QDSP_APP_STATE_DISABLED:
      case QDSP_APP_STATE_ENABLE_REQUESTED:
      case QDSP_APP_STATE_ENABLING:
        qdsp_app_table[app_id].state  = QDSP_APP_STATE_ENABLE_REQUESTED;
        qdsp_app_table[app_id].module_requested =
                                        packet->reset_and_enable_cmd.module;
        qdsp_app_table[app_id].reset  = TRUE;

        /* Send the CMD_ACCEPT event before we start processing the request. */
        qdsp_send_cmd_event( app_id, QDSP_CMD_ACCEPT );

        MSG_HIGH("reset_and_enable_processing calling clocks_enable: %d",
                 app_id, 0, 0);
        qdsp_clocks_enable( app_id );

        qdsp_process_pending_request();
        break;

      case QDSP_APP_STATE_PREEMPTING:
        MSG_ERROR("qdsp_reset_and_enable() called before responding to MOD_DISABLE",
                  0, 0, 0);
        qdsp_send_cmd_event( app_id, QDSP_CMD_FAIL );
        break;

      default:
        ERR_FATAL("Unknown Application state!", 0, 0, 0);
        qdsp_app_table[app_id].state  = QDSP_APP_STATE_DISABLED;
        qdsp_send_cmd_event( app_id, QDSP_CMD_FAIL );
        break;
    }
  }
} /* qdsp_reset_and_enable_processing */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_DISABLE_EVENT_RSP_PROCESSING

DESCRIPTION
  Processes the clients' response to a previously sent MOD_DISABLE event.

DEPENDENCIES
  qdsp_app_table

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void qdsp_disable_event_rsp_processing (
  qdsp_packet_type* packet /* The disable_event_rsp command packet */
)
{
  qdsp_app_type app_id;

  ASSERT( packet != NULL );

  /* app_id already verified in qdsp_disable_event_rsp() */
  app_id = packet->disable_event_rsp_cmd.app_id;

  if ( app_id >= QDSP_APP_MAX )
  {
    /* Can't issue failure callback, since the app_id is invalid. */
    ERR_FATAL("Invalid QDSP AppId %d", app_id, 0, 0);
  }

  if (qdsp_app_table[app_id].registered == FALSE)
  {
    MSG_ERROR("Unregistered app_id = %d", app_id,0,0);
  }
  else
  {
    switch (qdsp_app_table[app_id].state)
    {
      case QDSP_APP_STATE_PREEMPTING:
        qdsp_app_table[app_id].state = QDSP_APP_STATE_ENABLE_REQUESTED;
        qdsp_app_table[app_id].nested_enable = FALSE;
        qdsp_app_table[app_id].module_enabled = QDSP_MODULE_MAX;
        qdsp_send_cmd_event( app_id, QDSP_CMD_ACCEPT );
        qdsp_process_pending_request();
        return;

      case QDSP_APP_STATE_DISABLED:
      case QDSP_APP_STATE_ENABLE_REQUESTED:
      case QDSP_APP_STATE_ENABLING:
      case QDSP_APP_STATE_ENABLED:
        MSG_ERROR("Unexpected disable_event_rsp from app %d", app_id, 0, 0);
        break;

      default:
        ERR_FATAL("Unknown Application state!", 0, 0, 0);
        break;
    }
    qdsp_send_cmd_event( app_id, QDSP_CMD_FAIL );
  }

} /* qdsp_disable_event_rsp_processing */


/* <EJECT> */
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
void qdsp_clocks_enable (
  qdsp_app_type app_id /* Application ID */
)
{
  MSG_HIGH("clocks_enable: %d",app_id,0,0);
  if (app_id <= QDSP_APP_MAX || app_id == QDSP_APP_SELF)
  {
    /* Critical section - enable clocks, if not already */
    INTLOCK();

    if (qdsp_clock_mask == 0)
    {
      if ( qdsp_current_image < QDSP_IMAGE_MAX )
      {
        MSG_HIGH("clocks_enable calling config_hw_resources_for_image:%x,%d",
                 qdsp_clock_mask, qdsp_current_image, 0);
#ifdef QDSP_SET_MODULE_HW_RESOURCES
        /* Save the fact that we're changing the DSP clk and DME setting */
        qdsp_previous_dme_mode = 
          qdsp_get_image_dme_mode( qdsp_current_image );
        qdsp_previous_clock_rate = 
          qdsp_get_image_clock_rate( qdsp_current_image );
#endif
        qdsp_config_hw_resources_for_image( qdsp_current_image );
      }

#ifdef FEATURE_DUAL_CLK_RGM
      clk_regime_msm_enable( CLK_RGM_VOC_M ) ;
#else
      clk_regime_enable( CLK_RGM_VOC_M );
#endif

    }

    qdsp_clock_mask |= QDSP_APP_CLK_MASK( app_id );

    INTFREE();
  }
  else
  {
    ERR_FATAL("qdsp_clocks_enable failed: Invalid app_id = %d", app_id, 0, 0);
  }

} /* qdsp_clocks_enable */

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
void qdsp_clocks_disable (
  qdsp_app_type app_id /* Application ID */
)
{
  if (app_id <= QDSP_APP_MAX || app_id == QDSP_APP_SELF)
  {
    /* Critical section - disable clocks, if necessary */
    INTLOCK();

    qdsp_clock_mask &= ~QDSP_APP_CLK_MASK( app_id );

    if ( qdsp_clock_mask == 0 )
    {
#ifdef QDSP_SET_MODULE_HW_RESOURCES
      qdsp_previous_dme_mode   = CLKRGM_DME_NOT_REQUIRED;
#endif
      qdsp_config_hw_resources_for_sleep_mode();

#ifdef FEATURE_DUAL_CLK_RGM
      clk_regime_msm_disable( CLK_RGM_VOC_M ) ;
#else
#ifndef FEATURE_OEMSBL_AUDIBLE_TONE 
      clk_regime_disable( CLK_RGM_VOC_M );
#endif /* FEATURE_OEMSBL_AUDIBLE_TONE */
#endif

    }

    INTFREE();
  }
  else
  {
    ERR_FATAL("qdsp_clocks_disable failed: Invalid app_id = %d", app_id, 0, 0);
  }

} /* qdsp_clocks_disable */

#ifdef QDSP_MASTER_CONTROLLER_USED
/*===========================================================================

FUNCTION qdsp_master_controller_isr

DESCRIPTION
  Interrupt service routine which handles all the interrupts sent by the ADSP
  downloader.

DEPENDENCIES
  qdsp_current_image

RETURN VALUE
  None

SIDE EFFECTS
  qdsp_services_current_state

===========================================================================*/
void qdsp_master_controller_isr(void)
{
  if (qdsp_services_current_state ==
      QDSP_SERVICES_STATE_WAITING_FOR_MODULE_SWITCH)
  {
    /* Check if the requested modules could be enabled. */
    if (qdsp_read(QDSP_biosuPCommandReg) == 0)
    {
      if (qdsp_read(QDSP_biosStateReg) == QDSP_DOWNLOADER_STATE_DWNLD_DONE)
      {
        (void) rex_clr_timer( &qdsp_dsp_cmd_rsp_timer );
        (void) rex_clr_sigs( &qdsp_tcb, QDSP_DSP_CMD_TIMEOUT_SIG );

        qdsp_services_current_state =
                                QDSP_SERVICES_STATE_MODULE_SWITCH_SUCCESSFUL;

        (void) rex_set_sigs( &qdsp_tcb, QDSP_CHECK_STATE_SIG );
      }
    }
  }
}
#endif /* QDSP_MASTER_CONTROLLER_USED */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_SERVICES_ISR

DESCRIPTION
  Interrupt service routine which handles all the interrupts sent by the QDSP.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void qdsp_services_isr(
  qdsp_isr_type interrupt_line /* Line that caused the interrupt */
)
{
  qdsp_app_type       i;
  qdsp_app_table_type *app_ptr;

  if (interrupt_line >= QDSP_ISR_MAX )
  {
    /* invalid interrupt line */
    ERR_FATAL("Invalid QDSP Interrupt Line %d", interrupt_line, 0, 0);
  }

  if ((qdsp_current_image < QDSP_IMAGE_MAX) && qdsp_clock_mask)
  {
#ifdef QDSP_MASTER_CONTROLLER_USED
    if (qdsp_controller_ints[qdsp_current_image] == interrupt_line)
    {
      qdsp_master_controller_isr();
    }
#endif

    /* Call ISR for all enabled applications; highest priority first */
    if (qdsp_current_image < QDSP_IMAGE_MAX)
    {
      for (i = (qdsp_app_type)0; i < QDSP_APP_MAX; i++)
      {
        app_ptr = &qdsp_app_table[i];
        if (app_ptr->isrs[interrupt_line] != NULL)
        {
          switch (app_ptr->state)
          {
            case QDSP_APP_STATE_ENABLING:
              if (qdsp_services_current_state !=
                  QDSP_SERVICES_STATE_MODULE_SWITCH_SUCCESSFUL)
              {
                break;
              }
              /* Fall Through */

            case QDSP_APP_STATE_ENABLED:
            case QDSP_APP_STATE_PREEMPTING:
              (app_ptr->isrs[interrupt_line])();
              break;

            default:
              break;
          }
        }
      }
    }
  }
}

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
void qdsp_cmd (
  qdsp_packet_type* qdsp_cmd_ptr /* The command packet */
)
{
  ASSERT( qdsp_cmd_ptr != NULL );

  /* Put the command packet in the queue */
  (void) q_link( qdsp_cmd_ptr, &qdsp_cmd_ptr->hdr.link );
  q_put( &qdsp_cmd_q, &qdsp_cmd_ptr->hdr.link);

  /* Signal task to process command queue */
  (void) rex_set_sigs( &qdsp_tcb, QDSP_CMD_Q_SIG );

} /* qdsp_cmd */

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
  qdsp_free_q (and the packet fetched off qdsp_free_q)

===========================================================================*/
qdsp_packet_type* qdsp_get_packet( void )
{
  qdsp_packet_type* packet;

  /* Get a packet and then either report an error, or clear the packet out
  ** so the previous user's garbage doesn't do something unexpected.
  */
  packet = (qdsp_packet_type*) q_get( &qdsp_free_q );

  if (packet == NULL)
  {
    ERR( "Ran out of QDSP packets", 0,0,0 );
  }
  else
  {
    /* Fill the entire QDSP packet with 0's to get
    ** default behavior on un-initialized fields.
    */
#ifdef  FIX_LINKITEM_INITBUG
    q_link_type teplink;
    
    (void)memcpy(&teplink,  &(packet->hdr.link), sizeof(q_link_type));
    (void) memset (packet, 0, sizeof (voc_packet_type));
    (void)memcpy(&(packet->hdr.link), &teplink, sizeof(q_link_type));
#else    
    (void) memset (packet, 0, sizeof (voc_packet_type));
#endif
    packet->hdr.cmd = QDSP_CMD_INVALID;
  }

  return( packet );

} /* qdsp_get_packet */

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_cmd_done

DESCRIPTION
  Dispose of a command which is done being processed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void qdsp_cmd_done (
  /* The command packet */
  qdsp_packet_type* qdsp_cmd_ptr
)
{
  q_put( &qdsp_free_q, &qdsp_cmd_ptr->hdr.link );

} /* qdsp_cmd_done */

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_CMD_PROCESSOR

DESCRIPTION
  Processes all of the commands in the QDSP Services command queue.  It must
  be called in response to the QDSP_EV_CMD_Q event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void qdsp_cmd_processor( void )
{
  qdsp_packet_type *cmd_ptr;

  /* Clear the 'queued commands' signal prior to emptying the queue.
  ** This is done in order to avoid race conditions where an item is
  ** added to the queue just as we are done servicing the queue.
  */
  (void) rex_clr_sigs( &qdsp_tcb, QDSP_CMD_Q_SIG );

  /* While there are commands to process... */
  while( (cmd_ptr = (qdsp_packet_type *) q_get( &qdsp_cmd_q )) != NULL )
  {
    /* Process each command type */
    switch( cmd_ptr->hdr.cmd )
    {
      /* Register an application */
      case QDSP_CMD_REGISTER_APP:
        qdsp_register_app_processing( cmd_ptr );
        break;

      /* Register an ISR */
      case QDSP_CMD_REGISTER_ISR:
        qdsp_register_isr_processing( cmd_ptr );
        break;

      /* Enable the QDSP */
      case QDSP_CMD_ENABLE:
        qdsp_enable_processing( cmd_ptr );
        break;

      /* Disable the QDSP */
      case QDSP_CMD_DISABLE:
        qdsp_disable_processing( cmd_ptr );
        break;

      /* Reset the QDSP */
      case QDSP_CMD_RESET_AND_ENABLE:
        qdsp_reset_and_enable_processing( cmd_ptr );
        break;

      /* Application ready for disabling */
      case QDSP_CMD_DISABLE_EVENT_RSP:
        qdsp_disable_event_rsp_processing( cmd_ptr );
        break;

      default:
        ERR_FATAL( "Illegal command: %x", cmd_ptr->hdr.cmd, 0, 0 );
        /* Does not return */
    }

    qdsp_cmd_done( cmd_ptr );
  }

} /* qdsp_cmd_processor */

/*===========================================================================

FUNCTION qdsp_fatal_error_core_dump

DESCRIPTION
  Get a QDSP MEMA, MEMB , MEMC and REGISTER memory dumps in case of ERROR_FATAL.
  
DEPENDENCIES
  
RETURN VALUE
  void
===========================================================================*/
void qdsp_fatal_error_core_dump( void )  
{
#ifdef FEATURE_QDSP_ERR_LOGS_STORE
#error code not present
#endif /* FEATURE_QDSP_ERR_LOGS_STORE */
}

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_task_init

DESCRIPTION
  Initializes the QDSP task.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  qdsp_free_q

===========================================================================*/
void qdsp_task_init( void )
{
  uint16 i;

  /* Initialize timers */
  rex_def_timer( &qdsp_rpt_timer, &qdsp_tcb, QDSP_RPT_TIMER_SIG );
  rex_def_timer( &qdsp_image_reset_timer, &qdsp_tcb, QDSP_IMAGE_RESET_SIG );
#ifdef QDSP_MASTER_CONTROLLER_USED
  rex_def_timer( &qdsp_dsp_cmd_rsp_timer, &qdsp_tcb, QDSP_DSP_CMD_TIMEOUT_SIG);
#endif

  /* Initialize the command queues */
  (void) q_init( &qdsp_cmd_q );
  (void) q_init( &qdsp_free_q );

  /* Add packets to the free command queue */
  for( i = 0; i < QDSP_NUM_PACKETS; i++ )
  {
    q_link_type *link_item;
    link_item = q_link( &qdsp_free_packet[i], &qdsp_free_packet[i].hdr.link );
    q_put( &qdsp_free_q, link_item );
  }

  /* Install the QDSP-to-ARM ISR's */
  qdsp_register_dsp_services_isrs();

  /* Initialize the application table */
  for( i = (qdsp_app_type)0; i < QDSP_APP_MAX; i++ )
  {
    qdsp_app_table[i].registered       = FALSE;
    qdsp_app_table[i].state            = QDSP_APP_STATE_DISABLED;
    qdsp_app_table[i].module_requested = QDSP_MODULE_MAX;
    qdsp_app_table[i].module_enabled   = QDSP_MODULE_MAX;
    qdsp_app_table[i].cb_ptr           = NULL;
    qdsp_app_table[i].event_mask       = 0;
    qdsp_app_table[i].nested_enable    = FALSE;
    qdsp_app_table[i].reset            = FALSE;
  }

  MSG_LOW( "QDSP task initialized", 0, 0, 0 );

} /* qdsp_task_init */

/* <EJECT> */
/*===========================================================================

FUNCTION qdsp_task

DESCRIPTION
  QDSP Services task processing function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void qdsp_task (
  /* Parameter received from Main Control task - ignored */
  dword ignored
  /*lint -esym(715,ignored)
  ** Have lint not complain about the ignored parameter 'ignored' which is
  ** specified to make this routine match the template for rex_def_task().
  */
)
{
  rex_sigs_type sigs;
  qdsp_app_type app;

  /* Initialize QDSP task */
  qdsp_task_init();

  /* Wait for start signal from Main Control task */
#ifdef FEATURE_MULTIMODE_ARCH
  task_start( QDSP_RPT_TIMER_SIG, DOG_QDSP_RPT, &qdsp_rpt_timer );
#else
  mc_task_start( QDSP_RPT_TIMER_SIG, DOG_QDSP_RPT, &qdsp_rpt_timer );
#endif /* FEATURE_MULTIMODE_ARCH */

  /* Clear start signal */
  (void) rex_clr_sigs( &qdsp_tcb, TASK_START_SIG );

  /* Set the watchdog report timer signal so that we'll
  ** reset the watchdog as soon as we start checking for events
  */
  (void) rex_set_sigs( &qdsp_tcb, QDSP_RPT_TIMER_SIG );

  /* Infinite loop -- process events forever */
  for( ;; )
  {
    /* Wait for something to happen */
    sigs = rex_wait( QDSP_CMD_Q_SIG            |  /* Command Queue Signal   */
                     QDSP_IMAGE_RESET_SIG      |  /* Image reset signal     */
                     QDSP_CHECK_STATE_SIG      |  /* Update module states   */
#ifdef QDSP_MASTER_CONTROLLER_USED
                     QDSP_DSP_CMD_TIMEOUT_SIG  |  /* DSP cmd rsp. timed out */
#endif
                     QDSP_RPT_TIMER_SIG        |  /* Watchdog report timer  */
                     TASK_STOP_SIG             |  /* Task stop from MC      */
                     TASK_OFFLINE_SIG );          /* Task offline from MC   */

    /* If watchdog report timer expired, report and restart the timer */
    if ( ( sigs & QDSP_RPT_TIMER_SIG ) != 0 )
    {
      dog_report( DOG_QDSP_RPT );
      (void) rex_set_timer( &qdsp_rpt_timer, DOG_QDSP_RPT_TIME );
    }

#ifdef QDSP_MASTER_CONTROLLER_USED
    /* The DSP did not respond to a command. */
    if ( ( sigs & QDSP_DSP_CMD_TIMEOUT_SIG ) != 0 )
    {
      (void) rex_clr_sigs( &qdsp_tcb, QDSP_DSP_CMD_TIMEOUT_SIG );

      INTLOCK();
      qdsp_master_controller_isr();
      if (qdsp_services_current_state !=
          QDSP_SERVICES_STATE_MODULE_SWITCH_SUCCESSFUL)
      {
        qdsp_fatal_error_core_dump();
        ERR_FATAL("BIOS interface for image=%d not responding",
                  qdsp_current_image, 0, 0);
      }
      INTFREE();
    }
#endif

    /* Process the command queue */
    if ( ( sigs & QDSP_CMD_Q_SIG ) != 0 )
    {
      qdsp_cmd_processor();
    }

    /* Process the image reset signal */
    if ( ( sigs & QDSP_IMAGE_RESET_SIG ) != 0 )
    {
      /* If this signal occurs, one or more applications has not
      ** called qdsp_allow_reset within the allotted time.
      */
      ERR("QDSP Image Reset Timer expired",0,0,0);

      /* Find the offending application!
      */
      if (qdsp_all_acks_recvd(&app) == FALSE)
      {
        /* Rogue application is not releasing the DSP */
        ERR_FATAL("QDSP App %d did not allow reset",app,0,0);
      }

      /* All apps. responded to the disable request.
       * We can enable the new set of modules.
       */
      (void) rex_clr_sigs( &qdsp_tcb, QDSP_IMAGE_RESET_SIG );
      (void) rex_clr_sigs( &qdsp_tcb, QDSP_CHECK_STATE_SIG );
      qdsp_process_pending_request();
    }

    /* Re-evaluate the application and module states */
    if ( ( sigs & QDSP_CHECK_STATE_SIG ) != 0 )
    {
      qdsp_check_state();
    }

    /* Handle task offline signal from MC */
    if ( ( sigs & TASK_OFFLINE_SIG ) != 0 )
    {
#ifdef FEATURE_MULTIMODE_ARCH
      task_offline();
#else
      (void) rex_set_sigs( &mc_tcb, MC_ACK_SIG );
#endif /* FEATURE_MULTIMODE_ARCH */
      (void) rex_clr_sigs( &qdsp_tcb, TASK_OFFLINE_SIG );
    }

    /* Handle task stop signal from MC */
    if ( ( sigs & TASK_STOP_SIG ) != 0 )
    {
#ifdef FEATURE_MULTIMODE_ARCH
      task_stop();
#else
      (void) rex_set_sigs( &mc_tcb, MC_ACK_SIG );
#endif /* FEATURE_MULTIMODE_ARCH */
      (void) rex_clr_sigs( &qdsp_tcb, TASK_STOP_SIG );
    }
  }
} /* qdsp_task */

