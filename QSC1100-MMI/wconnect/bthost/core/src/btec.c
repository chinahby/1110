/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


       B L U E T O O T H    D R I V E R    E V E N T    C O N T R O L

                     C O M M A N D    I N T E R F A C E

GENERAL DESCRIPTION
  This module contains the Event Control (EC) command routines of the
  Bluetooth driver.

              Copyright (c) 2000-2008 QUALCOMM Incorporated.               
                      All Rights Reserved.                                  
              Qualcomm Confidential and Proprietary                        
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSHeader:   O:/src/asw/COMMON/vcs/btec.c_v   1.7   03 Apr 2002 13:25:06   propach
$Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btec.c#1 $ $DateTime:

$Log:   O:/src/asw/COMMON/vcs/btec.c_v  $
 *
 *    #13        12 Mar 2008            RS
 * Increased the BT_EC_MAX_BT_APPS count to accomodated the increasing 
 * APP registrations and packed the concerned structures.
 *
 *    #12        08 Mar 2007            PN
 * Fixed compile error due to missing comma.
 *
 *    #12        07 Mar 2007            CO
 * Fixed a bug in sending debug msg.
 *
 *    #11        16 Feb 2007            PN
 * Fixed critical lint errors.
 *
 *    #10        19 Oct 2006            PN
 * Changed BREW SPP events dispatching mechanism.
 *
 *    #9         12 Jul 2005            JH
 * Increased maximum application IDs allowed.
 *
 *    #8         24 Jun 2005            PN
 * Added bt_ec_deregister_brew_spp().
 *
 *    #7        31  Mar 2005            JTL
 * Only print out successfull Cmd Done not delivered messages once.
 *
 *    #5-6      27  Aug 2004            JTL
 * Changed TASKLOCK/FREE to rex_enter_crit_sect. This fixes problems associated
 * with callbacks being called with tasks locked.
 *
 *    #4         3  Oct 2003            DP
 * Increased maximum application IDs allowed.
 * 
 *    Rev 1.7   03 Apr 2002 13:25:06   propach
 * Removed command done filter in HCI/APIT modes.
 * 
 *    Rev 1.6   29 Aug 2001 11:43:48   propach
 * Merged certification tip onto main tip.
 * 
 *    Rev 1.5.1.1   18 Jul 2001 16:51:18   propach
 * Added APIT HCI mode loading the stand alone DSP image.
 * 
 *    Rev 1.5.1.0   29 Jun 2001 15:22:38   waynelee
 * No change.
 * 
 *    Rev 1.5   26 Mar 2001 18:01:08   propach
 * Renamed command status values and restored exclusion for QXDM
 * error messages on command done events when in APIT mode (until
 * we update scripts).
 * 
 *    Rev 1.4   21 Mar 2001 11:00:38   propach
 * Removed exclusion for command done events on QXDM error message.
 * 
 *    Rev 1.3   02 Mar 2001 00:54:30   ryuen
 * 1. Fixed bt_cmd_ec_app_id_is_registered to not check for DC appID
 * 2. Fixed bt_ec_register_event_range to properly set the minimum and maximum
 *    event range
 * 3. Added bt_ec_unregister_bt_app_id
 * 4. Updated bt_ec_unreg_using_event_set_id to handle unregistration using
 *    the ALL event set
 * 5. Updated bt_cmd_ec_free_application_id to call bt_ec_unregister_bt_app_id
 * 
 *    Rev 1.2   26 Feb 2001 12:36:06   propach
 * Fixed free app ID when unregistered already.
 * 
 *    Rev 1.1   16 Feb 2001 16:10:42   propach
 * Completed EC routing of events by BT application ID.
 * 
 *    Rev 1.0   29 Jan 2001 17:06:14   propach
 * Initial revision.
 * 
 *    Rev 1.12   01 Dec 2000 14:56:06   sramacha
 * Added NA event handling in bt_ec_send_event.
 * 
 *    Rev 1.11   29 Nov 2000 13:53:52   propach
 * Removed FEATURE_BT_NA.
 * 
 *    Rev 1.10   22 Nov 2000 11:45:04   sramacha
 * 
 *    Rev 1.9   20 Sep 2000 11:53:42   propach
 * Made bt_cmd_* return status and validate app ID per API document.
 *
 *    Rev 1.8   14 Sep 2000 19:03:42   ryuen
 * Updated bt_ec_send_event() to send the Driver Online and Offline events
 * to all registered application
 *
 *    Rev 1.7   08 Sep 2000 16:18:04   propach
 * Fix recording faults.  Make EC table cleaner upon unregister.
 *
 *    Rev 1.6   07 Sep 2000 15:31:54   propach
 * Fixed multiple calls to set HCI mode and fixed unregistering in EC.
 *
 *    Rev 1.5   07 Sep 2000 12:27:04   propach
 * Fixed build without AG.
 *
 *    Rev 1.4   05 Sep 2000 17:50:30   propach
 * Fixed HS featurization.
 *
 *    Rev 1.3   30 Aug 2000 09:41:26   waynelee
 * Made changes for new BT_MSG macros
 *
 *    Rev 1.2   29 Aug 2000 08:24:46   propach
 * Moved comdef.h includes above FEATURE_BT to remove compiler
 * warning when feature is disabled.
 *
 *    Rev 1.1   17 Aug 2000 13:42:46   propach
 * Added Event Control (EC).  Fixed minor problems in RFCOMM.
 *
 *    Rev 1.0   02 Aug 2000 09:34:44   propach
 * Initial revision.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_BT
#include "bt.h"
#include "bti.h"
#include "btmsg.h"
#include "rex.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/

#define BT_MSG_LAYER BT_MSG_GN

#define BT_EC_MAX_BT_APPS 60

typedef BT_PACKED struct
{
  bt_ev_cb_func_type*  ev_cb_ptr;
  boolean              in_use;
  boolean              is_registered;
  boolean              is_warned; /* User has been warned about 
                                   * undelivered messages */
} bt_app_id_table_type;

#define BT_EC_MAX_EC_ENTRIES 40

typedef BT_PACKED struct
{
  bt_ev_cb_func_type*  ev_cb_ptr;
  bt_event_type        min_event;
  bt_event_type        max_event;
  boolean              app_id_must_match;
  bt_app_id_type       bt_app_id;
} bt_ec_table_type;

typedef struct
{
  bt_event_set_type  ev_set_id;
  bt_event_type      min_event;
  bt_event_type      max_event;
} bt_ec_ev_set_table_type;


/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                         Data Definitions                                */
/*                                                                         */
/*-------------------------------------------------------------------------*/

LOCAL uint32 bt_ec_events_sent;

bt_app_id_table_type  bt_app_id_table[ BT_EC_MAX_BT_APPS ];

uint16            bt_ect_entries;
bt_ec_table_type  bt_ec_table[ BT_EC_MAX_EC_ENTRIES ];
bt_event_type     bt_ect_min_event;
bt_event_type     bt_ect_max_event;

#define BT_ECT_MIN( base ) ( ((bt_event_type)           \
                                   BT_CMD_EV_##base##_BASE) )

#define BT_ECT_MAX( base ) ( ((bt_event_type)                    \
              (BT_CMD_EV_##base##_BASE | BT_EC_EV_MAX_RANGE_MASK)) )

LOCAL const bt_ec_ev_set_table_type  bt_ec_ev_set_table[] =
  { { BT_EC_ES_ALL,           BT_EV_MIN_EVENT, BT_EV_MAX_EVENT },
    { BT_EC_ES_NETWORK_ACCESS, BT_ECT_MIN( NA ), BT_ECT_MAX( NA ) },
#ifdef FEATURE_BT_AG
    { BT_EC_ES_AUDIO_GATEWAY,  BT_ECT_MIN( AG ), BT_ECT_MAX( AG ) },
#endif
#ifdef FEATURE_BT_HS
#error code not present
#endif
    { BT_EC_ES_BONDING_APP,    BT_ECT_MIN( BA ), BT_ECT_MAX( BA ) },
    { BT_EC_ES_SERVICE_DISC,   BT_ECT_MIN( SD ), BT_ECT_MAX( SD ) },
    { BT_EC_ES_RFCOMM,         BT_ECT_MIN( RC ), BT_ECT_MAX( RC ) },
    { BT_EC_ES_L2CAP,          BT_ECT_MIN( L2 ), BT_ECT_MAX( L2 ) },
    { BT_EC_ES_RADIO_MANAGER,  BT_ECT_MIN( RM ), BT_ECT_MAX( RM ) },
    { BT_EC_ES_HCI,            BT_ECT_MIN( HC ), BT_ECT_MAX( HC ) },
    { BT_EC_ES_CUSTOM,         BT_EV_NULL, BT_EV_NULL } };

// Used to protect the ec tables:
LOCAL rex_crit_sect_type bt_ec_crit_sect;

#ifdef FEATURE_IBT
LOCAL bt_brew_spp_cb* brew_spp_cb_table[ 3 ];
#endif

/*===========================================================================

                         FUNCTION DEFINITIONS

===========================================================================*/

#ifdef FEATURE_IBT
/*===========================================================================

FUNCTION
  bt_ec_register_brew_spp

DESCRIPTION
  Initializes a callback function for BREW's SPP implementation

===========================================================================*/
void bt_ec_register_brew_spp( bt_brew_spp_cb* cb_ptr )
{

  uint8 i;
  for ( i = 0; i < ARR_SIZE( brew_spp_cb_table ); i++)
  {
    if ( brew_spp_cb_table[ i ] == NULL )
    {
      BT_MSG_DEBUG( "BT EC: BREW SPP Callback registered %d", i, 0, 0 );
      brew_spp_cb_table[ i ] = cb_ptr;
      break;
    }
  }
  if ( i >= ARR_SIZE( brew_spp_cb_table ) )
  {
    BT_ERR( "BT EC: Max BREW SPP Callbacks registered", 0, 0, 0 );
  }

}
/*===========================================================================

FUNCTION
  bt_ec_deregister_brew_spp

DESCRIPTION
  Clears the callback function for BREW's SPP implementation.

===========================================================================*/
void bt_ec_deregister_brew_spp( bt_brew_spp_cb* cb_ptr )
{

  uint8 i;
  for ( i = 0; i < ARR_SIZE( brew_spp_cb_table ); i++)
  {
    if ( brew_spp_cb_table[ i ] == cb_ptr )
    {
      BT_MSG_DEBUG( "BT EC: BREW SPP Callback deregistered %d", i, 0, 0 );
      brew_spp_cb_table[ i ] = NULL;
      break;
    }
  }
  if ( i >= ARR_SIZE( brew_spp_cb_table ) )
  {
    BT_ERR( "BT EC: BREW SPP Callback not found", 0, 0, 0 );
  }

}

/*===========================================================================

FUNCTION
  bt_ec_dispatch_brew_spp_ev

DESCRIPTION
  Invoke callback functions for BREW's SPP implementation.

===========================================================================*/
void bt_ec_dispatch_brew_spp_ev( void )
{

  uint8 i;
  for ( i = 0; i < ARR_SIZE( brew_spp_cb_table ); i++)
  {
    if ( brew_spp_cb_table[ i ] != NULL )
    {
      (brew_spp_cb_table[ i ])();
    }
  }
}
#endif /* FEATURE_IBT */

/*===========================================================================

FUNCTION
  bt_ec_set_ect_min_max

DESCRIPTION
  Sets the minimum and maximum events contained in the entire EC Table.

===========================================================================*/
LOCAL void bt_ec_set_ect_min_max
(
  void
)
{

  uint16  i;

  bt_ect_min_event = BT_EV_NULL;
  bt_ect_max_event = BT_EV_NULL;
  
  if ( bt_ect_entries > 0 )
  {
    bt_ect_min_event = bt_ec_table[ 0 ].min_event;
    bt_ect_max_event = BT_EV_MIN_EVENT;
    for ( i = 0; i < bt_ect_entries; i++ )
    {
      if ( bt_ec_table[ i ].max_event > bt_ect_max_event )
      {
        bt_ect_max_event = bt_ec_table[ i ].max_event;
      }
    }
  }

}


/*===========================================================================

FUNCTION
  bt_ec_init_app_id_table_entry

DESCRIPTION
  Initializes a specified entry in the Application ID table.

===========================================================================*/
LOCAL void bt_ec_init_app_id_table_entry
(
  bt_app_id_type  bt_app_id
)
{

  bt_app_id_table[ bt_app_id ].is_registered = FALSE;
  bt_app_id_table[ bt_app_id ].ev_cb_ptr = NULL;
  bt_app_id_table[ bt_app_id ].in_use = FALSE;
  bt_app_id_table[ bt_app_id ].is_warned = FALSE;

}


/*===========================================================================

FUNCTION
  bt_ec_init_ec_table_entry

DESCRIPTION
  Initializes a specified entry in the Event Control table.

===========================================================================*/
LOCAL void bt_ec_init_ec_table_entry
(
  uint16  ect_idx
)
{

  bt_ec_table[ ect_idx ].bt_app_id = BT_APP_ID_NULL;
  bt_ec_table[ ect_idx ].ev_cb_ptr = NULL;
  bt_ec_table[ ect_idx ].min_event = BT_EV_NULL;
  bt_ec_table[ ect_idx ].max_event = BT_EV_NULL;
  bt_ec_table[ ect_idx ].app_id_must_match = TRUE;

}


/*===========================================================================

FUNCTION
  bt_ec_initialize

DESCRIPTION
  Initializes entire Event Control (EC) layer.

===========================================================================*/
void bt_ec_initialize
(
  void
)
{

   bt_app_id_type  i;
   uint16          j;

   bt_ec_events_sent = 0;

   for ( i = 0; i < BT_EC_MAX_BT_APPS; i++ )
   {
     bt_ec_init_app_id_table_entry( i );
   }

   bt_ect_entries = 0;
   for ( j = 0; j < BT_EC_MAX_EC_ENTRIES; j++ )
   {
     bt_ec_init_ec_table_entry( j );
   }
   bt_ec_set_ect_min_max();
   rex_init_crit_sect (&bt_ec_crit_sect);

#ifdef FEATURE_IBT
   for ( i = 0; i < ARR_SIZE( brew_spp_cb_table ); i++)
   {
     brew_spp_cb_table[ i ] = NULL;
   }
#endif

}


/*===========================================================================

FUNCTION
  bt_cmd_ec_get_app_id_and_register

DESCRIPTION
  If available, this returns an available Bluetooth Application ID.
  Otherwise it returns BT_APP_ID_NULL.  It also registers a specified
  callback function for all events to be routed to the specified BT
  application ID.

===========================================================================*/
bt_app_id_type bt_cmd_ec_get_app_id_and_register
(
  bt_ev_cb_func_type*  ev_cb_ptr
)
{

  bt_app_id_type  i = 0, bt_app_id = BT_APP_ID_NULL;

  if ( ev_cb_ptr != NULL )
  {
    rex_enter_crit_sect(&bt_ec_crit_sect);
    while ( (bt_app_id == BT_APP_ID_NULL) && (i < BT_EC_MAX_BT_APPS) )
    {
      if ( bt_app_id_table[ i ].in_use == FALSE )
      {
        bt_app_id = i;
        bt_app_id_table[ i ].in_use = TRUE;
        bt_app_id_table[ i ].is_registered = TRUE;
        bt_app_id_table[ i ].is_warned = FALSE;
        bt_app_id_table[ i ].ev_cb_ptr = ev_cb_ptr;
      }
      i++;
    }
    rex_leave_crit_sect(&bt_ec_crit_sect);
  }
  else
  {
    BT_ERR( "BT EC: Bad CB ptr Get AID & Reg", 0, 0, 0 );
  }

  BT_MSG_API( "BT EC RTX: Get AID %x & Reg CB %x",
              bt_app_id, ev_cb_ptr, 0 );
  
  return ( bt_app_id );

}


/*===========================================================================

FUNCTION
  bt_cmd_ec_reregister_app_id

DESCRIPTION
  This routine overwrites the event callback function for a
  specified for all events to be routed to the specified BT
  application ID.

===========================================================================*/
bt_cmd_status_type bt_cmd_ec_reregister_app_id
(
  bt_app_id_type       bt_app_id,
  bt_ev_cb_func_type*  ev_cb_ptr
)
{

  bt_cmd_status_type    cmd_status = BT_CS_GN_SUCCESS;
  bt_app_id_table_type* table_entry_ptr = NULL;

  if ( bt_app_id < ARR_SIZE(bt_app_id_table) )
  {
    table_entry_ptr = &bt_app_id_table[ bt_app_id ];
  }

  rex_enter_crit_sect(&bt_ec_crit_sect);
  if ( (table_entry_ptr != NULL) &&                   /* valid app ID?       */
       (table_entry_ptr->in_use != FALSE) &&          /* found in table?     */
       ((table_entry_ptr->ev_cb_ptr != NULL) ||       /* valid callback? or  */
        (table_entry_ptr->is_registered == FALSE)) )  /* ... not registered? */
  {
    table_entry_ptr->ev_cb_ptr = ev_cb_ptr;
    if ( ev_cb_ptr == NULL )
    {
      table_entry_ptr->is_registered = FALSE;
    }
    else
    {
      table_entry_ptr->is_registered = TRUE;
    }
  }
  else
  {
    // invalid app id or app didn't register for all event
    cmd_status = BT_CS_GN_BAD_APP_ID;
    BT_ERR( "BT EC: Rereg bad AID %x", bt_app_id, 0, 0 );
  }
  rex_leave_crit_sect(&bt_ec_crit_sect);

  BT_MSG_API( "BT EC RTX: Rereg AID %x CB %x",
              bt_app_id, ev_cb_ptr, 0 );
  
  return ( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_ec_get_application_id

DESCRIPTION
  If available, this returns an available Bluetooth Application ID.
  Otherwise it returns BT_APP_ID_NULL.

===========================================================================*/
bt_app_id_type bt_cmd_ec_get_application_id
(
  void
)
{

  bt_app_id_type  i = 0, bt_app_id = BT_APP_ID_NULL;

  rex_enter_crit_sect(&bt_ec_crit_sect);
  while ( (bt_app_id == BT_APP_ID_NULL) && (i < BT_EC_MAX_BT_APPS) )
  {
    if ( bt_app_id_table[ i ].in_use == FALSE )
    {
      bt_app_id_table[ i ].in_use = TRUE;
      bt_app_id = i;
    }
    i++;
  }
  rex_leave_crit_sect(&bt_ec_crit_sect);

  BT_MSG_API( "BT EC RTX: Get AID %x", bt_app_id, 0, 0 );

  if ( bt_app_id == BT_APP_ID_NULL )
  {
    BT_ERR( "BT EC: Out of App IDs!", 0, 0, 0 );
  }

  return ( bt_app_id );

}


/*===========================================================================

FUNCTION
  bt_cmd_ec_app_id_is_registered

DESCRIPTION
  Returns TRUE if a specified BT application ID is in use and
  registered to receive events, otherwise it returns FALSE.

===========================================================================*/
boolean bt_cmd_ec_app_id_is_registered
(
  bt_app_id_type  bt_app_id
)
{

  boolean  app_id_is_regd = FALSE;

  if ( (bt_app_id < BT_EC_MAX_BT_APPS) &&
       (bt_app_id_table[ bt_app_id ].in_use != FALSE) )
  {
    app_id_is_regd = bt_app_id_table[ bt_app_id ].is_registered;
  }

  return ( app_id_is_regd );

}


/*===========================================================================

FUNCTION
  bt_ec_verify_parameters

DESCRIPTION
  Verifies a specified set of parameters as valid.

===========================================================================*/
LOCAL bt_cmd_status_type bt_ec_verify_parameters
(
  bt_app_id_type     bt_app_id,
  bt_event_set_type  ev_set_id,
  bt_event_type      min_event,
  bt_event_type      max_event
)
{

  uint16              i = 0;
  boolean             event_set_ok = FALSE;
  bt_cmd_status_type  cmd_status = BT_CS_GN_SUCCESS;

  if ( (bt_app_id < BT_EC_MAX_BT_APPS) &&
       (bt_app_id_table[ bt_app_id ].in_use != FALSE) &&
       (bt_app_id_table[ bt_app_id ].ev_cb_ptr == NULL) )
  {
    if ( ev_set_id != BT_EC_ES_CUSTOM )
    {
      while ( (event_set_ok == FALSE) &&
              (bt_ec_ev_set_table[ i ].min_event != BT_EV_NULL) )
      {
        if ( bt_ec_ev_set_table[ i ].ev_set_id == ev_set_id )
        {
          event_set_ok = TRUE;
        }
        i++;
      }
      if ( event_set_ok == FALSE )
      {
        cmd_status = BT_CS_EC_BAD_EVENT_SET;
      }
    }
    else
    {
      if ( (min_event > max_event) ||
           (min_event <= BT_EV_MIN_EVENT) ||
           (max_event >= BT_EV_MAX_EVENT) )
      {
        cmd_status = BT_CS_EC_BAD_EVENT_SET;
      }
    }
  }
  else
  {
    cmd_status = BT_CS_GN_BAD_APP_ID;
  }

  if ( cmd_status != BT_CS_GN_SUCCESS )
  {
    BT_ERR( "BT EC: Bad params AID %x ES %x St %x",
            bt_app_id, ev_set_id, cmd_status );
  }

  return ( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_ec_register_event_range

DESCRIPTION
  Inserts an entry into the Event Control Table representing a specified
  range of BT events and associated application information.

===========================================================================*/
LOCAL bt_cmd_status_type bt_ec_register_event_range
(
  bt_app_id_type       bt_app_id,
  bt_ev_cb_func_type*  ev_cb_ptr,
  bt_event_type        min_event,
  bt_event_type        max_event,
  boolean              app_id_must_match
)
{

  uint16              i, put_idx = 0;
  boolean             found = FALSE;
  bt_cmd_status_type  cmd_status = BT_CS_GN_SUCCESS;

  rex_enter_crit_sect(&bt_ec_crit_sect);
  
  while ( (cmd_status == BT_CS_GN_SUCCESS) && (found == FALSE) )
  {
    if ( put_idx < bt_ect_entries )
    {
      if ( min_event < bt_ec_table[ put_idx ].min_event )
      {
        found = TRUE;
      }
      else if ( (bt_app_id == bt_ec_table[ put_idx ].bt_app_id) &&
                (ev_cb_ptr == bt_ec_table[ put_idx ].ev_cb_ptr) &&
                (min_event == bt_ec_table[ put_idx ].min_event) &&
                (max_event == bt_ec_table[ put_idx ].max_event) &&
                (app_id_must_match ==
                   bt_ec_table[ put_idx ].app_id_must_match) )
      {
        cmd_status = BT_CS_EC_DUPLICATE_REG;
        BT_ERR( "BT EC: Dup reg AID %x ES %x %x",
                bt_app_id, min_event, max_event );
      }
      else
      {
        put_idx++;
      }
    }
    else
    {
      found = TRUE;
    }
  }

  if ( cmd_status == BT_CS_GN_SUCCESS )
  {
    for ( i = bt_ect_entries; i > put_idx; i-- )
    {
      bt_ec_table[ i ] = bt_ec_table[ (i - 1) ];
    }

    bt_ec_table[ put_idx ].bt_app_id = bt_app_id;
    bt_ec_table[ put_idx ].ev_cb_ptr = ev_cb_ptr;
    bt_ec_table[ put_idx ].min_event = min_event;
    bt_ec_table[ put_idx ].max_event = max_event;
    bt_ec_table[ put_idx ].app_id_must_match = app_id_must_match;

    bt_ect_entries++;
    
    bt_app_id_table[ bt_app_id ].is_registered = TRUE;
  
    bt_ec_set_ect_min_max();
  }
  
  rex_leave_crit_sect(&bt_ec_crit_sect);

  return ( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_ec_reg_using_event_set_id

DESCRIPTION
  Registers a specified set of BT events to be provided to a
  specified BT application using an event set identifier.

===========================================================================*/
LOCAL bt_cmd_status_type bt_ec_reg_using_event_set_id
(
  bt_app_id_type       bt_app_id,
  bt_ev_cb_func_type*  ev_cb_ptr,
  bt_event_set_type    ev_set_id,
  boolean              app_id_must_match
)
{

  uint16              i = 0;
  bt_cmd_status_type  cmd_status = BT_CS_GN_SUCCESS;

  while ( bt_ec_ev_set_table[ i ].min_event != BT_EV_NULL )
  {
    if ( bt_ec_ev_set_table[ i ].ev_set_id == ev_set_id )
    {
      cmd_status = bt_ec_register_event_range(
                     bt_app_id, ev_cb_ptr,
                     bt_ec_ev_set_table[ i ].min_event,
                     bt_ec_ev_set_table[ i ].max_event,
                     app_id_must_match );
    }
    i++;
  }
  
  return ( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_ec_register_event_set

DESCRIPTION
  Registers a specified set of BT events to be provided to a
  specified BT application.  This routine is internal to EC to
  allow parameterization of whether the registration includes
  matching the BT application ID or not.

===========================================================================*/
LOCAL bt_cmd_status_type bt_ec_register_event_set
(
  bt_app_id_type       bt_app_id,
  bt_ev_cb_func_type*  ev_cb_ptr,
  bt_event_set_type    ev_set_id,
  bt_event_type        min_event,
  bt_event_type        max_event,
  boolean              app_id_must_match
)
{

  bt_cmd_status_type  cmd_status;

  if ( bt_ect_entries < BT_EC_MAX_EC_ENTRIES )
  {
    if ( ev_cb_ptr != NULL )
    {
      cmd_status = bt_ec_verify_parameters( bt_app_id, ev_set_id,
                                            min_event, max_event );

      if ( cmd_status == BT_CS_GN_SUCCESS )
      {
        if ( ev_set_id != BT_EC_ES_CUSTOM )
        {
          cmd_status = bt_ec_reg_using_event_set_id(
                         bt_app_id, ev_cb_ptr, ev_set_id,
                         app_id_must_match );
        }
        else
        {
          cmd_status = bt_ec_register_event_range(
                         bt_app_id, ev_cb_ptr,
                         min_event, max_event,
                         app_id_must_match );
        }
      }
    }
    else
    {
      cmd_status = BT_CS_EC_BAD_CALLBACK_PTR;
    }
  }
  else
  {
    cmd_status = BT_CS_EC_REG_FULL;
    BT_ERR( "BT EC: Reg Full %x", bt_ect_entries, 0, 0 );
  }

  return ( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_ec_register_event_set

DESCRIPTION
  Registers a specified set of BT events to be provided to a specified
  BT Application when the events are routed specifically to the
  specified application (i.e., the BT application ID in the event
  header matches).

===========================================================================*/
bt_cmd_status_type bt_cmd_ec_register_event_set
(
  bt_app_id_type       bt_app_id,
  bt_ev_cb_func_type*  ev_cb_ptr,
  bt_event_set_type    ev_set_id,
  bt_event_type        min_event,
  bt_event_type        max_event
)
{

  bt_cmd_status_type  cmd_status;

  BT_MSG_API( "BT EC CMD RX: Reg AID %x ES %x",
              bt_app_id, ev_set_id, 0 );
  
  cmd_status = bt_ec_register_event_set(
                 bt_app_id, ev_cb_ptr, ev_set_id,
                 min_event, max_event, TRUE );
  
  return ( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_ec_reg_event_set_any_app_id

DESCRIPTION
  Registers a specified set of BT events to be provided to a specified
  BT Application no matter which BT Application to which the events are
  routed (i.e., the BT application ID in the event header does not
  need to match).

===========================================================================*/
bt_cmd_status_type bt_cmd_ec_reg_event_set_any_app_id
(
  bt_app_id_type       bt_app_id,
  bt_ev_cb_func_type*  ev_cb_ptr,
  bt_event_set_type    ev_set_id,
  bt_event_type        min_event,
  bt_event_type        max_event
)
{

  bt_cmd_status_type  cmd_status;

  BT_MSG_API( "BT EC CMD RX: Reg Any AID %x ES %x",
              bt_app_id, ev_set_id, 0 );
  
  cmd_status = bt_ec_register_event_set(
                 bt_app_id, ev_cb_ptr, ev_set_id,
                 min_event, max_event, FALSE );

  return ( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_ec_unregister_event_range

DESCRIPTION
  Removes an entry from the Event Control Table representing a specified
  range of BT events and associated application information.

===========================================================================*/
LOCAL void bt_ec_unregister_event_range
(
  bt_app_id_type  bt_app_id,
  bt_event_type   min_event,
  bt_event_type   max_event,
  boolean         app_id_must_match
)
{

  uint16  i = 0, j;

  rex_enter_crit_sect(&bt_ec_crit_sect);
  
  bt_app_id_table[ bt_app_id ].is_registered = FALSE;
  
  while ( i < bt_ect_entries )
  {
    if ( bt_ec_table[ i ].bt_app_id == bt_app_id )
    {
      if ( (bt_ec_table[ i ].min_event == min_event) &&
           (bt_ec_table[ i ].max_event == max_event) &&
           (bt_ec_table[ i ].app_id_must_match ==
              app_id_must_match) )
      {
        for ( j = i; j < (bt_ect_entries - 1); j++ )
        {
          bt_ec_table[ j ] = bt_ec_table[ (j + 1) ];
        }
        bt_ect_entries--;
        bt_ec_init_ec_table_entry( bt_ect_entries );
      }
      else
      {
        bt_app_id_table[ bt_app_id ].is_registered = TRUE;
        i++;
      }
    }
    else
    {
      i++;
    }
  }
  
  bt_ec_set_ect_min_max();

  rex_leave_crit_sect(&bt_ec_crit_sect);

}


/*===========================================================================

FUNCTION
  bt_ec_unregister_bt_app_id

DESCRIPTION
  Removes all registrations for a specified BT application ID.

===========================================================================*/
LOCAL void bt_ec_unregister_bt_app_id
(
  bt_app_id_type  bt_app_id
)
{

  uint16  i = 0, j;

  rex_enter_crit_sect(&bt_ec_crit_sect);

  if ( (bt_app_id_table[ bt_app_id ].ev_cb_ptr == NULL) &&
       (bt_app_id_table[ bt_app_id ].is_registered != FALSE) )
  {
    while ( i < bt_ect_entries )
    {
      if ( bt_ec_table[ i ].bt_app_id == bt_app_id )
      {
        for ( j = i; j < (bt_ect_entries - 1); j++ )
        {
          bt_ec_table[ j ] = bt_ec_table[ (j + 1) ];
        }
        bt_ect_entries--;
        bt_ec_init_ec_table_entry( bt_ect_entries );
      }
      else
      {
        i++;
      }
    }
    bt_ec_set_ect_min_max();
  }
  bt_app_id_table[ bt_app_id ].ev_cb_ptr = NULL;
  bt_app_id_table[ bt_app_id ].is_registered = FALSE;
  bt_app_id_table[ bt_app_id ].is_warned = FALSE;
  
  rex_leave_crit_sect(&bt_ec_crit_sect);

}


/*===========================================================================

FUNCTION
  bt_ec_unreg_using_event_set_id

DESCRIPTION
  Unregisters a specified set of BT events from being provided to a
  specified BT application using an event set identifier.

===========================================================================*/
LOCAL void bt_ec_unreg_using_event_set_id
(
  bt_app_id_type       bt_app_id,
  bt_event_set_type    ev_set_id,
  boolean              app_id_must_match
)
{

  uint16  i = 0;

  if ( ev_set_id == BT_EC_ES_ALL )
  {
    bt_ec_unregister_bt_app_id( bt_app_id );
  }
  else
  {
    while ( bt_ec_ev_set_table[ i ].min_event != BT_EV_NULL )
    {
      if ( bt_ec_ev_set_table[ i ].ev_set_id == ev_set_id )
      {
        bt_ec_unregister_event_range(
          bt_app_id,
          bt_ec_ev_set_table[ i ].min_event,
          bt_ec_ev_set_table[ i ].max_event,
          app_id_must_match );
      }
      i++;
    }
  }

}


/*===========================================================================

FUNCTION
  bt_ec_unregister_event_set

DESCRIPTION
  Unregisters a specified set of BT events from being provided
  to a specified BT application.  This routine is internal to
  EC to allow parameterization of whether the registration to
  be removed includes matching the BT application ID or not.

===========================================================================*/
LOCAL bt_cmd_status_type bt_ec_unregister_event_set
(
  bt_app_id_type     bt_app_id,
  bt_event_set_type  ev_set_id,
  bt_event_type      min_event,
  bt_event_type      max_event,
  boolean            app_id_must_match
)
{

  bt_cmd_status_type  cmd_status;

  cmd_status = bt_ec_verify_parameters( bt_app_id, ev_set_id,
                                        min_event, max_event );
  
  if ( cmd_status == BT_CS_GN_SUCCESS )
  {
    if ( ev_set_id != BT_EC_ES_CUSTOM )
    {
      bt_ec_unreg_using_event_set_id( bt_app_id, ev_set_id,
                                      app_id_must_match );
    }
    else
    {
      bt_ec_unregister_event_range( bt_app_id, min_event,
                                    max_event, app_id_must_match );
    }
  }

  return ( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_ec_unregister_event_set

DESCRIPTION
  Unregisters a specified set of BT events from being provided to a
  specified BT Application when the events are routed specifically to
  the specified application (i.e., the BT application ID in the event
  header must match).

===========================================================================*/
bt_cmd_status_type bt_cmd_ec_unregister_event_set
(
  bt_app_id_type     bt_app_id,
  bt_event_set_type  ev_set_id,
  bt_event_type      min_event,
  bt_event_type      max_event
)
{

  bt_cmd_status_type  cmd_status;

  BT_MSG_API( "BT EC CMD RX: Unreg AID %x ES %x",
              bt_app_id, ev_set_id, 0 );
  
  cmd_status = bt_ec_unregister_event_set( bt_app_id, ev_set_id,
                                           min_event, max_event,
                                           TRUE );

  return ( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_cmd_ec_unreg_event_set_any_app_id
  
DESCRIPTION
  Unregisters a specified set of BT events from being provided to a
  specified BT Application when the events are routed no matter which
  BT Application to which the events are routed (i.e., the BT
  application ID in the event header does not need to match).

===========================================================================*/
bt_cmd_status_type bt_cmd_ec_unreg_event_set_any_app_id
(
  bt_app_id_type     bt_app_id,
  bt_event_set_type  ev_set_id,
  bt_event_type      min_event,
  bt_event_type      max_event
)
{

  bt_cmd_status_type  cmd_status;

  BT_MSG_API( "BT EC CMD RX: Unreg Any AID %x ES %x",
              bt_app_id, ev_set_id, 0 );
  
  cmd_status = bt_ec_unregister_event_set( bt_app_id, ev_set_id,
                                           min_event, max_event,
                                           FALSE );

  return ( cmd_status );
  
}


/*===========================================================================

FUNCTION
  bt_cmd_ec_free_application_id

DESCRIPTION
  Frees a specified BT application ID and removes all entries in the EC
  table for the specified BT application ID.

===========================================================================*/
bt_cmd_status_type bt_cmd_ec_free_application_id
(
  bt_app_id_type  bt_app_id
)
{

  bt_cmd_status_type  cmd_status = BT_CS_GN_SUCCESS;

  BT_MSG_API( "BT EC CMD RX: Free AID %x", bt_app_id, 0, 0 );
  
  rex_enter_crit_sect(&bt_ec_crit_sect);
  if ( (bt_app_id < BT_EC_MAX_BT_APPS) &&
       (bt_app_id_table[ bt_app_id ].in_use != FALSE) )
  {
    bt_ec_unregister_bt_app_id( bt_app_id );

    bt_ec_init_app_id_table_entry( bt_app_id );
  }
  else
  {
    cmd_status = BT_CS_GN_BAD_APP_ID;
    BT_ERR( "BT EC: Bad App ID %x", bt_app_id, 0, 0 );
  }
  rex_leave_crit_sect(&bt_ec_crit_sect);

  return ( cmd_status );

}


/*===========================================================================

FUNCTION
  bt_ec_send_event

DESCRIPTION
  Delivers a specified event to all BT applications registered for it.

===========================================================================*/
void bt_ec_send_event
(
  struct bt_ev_msg_struct*  bt_ev_ptr
)
{

  uint16          i = 0;
  boolean         sent = FALSE;
  bt_app_id_type  bt_app_id;
  bt_event_type   ev_type;
  bt_app_id_table_type* table_entry_ptr = NULL;
  
  bt_app_id = bt_ev_ptr->ev_hdr.bt_app_id;
  ev_type   = bt_ev_ptr->ev_hdr.ev_type;

  rex_enter_crit_sect(&bt_ec_crit_sect);
  if ( (bt_app_id < ARR_SIZE(bt_app_id_table) ) &&
       (bt_app_id_table[ bt_app_id ].in_use != FALSE) &&
       (bt_app_id_table[ bt_app_id ].is_registered != FALSE) )
  {
    table_entry_ptr = &bt_app_id_table[ bt_app_id ];
  }
  if ( table_entry_ptr != NULL )
  {
    if ( table_entry_ptr->ev_cb_ptr != NULL )
    {
      bt_ev_ptr->ev_hdr.ev_cb_ptr = table_entry_ptr->ev_cb_ptr;
      
      (table_entry_ptr->ev_cb_ptr)( bt_ev_ptr );
      
      sent = TRUE;
    }
    
    if ( (ev_type >= bt_ect_min_event) &&
         (ev_type <= bt_ect_max_event) )
    {
      for ( i = 0; i < bt_ect_entries; i++ )
      {
        if ( (ev_type >= bt_ec_table[ i ].min_event) &&
             (ev_type <= bt_ec_table[ i ].max_event) &&
             ((bt_ec_table[ i ].bt_app_id == bt_app_id) ||
              (bt_ec_table[ i ].app_id_must_match == FALSE)) )
        {
          bt_ev_ptr->ev_hdr.ev_cb_ptr =
            bt_ec_table[ i ].ev_cb_ptr;
    
          (bt_ec_table[ i ].ev_cb_ptr)( bt_ev_ptr );
    
          sent = TRUE;
        }
      }
    }
  }
  rex_leave_crit_sect(&bt_ec_crit_sect);

  if ( sent != FALSE )
  {
    bt_ec_events_sent++;
  }
  else
  {
    if ( bt_ev_ptr->ev_hdr.ev_type == BT_EV_GN_CMD_DONE )
    {
      if ( bt_ev_ptr->ev_msg.ev_gn_cmd_done.cmd_status == BT_CS_GN_SUCCESS )
      {
        rex_enter_crit_sect(&bt_ec_crit_sect);
        if ( (table_entry_ptr != NULL) && 
             (table_entry_ptr->is_warned == FALSE) )
        {
          table_entry_ptr->is_warned = TRUE;
        }
        BT_MSG_DEBUG( "BT EC: CmdDn Not sent CT %x AID %x",
                      bt_ev_ptr->ev_msg.ev_gn_cmd_done.cmd_type,
                      bt_ev_ptr->ev_hdr.bt_app_id, 0 );
        rex_leave_crit_sect(&bt_ec_crit_sect);
      }
      else
      {
        BT_ERR( "BT EC: CmdDn NOT sent CS %x CT %x AID %x",
                bt_ev_ptr->ev_msg.ev_gn_cmd_done.cmd_status,
                bt_ev_ptr->ev_msg.ev_gn_cmd_done.cmd_type,
                bt_ev_ptr->ev_hdr.bt_app_id );
      }
    }
    else
    {
      BT_ERR( "BT EC: Ev not sent AID %x ET %x",
              bt_ev_ptr->ev_hdr.bt_app_id,
              bt_ev_ptr->ev_hdr.ev_type, 0 );
    }
  }

}

#endif /* FEATURE_BT */

