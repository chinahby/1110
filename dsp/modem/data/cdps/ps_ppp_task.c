/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                          P S _ P P P _ T A S K . C

GENERAL DESCRIPTION
  This is the file that defines all of the PPP functions that are tied to
  executing PPP in some task context.

EXTERNALIZED FUNCTIONS
  ppp_task_powerup_init()
    The fucntion that must be called from the executing tasks powerup
    initialization function to initialize PPP and register all of the
    callbacks etc...

Copyright (c) 2002-2008 QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_task.c_v   1.6   31 Jan 2003 18:47:34   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ppp_task.c#2 $ $DateTime: 2008/06/27 15:37:54 $ $Author: praveenp $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/28/08    pp     Fixed RVCT compiler warnings.
04/18/03    vsk    enode the right dev & prot fields in 
                   ppp_task_process_inject_pkt()
04/07/03    usb    Added PPP_INJECT_PACKET_CMD processing to replay an out 
                   of order PPP packet for PPP receive processing.
01/27/03    usb    Added PPP_SET_MODE_CMD processing to set PPP mode in PS 
                   context only 
09/23/02    mvl    Modified signal usage.
09/12/02    mvl    removed unused variable. Removed netmodel featurization.
09/02/02    mvl    Removed the PPP rx callback, and registration as this is
                   done in the start command.
08/13/02    mvl    Session handle support.
07/25/02    mvl    Updates for PPP renaming.
07/24/02    mvl    Now ensuring that is PPP mode is NONE discard before
                   sending to any of the PPP unframing functions or input.
07/11/02    mvl    Updates for new PPP architecture.
07/01/02    mvl    Make the file more independent of PS via MACROs defined in
                   header file
04/01/02    mvl    Created Module
===========================================================================*/



/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                          INCLUDE FILES FOR MODULE

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
#include "customer.h"
#include "comdef.h"
/*---------------------------------------------------------------------------
  The following include is the task include that specifies the support
  functions PPP uses to set itself up as a service executing in that task
---------------------------------------------------------------------------*/
#include "ps_svc.h"
#include "ps_ppp_defs.h"

/*---------------------------------------------------------------------------
  The general includes
---------------------------------------------------------------------------*/
#include "assert.h"
#include "dsm.h"
#include "ps_ppp_task.h"
#include "ps_ppp_fsm.h"
#include "ps_pppi.h"

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                         INTERNAL DATA DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            FORWARD DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
void ppp_task_process_start
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);

void ppp_task_process_stop
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);

void ppp_task_process_abort
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);

void ppp_task_process_set_mode
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);

void ppp_task_process_inject_pkt
(
  ps_cmd_enum_type cmd,
  void *data_ptr
);


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

                            EXTERNAL DECLARATIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

/*===========================================================================
FUNCTION PPP_TASK_POWERUP_INIT()

DESCRIPTION
  This function does the powerup initialization for the PPP module.  This
  includes registering handlers with PS (the task executing it) for the PPP
  RX signals.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_powerup_init( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    register the handlers for the various PPP commands that must be handled
    in the task context.
  -------------------------------------------------------------------------*/
  ps_set_cmd_handler(PPP_START_CMD, ppp_task_process_start);
  ps_set_cmd_handler(PPP_STOP_CMD,  ppp_task_process_stop);
  ps_set_cmd_handler(PPP_ABORT_CMD, ppp_task_process_abort);
  ps_set_cmd_handler(PPP_SET_MODE_CMD, ppp_task_process_set_mode);
  ps_set_cmd_handler(PPP_INJECT_PKT_CMD, ppp_task_process_inject_pkt);

  /*-------------------------------------------------------------------------
    Call the PPP module initialization funtion
  -------------------------------------------------------------------------*/
  pppi_powerup_init();

} /* ppp_task_powerup_init() */



/*===========================================================================
FUNCTION PPP_TASK_PROCESS_START()

DESCRIPTION
  This function handles the ppp_start command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_start
(
  ps_cmd_enum_type cmd,
  void *data_ptr
)
{
  pppi_start_info_type start_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cmd != PPP_START_CMD)
  {
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Get the PPP command information and call pppi_start()
  -------------------------------------------------------------------------*/
  memcpy((void*)&start_info, &data_ptr, sizeof(start_info));

  pppi_start(start_info);

} /* ppp_task_process_start() */



/*===========================================================================
FUNCTION PPP_TASK_PROCESS_ABORT()

DESCRIPTION
  This function handles the ppp_abort command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_abort
(
  ps_cmd_enum_type cmd,
  void *data_ptr
)
{
  ppp_dev_enum_type device;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    sanity checking
  -------------------------------------------------------------------------*/
  if(cmd != PPP_ABORT_CMD)
  {
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Get the PPP command information and call pppi_abort()
  -------------------------------------------------------------------------*/
  device = (ppp_dev_enum_type)((uint32)data_ptr);

  pppi_abort(device);

} /* ppp_task_process_abort() */



/*===========================================================================
FUNCTION PPP_TASK_PROCESS_STOP()

DESCRIPTION
  This function handles the ppp_close command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_stop
(
  ps_cmd_enum_type cmd,
  void *data_ptr
)
{
  ppp_dev_enum_type device;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    sanity checking
  -------------------------------------------------------------------------*/
  if(cmd != PPP_STOP_CMD)
  {
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Get the PPP command information and call pppi_abort()
  -------------------------------------------------------------------------*/
  device = (ppp_dev_enum_type)((uint32)data_ptr);

  pppi_stop(device);

} /* ppp_task_process_stop() */


/*===========================================================================
FUNCTION PPP_TASK_PROCESS_SET_MODE()

DESCRIPTION
  This function handles the ppp_start command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_set_mode
(
  ps_cmd_enum_type cmd,
  void *data_ptr
)
{
  pppi_mode_info_type mode_info;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(cmd != PPP_SET_MODE_CMD)
  {
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Get the PPP command information and call pppi_set_mode()
  -------------------------------------------------------------------------*/
  memcpy((void*)&mode_info, &data_ptr, sizeof(mode_info));

  pppi_set_mode(mode_info);

} /* ppp_task_process_set_mode() */


/*===========================================================================
FUNCTION PPP_TASK_PROCESS_INJECT_PKT()

DESCRIPTION
  This function handles the ppp_inject_pkt command.

PARAMETERS
  cmd_ptr: the pointer to the command data structure that was sent.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void ppp_task_process_inject_pkt
(
  ps_cmd_enum_type cmd,
  void *data_ptr
)
{
  ppp_dev_enum_type device;
  uint16 protocol;
  dsm_item_type *pkt_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    sanity checking
  -------------------------------------------------------------------------*/
  if(cmd != PPP_INJECT_PKT_CMD)
  {
    ASSERT(0);
    return;
  }

  /*-------------------------------------------------------------------------
    Get the PPP command information and call pppi_receive()
  -------------------------------------------------------------------------*/
  pkt_ptr = (dsm_item_type*) data_ptr;
  device = (ppp_dev_enum_type)(pkt_ptr->app_field & 0xFFFF);
  protocol = (pkt_ptr->app_field >> 16) & 0xFFFF;

  pppi_receive(device, protocol, &pkt_ptr);

} /* ppp_task_process_inject_pkt() */

