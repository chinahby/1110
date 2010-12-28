/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

B L U E T O O T H   A V   R E M O T E   C O N T R O L   P R O T O C O L

GENERAL DESCRIPTION
  This module contains the Bluetooth AVRCP interface (commands and events)
  implementation.

  Copyright (c) 2005-2008 by QUALCOMM Incorporated. 
           All Rights Reserved.
  Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  when        who  what, where, why
 ----------   ---  -----------------------------------------------------------
 2008-09-16    hs   Fixed the compiler warnings.
 2008-07-02    hs   Fixed the bug about supported charactersets.
 2008-05-28    hs   Added the support for AVRCP 1.3 meta data feature.
               
*
*   #14      22 Feb 2007          GS
* Fixed critical lint error.
*
*   #13      23 Jan 2007          JTL
* Reject passthrough commands with unknown opcodes.
*
*   #12      21 Jun 2006          JH
* Unregister with SD and free app ID when disabling AVRCP.
*
*   #11      19 May 2006          JTL
* Changing vendor spec commands/events to use DSMs.
*
*   #10      03 Mar 2006          JTL
* Add "break;" to stop printing erroneous errors.
*
*   #9       21 Dec 2005          JTL
* Generate a disconnected event if we get a disconnect command when already
* disconnected.
*
*   #8        9 Nov 2005          DP
* Now use new SD service registration command soas to not
* be connectable solely for the SD service registration.
*
*   #6-7     17 Aug 2005          JTL
* Adding status to AVRCP replies.
*
*   #3-4     23 Jun 2005          JTL
* Fix enum value in send_reply.
* Added callback function for events.
* Added support for service registration: SDB needs to know which categories
* are supported.
*
*   #1-2     27 Apr 2005          JTL
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
#include "btsd.h"

#include "dsm.h"
#include "oi_avrcp.h"
#include "oi_avrcp_consts.h"

/* Open Interface Source files */
#include "avrcp.c"
#undef __OI_MODULE__
#include "avrcp_target.c"
#undef __OI_MODULE__
#include "avrcp_metadata.c"
#undef __OI_MODULE__
#include "avrcp_controller.c"
#undef __OI_MODULE__
#include "avrcp_helper.c"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

/*===========================================================================

                    INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*-------------------------------------------------------------------------*/
/*  Version number of AVRCP                                                */
/*-------------------------------------------------------------------------*/
#ifdef FEATURE_BT_AVRCP_13
#define BT_PF_AVRCP_VERSION_NUMBER 0x0103
#else 
#define BT_PF_AVRCP_VERSION_NUMBER 0x0100
#endif

#define BT_PF_AVCTP_PSM 0x0017

#define BT_MSG_LAYER  BT_MSG_GN


/*-------------------------------------------------------------------------*/
/*  This app id is the ID of the app using AVRCP                           */
/*-------------------------------------------------------------------------*/
LOCAL bt_app_id_type bt_pf_avrcp_ext_app_id = BT_APP_ID_NULL;

/*-------------------------------------------------------------------------*/
/*  This app id is for internal use for commands to the driver             */
/*-------------------------------------------------------------------------*/
LOCAL bt_app_id_type bt_pf_avrcp_int_app_id = BT_APP_ID_NULL;

/*-------------------------------------------------------------------------*/
/*  Bluetooth address of the remote device                                 */
/*-------------------------------------------------------------------------*/
LOCAL bt_bd_addr_type bt_pf_avrcp_bd_addr = BT_PF_BD_ADDR_NULL;

/*-------------------------------------------------------------------------*/
/*  Supported categories                                                   */
/*-------------------------------------------------------------------------*/
LOCAL uint16 bt_pf_avrcp_supported_categories;

#ifdef FEATURE_BT_AVRCP_13

/*-------------------------------------------------------------------------*/
/*  This app id is the ID of the app using AVRCP Metadata 1.3              */
/*-------------------------------------------------------------------------*/
LOCAL bt_app_id_type bt_pf_avrcp_13_ext_app_id = BT_APP_ID_NULL;

/*-------------------------------------------------------------------------*/
/*  List of spec supported media attributes                                */
/*-------------------------------------------------------------------------*/
LOCAL uint32 bt_pf_avrcp_media_attributes[] = {
    BT_PF_AVRCP_MEDIA_ATTR_ILLEGAL,
    BT_PF_AVRCP_MEDIA_ATTR_TITLE,
    BT_PF_AVRCP_MEDIA_ATTR_ARTIST,
    BT_PF_AVRCP_MEDIA_ATTR_ALBUM,
    BT_PF_AVRCP_MEDIA_ATTR_TRACK_NUMBER,
    BT_PF_AVRCP_MEDIA_ATTR_TOTAL_TRACK_NUMBER,
    BT_PF_AVRCP_MEDIA_ATTR_GENRE,
    BT_PF_AVRCP_MEDIA_ATTR_PLAYING_TIME
};

/*-------------------------------------------------------------------------*/
/* Max number of player attributes defined by AVRCP 1.3 Bluetooth spec     */
/*-------------------------------------------------------------------------*/
#define BT_PF_AVRCP_MAX_PLAYER_APP_SETTINGS          4

/*---------------------------------------------------------------------------*/
/* Max number of player attribute values defined by AVRCP 1.3 Bluetooth spec */
/*---------------------------------------------------------------------------*/
#define BT_PF_AVRCP_MAX_PLAYER_ATTR_VAL_SETTINGS     4

/*-------------------------------------------------------------------------*/
/* Max number of media attributes defined by AVRCP 1.3 Bluetooth spec      */
/*-------------------------------------------------------------------------*/
#define BT_PF_AVRCP_MAX_MEDIA_ATTRIBUTES \
       (OI_ARRAYSIZE(bt_pf_avrcp_media_attributes) - 1)

/*-------------------------------------------------------------------------*/
/* Structue for player attributes defined by AVRCP 1.3 Bluetooth spec      */
/*-------------------------------------------------------------------------*/
LOCAL OI_AVRCP_ATTRIBUTE_TEXT
      bt_pf_avrcp_player_settings[BT_PF_AVRCP_MAX_PLAYER_APP_SETTINGS];

/*-------------------------------------------------------------------------*/
/* Structue for player attribute val defined by AVRCP 1.3 Bluetooth spec   */
/*-------------------------------------------------------------------------*/
LOCAL OI_AVRCP_ATTRIBUTE_TEXT
      bt_pf_avrcp_player_attr_val_settings[BT_PF_AVRCP_MAX_PLAYER_ATTR_VAL_SETTINGS];

/*-------------------------------------------------------------------------*/
/* Structue for media attributes defined by AVRCP 1.3 Bluetooth spec      */
/*-------------------------------------------------------------------------*/
LOCAL OI_AVRCP_ELEMENT_ATTRIBUTE 
      bt_pf_avrcp_media_elements[BT_PF_AVRCP_MAX_MEDIA_ATTRIBUTES];

/*-------------------------------------------------------------------------*/
/* Structue for notification events defined by AVRCP 1.3 Bluetooth spec      */
/*-------------------------------------------------------------------------*/
LOCAL OI_AVRCP_EVENT                       bt_pf_avrcp_notify_ev_metadata;

/*-------------------------------------------------------------------------*/
/* Structue to keep the fragments in case of fragmentation                 */
/*-------------------------------------------------------------------------*/
typedef struct {
  OI_AVRCP_FRAGMENT_DATA base;
  OI_BD_ADDR             addr;
  boolean                is_pending;
  uint16                 count;
  uint8                  pdu_id;
} bt_pf_avrcp_fragment_info_type;

LOCAL bt_pf_avrcp_fragment_info_type bt_pf_avrcp_fragment_info;

/*-------------------------------------------------------------------------*/
/* Variable to keep track of pending fragmentation responses               */
/*-------------------------------------------------------------------------*/
LOCAL uint32  bt_pf_avrcp_num_frag_pending;      

#endif /* FEATURE_BT_AVRCP_13 */
/*-------------------------------------------------------------------------*/
/*  Mapping from spec AVRCP commands to internal events                    */
/*-------------------------------------------------------------------------*/
typedef struct bt_pf_avrcp_event_map_struct
{
  uint32        spec_value;
  bt_event_type driver_value;
} bt_pf_avrcp_event_map_type;

LOCAL const bt_pf_avrcp_event_map_type bt_pf_avrcp_event_map[] =
  { { OI_AVRCP_OP_SELECT             , BT_EV_PF_AVRCP_OP_SELECT },
    { OI_AVRCP_OP_UP                 , BT_EV_PF_AVRCP_OP_UP },
    { OI_AVRCP_OP_DOWN               , BT_EV_PF_AVRCP_OP_DOWN },
    { OI_AVRCP_OP_LEFT               , BT_EV_PF_AVRCP_OP_LEFT },
    { OI_AVRCP_OP_RIGHT              , BT_EV_PF_AVRCP_OP_RIGHT },
    { OI_AVRCP_OP_RIGHT_UP           , BT_EV_PF_AVRCP_OP_RIGHT_UP },
    { OI_AVRCP_OP_RIGHT_DOWN         , BT_EV_PF_AVRCP_OP_RIGHT_DOWN },
    { OI_AVRCP_OP_LEFT_UP            , BT_EV_PF_AVRCP_OP_LEFT_UP },
    { OI_AVRCP_OP_LEFT_DOWN          , BT_EV_PF_AVRCP_OP_LEFT_DOWN },
    { OI_AVRCP_OP_ROOT_MENU          , BT_EV_PF_AVRCP_OP_ROOT_MENU },
    { OI_AVRCP_OP_SETUP_MENU         , BT_EV_PF_AVRCP_OP_SETUP_MENU },
    { OI_AVRCP_OP_CONTENTS_MENU      , BT_EV_PF_AVRCP_OP_CONTENTS_MENU },
    { OI_AVRCP_OP_FAVORITE_MENU      , BT_EV_PF_AVRCP_OP_FAVORITE_MENU },
    { OI_AVRCP_OP_EXIT               , BT_EV_PF_AVRCP_OP_EXIT },
    { OI_AVRCP_OP_0                  , BT_EV_PF_AVRCP_OP_0 },
    { OI_AVRCP_OP_1                  , BT_EV_PF_AVRCP_OP_1 },
    { OI_AVRCP_OP_2                  , BT_EV_PF_AVRCP_OP_2 },
    { OI_AVRCP_OP_3                  , BT_EV_PF_AVRCP_OP_3 },
    { OI_AVRCP_OP_4                  , BT_EV_PF_AVRCP_OP_4 },
    { OI_AVRCP_OP_5                  , BT_EV_PF_AVRCP_OP_5 },
    { OI_AVRCP_OP_6                  , BT_EV_PF_AVRCP_OP_6 },
    { OI_AVRCP_OP_7                  , BT_EV_PF_AVRCP_OP_7 },
    { OI_AVRCP_OP_8                  , BT_EV_PF_AVRCP_OP_8 },
    { OI_AVRCP_OP_9                  , BT_EV_PF_AVRCP_OP_9 },
    { OI_AVRCP_OP_DOT                , BT_EV_PF_AVRCP_OP_DOT },
    { OI_AVRCP_OP_ENTER              , BT_EV_PF_AVRCP_OP_ENTER },
    { OI_AVRCP_OP_CLEAR              , BT_EV_PF_AVRCP_OP_CLEAR },
    { OI_AVRCP_OP_CHANNEL_UP         , BT_EV_PF_AVRCP_OP_CHANNEL_UP },
    { OI_AVRCP_OP_CHANNEL_DOWN       , BT_EV_PF_AVRCP_OP_CHANNEL_DOWN },
    { OI_AVRCP_OP_PREVIOUS_CHANNEL   , BT_EV_PF_AVRCP_OP_PREVIOUS_CHANNEL },
    { OI_AVRCP_OP_SOUND_SELECT       , BT_EV_PF_AVRCP_OP_SOUND_SELECT },
    { OI_AVRCP_OP_INPUT_SELECT       , BT_EV_PF_AVRCP_OP_INPUT_SELECT },
    { OI_AVRCP_OP_DISPLAY_INFORMATION, BT_EV_PF_AVRCP_OP_DISPLAY_INFORMATION },
    { OI_AVRCP_OP_HELP               , BT_EV_PF_AVRCP_OP_HELP },
    { OI_AVRCP_OP_PAGE_UP            , BT_EV_PF_AVRCP_OP_PAGE_UP },
    { OI_AVRCP_OP_PAGE_DOWN          , BT_EV_PF_AVRCP_OP_PAGE_DOWN },
    { OI_AVRCP_OP_POWER              , BT_EV_PF_AVRCP_OP_POWER },
    { OI_AVRCP_OP_VOLUME_UP          , BT_EV_PF_AVRCP_OP_VOLUME_UP },
    { OI_AVRCP_OP_VOLUME_DOWN        , BT_EV_PF_AVRCP_OP_VOLUME_DOWN },
    { OI_AVRCP_OP_MUTE               , BT_EV_PF_AVRCP_OP_MUTE },
    { OI_AVRCP_OP_PLAY               , BT_EV_PF_AVRCP_OP_PLAY },
    { OI_AVRCP_OP_STOP               , BT_EV_PF_AVRCP_OP_STOP },
    { OI_AVRCP_OP_PAUSE              , BT_EV_PF_AVRCP_OP_PAUSE },
    { OI_AVRCP_OP_RECORD             , BT_EV_PF_AVRCP_OP_RECORD },
    { OI_AVRCP_OP_REWIND             , BT_EV_PF_AVRCP_OP_REWIND },
    { OI_AVRCP_OP_FAST_FORWARD       , BT_EV_PF_AVRCP_OP_FAST_FORWARD },
    { OI_AVRCP_OP_EJECT              , BT_EV_PF_AVRCP_OP_EJECT },
    { OI_AVRCP_OP_FORWARD            , BT_EV_PF_AVRCP_OP_FORWARD },
    { OI_AVRCP_OP_BACKWARD           , BT_EV_PF_AVRCP_OP_BACKWARD },
    { OI_AVRCP_OP_ANGLE              , BT_EV_PF_AVRCP_OP_ANGLE },
    { OI_AVRCP_OP_SUBPICTURE         , BT_EV_PF_AVRCP_OP_SUBPICTURE },
    { OI_AVRCP_OP_F1                 , BT_EV_PF_AVRCP_OP_F1 },
    { OI_AVRCP_OP_F2                 , BT_EV_PF_AVRCP_OP_F2 },
    { OI_AVRCP_OP_F3                 , BT_EV_PF_AVRCP_OP_F3 },
    { OI_AVRCP_OP_F4                 , BT_EV_PF_AVRCP_OP_F4 },
    { OI_AVRCP_OP_F5                 , BT_EV_PF_AVRCP_OP_F5 },
    { OI_AVRCP_OP_VENDOR_UNIQUE      , BT_EV_PF_AVRCP_OP_GENERIC },
  };

#ifdef FEATURE_BT_AVRCP_13

/*===========================================================================
                    AVRCP METADATA INTERNAL FUNCTIONS
===========================================================================*/

/*===========================================================================

FUNCTION
  bt_pf_avrcp_target_validate_attr_vals

DESCRIPTION
  It validates the player attribute settings values

===========================================================================*/
LOCAL void bt_pf_avrcp_target_validate_attr_vals( uint8 attr_id, 
                                                  uint8 attr_val, 
                                                  uint8* status )
{
  switch( attr_id )
  {
    case OI_AVRCP_SETTING_EQUALIZER_STATUS:
      if ( (attr_val == OI_AVRCP_EQUALIZER_STATUS_OFF) ||
           (attr_val == OI_AVRCP_EQUALIZER_STATUS_ON) )
      {
        *status = TRUE;
      }
      break;
    case OI_AVRCP_SETTING_REPEAT_MODE_STATUS:
      if ( (attr_val >= OI_AVRCP_REPEAT_MODE_OFF) &&
           (attr_val <= OI_AVRCP_REPEAT_MODE_GROUP) )
      {
        *status = TRUE;
      }
      break;
    case OI_AVRCP_SETTING_SHUFFLE_STATUS:
      if ( (attr_val >= OI_AVRCP_SHUFFLE_MODE_OFF) &&
           (attr_val <= OI_AVRCP_SHUFFLE_MODE_GROUP) )
      {
        *status = TRUE;
      }
      break;
    case OI_AVRCP_SETTING_SCAN_STATUS:
     if ( (attr_val >= OI_AVRCP_SCAN_MODE_OFF) ||
           (attr_val <= OI_AVRCP_SCAN_MODE_GROUP) )
      {
        *status = TRUE;
      }
      break;
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_free_memory

DESCRIPTION
  It frees all the dynamically allocated memory

===========================================================================*/
LOCAL void bt_pf_avrcp_free_memory( void )
{
  uint8                      *avrcp_attr;
  OI_AVRCP_ELEMENT_ATTRIBUTE *avrcp_media_element;
  OI_AVRCP_ATTRIBUTE_TEXT    *avrcp_player_attr;
  uint8                       i;

  switch( bt_pf_avrcp_fragment_info.pdu_id )
  {
    case OI_AVRCP_GET_CAPABILITIES:
    case OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES:
    case OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_VALUES:      
    case OI_AVRCP_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE:
    case OI_AVRCP_SET_PLAYER_APPLICATION_SETTING_VALUE:   
    case OI_AVRCP_REGISTER_NOTIFICATION:
      avrcp_attr = (uint8*) bt_pf_avrcp_fragment_info.base.attr;
      if ( avrcp_attr != NULL ) 
      {
        OI_Free( avrcp_attr );
        avrcp_attr = NULL;
      } 
      break;

    case OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT:
    case OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT:
      avrcp_player_attr = (OI_AVRCP_ATTRIBUTE_TEXT* ) 
      bt_pf_avrcp_fragment_info.base.attr;
      for ( i = 0; i < bt_pf_avrcp_fragment_info.count; i++ ) 
      {
        if( avrcp_player_attr[i].value.p != NULL )
        {
          OI_Free( avrcp_player_attr[i].value.p );
          avrcp_player_attr[i].value.p = NULL;
        }
      }
      break;
        
  case OI_AVRCP_GET_ELEMENT_ATTRIBUTES:
      avrcp_media_element =  (OI_AVRCP_ELEMENT_ATTRIBUTE* )
        bt_pf_avrcp_fragment_info.base.attr;
      for ( i = 0; i < bt_pf_avrcp_fragment_info.count; i++ ) 
      {
        if( avrcp_media_element[i].value.p != NULL )
        {
          OI_Free( avrcp_media_element[i].value.p );
          avrcp_media_element[i].value.p = NULL;
        }
      }
      break;
  default:
      BT_ERR("BT PF AVRCP: unknown PDU", 0, 0, 0);

  }
}
/*===========================================================================

FUNCTION
  bt_pf_avrcp_clear_pending_response

DESCRIPTION
  It clears the pending fragments from the memory

===========================================================================*/
LOCAL void bt_pf_avrcp_clear_pending_response( OI_BD_ADDR *addr )
{
  OI_BD_ADDR zero_addr = { {0,0,0,0,0,0} };

  if (SAME_BD_ADDR(&bt_pf_avrcp_fragment_info.addr, addr)) 
  {
    if( bt_pf_avrcp_fragment_info.is_pending == TRUE ) 
    {
      bt_pf_avrcp_fragment_info.is_pending = FALSE;
      bt_pf_avrcp_fragment_info.addr = zero_addr;

      if( bt_pf_avrcp_num_frag_pending > 0) 
      {
        bt_pf_avrcp_num_frag_pending--;
      }

      bt_pf_avrcp_free_memory();
      
      BT_MSG_DEBUG( "BT PF AVRCP: clearing pending response pdu id: %x",
                    bt_pf_avrcp_fragment_info.pdu_id, 0, 0 );
    }
  }
  else
  {
    BT_ERR( "BT PF AVRCP: Invalid BD Address ",0,0,0 );
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_get_pending_response

DESCRIPTION
  It will check the validity of continue reply request from the CT

===========================================================================*/
LOCAL boolean bt_pf_avrcp_get_pending_response( OI_BD_ADDR *addr,
                                                uint8 pdu_id, 
                                                uint16 curr_attr_index, 
                                                uint16 curr_data_pos, 
                                                OI_AVRCP_FRAGMENT_DATA *frag_msg 
                                               )                                         
{
  if (SAME_BD_ADDR(&bt_pf_avrcp_fragment_info.addr, addr)) 
  {
    if (bt_pf_avrcp_fragment_info.is_pending == TRUE) 
    {
       /* Check PDU */
      if (pdu_id != bt_pf_avrcp_fragment_info.pdu_id) 
      {
        BT_ERR( "BT PF AVRCP: PDU does not match: 0x%x (should be 0x%x)", 
                pdu_id, bt_pf_avrcp_fragment_info.pdu_id, 0 );
        return FALSE;
      }

     /* In our case, target's full reply fits into the application and BT 
        driver memory so application or BT Pf layer doesn't need to fragment
        and can send the complete reply in one packet.

        The profile will take care of fagmentation. The start data pointer 
        is already given to the stack so it will calculate and send the pending 
        fragments automatically. No need to give the data and index pointer of 
        remaining fragments. It can be changed for the targets if there is any 
        memory constraint due to which app or BT Pf need to fragment the reply*/

      frag_msg->attr = NULL;
      frag_msg->data = NULL;
      return TRUE;
    }
  }
  else
  {
    BT_ERR( "BT PF AVRCP: Invalid BD Address ",0,0,0 );
  }
  return FALSE;
}

#endif /* FEATURE_BT_AVRCP_13 */

/*===========================================================================

                    AVRCP CALLBACK FUNCTIONS
            These functions handle AVRCP callbacks from the OI layer

===========================================================================*/

/*===========================================================================

FUNCTION
  bt_pf_avrcp_connect_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avrcp_connect_ind( OI_BD_ADDR *addr )
{
  // Accept this connection
  // We should receive a connect_cfm event below once connected.
  OI_AVRCP_Accept( addr, TRUE );
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_connect_cfm

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avrcp_connect_cfm( OI_BD_ADDR *addr, OI_STATUS result )
{
  bt_ev_msg_type event;
  
  event.ev_hdr.bt_app_id = bt_pf_avrcp_ext_app_id;
  if( result == OI_OK )
  {
    // Send a connected event
    event.ev_hdr.ev_type = BT_EV_PF_AVRCP_CON;
    bt_pf_copy_bd_addr_from_oi(&bt_pf_avrcp_bd_addr, addr);
    bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_avrcp_con.bd_addr, addr);
#ifdef FEATURE_BT_AVRCP_13
    bt_pf_copy_bd_addr_to_oi(&bt_pf_avrcp_fragment_info.addr, &bt_pf_avrcp_bd_addr);
#endif /* FEATURE_BT_AVRCP_13 */
    BT_MSG_DEBUG("BT PF AVRCP TX EV: con. AppID: %x",
                 bt_pf_avrcp_ext_app_id,0,0);
  }
  else
  {
    // Send a connected event
    event.ev_hdr.ev_type = BT_EV_PF_AVRCP_CON_FAILED;
    bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_avrcp_con_failed.bd_addr, addr);
    event.ev_msg.ev_avrcp_con_failed.reason = result;
    BT_MSG_DEBUG("BT PF AVRCP TX EV: con failed. AppID: %x",
                 bt_pf_avrcp_ext_app_id,0,0);
  }

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_disconnect_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avrcp_disconnect_ind( OI_BD_ADDR *addr, OI_STATUS reason )
{
  bt_ev_msg_type            event;

  event.ev_hdr.bt_app_id = bt_pf_avrcp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVRCP_DISCON;
  bt_pf_copy_bd_addr_from_oi(&event.ev_msg.ev_avrcp_discon.bd_addr, addr);
  event.ev_msg.ev_avrcp_discon.reason = reason;

  BT_MSG_DEBUG("BT PF AVRCP TX EV: discon AppID: %x",
               bt_pf_avrcp_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_target_command_ind

DESCRIPTION
  A callback function of this type indicates that an AVRCP command has
  been sent. The device should respond immediately with
  OI_AVRCPTarget_SendReply().

===========================================================================*/
LOCAL void bt_pf_avrcp_target_command_ind( OI_BD_ADDR *addr, 
                                           OI_UINT8 transaction, 
                                           OI_UINT8 op)
{
  bt_ev_msg_type            event;
  uint32                    i;
  uint32                    opcode;
  uint8                     avrcp_evt_map_size;

  event.ev_hdr.bt_app_id = bt_pf_avrcp_ext_app_id;
  
  opcode = OI_AVRCP_OP_CODE(op);

  avrcp_evt_map_size = 
    sizeof(bt_pf_avrcp_event_map) / sizeof(bt_pf_avrcp_event_map_type);
  for( i = 0; i < avrcp_evt_map_size; i++ )
  {
    if( bt_pf_avrcp_event_map[i].spec_value == opcode )
    {
      break;
    }
  }

  if ( i >= avrcp_evt_map_size) 
  {
    BT_ERR ("BT PF AVRCP: bad opcode in command_ind %x", op, 0, 0);
    event.ev_hdr.ev_type = BT_EV_PF_AVRCP_OP_GENERIC;
  }
  else
  {
    event.ev_hdr.ev_type = bt_pf_avrcp_event_map[i].driver_value;
  }

  event.ev_msg.ev_avrcp_op.txn_id = transaction;
  if( OI_AVRCP_OP_IS_PRESS(op) )
  {
    event.ev_msg.ev_avrcp_op.status = BT_PF_AVRCP_OP_PRESSED;
  }
  else if( OI_AVRCP_OP_IS_RELEASE(op) )
  {
    event.ev_msg.ev_avrcp_op.status = BT_PF_AVRCP_OP_RELEASED;
  }
  else
  {
    BT_ERR("BT PF AVRCP: command is neither press nor release",0,0,0);
  }

  if( event.ev_hdr.ev_type == BT_EV_PF_AVRCP_OP_GENERIC )
  {
    BT_ERR("BT PF AVRCP: RX invalid passthrough command %x", op, 0, 0 );
    OI_AVRCPTarget_SendReply( addr,
                              transaction,
                              op,
                              BT_PF_AVRCP_RESPONSE_REJECTED );
  }
  else
  {
    BT_MSG_DEBUG("BT PF AVRCP TX EV: raw op %x AppID: %x cooked event %x",
                 op, bt_pf_avrcp_ext_app_id, event.ev_hdr.ev_type );
    
    bt_ec_send_event( &event );
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_target_generic_ind

DESCRIPTION
  A callback function of this type indicates that a vendor-dependent
  AVRCP command has been sent.  If this callback is defined, the
  device should respond with OI_AVRCPTarget_SendReply().

===========================================================================*/
LOCAL void bt_pf_avrcp_target_generic_ind( OI_BD_ADDR *addr, 
                                           OI_UINT8 transaction, 
                                           OI_AVRCP_FRAME *frame)
{
  bt_ev_msg_type   event;
  dsm_item_type   *dsm_ptr;

  
  event.ev_hdr.bt_app_id = bt_pf_avrcp_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVRCP_OP_GENERIC;
  event.ev_msg.ev_avrcp_generic_op.txn_id = transaction;

  event.ev_msg.ev_avrcp_generic_op.cmd = frame->cmd;
  event.ev_msg.ev_avrcp_generic_op.subunit_type = frame->subunitType;
  event.ev_msg.ev_avrcp_generic_op.subunit_id = frame->subunitID;
  event.ev_msg.ev_avrcp_generic_op.opcode = frame->opcode;
  dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB, frame->operandCount );
  if ( dsm_pushdown_tail( &dsm_ptr,
                          (void*)frame->operand,
                          frame->operandCount,
                          DSM_DS_SMALL_ITEM_POOL ) != frame->operandCount )
  {
    BT_ERR("BT PF AVRCP: Pushdown failed %x bytes wanted",
           frame->operandCount,0,0);
  }
  event.ev_msg.ev_avrcp_generic_op.operand = dsm_ptr;

  BT_MSG_DEBUG("BT PF AVRCP TX EV: genericop AppID: %x",
               bt_pf_avrcp_ext_app_id, 0, 0);

  bt_ec_send_event( &event );
}

#ifdef FEATURE_BT_AVRCP_13

/* ==========================================================================
                            AVRCP METADATA EVENTS
============================================================================*/

/*===========================================================================

FUNCTION
  bt_pf_avrcp_target_capability_command_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avrcp_target_capability_command_ind( OI_BD_ADDR *addr,
                                                      OI_UINT8 transaction,
                                                      OI_UINT8 capability)
{
  bt_ev_msg_type   event;
  uint16           status;

  /* Clearing the pending fragment, if any */
  bt_pf_avrcp_clear_pending_response( addr );

  if ( (capability != OI_AVRCP_CAPABILITY_COMPANY_ID) &&
       (capability != OI_AVRCP_CAPABILITY_EVENTS_SUPPORTED) ) 
  {
    status = OI_AVRCPTarget_SendSimpleMetaReply( addr,
                                                 transaction,
                                                 OI_AVRCP_GET_CAPABILITIES,
                                                 OI_AVRCP_RESPONSE_INVALID_PARAMETER);
    if (!OI_SUCCESS(status)) 
    {
      BT_ERR ("BT PF AVRCP METADATA: Error replying to invalid capability command_ind %x", 
                capability, 0, 0);
    }
  }
  else
  {
    event.ev_hdr.bt_app_id = bt_pf_avrcp_13_ext_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_AVRCP_CAPABILITY_COMMAND_IND;
    bt_pf_copy_bd_addr_from_oi( &event.ev_msg.ev_avrcp_capability_command.bd_addr,
                                addr);  
    event.ev_msg.ev_avrcp_capability_command.txn = transaction;
    event.ev_msg.ev_avrcp_capability_command.cap_id = capability;
    BT_MSG_DEBUG("BT PF AVRCP METADATA TX EV: capability AppID: %x",
               bt_pf_avrcp_13_ext_app_id,0,0);
    bt_ec_send_event( &event );
  }  
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_target_list_attributes_command_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avrcp_target_list_attributes_command_ind( OI_BD_ADDR *addr,
                                                           OI_UINT8 transaction)                                                       
{
  bt_ev_msg_type   event;

  /* Clearing the pending fragment, if any */
  bt_pf_avrcp_clear_pending_response( addr );

  event.ev_hdr.bt_app_id = bt_pf_avrcp_13_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVRCP_LIST_ATTRIBUTES_COMMAND_IND;
  bt_pf_copy_bd_addr_from_oi( &event.ev_msg.ev_avrcp_list_attributes_command.bd_addr,
                              addr);  
  event.ev_msg.ev_avrcp_list_attributes_command.txn = transaction;
  
  BT_MSG_DEBUG("BT PF AVRCP METADATA TX EV: list attrs AppID: %x",
               bt_pf_avrcp_13_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_target_list_attribute_values_command_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avrcp_target_list_attribute_values_command_ind( OI_BD_ADDR *addr,
                                                                 OI_UINT8 transaction,
                                                                 OI_UINT8 attr_id)
{
  bt_ev_msg_type   event;
  uint16 status;

  /* Clearing the pending fragment, if any */
  bt_pf_avrcp_clear_pending_response( addr );

  if ( (attr_id == OI_AVRCP_SETTING_ILLEGAL) ||
       (attr_id > OI_AVRCP_SETTING_SCAN_STATUS) ) 
  {
    status = OI_AVRCPTarget_SendSimpleMetaReply( addr,
                                                 transaction,
                                                 OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_VALUES,
                                                 OI_AVRCP_RESPONSE_INVALID_PARAMETER);
    if ( !OI_SUCCESS(status) ) 
    {
      BT_ERR ("BT PF AVRCP METADATA: Error replying to invalid list attr val command_ind %x", 
                attr_id, 0, 0);
    }
  } 
  else 
  {
    event.ev_hdr.bt_app_id = bt_pf_avrcp_13_ext_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_AVRCP_LIST_ATTRIBUTE_VALUES_COMMAND_IND;
    bt_pf_copy_bd_addr_from_oi( &event.ev_msg.ev_avrcp_list_attribute_values_command.bd_addr,
                                addr);  
    event.ev_msg.ev_avrcp_list_attribute_values_command.txn = transaction;
    event.ev_msg.ev_avrcp_list_attribute_values_command.attr_id = attr_id;
    BT_MSG_DEBUG("BT PF AVRCP METADATA TX EV: list attr values AppID: %x",
               bt_pf_avrcp_13_ext_app_id,0,0);

    bt_ec_send_event( &event );
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_target_get_attribute_values_command_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avrcp_target_get_attribute_values_command_ind( OI_BD_ADDR *addr,
                                                                OI_UINT8 transaction,
                                                                OI_UINT8 count,
                                                                OI_UINT8 *attr_ids)
{
  bt_ev_msg_type   event;
  uint16           status;
  uint32            i;
  boolean          is_valid_attr_id = FALSE;

  /* Clearing the pending fragment, if any */
  bt_pf_avrcp_clear_pending_response( addr );

  for( i = 0; i < count; i++ )
  {
    if ( (attr_ids[i] > OI_AVRCP_SETTING_ILLEGAL) &&
         (attr_ids[i] <= OI_AVRCP_SETTING_SCAN_STATUS) )
    {
      /* The values can be passed to the OEM even if one attr is valid.
         Invalid values are ignored */
      is_valid_attr_id = TRUE;
      break;
    }
  }

  if( is_valid_attr_id != TRUE )
  {
    status = OI_AVRCPTarget_SendSimpleMetaReply( addr,
                                                 transaction,
                                                 OI_AVRCP_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE,
                                                 OI_AVRCP_RESPONSE_INVALID_PARAMETER);
    if (!OI_SUCCESS(status)) 
    {
      BT_ERR ("BT PF AVRCP METADATA: Error replying to invalid get attr val command_ind", 
               0, 0, 0);
    }
  } 
  else
  {
    event.ev_hdr.bt_app_id = bt_pf_avrcp_13_ext_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_AVRCP_GET_ATTRIBUTE_VALUES_COMMAND_IND;
    bt_pf_copy_bd_addr_from_oi( &event.ev_msg.ev_avrcp_get_attribute_values_command.bd_addr,
                                addr);  
    event.ev_msg.ev_avrcp_get_attribute_values_command.txn = transaction;
    event.ev_msg.ev_avrcp_get_attribute_values_command.attr_count = count;
    event.ev_msg.ev_avrcp_get_attribute_values_command.attr_ids = attr_ids;
    BT_MSG_DEBUG("BT PF AVRCP METADATA TX EV: get attr values AppID: %x",
               bt_pf_avrcp_13_ext_app_id,0,0);

    bt_ec_send_event( &event );
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_target_get_attribute_values_command_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/

LOCAL void bt_pf_avrcp_target_set_attribute_values_command_ind( OI_BD_ADDR *addr,
                                                                OI_UINT8 transaction,
                                                                OI_UINT8 count,
                                                                OI_AVRCP_ATTRIBUTE_PAIR attrs)
{
  bt_ev_msg_type   event;
  uint16           status;
  uint32            i;
  boolean          is_attr_id_valid = FALSE;
  boolean          is_attr_val_valid = FALSE;

  /* Clearing the pending fragment, if any */
  bt_pf_avrcp_clear_pending_response( addr );

  for( i = 0; i < (count * 2); i = i + 2 )
  {
    if ( ( attrs[i] > OI_AVRCP_SETTING_ILLEGAL ) &&
       ( attrs[i] <= OI_AVRCP_SETTING_SCAN_STATUS ) )
    {
      is_attr_id_valid = TRUE;
       /* The values can be passed to the OEM even if one attr is valid.
         Invalid values are ignored */
      bt_pf_avrcp_target_validate_attr_vals( attrs[i], attrs[i + 1], &is_attr_val_valid );
      if( is_attr_val_valid == TRUE )
      {
        /* The values can be passed to the OEM even if one attr val is valid.
         Invalid values are ignored */
        break;
      }
    }
  }

  if( (is_attr_id_valid != TRUE) || (is_attr_val_valid != TRUE) )
  {
    status = OI_AVRCPTarget_SendSimpleMetaReply( addr,
                                                 transaction,
                                                 OI_AVRCP_SET_PLAYER_APPLICATION_SETTING_VALUE,
                                                 OI_AVRCP_RESPONSE_INVALID_PARAMETER);
    if (!OI_SUCCESS(status)) 
    {
      BT_ERR ("BT PF AVRCP METADATA: Error replying to invalid set attr val command_ind", 
               0, 0, 0);
    }
  } 
  else
  {
    event.ev_hdr.bt_app_id = bt_pf_avrcp_13_ext_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_AVRCP_SET_ATTRIBUTE_VALUES_COMMAND_IND;
    bt_pf_copy_bd_addr_from_oi( &event.ev_msg.ev_avrcp_set_attribute_values_command.bd_addr,
                                addr);  
    event.ev_msg.ev_avrcp_set_attribute_values_command.txn = transaction;
    event.ev_msg.ev_avrcp_set_attribute_values_command.attr_id_count = count;
    event.ev_msg.ev_avrcp_set_attribute_values_command.attrs = attrs;
    BT_MSG_DEBUG("BT PF AVRCP METADATA TX EV: set attr val AppID: %x",
               bt_pf_avrcp_13_ext_app_id,0,0);

    bt_ec_send_event( &event );
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_target_get_attribute_setting_text_command_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avrcp_target_get_attribute_setting_text_command_ind( OI_BD_ADDR *addr,
                                                                      OI_UINT8 transaction,
                                                                      OI_UINT8 count,
                                                                      OI_UINT8 *attr_ids)
{
  bt_ev_msg_type   event;
  uint16           status;
  uint32            i;
  boolean          is_attr_id_valid = FALSE;

  /* Clearing the pending fragment, if any */
  bt_pf_avrcp_clear_pending_response( addr );

  for( i = 0; i < count; i++ )
  {
    if ( (attr_ids[i] > OI_AVRCP_SETTING_ILLEGAL) &&
       (attr_ids[i] <= OI_AVRCP_SETTING_SCAN_STATUS) )
    {
       /* The values can be passed to the OEM even if one attr is valid.
         Invalid attrs are ignored */
      is_attr_id_valid = TRUE;
      break;
    }
  }

  if( is_attr_id_valid != TRUE )
  {
    status = OI_AVRCPTarget_SendSimpleMetaReply( addr,
                                                 transaction,
                                                 OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT,
                                                 OI_AVRCP_RESPONSE_INVALID_PARAMETER);
    if (!OI_SUCCESS(status)) 
    {
      BT_ERR ("BT PF AVRCP METADATA: Error replying to invalid get attr text command_ind", 
               0, 0, 0);
    }
  } 
  else
  {
    event.ev_hdr.bt_app_id = bt_pf_avrcp_13_ext_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_AVRCP_GET_ATTRIBUTE_SETTING_TEXT_COMMAND_IND;
    bt_pf_copy_bd_addr_from_oi( &event.ev_msg.ev_get_attribute_setting_text_command.bd_addr,
                                addr);  
    event.ev_msg.ev_get_attribute_setting_text_command.txn = transaction;
    event.ev_msg.ev_get_attribute_setting_text_command.attr_count = count;
    event.ev_msg.ev_get_attribute_setting_text_command.attr_ids = attr_ids;
    BT_MSG_DEBUG("BT PF AVRCP METADATA TX EV: get attr text AppID: %x",
               bt_pf_avrcp_13_ext_app_id,0,0);

    bt_ec_send_event( &event );
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_target_get_attribute_setting_value_text_command_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avrcp_target_get_attribute_setting_value_text_command_ind( OI_BD_ADDR *addr,
                                                                            OI_UINT8 transaction,
                                                                            OI_UINT8 attr_id,
                                                                            OI_UINT8 count,
                                                                            OI_UINT8 *value_ids)
{
  bt_ev_msg_type   event;
  uint16           status;
  uint16           error_code = OI_OK;
  uint32            i;
  boolean          is_attr_val_valid = FALSE;

  /* Clearing the pending fragment, if any */
  bt_pf_avrcp_clear_pending_response( addr );

  if ( (attr_id == OI_AVRCP_SETTING_ILLEGAL) ||
       (attr_id > OI_AVRCP_SETTING_SCAN_STATUS) )
  {
    error_code = OI_AVRCP_RESPONSE_INVALID_PARAMETER;
  }
  else
  {
    for( i = 0; i < count; i++ )
    {
      bt_pf_avrcp_target_validate_attr_vals( attr_id, value_ids[i], &is_attr_val_valid );
      if( is_attr_val_valid == TRUE )
      {
        /* The values can be passed to the OEM even if one attr val is valid.
        Invalid values are ignored */
        break;
      }
      
    }
  }

  if( ( is_attr_val_valid != TRUE ) || (error_code == OI_AVRCP_RESPONSE_INVALID_PARAMETER))
  {
    status = OI_AVRCPTarget_SendSimpleMetaReply( addr,
                                                 transaction,
                                                 OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT,
                                                 OI_AVRCP_RESPONSE_INVALID_PARAMETER);
    if (!OI_SUCCESS(status)) 
    {
      BT_ERR ("BT PF AVRCP METADATA: Error replying to invalid get attr set val text command_ind", 
               0, 0, 0);
    }
  } 
  else
  {
    event.ev_hdr.bt_app_id = bt_pf_avrcp_13_ext_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_AVRCP_GET_ATTRIBUTE_SETTING_VALUE_TEXT_COMMAND_IND;
    bt_pf_copy_bd_addr_from_oi( &event.ev_msg.ev_avrcp_get_attribute_setting_value_text_command.bd_addr,
                                addr);  
    event.ev_msg.ev_get_attribute_setting_text_command.txn = transaction;
    event.ev_msg.ev_avrcp_get_attribute_setting_value_text_command.attr_id = attr_id;
    event.ev_msg.ev_avrcp_get_attribute_setting_value_text_command.attr_val_count = count;
    event.ev_msg.ev_avrcp_get_attribute_setting_value_text_command.attr_vals = value_ids;
    BT_MSG_DEBUG("BT PF AVRCP METADATA TX EV: set attr val text AppID: %x",
               bt_pf_avrcp_13_ext_app_id,0,0);

    bt_ec_send_event( &event );
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_target_inform_character_set_command_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avrcp_target_inform_character_set_command_ind( OI_BD_ADDR *addr,
                                                                OI_UINT8 transaction,
                                                                OI_UINT8 count,
                                                                OI_UINT16 *char_set_ids)
{
  bt_ev_msg_type   event;
  uint16           status;
  OI_STATUS        error_code = OI_OK;
  uint32            i;

  /* Clearing the pending fragment, if any */
  bt_pf_avrcp_clear_pending_response( addr );

  for( i = 0; i < count; i++ )
  {
    if ( ( char_set_ids[i] != OI_LANG_ENCODING_UTF8 ) &&
         ( char_set_ids[i] != OI_LANG_ENCODING_UTF16 ))
    {
      error_code = OI_AVRCP_NOT_IMPLEMENTED;
    }
    else
    {
      /* The values can be passed to the OEM even if one charset is valid.
         Invalid values are ignored */
      error_code = OI_OK;
      break;
    }
  }

  if( (!count) || (error_code == OI_AVRCP_NOT_IMPLEMENTED) )
  {
    if (!count) 
    {
      error_code = OI_AVRCP_RESPONSE_INVALID_PARAMETER;
    }
    status = OI_AVRCPTarget_SendSimpleMetaReply( addr,
                                                 transaction,
                                                 OI_AVRCP_INFORM_DISPLAYABLE_CHARACTER_SET,
                                                 error_code);
    if (!OI_SUCCESS(status)) 
    {
      BT_ERR ("BT PF AVRCP METADATA: Error replying to invalid inform charset command_ind", 
               0, 0, 0);
    }
  } 
  else
  {
    event.ev_hdr.bt_app_id = bt_pf_avrcp_13_ext_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_AVRCP_INFORM_CHARACTER_SET_COMMAND_IND;
    bt_pf_copy_bd_addr_from_oi( &event.ev_msg.ev_avrcp_inform_character_set_command.bd_addr,
                                addr );  
    event.ev_msg.ev_avrcp_inform_character_set_command.txn = transaction;
    event.ev_msg.ev_avrcp_inform_character_set_command.charset_count = count;
    event.ev_msg.ev_avrcp_inform_character_set_command.charset_ids = char_set_ids;
    BT_MSG_DEBUG("BT PF AVRCP METADATA TX EV: inform charset AppID: %x",
               bt_pf_avrcp_13_ext_app_id,0,0);

    bt_ec_send_event( &event );
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_target_inform_battery_status_of_ct_command_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avrcp_target_inform_battery_status_of_ct_command_ind( OI_BD_ADDR *addr,
                                                                       OI_UINT8 transaction,
                                                                       OI_UINT8 battery_status)
{
  bt_ev_msg_type   event;
  uint16 status;

  /* Clearing the pending fragment, if any */
  bt_pf_avrcp_clear_pending_response( addr );

  if ( battery_status > OI_AVRCP_BATTERY_STATUS_FULL_CHARGE ) 
  {
    status = OI_AVRCPTarget_SendSimpleMetaReply( addr,
                                                 transaction,
                                                 OI_AVRCP_INFORM_BATTERY_STATUS_OF_CT,
                                                 OI_AVRCP_RESPONSE_INVALID_PARAMETER);
    if (!OI_SUCCESS(status)) 
    {
      BT_ERR ("BT PF AVRCP METADATA: Error replying to invalid CT battery status command_ind %x", 
                battery_status, 0, 0);
    }
  } 
  else 
  {
    event.ev_hdr.bt_app_id = bt_pf_avrcp_13_ext_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_AVRCP_INFORM_BATTERY_STATUS_OF_CT_COMMAND_IND;
    bt_pf_copy_bd_addr_from_oi( &event.ev_msg.ev_avrcp_inform_battery_status_of_ct_command.bd_addr,
                                  addr );  
    event.ev_msg.ev_avrcp_inform_battery_status_of_ct_command.txn = transaction;
    event.ev_msg.ev_avrcp_inform_battery_status_of_ct_command.batt_status = battery_status;
    BT_MSG_DEBUG("BT PF AVRCP METADATA TX EV: CT battery status AppID: %x",
               bt_pf_avrcp_13_ext_app_id,0,0);

    bt_ec_send_event( &event );
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_target_get_element_attribute_command_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avrcp_target_get_element_attribute_command_ind( OI_BD_ADDR *addr,
                                                                 OI_UINT8 transaction,
                                                                 OI_UINT8 *elem_id,
                                                                 OI_UINT8  count,
                                                                 OI_UINT32 *attributes)
{ 
  bt_ev_msg_type   event;
  uint16           status;
  OI_STATUS        error_code = OI_OK;
  uint32            i;
  uint32            j;
  boolean          is_valid_media_attr = FALSE;

  /* Clearing the pending fragment, if any */
  bt_pf_avrcp_clear_pending_response( addr );

  for ( i = 0; i < BT_PF_AVRCP_SIZE_OF_CURRENT_TRACK_ID; i++ ) 
  {
    if( elem_id[i] != 0 )
    {
      error_code = OI_AVRCP_RESPONSE_INVALID_PARAMETER;
      break;
    }
  }

  if( !count ) 
  {
    count = BT_PF_MAX_NUM_MEDIA_ATTRIBUTES;
    attributes = bt_pf_avrcp_media_attributes;
  }

  for( j = 0; j < count; j++ )
  {
    if ( (attributes[j] > OI_AVRCP_MEDIA_ATTR_ILLEGAL) &&
       (attributes[j] <= OI_AVRCP_MEDIA_ATTR_PLAYING_TIME) )
    {
      /* The values can be passed to the OEM even if one attr is valid.
         Invalid values are ignored */
      is_valid_media_attr = TRUE;
      break;
    }
  }

  if( ( error_code == OI_AVRCP_RESPONSE_INVALID_PARAMETER ) ||
      ( is_valid_media_attr != TRUE ) )
  {
    status = OI_AVRCPTarget_SendSimpleMetaReply( addr,
                                                 transaction,
                                                 OI_AVRCP_GET_ELEMENT_ATTRIBUTES,
                                                 error_code);
    if (!OI_SUCCESS(status)) 
    {
      BT_ERR ("BT PF AVRCP METADATA: Error replying to invalid element attr command_ind %x", 
                0, 0, 0);
    }
  }
  else
  {
    event.ev_hdr.bt_app_id = bt_pf_avrcp_13_ext_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_AVRCP_GET_ELEMENT_ATTRIBUTE_COMMAND_IND;
    bt_pf_copy_bd_addr_from_oi( &event.ev_msg.ev_avrcp_get_element_attribute_command.bd_addr,
                              addr);  
    event.ev_msg.ev_avrcp_get_element_attribute_command.txn = transaction;
    event.ev_msg.ev_avrcp_get_element_attribute_command.media_element_id = elem_id;
    event.ev_msg.ev_avrcp_get_element_attribute_command.elements_count = count;
    event.ev_msg.ev_avrcp_get_element_attribute_command.media_attrbutes = attributes;
    BT_MSG_DEBUG("BT PF AVRCP METADATA TX EV: element attr AppID: %x",
               bt_pf_avrcp_13_ext_app_id,0,0);

    bt_ec_send_event( &event );
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_target_get_play_status_command_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avrcp_target_get_play_status_command_ind( OI_BD_ADDR *addr,
                                                           OI_UINT8 transaction)
{
  bt_ev_msg_type   event;

  /* Clearing the pending fragment, if any */
  bt_pf_avrcp_clear_pending_response( addr );

  event.ev_hdr.bt_app_id = bt_pf_avrcp_13_ext_app_id;
  event.ev_hdr.ev_type = BT_EV_PF_AVRCP_GET_PLAY_STATUS_COMMAND_IND;
  bt_pf_copy_bd_addr_from_oi( &event.ev_msg.ev_avrcp_get_play_status_command.bd_addr,
                              addr);  
  event.ev_msg.ev_avrcp_get_play_status_command.txn = transaction;
  
  BT_MSG_DEBUG("BT PF AVRCP METADATA TX EV: get play status AppID: %x",
               bt_pf_avrcp_13_ext_app_id,0,0);

  bt_ec_send_event( &event );
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_target_event_notification_command_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avrcp_target_event_notification_command_ind( OI_BD_ADDR *addr,
                                                              OI_UINT8 transaction,
                                                              OI_UINT8 event_id,
                                                              OI_UINT32 playback_interval)
{
  bt_ev_msg_type   event;
  uint16 status;
  OI_AVRCP_EVENT   *ev_metadata;

  /* Clearing the pending fragment, if any */
  bt_pf_avrcp_clear_pending_response( addr );
  if ( (event_id < OI_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED) ||
       (event_id > OI_AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED) ) 
  {
    status = OI_AVRCPTarget_SendSimpleMetaReply( addr,
                                                 transaction,
                                                 OI_AVRCP_REGISTER_NOTIFICATION,
                                                 OI_AVRCP_RESPONSE_INVALID_PARAMETER);
    if (!OI_SUCCESS(status)) 
    {
      BT_ERR ("BT PF AVRCP METADATA: Error replying to invalid event notify command_ind %x", 
                event_id, 0, 0);
    }
  } 
  else 
  {
    event.ev_hdr.bt_app_id = bt_pf_avrcp_13_ext_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_AVRCP_EVENT_NOTIFICATION_COMMAND_IND;
    bt_pf_copy_bd_addr_from_oi( &event.ev_msg.ev_avrcp_event_notification_command.bd_addr,
                               addr);  
    event.ev_msg.ev_avrcp_event_notification_command.txn = transaction;
    event.ev_msg.ev_avrcp_event_notification_command.notif_event_id = event_id;
    if ( event_id == OI_AVRCP_EVENT_PLAYBACK_POS_CHANGED ) 
    {
      /* Converting the time in secs to milli secs */
      event.ev_msg.ev_avrcp_event_notification_command.playback_interval =
      (playback_interval * 1000);
    }
    else
    {
      /* Now this field should be ignored */
      event.ev_msg.ev_avrcp_event_notification_command.playback_interval = 0;
    }

    if( event_id == OI_AVRCP_EVENT_SYSTEM_STATUS_CHANGED ) 
    {
      if( !bt_pf_avrcp_13_ext_app_id )
      {
        ev_metadata = &bt_pf_avrcp_notify_ev_metadata;
        ev_metadata->id = event_id;
        ev_metadata->u.systemStatus = BT_PF_AVRCP_SYSTEM_STATUS_UNPLUGGED;

        status = OI_AVRCPTarget_RegisterNotificationReply( addr,
                                                           transaction,
                                                           ev_metadata,
                                                           TRUE );
        if (OI_SUCCESS(status)) 
        {
          BT_MSG_DEBUG("BT PF AVRCP: status %x", 
                    status,0,0);
        }
      }
    }  
    BT_MSG_DEBUG("BT PF AVRCP METADATA TX EV: event notify AppID: %x",
               bt_pf_avrcp_13_ext_app_id,0,0);

    bt_ec_send_event( &event );
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_target_continue_response_command_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avrcp_target_continue_response_command_ind( OI_BD_ADDR *addr,
                                                             OI_UINT8 transaction,
                                                             OI_UINT8 pdu_id,
                                                             OI_UINT16 curr_attr_index,
                                                             OI_UINT16 curr_data_pos)
{ 
  uint16                  status;
  OI_STATUS               error_code = OI_OK;
  OI_AVRCP_FRAGMENT_DATA  fragment_msg;

   /* The success case response needs to be routed from BT PF */
  if ( (pdu_id > OI_AVRCP_ABORT_CONTINUING_RESPONSE ) ||
       (pdu_id < OI_AVRCP_GET_CAPABILITIES ) )
  {
    error_code = OI_AVRCP_RESPONSE_INVALID_PDU; 
  } 
  else
  {
    if ( !bt_pf_avrcp_get_pending_response( addr, 
                                            pdu_id, 
                                            curr_attr_index, 
                                            curr_data_pos,
                                            &fragment_msg  ) )
    {
      error_code = OI_AVRCP_RESPONSE_INTERNAL_ERROR;
    }
    else
    {
      status = OI_AVRCPTarget_FragmentReply (addr, transaction, pdu_id, &fragment_msg);

      if( status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW) 
      {
        BT_MSG_DEBUG("BT PF AVRCP METADATA: fragment reply partially sent",
                    0,0,0);
      } 
      else if (OI_SUCCESS(status)) 
      {
        BT_MSG_DEBUG("BT PF AVRCP METADATA: Sent last fragment",
                      0,0,0);
        bt_pf_avrcp_clear_pending_response(addr);
      } 
      else 
      {
        BT_ERR ("BT PF AVRCP METADATA: Failure sending fragment reply %x", 
                pdu_id, 0, 0);
      }
    }
  } 

  if( error_code != OI_OK )
  {
    status = OI_AVRCPTarget_SendSimpleMetaReply( addr,
                                                 transaction,
                                                 OI_AVRCP_REQUEST_CONTINUING_RESPONSE,
                                                 error_code);
    if (!OI_SUCCESS(status)) 
    {
      BT_ERR ("BT PF AVRCP METADATA: Error replying to invalid continue response error %x", 
               error_code, 0, 0);
    }
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_target_abort_response_command_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avrcp_target_abort_response_command_ind( OI_BD_ADDR *addr,
                                                          OI_UINT8 transaction,
                                                          OI_UINT8 pdu_id)
{
  uint16                   status;
  OI_STATUS                error_code = OI_OK;
  OI_AVRCP_FRAGMENT_DATA   fragment_msg;

  /* The success case response needs to be routed from BT PF */
   if ( (pdu_id > OI_AVRCP_ABORT_CONTINUING_RESPONSE ) ||
       (pdu_id < OI_AVRCP_GET_CAPABILITIES ) )
  {
    error_code = OI_AVRCP_RESPONSE_INVALID_PDU;
  }
  else
  {
    if ( !bt_pf_avrcp_get_pending_response( addr, 
                                            pdu_id, 
                                            0, 
                                            0,
                                            &fragment_msg ) )
    {
      error_code = OI_AVRCP_RESPONSE_INTERNAL_ERROR;
    }
    else
    {
      bt_pf_avrcp_clear_pending_response(addr);
    }
  }

  if( error_code != OI_OK )
  {
    status = OI_AVRCPTarget_SendSimpleMetaReply( addr,
                                                 transaction,
                                                 OI_AVRCP_ABORT_CONTINUING_RESPONSE,
                                                 error_code);
    if (!OI_SUCCESS(status)) 
    {
      BT_ERR ("BT PF AVRCP METADATA: Error replying to invalid abort response error %x", 
               error_code, 0, 0);
    }
  }

}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_target_group_navigation_command_ind

DESCRIPTION
  Event callback generated by OI code. Translates OI callback into AMSS
  event.

===========================================================================*/
LOCAL void bt_pf_avrcp_target_group_navigation_command_ind( OI_BD_ADDR *addr,
                                                            OI_UINT8 transaction,
                                                            OI_UINT16 vendor_unique_id,
                                                            OI_BOOL button_press)
{
  bt_ev_msg_type   event;
  uint16 status;

  /* Clearing the pending fragment, if any */
  bt_pf_avrcp_clear_pending_response( addr );
  if ( (vendor_unique_id != OI_AVRCP_NEXT_GROUP) &&
       (vendor_unique_id != OI_AVRCP_PREV_GROUP) ) 
  {
    status = OI_AVRCPTarget_GroupNavigationReply(addr,
                                                 transaction,
                                                 vendor_unique_id,
                                                 button_press,
                                                 OI_AVRCP_RESPONSE_REJECTED);
    if (!OI_SUCCESS(status)) 
    {
      BT_ERR ("BT PF AVRCP METADATA: Error replying to invalid group navigation command_ind %x", 
                vendor_unique_id, 0, 0);
    }
  } 
  else 
  {
    event.ev_hdr.bt_app_id = bt_pf_avrcp_13_ext_app_id;
    event.ev_hdr.ev_type = BT_EV_PF_AVRCP_GROUP_NAVIGATION_COMMAND_IND;
    bt_pf_copy_bd_addr_from_oi( &event.ev_msg.ev_avrcp_group_navigation_command.bd_addr,
                                  addr);  
    event.ev_msg.ev_avrcp_group_navigation_command.txn = transaction;
    event.ev_msg.ev_avrcp_group_navigation_command.vendor_unique = vendor_unique_id;
    event.ev_msg.ev_avrcp_group_navigation_command.button_press = button_press;
    BT_MSG_DEBUG("BT PF AVRCP METADATA TX EV: group navigation AppID: %x, butt press = %x",
               bt_pf_avrcp_13_ext_app_id, button_press,0);

    bt_ec_send_event( &event );
  }
}
#endif /* FEATURE_BT_AVRCP_13 */

/*===========================================================================

FUNCTION
  bt_pf_avrcp_process_event

DESCRIPTION
  Handles all BT driver events coming to the AVRCP layer.

===========================================================================*/
LOCAL void bt_pf_avrcp_process_event( bt_ev_msg_type*  ev_msg_ptr )
{
  switch ( ev_msg_ptr->ev_hdr.ev_type )
  {
    case BT_EV_GN_CMD_DONE:
      {
        if ( ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status !=
             BT_CS_GN_SUCCESS )
        {
          BT_ERR( "BT AVRCP: Bad CMD Done Stat %x Typ %x AID %x",
                  ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_status,
                  ev_msg_ptr->ev_msg.ev_gn_cmd_done.cmd_type,
                  ev_msg_ptr->ev_hdr.bt_app_id );
        }
      }
      break;
    default:
      BT_ERR( "BT AVRCP: Unexpected event Typ %x",
              ev_msg_ptr->ev_hdr.ev_type,0,0 );
  }
}

/*===========================================================================

                     CMD PROCESSING FUNCTION DEFINITIONS

===========================================================================*/
/*===========================================================================

FUNCTION
  bt_pf_avrcp_cmd_enable

DESCRIPTION

===========================================================================*/

void bt_pf_avrcp_cmd_enable ( bt_pf_cmd_msg_type *cmd_ptr )
{
#ifdef FEATURE_BT_AVRCP_13
  OI_BD_ADDR zero_addr = { {0,0,0,0,0,0} }; 
#endif

  static const OI_AVRCP_CALLBACKS bt_pf_avrcp_callbacks = {
    bt_pf_avrcp_connect_ind,
    bt_pf_avrcp_disconnect_ind,
    bt_pf_avrcp_connect_cfm
  };

#ifdef FEATURE_BT_AVRCP_13

  static OI_AVRCP_TARGET_METADATA_CALLBACKS bt_pf_avrcp_target_metadata_callbacks = {
	bt_pf_avrcp_target_capability_command_ind,
    bt_pf_avrcp_target_list_attributes_command_ind,
    bt_pf_avrcp_target_list_attribute_values_command_ind,
    bt_pf_avrcp_target_get_attribute_values_command_ind,
    bt_pf_avrcp_target_set_attribute_values_command_ind,
    bt_pf_avrcp_target_get_attribute_setting_text_command_ind,
    bt_pf_avrcp_target_get_attribute_setting_value_text_command_ind,
    bt_pf_avrcp_target_inform_character_set_command_ind,
    bt_pf_avrcp_target_inform_battery_status_of_ct_command_ind,
    bt_pf_avrcp_target_get_element_attribute_command_ind,
    bt_pf_avrcp_target_get_play_status_command_ind,
    bt_pf_avrcp_target_event_notification_command_ind,
    bt_pf_avrcp_target_continue_response_command_ind,
    bt_pf_avrcp_target_abort_response_command_ind,
    bt_pf_avrcp_target_group_navigation_command_ind
  };
#endif /* FEATURE_BT_AVRCP_13 */

  static const OI_AVRCP_TARGET_CALLBACKS bt_pf_avrcp_target_callbacks = {
    bt_pf_avrcp_target_command_ind,
    bt_pf_avrcp_target_generic_ind,
#ifdef FEATURE_BT_AVRCP_13
   &bt_pf_avrcp_target_metadata_callbacks
#endif /* FEATURE_BT_AVRCP_13 */
  };

  static const OI_SDP_STRINGS bt_pf_avrcp_empty_string = {
    NULL,
    0
  };

  bt_cmd_pf_avrcp_enable_type *cmd = &cmd_ptr->cmd_msg.cmd_avrcp_enable;

  BT_MSG_DEBUG("BT PF AVRCP: new supp cat %x", 
                     cmd->supported_categories,0,0); 

  bt_pf_avrcp_ext_app_id = cmd_ptr->cmd_hdr.app_id;

  cmd_ptr->cmd_hdr.cmd_status =
    OI_AVRCP_Register( (OI_AVRCP_CALLBACKS*)&bt_pf_avrcp_callbacks );
  if( cmd_ptr->cmd_hdr.cmd_status == BT_CS_GN_SUCCESS )
  {
    cmd_ptr->cmd_hdr.cmd_status =
      OI_AVRCP_RegisterTarget( (OI_AVRCP_TARGET_CALLBACKS*)&bt_pf_avrcp_target_callbacks,
                               cmd->supported_categories,
                               NULL, /* Put pointer to 3-byte company ID here */
                               &bt_pf_avrcp_empty_string );

    BT_MSG_DEBUG("BT PF AVRCP: supported categories old cat %x, new cat %x", 
                     bt_pf_avrcp_supported_categories, 
                     cmd->supported_categories, 0 );
    bt_pf_avrcp_supported_categories = cmd->supported_categories;

    bt_pf_avrcp_int_app_id = 
      bt_cmd_ec_get_app_id_and_register( bt_pf_avrcp_process_event );

    bt_cmd_sd_register_serv_ext( bt_pf_avrcp_int_app_id,
                                 BT_SD_SERVICE_CLASS_AV_REMOTE_CONTROL_TARGET,
                                 BT_PF_AVRCP_VERSION_NUMBER,
                                 0,
                                 BT_PF_AVCTP_PSM,
                                 FALSE,  /*  Do not page scan for this.  */
#ifdef FEATURE_BT_AVRCP_13
                                 "AVRCP Target1.3"
#else
                                 "AVRCP Target1.0"
#endif                          
                               );
                            
  }

#ifdef FEATURE_BT_AVRCP_13
  
  bt_pf_avrcp_notify_ev_metadata.id = 0;
  bt_pf_avrcp_notify_ev_metadata.u.playStatus = 0;
  OI_MemSet( bt_pf_avrcp_notify_ev_metadata.u.trackIndex, 0, 
            BT_PF_AVRCP_SIZE_OF_CURRENT_TRACK_ID ) ;
  bt_pf_avrcp_notify_ev_metadata.u.playbackPosition = 0;
  bt_pf_avrcp_notify_ev_metadata.u.batteryStatus = 0;
  bt_pf_avrcp_notify_ev_metadata.u.systemStatus = 0;
  bt_pf_avrcp_notify_ev_metadata.u.deviceSetting.attributes = NULL;
  bt_pf_avrcp_notify_ev_metadata.u.deviceSetting.numAttr = 0; 

  bt_pf_avrcp_fragment_info.addr = zero_addr;
  bt_pf_avrcp_fragment_info.count = 0;
  bt_pf_avrcp_fragment_info.is_pending = FALSE;
  bt_pf_avrcp_fragment_info.pdu_id = 0;
  bt_pf_avrcp_fragment_info.base.data = NULL;
  bt_pf_avrcp_fragment_info.base.attr = NULL;
  bt_pf_avrcp_num_frag_pending = 0;

#endif /* FEATURE_BT_AVRCP_13 */

  cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_cmd_connect

DESCRIPTION

===========================================================================*/
void bt_pf_avrcp_cmd_connect ( bt_pf_cmd_msg_type *cmd_ptr )
{
  bt_cmd_pf_avrcp_connect_type *cmd = &cmd_ptr->cmd_msg.cmd_avrcp_connect;
  static const OI_L2CAP_CONNECT_PARAMS bt_pf_avrcp_l2_params = {
    BT_L2_DEFAULT_MTU,
    BT_L2_DEFAULT_FLUSH_TIMEOUT,
    30000 /* Link timeout */
  };
  OI_BD_ADDR oi_bd_addr;

  bt_pf_copy_bd_addr_to_oi( &oi_bd_addr, &cmd->bd_addr );
  bt_pf_avrcp_bd_addr = cmd->bd_addr;

  cmd_ptr->cmd_hdr.cmd_status = 
    OI_AVRCP_Connect( &oi_bd_addr,
                      (OI_L2CAP_CONNECT_PARAMS*)&bt_pf_avrcp_l2_params );
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_cmd_disconnect

DESCRIPTION

===========================================================================*/
void bt_pf_avrcp_cmd_disconnect ( bt_pf_cmd_msg_type *cmd_ptr )
{
  OI_BD_ADDR oi_bd_addr;

  bt_pf_copy_bd_addr_to_oi( &oi_bd_addr, &bt_pf_avrcp_bd_addr );
  cmd_ptr->cmd_hdr.cmd_status = OI_AVRCP_Disconnect( &oi_bd_addr );

  if( cmd_ptr->cmd_hdr.cmd_status == BT_CS_PF_NOT_FOUND )
  {
    /* We're not connected to this device. Just send a disconnected event */
    bt_pf_avrcp_disconnect_ind( &oi_bd_addr, BT_CS_PF_NOT_CONNECTED );
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_cmd_disable

DESCRIPTION

===========================================================================*/
void bt_pf_avrcp_cmd_disable ( bt_pf_cmd_msg_type *cmd_ptr )
{
#ifdef FEATURE_BT_AVRCP_13
  bt_pf_avrcp_free_memory();
#endif /* FEATURE_BT_AVRCP_13*/

  cmd_ptr->cmd_hdr.cmd_status = OI_AVRCP_DeregisterTarget();
  if( cmd_ptr->cmd_hdr.cmd_status == BT_CS_GN_SUCCESS )
  {
    cmd_ptr->cmd_hdr.cmd_status = OI_AVRCP_Deregister();

    if( cmd_ptr->cmd_hdr.cmd_status == BT_CS_GN_SUCCESS ) 
    { 
      bt_cmd_sd_unregister_srv_by_psm( 
        bt_pf_avrcp_int_app_id, 
        BT_SD_SERVICE_CLASS_AV_REMOTE_CONTROL_TARGET, 
        BT_PF_AVCTP_PSM ); 
    }

    bt_cmd_ec_free_application_id( bt_pf_avrcp_int_app_id );
    bt_pf_avrcp_int_app_id = BT_APP_ID_NULL;
  }
  else
  {
    BT_ERR("BT PF AVRCP: Can't deregister target!",0,0,0);
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_cmd_send_reply

DESCRIPTION

===========================================================================*/
void bt_pf_avrcp_cmd_send_reply ( bt_pf_cmd_msg_type *cmd_ptr )
{
  OI_BD_ADDR oi_bd_addr;
  uint32     i;
  uint8      reply_op;
  bt_cmd_pf_avrcp_send_reply_type *cmd = &cmd_ptr->cmd_msg.cmd_avrcp_send_reply;

  bt_pf_copy_bd_addr_to_oi( &oi_bd_addr, &bt_pf_avrcp_bd_addr );

  for( i = 0;
       i < sizeof(bt_pf_avrcp_event_map) / sizeof(bt_pf_avrcp_event_map_type);
       i++ )
  {
    if( bt_pf_avrcp_event_map[i].driver_value == cmd->opcode )
    {
      break;
    }
  }

  if( i == sizeof(bt_pf_avrcp_event_map) / sizeof(bt_pf_avrcp_event_map_type) )
  {
    BT_ERR("BT PF AVRCP: Couldn't find opcode in table %x", cmd->opcode,0,0);
    i = 0;
  }

  reply_op = bt_pf_avrcp_event_map[i].spec_value;
  if( cmd->status == 0 )
  {
    reply_op |= OI_AVRCP_OP_PRESS;
  }
  else
  {
    reply_op |= OI_AVRCP_OP_RELEASE;
  }

  BT_MSG_DEBUG("BT PF AVRCP RX CMD: send reply. cooked cmd %x AppID: %x raw op %x",
               cmd->opcode,
               bt_pf_avrcp_ext_app_id, 
               reply_op);

  BT_MSG_DEBUG("BT PF AVRCP: status %x", cmd->status,0,0);

  cmd_ptr->cmd_hdr.cmd_status = 
    OI_AVRCPTarget_SendReply( &oi_bd_addr,
                              cmd->txn,
                              reply_op,
                              cmd->response );
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_cmd_send_generic_reply

DESCRIPTION

===========================================================================*/
void bt_pf_avrcp_cmd_send_generic_reply ( bt_pf_cmd_msg_type *cmd_ptr )
{
  OI_BD_ADDR     oi_bd_addr;
  OI_AVRCP_FRAME oi_frame;
  dsm_item_type *dsm_ptr;

  bt_cmd_pf_avrcp_send_generic_reply_type *cmd = 
    &cmd_ptr->cmd_msg.cmd_avrcp_send_generic_reply;

  dsm_ptr = cmd->frame.operand;

  bt_pf_copy_bd_addr_to_oi( &oi_bd_addr, &bt_pf_avrcp_bd_addr );

  oi_frame.cmd = cmd->frame.cmd;
  oi_frame.subunitType = cmd->frame.subunit_type;
  oi_frame.subunitID = cmd->frame.subunit_id;
  oi_frame.opcode = cmd->frame.opcode;
  oi_frame.operandCount = dsm_length_packet( cmd->frame.operand );
  oi_frame.operand = OI_Malloc( oi_frame.operandCount );
  if( oi_frame.operand != NULL )
  {
    dsm_pullup( &dsm_ptr, oi_frame.operand, oi_frame.operandCount );
    cmd_ptr->cmd_hdr.cmd_status =
      OI_AVRCPTarget_SendGenericReply( &oi_bd_addr,
                                       cmd->txn,
                                       &oi_frame );
  }
  else
  {
    dsm_free_packet( &dsm_ptr );	
    cmd_ptr->cmd_hdr.cmd_status = BT_CS_PF_OUT_OF_MEMORY;
  }
  cmd->frame.operand = dsm_ptr;
}

#ifdef FEATURE_BT_AVRCP_13

/*===========================================================================
                   AVRCP METADATA TRANSFER COMMANDS
===========================================================================*/
/*===========================================================================

FUNCTION
  bt_pf_cmd_avrcp_metadata_enable

DESCRIPTION

===========================================================================*/
void bt_pf_cmd_avrcp_metadata_enable( bt_pf_cmd_msg_type *cmd_ptr ) 
{
  bt_pf_avrcp_13_ext_app_id = cmd_ptr->cmd_hdr.app_id;
}
/*===========================================================================

FUNCTION
  bt_pf_avrcp_cmd_send_eventid_reply

DESCRIPTION

===========================================================================*/
void bt_pf_avrcp_cmd_send_eventid_reply ( bt_pf_cmd_msg_type *cmd_ptr )
{
  OI_BD_ADDR                                 oi_bd_addr;
  uint8                                      *event_ids;
  OI_STATUS                                  status; 
  bt_cmd_pf_avrcp_send_eventid_reply_type    *cmd;  

  cmd = &cmd_ptr->cmd_msg.cmd_eventid_reply_type;

  bt_pf_copy_bd_addr_to_oi( &oi_bd_addr, &bt_pf_avrcp_bd_addr );
  event_ids = OI_Malloc( cmd->event_count );

  if( event_ids != NULL ) 
  {
    OI_MemCopy(event_ids, cmd->event_ids, cmd->event_count);
     
    status = OI_AVRCPTarget_SendEventIDReply( &oi_bd_addr,
                                              cmd->txn,
                                              cmd->event_count,
                                              event_ids );

    cmd_ptr->cmd_hdr.cmd_status = status;

    if ( status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW ) 
    {
  
      bt_pf_copy_bd_addr_to_oi( &bt_pf_avrcp_fragment_info.addr, 
                                &bt_pf_avrcp_bd_addr );
      bt_pf_avrcp_fragment_info.count = 1;
      bt_pf_avrcp_fragment_info.is_pending = TRUE;
      bt_pf_avrcp_fragment_info.pdu_id = 
      OI_AVRCP_GET_CAPABILITIES;
      bt_pf_avrcp_fragment_info.base.data = NULL;
      bt_pf_avrcp_fragment_info.base.attr = (void *) &event_ids[0];
      bt_pf_avrcp_num_frag_pending++ ;
    }
    else
    { 
      if (OI_SUCCESS(status)) 
      {
        BT_MSG_DEBUG("BT PF AVRCP: event id reply status %x", 
                     status,0,0);
      }
      else
      {
        BT_ERR("BT PF AVRCP: error send event id reply status %x", 
               status,0,0);
      }

      if( event_ids != NULL )
      {
        OI_Free( event_ids );
        event_ids = NULL;
      }
      
    }
  }
  else
  {  
    cmd_ptr->cmd_hdr.cmd_status = BT_CS_PF_OUT_OF_MEMORY;
    BT_ERR("BT PF AVRCP: send event id reply. Out of Memory", 0,0,0);
  }

  if( cmd->event_ids != NULL )
  {
    AV_Free( cmd->event_ids );
    cmd->event_ids = NULL;
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_cmd_send_compid_reply

DESCRIPTION

===========================================================================*/

void bt_pf_avrcp_cmd_send_compid_reply ( bt_pf_cmd_msg_type *cmd_ptr )
{
  OI_BD_ADDR                                 oi_bd_addr;
  uint32                                     *comp_ids;
  OI_STATUS                                  status; 
  bt_cmd_pf_avrcp_send_compid_reply_type     *cmd;
  uint32                                     i;

  cmd = &cmd_ptr->cmd_msg.cmd_compid_reply_type;

  bt_pf_copy_bd_addr_to_oi( &oi_bd_addr, &bt_pf_avrcp_bd_addr );
  comp_ids = OI_Malloc( cmd->comp_count * sizeof(uint32));

  if( comp_ids != NULL ) 
  {
    for(i = 0; i < cmd->comp_count; i++ )
    {
      comp_ids[i] = cmd->comp_ids[i];
    }

    status = OI_AVRCPTarget_SendCompanyIDReply( &oi_bd_addr,
                                                cmd->txn,
                                                cmd->comp_count,
                                                comp_ids );

    cmd_ptr->cmd_hdr.cmd_status = status;

    if ( status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW ) 
    {
      
      bt_pf_copy_bd_addr_to_oi( &bt_pf_avrcp_fragment_info.addr, 
                                &bt_pf_avrcp_bd_addr );
      bt_pf_avrcp_fragment_info.count = 1;
      bt_pf_avrcp_fragment_info.is_pending = TRUE;
      bt_pf_avrcp_fragment_info.pdu_id = 
      OI_AVRCP_GET_CAPABILITIES;
      bt_pf_avrcp_fragment_info.base.data = NULL;
      bt_pf_avrcp_fragment_info.base.attr = (void *) &comp_ids[0];
      bt_pf_avrcp_num_frag_pending++;
    }
    else
    { 
      if (OI_SUCCESS(status)) 
      {
        BT_MSG_DEBUG("BT PF AVRCP: status %x", 
                     status,0,0);
      }
      else
      {
        BT_ERR("BT PF AVRCP: error send company id reply status %x", 
               status,0,0);
      }
      if( comp_ids != NULL )
      {
        OI_Free( comp_ids );
        comp_ids = NULL;
      }
    }
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_status = BT_CS_PF_OUT_OF_MEMORY;
    BT_ERR("BT PF AVRCP: send company id reply. Out of Memory", 0,0,0);
  }

  if( cmd->comp_ids != NULL )
  {
    AV_Free( cmd->comp_ids );
    cmd->comp_ids = NULL;
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_cmd_send_list_attrid_reply

DESCRIPTION

===========================================================================*/
void bt_pf_avrcp_cmd_send_list_attrid_reply ( bt_pf_cmd_msg_type *cmd_ptr )
{
  OI_BD_ADDR                                   oi_bd_addr;
  uint8                                        *attr_ids;
  OI_STATUS                                    status; 
  bt_cmd_pf_avrcp_send_list_attrid_reply_type  *cmd;

  cmd = &cmd_ptr->cmd_msg.cmd_list_attrid_reply_type;

  bt_pf_copy_bd_addr_to_oi( &oi_bd_addr, &bt_pf_avrcp_bd_addr );
  attr_ids = OI_Malloc( cmd->attr_count );

  if( attr_ids != NULL ) 
  {
    OI_MemCopy(attr_ids, cmd->attr_ids, cmd->attr_count);
     
    status =   OI_AVRCPTarget_ListAttrIDReply( &oi_bd_addr,
                                               cmd->txn,
                                               cmd->attr_count,
                                               attr_ids );

    cmd_ptr->cmd_hdr.cmd_status = status;

    if ( status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW ) 
    {
      bt_pf_copy_bd_addr_to_oi( &bt_pf_avrcp_fragment_info.addr, 
                                &bt_pf_avrcp_bd_addr );
      bt_pf_avrcp_fragment_info.count = 1;
      bt_pf_avrcp_fragment_info.is_pending = TRUE;
      bt_pf_avrcp_fragment_info.pdu_id = 
      OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_ATTRIBUTES;
      bt_pf_avrcp_fragment_info.base.data = NULL;
      bt_pf_avrcp_fragment_info.base.attr = (void *) &attr_ids[0];
      bt_pf_avrcp_num_frag_pending++;
    }
    else
    { 
      if (OI_SUCCESS(status)) 
      {
        BT_MSG_DEBUG("BT PF AVRCP: status %x", 
                     status,0,0);
      }
      else
      {
        BT_ERR("BT PF AVRCP: error send attribute id reply status %x", 
               status,0,0);
      }

      if( attr_ids != NULL )
      {
        OI_Free( attr_ids );
        attr_ids = NULL;
      }  
    }
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_status = BT_CS_PF_OUT_OF_MEMORY;
    BT_ERR("BT PF AVRCP: send attribute id reply. Out of Memory", 0,0,0);
  } 

  if( cmd->attr_ids != NULL )
  {
    AV_Free( cmd->attr_ids );
    cmd->attr_ids = NULL;
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_cmd_send_list_attrvalue_reply

DESCRIPTION

===========================================================================*/
void bt_pf_avrcp_cmd_send_list_attrvalue_reply ( bt_pf_cmd_msg_type *cmd_ptr )
{
  OI_BD_ADDR                                      oi_bd_addr;
  uint8                                           *attr_settings_vals;
  OI_STATUS                                       status; 
  bt_cmd_pf_avrcp_send_list_attrvalue_reply_type  *cmd;

  cmd = &cmd_ptr->cmd_msg.cmd_list_attrvalue_reply_type;

  bt_pf_copy_bd_addr_to_oi( &oi_bd_addr, &bt_pf_avrcp_bd_addr );
  attr_settings_vals = OI_Malloc( cmd->attr_val_count );

  if( attr_settings_vals != NULL ) 
  {
    OI_MemCopy(attr_settings_vals, cmd->attr_settings_vals, cmd->attr_val_count);
     
    status = OI_AVRCPTarget_ListAttrValuesReply( &oi_bd_addr,
                                                 cmd->txn,
                                                 cmd->attr_id,
                                                 cmd->attr_val_count,
                                                 cmd->attr_settings_vals );

    cmd_ptr->cmd_hdr.cmd_status = status;

    if ( status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW ) 
    {
      bt_pf_copy_bd_addr_to_oi( &bt_pf_avrcp_fragment_info.addr, 
                                &bt_pf_avrcp_bd_addr );
      bt_pf_avrcp_fragment_info.count = 1;
      bt_pf_avrcp_fragment_info.is_pending = TRUE;
      bt_pf_avrcp_fragment_info.pdu_id = 
      OI_AVRCP_LIST_PLAYER_APPLICATION_SETTING_VALUES;
      bt_pf_avrcp_fragment_info.base.data = NULL;
      bt_pf_avrcp_fragment_info.base.attr = (void *) &attr_settings_vals[0];
      bt_pf_avrcp_num_frag_pending++;
    }
    else
    { 
      if (OI_SUCCESS(status)) 
      {
        BT_MSG_DEBUG("BT PF AVRCP: status %x", 
                     status,0,0);
      }
      else
      {
        BT_ERR("BT PF AVRCP: error send list attribute val reply status %x", 
               status,0,0);
      }

      if( attr_settings_vals != NULL )
      {
        OI_Free( attr_settings_vals );
        attr_settings_vals = NULL;
      }
    }
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_status = BT_CS_PF_OUT_OF_MEMORY;
    BT_ERR("BT PF AVRCP: send list attribute val reply. Out of Memory", 0,0,0);
  }  

  if( cmd->attr_settings_vals != NULL )
  {
    AV_Free( cmd->attr_settings_vals );
    cmd->attr_settings_vals = NULL;
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_cmd_send_get_attr_value_reply

DESCRIPTION

===========================================================================*/
void bt_pf_avrcp_cmd_send_get_attr_value_reply ( bt_pf_cmd_msg_type *cmd_ptr )
{
  OI_BD_ADDR                                      oi_bd_addr;
  OI_AVRCP_ATTRIBUTE_PAIR                         attrs;
  OI_STATUS                                       status; 
  bt_cmd_pf_avrcp_send_get_attr_value_reply_type  *cmd;

  cmd = &cmd_ptr->cmd_msg.cmd_get_attr_value_reply_type;

  bt_pf_copy_bd_addr_to_oi( &oi_bd_addr, &bt_pf_avrcp_bd_addr );
  /* Allocate memory for attrids & attr values corresponding to each attrid */ 
  attrs = OI_Malloc( cmd->attr_id_count * 2 * sizeof(OI_AVRCP_ATTRIBUTE_PAIR));

  if( attrs != NULL ) 
  {
    OI_MemCopy( attrs, cmd->attrs, (cmd->attr_id_count * 2) );
     
    status = OI_AVRCPTarget_GetAttrValueReply( &oi_bd_addr,
                                               cmd->txn,
                                               cmd->attr_id_count,
                                               attrs );

    cmd_ptr->cmd_hdr.cmd_status = status;

    if ( status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW ) 
    {
      bt_pf_copy_bd_addr_to_oi( &bt_pf_avrcp_fragment_info.addr, 
                                &bt_pf_avrcp_bd_addr );
      bt_pf_avrcp_fragment_info.count = 1;
      bt_pf_avrcp_fragment_info.is_pending = TRUE;
      bt_pf_avrcp_fragment_info.pdu_id = 
      OI_AVRCP_GET_CURRENT_PLAYER_APPLICATION_SETTING_VALUE;
      bt_pf_avrcp_fragment_info.base.data = NULL;
      bt_pf_avrcp_fragment_info.base.attr = (void *) &attrs[0];
      bt_pf_avrcp_num_frag_pending++;
    }
    else
    { 
      if (OI_SUCCESS(status)) 
      {
        BT_MSG_DEBUG("BT PF AVRCP: status %x", 
                     status,0,0);
      }
      else
      {
        BT_ERR("BT PF AVRCP: error get attribute val reply status %x", 
               status,0,0);
      }   
      if( attrs != NULL )
      {
        OI_Free( attrs );
        attrs = NULL;
      }
    }
  }
  else
  {
    cmd_ptr->cmd_hdr.cmd_status = BT_CS_PF_OUT_OF_MEMORY;
    BT_ERR("BT PF AVRCP: send get attribute val reply. Out of Memory", 0,0,0);
  } 

  if( cmd->attrs != NULL )
  {
    AV_Free( cmd->attrs );
    cmd->attrs = NULL;
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_cmd_send_list_attrvalue_reply

DESCRIPTION

===========================================================================*/
void bt_pf_avrcp_cmd_send_get_attr_setting_text_reply ( bt_pf_cmd_msg_type *cmd_ptr )
{
  OI_BD_ADDR                                             oi_bd_addr;
  OI_AVRCP_ATTRIBUTE_TEXT                                *attrs_text;
  OI_STATUS                                              status; 
  bt_cmd_pf_avrcp_send_get_attr_setting_text_reply_type  *cmd;
  uint32                                                   i;
  uint32                                                   j;
  boolean                                                 mem_alloc = TRUE;

  cmd = &cmd_ptr->cmd_msg.cmd_get_attr_setting_text_reply_type;
  attrs_text = bt_pf_avrcp_player_settings;

  bt_pf_copy_bd_addr_to_oi( &oi_bd_addr, &bt_pf_avrcp_bd_addr );

  for ( i = 0; i < cmd->attr_id_count; i++ ) 
  {
    attrs_text[i].id      = cmd->attrs_text[i].item_id;
    attrs_text[i].charSet = cmd->attrs_text[i].item_charset;
    attrs_text[i].value.sz = 
    cmd->attrs_text[i].item_string_val.num_of_bytes;

    attrs_text[i].value.p = 
    OI_Malloc( attrs_text[i].value.sz );
  

    if( ( attrs_text[i].value.p ) == NULL ) 
    {
      cmd_ptr->cmd_hdr.cmd_status = BT_CS_PF_OUT_OF_MEMORY;
      BT_ERR("BT PF AVRCP: send get attribute setting text reply. Out of Memory", 
             0,0,0);

      for ( j = 0; j < i ; j++ ) 
      {
        OI_Free( attrs_text[j].value.p );
        attrs_text[j].value.p = NULL;
      }
      mem_alloc = FALSE;
      break;
    }
    else
    {
      OI_MemCopy( attrs_text[i].value.p, 
                  cmd->attrs_text[i].item_string_val.p, 
                  attrs_text[i].value.sz );
    }
  }

  /* Freeing the memory allocated in OEM before returning */
  for( i = 0; i < cmd->attr_id_count; i++ )
  {
    if( cmd->attrs_text[i].item_string_val.p != NULL )
    {
      AV_Free( cmd->attrs_text[i].item_string_val.p );
      cmd->attrs_text[i].item_string_val.p = NULL;
    }
  }

  if( cmd->attrs_text != NULL )
  {
    AV_Free( (void *)cmd->attrs_text );
    cmd->attrs_text = NULL;
  }

  if ( mem_alloc == FALSE )
  {
    return;
  }
     
  status = OI_AVRCPTarget_GetAttrSettingTextReply( &oi_bd_addr,
                                                   cmd->txn,
                                                   cmd->attr_id_count,
                                                   attrs_text );

  cmd_ptr->cmd_hdr.cmd_status = status;

  if ( status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW ) 
  {
    bt_pf_copy_bd_addr_to_oi( &bt_pf_avrcp_fragment_info.addr, 
                              &bt_pf_avrcp_bd_addr );
    bt_pf_avrcp_fragment_info.count = cmd->attr_id_count;
    bt_pf_avrcp_fragment_info.is_pending = TRUE;
    bt_pf_avrcp_fragment_info.pdu_id = 
      OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_ATTRIBUTE_TEXT;
    bt_pf_avrcp_fragment_info.base.data = attrs_text[0].value.p;
    bt_pf_avrcp_fragment_info.base.attr = (void *) &attrs_text[0];
    bt_pf_avrcp_num_frag_pending++;
  }
  else
  { 
    if (OI_SUCCESS(status)) 
    {
      BT_MSG_DEBUG("BT PF AVRCP: status %x", 
                   status,0,0);
    }
    else
    {
      BT_ERR("BT PF AVRCP: error get attribute setting text reply status %x", 
             status,0,0);
    }  

    for ( i = 0; i < cmd->attr_id_count; i++ )
    {
      if( attrs_text[i].value.p != NULL )
      {
        OI_Free( attrs_text[i].value.p );
        attrs_text[i].value.p = NULL;
      }
    }
  }     
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_cmd_send_get_attr_setting_value_text_reply

DESCRIPTION

===========================================================================*/
void bt_pf_avrcp_cmd_send_get_attr_setting_value_text_reply ( bt_pf_cmd_msg_type *cmd_ptr )
{
  OI_BD_ADDR                                                   oi_bd_addr;
  OI_AVRCP_ATTRIBUTE_TEXT                                      *attrs_val_text;
  OI_STATUS                                                    status; 
  bt_cmd_pf_avrcp_send_get_attr_setting_value_text_reply_type  *cmd;
  uint32                                                         i;
  uint32                                                         j; 
  boolean                                                      mem_alloc = TRUE;

  cmd = &cmd_ptr->cmd_msg.cmd_get_attr_setting_value_text_reply_type;
  attrs_val_text = bt_pf_avrcp_player_attr_val_settings;

  bt_pf_copy_bd_addr_to_oi( &oi_bd_addr, &bt_pf_avrcp_bd_addr );

  for ( i = 0; i < cmd->attr_val_count; i++ )
  {
    attrs_val_text[i].id      = cmd->attrs_val_text[i].item_id;
    attrs_val_text[i].charSet = cmd->attrs_val_text[i].item_charset;
    attrs_val_text[i].value.sz = 
    cmd->attrs_val_text[i].item_string_val.num_of_bytes;
    attrs_val_text[i].value.p = 
    OI_Malloc( attrs_val_text[i].value.sz );

    if( ( attrs_val_text[i].value.p ) == NULL ) 
    {
      cmd_ptr->cmd_hdr.cmd_status = BT_CS_PF_OUT_OF_MEMORY;
      BT_ERR("BT PF AVRCP: send get attr setting val text reply. Out of Memory", 
           0,0,0);

      for ( j = 0; j < i ; j++ ) 
      {
        OI_Free( attrs_val_text[j].value.p );
        attrs_val_text[j].value.p = NULL;
      }
      mem_alloc = FALSE;
      break;
    } 
    else  
    {
      OI_MemCopy( attrs_val_text[i].value.p, 
                  cmd->attrs_val_text[i].item_string_val.p, 
                  attrs_val_text[i].value.sz );
     
    }
  }

  /* Freeing the memory allocated in OEM before returning */
  for( i = 0; i < cmd->attr_val_count; i++ )
  {
    if( cmd->attrs_val_text[i].item_string_val.p != NULL )
    {
      AV_Free( cmd->attrs_val_text[i].item_string_val.p );
      cmd->attrs_val_text[i].item_string_val.p = NULL;
    }
  }

  if( cmd->attrs_val_text != NULL )
  {
    AV_Free( (void *)cmd->attrs_val_text );
    cmd->attrs_val_text = NULL;
  }

  if ( mem_alloc == FALSE )
  {
    return;
  }

  status = OI_AVRCPTarget_GetAttrSettingValueTextReply( &oi_bd_addr,
                                                        cmd->txn,
                                                        cmd->attr_val_count,
                                                        attrs_val_text );
  cmd_ptr->cmd_hdr.cmd_status = status;
  if ( status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW ) 
  {
    bt_pf_copy_bd_addr_to_oi( &bt_pf_avrcp_fragment_info.addr, 
                              &bt_pf_avrcp_bd_addr );
    bt_pf_avrcp_fragment_info.count = cmd->attr_val_count;
    bt_pf_avrcp_fragment_info.is_pending = TRUE;
    bt_pf_avrcp_fragment_info.pdu_id = 
      OI_AVRCP_GET_PLAYER_APPLICATION_SETTING_VALUE_TEXT;
    bt_pf_avrcp_fragment_info.base.data = attrs_val_text[0].value.p;
    bt_pf_avrcp_fragment_info.base.attr = (void *) &attrs_val_text[0];
    bt_pf_avrcp_num_frag_pending++;
  }
  else
  { 
    if (OI_SUCCESS(status)) 
    {
      BT_MSG_DEBUG("BT PF AVRCP: status %x", 
                   status,0,0);
    }
    else
    {
      BT_ERR("BT PF AVRCP: error get attr. setting val text reply status %x", 
             status,0,0);
    } 

    for ( i = 0; i < cmd->attr_val_count; i++ )
    {
      if( attrs_val_text[i].value.p != NULL )
      {
        OI_Free( attrs_val_text[i].value.p );
        attrs_val_text[i].value.p = NULL;
      }
    } 
  }
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_cmd_send_list_attrvalue_reply

DESCRIPTION

===========================================================================*/
void bt_pf_avrcp_cmd_send_play_status_reply ( bt_pf_cmd_msg_type *cmd_ptr )
{
  OI_BD_ADDR                                            oi_bd_addr;
  OI_AVRCP_PLAY_STATUS                                  play_status;
  OI_STATUS                                             status; 
  bt_cmd_pf_avrcp_send_play_status_reply_type           *cmd;

  cmd = &cmd_ptr->cmd_msg.cmd_play_status_reply_type;

  bt_pf_copy_bd_addr_to_oi( &oi_bd_addr, &bt_pf_avrcp_bd_addr );

  play_status.songLength    = cmd->play_status.song_length;
  play_status.songPosition  = cmd->play_status.song_position;
  play_status.status        = cmd->play_status.status;

  status =  OI_AVRCPTarget_PlayStatusReply( &oi_bd_addr,
                                            cmd->txn,
                                            &play_status );

  cmd_ptr->cmd_hdr.cmd_status = status;

  if (OI_SUCCESS(status)) 
  {
    BT_MSG_DEBUG("BT PF AVRCP: play ststus reply %x", 
                 status,0,0);
  }
  else
  {
    BT_ERR("BT PF AVRCP: error send play status reply status %x", 
           status,0,0);
  }    
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_cmd_send_list_attrvalue_reply

DESCRIPTION

===========================================================================*/
void bt_pf_avrcp_cmd_send_get_element_attributes_reply ( bt_pf_cmd_msg_type *cmd_ptr )
{
  OI_BD_ADDR                                                   oi_bd_addr;
  OI_AVRCP_ELEMENT_ATTRIBUTE                                   *media_element_attr;
  OI_STATUS                                                    status; 
  bt_cmd_pf_avrcp_send_get_element_attributes_reply_type       *cmd;
  uint32                                                        i;
  uint32                                                        j;
  boolean                                                      mem_alloc = TRUE;

  cmd = &cmd_ptr->cmd_msg.cmd_get_element_attributes_reply_type;
  media_element_attr = bt_pf_avrcp_media_elements;
  bt_pf_copy_bd_addr_to_oi( &oi_bd_addr, &bt_pf_avrcp_bd_addr );
  
  for (i = 0; i < cmd->media_attr_count; i++ ) 
  {
    media_element_attr[i].attrID  = cmd->media_element_attr[i].media_attr_id;
    media_element_attr[i].charSet = cmd->media_element_attr[i].char_set;
    media_element_attr[i].value.sz = 
    cmd->media_element_attr[i].media_attr_val.num_of_bytes;
    media_element_attr[i].value.p = 
    OI_Malloc( media_element_attr[i].value.sz );

    if ( ( media_element_attr[i].value.p ) == NULL ) 
    {
      cmd_ptr->cmd_hdr.cmd_status = BT_CS_PF_OUT_OF_MEMORY;
      BT_ERR("BT PF AVRCP: send get media element attr reply. Out of Memory", 
             0,0,0);

      for ( j = 0; j < i ; j++ ) 
      {
        OI_Free( media_element_attr[j].value.p );
        media_element_attr[j].value.p = NULL;
      }
      mem_alloc = FALSE;
      break;
    }
    else
    {
      OI_MemCopy( media_element_attr[i].value.p, 
                  cmd->media_element_attr[i].media_attr_val.p, 
                  media_element_attr[i].value.sz );
    }
  }

  /* Freeing all the memory allocated in the OEM before returning */
  for( i = 0; i < cmd->media_attr_count; i++ )
  {
    if( cmd->media_element_attr[i].media_attr_val.p != NULL )
    {
      AV_Free( cmd->media_element_attr[i].media_attr_val.p );
      cmd->media_element_attr[i].media_attr_val.p = NULL;
    }
  }

  if( cmd->media_element_attr != NULL )
  {
    AV_Free( (void *)cmd->media_element_attr );
    cmd->media_element_attr = NULL;
  }

  if ( mem_alloc == FALSE )
  {
    return;
  }


  status = OI_AVRCPTarget_GetElementAttributesReply( &oi_bd_addr,
                                                     cmd->txn,
                                                     cmd->media_attr_count,
                                                     media_element_attr );

  cmd_ptr->cmd_hdr.cmd_status = status;

  if ( status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW ) 
  {
    bt_pf_copy_bd_addr_to_oi( &bt_pf_avrcp_fragment_info.addr, 
                              &bt_pf_avrcp_bd_addr );
    bt_pf_avrcp_fragment_info.count = cmd->media_attr_count;
    bt_pf_avrcp_fragment_info.is_pending = TRUE;
    bt_pf_avrcp_fragment_info.pdu_id = OI_AVRCP_GET_ELEMENT_ATTRIBUTES;
    bt_pf_avrcp_fragment_info.base.data = media_element_attr[0].value.p;
    bt_pf_avrcp_fragment_info.base.attr = (void *) &media_element_attr[0];
    bt_pf_avrcp_num_frag_pending++;
  }
  else
  { 
    if (OI_SUCCESS(status)) 
    {
      BT_MSG_DEBUG("BT PF AVRCP: status %x", 
                    status,0,0);
    }
    else
    {
      BT_ERR("BT PF AVRCP: error get media element attr reply status %x", 
             status,0,0);
    }   

    for (i = 0; i < cmd->media_attr_count; i++ )
    {
      if( media_element_attr[i].value.p != NULL )
      {
        OI_Free( media_element_attr[i].value.p );
        media_element_attr[i].value.p = NULL;
      }
    }    
  }  
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_cmd_send_list_attrvalue_reply

DESCRIPTION

===========================================================================*/
void bt_pf_avrcp_cmd_send_register_notification_reply ( bt_pf_cmd_msg_type *cmd_ptr )
{
  OI_BD_ADDR                                                   oi_bd_addr;
  OI_AVRCP_EVENT                                               *ev_metadata;
  OI_STATUS                                                    status; 
  bt_cmd_pf_avrcp_send_register_notification_reply_type        *cmd;

  cmd = &cmd_ptr->cmd_msg.cmd_register_notification_reply_type;
  ev_metadata = &bt_pf_avrcp_notify_ev_metadata;
  bt_pf_copy_bd_addr_to_oi( &oi_bd_addr, &bt_pf_avrcp_bd_addr );

  ev_metadata->id = cmd->ev_metadata.event_id;
  
  switch( ev_metadata->id )
  {
    case BT_PF_AVRCP_EVENT_PLAYBACK_STATUS_CHANGED:
      ev_metadata->u.playStatus = cmd->ev_metadata.u.play_status;
      break;

    case BT_PF_AVRCP_EVENT_TRACK_CHANGED:
      OI_MemCopy( ev_metadata->u.trackIndex, (void const *)cmd->ev_metadata.u.track_index, 
                  BT_PF_AVRCP_SIZE_OF_CURRENT_TRACK_ID );
    case BT_PF_AVRCP_EVENT_TRACK_REACHED_END:
    case BT_PF_AVRCP_EVENT_TRACK_REACHED_START:
      break;

    case BT_PF_AVRCP_EVENT_PLAYBACK_POS_CHANGED:
      ev_metadata->u.playbackPosition = cmd->ev_metadata.u.playback_pos;
      break;

    case BT_PF_AVRCP_EVENT_BATT_STATUS_CHANGED:
      ev_metadata->u.batteryStatus = cmd->ev_metadata.u.battery_status;
      break;

    case BT_PF_AVRCP_EVENT_SYSTEM_STATUS_CHANGED:
      ev_metadata->u.systemStatus = cmd->ev_metadata.u.system_status;
      break;

    case BT_PF_AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED:   
      ev_metadata->u.deviceSetting.numAttr = 
        cmd->ev_metadata.u.device_setting.num_of_attr;
      ev_metadata->u.deviceSetting.attributes = 
      OI_Malloc( ev_metadata->u.deviceSetting.numAttr * 2);
      if( ev_metadata->u.deviceSetting.attributes != NULL )
      {
        OI_MemCopy( ev_metadata->u.deviceSetting.attributes, 
                   cmd->ev_metadata.u.device_setting.attributes,
                   ev_metadata->u.deviceSetting.numAttr * 2);
      }
      else
      {
        cmd_ptr->cmd_hdr.cmd_status = BT_CS_PF_OUT_OF_MEMORY;
        BT_ERR("BT PF AVRCP: register notification reply. Out of Memory", 
               0,0,0);
      }
      if( cmd->ev_metadata.u.device_setting.attributes != NULL )
      {
        AV_Free( cmd->ev_metadata.u.device_setting.attributes );
        cmd->ev_metadata.u.device_setting.attributes = NULL;
      }
      break;
  }

  
  status = OI_AVRCPTarget_RegisterNotificationReply( &oi_bd_addr,
                                                     cmd->txn,
                                                     ev_metadata,
                                                     cmd->is_final );

  cmd_ptr->cmd_hdr.cmd_status = status;

  if ( status == OI_AVRCP_RESPONSE_PACKET_OVERFLOW ) 
  {
    bt_pf_copy_bd_addr_to_oi( &bt_pf_avrcp_fragment_info.addr, 
                             &bt_pf_avrcp_bd_addr );
    bt_pf_avrcp_fragment_info.count = 1;
    bt_pf_avrcp_fragment_info.is_pending = TRUE;
    bt_pf_avrcp_fragment_info.pdu_id = OI_AVRCP_REGISTER_NOTIFICATION;
    bt_pf_avrcp_fragment_info.base.data = NULL;
    bt_pf_avrcp_fragment_info.base.attr = 
    (void *) &ev_metadata->u.deviceSetting.attributes[0];
    bt_pf_avrcp_num_frag_pending++ ;
  } 
  else
  { 
    if (OI_SUCCESS(status)) 
    {
      BT_MSG_DEBUG("BT PF AVRCP: status %x, metadata event = %x, final=%x", 
                    status, ev_metadata->id,cmd->is_final);
    }
    else
    {
      BT_ERR("BT PF AVRCP: error reg notif reply status %x, event =%x, final=%x", 
             status,ev_metadata->id,cmd->is_final);
    }   

    if( ev_metadata->id == BT_PF_AVRCP_EVENT_PLAYER_APPLICATION_SETTING_CHANGED )
    {
      if( ev_metadata->u.deviceSetting.attributes != NULL )
      {
        OI_Free( ev_metadata->u.deviceSetting.attributes );
        ev_metadata->u.deviceSetting.attributes = NULL;    
      }  
    } 
  } 
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_cmd_send_group_navigation_reply

DESCRIPTION

===========================================================================*/
void bt_pf_avrcp_cmd_send_group_navigation_reply ( bt_pf_cmd_msg_type *cmd_ptr )
{
  OI_BD_ADDR                                            oi_bd_addr;
  OI_STATUS                                             status; 
  bt_cmd_pf_avrcp_send_group_navigation_reply_type      *cmd;

  cmd = &cmd_ptr->cmd_msg.cmd_group_navigation_reply_type;

  bt_pf_copy_bd_addr_to_oi( &oi_bd_addr, &bt_pf_avrcp_bd_addr );

  status =   OI_AVRCPTarget_GroupNavigationReply( &oi_bd_addr,
                                                   cmd->txn,
                                                   cmd->vendor_unique_id,
                                                   cmd->button_press,
                                                   cmd->resp_code );

  cmd_ptr->cmd_hdr.cmd_status = status;

  if (OI_SUCCESS(status)) 
  {
    BT_MSG_DEBUG("BT PF AVRCP: send group navifation reply %x button press = %x", 
                 status,cmd->button_press,0);
  }
  else
  {
    BT_ERR("BT PF AVRCP: error send group navigation reply status %x", 
           status,0,0);
  }   
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_cmd_send_simple_meta_reply

DESCRIPTION

===========================================================================*/
void bt_pf_avrcp_cmd_send_simple_meta_reply ( bt_pf_cmd_msg_type *cmd_ptr )
{
  OI_BD_ADDR                                                   oi_bd_addr;
  OI_STATUS                                                    status; 
  bt_cmd_pf_avrcp_cmd_send_simple_meta_reply_type              *cmd;

  cmd = &cmd_ptr->cmd_msg.cmd_send_simple_meta_reply_type;

  bt_pf_copy_bd_addr_to_oi( &oi_bd_addr, &bt_pf_avrcp_bd_addr );

  status =   OI_AVRCPTarget_SendSimpleMetaReply(&oi_bd_addr,
                                                cmd->txn,
                                                cmd->pdu_id,
                                                cmd->status);

  cmd_ptr->cmd_hdr.cmd_status = status;

  if (OI_SUCCESS(status)) 
  {
    BT_MSG_DEBUG("BT PF AVRCP: simple metadata reply %x", 
                 status,0,0);
  }
  else
  {
    BT_ERR("BT PF AVRCP: error simple metadata reply status %x", 
           status,0,0);
  }      
}

#endif /* FEATURE_BT_AVRCP_13 */

/*=========================================================================*/
/*                                                                         */
/*            EXTERNAL FUNCTIONS                                           */
/*                                                                         */
/*=========================================================================*/
/*===========================================================================

FUNCTION
  bt_pf_avrcp_current_supported_features

DESCRIPTION
  Returns the categories supported by AVRCP.

===========================================================================*/
extern uint16 bt_pf_avrcp_current_supported_features( void )
{
  return bt_pf_avrcp_supported_categories;
}

/*===========================================================================

FUNCTION
  bt_pf_avrcp_process_command

DESCRIPTION
  Processes all incomming commands to the AVRCP layer.

===========================================================================*/
void bt_pf_avrcp_process_command( bt_pf_cmd_msg_type* cmd_ptr )
{
  cmd_ptr->cmd_hdr.cmd_status = BT_CS_GN_SUCCESS;

  switch ( cmd_ptr->cmd_hdr.cmd_type )
  {
    case BT_PF_CMD_AVRCP_ENABLE:
      bt_pf_avrcp_cmd_enable( cmd_ptr );
      break;
    case BT_PF_CMD_AVRCP_CONNECT:
      bt_pf_avrcp_cmd_connect( cmd_ptr );
      break;
    case BT_PF_CMD_AVRCP_DISCONNECT:
      bt_pf_avrcp_cmd_disconnect( cmd_ptr );
      break;
    case BT_PF_CMD_AVRCP_DISABLE:
      bt_pf_avrcp_cmd_disable( cmd_ptr );
      break;
    case BT_PF_CMD_AVRCP_SEND_REPLY:
      bt_pf_avrcp_cmd_send_reply( cmd_ptr );
      break;
    case BT_PF_CMD_AVRCP_SEND_GENERIC_REPLY:
      bt_pf_avrcp_cmd_send_generic_reply( cmd_ptr );
      break;
#ifdef FEATURE_BT_AVRCP_13
  case BT_PF_CMD_AVRCP_METADATA_ENABLE:
     bt_pf_cmd_avrcp_metadata_enable( cmd_ptr );
     break;
  /* AVRCP meta data commands */
    case BT_PF_CMD_AVRCP_EVENTID_REPLY:
      bt_pf_avrcp_cmd_send_eventid_reply( cmd_ptr );
      break;
    case BT_PF_CMD_AVRCP_COMPID_REPLY:
      bt_pf_avrcp_cmd_send_compid_reply( cmd_ptr );
      break;
    case BT_PF_CMD_AVRCP_LIST_ATTRID_REPLY:
      bt_pf_avrcp_cmd_send_list_attrid_reply( cmd_ptr );
      break;
    case BT_PF_CMD_AVRCP_LIST_ATTRVALUE_REPLY:
      bt_pf_avrcp_cmd_send_list_attrvalue_reply( cmd_ptr );
      break;
    case BT_PF_CMD_AVRCP_GET_ATTRVALUE_REPLY:
      bt_pf_avrcp_cmd_send_get_attr_value_reply( cmd_ptr );
      break;
    case BT_PF_CMD_AVRCP_GET_ATTR_SETTING_TEXT_REPLY:
      bt_pf_avrcp_cmd_send_get_attr_setting_text_reply( cmd_ptr );
      break;
    case BT_PF_CMD_AVRCP_GET_ATTR_SETTING_VALUE_TEXT_REPLY:
      bt_pf_avrcp_cmd_send_get_attr_setting_value_text_reply( cmd_ptr );
      break;
    case BT_PF_CMD_AVRCP_PLAY_STATUS_REPLY:
      bt_pf_avrcp_cmd_send_play_status_reply( cmd_ptr );
      break;
    case BT_PF_CMD_AVRCP_GET_ELEMENTS_ATTR_REPLY:
      bt_pf_avrcp_cmd_send_get_element_attributes_reply( cmd_ptr );
      break;
    case BT_PF_CMD_AVRCP_REG_NOTIFICATION_REPLY:
      bt_pf_avrcp_cmd_send_register_notification_reply( cmd_ptr );
      break;
    case BT_PF_CMD_AVRCP_FRAGMENT_REPLY:
      break; 
    case BT_PF_CMD_AVRCP_GROUP_NAV_REPLY:
      bt_pf_avrcp_cmd_send_group_navigation_reply( cmd_ptr );
      break;
    case BT_PF_CMD_AVRCP_SIMPLE_META_REPLY:
      bt_pf_avrcp_cmd_send_simple_meta_reply( cmd_ptr );
      break;
#endif /* FEATURE_BT_AVRCP_13 */
    default:
      BT_ERR("BT PF AVRCP: Unknown command %x", cmd_ptr->cmd_hdr.cmd_type,0,0);
  }
}
#endif /* FEATURE_BT_PFEXT_AV */
#endif /* FEATURE_BT */
