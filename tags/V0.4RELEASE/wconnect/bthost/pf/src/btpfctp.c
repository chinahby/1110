/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

B L U E T O O T H    C O R D L E S S   T E L E P H O N Y   P R O F I L E

GENERAL DESCRIPTION
  This module contains the Bluetooth Cordless Telephony Profile (CTP)
  interface (commands and events) implementation.

Copyright (c) 2004-2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
*
*   #9       21 May 2007          SSK
* Fix compiler warnings by typecasting BT types to OI types and vice-versa.
*
*   #8       24 Oct 2005          DSN
* Removing superflous header file inclusing.
*
*   #7       04 Mar 2005          JTL
* Making copy to OI bluetooth address be a common function.
*
*   #6       07 Nov 2004          JTL
* Clear internal app ID when deregistering. Remove extraneous white space.
*
*   #4-5     06 Sep 2004          JTL
* Update code to work with new TCS driver. Still need work on new events
* and new handling of bearer information.
*
*   #3       10 Aug 2004          JTL
* Make ourselves not connectable when in a call. Use the right app id, and
* don't use an appid after deregistering.
*
*   #2       06 Aug 2004          JTL
* Rearranging code. Fixing bugs. Code is now mostly working.
*
*   #1       22 Jul 2004          JTL
* Initial revision
*
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF_CTP

#include "bt.h"
#include "btpfi.h"
#include "btpf.h"
#include "btpfcmdi.h"
#include "bti.h"
#include "btmsg.h"
#include "btpfcs.h"
#include "btsd.h"
#include "snd.h"
#include "msmaud.h"
#include "qdspext.h"
#include "qdspcmd.h"
#include "voc.h"

/* Open interface headers */
#include "oi_tcs.h"
#include "oi_ctp.h"
#include "oi_sdpconfig.h"
#include "oi_dataelem.h"

/* Open interface code: */
#include "tcs_cc.c"
#include "tcs_helper.c"
#include "tcs_coding.c"
#include "tcs_msg.c"
#include "tcs_debug.c"

#undef __OI_MODULE__
#include "ctp.c"
#include "ctp_tl.c"

#undef __OI_MODULE__
#include "intercom.c"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#define BT_MSG_LAYER  BT_MSG_GN

#define BT_PF_TCS_VERSION_NUMBER 0x0100
#define QDSP_WAIT_CMD()    while( qdsp_read( QDSP_uPCommandReg ) != 0x0000 )

/*-------------------------------------------------------------------------*/
/* Check command status of bt driver commands                              */
/*-------------------------------------------------------------------------*/
#define BT_CMD(cmd,status) if( ((status) = (cmd)) == BT_CS_GN_PENDING){(status)=OI_OK;}

/*-------------------------------------------------------------------------*/
/*  These app id's are the ID's of the apps using CTP and ICP              */
/*-------------------------------------------------------------------------*/
LOCAL bt_app_id_type bt_pf_ctp_ext_app_id  = BT_APP_ID_NULL;
LOCAL bt_app_id_type bt_pf_icp_ext_app_id  = BT_APP_ID_NULL;

/*-------------------------------------------------------------------------*/
/*  This is the App ID for internal use by the CTP/ICP driver to call      */
/*  lower layer functions                                                  */
/*-------------------------------------------------------------------------*/
LOCAL bt_app_id_type bt_pf_ctp_int_app_id = BT_APP_ID_NULL;
LOCAL bt_app_id_type bt_pf_icp_int_app_id = BT_APP_ID_NULL;

static const OI_SDPDB_ATTRIBUTE strAttrs[] = {
  { BASE_ENGLISH(OI_ATTRID_ServiceName),  OI_ELEMENT_STRING("Cordless telephony") } };

LOCAL void ctp_connect_ind( OI_BD_ADDR *addr );
LOCAL void ctp_connect_cfm( OI_BD_ADDR *addr, OI_STATUS result );
LOCAL void ctp_disconnect_cfm( OI_BD_ADDR *addr );
LOCAL void ctp_establish_ind( OI_BD_ADDR *addr, OI_TCS_CONNECT_INFO *info );
LOCAL void ctp_establish_cfm( OI_BD_ADDR *addr, OI_STATUS result );
LOCAL void ctp_information_ind( OI_BD_ADDR *addr,
                                OI_BOOL sendingComplete,
                                OI_CHAR keypadFacility,
                                OI_TCS_NUMBER *callingNumber,
                                OI_TCS_NUMBER *calledNumber,
                                OI_TCS_AUDIO_CONTROL audioControl,
                                OI_UINT8 *companySpecific,
                                OI_UINT8 companySpecificSize );
LOCAL void ctp_dtmf_start( OI_BD_ADDR *addr, OI_CHAR tone );
LOCAL void ctp_dtmf_start_ack( OI_BD_ADDR *addr,
                               OI_CHAR tone );
LOCAL void ctp_dtmf_start_reject( OI_BD_ADDR *addr,
                                  OI_TCS_CAUSE cause );
LOCAL void ctp_dtmf_stop( OI_BD_ADDR *addr );
LOCAL void ctp_dtmf_stop_ack( OI_BD_ADDR *addr,
                              OI_CHAR tone);
LOCAL void ctp_release_ind( OI_BD_ADDR *addr, OI_TCS_CAUSE cause );
LOCAL void ctp_outgoing_status( OI_BD_ADDR *addr,
                                OI_TCS_CALL_STATUS status,
                                OI_UINT8 *companySpecific,
                                OI_UINT8 companySpecificSize);
LOCAL void ctp_call_data_ind( OI_BD_ADDR *addr, OI_BYTE *data, OI_UINT16 dataLen );
LOCAL void ctp_cl_info_ind( OI_BD_ADDR *addr,
                            OI_TCS_AUDIO_CONTROL audioControl,
                            OI_UINT8 *companySpecific,
                            OI_UINT8 companySpecificSize );

LOCAL const OI_TCS_CALLBACKS ctp_callbacks = {
  ctp_connect_ind,
  ctp_connect_cfm,
  ctp_disconnect_cfm,
  ctp_establish_cfm,
  ctp_establish_ind,
  ctp_information_ind,
  ctp_release_ind,
  ctp_dtmf_start,
  ctp_dtmf_start_ack,
  ctp_dtmf_start_reject,
  ctp_dtmf_stop,
  ctp_dtmf_stop_ack,
  ctp_outgoing_status,
  ctp_call_data_ind,
  ctp_cl_info_ind
};


LOCAL void icp_connect_ind( OI_BD_ADDR *addr );
LOCAL void icp_connect_cfm( OI_BD_ADDR *addr, OI_STATUS result );
LOCAL void icp_disconnect_cfm( OI_BD_ADDR *addr );
LOCAL void icp_establish_ind( OI_BD_ADDR *addr, OI_TCS_CONNECT_INFO *info );
LOCAL void icp_establish_cfm( OI_BD_ADDR *addr, OI_STATUS result );
LOCAL void icp_information_ind( OI_BD_ADDR *addr,
                                OI_BOOL sendingComplete,
                                OI_CHAR keypadFacility,
                                OI_TCS_NUMBER *callingNumber,
                                OI_TCS_NUMBER *calledNumber,
                                OI_TCS_AUDIO_CONTROL audioControl,
                                OI_UINT8 *companySpecific,
                                OI_UINT8 companySpecificSize);
LOCAL void icp_dtmf_start( OI_BD_ADDR *addr, OI_CHAR tone );
LOCAL void icp_dtmf_start_ack( OI_BD_ADDR *addr,
                               OI_CHAR tone );
LOCAL void icp_dtmf_start_reject( OI_BD_ADDR *addr,
                                  OI_TCS_CAUSE cause );
LOCAL void icp_dtmf_stop( OI_BD_ADDR *addr );
LOCAL void icp_dtmf_stop_ack( OI_BD_ADDR *addr,
                              OI_CHAR tone);
LOCAL void icp_release_ind( OI_BD_ADDR *addr, OI_TCS_CAUSE cause );
LOCAL void icp_outgoing_status( OI_BD_ADDR *addr,
                                OI_TCS_CALL_STATUS status,
                                OI_UINT8 *companySpecific,
                                OI_UINT8 companySpecificSize );
LOCAL void icp_call_data_ind( OI_BD_ADDR *addr, OI_BYTE *data, OI_UINT16 dataLen );
LOCAL void icp_cl_info_ind( OI_BD_ADDR *addr,
                            OI_TCS_AUDIO_CONTROL audioControl,
                            OI_UINT8 *companySpecific,
                            OI_UINT8 companySpecificSize );

LOCAL const OI_TCS_CALLBACKS icp_callbacks = {
  icp_connect_ind,
  icp_connect_cfm,
  icp_disconnect_cfm,
  icp_establish_cfm,
  icp_establish_ind,
  icp_information_ind,
  icp_release_ind,
  icp_dtmf_start,
  icp_dtmf_start_ack,
  icp_dtmf_start_reject,
  icp_dtmf_stop,
  icp_dtmf_stop_ack,
  icp_outgoing_status,
  icp_call_data_ind,
  icp_cl_info_ind
};

/*===========================================================================

                    EVENT CALLBACK FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================

FUNCTION
  bt_pf_icp_ev_cb

DESCRIPTION
  Event callback routine for ICP.

===========================================================================*/
void bt_pf_icp_ev_cb( struct bt_ev_msg_struct* ev_msg_ptr )
{
  switch (ev_msg_ptr->ev_hdr.ev_type)
  {
    case BT_EV_GN_CMD_DONE:
      BT_MSG_DEBUG( "BT PF ICP: Cmd Done, Status: %x, App: %x, Cmd: %x",
                    ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status,
                    ev_msg_ptr->ev_msg.ev_gn_cmd_done.bt_app_id,
                    ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_type );
      break;

    default:
      BT_MSG_DEBUG( "BT PF ICP: unknown event type %x",
              ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
  }
  return;
}

/*===========================================================================

FUNCTION
  bt_pf_ctp_ev_cb

DESCRIPTION
  Event callback routine for CTP.

===========================================================================*/
void bt_pf_ctp_ev_cb( struct bt_ev_msg_struct* ev_msg_ptr )
{
  switch (ev_msg_ptr->ev_hdr.ev_type)
  {
    case BT_EV_GN_CMD_DONE:
      BT_MSG_DEBUG( "BT PF CTP: Cmd Done, Status: %x, App: %x, Cmd: %x",
                    ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status,
                    ev_msg_ptr->ev_msg.ev_gn_cmd_done.bt_app_id,
                    ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_type );
      break;

    default:
      BT_MSG_DEBUG( "BT PF CTP: unknown event type %x",
              ev_msg_ptr->ev_hdr.ev_type, 0, 0 );
  }
  return;
}

/*===========================================================================

                    INTERNAL FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================

FUNCTION
  check_ctp_app_id

DESCRIPTION
  Insures that the associated app ID is registered with CTP.
  Returns BT_CS_GN_BAD_APP_ID or BT_CS_GN_SUCCESS.

===========================================================================*/
LOCAL bt_cmd_status_type check_ctp_app_id( bt_pf_cmd_msg_type* cmd_ptr )
{
  if( cmd_ptr->cmd_hdr.app_id != bt_pf_ctp_ext_app_id )
  {
    return BT_CS_GN_BAD_APP_ID;
  }
  return BT_CS_GN_SUCCESS;
}

/*===========================================================================

FUNCTION
  check_icp_app_id

DESCRIPTION
  Insures that the associated app ID is registered with ICP.
  Returns BT_CS_GN_BAD_APP_ID or BT_CS_GN_SUCCESS.

===========================================================================*/
LOCAL bt_cmd_status_type check_icp_app_id( bt_pf_cmd_msg_type* cmd_ptr )
{
  if( cmd_ptr->cmd_hdr.app_id != bt_pf_icp_ext_app_id )
  {
    return BT_CS_GN_BAD_APP_ID;
  }
  return BT_CS_GN_SUCCESS;
}

/*===========================================================================

FUNCTION
  copy_ctp_number

DESCRIPTION
  Helper function used to copy OI data from one structure to another.

===========================================================================*/
LOCAL void copy_ctp_number( bt_pf_ctp_number_type *dest, const OI_TCS_NUMBER *src )
{
  dest->calling = (boolean)src->calling;
  dest->type = (bt_pf_ctp_number_type_type)src->type;
  dest->plan = (bt_pf_ctp_number_plan_type)src->plan;
  dest->presentation = (bt_pf_ctp_presentation_type)src->presentation;
  dest->screening = (bt_pf_ctp_screening_type)src->screening;
  dest->num_digits = (char)src->numDigits;
  memcpy((void*)dest->digits, src->digits, MIN(dest->num_digits, BT_PF_CTP_MAX_DIGITS) );
}

/*===========================================================================

FUNCTION
  bt_pf_ctp_copy_to_tcs_number

DESCRIPTION
  Helper function used to copy OI data from one structure to another.

===========================================================================*/
LOCAL void bt_pf_ctp_copy_to_tcs_number( OI_TCS_NUMBER *dest,
                                         const bt_pf_ctp_number_type *src,
                                         OI_UINT8 *digits )
{
  dest->calling = (OI_BOOL)src->calling;
  dest->type = (OI_TCS_NUMBER_TYPE)src->type;
  dest->plan = (OI_TCS_NUMBER_PLAN)src->plan;
  dest->presentation = (OI_TCS_PRESENTATION)src->presentation;
  dest->screening = (OI_TCS_SCREENING)src->screening;
  dest->numDigits = (OI_UINT8)src->num_digits;

  dest->digits = (OI_UINT8*)digits;

  memcpy(digits, (void*)src->digits, MIN(dest->numDigits, BT_PF_CTP_MAX_DIGITS) );
}

/*===========================================================================

                    CTP CALLBACK FUNCTION DEFINITIONS
                       These functions are called by the OI implementation

===========================================================================*/

/*===========================================================================

FUNCTION
   ctp_connect_ind

DESCRIPTION
   Indicates that a remote device is attempting to connect.
===========================================================================*/
LOCAL void ctp_connect_ind( OI_BD_ADDR *addr )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_ctp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_CTP_CON_IND;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_ctp_connect_ind.bd_addr, addr);

  BT_MSG_DEBUG("BT PF CTP TX EV: connect ind. AppID: %d",
               bt_pf_ctp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
   ctp_connect_cfm

DESCRIPTION
   Confirms the connection to another device.
===========================================================================*/
LOCAL void ctp_connect_cfm( OI_BD_ADDR *addr, OI_STATUS result )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_ctp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_CTP_CON_CFM;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_ctp_connect_cfm.bd_addr, addr);
  event.ev_msg.ev_ctp_connect_cfm.status = result;

  BT_MSG_DEBUG("BT PF CTP TX EV: connect cfm. AppID: %d",
               bt_pf_ctp_ext_app_id,0,0);

  bt_ec_send_event( &event );

  /* We're connected, so we're no longer discoverable */
  bt_cmd_rm_set_connectable( bt_pf_ctp_int_app_id, FALSE,
                             BT_RM_AVP_AUTOMATIC );

}

/*===========================================================================

FUNCTION
   ctp_disconnect_cfm

DESCRIPTION
   Confirms a disconnection from another device.
===========================================================================*/
LOCAL void ctp_disconnect_cfm( OI_BD_ADDR *addr )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_ctp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_CTP_DCN_CFM;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_ctp_disconnect_cfm.bd_addr, addr);

  BT_MSG_DEBUG("BT PF CTP TX EV: disconnect cfm. AppID: %d",
               bt_pf_ctp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
   ctp_establish_ind

DESCRIPTION
   Indicates that a remote device is attempting to establish a
   call. If sendingComplete is TRUE, then the outgoing side has
   completed sending the called party number.
===========================================================================*/
LOCAL void ctp_establish_ind( OI_BD_ADDR *addr, OI_TCS_CONNECT_INFO *info )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_ctp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_CTP_EST_IND;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_ctp_establish_ind.bd_addr, addr);
  event.ev_msg.ev_ctp_establish_ind.info.call_class =
    (bt_pf_ctp_call_class_type)info->callClass;
  event.ev_msg.ev_ctp_establish_ind.info.sending_complete = info->sendingComplete;
  event.ev_msg.ev_ctp_establish_ind.info.signal =
    (bt_pf_ctp_signal_type)info->signal;
  copy_ctp_number( &event.ev_msg.ev_ctp_establish_ind.info.calling_number,
                   info->callingNumber );
  copy_ctp_number( &event.ev_msg.ev_ctp_establish_ind.info.called_number,
                   info->calledNumber );

  BT_MSG_DEBUG("BT PF CTP TX EV: est ind. AppID: %d",
               bt_pf_ctp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
   ctp_establish_cfm
DESCRIPTION
   Confirms establishment of an outgoing call.
===========================================================================*/
LOCAL void ctp_establish_cfm( OI_BD_ADDR *addr, OI_STATUS result )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_ctp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_CTP_EST_CFM;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_ctp_establish_cfm.bd_addr, addr);
  event.ev_msg.ev_ctp_establish_cfm.status = result;

  BT_MSG_DEBUG("BT PF CTP TX EV: est cfm. AppID: %d",
               bt_pf_ctp_ext_app_id,0,0);
  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
   ctp_information_ind

DESCRIPTION
   Indicates information has been received. If sendingComplete is
   TRUE, then the outgoing side has completed sending the called party
   number.
===========================================================================*/
LOCAL void ctp_information_ind( OI_BD_ADDR *addr,
                                OI_BOOL sendingComplete,
                                OI_CHAR keypadFacility,
                                OI_TCS_NUMBER *callingNumber,
                                OI_TCS_NUMBER *calledNumber,
                                OI_TCS_AUDIO_CONTROL audioControl,
                                OI_UINT8 *companySpecific,
                                OI_UINT8 companySpecificSize )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_ctp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_CTP_INFO_IND;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_ctp_info_ind.bd_addr, addr);
  event.ev_msg.ev_ctp_info_ind.sending_complete = (boolean)sendingComplete;
  event.ev_msg.ev_ctp_info_ind.keypad_facility = keypadFacility;
  event.ev_msg.ev_ctp_info_ind.audio_control =
    (bt_pf_ctp_audio_control_type)audioControl;
  copy_ctp_number( &event.ev_msg.ev_ctp_info_ind.calling_number,
                   callingNumber );
  copy_ctp_number( &event.ev_msg.ev_ctp_info_ind.called_number,
                   calledNumber );

  BT_MSG_DEBUG("BT PF CTP TX EV: info ind. AppID: %d",
               bt_pf_ctp_ext_app_id,0,0);
  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
   ctp_dtmf_start

DESCRIPTION
   Indicates that a DTMF tone should be started.
===========================================================================*/
LOCAL void ctp_dtmf_start( OI_BD_ADDR *addr, OI_CHAR tone )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_ctp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_CTP_DTMF_START;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_ctp_dtmf_start.bd_addr, addr);
  event.ev_msg.ev_ctp_dtmf_start.tone = tone;

  BT_MSG_DEBUG("BT PF CTP TX EV: dtmf start. AppID: %d",
               bt_pf_ctp_ext_app_id,0,0);
  bt_ec_send_event( &event );

  /* Ack the DTMF start here */
  /* JTL TODO: make this a new command */
  OI_TCS_StartDTMFAck( addr, tone );
}
/*===========================================================================

FUNCTION
   ctp_dtmf_start_ack

DESCRIPTION
   Indicates that a DTMF tone has been started
===========================================================================*/
LOCAL void ctp_dtmf_start_ack( OI_BD_ADDR *addr,
                               OI_CHAR tone )
{
  BT_MSG_DEBUG("BT PF CTP TX EV: dtmf start ack. AppID: %d",
               bt_pf_ctp_ext_app_id,0,0);
  // JTL TODO: Implement
}
/*===========================================================================

FUNCTION
   ctp_dtmf_start_reject

DESCRIPTION
   Indicates that a DTMF tone has been rejected
===========================================================================*/
LOCAL void ctp_dtmf_start_reject( OI_BD_ADDR *addr,
                                  OI_TCS_CAUSE cause )
{
  BT_MSG_DEBUG("BT PF CTP TX EV: dtmf start reject. AppID: %d",
               bt_pf_ctp_ext_app_id,0,0);
  // JTL TODO: Implement
}

/*===========================================================================

FUNCTION
   ctp_dtmf_stop
DESCRIPTION
   Indicates that a DTMF tone should be stopped.
===========================================================================*/
LOCAL void ctp_dtmf_stop( OI_BD_ADDR *addr )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_ctp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_CTP_DTMF_STOP;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_ctp_dtmf_stop.bd_addr, addr);

  BT_MSG_DEBUG("BT PF CTP TX EV: dtmf stop. AppID: %d",
               bt_pf_ctp_ext_app_id,0,0);
  bt_ec_send_event( &event );

  /* Ack the DTMF start here */
  /* JTL TODO: make this a new command */
  OI_TCS_StopDTMFAck( addr, 0 );
}

/*===========================================================================

FUNCTION
   ctp_dtmf_stop_ack
DESCRIPTION
   Indicates that a DTMF tone has been stopped.
===========================================================================*/
LOCAL void ctp_dtmf_stop_ack( OI_BD_ADDR *addr,
                              OI_CHAR tone)
{
  BT_MSG_DEBUG("BT PF CTP TX EV: dtmf stop ack. AppID: %d",
               bt_pf_ctp_ext_app_id,0,0);
  // JTL TODO: Implement
}

/*===========================================================================

FUNCTION
   ctp_release_ind

DESCRIPTION
   Indicates that the call has been released
===========================================================================*/
LOCAL void ctp_release_ind( OI_BD_ADDR *addr, OI_TCS_CAUSE cause )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_ctp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_CTP_REL_IND;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_ctp_release_ind.bd_addr, addr);
  event.ev_msg.ev_ctp_release_ind.cause = (bt_pf_ctp_cause_type)cause;

  BT_MSG_DEBUG("BT PF CTP TX EV: release ind. AppID: %d",
               bt_pf_ctp_ext_app_id,0,0);

  bt_ec_send_event( &event );

  /* No longer connected, so make us connectable again */
  bt_cmd_rm_set_connectable( bt_pf_ctp_int_app_id, TRUE,
                             BT_RM_AVP_AUTOMATIC );

}

/*===========================================================================

FUNCTION
   ctp_outgoing_status

DESCRIPTION
   Provides the status of an outgoing call.
===========================================================================*/
LOCAL void ctp_outgoing_status( OI_BD_ADDR *addr,
                                OI_TCS_CALL_STATUS status,
                                OI_UINT8 *companySpecific,
                                OI_UINT8 companySpecificSize )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_ctp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_CTP_OUT_STATUS;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_ctp_outgoing_status.bd_addr, addr);
  event.ev_msg.ev_ctp_outgoing_status.status =
    (bt_pf_ctp_call_status_type)status;

  BT_MSG_DEBUG("BT PF CTP TX EV: outgoing status. AppID: %d",
               bt_pf_ctp_ext_app_id,0,0);
  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
   ctp_call_data_ind

DESCRIPTION
   This should not be called
===========================================================================*/
LOCAL void ctp_call_data_ind( OI_BD_ADDR *addr, OI_BYTE *data, OI_UINT16 dataLen )
{
  BT_ERR("BT PF CTP: ctp_call_data_ind erroneously called",0,0,0);
}

/*===========================================================================

FUNCTION
   ctp_cl_info_ind

DESCRIPTION
   This should not be called.
   Received information on a connectionless link
===========================================================================*/
LOCAL void ctp_cl_info_ind( OI_BD_ADDR *addr,
                            OI_TCS_AUDIO_CONTROL audioControl,
                            OI_UINT8 *companySpecific,
                            OI_UINT8 companySpecificSize )
{
  BT_ERR("BT PF CTP: ctp_cl_info_ind erroneously called",0,0,0);
}

/*===========================================================================

                    ICP CALLBACK FUNCTION DEFINITIONS
                       These functions are called by the OI implementation

===========================================================================*/

/*===========================================================================

FUNCTION
   icp_connect_ind

DESCRIPTION
   Indicates that a remote device is attempting to connect.

===========================================================================*/
LOCAL void icp_connect_ind( OI_BD_ADDR *addr )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_icp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_ICP_CON_IND;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_icp_connect_ind.bd_addr, addr);

  BT_MSG_DEBUG("BT PF ICP TX EV: connect ind. AppID: %d",
               bt_pf_icp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
   icp_connect_cfm

DESCRIPTION
   Confirms the connection to another device
===========================================================================*/
LOCAL void icp_connect_cfm( OI_BD_ADDR *addr, OI_STATUS result )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_icp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_ICP_CON_CFM;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_icp_connect_cfm.bd_addr, addr);
  event.ev_msg.ev_icp_connect_cfm.status = result;

  BT_MSG_DEBUG("BT PF ICP TX EV: connect cfm. AppID: %d",
               bt_pf_icp_ext_app_id,0,0);

  bt_ec_send_event( &event );

  /* We're connected. So we're no longer connectable */
  bt_cmd_rm_set_connectable( bt_pf_icp_int_app_id, FALSE,
                             BT_RM_AVP_AUTOMATIC );
}

/*===========================================================================

FUNCTION
   icp_disconnect_cfm

DESCRIPTION
   Confirms a disconnection from another device.
===========================================================================*/
LOCAL void icp_disconnect_cfm( OI_BD_ADDR *addr )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_icp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_ICP_DCN_CFM;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_icp_disconnect_cfm.bd_addr, addr);

  BT_MSG_DEBUG("BT PF ICP TX EV: disconnect cfm. AppID: %d",
               bt_pf_icp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
   icp_establish_ind

DESCRIPTION
   Indicates that a remote device is attempting to establish a
   call. If sendingComplete is TRUE, then the outgoing side has
   completed sending the called party number.
===========================================================================*/
LOCAL void icp_establish_ind( OI_BD_ADDR *addr, OI_TCS_CONNECT_INFO *info )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_icp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_ICP_EST_IND;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_icp_establish_ind.bd_addr, addr);
  event.ev_msg.ev_icp_establish_ind.info.call_class =
    (bt_pf_ctp_call_class_type)info->callClass;
  event.ev_msg.ev_icp_establish_ind.info.sending_complete = info->sendingComplete;
  event.ev_msg.ev_icp_establish_ind.info.signal =
    (bt_pf_ctp_signal_type)info->signal;
  copy_ctp_number( &event.ev_msg.ev_icp_establish_ind.info.calling_number,
                   info->callingNumber );
  copy_ctp_number( &event.ev_msg.ev_icp_establish_ind.info.called_number,
                   info->calledNumber );

  BT_MSG_DEBUG("BT PF ICP TX EV: est ind. AppID: %d",
               bt_pf_icp_ext_app_id,0,0);
  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
   icp_establish_cfm

DESCRIPTION
   Confirms establishment of an outgoing call.
===========================================================================*/
LOCAL void icp_establish_cfm( OI_BD_ADDR *addr, OI_STATUS result )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_icp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_ICP_EST_CFM;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_icp_establish_cfm.bd_addr, addr);
  event.ev_msg.ev_icp_establish_cfm.status  = result;

  BT_MSG_DEBUG("BT PF ICP TX EV: est cfm. AppID: %d",
               bt_pf_icp_ext_app_id,0,0);
  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
   icp_information_ind

DESCRIPTION
   Indicates information has been received. If sendingComplete is
   TRUE, then the outgoing side has completed sending the called party
   number.
===========================================================================*/
LOCAL void icp_information_ind( OI_BD_ADDR *addr,
                                OI_BOOL sendingComplete,
                                OI_CHAR keypadFacility,
                                OI_TCS_NUMBER *callingNumber,
                                OI_TCS_NUMBER *calledNumber,
                                OI_TCS_AUDIO_CONTROL audioControl,
                                OI_UINT8 *companySpecific,
                                OI_UINT8 companySpecificSize )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_icp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_ICP_INFO_IND;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_icp_info_ind.bd_addr, addr);
  event.ev_msg.ev_icp_info_ind.sending_complete = sendingComplete;
  event.ev_msg.ev_icp_info_ind.keypad_facility = keypadFacility;
  event.ev_msg.ev_icp_info_ind.audio_control =
    (bt_pf_ctp_audio_control_type)audioControl;
  copy_ctp_number( &event.ev_msg.ev_icp_info_ind.calling_number,
                   callingNumber );
  copy_ctp_number( &event.ev_msg.ev_icp_info_ind.called_number,
                   calledNumber );

  BT_MSG_DEBUG("BT PF ICP TX EV: info ind. AppID: %d",
               bt_pf_icp_ext_app_id,0,0);
  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
   icp_dtmf_start

DESCRIPTION
   Indicates that a DTMF tone should be started.
===========================================================================*/
LOCAL void icp_dtmf_start( OI_BD_ADDR *addr, OI_CHAR tone )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_icp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_ICP_DTMF_START;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_icp_dtmf_start.bd_addr, addr);
  event.ev_msg.ev_icp_dtmf_start.tone    = tone;

  BT_MSG_DEBUG("BT PF ICP TX EV: dtmf start. AppID: %d",
               bt_pf_icp_ext_app_id,0,0);
  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
   icp_dtmf_start_ack

DESCRIPTION
   Indicates that a DTMF tone has been started
===========================================================================*/
LOCAL void icp_dtmf_start_ack( OI_BD_ADDR *addr,
                               OI_CHAR tone )
{
  BT_MSG_DEBUG("BT PF ICP TX EV: dtmf start ack. AppID: %d",
               bt_pf_ctp_ext_app_id,0,0);
  // JTL TODO: Implement
}

/*===========================================================================

FUNCTION
   icp_dtmf_start_reject

DESCRIPTION
   Indicates that a DTMF tone has been rejected
===========================================================================*/
LOCAL void icp_dtmf_start_reject( OI_BD_ADDR *addr,
                                  OI_TCS_CAUSE cause )
{
  BT_MSG_DEBUG("BT PF ICP TX EV: dtmf start reject. AppID: %d",
               bt_pf_ctp_ext_app_id,0,0);
  // JTL TODO: Implement
}

/*===========================================================================

FUNCTION
   icp_dtmf_stop

DESCRIPTION
   Indicates that a DTMF tone should be stopped
===========================================================================*/
LOCAL void icp_dtmf_stop( OI_BD_ADDR *addr )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_icp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_ICP_DTMF_STOP;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_icp_dtmf_stop.bd_addr, addr);

  BT_MSG_DEBUG("BT PF ICP TX EV: dtmf stop. AppID: %d",
               bt_pf_icp_ext_app_id,0,0);
  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
   icp_dtmf_stop_ack
DESCRIPTION
   Indicates that a DTMF tone has been stopped.
===========================================================================*/
LOCAL void icp_dtmf_stop_ack( OI_BD_ADDR *addr,
                              OI_CHAR tone)
{
  BT_MSG_DEBUG("BT PF ICP TX EV: dtmf stop ack. AppID: %d",
               bt_pf_ctp_ext_app_id,0,0);
  // JTL TODO: Implement
}

/*===========================================================================

FUNCTION
   icp_release_ind

DESCRIPTION
   Indicates that the call has been released
===========================================================================*/
LOCAL void icp_release_ind( OI_BD_ADDR *addr, OI_TCS_CAUSE cause )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_icp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_ICP_REL_IND;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_icp_release_ind.bd_addr, addr);
  event.ev_msg.ev_icp_release_ind.cause =
    (bt_pf_ctp_cause_type)cause;

  BT_MSG_DEBUG("BT PF ICP TX EV: release ind. AppID: %d",
               bt_pf_icp_ext_app_id,0,0);
  bt_ec_send_event( &event );

  /* No longer connected, so make us connectable */
  bt_cmd_rm_set_connectable( bt_pf_icp_int_app_id, TRUE,
                             BT_RM_AVP_AUTOMATIC );
}

/*===========================================================================

FUNCTION
   icp_outgoing_status

DESCRIPTION
   Provides the status of an outgoing call
===========================================================================*/
LOCAL void icp_outgoing_status( OI_BD_ADDR *addr,
                                OI_TCS_CALL_STATUS status,
                                OI_UINT8 *companySpecific,
                                OI_UINT8 companySpecificSize )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_icp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_ICP_OUT_STATUS;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_icp_outgoing_status.bd_addr, addr);
  event.ev_msg.ev_icp_outgoing_status.status =
    (bt_pf_ctp_call_status_type)status;

  BT_MSG_DEBUG("BT PF ICP TX EV: outgoing status. AppID: %d",
               bt_pf_icp_ext_app_id,0,0);
  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
   icp_call_data_ind

DESCRIPTION
   This should not be called
===========================================================================*/
LOCAL void icp_call_data_ind( OI_BD_ADDR *addr, OI_BYTE *data, OI_UINT16 dataLen )
{
  BT_ERR("BT PF ICP: icp_call_data_ind erroneously called",0,0,0);
}

/*===========================================================================

FUNCTION
   icp_cl_info_ind

DESCRIPTION
   This should not be called.
   Received information on a connectionless link
===========================================================================*/
LOCAL void icp_cl_info_ind( OI_BD_ADDR *addr,
                            OI_TCS_AUDIO_CONTROL audioControl,
                            OI_UINT8 *companySpecific,
                            OI_UINT8 companySpecificSize )
{
  BT_ERR("BT PF ICP: icp_cl_info_ind erroneously called",0,0,0);
}

/*===========================================================================

                     CMD PROCESSING FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION
   bt_pf_ctp_cmd_connect

DESCRIPTION
   Procces a CTP connect command.
===========================================================================*/
LOCAL void bt_pf_ctp_cmd_connect( bt_pf_cmd_msg_type* cmd_ptr )
{
  BT_MSG_DEBUG( "BT PF CTP CMD RX: Connect", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status =
    OI_CTP_Connect( (OI_BD_ADDR*)&cmd_ptr->cmd_msg.cmd_ctp_connect.bd_addr );
}

/*===========================================================================

FUNCTION
   bt_pf_ctp_cmd_accept_connection

DESCRIPTION
   Process a CTP/ICP accept connection command.
===========================================================================*/
#define bt_pf_icp_cmd_accept_connection bt_pf_ctp_cmd_accept_connection
LOCAL void bt_pf_ctp_cmd_accept_connection( bt_pf_cmd_msg_type* cmd_ptr )
{
  BT_MSG_DEBUG( "BT PF CTP CMD RX: Accept Connection", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status =
    OI_CTP_AcceptConnection( (OI_BD_ADDR*)&cmd_ptr->cmd_msg.cmd_ctp_accept_connection.bd_addr,
                             cmd_ptr->cmd_msg.cmd_ctp_accept_connection.accept);
}

/*===========================================================================

FUNCTION
   bt_pf_ctp_cmd_disconnect

DESCRIPTION
   Process a CTP/ICP disconnect command
===========================================================================*/
#define bt_pf_icp_cmd_disconnect bt_pf_ctp_cmd_disconnect
void bt_pf_ctp_cmd_disconnect( bt_pf_cmd_msg_type* cmd_ptr )
{
  BT_MSG_DEBUG( "BT PF CTP CMD RX: Disconnect", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status =
    OI_CTP_Disconnect( (OI_BD_ADDR*)&cmd_ptr->cmd_msg.cmd_ctp_disconnect.bd_addr);
}

/*===========================================================================

FUNCTION
   bt_pf_ctp_cmd_clear_call

DESCRIPTION
   Process a CTP/ICP clear call command.
===========================================================================*/
#define bt_pf_icp_cmd_clear_call bt_pf_ctp_cmd_clear_call
void bt_pf_ctp_cmd_clear_call( bt_pf_cmd_msg_type* cmd_ptr )
{
  BT_MSG_DEBUG( "BT PF CTP CMD RX: Clear Call", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status =
    OI_CTP_ClearCall( (OI_BD_ADDR*)&cmd_ptr->cmd_msg.cmd_ctp_clear_call.bd_addr,
                      (OI_TCS_CAUSE)cmd_ptr->cmd_msg.cmd_ctp_clear_call.cause);
}

/*===========================================================================

FUNCTION
   bt_pf_ctp_cmd_establish_call

DESCRIPTION
   Process a CTP/ICP establish call command.
===========================================================================*/
#define bt_pf_icp_cmd_establish_call bt_pf_ctp_cmd_establish_call
void bt_pf_ctp_cmd_establish_call( bt_pf_cmd_msg_type* cmd_ptr )
{
  OI_TCS_NUMBER calling_number;
  OI_TCS_NUMBER called_number;
  OI_UINT8      calling_digits[BT_PF_CTP_MAX_DIGITS];
  OI_UINT8      called_digits[BT_PF_CTP_MAX_DIGITS];

  BT_MSG_DEBUG( "BT PF CTP CMD RX: Establish Call", 0, 0, 0 );

  bt_pf_ctp_copy_to_tcs_number( &calling_number,
                                &cmd_ptr->cmd_msg.cmd_ctp_establish_call.calling_number,
                                calling_digits);
  bt_pf_ctp_copy_to_tcs_number( &called_number,
                                &cmd_ptr->cmd_msg.cmd_ctp_establish_call.called_number,
                                called_digits);


  cmd_ptr->cmd_hdr.cmd_status =
    OI_CTP_EstablishCall( (OI_BD_ADDR*)&cmd_ptr->cmd_msg.cmd_ctp_establish_call.bd_addr,
                          OI_TCS_BEARER_LINK_TYPE_SCO,
                          &calling_number,
                          &called_number );
}

/*===========================================================================

FUNCTION
   bt_pf_ctp_cmd_info

DESCRIPTION
   Process a CTP info command.
===========================================================================*/
void bt_pf_ctp_cmd_info( bt_pf_cmd_msg_type* cmd_ptr )
{
  OI_TCS_NUMBER calling_number;
  OI_TCS_NUMBER called_number;
  OI_UINT8      calling_digits[BT_PF_CTP_MAX_DIGITS];
  OI_UINT8      called_digits[BT_PF_CTP_MAX_DIGITS];

  bt_pf_ctp_copy_to_tcs_number( &calling_number,
                                &cmd_ptr->cmd_msg.cmd_ctp_info.calling_number,
                                calling_digits );
  bt_pf_ctp_copy_to_tcs_number( &called_number,
                                &cmd_ptr->cmd_msg.cmd_ctp_info.called_number,
                                called_digits );

  BT_MSG_DEBUG( "BT PF CTP CMD RX: Info", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status =
    OI_CTP_Info( (OI_BD_ADDR*)&cmd_ptr->cmd_msg.cmd_ctp_info.bd_addr,
                 cmd_ptr->cmd_msg.cmd_ctp_info.sending_complete,
                 cmd_ptr->cmd_msg.cmd_ctp_info.keypad_facility,
                 &calling_number,
                 &called_number,
                 (OI_TCS_AUDIO_CONTROL)cmd_ptr->cmd_msg.cmd_ctp_info.audio_control,
                 NULL, 0 );
}

/*===========================================================================

FUNCTION
   bt_pf_ctp_cmd_incoming_status

DESCRIPTION
   Process a CTP/ICP incoming status command
===========================================================================*/
#define bt_pf_icp_cmd_incoming_status bt_pf_ctp_cmd_incoming_status
void bt_pf_ctp_cmd_incoming_status( bt_pf_cmd_msg_type* cmd_ptr )
{
  // JTL TODO: Use default bearer here?
  BT_MSG_DEBUG( "BT PF CTP CMD RX: Status", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status =
    OI_CTP_IncomingStatus( (OI_BD_ADDR*)&cmd_ptr->cmd_msg.cmd_ctp_incoming_status.bd_addr,
                           (OI_TCS_CALL_STATUS)cmd_ptr->cmd_msg.cmd_ctp_incoming_status.status,
                           NULL, /* Default Bearer */
                           NULL, /* Company Info */
                           0 );
}

/*===========================================================================

FUNCTION
   bt_pf_ctp_cmd_accept_call

DESCRIPTION
   Process a CTP/ICP accept call command
===========================================================================*/
#define bt_pf_icp_cmd_accept_call bt_pf_ctp_cmd_accept_call
void bt_pf_ctp_cmd_accept_call( bt_pf_cmd_msg_type* cmd_ptr )
{
  // JTL TODO: Change struct to pass in TCS bearer?
  /*
    static OI_TCS_BEARER sco_bearer;
    sco_bearer.type = OI_TCS_BEARER_LINK_TYPE_SCO;
    sco_bearer.info.sco.packetType = OI_TCS_BEARER_CODING_CVSD;
    sco_bearer.info.sco.coding = OI_TCS_BEARER_CODING_CVSD;
  */

  BT_MSG_DEBUG( "BT PF CTP CMD RX: Accpet Call", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status =
    OI_CTP_AcceptCall( (OI_BD_ADDR*)&cmd_ptr->cmd_msg.cmd_ctp_accept_call.bd_addr, NULL );
}

/*===========================================================================

FUNCTION
   bt_pf_ctp_cmd_start_dtmf

DESCRIPTION
   Process a CTP start dtmf command
===========================================================================*/
void bt_pf_ctp_cmd_start_dtmf( bt_pf_cmd_msg_type* cmd_ptr )
{
  BT_MSG_DEBUG( "BT PF CTP CMD RX: Start DTMF", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status =
    OI_CTP_StartDTMF( (OI_BD_ADDR*)&cmd_ptr->cmd_msg.cmd_ctp_start_dtmf.bd_addr,
                      cmd_ptr->cmd_msg.cmd_ctp_start_dtmf.tone );
}

/*===========================================================================

FUNCTION
   bt_pf_ctp_cmd_stop_dtmf

DESCRIPTION
   Process a CTP stop dtmf command
===========================================================================*/
void bt_pf_ctp_cmd_stop_dtmf( bt_pf_cmd_msg_type* cmd_ptr )
{
  BT_MSG_DEBUG( "BT PF CTP CMD RX: Stop DTMF", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status =
    OI_CTP_StopDTMF( (OI_BD_ADDR*)&cmd_ptr->cmd_msg.cmd_ctp_stop_dtmf.bd_addr );
}

/*===========================================================================

FUNCTION
   bt_pf_ctp_cmd_register

DESCRIPTION
  Process a CTP register command
===========================================================================*/
void bt_pf_ctp_cmd_register( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_status_type status = OI_OK;
  OI_SDP_STRINGS strings = { strAttrs, OI_ARRAYSIZE(strAttrs) };

  BT_MSG_DEBUG( "BT PF CTP CMD RX: Register", 0, 0, 0 );
  if( bt_pf_ctp_ext_app_id == BT_APP_ID_NULL )
  {
    bt_pf_ctp_ext_app_id = cmd_ptr->cmd_hdr.app_id;
  }
  else {
    /* We already have a CTP interface registered */
    status = BT_CS_PF_ALREADY_REGISTERED;
  }
  if(  bt_pf_ctp_int_app_id == BT_APP_ID_NULL && status == OI_OK )
  {
    bt_pf_ctp_int_app_id = bt_cmd_ec_get_app_id_and_register( bt_pf_ctp_ev_cb );
    if( bt_pf_ctp_int_app_id == BT_APP_ID_NULL )
    {
      status = BT_CS_PF_NO_RESOURCES;
    }
    else
    {
      BT_MSG_HIGH( "BT PF CTP AID is %x", bt_pf_ctp_int_app_id, 0, 0 );

      BT_CMD( bt_cmd_sd_register_service( bt_pf_ctp_int_app_id,
                                          BT_SD_SERVICE_CLASS_CORDLESS_TELEPHONY,
                                          BT_PF_TCS_VERSION_NUMBER,
                                          0,
                                          "Cordless Telephony" ),
              status );

      if( status == OI_OK )
      {
        BT_CMD( bt_cmd_rm_set_connectable( bt_pf_ctp_int_app_id, TRUE,
                                           BT_RM_AVP_AUTOMATIC ),
                status );
        if( status == OI_OK )
        {
        status = OI_CTP_TL_Register( &strings, &ctp_callbacks );
        }
      }
      if( status != OI_OK )
      {
        bt_cmd_sd_unregister_service( bt_pf_ctp_int_app_id,
                                      BT_SD_SERVICE_CLASS_CORDLESS_TELEPHONY );
        bt_cmd_ec_free_application_id( bt_pf_ctp_int_app_id );
      }
    }
  }
  cmd_ptr->cmd_hdr.cmd_status = status;
}

/*===========================================================================

FUNCTION
   bt_pf_ctp_cmd_deregister

DESCRIPTION
   Process a CTP deregister command
===========================================================================*/
void bt_pf_ctp_cmd_deregister( bt_pf_cmd_msg_type* cmd_ptr )
{
  BT_MSG_DEBUG( "BT PF CTP CMD RX: DeRegister", 0, 0, 0 );
  if( bt_pf_ctp_int_app_id != BT_APP_ID_NULL )
  {
    (void)bt_cmd_sd_unregister_service( bt_pf_ctp_int_app_id,
                                        BT_SD_SERVICE_CLASS_CORDLESS_TELEPHONY );
    (void)OI_CTP_TL_Deregister();
    bt_cmd_rm_set_connectable( bt_pf_ctp_int_app_id, FALSE,
                               BT_RM_AVP_AUTOMATIC );
    (void)bt_cmd_ec_free_application_id( bt_pf_ctp_int_app_id );
    cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
    bt_pf_ctp_int_app_id = BT_APP_ID_NULL;
    bt_pf_ctp_ext_app_id = BT_APP_ID_NULL;
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_status = BT_CS_PF_NOT_CONFIGURED;
  }
}

/*===========================================================================

FUNCTION
   bt_pf_ctp_cmd_register_recall

DESCRIPTION
   Process a CTP register recall command
===========================================================================*/
void bt_pf_ctp_cmd_register_recall( bt_pf_cmd_msg_type* cmd_ptr )
{
  BT_MSG_DEBUG( "BT PF CTP CMD RX: Register Recall", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status =
    OI_CTP_Info( (OI_BD_ADDR*)&cmd_ptr->cmd_msg.cmd_ctp_register_recall.bd_addr,
                 TRUE,
                 BT_PF_CTP_CHAR_REGISTER_RECALL,
                 NULL,
                 NULL,
                 (OI_TCS_AUDIO_CONTROL)BT_PF_CTP_AUDIO_CONTROL_NONE,
                 NULL,
                 0 );
}

/*===========================================================================

FUNCTION
   bt_pf_icp_cmd_connect

DESCRIPTION
   Process an ICP connect command
===========================================================================*/
void bt_pf_icp_cmd_connect( bt_pf_cmd_msg_type* cmd_ptr )
{
  BT_MSG_DEBUG( "BT PF ICP CMD RX: Connect", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status =
    OI_CTP_ConnectIntercom( (OI_BD_ADDR*)&cmd_ptr->cmd_msg.cmd_ctp_connect.bd_addr );
}

/*===========================================================================

FUNCTION
   bt_pf_icp_cmd_register

DESCRIPTION
   Process an ICP register command
===========================================================================*/
void bt_pf_icp_cmd_register( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_status_type status = OI_OK;

  BT_MSG_DEBUG( "BT PF ICP CMD RX: Register", 0, 0, 0 );
  if( bt_pf_icp_ext_app_id == BT_APP_ID_NULL )
  {
    bt_pf_icp_ext_app_id = cmd_ptr->cmd_hdr.app_id;
  }
  else
  {
    /* We already have a ICP interface registered */
    status = BT_CS_PF_ALREADY_REGISTERED;
  }
  if(  bt_pf_icp_int_app_id == BT_APP_ID_NULL && status == OI_OK)
  {
    bt_pf_icp_int_app_id = bt_cmd_ec_get_app_id_and_register( bt_pf_icp_ev_cb );
    if( bt_pf_icp_int_app_id == BT_APP_ID_NULL )
    {
      status = BT_CS_PF_NO_RESOURCES;
    }
    else
    {
      BT_MSG_HIGH( "BT PF ICP AID is %x", bt_pf_icp_int_app_id, 0, 0 );
      BT_CMD( bt_cmd_sd_register_service( bt_pf_icp_int_app_id,
                                          BT_SD_SERVICE_CLASS_INTERCOM,
                                          BT_PF_TCS_VERSION_NUMBER,
                                          0,
                                          "Intercom" ),
              status );
      if( status == OI_OK )
      {
        BT_CMD( bt_cmd_rm_set_connectable( bt_pf_icp_int_app_id, TRUE,
                                           BT_RM_AVP_AUTOMATIC ),
                status );
        if( status == OI_OK )
        {
          status = OI_CTP_TL_RegisterIntercom( NULL, (OI_TCS_CALLBACKS*)(&icp_callbacks) );
        }
      }
      if( status != OI_OK )
      {
        bt_cmd_sd_unregister_service( bt_pf_icp_int_app_id,
                                      BT_SD_SERVICE_CLASS_INTERCOM );
        bt_cmd_ec_free_application_id( bt_pf_icp_int_app_id );
      }
    }
  }
  cmd_ptr->cmd_hdr.cmd_status = status;
}

/*===========================================================================

FUNCTION
   bt_pf_icp_cmd_deregister

DESCRIPTION
   Process an ICP deregister command
===========================================================================*/
void bt_pf_icp_cmd_deregister( bt_pf_cmd_msg_type* cmd_ptr )
{
  BT_MSG_DEBUG( "BT PF ICP CMD RX: DeRegister", 0, 0, 0 );
  if( bt_pf_icp_int_app_id != BT_APP_ID_NULL )
  {
    (void)bt_cmd_sd_unregister_service( bt_pf_icp_int_app_id,
                                        BT_SD_SERVICE_CLASS_INTERCOM );
    (void)OI_CTP_TL_Deregister();
    bt_cmd_rm_set_connectable( bt_pf_icp_int_app_id, FALSE,
                               BT_RM_AVP_AUTOMATIC );
    (void)bt_cmd_ec_free_application_id( bt_pf_icp_int_app_id );
    cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
    bt_pf_icp_int_app_id = BT_APP_ID_NULL;
    bt_pf_icp_ext_app_id = BT_APP_ID_NULL;
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_status = BT_CS_PF_NOT_CONFIGURED;
  }
}

/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION
  bt_pf_ctp_process_command

DESCRIPTION
    Processes the given command by calling the underlying OI OPP driver.

===========================================================================*/
void bt_pf_ctp_process_command( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_status_type         status = OI_OK;

  switch ( cmd_ptr->cmd_hdr.cmd_type )
  {
    case BT_PF_CMD_CTP_CONNECT:           /* no break */
    case BT_PF_CMD_CTP_ACCEPT_CONNECTION: /* no break */
    case BT_PF_CMD_CTP_DISCONNECT:        /* no break */
    case BT_PF_CMD_CTP_CLEAR_CALL:        /* no break */
    case BT_PF_CMD_CTP_ESTABLISH_CALL:    /* no break */
    case BT_PF_CMD_CTP_INFO:              /* no break */
    case BT_PF_CMD_CTP_INCOMING_STATUS:   /* no break */
    case BT_PF_CMD_CTP_ACCEPT_CALL:       /* no break */
    case BT_PF_CMD_CTP_START_DTMF:        /* no break */
    case BT_PF_CMD_CTP_STOP_DTMF:         /* no break */
    case BT_PF_CMD_CTP_REGISTER_RECALL:   /* no break */
    case BT_PF_CMD_CTP_DEREGISTER:        /* no break */
      status = check_ctp_app_id( cmd_ptr );
      break;
    case BT_PF_CMD_ICP_ACCEPT_CONNECTION: /* No break */
    case BT_PF_CMD_ICP_DISCONNECT:        /* No break */
    case BT_PF_CMD_ICP_CLEAR_CALL:        /* No break */
    case BT_PF_CMD_ICP_ESTABLISH_CALL:    /* No break */
    case BT_PF_CMD_ICP_INCOMING_STATUS:   /* No break */
    case BT_PF_CMD_ICP_ACCEPT_CALL:       /* No break */
    case BT_PF_CMD_ICP_CONNECT:           /* no break */
    case BT_PF_CMD_ICP_DEREGISTER:        /* no break */
      status = check_icp_app_id( cmd_ptr );
      break;
    default:
      break;
  }


  if( status != BT_CS_GN_SUCCESS )
  {
    cmd_ptr->cmd_hdr.cmd_status = status;
  }
  else
  {
    switch ( cmd_ptr->cmd_hdr.cmd_type )
    {
      case BT_PF_CMD_CTP_CONNECT:
        bt_pf_ctp_cmd_connect( cmd_ptr );
        break;

      case BT_PF_CMD_ICP_ACCEPT_CONNECTION: /* No break */
      case BT_PF_CMD_CTP_ACCEPT_CONNECTION:
        bt_pf_ctp_cmd_accept_connection( cmd_ptr );
        break;

      case BT_PF_CMD_ICP_DISCONNECT: /* No break */
      case BT_PF_CMD_CTP_DISCONNECT:
        bt_pf_ctp_cmd_disconnect( cmd_ptr );
        break;

      case BT_PF_CMD_ICP_CLEAR_CALL: /* No break */
      case BT_PF_CMD_CTP_CLEAR_CALL:
        bt_pf_ctp_cmd_clear_call( cmd_ptr );
        break;

      case BT_PF_CMD_ICP_ESTABLISH_CALL: /* No break */
      case BT_PF_CMD_CTP_ESTABLISH_CALL:
        bt_pf_ctp_cmd_establish_call( cmd_ptr );
        break;

      case BT_PF_CMD_CTP_INFO:
        bt_pf_ctp_cmd_info( cmd_ptr );
        break;

      case BT_PF_CMD_ICP_INCOMING_STATUS: /* No break */
      case BT_PF_CMD_CTP_INCOMING_STATUS:
        bt_pf_ctp_cmd_incoming_status( cmd_ptr );
        break;

      case BT_PF_CMD_ICP_ACCEPT_CALL: /* No break */
      case BT_PF_CMD_CTP_ACCEPT_CALL:
        bt_pf_ctp_cmd_accept_call( cmd_ptr );
        break;

      case BT_PF_CMD_CTP_START_DTMF:
        bt_pf_ctp_cmd_start_dtmf( cmd_ptr );
        break;

      case BT_PF_CMD_CTP_STOP_DTMF:
        bt_pf_ctp_cmd_stop_dtmf( cmd_ptr );
        break;

      case BT_PF_CMD_CTP_REGISTER:
        bt_pf_ctp_cmd_register( cmd_ptr );
        break;

      case BT_PF_CMD_CTP_DEREGISTER:
        bt_pf_ctp_cmd_deregister( cmd_ptr );
        break;

      case BT_PF_CMD_CTP_REGISTER_RECALL:
        bt_pf_ctp_cmd_register_recall( cmd_ptr );
        break;

        /*
         * ICP Commands:
         */

      case BT_PF_CMD_ICP_CONNECT:
        bt_pf_icp_cmd_connect( cmd_ptr );
        break;

      case BT_PF_CMD_ICP_REGISTER:
        bt_pf_icp_cmd_register( cmd_ptr );
        break;

      case BT_PF_CMD_ICP_DEREGISTER:
        bt_pf_icp_cmd_deregister( cmd_ptr );
        break;

      default:
        BT_ERR( "BT PF CTP/ICP CMD RX: Unknown: %x",
                cmd_ptr->cmd_hdr.cmd_type, 0, 0 );
        cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_UNRECOGNIZED_CMD;
        break;
    }
  }
}
#endif /* FEATURE_BT_EXTPF_CTP */
#endif /* FEATURE_BT */
