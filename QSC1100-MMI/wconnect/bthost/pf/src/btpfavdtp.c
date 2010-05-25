/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

B L U E T O O T H   A U D I O / V I D E O   D I S T R I B U T I O N
                  T R A N S P O R T   P R O F I L E

GENERAL DESCRIPTION
  This module contains the Bluetooth AVDTP interface
  (commands and events) implementation.

Copyright (c) 2005-2006 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
 ----------   ---  -----------------------------------------------------------
 2008-06-24   hs   Fixed Klockwork errors

*   #13     21 May 2007          SSK
* Fix compiler warnings by typecasting enums.
*
*   #12     03 Nov 2006          DM
* Changed to use default flush timeout (infinite) when AV-Sync is not defined.
*
*   #11     07 Jul 2006          DM
* Changed flush timeout to a #define value.
*
*   #10     26 Jul 2006          DM
* Changes related to sending frame handle to L2CAP Write
*
*   #9      02 Sep 2005          JTL
* Fixing DSM leak if there's a mismatch in states between A2DP and AVDTP.
*
*   #8      26 Jul 2005          JTL
* Updating to latest code.
*
*   #7      05 Jul 2005          JTL
* Fixing parenthesis. Better debug messages.
*
*   #6      02 Jun 2005          JTL
* Better fix for #5.
* Fix bug using wrong data type in get_cap_cfm.
*
*   #5      14 Apr 2005          JTL
* Pass static structure to OI code for enpoint CAP support. Currently, only supports
* one endpoint.
*
*   #1-4    31 Mar 2005          JTL
* Initial revision.
*
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF_AV

#include "bt.h"
#include "btpfi.h"
#include "btpf.h"
#include "btpfcmdi.h"
#include "bti.h"
#include "btmsg.h"

#include "audiosbcenc.h"

/* Open interface headers and code */
#include "oi_status.h"
#undef __OI_MODULE__
#include "avdtp.c"
#undef __OI_MODULE__
#include "avdtp_signal.c"
/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#define BT_MSG_LAYER  BT_MSG_GN
/*-------------------------------------------------------------------------*/
/* Check command status of bt driver commands                              */
/*-------------------------------------------------------------------------*/
#define BT_CMD(cmd,status) if( ((status) = (cmd)) == BT_CS_GN_PENDING){(status)=OI_OK;}

/*-------------------------------------------------------------------------*/
/*  This app id is the ID's of the app using AVDTP                         */
/*-------------------------------------------------------------------------*/
LOCAL bt_app_id_type bt_pf_avdtp_ext_app_id  = BT_APP_ID_NULL;

/*===========================================================================

                    INTERNAL FUNCTION PROTOTYPES

===========================================================================*/
OI_STATUS OI_AVDTP_SendWithHeader_Ex(OI_UINT16 handle,
                                     OI_UINT16 seqNum,
                                     OI_UINT32 timestamp,
                                     OI_UINT8 payloadType,
                                     OI_UINT16 marker,
                                     dsm_item_type *data,
                                     dsm_item_type *app_header,
                                     void *frame_handle);

OI_STATUS OI_AVDTP_SecurityControl_Ex(OI_UINT16 handle,
                                      dsm_item_type *data,
                                      OI_UINT8 *transaction);

OI_STATUS OI_AVDTP_SecurityControlRsp_Ex(OI_UINT16 handle,
                                         OI_UINT8 transaction,
                                         dsm_item_type *data,
                                         OI_UINT8 error);

/*===========================================================================

                    INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION
  bt_pf_avdtp_copy_caps_from_oi

DESCRIPTION
  Utility function to copy OI record info from OI structures into DMSS ones.

===========================================================================*/
LOCAL void bt_pf_avdtp_copy_caps_from_oi( bt_pf_avdtp_cap_type* dest,
                                          OI_AVDTP_CAP* src,
                                          uint8 num_caps )
{
  int i,j;
  for( i = 0; i < num_caps; i++ )
  {
    dest->type = src->type;
    if( dest->type == BT_PF_AVDTP_CAP_CODEC )
    {
      dest->data.codec_data.media_type = src->data.codecData.mediaType ;
      dest->data.codec_data.codec_type = src->data.codecData.codecType ;

      if( dest->data.codec_data.codec_type == BT_PF_AVDTP_CODEC_SBC &&
          dest->data.codec_data.media_type == BT_PF_AVDTP_MEDIA_TYPE_AUDIO )
      {
        for( j = 0; j < sizeof(audiosbcenc_data_type); j++ )
        {
          ((byte*)&dest->data.codec_data.data.sbc_data)[j] =
            src->data.codecData.data[j];
        }
      }
      else
      {
        BT_ERR("Generic codec data unsupported %x; continuing anyway",
               dest->data.codec_data.codec_type,0,0);
      }
    }
    else if( dest->type != BT_PF_AVDTP_CAP_MEDIA_TRANSPORT )
    {
      BT_ERR("Generic cap data unsupported %x; continuing anyway",
             dest->type,0,0);
    }
    src++;
    dest++;
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_copy_records_from_oi

DESCRIPTION
  Utility function to copy OI record info from OI structures into DMSS ones.

===========================================================================*/
LOCAL void bt_pf_avdtp_copy_records_from_oi( bt_pf_avdtp_discover_rsp_info* dest,
                                             OI_AVDTP_DISCOVER_RSP_INFO* src,
                                             uint8 num_records )
{
  int i;
  for( i = 0; i < num_records; i++ )
  {
    dest->media_type    = src->mediaType;
    dest->endpoint_type = src->endpointType ;
    dest->seid          = src->seid ;
    dest->in_use        = src->inUse ;

    src++;
    dest++;
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_copy_to_oi_cap

DESCRIPTION
  Utility function to copy a Bluetooth driver AVDTP capabilities to an OI
  capabilities struct.

===========================================================================*/
LOCAL void bt_pf_avdtp_copy_to_oi_cap( OI_AVDTP_CAP* dest,
                                       bt_pf_avdtp_cap_type* src,
                                       uint8 num_caps )
{
  for( ; num_caps > 0; num_caps-- )
  {
    dest->type = src->type;
    if( dest->type == BT_PF_AVDTP_CAP_CODEC )
    {
      dest->data.codecData.mediaType = src->data.codec_data.media_type;
      dest->data.codecData.codecType = src->data.codec_data.codec_type;
      if( src->data.codec_data.codec_type == BT_PF_AVDTP_CODEC_SBC &&
          src->data.codec_data.media_type == BT_PF_AVDTP_MEDIA_TYPE_AUDIO )
      {
        dest->data.codecData.data      = (byte*)&src->data.codec_data.data.sbc_data;
        dest->data.codecData.dataLen   = sizeof(audiosbcenc_data_type);
      }
      else
      {
        dest->data.codecData.data    = src->data.codec_data.data.generic_codec.data;
        dest->data.codecData.dataLen = src->data.codec_data.data.generic_codec.data_len;
      }
    }
    else
    {
      dest->data.genericData.data = src->data.generic_data.data;
      dest->data.genericData.dataLen = src->data.generic_data.data_len;
    }
    src++;
    dest++;
  }
}

/*===========================================================================

                    AVDTP CALLBACK FUNCTION DEFINITIONS
                       These functions are called by the OI implementation

===========================================================================*/

/*===========================================================================

FUNCTION
  bt_pf_avdtp_connect_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_connect_ind ( OI_BD_ADDR *addr)
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_avdtp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVDTP_CON_IND;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_avdtp_con_ind.bd_addr, addr);

  BT_MSG_DEBUG("BT PF AVDTP TX EV: con ind. AppID: %x",
               bt_pf_avdtp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_connect_cfm

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_connect_cfm ( OI_BD_ADDR *addr,
                                     OI_STATUS   result )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_avdtp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVDTP_CON_CFM;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_avdtp_con_cfm.bd_addr, addr);
  event.ev_msg.ev_avdtp_con_cfm.result = result;

  BT_MSG_DEBUG("BT PF AVDTP TX EV: con ind. AppID: %x",
               bt_pf_avdtp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_disconnect_cb

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_disconnect_cb ( OI_BD_ADDR *addr,
                                       OI_STATUS   reason )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_avdtp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVDTP_DISCON_IND;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_avdtp_discon_ind.bd_addr, addr);
  event.ev_msg.ev_avdtp_discon_ind.reason = reason;

  BT_MSG_DEBUG("BT PF AVDTP TX EV: discon ind AppID: %x",
               bt_pf_avdtp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_discover_cfm

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_discover_cfm ( OI_UINT8                    transaction,
                                      OI_BD_ADDR                 *addr,
                                      OI_AVDTP_DISCOVER_RSP_INFO *records,
                                      OI_UINT8                    numRecords,
                                      OI_UINT8                    error )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_avdtp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVDTP_DISC_CFM;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_avdtp_disc_cfm.bd_addr, addr);
  event.ev_msg.ev_avdtp_disc_cfm.txn = transaction;
  event.ev_msg.ev_avdtp_disc_cfm.error =
    ( error == OI_AVDTP_SUCCESS ?
      BT_CS_GN_SUCCESS
      : (bt_cmd_status_type)(error + BT_CMD_EV_PF_AVDTP_BASE));

  if( numRecords > BT_PF_AVDTP_MAX_REMOTE_RECORDS)
  {
    BT_ERR("BT PF AVDTP: Too many recs in disc cfm %x, max %x",
           numRecords, BT_PF_AVDTP_MAX_REMOTE_RECORDS, 0);
    numRecords = BT_PF_AVDTP_MAX_REMOTE_RECORDS;
  }
  event.ev_msg.ev_avdtp_disc_cfm.num_records = numRecords;
  bt_pf_avdtp_copy_records_from_oi( event.ev_msg.ev_avdtp_disc_cfm.records,
                                    records,
                                    numRecords );

  BT_MSG_DEBUG("BT PF AVDTP TX EV: disc cfm AppID: %x",
               bt_pf_avdtp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_get_capabilities_cfm

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_get_capabilities_cfm ( OI_UINT8      transaction,
                                              OI_BD_ADDR   *addr,
                                              OI_UINT8      remoteSEID,
                                              OI_AVDTP_CAP *caps,
                                              OI_UINT8      numCaps,
                                              OI_UINT8      error )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_avdtp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVDTP_GET_CAP_CFM;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_avdtp_get_cap_cfm.bd_addr, addr);
  event.ev_msg.ev_avdtp_get_cap_cfm.remote_seid = remoteSEID;
  event.ev_msg.ev_avdtp_get_cap_cfm.error =
    ( error == OI_AVDTP_SUCCESS ?
      BT_CS_GN_SUCCESS
      : (bt_cmd_status_type)(error + BT_CMD_EV_PF_AVDTP_BASE));     
  event.ev_msg.ev_avdtp_get_cap_cfm.txn = transaction;

  if( numCaps > BT_PF_AVDTP_MAX_REMOTE_CAPS )
  {
    BT_ERR("BT PF AVDTP: Too many recs in get cap cfm %x, max %x",
           numCaps, BT_PF_AVDTP_MAX_REMOTE_CAPS, 0);
    numCaps = BT_PF_AVDTP_MAX_REMOTE_CAPS;
  }
  event.ev_msg.ev_avdtp_get_cap_cfm.num_caps = numCaps;

  bt_pf_avdtp_copy_caps_from_oi( event.ev_msg.ev_avdtp_get_cap_cfm.caps,
                                 caps,
                                 numCaps );

  BT_MSG_DEBUG("BT PF AVDTP TX EV: get cap cfm AppID: %x",
               bt_pf_avdtp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_set_config_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_set_config_ind ( OI_UINT8      transaction,
                                        OI_BD_ADDR   *addr,
                                        OI_UINT8      localSEID,
                                        OI_UINT8      remoteSEID,
                                        OI_UINT16     handle,
                                        OI_AVDTP_CAP *caps,
                                        OI_UINT8      numCaps )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_avdtp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVDTP_SET_CFG_IND;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_avdtp_set_cfg_ind.bd_addr, addr);
  event.ev_msg.ev_avdtp_set_cfg_ind.remote_seid = remoteSEID;
  event.ev_msg.ev_avdtp_set_cfg_ind.local_seid = localSEID;
  event.ev_msg.ev_avdtp_set_cfg_ind.handle = handle;
  event.ev_msg.ev_avdtp_set_cfg_ind.txn = transaction;

  if( numCaps > BT_PF_AVDTP_MAX_REMOTE_CAPS )
  {
    BT_ERR("BT PF AVDTP: Too many recs in get cap cfm %x, max %x",
           numCaps, BT_PF_AVDTP_MAX_REMOTE_CAPS, 0);
    numCaps = BT_PF_AVDTP_MAX_REMOTE_CAPS;
  }
  event.ev_msg.ev_avdtp_set_cfg_ind.num_caps = numCaps;

  bt_pf_avdtp_copy_caps_from_oi( event.ev_msg.ev_avdtp_set_cfg_ind.caps,
                                 caps,
                                 numCaps );

  BT_MSG_DEBUG("BT PF AVDTP TX EV: set cfg ind AppID: %x",
               bt_pf_avdtp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_set_config_cfm

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_set_config_cfm ( OI_UINT8  transaction,
                                        OI_UINT16 handle,
                                        OI_UINT8  category,
                                        OI_UINT8  error )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_avdtp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVDTP_SET_CFG_CFM;
  event.ev_msg.ev_avdtp_set_cfg_cfm.txn      = transaction;
  event.ev_msg.ev_avdtp_set_cfg_cfm.handle   = handle;
  event.ev_msg.ev_avdtp_set_cfg_cfm.category = category;
  event.ev_msg.ev_avdtp_set_cfg_cfm.error    =
    ( error == OI_AVDTP_SUCCESS ?
      BT_CS_GN_SUCCESS
      : (bt_cmd_status_type)(error + BT_CMD_EV_PF_AVDTP_BASE));

  BT_MSG_DEBUG("BT PF AVDTP TX EV: set cfg cfm AppID: %x",
               bt_pf_avdtp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_get_config_cfm

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_get_config_cfm ( OI_UINT8      transaction,
                                        OI_UINT16     handle,
                                        OI_AVDTP_CAP *caps,
                                        OI_UINT8      numCaps,
                                        OI_UINT8      error )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_avdtp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVDTP_GET_CFG_CFM;
  event.ev_msg.ev_avdtp_get_cfg_cfm.txn = transaction;
  event.ev_msg.ev_avdtp_get_cfg_cfm.handle = handle;
  event.ev_msg.ev_avdtp_get_cfg_cfm.error =
    ( error == OI_AVDTP_SUCCESS ?
      BT_CS_GN_SUCCESS
      : (bt_cmd_status_type)(error + BT_CMD_EV_PF_AVDTP_BASE));

  if( numCaps > BT_PF_AVDTP_MAX_REMOTE_CAPS )
  {
    BT_ERR("BT PF AVDTP: Too many recs in get cfg cfm %x, max %x",
           numCaps, BT_PF_AVDTP_MAX_REMOTE_CAPS, 0);
    numCaps = BT_PF_AVDTP_MAX_REMOTE_CAPS;
  }
  event.ev_msg.ev_avdtp_get_cfg_cfm.num_caps = numCaps;

  bt_pf_avdtp_copy_caps_from_oi( event.ev_msg.ev_avdtp_get_cfg_cfm.caps,
                                 caps,
                                 numCaps );

  BT_MSG_DEBUG("BT PF AVDTP TX EV: get cfg cfm AppID: %x",
               bt_pf_avdtp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_open_cfm

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_open_cfm ( OI_UINT8  transaction,
                                  OI_UINT16 handle,
                                  OI_UINT16 outMTU,
                                  OI_UINT8  error )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_avdtp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVDTP_OPEN_CFM;
  event.ev_msg.ev_avdtp_open_cfm.txn      = transaction;
  event.ev_msg.ev_avdtp_open_cfm.handle   = handle;
  event.ev_msg.ev_avdtp_open_cfm.out_mtu  = outMTU;
  event.ev_msg.ev_avdtp_open_cfm.error    =
    ( error == OI_AVDTP_SUCCESS ?
      BT_CS_GN_SUCCESS
      : (bt_cmd_status_type)(error + BT_CMD_EV_PF_AVDTP_BASE));

  BT_MSG_DEBUG("BT PF AVDTP TX EV: open cfm AppID: %x",
               bt_pf_avdtp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_open_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_open_ind ( OI_UINT16 handle,
                                  OI_UINT16 outMTU )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_avdtp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVDTP_OPEN_IND;
  event.ev_msg.ev_avdtp_open_ind.handle   = handle;
  event.ev_msg.ev_avdtp_open_ind.out_mtu  = outMTU;

  BT_MSG_DEBUG("BT PF AVDTP TX EV: open ind AppID: %x",
               bt_pf_avdtp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_close_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_close_ind ( OI_UINT8  transaction,
                                   OI_UINT16 handle )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_avdtp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVDTP_CLOSE_IND;
  event.ev_msg.ev_avdtp_close_ind.txn      = transaction;
  event.ev_msg.ev_avdtp_close_ind.handle   = handle;

  BT_MSG_DEBUG("BT PF AVDTP TX EV: close ind AppID: %x",
               bt_pf_avdtp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_close_cfm

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_close_cfm ( OI_UINT8  transaction,
                                   OI_UINT16 handle,
                                   OI_UINT8  error)
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_avdtp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVDTP_CLOSE_CFM;
  event.ev_msg.ev_avdtp_close_cfm.txn     = transaction;
  event.ev_msg.ev_avdtp_close_cfm.handle  = handle;
  event.ev_msg.ev_avdtp_close_cfm.error   =
    ( error == OI_AVDTP_SUCCESS ?
      BT_CS_GN_SUCCESS
      : (bt_cmd_status_type)(error + BT_CMD_EV_PF_AVDTP_BASE));

  BT_MSG_DEBUG("BT PF AVDTP TX EV: close cfm AppID: %x",
               bt_pf_avdtp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_start_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_start_ind ( OI_UINT8   transaction,
                                   OI_UINT16 *handles,
                                   OI_UINT8   num )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_avdtp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVDTP_START_IND;
  event.ev_msg.ev_avdtp_start_ind.txn     = transaction;
  event.ev_msg.ev_avdtp_start_ind.handle  = *handles;

  if( num > 1 )
  {
    BT_ERR("BT PF AVDTP: multiple start ind's not handled %x",
           num, 0, 0);
  }

  BT_MSG_DEBUG("BT PF AVDTP TX EV: start_ind AppID: %x",
               bt_pf_avdtp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_start_cfm

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_start_cfm ( OI_UINT8   transaction,
                                   OI_UINT16 *handles,
                                   OI_UINT8   numHandles,
                                   OI_UINT8   errorHandle,
                                   OI_UINT8   error )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_avdtp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVDTP_START_CFM;
  event.ev_msg.ev_avdtp_start_cfm.txn     = transaction;
  event.ev_msg.ev_avdtp_start_cfm.handle  = *handles;
  if( numHandles == 1 )
  {
    event.ev_msg.ev_avdtp_start_cfm.error =
      ( error == OI_AVDTP_SUCCESS ?
        BT_CS_GN_SUCCESS
        : (bt_cmd_status_type)(error + BT_CMD_EV_PF_AVDTP_BASE));
  }
  else
  {
    event.ev_msg.ev_avdtp_start_cfm.error   = BT_CS_GN_SUCCESS;
  }

  if( numHandles > 1 )
  {
    BT_ERR("BT PF AVDTP: multiple start cfm's not handled %x",
           numHandles, 0, 0);
  }

  BT_MSG_DEBUG("BT PF AVDTP TX EV: start cfm AppID: %x",
               bt_pf_avdtp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_suspend_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_suspend_ind ( OI_UINT8   transaction,
                                     OI_UINT16 *handles,
                                     OI_UINT8   numHandles )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_avdtp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVDTP_SUSPEND_IND;
  event.ev_msg.ev_avdtp_suspend_ind.txn     = transaction;
  event.ev_msg.ev_avdtp_suspend_ind.handle  = *handles;

  if( numHandles > 1 )
  {
    BT_ERR("BT PF AVDTP: multiple suspend ind's not handled %x",
           numHandles, 0, 0);
  }

  BT_MSG_DEBUG("BT PF AVDTP TX EV: suspend_ind AppID: %x",
               bt_pf_avdtp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_suspend_cfm

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_suspend_cfm ( OI_UINT8   transaction,
                                     OI_UINT16 *handles,
                                     OI_UINT8   numHandles,
                                     OI_UINT8   errorHandle,
                                     OI_UINT8   error )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_avdtp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVDTP_SUSPEND_CFM;
  event.ev_msg.ev_avdtp_suspend_cfm.txn     = transaction;
  event.ev_msg.ev_avdtp_suspend_cfm.handle  = *handles;
  if( numHandles == 1 )
  {
    event.ev_msg.ev_avdtp_suspend_cfm.error   =
      ( error == OI_AVDTP_SUCCESS ?
        BT_CS_GN_SUCCESS
        : (bt_cmd_status_type)(error + BT_CMD_EV_PF_AVDTP_BASE));
  }
  else
  {
    event.ev_msg.ev_avdtp_suspend_cfm.error   = BT_CS_GN_SUCCESS;
  }

  if( numHandles > 1 )
  {
    BT_ERR("BT PF AVDTP: multiple suspend cfm's not handled %x",
           numHandles, 0, 0);
  }

  BT_MSG_DEBUG("BT PF AVDTP TX EV: suspend cfm AppID: %x",
               bt_pf_avdtp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_reconfigure_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_reconfigure_ind ( OI_UINT8      transaction,
                                         OI_UINT16     handle,
                                         OI_AVDTP_CAP *caps,
                                         OI_UINT8      numCaps )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_avdtp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVDTP_RECFG_IND;
  event.ev_msg.ev_avdtp_recfg_ind.txn = transaction;
  event.ev_msg.ev_avdtp_recfg_ind.handle = handle;

  if( numCaps > BT_PF_AVDTP_MAX_REMOTE_CAPS )
  {
    BT_ERR("BT PF AVDTP: Too many recs in recfg ind %x, max %x",
           numCaps, BT_PF_AVDTP_MAX_REMOTE_CAPS, 0);
    numCaps = BT_PF_AVDTP_MAX_REMOTE_CAPS;
  }
  event.ev_msg.ev_avdtp_recfg_ind.num_caps = numCaps;

  bt_pf_avdtp_copy_caps_from_oi( event.ev_msg.ev_avdtp_recfg_ind.caps,
                                 caps,
                                 numCaps );

  BT_MSG_DEBUG("BT PF AVDTP TX EV: recfg ind AppID: %x",
               bt_pf_avdtp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_reconfigure_cfm

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_reconfigure_cfm ( OI_UINT8  transaction,
                                         OI_UINT16 handle,
                                         OI_UINT8  category,
                                         OI_UINT8  error )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_avdtp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVDTP_RECFG_CFM;
  event.ev_msg.ev_avdtp_recfg_cfm.txn      = transaction;
  event.ev_msg.ev_avdtp_recfg_cfm.handle   = handle;
  event.ev_msg.ev_avdtp_recfg_cfm.category = category;
  event.ev_msg.ev_avdtp_recfg_cfm.error    =
    ( error == OI_AVDTP_SUCCESS ?
      BT_CS_GN_SUCCESS
      : (bt_cmd_status_type)(error + BT_CMD_EV_PF_AVDTP_BASE));

  BT_MSG_DEBUG("BT PF AVDTP TX EV: recfg cfm AppID: %x",
               bt_pf_avdtp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_abort_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_abort_ind ( OI_UINT8  transaction,
                                   OI_UINT16 handle )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_avdtp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVDTP_ABORT_IND;
  event.ev_msg.ev_avdtp_abort_ind.txn     = transaction;
  event.ev_msg.ev_avdtp_abort_ind.handle  = handle;

  BT_MSG_DEBUG("BT PF AVDTP TX EV: abort ind AppID: %x",
               bt_pf_avdtp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_abort_cfm

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_abort_cfm ( OI_UINT8  transaction,
                                   OI_UINT16 handle,
                                   OI_UINT8  error )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_avdtp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVDTP_ABORT_CFM;
  event.ev_msg.ev_avdtp_abort_cfm.txn     = transaction;
  event.ev_msg.ev_avdtp_abort_cfm.handle  = handle;
  event.ev_msg.ev_avdtp_abort_cfm.error   =
    ( error == OI_AVDTP_SUCCESS ?
      BT_CS_GN_SUCCESS
      : (bt_cmd_status_type)(error + BT_CMD_EV_PF_AVDTP_BASE));

  BT_MSG_DEBUG("BT PF AVDTP TX EV: abort cfm AppID: %x",
               bt_pf_avdtp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_security_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_security_ind ( OI_UINT8  transaction,
                                      OI_UINT16 handle,
                                      OI_BYTE  *data,
                                      OI_UINT16 dataLen )
{
  BT_ERR("BT PF AVDTP: Sec Ind not handled!",0,0,0);
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_security_cfm

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_security_cfm ( OI_UINT8  transaction,
                                      OI_UINT16 handle,
                                      OI_BYTE  *data,
                                      OI_UINT16 dataLen,
                                      OI_UINT8  error )
{
  BT_ERR("BT PF AVDTP: Sec Cfm not handled!",0,0,0);
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_recv_data_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_recv_data_ind ( OI_UINT16 handle,
                                       OI_UINT16 seqNum,
                                       OI_UINT32 timestamp,
                                       OI_BYTE  *data,
                                       OI_UINT16 dataLen,
                                       OI_UINT8  payloadType,
                                       OI_UINT16 marker,
                                       OI_UINT16 reliability )
{
  BT_ERR("BT PF AVDTP: recv data not handled!",0,0,0);
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_send_cfm

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avdtp_send_cfm ( OI_UINT16 handle,
                                  OI_BYTE  *buf,
                                  OI_UINT16 bufLen,
                                  OI_STATUS result )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_avdtp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVDTP_SEND_CFM;
  event.ev_msg.ev_avdtp_send_cfm.handle  = handle;
  event.ev_msg.ev_avdtp_send_cfm.result   = result;

  //  BT_MSG_DEBUG("BT PF AVDTP TX EV: send cfm AppID: %x",
  //               bt_pf_avdtp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avdtp_recv_broadcast_data_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event. This one should never be called.

===========================================================================*/
LOCAL void bt_pf_avdtp_recv_broadcast_data_ind ( OI_UINT16 handle,
                                                 OI_UINT8  seqNum,
                                                 OI_UINT8  offset,
                                                 OI_UINT8  fractional,
                                                 OI_BOOL   fec,
                                                 OI_BYTE * data,
                                                 OI_UINT16 dataLen )

{
  BT_ERR("BT PF AVDTP: recv broadcast data ind not handled!",0,0,0);
}

/*===========================================================================

                     CMD PROCESSING FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================

FUNCTION
   bt_pf_avdtp_cmd_register

DESCRIPTION
   Procces a AVDTP connect command.
===========================================================================*/
LOCAL void bt_pf_avdtp_cmd_register( bt_pf_cmd_msg_type* cmd_ptr )
{
  static const OI_AVDTP_CALLBACKS bt_pf_avdtp_callbacks =
    { bt_pf_avdtp_connect_ind,
      bt_pf_avdtp_connect_cfm,
      bt_pf_avdtp_disconnect_cb,
      bt_pf_avdtp_discover_cfm,
      bt_pf_avdtp_get_capabilities_cfm,
      bt_pf_avdtp_set_config_ind,
      bt_pf_avdtp_set_config_cfm,
      bt_pf_avdtp_get_config_cfm,
      bt_pf_avdtp_open_cfm,
      bt_pf_avdtp_open_ind,
      bt_pf_avdtp_close_ind,
      bt_pf_avdtp_close_cfm,
      bt_pf_avdtp_start_ind,
      bt_pf_avdtp_start_cfm,
      bt_pf_avdtp_suspend_ind,
      bt_pf_avdtp_suspend_cfm,
      bt_pf_avdtp_reconfigure_ind,
      bt_pf_avdtp_reconfigure_cfm,
      bt_pf_avdtp_security_ind,
      bt_pf_avdtp_security_cfm,
      bt_pf_avdtp_abort_ind,
      bt_pf_avdtp_abort_cfm,
      bt_pf_avdtp_recv_data_ind,
      bt_pf_avdtp_send_cfm,
      bt_pf_avdtp_recv_broadcast_data_ind };

  static const OI_L2CAP_CONNECT_PARAMS bt_pf_avdtp_l2_params =
    {
      BT_L2_DEFAULT_MTU,
#ifdef FEATURE_BT_AVSYNC
      BT_L2FLUSH_MAX_TIME,  /* Flush Timeout in milli seconds */
#else
      BT_L2_DEFAULT_FLUSH_TIMEOUT,
#endif
      30000                 /* Link timeout */
    };
  BT_MSG_DEBUG( "BT PF AVDTP CMD RX: Register", 0, 0, 0 );

  bt_pf_avdtp_ext_app_id = cmd_ptr->cmd_hdr.app_id;

  OI_AVDTP_Register( &bt_pf_avdtp_callbacks,
                     &bt_pf_avdtp_l2_params );
}

/*===========================================================================

FUNCTION
   bt_pf_avdtp_cmd_deregister

DESCRIPTION
   Procces a AVDTP connect command.
===========================================================================*/
LOCAL void bt_pf_avdtp_cmd_deregister( bt_pf_cmd_msg_type* cmd_ptr )
{
  BT_MSG_DEBUG( "BT PF AVDTP CMD RX: Deregister", 0, 0, 0 );
  bt_pf_avdtp_ext_app_id = BT_APP_ID_NULL;
  OI_AVDTP_Deregister();
}

/*===========================================================================

FUNCTION
   bt_pf_avdtp_cmd_register_endpoint

DESCRIPTION
   Procces a AVDTP connect command.
===========================================================================*/
LOCAL void bt_pf_avdtp_cmd_register_endpoint( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_pf_avdtp_register_endpoint_type* cmd
    = &cmd_ptr->cmd_msg.cmd_avdtp_register_endpoint;

  static OI_AVDTP_CAP                           oi_cap[BT_PF_AVDTP_MAX_CAPS];
  static bt_cmd_pf_avdtp_register_endpoint_type saved_cmd;
  memcpy( (void*)&saved_cmd, (void*)cmd, sizeof(bt_cmd_pf_avdtp_register_endpoint_type) );
  // OI doesn't copy the caps data, must make this static.
  // Only one person can be registered as an AVDTP endpoint at a time.
  // TODO: This needs to be enforced.

  bt_pf_avdtp_copy_to_oi_cap( oi_cap, saved_cmd.caps, saved_cmd.num_caps );

  BT_MSG_DEBUG( "BT PF AVDTP CMD RX: Reg Endpoint", 0, 0, 0 );

  cmd_ptr->cmd_hdr.cmd_status =
    OI_AVDTP_RegisterEndpoint( saved_cmd.media_type,
                               saved_cmd.endpoint_type,
                               oi_cap,
                               saved_cmd.num_caps,
                               saved_cmd.seid );
}

/*===========================================================================

FUNCTION
   bt_pf_avdtp_cmd_deregister_endpoint

DESCRIPTION
   Procces a AVDTP connect command.
===========================================================================*/
LOCAL void bt_pf_avdtp_cmd_deregister_endpoint( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_pf_avdtp_deregister_endpoint_type* cmd
    = &cmd_ptr->cmd_msg.cmd_avdtp_deregister_endpoint;
  BT_MSG_DEBUG( "BT PF AVDTP CMD RX: Dereg Endpoint", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status = OI_AVDTP_DeregisterEndpoint( cmd->seid );
}

/*===========================================================================

FUNCTION
   bt_pf_avdtp_cmd_connect

DESCRIPTION
   Procces a AVDTP connect command.
===========================================================================*/
LOCAL void bt_pf_avdtp_cmd_connect( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_pf_avdtp_connect_type* cmd
    = &cmd_ptr->cmd_msg.cmd_avdtp_connect;

  OI_BD_ADDR oi_bd_addr;
  bt_pf_copy_bd_addr_to_oi( &oi_bd_addr, &cmd->bd_addr );

  BT_MSG_DEBUG( "BT PF AVDTP CMD RX: Connect", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status = OI_AVDTP_Connect( &oi_bd_addr );
}

/*===========================================================================

FUNCTION
   bt_pf_avdtp_cmd_accept

DESCRIPTION
   Procces a AVDTP connect command.
===========================================================================*/
LOCAL void bt_pf_avdtp_cmd_accept( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_pf_avdtp_accept_type* cmd
    = &cmd_ptr->cmd_msg.cmd_avdtp_accept;

  OI_BD_ADDR oi_bd_addr;
  bt_pf_copy_bd_addr_to_oi( &oi_bd_addr, &cmd->bd_addr );

  BT_MSG_DEBUG( "BT PF AVDTP CMD RX: Accept", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status = OI_AVDTP_Accept( &oi_bd_addr, cmd->accept );
}

/*===========================================================================

FUNCTION
   bt_pf_avdtp_cmd_disconnect

DESCRIPTION
   Procces a AVDTP connect command.
===========================================================================*/
LOCAL void bt_pf_avdtp_cmd_disconnect( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_pf_avdtp_disconnect_type* cmd
    = &cmd_ptr->cmd_msg.cmd_avdtp_disconnect;

  OI_BD_ADDR oi_bd_addr;
  bt_pf_copy_bd_addr_to_oi( &oi_bd_addr, &cmd->bd_addr );

  BT_MSG_DEBUG( "BT PF AVDTP CMD RX: Discon", 0, 0, 0 );

  cmd_ptr->cmd_hdr.cmd_status = OI_AVDTP_Disconnect( &oi_bd_addr );
}

/*===========================================================================

FUNCTION
   bt_pf_avdtp_cmd_discover

DESCRIPTION
   Procces a AVDTP connect command.
===========================================================================*/
LOCAL void bt_pf_avdtp_cmd_discover( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_pf_avdtp_discover_type* cmd
    = &cmd_ptr->cmd_msg.cmd_avdtp_discover;

  OI_BD_ADDR oi_bd_addr;
  bt_pf_copy_bd_addr_to_oi( &oi_bd_addr, &cmd->bd_addr );

  BT_MSG_DEBUG( "BT PF AVDTP CMD RX: Disc", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status = OI_AVDTP_Discover( &oi_bd_addr, cmd->txn );
}

/*===========================================================================

FUNCTION
   bt_pf_avdtp_cmd_get_cap

DESCRIPTION
   Procces a AVDTP connect command.
===========================================================================*/
LOCAL void bt_pf_avdtp_cmd_get_cap( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_pf_avdtp_get_cap_type* cmd
    = &cmd_ptr->cmd_msg.cmd_avdtp_get_cap;

  OI_BD_ADDR oi_bd_addr;
  bt_pf_copy_bd_addr_to_oi( &oi_bd_addr, &cmd->bd_addr );

  BT_MSG_DEBUG( "BT PF AVDTP CMD RX: Get Cap", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status =
    OI_AVDTP_GetCap( &oi_bd_addr, cmd->remote_seid, cmd->txn );
}

/*===========================================================================

FUNCTION
   bt_pf_avdtp_cmd_set_config

DESCRIPTION
   Procces a AVDTP connect command.
===========================================================================*/
LOCAL void bt_pf_avdtp_cmd_set_config( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_pf_avdtp_set_config_type* cmd
    = &cmd_ptr->cmd_msg.cmd_avdtp_set_config;
  OI_BD_ADDR oi_bd_addr;
  OI_AVDTP_CAP oi_cap[BT_PF_AVDTP_MAX_CAPS];

  if( cmd->num_caps > BT_PF_AVDTP_MAX_CAPS )
  {
    BT_ERR("BT PF AVDTP: Too many caps in set config %x",
           cmd->num_caps, 0, 0);
    cmd->num_caps = BT_PF_AVDTP_MAX_CAPS;
  }

  bt_pf_avdtp_copy_to_oi_cap( oi_cap, cmd->caps, cmd->num_caps );
  bt_pf_copy_bd_addr_to_oi( &oi_bd_addr, &cmd->bd_addr );

  BT_MSG_DEBUG( "BT PF AVDTP CMD RX: Set cfg", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status =
    OI_AVDTP_SetConfig( &oi_bd_addr,
                        cmd->remote_seid,
                        cmd->local_seid,
                        oi_cap,
                        cmd->num_caps,
                        cmd->txn,
                        cmd->handle );
}

/*===========================================================================

FUNCTION
   bt_pf_avdtp_cmd_get_config

DESCRIPTION
   Procces a AVDTP connect command.
===========================================================================*/
LOCAL void bt_pf_avdtp_cmd_get_config( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_pf_avdtp_get_config_type* cmd
    = &cmd_ptr->cmd_msg.cmd_avdtp_get_config;

  BT_MSG_DEBUG( "BT PF AVDTP CMD RX: get cfg", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status = OI_AVDTP_GetConfig( cmd->handle, cmd->txn );
}

/*===========================================================================

FUNCTION
   bt_pf_avdtp_cmd_set_config_rsp

DESCRIPTION
   Procces a AVDTP connect command.
===========================================================================*/
LOCAL void bt_pf_avdtp_cmd_set_config_rsp( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_pf_avdtp_set_config_rsp_type* cmd
    = &cmd_ptr->cmd_msg.cmd_avdtp_set_config_rsp;

  BT_MSG_DEBUG( "BT PF AVDTP CMD RX: Set cfg rsp handle: %x cat %x err %x", 
                cmd->handle,
                cmd->category,
                cmd->error );

  cmd_ptr->cmd_hdr.cmd_status =
    OI_AVDTP_SetConfigRsp( cmd->handle,
                           cmd->txn,
                           cmd->category,
                           ( cmd->error == BT_CS_GN_SUCCESS ?
                             OI_AVDTP_SUCCESS
                             : (cmd->error - BT_CMD_EV_PF_AVDTP_BASE) ) );
}

/*===========================================================================

FUNCTION
   bt_pf_avdtp_cmd_open

DESCRIPTION
   Procces a AVDTP connect command.
===========================================================================*/
LOCAL void bt_pf_avdtp_cmd_open( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_pf_avdtp_open_type* cmd
    = &cmd_ptr->cmd_msg.cmd_avdtp_open;

  BT_MSG_DEBUG( "BT PF AVDTP CMD RX: Open", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status = OI_AVDTP_Open( cmd->handle, cmd->txn );
}

/*===========================================================================

FUNCTION
   bt_pf_avdtp_cmd_start

DESCRIPTION
   Procces a AVDTP connect command.
===========================================================================*/
LOCAL void bt_pf_avdtp_cmd_start( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_pf_avdtp_start_type* cmd
    = &cmd_ptr->cmd_msg.cmd_avdtp_start;

  OI_UINT8 txn;
  OI_UINT16 handle = cmd->handle;

  BT_MSG_DEBUG( "BT PF AVDTP CMD RX: Start", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status = OI_AVDTP_Start( &handle, 1,&txn );
  if( cmd->txn != NULL )
  {
    *cmd->txn = txn;
  }
}

/*===========================================================================

FUNCTION
   bt_pf_avdtp_cmd_close

DESCRIPTION
   Procces a AVDTP connect command.
===========================================================================*/
LOCAL void bt_pf_avdtp_cmd_close( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_pf_avdtp_close_type* cmd
    = &cmd_ptr->cmd_msg.cmd_avdtp_close;
  OI_UINT8 txn;

  BT_MSG_DEBUG( "BT PF AVDTP CMD RX: Close", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status = OI_AVDTP_Close( cmd->handle, &txn );
  if( cmd->txn != NULL )
  {
    *cmd->txn = txn;
  }
}

/*===========================================================================

FUNCTION
   bt_pf_avdtp_cmd_abort

DESCRIPTION
   Procces a AVDTP connect command.
===========================================================================*/
LOCAL void bt_pf_avdtp_cmd_abort( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_pf_avdtp_abort_type* cmd
    = &cmd_ptr->cmd_msg.cmd_avdtp_abort;
  OI_UINT8 txn;

  BT_MSG_DEBUG( "BT PF AVDTP CMD RX: Abort", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status = OI_AVDTP_Abort( cmd->handle, &txn );
  if( cmd->txn != NULL )
  {
    *cmd->txn = txn;
  }
}

/*===========================================================================

FUNCTION
   bt_pf_avdtp_cmd_suspend

DESCRIPTION
   Procces a AVDTP connect command.
===========================================================================*/
LOCAL void bt_pf_avdtp_cmd_suspend( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_pf_avdtp_suspend_type* cmd
    = &cmd_ptr->cmd_msg.cmd_avdtp_suspend;
  OI_UINT8 txn;
  OI_UINT16 handle = cmd->handle;

  BT_MSG_DEBUG( "BT PF AVDTP CMD RX: Suspend", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status =
    OI_AVDTP_Suspend( &handle,
                      1,
                      &txn );
  if( cmd->txn != NULL )
  {
    *cmd->txn = txn;
  }
}

/*===========================================================================

FUNCTION
   bt_pf_avdtp_cmd_reconfigure

DESCRIPTION
   Procces a AVDTP connect command.
===========================================================================*/
LOCAL void bt_pf_avdtp_cmd_reconfigure( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_pf_avdtp_reconfigure_type* cmd
    = &cmd_ptr->cmd_msg.cmd_avdtp_reconfigure;
  OI_UINT8 txn;
  OI_UINT16 handle = cmd->handle;
  OI_AVDTP_CAP oi_cap[BT_PF_AVDTP_MAX_REMOTE_CAPS];

  bt_pf_avdtp_copy_to_oi_cap( oi_cap, cmd->caps, cmd->num_caps );

  BT_MSG_DEBUG( "BT PF AVDTP CMD RX: Reconfig", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status =
    OI_AVDTP_Reconfigure( handle,
                          oi_cap,
                          cmd->num_caps,
                          &txn );
  if( cmd->txn != NULL )
  {
    *cmd->txn = txn;
  }
}

/*===========================================================================

FUNCTION
   bt_pf_avdtp_cmd_reconfigure_rsp

DESCRIPTION
   Procces a AVDTP connect command.
===========================================================================*/
LOCAL void bt_pf_avdtp_cmd_reconfigure_rsp( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_pf_avdtp_reconfigure_rsp_type* cmd
    = &cmd_ptr->cmd_msg.cmd_avdtp_reconfigure_rsp;
  OI_UINT16 handle = cmd->handle;

  BT_MSG_DEBUG( "BT PF AVDTP CMD RX: Reconfig rsp", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status =
    OI_AVDTP_ReconfigureRsp( handle,
                             cmd->txn,
                             cmd->category,
                             cmd->error == BT_CS_GN_SUCCESS ?
                             OI_AVDTP_SUCCESS
                             : (cmd->error - BT_CMD_EV_PF_AVDTP_BASE) );
}

/*===========================================================================

FUNCTION
   bt_pf_avdtp_cmd_send_with_header

DESCRIPTION
   Procces a AVDTP connect command.
===========================================================================*/
LOCAL void bt_pf_avdtp_cmd_send_data( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_pf_avdtp_send_data_type* cmd
    = &cmd_ptr->cmd_msg.cmd_avdtp_send_data;
  dsm_item_type *dsm = cmd->data;

  cmd_ptr->cmd_hdr.cmd_status =
    OI_AVDTP_SendWithHeader_Ex( cmd->handle,
                                cmd->seq_num,
                                cmd->timestamp,
                                cmd->payload_type,
                                cmd->marker,
                                dsm,
                                NULL,
                                cmd->frame_handle );
  if( cmd_ptr->cmd_hdr.cmd_status != BT_CS_GN_SUCCESS )
  {
    dsm_free_packet( &dsm );
  }
}

/*===========================================================================

FUNCTION
   bt_pf_avdtp_cmd_security_control

DESCRIPTION
   Procces a AVDTP connect command.
===========================================================================*/
LOCAL void bt_pf_avdtp_cmd_security_control( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_pf_avdtp_security_control_type* cmd
    = &cmd_ptr->cmd_msg.cmd_avdtp_security_control;
  OI_UINT8 txn = 0xFF; //initialising with some invalid value

  BT_MSG_DEBUG( "BT PF AVDTP CMD RX: Sec ctrl", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status =
    OI_AVDTP_SecurityControl_Ex( cmd->handle, cmd->data, &txn );
  if( cmd->txn != NULL )
  {
    *cmd->txn = txn;
  }
}

/*===========================================================================

FUNCTION
   bt_pf_avdtp_cmd_security_control_rsp

DESCRIPTION
   Procces a AVDTP connect command.
===========================================================================*/
LOCAL void bt_pf_avdtp_cmd_security_control_rsp( bt_pf_cmd_msg_type* cmd_ptr )
{
  bt_cmd_pf_avdtp_security_control_rsp_type* cmd
    = &cmd_ptr->cmd_msg.cmd_avdtp_security_control_rsp;

  BT_MSG_DEBUG( "BT PF AVDTP CMD RX: Sec ctrl rsp", 0, 0, 0 );
  cmd_ptr->cmd_hdr.cmd_status =
    OI_AVDTP_SecurityControlRsp_Ex( cmd->handle,
                                    cmd->txn,
                                    cmd->data,
                                    cmd->error == BT_CS_GN_SUCCESS ?
                                    OI_AVDTP_SUCCESS
                                    : (cmd->error - BT_CMD_EV_PF_AVDTP_BASE) );
}

/*===========================================================================

ADDITIONAL OI_AVDTP DEFINITIONS

===========================================================================*/
/*===========================================================================

FUNCTION
   OI_AVDTP_SendWithHeader_Ex

DESCRIPTION
   Identical to OI_AVDTP_SendWithHeader, but uses mbufs instead of
   byte pointers.
===========================================================================*/
OI_STATUS OI_AVDTP_SendWithHeader_Ex(OI_UINT16 handle,
                                     OI_UINT16 seqNum,
                                     OI_UINT32 timestamp,
                                     OI_UINT8 payloadType,
                                     OI_UINT16 marker,
                                     dsm_item_type *data,
                                     dsm_item_type *app_header,
                                     void *frame_handle)
{
  AVDTP_ENDPOINT *endpoint;
  OI_BYTE_STREAM *header;
  OI_MBUF *mbuf;
  OI_STATUS result;

  OI_CHECK_INIT(AVDTP);

  endpoint = GetEndpointBySignalCID(handle);
  if (NULL == endpoint) {
    return OI_STATUS_NOT_FOUND;
  }

  if (endpoint->state != AVDTP_STATE_STREAMING) {
    return OI_STATUS_INVALID_STATE;
  }

  //Create the header
  header = ByteStream_Alloc(AVDTP_MEDIA_HEADER_LEN);
  if (NULL == header) {
    return OI_STATUS_OUT_OF_MEMORY;
  }

  ByteStream_Open(*header, BYTESTREAM_WRITE);
  ByteStream_PutUINT8(*header, (2 << 6));
  ByteStream_PutUINT8(*header, (marker ? BIT7 : 0) | (payloadType & 0x7F) );
  ByteStream_PutUINT16(*header, seqNum, OI_BIG_ENDIAN_BYTE_ORDER);
  ByteStream_PutUINT32(*header, timestamp, OI_BIG_ENDIAN_BYTE_ORDER);
  ByteStream_PutUINT32(*header, 0, OI_BIG_ENDIAN_BYTE_ORDER); //SSRC
  ByteStream_Close(*header);

  mbuf = OI_MBUF_Alloc(3); //hdr + data
  if (NULL == mbuf) {
    ByteStream_Free(header);
    return OI_STATUS_OUT_OF_MEMORY;
  }

  //Add Data
  //Add application header

  if (app_header) {
    mbuf->dsm_ptr = app_header;
    dsm_append( &mbuf->dsm_ptr, &data );
  }
  else
  {
    // No header
    mbuf->dsm_ptr = data;
  }

  //Add AVDTP header
  result = OI_MBUF_Prepend(mbuf,
                           ByteStream_GetDataPointer(*header),
                           ByteStream_GetSize(*header),
                           MBUF_FREE);
  OI_ASSERT(OI_SUCCESS(result));

  if ( OI_L2CAP_IsGroupCID(endpoint->mediaCID) ){
    OI_DBGPRINT(("Broadcast packet Sent!\n"));
  }else{
    OI_DBGPRINT(("P-to-P packet Sent!\n"));
  }
  result = OI_L2CAP_WriteMBUF(MediaWriteMBUFCfm, endpoint->mediaCID, mbuf, frame_handle);
  if (!OI_SUCCESS(result)) {
    /* JTL: make sure not to free the DSM twice. Remove the DSM from the
     * mbuf before freeing the mbuf, since the DSM will be freed when
     * this function returns. */
    mbuf->dsm_ptr = NULL;
    OI_MBUF_Free(mbuf);
  }
  OI_Free(header); //we don't need the bytestream anymore, just its data
  return result;
}

/*===========================================================================

FUNCTION
   OI_AVDTP_SecurityControl_Ex

DESCRIPTION
   Identical to OI_AVDTP_SecurityControl, but uses mbufs instead of
   byte pointers.
===========================================================================*/
OI_STATUS OI_AVDTP_SecurityControl_Ex(OI_UINT16 handle,
                                      dsm_item_type *data,
                                      OI_UINT8 *transaction)
{
  BT_ERR("AVDTP: Security CTL unimplemented fcn",0,0,0);
  return OI_OK;
}

/*===========================================================================

FUNCTION
   OI_AVDTP_SecurityControlRsp_Ex

DESCRIPTION
   Identical to OI_AVDTP_SecurityControlRsp, but uses mbufs instead of
   byte pointers.
===========================================================================*/
OI_STATUS OI_AVDTP_SecurityControlRsp_Ex(OI_UINT16 handle,
                                         OI_UINT8 transaction,
                                         dsm_item_type *data,
                                         OI_UINT8 error)
{
  BT_ERR("AVDTP: Security CTL unimplemented fcn",0,0,0);
  return OI_OK;
}

/*===========================================================================

                     EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/
void bt_pf_avdtp_process_command( bt_pf_cmd_msg_type* cmd_ptr )
{
  cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  switch ( cmd_ptr->cmd_hdr.cmd_type )
  {
    case BT_PF_CMD_AVDTP_REGISTER:
      bt_pf_avdtp_cmd_register( cmd_ptr );
      break;
    case BT_PF_CMD_AVDTP_DEREGISTER:
      bt_pf_avdtp_cmd_deregister( cmd_ptr );
      break;
    case BT_PF_CMD_AVDTP_REGISTER_ENDPOINT:
      bt_pf_avdtp_cmd_register_endpoint( cmd_ptr );
      break;
    case BT_PF_CMD_AVDTP_DEREGISTER_ENDPOINT:
      bt_pf_avdtp_cmd_deregister_endpoint( cmd_ptr );
      break;
    case BT_PF_CMD_AVDTP_CONNECT:
      bt_pf_avdtp_cmd_connect( cmd_ptr );
      break;
    case BT_PF_CMD_AVDTP_ACCEPT:
      bt_pf_avdtp_cmd_accept( cmd_ptr );
      break;
    case BT_PF_CMD_AVDTP_DISCONNECT:
      bt_pf_avdtp_cmd_disconnect( cmd_ptr );
      break;
    case BT_PF_CMD_AVDTP_DISCOVER:
      bt_pf_avdtp_cmd_discover( cmd_ptr );
      break;
    case BT_PF_CMD_AVDTP_GET_CAP:
      bt_pf_avdtp_cmd_get_cap( cmd_ptr );
      break;
    case BT_PF_CMD_AVDTP_SET_CONFIG:
      bt_pf_avdtp_cmd_set_config( cmd_ptr );
      break;
    case BT_PF_CMD_AVDTP_GET_CONFIG:
      bt_pf_avdtp_cmd_get_config( cmd_ptr );
      break;
    case BT_PF_CMD_AVDTP_SET_CONFIG_RSP:
      bt_pf_avdtp_cmd_set_config_rsp( cmd_ptr );
      break;
    case BT_PF_CMD_AVDTP_OPEN:
      bt_pf_avdtp_cmd_open( cmd_ptr );
      break;
    case BT_PF_CMD_AVDTP_START:
      bt_pf_avdtp_cmd_start( cmd_ptr );
      break;
    case BT_PF_CMD_AVDTP_CLOSE:
      bt_pf_avdtp_cmd_close( cmd_ptr );
      break;
    case BT_PF_CMD_AVDTP_ABORT:
      bt_pf_avdtp_cmd_abort( cmd_ptr );
      break;
    case BT_PF_CMD_AVDTP_SUSPEND:
      bt_pf_avdtp_cmd_suspend( cmd_ptr );
      break;
    case BT_PF_CMD_AVDTP_RECONFIGURE:
      bt_pf_avdtp_cmd_reconfigure( cmd_ptr );
      break;
    case BT_PF_CMD_AVDTP_RECONFIGURE_RSP:
      bt_pf_avdtp_cmd_reconfigure_rsp( cmd_ptr );
      break;
    case BT_PF_CMD_AVDTP_SEND_DATA:
      bt_pf_avdtp_cmd_send_data( cmd_ptr );
      break;
    case BT_PF_CMD_AVDTP_SECURITY_CONTROL:
      bt_pf_avdtp_cmd_security_control( cmd_ptr );
      break;
    case BT_PF_CMD_AVDTP_SECURITY_CONTROL_RSP:
      bt_pf_avdtp_cmd_security_control_rsp( cmd_ptr );
      break;

    default:
      BT_ERR( "BT PF AVDTP CMD RX: Unk cmd: %x",
              cmd_ptr->cmd_hdr.cmd_type, 0, 0);
      cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_UNRECOGNIZED_CMD;
  }
}

#endif /* FEATURE_BT */
#endif /* FEATURE_BT_EXTPF_AV */
