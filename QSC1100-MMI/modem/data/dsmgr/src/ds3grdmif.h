#ifndef DS3GRDMIF_H
#define DS3GRDMIF_H
/*===========================================================================

               3 G   D A T A   S E R V I C E S   M A N A G E R

                R D M   I N T E R F A C E   F U N C T I O N S

	                    H E A D E R   F I L E

DESCRIPTION
  This file contains functions for interfacing to the Runtime Device Mapper,
  including open and close callback functions, and functions for handling
  commands received from RDM. This header file is intended to be used
  internal to the 3G Dsmgr module only.


  Copyright (c) 2001 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3grdmif.h_v   1.0   08 Aug 2002 11:19:48   akhare  $
  $Header: //source/qcom/qct/modem/data/common/dsmgr/main/lite/src/ds3grdmif.h#1 $ $DateTime: 2007/11/05 02:12:42 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/01   smp     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

#include "sio.h"
#include "dstask.h"


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

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

extern void  ds3gi_rdm_open_cb
(
  sio_port_id_type  port_id            /* Port id of the port to be opened */
);


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

extern void  ds3gi_rdm_close_cb( void );


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

extern void  ds3gi_handle_rdm_open_cmd
(
  ds_cmd_type  *cmd_ptr                          /* Pointer to the command */
);


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

extern void  ds3gi_handle_rdm_close_cmd( void );


#endif /* FEATURE_DATA */

#endif /* DS3GRDMIF_H */
