#ifndef DS3GMSIF_H
#define DS3GMSIF_H
/*===========================================================================

               3 G   D A T A   S E R V I C E S   M A N A G E R

                 M O D E - S P E C I F I C   S U B - T A S K  
       
                   I N T E R F A C E   F U N C T I O N S

                            H E A D E R    F I L E

DESCRIPTION
  This file contains functions for interfacing to mode-specific sub-tasks.
  This header file is intended to be used internal to the 3G Dsmgr module only.


  Copyright (c) 2002-2008 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3gmshif.h_v   1.7   28 Feb 2003 18:55:28   rchar  $
  $Header: //source/qcom/qct/modem/data/common/dsmgr/main/lite/inc/ds3gmshif.h#1 $ $DateTime: 2008/04/11 12:46:53 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/28/08   sn      Featurized WCDMA/GSM code.
02/26/03   rc      Added function ds3g_msh_ps_data_available_ind() to inform
                   CM that new data is available on the uplink.
12/10/02   tmr     Added ds3g_msh_is_call_connected() function
10/13/02   ak      Added function to set pkt state.  only for single-call
                   systems/JCDMA
10/10/02   tmr     Removed cmd_ptr parameter from ds3g_msh_complete_ll_connect()
10/03/02   rc      Changed prototype for ds3g_msh_call_rab_reestab_req().
08/19/02   ak      Updated for multiple calls.
04/19/02   vsk     Added PDP context preservation support
04/17/02   rc      Added prototype for ds3gi_process_initiate_call_cmd().
02/26/02   tmr     Initial revision

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

#include "dstask.h"


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION DS3G_MSH_IS_CALL_CONNECTED

DESCRIPTION
  This function returns whether or not the call identified by the input 
  call ID is in the connected state or not.


DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the call_id is valid and the call is in the connected state,
  FALSE otherwise

SIDE EFFECTS
  None.

===========================================================================*/
boolean  
ds3g_msh_is_call_connected
(
  cm_call_id_type call_id
);


/*===========================================================================

FUNCTION DS3G_MSH_COMPLETE_LL_CONNECT

DESCRIPTION
  This function processes the DS_COMPLETE_LL_CONNECT_CMD from lower
  layers when a connection has been delayed.  This is currently used by
  the GCSD protocol stack to support the +DR command which reports what
  type of compression is used in an NT data call. Compression is 
  negotiated at the RLP layer, and the +DR data must be reported by ATCOP prior
  to the CONNECT message, therefore the CONNECT is delayed while the RLP
  negotiation occurs.

  This function invokes the mode-specific complete_ll_connect_handler, and changes
  the serial mode to Rawdata or Packet, if directed to do so by the mode-
  specific handler. Note that the serial mode may have been changed earlier,
  either before the origination or when the call was answered.

DEPENDENCIES
  The DS_COMPLETE_LL_CONNECT_CMD command is valid only in the 
  DS3G_WAIT_FOR_LL_CONNECT_STATE call states, and is ignored in all other
  call states.

RETURN VALUE
  DS3G_SUCCESS if okay, else DS3G_FAILURE.

SIDE EFFECTS
  Updates the call_state. May also update the serial_state, and change the
  serial mode to Rawdata or Packet.

===========================================================================*/

boolean  ds3g_msh_complete_ll_connect
(
  cm_call_id_type         call_id
);


/*===========================================================================

FUNCTION DS3G_MSH_COMPLETE_LL_DISCONNECT

DESCRIPTION
  This function processes the DS_COMPLETE_LL_DISCONNECT_CMD from lower
  layers when a disconnect has been delayed.  This is currently used by
  the GCSD protocol stack for FAX calls so that any queued data can be 
  sent prior to a disconnect occuring.  This is necessary for FAX protocols

  This function invokes the mode-specific complete_ll_disconnect_handler, reports
  the disconnect to CM and outputs the AT response

DEPENDENCIES
  The DS_COMPLETE_LL_DISCONNECT_CMD command is valid only in the
  DS3G_WAIT_FOR_LL_DISCONNECT_STATE call state, and is ignored in all other
  call states.

RETURN VALUE
  DS3G_SUCCESS if okay, else DS3G_FAILURE.

SIDE EFFECTS
  Updates the call_state.

===========================================================================*/

boolean  ds3g_msh_complete_ll_disconnect
(
  cm_call_id_type   call_id
);

#ifdef FEATURE_WCDMA
#error code not present
#endif /* FEATURE_WCDMA */


/*===========================================================================

FUNCTION DS3G_MSH_INITIATE_CALL

DESCRIPTION
  This function originates a data call of the specified call type. It gets a
  call id from Call Manager, calls the mode-specific originate_call_handler,
  changes the serial mode (if requested to do so by the mode-specific
  handler), and then sends a command to Call Manager to originate a call.

  This function should be invoked by ATCoP when the dial command "ATD" (or
  equivalent) is received.

DEPENDENCIES
  The call will not be originated if any of these conditions is true:
  (a) The call state is not IDLE
  (b) No mode-specific handler table is registered for the current mode and
      call type
  (c) No originate_call_handler is registered for the current mode and call
      type (since this handler must specify the orig_params to be sent to
      Call Mgr)
  (d) A valid call id could not be obtained from Call Manager (this could
      occur if the phone is in use, offline etc.)

RETURN VALUE  TRUE - call was originated with CM.  
              FALSE - call could not be originated.

SIDE EFFECTS  Updates 3G Dsmgr state variables and may also change the
              serial mode, if the origination is successful.
===========================================================================*/
extern boolean ds3g_msh_initiate_call
(
  ds_call_type_e_type  call_type,     /* Call type - Circuit or Packet     */
  sys_sys_mode_e_type  sys_mode,      /* WCDMA, GSM, CDMA, or other?       */
  const byte          *dial_string,   /* Null-terminated dial string       */
  boolean              digit_mode,    /* 1 = dial str has non-digit chars  */
                                      /* 0 = dial str has only digits      */
  void                *user_info_ptr  /* points to user supplied info      */
);


/*===========================================================================

FUNCTION DS3G_MSH_ANSWER_CALL

DESCRIPTION
  This function answers an incoming data call. It calls the mode-specific
  user_answer_handler, changes the serial mode (if requested to do so by the
  mode-specific handler), and then sends a command to Call Manager to answer
  the call.
  
  This function should be invoked by ATCoP when the answer command "ATA" is
  received.

DEPENDENCIES
  The call will not be answered if any of these conditions is true:
  (a) The call state is not RINGING
  (b) No mode-specific handler table is registered for the current mode and
      call type
  (c) No user_answer_handler is registered for the current mode and call
      type (since this handler must specify the ans_params to be sent to
      Call Mgr)

RETURN VALUE
  DS3G_SUCCESS if okay, else DS3G_FAILURE.

SIDE EFFECTS
  Updates 3G Dsmgr state variables and may also change the serial mode, if
  the call is answered.

===========================================================================*/

extern boolean ds3g_msh_answer_call
(
  cm_call_id_type      cm_call_id
);


/*===========================================================================
FUNCTION      DS3G_MSH_HANGUP_CALL

DESCRIPTION   This function ends the currently active data call (if any).
              It calls the mode-specific user_end_handler, to allow any
              protocols to be shut down if necessary, and then sends a
              command to Call Manager to end the call.

DEPENDENCIES  No action is taken if no call is active i.e. the call state
              is IDLE.

RETURN VALUE  DS3G_SUCCESS if CM is told to end call.  
              DS3G_FAILURE if could not end call.

SIDE EFFECTS  Updates 3G Dsmgr state variables if a call was active.
===========================================================================*/
boolean ds3g_msh_hangup_call
(
  cm_call_id_type      call_id,       /* call id of call to end            */
  void                *user_info_ptr  /* points to user supplied info      */
);



#if (defined(FEATURE_GSM) || defined(FEATURE_WCDMA))
#error code not present
#endif /* FEATURE_GSM || FEATURE_WCDMA */

/*===========================================================================
FUNCTION      DS3G_MSH_SET_PKT_STATE

DESCRIPTION   This function is appropriate for one-call systems only.  It 
              allows the mode-specific handler to tell CM the state of its
              packet connection.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None
===========================================================================*/
void ds3g_msh_set_pkt_state
(
  cm_packet_state_e_type    pkt_state
);

#endif /* FEATURE_DATA */

#endif /* DS3GMSIF_H */

