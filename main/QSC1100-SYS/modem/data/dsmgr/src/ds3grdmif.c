/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       3 G   D A T A   S E R V I C E S

                R D M   I N T E R F A C E   F U N C T I O N S

GENERAL DESCRIPTION
  This software unit contains functions for interfacing to the Runtime Device
  Mapper (RDM).

EXTERNALIZED FUNCTIONS
  ds3gi_handle_rdm_open_cmd()
    Processes the DS_RDM_OPEN_CMD

  ds3gi_handle_rdm_close_cmd()
    Processes the DS_RDM_CLOSE_CMD

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The open and close callback functions must be registered with RDM before
  Data Services can use RDM's port mapping services.


  Copyright (c) 2001 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3grdmif.c_v   1.1   23 Aug 2002 19:12:30   akhare  $
  $Header: //source/qcom/qct/modem/data/common/dsmgr/main/lite/src/ds3grdmif.c#1 $ $DateTime: 2007/11/05 02:12:42 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/31/01   smp     When a port can't be opened successfully, notify RDM that
                   the open failed (via RDM_APP_BUSY_S).
06/05/01   smp     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

#include "msg.h"
#include "err.h"
#include "rex.h"
#include "task.h"
#include "rdevmap.h"
#include "dstask.h"
#include "ds3gmgrint.h"
#include "ds3gsiolib.h"
#include "ds3grdmif.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/


/*===========================================================================

FUNCTION DS3GI_RDM_OPEN_CB

DESCRIPTION
  This function is called by RDM to tell DS to open the specified port. If DS
  already has a port open, it must first close that port and then open the
  specified port.

  This function simply sends a command to the DS task, the actual open (and
  prior close, if necessary) is done in the DS task context.

  This function must be registered with RDM, before DS can use the RDM port
  mapping services.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3gi_rdm_open_cb
(
  sio_port_id_type  port_id            /* Port id of the port to be opened */
)
{
  ds_cmd_type  *cmd_ptr;                             /* Pointer to command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    Get a command buffer from the DS task.
  -------------------------------------------------------------------------*/
  if( (cmd_ptr = ds_get_cmd_buf()) != NULL )
  {
    /*-----------------------------------------------------------------------
      Fill in the parameters, and send the command to the DS task.
    -----------------------------------------------------------------------*/
    cmd_ptr->hdr.cmd_id = DS_RDM_OPEN_CMD;
    cmd_ptr->cmd.rdm_open.port_id = port_id;

    ds_put_cmd( cmd_ptr );
  }
  else
  {
    /*-----------------------------------------------------------------------
      No free command buffers available. Log an error, and notify RDM that
      the open failed.
    -----------------------------------------------------------------------*/
    ERR( "Can't get cmd buf from DS task: %d", DS_RDM_OPEN_CMD, 0, 0 );

    rdm_notify( RDM_DATA_SRVC, RDM_APP_BUSY_S );
  }

} /* ds3gi_rdm_open_cb() */


/*===========================================================================

FUNCTION DS3GI_RDM_CLOSE_CB

DESCRIPTION
  This function is called by RDM to tell DS to close the specified port.

  This function simply sends a command to the DS task, the actual close is
  done in the DS task context.

  This function must be registered with RDM, before DS can use the RDM port
  mapping services.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3gi_rdm_close_cb( void )
{
  ds_cmd_type  *cmd_ptr;                             /* Pointer to command */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    Get a command buffer from the DS task.
  -------------------------------------------------------------------------*/
  if( (cmd_ptr = ds_get_cmd_buf()) != NULL )
  {
    /*-----------------------------------------------------------------------
      Fill in the parameters, and send the command to the DS task.
    -----------------------------------------------------------------------*/
    cmd_ptr->hdr.cmd_id = DS_RDM_CLOSE_CMD;

    ds_put_cmd( cmd_ptr );
  }
  else
  {
    /*-----------------------------------------------------------------------
      No free command buffers available. Log an error, and notify RDM that
      the close failed.
    -----------------------------------------------------------------------*/
    ERR( "Can't get cmd buf from DS task: %d", DS_RDM_CLOSE_CMD, 0, 0 );

    rdm_notify( RDM_DATA_SRVC, RDM_APP_BUSY_S );
  }

} /* ds3gi_rdm_close_cb() */


/*===========================================================================

FUNCTION DS3GI_HANDLE_RDM_OPEN_CMD

DESCRIPTION
  This function processes the DS_RDM_OPEN_CMD, which is a command from RDM to
  open a serial port. If a port is already open, it is first closed, but only
  if no data call is active (or in progress), and then the port specified by
  RDM is opened. RDM is notified whether the open was successful or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Changes the serial state and the ATCoP state, if no serial port was
  previously open.

===========================================================================*/

void  ds3gi_handle_rdm_open_cmd
(
  ds_cmd_type  *cmd_ptr
)
{
  ds3g_siolib_status_e_type  open_status;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    Command from RDM to open a serial port. If we already have a port open,
    we must close it first and then open the port specified by RDM.
  -------------------------------------------------------------------------*/
  MSG_HIGH( "Rx'ed RDM_OPEN_CMD, port id: %d", cmd_ptr->cmd.rdm_open.port_id,
        0, 0 );

  if ( ds3g_siolib_state.call_coming_up )
  {
    /*-----------------------------------------------------------------------
      We are busy, so don't close the port that is already open. *** This
      logic will need to change when support for socket calls is added. ***
    -----------------------------------------------------------------------*/
    ERR( "RDM_OPEN_CMD ignored, DS is busy", 0, 0, 0 );
    rdm_notify( RDM_DATA_SRVC, RDM_APP_BUSY_S );
  }
  else
  {
    /*-----------------------------------------------------------------------
      We are idle, ok to close the current port (if any).
    -----------------------------------------------------------------------*/
    if( ds3g_siolib_state.serial_info.serial_state != 
        DS3G_SIOLIB_NULL_SERIAL_STATE )
    {
      /*---------------------------------------------------------------------
    We have a port open, close it first.
      ---------------------------------------------------------------------*/
      ds3g_siolib_close();
    }
    
    /*-----------------------------------------------------------------------
      Now attempt to open the port specified by RDM.
    -----------------------------------------------------------------------*/
    open_status = ds3g_siolib_open( cmd_ptr->cmd.rdm_open.port_id,
                  ds3g_siolib_state.serial_info.rx_wm_ptr,
                  ds3g_siolib_state.serial_info.tx_wm_ptr );

    if( open_status == DS3G_SIOLIB_OK )
    {
      /*---------------------------------------------------------------------
    Notify RDM that the open was successful.
      ---------------------------------------------------------------------*/
      rdm_notify( RDM_DATA_SRVC, RDM_DONE_S );

      /*---------------------------------------------------------------------
    Set the serial state to Autodetect and ATCoP state to Command state.
      ---------------------------------------------------------------------*/
      ds3g_siolib_state.serial_info.serial_state = DS3G_SIOLIB_AUTODETECT_SERIAL_STATE;
      ds3g_siolib_state.at_state = DS3G_SIOLIB_COMMAND_AT_STATE;
    }
    else
    {
      /*---------------------------------------------------------------------
    Notify RDM that the open was unsuccessful.
      ---------------------------------------------------------------------*/
      rdm_notify( RDM_DATA_SRVC, RDM_APP_BUSY_S );
    }
  }

} /* ds3gi_handle_rdm_open_cmd() */


/*===========================================================================

FUNCTION DS3GI_HANDLE_RDM_CLOSE_CMD

DESCRIPTION
  This function processes the DS_RDM_CLOSE_CMD, which is a command from RDM
  to close a serial port. The port is closed only if no data call is active
  (or in progress). RDM is notified whether the close was successful or not.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Changes the serial state and ATCoP state, if the close is actually
  performed.

===========================================================================*/

void  ds3gi_handle_rdm_close_cmd( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    Command from RDM to close a serial port.
  -------------------------------------------------------------------------*/
  MSG_HIGH( "Rx'ed RDM_CLOSE_CMD", 0, 0, 0 );

  if ( ds3g_siolib_state.call_coming_up )
  {
    /*-----------------------------------------------------------------------
      We are busy, so don't close the port. Notify RDM that the close was
      unsuccessful. *** This logic will need to change when support for
      socket calls is added. ***
    -----------------------------------------------------------------------*/
    ERR( "RDM_CLOSE_CMD ignored, DS is busy", 0, 0, 0 );
    rdm_notify( RDM_DATA_SRVC, RDM_APP_BUSY_S );
  }
  else
  {
    /*-----------------------------------------------------------------------
      We are idle, ok to close the port. Notify RDM that the close is done.
    -----------------------------------------------------------------------*/
    ds3g_siolib_close();
    rdm_notify( RDM_DATA_SRVC, RDM_DONE_S );

    /*-----------------------------------------------------------------------
      Set the serial and ATCoP states to Null, and discard any data that is
      sitting in the Autodetect watermark.
    -----------------------------------------------------------------------*/
    ds3g_siolib_state.serial_info.serial_state = 
        DS3G_SIOLIB_NULL_SERIAL_STATE;
    ds3g_siolib_state.at_state = DS3G_SIOLIB_NULL_AT_STATE;

    ds3g_siolib_cleanup_autodetect_rx_watermark();

    /*-----------------------------------------------------------------------
      Clear SIOLIB callback function table.
    -----------------------------------------------------------------------*/
    ds3g_siolib_deregister_callback_func(ds3g_siolib_state.func_tbl);
  }

} /* ds3gi_handle_rdm_close_cmd() */


#endif /* FEATURE_DATA */
