#ifndef QDSP_H
#define QDSP_H
/*===========================================================================

           Q D S P    S E R V I C E S    H E A D E R    F I L E

DESCRIPTION
  This file contains declarations associated with the QDSP Services module.

REFERENCES
  None

Copyright (c) 2000 - 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/multimedia/core/qdspservices/6k/ver1/qdspsvcs/main/latest/src/qdsp.h#5 $ $DateTime: 2008/11/19 04:16:22 $ $Author: shilpaj $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/09/08   knm     Added Cache Flush & Cache Invalidate macros
04/18/07   act     Added QDSP_APP_JPEGE and QDSP_APP_JPEGD.
01/08/07    sg     Added QDSP_APP_CAMERA.
12/01/06   act     Added QDSP_APP_Q3DIMENSION.
03/27/06    sg     Lowered priority of APP_CRYPT since it interferes with
                   other concurrencies. Also, Crypto will still be able to
                   operate using ARM-based algorithms in case the DSP engine is
                   not available to it.
08/24/05   vlc     Added QDSP_APP_CAMERA_PROXY to qdsp_app_type to support in 
                   call hand off while camera is in operation.
11/19/04    sg     Merged module-based QDSP services from 00.00.13.01. Added
                   support for concurrent images.
03/05/04    sg     Added APP_VT for video telephone. Increased MODMATH priority.
10/09/03    sg     Removed APP id feature-ization to allow OEMs to change their
                   feature set, without having to recompile their libraries.
08/19/03    sg     Added high priority vocoder clientID for ACP.
01/30/03    st     Modified code to enable QDSP_APP_IQCAPTURE for
                   FEATURE_FACTORY_TESTMODE.
12/03/02    st     Added num_apps parameter to qdsp_ev_image_type.
06/11/02    st     Modified to support FEATURE_HWTC.
04/27/02    sg     Modified to allow the specification of zero interrupts in
                   the qdspext configuration file. Moved interrupt definition
                   to qdspext files.
03/21/02    st     Added QDSP_APP_ICG and QDSP_APP_IM2CAL clients to 
                   qdsp_app_type.
02/21/02    sm     Added qdsp_reset() function.
02/13/01    sm     Added support for crypt and modmath app IDs.  Moved SRCH
                   appID just below GPS.
02/05/01   vlc     Added MSM5100 support.
11/06/00    ro     Supports new image parser.  Access functions removed.
10/18/00    sm     Changed qdsp_peek/poke() to use int as length.
10/13/00   ymc     Added MSM5105 support; T_MSM33 as T_MSM33 && !T_MSM5105;
                   !T_MSM33 as !T_MSM33 || T_MSM5105.
10/04/00    sm     Added qdsp_block_read(), qdsp_block_write().
                   Added qdsp_ev_type and app_id to all events.
08/28/00    ro     Unit integration.
08/17/00   spf     Initial revision.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                           INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"              /* Target specific definitions           */
#include "comdef.h"              /* Definitions for byte, word, etc.      */
#include "qdspext.h"             /* QDSP firmware images and access       */

#ifdef CACHE_POLICY_WRITE_BACK
#include "cache_mmu.h"           /* Cache MMU APIs*/
#include "armmmu.h"
#endif /* CACHE_POLICY_WRITE_BACK */

/* Definitions for cache flush and cache invalidate APIs */
#ifdef CACHE_POLICY_WRITE_BACK
#define INVALIDATE_DATA_CACHE(ptr,size) mmu_dcache_inval_region((void *)ptr,(uint32)size)
#define FLUSH_DATA_CACHE(ptr,size)      mmu_dcache_flush_region((void *)ptr,(uint32)size)
#else
#define INVALIDATE_DATA_CACHE(ptr,size) mmu_invalidate_data_cache_lines((uint32 *)ptr,(uint32)size)
#define FLUSH_DATA_CACHE(prt,size)
#endif /* CACHE_POLICY_WRITE_BACK */

/* <EJECT> */
/*===========================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed to interface with the QDSP Services module.

===========================================================================*/

#define FEATURE_QDSP_USES_MODULES

/* Application identifiers - highest priority app first */
typedef enum
{
  QDSP_APP_IM2CAL,    /* IM2CAL Services           - highest priority */
  QDSP_APP_VOC_HIGH,  /* High priority Vocoder Services client        */
  QDSP_APP_VT,        /* Video Telephony client                       */
  QDSP_APP_MODMATH,
  QDSP_APP_IGG,       /* Image, graphics                              */
  QDSP_APP_CAMERA,    /* Camera drivers                               */
  QDSP_APP_JPEGE,     /* JPEGE driver                                 */
  QDSP_APP_JPEGD,     /* JPEGD driver                                 */
  QDSP_APP_DTV,       /* Digital TV application                       */
  QDSP_APP_Q3DIMENSION, /* 3D graphics                                */
  QDSP_APP_VOC,       /* Vocoder Services                             */
  /* APP_CAMERA_PROXY is the equivalent of APP_CAMERA, used by Qcamera
   * while the phone is in a voice call.  This allows voice image swap 
   * in case a hand off requires it.
   */
  QDSP_APP_CAMERA_PROXY, 
  QDSP_APP_CRYPT,
#if defined(FEATURE_HWTC) || defined (FEATURE_FACTORY_TESTMODE)
  QDSP_APP_IQCAPTURE, /* iq capture application                       */
#endif /* FEATURE_HWTC || FEATURE_FACTORY_TESTMODE */
  QDSP_APP_MAX,

  /* For Internal QDSP Services Use Only! It is used to represent QDSPS
   * when setting the clock mask.
   */
  QDSP_APP_SELF,

  /* DO NOT USE: Force this enum to be a 32bit type to improve speed  */
  QDSP_APP_32BIT_DUMMY = 0X10000
} qdsp_app_type;

/* --------------------------------------------------------------------------
                         QDSP Event Definitions
----------------------------------------------------------------------------*/

/* QDSP events (bitmasks) */
typedef enum
{
  QDSP_EV_COMMAND       = 1,   /* Command status */
  QDSP_EV_MODULE        = 2,   /* Module status  */
  QDSP_EV_IMAGE_CHANGE  = 4
} qdsp_event_type;

/* Command status values */
typedef enum
{
  QDSP_CMD_ACCEPT,     /* Command was accepted */
  QDSP_CMD_FAIL        /* Command failed       */
} qdsp_command_status_type;

/* Basic event type */
typedef struct
{
  qdsp_event_type event;
  qdsp_app_type   app_id;
} qdsp_ev_type;

/* Command Status Event info */
typedef struct
{
  qdsp_event_type          event;
  qdsp_app_type            app_id;
  qdsp_command_status_type cmd_status;
} qdsp_ev_cmd_type;

/* Module status values */
typedef enum
{
  QDSP_MOD_READY,          /* Module is running and ready */
  QDSP_MOD_DISABLE         /* Module is not available     */
} qdsp_mod_status_type;

/* Used to provide additional information about a module event */
typedef union
{
  /* MOD_READY info */
  struct
  {
    qdsp_image_type image; /* Image that is used to enable the module */
  } ready;

  /* MOD_DISABLE info: unused */
  struct
  {
    int dummy;
  } disable;

} qdsp_ev_mod_info_type;

/* Module Event info */
typedef struct
{
  qdsp_event_type       event;
  qdsp_app_type         app_id;
  qdsp_module_type      module;
  qdsp_mod_status_type  mod_status;
  qdsp_ev_mod_info_type info;
} qdsp_ev_mod_type;

typedef struct
{
  qdsp_event_type       event;
  qdsp_app_type         app_id;
  qdsp_image_type       image;
  uint32                num_higher_priority_apps;
} qdsp_ev_image_change_type;

/* QDSP event data type */
typedef union
{
  qdsp_event_type              event;
  qdsp_ev_type                 app;
  qdsp_ev_cmd_type             cmd;
  qdsp_ev_mod_type             mod;
  qdsp_ev_image_change_type    img_chg;
} qdsp_event_data_type;

/* Event callback function */
typedef void (*qdsp_event_cb_type) (qdsp_event_data_type* event_ptr);

/* <EJECT> */
/*===========================================================================

                        PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION QDSP_PEEK

DESCRIPTION
  Reads a block of data from the QDSP/ARM shared memory area.  This function
  will activate the clocks if they are not active so it can be called at any
  time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_peek (
  void*    src_ptr,    /* The start address of the QDSP memory block to read */
  void*    dst_ptr,    /* The address of the ARM destination buffer          */
  uint32   byte_length /* The length in bytes of the memory block            */
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_POKE

DESCRIPTION
  Writes a block of data to the QDSP/ARM shared memory area.  This function
  will activate the QDSP clocks if they are not active so it can be called 
  at any time.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_poke (
  void*  src_ptr,    /* The address of the source buffer to write to the QDSP */
  void*  dst_ptr,    /* The start address of the QDSP memory block to write   */
  uint32 byte_length /* The length in bytes of the memory block               */
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_REGISTER_APP

DESCRIPTION
  Applications use this function to register interrupt handler functions,
  event callback function and event mask. Once registered, applications will
  begin receiving all events in their event mask. The application's
  interrupt handlers will only be called when the application has enabled the
  QDSP by calling qdsp_enable().

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
  command has completed.

===========================================================================*/
extern void qdsp_register_app (
  qdsp_app_type      app_id,    /* Application ID                            */
  qdsp_isr_func_type *isrs_ptr, /* Pointer to a table of ISRs for the App    */
  qdsp_event_cb_type cb_ptr,    /* Application's event callback function     */
  uint16 event_mask /* Mask of events that the application needs to receive  */
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_REGISTER_ISR

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
  command has completed.

===========================================================================*/
extern void qdsp_register_isr (
  qdsp_app_type            app_id,  /* Application ID                         */
  qdsp_isr_func_type       isr_ptr, /* Function to call when interrupt occurs */
  qdsp_isr_type            isr_type /* Interrupt identifier                   */
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_ENABLE

DESCRIPTION
  Applications use this function to indicate which QDSP module should be
  enabled. When the calling application's priority is high enough,
  QDSP Services will issue a callback to the application, indicating that it
  has been enabled. When the "MOD_READY" event is received, the QDSP clocks are
  on, the current firmware image is running and the requested module is ready
  for use on the QDSP.

  When QDSP-to-ARM interrupts occur, QDSP Services will call the registered
  interrupt handlers (highest priority first) of each application is currently
  enabled.

DEPENDENCIES
  It is important that applications call qdsp_disable() when they finish using
  the QDSP.  If the QDSP is left enabled, excessive power comsumption will
  result.

RETURN VALUE
  None

SIDE EFFECTS
  A QDSP_EV_COMMAND event will issued to the calling application immediately
  after the command is accepted. Note however, that this does not mean that
  the requested module has been enabled. The client application _must_ wait
  to receive the QDSP_MOD_READY event.

===========================================================================*/
extern void qdsp_enable (
  qdsp_app_type    app_id,  /* Application ID       */
  qdsp_module_type module   /* The module to enable */
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_DISABLE

DESCRIPTION
  Applications use this function to disable the QDSP.  QDSP Services disables
  the QDSP when no application require it to be enabled.  When the QDSP is
  disabled, its clocks are off and the current firmware image is not running.
  The QDSP draws minimum current when it is disabled.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A QDSP_EV_COMMAND event will issued to the calling application once the
  command has completed.

===========================================================================*/
extern void qdsp_disable (
  qdsp_app_type app_id /* Application ID */
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_RESET_AND_ENABLE

DESCRIPTION
  Applications use this function to reset the DSP and enable a module.
  This is useful when the client wants to ensure that QDSP services actually
  does a reset and redownload of the image, before it is enabled. In the
  normal scenario, the DSP is not reset between successive calls to
  qdsp_enable() and qdsp_disable(), if the image has not changed.

  IMPORTANT: This interface must used only as a debugging aid, since resetting
  the DSP may cause other QDSP Services clients to react adversely.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  A QDSP_EV_COMMAND event will issued to the calling application once the
  command has completed.

===========================================================================*/
extern void qdsp_reset_and_enable (
  qdsp_app_type    app_id,  /* Application ID       */
  qdsp_module_type module   /* The module to enable */
);

/* <EJECT> */
/*===========================================================================

FUNCTION QDSP_DISABLE_EVENT_RSP

DESCRIPTION
  Applications should call this function in response to a QDSP_EV_MODULE event
  with QDSP_MOD_DISABLE status. QDSP Services issues this event whenever the
  client application's use of the QDSP will be preempted temporarily. Once the
  client receives this event, it may possibly save any context information and
  stop using the DSP. It should then call this function to indicate to QDSP
  Services that it is ready to be disabled.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void qdsp_disable_event_rsp (
  qdsp_app_type app_id /* Application ID */
);

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
extern void qdsp_task (
  dword ignored /* Parameter received from Main Control task - ignored */
);

/* Undocumented internal API, subject to change: DO NOT USE! */
extern boolean qdsp_module_currently_available ( qdsp_module_type module );
/*===========================================================================

FUNCTION qdsp_fatal_error_core_dump

DESCRIPTION
  Get a QDSP MEMA, MEMB , MEMC and REGISTER memory dumps in case of ERROR_FATAL.

DEPENDENCIES


RETURN VALUE
  void
===========================================================================*/
extern void qdsp_fatal_error_core_dump( void );
#endif /* QDSP_H */
