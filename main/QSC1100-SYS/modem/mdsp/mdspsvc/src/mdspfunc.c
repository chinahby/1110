/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   M D S P   S E R V I C E S   M O D U L E
                         A P I   F U N C T I O N S
                     
GENERAL DESCRIPTION
  This module provides the software interface that allows applications to
  download selected firmware images, enable and disable MDSP operation and
  gain access to the MDSP/ARM shared memory area.

EXTERNALIZED FUNCTIONS
  mdsp_peek
    Read a block of data from the MDSP/ARM shared memory. Enables DSP clocks if
    necessary.
  mdsp_poke
    Write a block of data to the MDSP/ARM shared memory. Enables DSP clocks if
    necessary.
  mdsp_memcpy
    Copies a block of 16-bit values to or from the QDSP/ARM shared memory area,
    and assumes that the DSP clocks are enabled.
  mdsp_memcpy32
    Copies a block of 32-bit values to or from the QDSP/ARM shared memory area,
    and assumes that the DSP clocks are enabled.
  mdsp_register_app
    Register an application with the MDSP Services.
  mdsp_register_isr
    Register an ISR with the MDSP Services.
  mdsp_enable
    Activate the a DSP module.
  mdsp_disable
    De-activate a DSP module.
  mdsp_reset_and_enable
    Force the DSP to be reset before enabling a DSP module (Debugging use only).
  mdsp_disable_event_rsp
    Notifies MDSP Services that the calling Application's DSP module may be
    disabled.
  mdsp_dsp_sleep
    Disable the DSP clock if no other application is enabled.
  mdsp_dsp_wakeup
    Enable the DSP clock (see dependency notes below).
  mdsp_log_set_mode
    Set the log mode and period for the given application.
  mdsp_log_send
    Send a log packet containing the internal mDSP log.
  mdsp_log_save
    Save the stored mDSP log to EFS.
  mdsp_memory_save
    Save the contents of mDSP memory to a file.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright(c) 2002 - 2008 by QUALCOMM, Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/qsc1100/services/mdspsvc/mdspfunc.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/19/08   ah      Branched from MSMSHARED_MDSP_SVCS.00.01.25
06/19/06   ah      Fixed Lint errors.
02/17/06   gfr     Added mDSP logging support
07/29/05    sg     Modified peek and poke functions to not leave the clock
                   enabled - this can occur in situations where the clock is not
                   already enabled.
11/30/04    sg     Added debug messages.
03/13/03    sg     Updated peek and poke to use INTLOCK instead of INLOCK_SAV.
02/18/03    sg     Modified to support module based interfaces.
01/24/03    sg     Added mdsp_memcpy() and mdsp_memcpy32() functions.
12/12/02    sg     Added mdsp_reset(). Rename mdsp_sleep()/mdsp_wakeup() as
                   mdsp_dsp_sleep()/mdsp_dsp_wakeup().
11/20/02    sg     Modified mdsp_sleep()/wakeup() to use synchronous interfaces.
11/15/02    sg     Added interfaces to allow applications to command the MDSP
                   to sleep/wakeup after acquiring the MDSP.
08/21/02    sg     Added checks to public interfaces to verify client's input
                   parameters. Also attempts to issue a callback indicating
                   failure whenever possible.
04/02/02    sg     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/

#include "target.h"    /* Target specific definitions            */
#include "comdef.h"    /* Common stuff                           */
#include "err.h"       /* Error reporting                        */
#include "msg.h"       /* Diagnostic error messages              */
#include "mdspext.h"   /* MDSP firmware images and access        */
#include "mdspi.h"     /* Internal MDSP Services definitions     */

/* <EJECT> */
/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed to interface with the MDSP Services module.

===========================================================================*/

/* Mask to track application clock usage */
extern uint16 mdsp_clock_mask;

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
void mdsp_peek (
  /* The start address of the MDSP memory block to read */
  void* src_ptr,
  /* The address of the ARM destination buffer */
  void* dst_ptr,
  /* The length in bytes of the memory block */
  uint32 byte_length
)
{
  /* Critical section.  Do not want MDSP task to perform any memory 
  ** accesses while peek operation is in progress.
  */
  INTLOCK();

  /* Enable MDSP clocks */
  mdsp_clocks_enable( MDSP_APP_MAX );
    
  qdsp_byte_read(src_ptr, dst_ptr, byte_length);
  
  /* Disable MDSP clocks */
  mdsp_clocks_disable( MDSP_APP_MAX );

  INTFREE();

} /* mdsp_peek */

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
void mdsp_poke (
  /* The address of the source buffer to write to the MDSP */
  void* src_ptr,
  /* The start address of the MDSP memory block to write */
  void* dst_ptr,
  /* The length in bytes of the memory block */
  uint32 byte_length
)
{
  /* Critical section.  Do not want MDSP task to perform any memory 
  ** accesses while poke operation is in progress.
  */
  INTLOCK();
  
  /* Enable MDSP clocks */
  mdsp_clocks_enable( MDSP_APP_MAX );
    
  qdsp_byte_write(src_ptr, dst_ptr, byte_length);
  
  /* Disable MDSP clocks */
  mdsp_clocks_disable( MDSP_APP_MAX );

  INTFREE();

} /* mdsp_poke */

/*===========================================================================

FUNCTION MDSP_MEMCPY

DESCRIPTION
  Copies a block of 16-bit values to or from the QDSP/ARM shared memory area.

DEPENDENCIES
  The correct QDSP clock regime must be enabled before accessing memory.
  Assumes that both the dest and src pointers are non-NULL to reduce the
  execution time.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_memcpy (
  uint16  *dest,  /* Destination address                         */
  uint16  *src,   /* Source address                              */
  uint32  wlen,   /* Length of data to copy, in words            */
  boolean swap    /* Boolean to swap when copying to destination */
)
{
#ifndef FEATURE_MDSP_DISABLE_MEMCPY_CHECKS
  uint16  tword;
#endif /* FEATURE_MDSP_DISABLE_MEMCPY_CHECKS */

  /* Prepare for loops */
  wlen++;

#ifndef FEATURE_MDSP_DISABLE_MEMCPY_CHECKS
  switch ( ((uint32)src & 1) | (((uint32)dest & 1) << 1) )
  {
    case 0:
#endif /* FEATURE_MDSP_DISABLE_MEMCPY_CHECKS */
      /* Both Dest and Src are aligned */
      if (swap)
      {
        while (--wlen)
        {
          *dest++ = ((uint16)(*src << 8)) | ((uint16)(*src >> 8));
          src++;
        }
      }
      else
      {
        while (--wlen)
        {
          *dest++ = *src++;
        }
      }
#ifndef FEATURE_MDSP_DISABLE_MEMCPY_CHECKS
      break;

    case 1:
      /* Src is misaligned */
      if (swap)
      {
        while (--wlen) {
          tword = (((uint16) ((uint8*)src)[0]) << 8) |
                  ((uint16) ((uint8*)src)[1]);
          *dest++ = tword;
          src++;
        }
      }
      else
      {
        while (--wlen) {
          tword = (((uint16) ((uint8*)src)[1]) << 8) |
                  ((uint16) ((uint8*)src)[0]);
          *dest++ = tword;
          src++;
        }
      }
      break;

    case 2:
      /* Dest is misaligned */
      if (swap)
      {
        while (--wlen)
        {
          tword = *src++;
          ((uint8*)dest)[0] = (uint8)(tword >> 8);
          ((uint8*)dest)[1] = (uint8)tword;
          dest++;
        }
      }
      else
      {
        while (--wlen)
        {
          tword = *src++;
          ((uint8*)dest)[0] = (uint8)tword;
          ((uint8*)dest)[1] = (uint8)(tword >> 8);
          dest++;
        }
      }
      break;

    case 3: /* Both misaligned */
    default:
      break;
  }
#endif /* FEATURE_MDSP_DISABLE_MEMCPY_CHECKS */
} /* mdsp_memcpy */

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_MEMCPY32

DESCRIPTION
  Copies a block of 32-bit values to or from the QDSP/ARM shared memory area.

DEPENDENCIES
  The correct QDSP clock regime must be enabled before accessing memory.
  Assumes that both the dest and src pointers are non-NULL to reduce the
  execution time.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void mdsp_memcpy32 (
  uint32 *dest,  /* Destination address                         */
  uint32 *src,   /* Source address                              */
  uint32 len,    /* Length of data to copy, in dwords           */
  boolean swap   /* Boolean to swap when copying to destination */
)
{
#ifndef FEATURE_MDSP_DISABLE_MEMCPY_CHECKS
  uint32  tdword;
#endif /* FEATURE_MDSP_DISABLE_MEMCPY_CHECKS */
  uint16 *src16;

  /* Prepare for loops */
  len++;

#ifndef FEATURE_MDSP_DISABLE_MEMCPY_CHECKS
  switch ( ((uint32)src & 1) | (((uint32)dest & 1) << 1) )
  {
    case 0:
#endif /* FEATURE_MDSP_DISABLE_MEMCPY_CHECKS */
      /* Dest and Src are aligned */
      if (swap)
      {
        src16 = (uint16*)src;
        while (--len)
        {
          *dest++ = ((uint32)((*src16 << 8) | (*src16 >> 8)) << 16) |
                    ( ( (*(src16+1) << 8) | (*(src16+1) >> 8) ) & 0xFFFF );
          src16+=2;
        }
      }
      else
      {
        while (--len)
        {
          *dest++ = *src++;
        }
      }
#ifndef FEATURE_MDSP_DISABLE_MEMCPY_CHECKS
      break;

    case 1:
      /* Src is misaligned */
      if (swap)
      {
        while (--len)
        {
          tdword = (((uint32) ((uint8*)src)[0]) << 24) |
                   (((uint32) ((uint8*)src)[1]) << 16) |
                   (((uint32) ((uint8*)src)[2]) << 8)  |
                   ((uint32) ((uint8*)src)[3]);
          *dest++ = tdword;
          src++;
        }
      }
      else
      {
        while (--len)
        {
          tdword = (((uint32) ((uint8*)src)[3]) << 24) |
                   (((uint32) ((uint8*)src)[2]) << 16) |
                   (((uint32) ((uint8*)src)[1]) << 8)  |
                   ((uint32) ((uint8*)src)[0]);
          *dest++ = tdword;
          src++;
        }
      }
      break;

    case 2:
      /* Dest is misaligned */
      if (swap)
      {
        while (--len)
        {
          tdword = *src++;
          ((uint8*)dest)[0] = (uint8)(tdword >> 24);
          ((uint8*)dest)[1] = (uint8)(tdword >> 16);
          ((uint8*)dest)[2] = (uint8)(tdword >> 8);
          ((uint8*)dest)[3] = (uint8)tdword;
          dest++;
        }
      }
      else
      {
        while (--len)
        {
          tdword = *src++;
          ((uint8*)dest)[0] = (uint8)tdword;
          ((uint8*)dest)[1] = (uint8)(tdword >> 8);
          ((uint8*)dest)[2] = (uint8)(tdword >> 16);
          ((uint8*)dest)[3] = (uint8)(tdword >> 24);
          dest++;
        }
      }
      break;

    default:
      break;
  }
#endif /* FEATURE_MDSP_DISABLE_MEMCPY_CHECKS */
} /* mdsp_memcpy32 */

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
void mdsp_register_app (
  mdsp_app_type      app_id,    /* Application ID                            */
  mdsp_isr_func_type *isrs_ptr, /* Pointer to a table of ISRs for the App    */
  mdsp_event_cb_type cb_ptr,    /* Application's event callback function     */
  uint16 event_mask  /* Mask of events that the application needs to receive */
)
{
  /* Verification status. FALSE => tests failed - issue a failure callback. */
  boolean           status = FALSE;
  mdsp_packet_type  *packet;

  if ( app_id < MDSP_APP_MAX )
  {
    packet = mdsp_get_packet();

    if ( packet != NULL )
    {
      packet->hdr.cmd                     = MDSP_CMD_REGISTER_APP;
      packet->register_app_cmd.app_id     = app_id;
      packet->register_app_cmd.cb_ptr     = cb_ptr;
      packet->register_app_cmd.event_mask = event_mask;

#ifdef MDSP_INTERRUPTS_USED
      packet->register_app_cmd.isrs_ptr   = isrs_ptr;
#endif

      mdsp_cmd( packet );
      status = TRUE;
    }
    /* else ran out of packets */
  }
  else
  {
    /* Can't issue failure callback, since the app_id is invalid. */
    ERR_FATAL("Invalid MDSP AppId %d", app_id, 0, 0);
  }

  if ( status == FALSE && cb_ptr != NULL )
  {
    /* Can't use mdsp_send_cmd_event() here, since the app. is unregistered. */
    mdsp_ev_cmd_type cmd_event;
    cmd_event.event      = MDSP_EV_COMMAND;
    cmd_event.app_id     = app_id;
    cmd_event.cmd_status = MDSP_CMD_FAIL;

    cb_ptr((mdsp_event_data_type*)&cmd_event);
  }
} /* mdsp_register_app */

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
void mdsp_register_isr (
  mdsp_app_type            app_id,  /* Application ID                      */
  mdsp_isr_func_type       isr_ptr, /* ISR function pointer                */
  mdsp_isr_type            isr_type /* Interrupt to register with          */
)
{
  /* Verification status. FALSE => tests failed - issue a failure callback. */
  boolean           status = FALSE;
  mdsp_packet_type  *packet;

  if ( app_id < MDSP_APP_MAX )
  {
    if ( isr_type < MDSP_ISR_MAX )
    {
      packet = mdsp_get_packet();

      if ( packet != NULL )
      {
        packet->hdr.cmd                     = MDSP_CMD_REGISTER_ISR;
        packet->register_isr_cmd.app_id     = app_id;
        packet->register_isr_cmd.isr_ptr    = isr_ptr;
        packet->register_isr_cmd.isr_type   = isr_type;

        mdsp_cmd( packet );
        status = TRUE;
      }
      /* else ran out of packets */
    }
    else
    {
      MSG_HIGH("Invalid ISR type %d", (int)isr_type, 0, 0);
    }
  }
  else
  {
    /* Can't issue failure callback, since the app_id is invalid. */
    ERR_FATAL("Invalid MDSP AppId %d", app_id, 0, 0);
  }

  if ( status == FALSE  )
  {
    /* Issue a Event callback to indicate failure */
    mdsp_send_cmd_event(app_id, MDSP_CMD_FAIL);
  }

} /* mdsp_register_isr */
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
void mdsp_enable (
  /* Application ID */
  mdsp_app_type app_id,
  /* The module to enable */
  mdsp_module_type module
)
{
  mdsp_packet_type  *packet;
  mdsp_command_status_type cmd_status = MDSP_CMD_FAIL;

  if ( app_id < MDSP_APP_MAX )
  {
    if( module < MDSP_MODULE_MAX )
    {
      packet = mdsp_get_packet();

      if ( packet != NULL )
      {
        packet->hdr.cmd           = MDSP_CMD_ENABLE;
        packet->enable_cmd.app_id = app_id;
        packet->enable_cmd.module = module;

        MSG_HIGH("mdsp_enable(%d, %d)", app_id, module, 0);
        mdsp_cmd( packet );
        cmd_status = MDSP_CMD_ACCEPT;
      }
      /* else ran out of packets */
    }
    else
    {
      MSG_HIGH("Invalid module %d", module, 0, 0);
    }
  }
  else
  {
    /* Can't issue failure callback, since the app_id is invalid. */
    ERR_FATAL("Invalid MDSP AppId %d", app_id, 0, 0);
  }

  if (cmd_status == MDSP_CMD_FAIL)
  {
    /* Send failure indication right away. */
    mdsp_send_cmd_event(app_id, cmd_status);
  }

} /* mdsp_enable */

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
void mdsp_disable (
  /* Application ID */
  mdsp_app_type app_id
)
{
  mdsp_packet_type  *packet;

  if ( app_id < MDSP_APP_MAX )
  {
    packet = mdsp_get_packet();

    if ( packet != NULL )
    {
      packet->hdr.cmd            = MDSP_CMD_DISABLE;
      packet->disable_cmd.app_id = app_id;

      MSG_HIGH("mdsp_disable(%d)", app_id, 0, 0);
      mdsp_cmd( packet );
    }
    else
    {
      /* Ran out of command packets. */
      mdsp_send_cmd_event(app_id, MDSP_CMD_FAIL);
    }
  }
  else
  {
    /* Can't issue failure callback, since the app_id is invalid. */
    ERR_FATAL("Invalid MDSP AppId %d", app_id, 0, 0);
  }
} /* mdsp_disable */

/* <EJECT> */
/*===========================================================================

FUNCTION MDSP_RESET_AND_ENABLE

DESCRIPTION
  Applications use this function to reset the DSP and enable a module.
  This is useful when the client wants to ensure that MDSP services actually
  does a reset and redownload of the image, before it is enabled. In the
  normal scenario, the DSP is not reset between successive calls to
  mdsp_enable() and mdsp_disable(), if the image has not changed.

  NOTE: This interface must used only as a debugging aid, since resetting the
  DSP may cause other MDSP Services clients to react adversely.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A MDSP_EV_COMMAND event will issued to the calling application once the
  command has completed.

===========================================================================*/
void mdsp_reset_and_enable (
  /* Application ID */
  mdsp_app_type app_id,
  /* The module to enable */
  mdsp_module_type module
)
{
  mdsp_packet_type  *packet;
  mdsp_command_status_type cmd_status = MDSP_CMD_FAIL;

  if ( app_id < MDSP_APP_MAX )
  {
    if( module < MDSP_MODULE_MAX )
    {
      packet = mdsp_get_packet();

      if ( packet != NULL )
      {
        packet->hdr.cmd = MDSP_CMD_RESET_AND_ENABLE;
        packet->reset_and_enable_cmd.app_id = app_id;
        packet->reset_and_enable_cmd.module = module;

        MSG_HIGH("mdsp_reset_and_enable(%d, %d)", app_id, module, 0);
        mdsp_cmd( packet );
        cmd_status = MDSP_CMD_ACCEPT;
      }
      /* else ran out of packets */
    }
    else
    {
      MSG_HIGH("Invalid module %d", module, 0, 0);
    }
  }
  else
  {
    /* Can't issue failure callback, since the app_id is invalid. */
    ERR_FATAL("Invalid MDSP AppId %d", app_id, 0, 0);
  }

  if (cmd_status == MDSP_CMD_FAIL)
  {
    /* Send failure indication right away. */
    mdsp_send_cmd_event(app_id, cmd_status);
  }

} /* mdsp_reset_and_enable */

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
void mdsp_disable_event_rsp (
  /* Application ID */
  mdsp_app_type app_id
)
{
  mdsp_packet_type  *packet;

  if ( app_id < MDSP_APP_MAX )
  {
    packet = mdsp_get_packet();

    if ( packet != NULL )
    {
      packet->hdr.cmd = MDSP_CMD_DISABLE_EVENT_RSP;
      packet->disable_event_rsp_cmd.app_id = app_id;

      MSG_HIGH("mdsp_disable_event_rsp(%d)", app_id, 0, 0);
      mdsp_cmd( packet );
    }
    else
    {
      /* Ran out of command packets. */
      mdsp_send_cmd_event(app_id, MDSP_CMD_FAIL);
    }
  }
  else
  {
    /* Can't issue failure callback, since the app_id is invalid. */
    ERR_FATAL("Invalid MDSP AppId %d", app_id, 0, 0);
  }
} /* mdsp_disable_event_rsp */

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
)
{
  mdsp_command_status_type cmd_status = MDSP_CMD_FAIL;

  if ( app_id < MDSP_APP_MAX )
  {
    if ( mdsp_app_table[app_id].registered == TRUE )
    {
      /* Critical section - synchronous call to manipulate clock! */
      INTLOCK();

      if ( mdsp_app_table[app_id].state == MDSP_APP_STATE_ENABLED )
      {
        mdsp_clocks_disable( app_id );
        cmd_status = MDSP_CMD_ACCEPT;
      }
      else
      {
        MSG_ERROR( "mdsp_dsp_sleep failed: App not enabled id=%d", app_id, 0,0);
      }

      INTFREE();

      mdsp_send_cmd_event(app_id, cmd_status);
    }
    else
    {
      MSG_ERROR( "mdsp_dsp_sleep failed: Unregistered app_id = %d", app_id,0,0);
    }
  }
  else
  {
    /* Can't issue failure callback, since the app_id is invalid. */
    ERR_FATAL("Invalid MDSP AppId %d", app_id, 0, 0);
  }
} /* mdsp_dsp_sleep */

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
)
{
  mdsp_command_status_type cmd_status = MDSP_CMD_FAIL;

  if ( app_id < MDSP_APP_MAX )
  {
    if ( mdsp_app_table[app_id].registered == TRUE )
    {
      /* Critical section - synchronous call to manipulate clock! */
      INTLOCK();

      if ( mdsp_app_table[app_id].state == MDSP_APP_STATE_ENABLED )
      {
        mdsp_clocks_enable( app_id );
        cmd_status = MDSP_CMD_ACCEPT;
      }
      else
      {
        MSG_ERROR( "mdsp_dsp_wakeup failed: App not enabled id=%d", app_id,0,0);
      }

      INTFREE();

      mdsp_send_cmd_event(app_id, cmd_status);
    }
    else
    {
      MSG_ERROR( "mdsp_dsp_wakeup failed: Unregistered app_id = %d", app_id,0,0);
    }
  }
  else
  {
    /* Can't issue failure callback, since the app_id is invalid. */
    ERR_FATAL("Invalid MDSP AppId %d", app_id, 0, 0);
  }
} /* mdsp_dsp_wakeup */



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
)
{
  mdsp_packet_type *packet;

  if ( app_id < MDSP_APP_MAX )
  {
    packet = mdsp_get_packet();

    if ( packet != NULL )
    {
      packet->hdr.cmd                     = MDSP_CMD_LOG_SET_MODE;
      packet->log_set_mode_cmd.app_id     = app_id;
      packet->log_set_mode_cmd.log_mode   = log_mode;
      packet->log_set_mode_cmd.log_period = log_period;

      mdsp_cmd( packet );
    }
    else
    {
      /* Ran out of command packets. */
      mdsp_send_cmd_event(app_id, MDSP_CMD_FAIL);
    }
  }
  else
  {
    /* Can't issue failure callback, since the app_id is invalid. */
    ERR_FATAL("Invalid MDSP AppId %d", app_id, 0, 0);
  }
}


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
)
{
  mdsp_packet_type *packet;

  if ( app_id < MDSP_APP_MAX )
  {
    packet = mdsp_get_packet();

    if ( packet != NULL )
    {
      packet->hdr.cmd             = MDSP_CMD_LOG_SEND;
      packet->log_send_cmd.app_id = app_id;

      mdsp_cmd( packet );
    }
    else
    {
      /* Ran out of command packets. */
      mdsp_send_cmd_event(app_id, MDSP_CMD_FAIL);
    }
  }
  else
  {
    /* Can't issue failure callback, since the app_id is invalid. */
    ERR_FATAL("Invalid MDSP AppId %d", app_id, 0, 0);
  }
}


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
)
{
  mdsp_packet_type *packet;

  /* Validate app_id, note that we can request a save internally */
  if ( app_id < MDSP_APP_MAX || app_id == MDSP_APP_SELF )
  {
    packet = mdsp_get_packet();

    if ( packet != NULL )
    {
      packet->hdr.cmd             = MDSP_CMD_LOG_SAVE;
      packet->log_save_cmd.app_id = app_id;
      packet->log_save_cmd.tag    = tag;

      mdsp_cmd( packet );
    }
    else
    {
      /* Ran out of command packets. */
      mdsp_send_cmd_event(app_id, MDSP_CMD_FAIL);
    }
  }
  else
  {
    /* Can't issue failure callback, since the app_id is invalid. */
    ERR_FATAL("Invalid MDSP AppId %d", app_id, 0, 0);
  }
}


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
)
{
  mdsp_packet_type *packet;

  /* Validate app_id, note that we can request a save internally */
  if ( app_id < MDSP_APP_MAX || app_id == MDSP_APP_SELF )
  {
    packet = mdsp_get_packet();

    if ( packet != NULL )
    {
      packet->hdr.cmd                = MDSP_CMD_MEMORY_SAVE;
      packet->memory_save_cmd.app_id = app_id;
      packet->memory_save_cmd.tag    = tag;

      mdsp_cmd( packet );
    }
    else
    {
      /* Ran out of command packets. */
      mdsp_send_cmd_event(app_id, MDSP_CMD_FAIL);
    }
  }
  else
  {
    /* Can't issue failure callback, since the app_id is invalid. */
    ERR_FATAL("Invalid MDSP AppId %d", app_id, 0, 0);
  }
}


#endif /* FEATURE_MDSPSVC_LOG */

