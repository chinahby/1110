#ifndef DS3GCMIF_H
#define DS3GCMIF_H
/*===========================================================================

               3 G   D A T A   S E R V I C E S   M A N A G E R

           C A L L   M G R   I N T E R F A C E   F U N C T I O N S

                            H E A D E R    F I L E

DESCRIPTION
  This file contains functions for interfacing to Call Manager, including
  Data Services client initialization and processing of call events. This
  header file is intended to be used internal to the 3G Dsmgr module only.


  Copyright (c) 2001-2008 by QUALCOMM Incorporated. All Rights Reserved.  
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3gcmif.h_v   1.1   28 Feb 2003 18:53:34   rchar  $
  $Header: //source/qcom/qct/modem/data/common/dsmgr/main/lite/src/ds3gcmif.h#2 $ $DateTime: 2008/01/31 05:31:22 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/28/08   sn      Featurized WCDMA code.
04/09/03   rsl     Added ds3gi_process_cm_ret_val().
02/26/03   rc      Changed ds3gi_process_cm_ss_sys_mode_chg_cmd() to
                   ds3gi_process_cm_ss_srv_chg_cmd().
05/21/02   smp     Added prototype for ds3gi_process_cm_ss_sys_mode_chg_cmd().
04/19/02   vsk     Added PDP context preservation support
04/17/02   rc      Changed DS3G_CALL_TYPE_CKT and DS3G_CALL_TYPE PKT to 
                   DS_CALL_TYPE_CKT and DS_CALL_TYPE_PKT.
01/11/02   smp     Merged from packet data branch. Added macros to convert
                   from CM call type to DS call type and vice versa.
07/03/01   smp     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

#include "cm.h"
#include "dstask.h"

#define DS3G_GET_CM_CALL_TYPE( ds_call_type ) \
( (ds_call_type == DS_CALL_TYPE_CKT) ? CM_CALL_TYPE_CS_DATA : CM_CALL_TYPE_PS_DATA )

#define DS3G_GET_DS_CALL_TYPE( cm_call_type ) \
( (cm_call_type == CM_CALL_TYPE_CS_DATA) ? DS_CALL_TYPE_CKT : DS_CALL_TYPE_PKT )


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION DS3GI_CM_INIT

DESCRIPTION
  This function initializes the Data Services Call Manager client, and
  registers for call events of interest, namely:
    CM_CALL_EVENT_END
    CM_CALL_EVENT_INCOM
    CM_CALL_EVENT_CONNECT
    CM_CALL_EVENT_SETUP_IND (only applicable for GSM/WCDMA)
    CM_CALL_EVENT_CALL_CONF (only applicable for GSM/WCDMA)

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void  ds3gi_cm_init( void );


/*===========================================================================

FUNCTION DS3GI_CM_CALL_CMD_CB

DESCRIPTION
  Callback function invoked by Call Manager to notify 3G Dsmgr of the
  completion status of the call command previously issued by 3G Dsmgr.
  
  This callback function may be invoked in relation to the following call
  commands: cm_call_cmd_answer(), cm_call_cmd_orig(), cm_call_cmd_end(),
  cm_call_cmd_setup_res().

  In each case, if the command was successful, no action is taken. If the
  cm_call_cmd_orig() was unsuccessful, a DS_CM_CALL_ENDED_CMD is sent to
  the DS task. Errors in all other call commands do not require any action.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void  ds3gi_cm_call_cmd_cb
(
  void                   *data_block_ptr, /* Data blk provided in call cmd */
  cm_call_cmd_e_type      call_cmd,                     /* Call command id */
  cm_call_cmd_err_e_type  call_cmd_err          /* Call command error code */
);


/*===========================================================================

FUNCTION DS3GI_PROCESS_CM_CALL_END_CMD

DESCRIPTION
  This function processes the DS_CM_CALL_END_CMD from Call Manager. This
  command is either a confirmation of a release initiated by 3G Dsmgr, or a
  notification of call release by the network. Note that if a data call is
  released from the UI, this will appear as a network call release.

DEPENDENCIES
  The DS_CM_CALL_END_CMD command is valid in any call state, except IDLE. It
  is ignored if the call state is IDLE, or if the specified call id is not
  recognized.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void  ds3gi_process_cm_call_end_cmd
(
  ds_cmd_type  *cmd_ptr                          /* Pointer to the command */
);


/*===========================================================================

FUNCTION DS3GI_PROCESS_CM_CALL_INCOM_CMD

DESCRIPTION
  This function processes the DS_CM_CALL_INCOM_CMD from Call Manager. It
  calls the mode-specific incoming_call_handler, and then either answers the
  incoming call, rings the serial port or rejects the incoming call (i.e.
  does nothing), depending on the return value from the handler function.

  If an incoming call is received in the Orig call state, the outgoing call
  is released, and the incoming call is given precedence.

DEPENDENCIES
  The DS_CM_CALL_INCOM_CMD command is valid only in the IDLE or ORIG call
  states, and is ignored in all other call states.

RETURN VALUE
  None

SIDE EFFECTS
  Overwrites the call id and call type (in Idle or Orig call state only).
  Updates the call_state, serial_state, at_state and may change the serial
  mode to Rawdata or Packet.

===========================================================================*/

extern void  ds3gi_process_cm_call_incom_cmd
(
  ds_cmd_type  *cmd_ptr                          /* Pointer to the command */
);


/*===========================================================================

FUNCTION DS3GI_PROCESS_CM_CALL_CONNECTED_CMD

DESCRIPTION
  This function processes the DS_CM_CALL_CONNECTED_CMD from Call Manager.
  This command signifies that either an outgoing or an incoming call is
  connected, and end-to-end data transfer may proceed.

  This function invokes the mode-specific call_connected_handler, and changes
  the serial mode to Rawdata or Packet, if directed to do so by the mode-
  specific handler. Note that the serial mode may have been changed earlier,
  either before the origination or when the call was answered.

DEPENDENCIES
  The DS_CM_CALL_CONNECTED_CMD command is valid only in the ORIG and WAIT_
  FOR_CONNECT call states, and is ignored in all other call states. This
  command is also ignored if the call id is not recognized.

RETURN VALUE
  None

SIDE EFFECTS
  Updates the call_state. May also update the serial_state, and change the
  serial mode to Rawdata or Packet.

===========================================================================*/

extern void  ds3gi_process_cm_call_connected_cmd
(
  ds_cmd_type  *cmd_ptr                          /* Pointer to the command */
);


/*===========================================================================

FUNCTION DS3GI_PROCESS_CM_CALL_SETUP_CMD

DESCRIPTION
  This function processes the DS_CM_CALL_SETUP_CMD from Call Manager. It
  simply calls the mode-specific setup_handler, which validates the setup
  parameters, and either accepts or rejects the call.

  If the call is accepted, the mode-specific handler may specify new setup
  parameters which are sent to Call Manager in the setup response.

  No state change is involved for 3G Dsmgr.

DEPENDENCIES
  The DS_CM_CALL_SETUP_CMD is valid only in the IDLE or ORIG call states, and
  is ignored in all other call states.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void  ds3gi_process_cm_call_setup_cmd
(
  ds_cmd_type  *cmd_ptr                          /* Pointer to the command */
);


/*===========================================================================

FUNCTION DS3GI_PROCESS_CM_CALL_CONF_CMD

DESCRIPTION
  This function processes the DS_CM_CALL_CONF_CMD received from Call Manager.
  It simply calls the mode-specific call_conf_handler.

  No state change is involved for 3G Dsmgr.

DEPENDENCIES
  The DS_CM_CALL_CONF_CMD is valid only in the ORIG call state, and is
  ignored in all other call states. Its is also ignored if the call id is not
  recognized.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void  ds3gi_process_cm_call_conf_cmd
(
  ds_cmd_type  *cmd_ptr                          /* Pointer to the command */
);


#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */


/*===========================================================================

FUNCTION DS3GI_PROCESS_CM_SS_SRV_CHG_CMD

DESCRIPTION
  This function processes the DS_CM_SS_SRV_CHG_CMD from Call Manager.
  
  This function checks the suspend state (ps_data_suspend)from CM and does  
  the following:
  TRUE  : Calls the mode specific data suspend handler.
  FALSE : Calls the mode specific data suspend handler.
  
  This function also calls a function to set the new mode.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void  ds3gi_process_cm_ss_srv_chg_cmd
(
  ds_cmd_type  *cmd_ptr                          /* Pointer to the command */
);

/*===========================================================================

FUNCTION DS3GI_PROCESS_CM_RET_VAL

DESCRIPTION
  This function processes the return value from Call Manager functions.
  
  If an cm_mm_call_cmd_answer fails, calls force_call_down_hdlr which fills
  up the end_params and then calls cm_mm_call_cmd_end.
   
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void  ds3gi_process_cm_ret_val
(
  cm_call_id_type            call_id    /* call id    */
);
#endif /* FEATURE_DATA */

#endif /* DS3GCMIF_H */

