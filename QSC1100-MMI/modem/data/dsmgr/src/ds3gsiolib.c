/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 3 G   D A T A   S E R V I C E S   M A N A G E R

                  S I O   I N T E R F A C E

GENERAL DESCRIPTION
  This software unit contains wrapper functions for interfacing to the serial
  i/o driver, including routines to open/close a serial port, to change the
  serial mode and to set the states of serial port control signals.

EXTERNALIZED FUNCTIONS
  ds3g_siolib_open()
    Opens a specified serial port, in Autodetect mode

  ds3g_siolib_close()
    Closes the currently open serial port

  ds3g_siolib_change_mode()
    Sets the mode of the currently open serial port to the specified mode

  ds3g_siolib_cd_assert()
    Asserts the Carrier Detect (CD) signal

  ds3g_siolib_cd_deassert()
    De-asserts the Carrier Detect (CD) signal

  ds3g_siolib_ri_assert()
    Asserts the Ring Indicator (RI) signal

  ds3g_siolib_ri_deassert()
    De-asserts the Ring Indicator (RI) signal

  ds3g_siolib_set_inbound_flow()
    Used for flow control. Enables/disables inbound data flow from laptop

  ds3g_siolib_dsr_assert()
    Asserts the Data Set Ready (DSR) signal

  ds3g_siolib_dsr_deassert()
    De-asserts the Data Set Ready (DSR) signal

  ds3g_siolib_is_dtr_asserted()
    Returns the state of the Data Terminal Ready (DTR) signal

  ds3g_siolib_set_flow_ctl_method()
    Configures the flow control method for the currently open serial port

  ds3g_siolib_setup_tx()
    Queues data for serial transmission

  ds3g_siolib_change_baud_rate()
    Sets the baud rate of the currently open serial port to the specified
    value

  ds3g_siolib_set_cd_state()
    Arbitrator function for setting the Carrier Detect (CD) state

  ds3g_siolib_rearm_autodetect()
    Re-arms SIO autodetect mode

  ds3g_siolib_flush_tx()
    Schedules the SIO TX buffer to be flushed.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  All the serial port operations can be performed only after ds3g_siolib_open()
  is called at least once i.e. only if a serial port is currently open.


  Copyright (c) 2001-2008 by QUALCOMM Incorporated. All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3gsiolib.c_v   1.19   13 Feb 2003 17:17:20   trebman  $
  $Header: //source/qcom/qct/modem/data/common/dsmgr/main/lite/src/ds3gsiolib.c#3 $ $DateTime: 2008/04/11 05:12:17 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/08   sn      Fixed klockwork errors.
02/01/08   sn      Fixed lint errors.
10/18/07   sn      Changed DSM_SIO_MEM_LEVEL_FEW and _MANY to 
                   DSM_MEM_LEVEL_SIO_FEW and _MANY. Also modified the handling 
                   of 'pool_id' parameter.
01/22/04   atp     When ds3gsiolib_change_mode(AUTODETECT) is called, added
                   call to change atcop preprocessor state to command mode.
02/13/03   TMR     Commented out MSG_LOW from ds3g_siolib_set_inbound_flow()
01/17/02   sb      Reduced the priority of ERR message when rearming the Autodt
                   serial state. This has to do with fax switching between 
                   online-cmd/online-data modes during a fax call and should 
                   not be treated as error.
12/19/02   atp     Removed deprecated functions inbound_flow_enable/disable()
11/13/02   usb     Replaced DS_IS_FLOW_ENABLED with DS_FLOW_ALL_MASK
11/04/02   tmr     Change AT state to DS3G_SIOLIB_ONLINE_DATA_AT_STATE when
                   ds3g_siolib_change_mode() is called to change to any mode
                   except AUTODETECT.
10/15/02    ak     Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_DS_1X
10/13/02   ak      For JCDMA, use different times for RI intervals.
10/09/02   ak      When registering mem events with SIO, the many event
                   should be for FREEing an item.
09/26/02   tmr     Set call_answered to TRUE in ds3g_alert_user when 
                   auto_answer_handler is called
09/12/02   atp     Removed setting open_param fields in ds3gsiolib_ioctl().
07/11/02   ak      In setting FLOW_CONTROL, we had a MSG_HIGH.  Since this 
                   (seems) to be solid, make MSG_LOW.
07/01/02   ak      Minor fixes for first data call.
05/08/02   rsl     Added siolib related design changes.
03/05/02   smp     Added support for CD winking (&C2). Fixed bug due to which
                   Autodetect was not getting re-armed.
02/25/02   smp     In ds3g_siolib_setup_tx(), check if current serial mode is
                   Autodetect Mode, before re-arming Autodetect.
02/25/02   smp     Do not re-arm Autodetect if the current serial mode is not
                   Autodetect Mode.
02/19/02   rlp     Code review changes.
02/05/02   rlp     Added online-command mode support.
12/14/01   smp     Update ri_asserted state variable when RI signal is
                   asserted or de-asserted.
10/31/01   smp     Don't ERR_FATAL when sio_open() fails, just return a value
                   indicating that the port could not be opened. Also fixed
                   some compiler warnings.
10/04/01   smp     Added ds3g_siolib_is_rts_asserted() function.
08/28/01   smp     Fixed pointer issue in ds3g_siolib_is_dtr_asserted().
08/17/01   smp     Renamed ATCOP files/functions/data types.
06/07/01   smp     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

#include "assert.h"
#include "msg.h"
#include "err.h"
#include "rex.h"
#include "task.h"
#include "dstask.h"
#include "dsat.h"
#include "dsati.h"
#include "ds3gsiolib.h"
#include "ds3gmgrint.h"
#include "ds3gtimer.h"
#include "rdevmap.h"
#include "ds3grdmif.h"
#include "task.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
  The time intervals, in ms, for which the RI signal will be asserted and
  de-asserted when an incoming call is received.
---------------------------------------------------------------------------*/
#ifdef FEATURE_JCDMA_1X
  #define DS3G_SIOLIB_RI_ASSERT_INTERVAL      100
  #define DS3G_SIOLIB_RI_DEASSERT_INTERVAL   1900
#else
  #define DS3G_SIOLIB_RI_ASSERT_INTERVAL      100
  #define DS3G_SIOLIB_RI_DEASSERT_INTERVAL   5900
#endif /* FEATURE_JCDMA_1X */


/*---------------------------------------------------------------------------
  Variable to store the port id of the serial port currently assigned to data
  services. This is only valid if data services has a port open.
---------------------------------------------------------------------------*/
sio_port_id_type ds3g_siolib_port_id;

/*---------------------------------------------------------------------------
  Variable to store the stream id for the currently open stream, if any. If
  no stream is open, this should be set to SIO_NO_STREAM_ID.
---------------------------------------------------------------------------*/
sio_stream_id_type ds3g_siolib_stream_id = SIO_NO_STREAM_ID;

/*---------------------------------------------------------------------------
  The time interval in ms for which DCD is "winked" (i.e DCD is de-asserted)
  at the end of the call, if &C=2.
---------------------------------------------------------------------------*/
#define DS3G_SIOLIB_DCD_WINK_INTERVAL    1000

/*---------------------------------------------------------------------------
  Autodetect Watermark and Queue for storing data from/to SIO.
---------------------------------------------------------------------------*/
dsm_watermark_type  ds3g_from_sio_wm_item;
dsm_watermark_type  ds3g_to_sio_wm_item;
q_type              ds3g_from_sio_q;
q_type              ds3g_to_sio_q;

/*---------------------------------------------------------------------------
  Lo Water, Hi Water and Don't Exceed counts for the Autodetect watermark
  used for RX data bytes from SIO.
---------------------------------------------------------------------------*/
#define DS3G_SIOLIB_FROM_SIO_LOWATER_CNT              0
#define DS3G_SIOLIB_FROM_SIO_HIWATER_CNT             60
#define DS3G_SIOLIB_FROM_SIO_DONT_EXCEED_CNT        536

/*---------------------------------------------------------------------------
  Lo Water, Hi Water and Don't Exceed counts for the Autodetect watermark
  used for TX data bytes to SIO.  Don't exceed count size is set based on
  the largest AT command response that can be queued up to send to the TE.
  This is six DSM large item data payloads, plus one extra for safety
  margin.
---------------------------------------------------------------------------*/
#define DS3G_SIOLIB_TO_SIO_LOWATER_CNT                0
#define DS3G_SIOLIB_TO_SIO_HIWATER_CNT               60
#define DS3G_SIOLIB_TO_SIO_DONT_EXCEED_CNT      (7*536)

ds3g_siolib_state_info_type ds3g_siolib_state;

/*---------------------------------------------------------------------------
  Ring timer used when ringing the serial port for an incoming call.
---------------------------------------------------------------------------*/
rex_timer_type  ds3g_siolib_ring_timer;

/*---------------------------------------------------------------------------
  Timer used to wink Carrier Detect at the end of a call.
---------------------------------------------------------------------------*/
rex_timer_type  ds3g_siolib_dcd_wink_timer;


/*===========================================================================

                      INTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================
FUNCTION      DS3G_SIOLIB_TIMER_CB

DESCRIPTION
    Callback for Wink DCD and RI timers.

DEPENDENCIES
    None

RETURN VALUE
    None

SIDE EFFECTS
    None
===========================================================================*/

void ds3g_siolib_timer_cb
(
  unsigned long    timer_id
)
{
  ds_cmd_type           *cmd_ptr;                    /* Pointer to command */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( ( cmd_ptr = ds_get_cmd_buf() ) == NULL )
  {
    ERR_FATAL( "Can't get cmd buf from DS task", 0, 0, 0 );
  }

  switch ((ds3g_timer_enum_type)timer_id)
  {
    case DS3G_TIMER_RING_TE:
      cmd_ptr->hdr.cmd_id = DS_TIMER_EXPIRED_CMD;
      cmd_ptr->cmd.timer.timer_id = DS3G_TIMER_RING_TE;
      break;

    case DS3G_TIMER_WINK_DCD:
      cmd_ptr->hdr.cmd_id = DS_TIMER_EXPIRED_CMD;
      cmd_ptr->cmd.timer.timer_id = DS3G_TIMER_WINK_DCD;
      break;

    default:
      ASSERT(0);
      return;
  }
  ds_put_cmd( cmd_ptr );
} /* ds3g_siolib_timer_cb */

/*===========================================================================

FUNCTION DS3G_SIOLIB_WM_NON_EMPTY_CB

DESCRIPTION
  Callback function invoked when the Autodetect watermark item goes non-empty.

  This function simply sets the DS_1ST_SIO_RX_SIG.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3g_siolib_wm_non_empty_cb( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  rex_set_sigs( &ds_tcb, DS_1ST_SIO_RX_SIG );

} /* ds3g_siolib_wm_non_empty_cb() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_SETUP_WATERMARKS

DESCRIPTION
  This function initializes the watermarks used for RX and TX data
  from/to SIO. The lowater, hiwater and don't exceed counts are configured,
  and the watermark count (amount of data stored in the watermark) is set to
  zero.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3g_siolib_setup_watermarks( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Set the Lo Water, Hi Water and Don't Exceed counts for the RX data
    watermark.
  -------------------------------------------------------------------------*/
  ds3g_siolib_state.serial_info.rx_wm_ptr->lo_watermark =
                                           DS3G_SIOLIB_FROM_SIO_LOWATER_CNT;
  ds3g_siolib_state.serial_info.rx_wm_ptr->hi_watermark =
                                           DS3G_SIOLIB_FROM_SIO_HIWATER_CNT;
  ds3g_siolib_state.serial_info.rx_wm_ptr->dont_exceed_cnt =
                                       DS3G_SIOLIB_FROM_SIO_DONT_EXCEED_CNT;

  /*-------------------------------------------------------------------------
    Initialize the watermark internal byte counts for the RX data watermark.
  -------------------------------------------------------------------------*/
  ds3g_siolib_state.serial_info.rx_wm_ptr->current_cnt = 0;
#ifdef FEATURE_DSM_MEM_CHK
  ds3g_siolib_state.serial_info.rx_wm_ptr->highest_cnt = 0;
  ds3g_siolib_state.serial_info.rx_wm_ptr->total_rcvd_cnt = 0;
#endif

  /*-------------------------------------------------------------------------
    Register callback functions for the RX data watermark. The only event of
    interest is the non-empty event.
  -------------------------------------------------------------------------*/
  ds3g_siolib_state.serial_info.rx_wm_ptr->lowater_func_ptr      = NULL;
  ds3g_siolib_state.serial_info.rx_wm_ptr->hiwater_func_ptr      = NULL;
  ds3g_siolib_state.serial_info.rx_wm_ptr->each_enqueue_func_ptr = NULL;
  ds3g_siolib_state.serial_info.rx_wm_ptr->gone_empty_func_ptr   = NULL;
  ds3g_siolib_state.serial_info.rx_wm_ptr->non_empty_func_ptr    =
                                                 ds3g_siolib_wm_non_empty_cb;

  /*-------------------------------------------------------------------------
    Initialize and register the associated queue for storing rx'ed data.
  -------------------------------------------------------------------------*/
  q_init( &ds3g_from_sio_q );
  ds3g_siolib_state.serial_info.rx_wm_ptr->q_ptr = &ds3g_from_sio_q;

/*-------------------------------------------------------------------------
    Set the Lo Water, Hi Water and Don't Exceed counts for the TX data
    watermark.
  -------------------------------------------------------------------------*/
  ds3g_siolib_state.serial_info.tx_wm_ptr->lo_watermark =
                                             DS3G_SIOLIB_TO_SIO_LOWATER_CNT;
  ds3g_siolib_state.serial_info.tx_wm_ptr->hi_watermark =
                                             DS3G_SIOLIB_TO_SIO_HIWATER_CNT;
  ds3g_siolib_state.serial_info.tx_wm_ptr->dont_exceed_cnt =
                                         DS3G_SIOLIB_TO_SIO_DONT_EXCEED_CNT;

  /*-------------------------------------------------------------------------
    Initialize the watermark internal byte counts for the TX data watermark.
  -------------------------------------------------------------------------*/
  ds3g_siolib_state.serial_info.tx_wm_ptr->current_cnt = 0;
#ifdef FEATURE_DSM_MEM_CHK
  ds3g_siolib_state.serial_info.tx_wm_ptr->highest_cnt = 0;
  ds3g_siolib_state.serial_info.tx_wm_ptr->total_rcvd_cnt = 0;
#endif

  /*-------------------------------------------------------------------------
    No callback functions are registered for the TX data watermark.
  -------------------------------------------------------------------------*/
  ds3g_siolib_state.serial_info.tx_wm_ptr->lowater_func_ptr = NULL;
  ds3g_siolib_state.serial_info.tx_wm_ptr->gone_empty_func_ptr = NULL;
  ds3g_siolib_state.serial_info.tx_wm_ptr->non_empty_func_ptr = NULL;
  ds3g_siolib_state.serial_info.tx_wm_ptr->hiwater_func_ptr = NULL;

  /*-------------------------------------------------------------------------
    Initialize and register the associated queue for storing data to TX.
  -------------------------------------------------------------------------*/
  q_init( &ds3g_to_sio_q );
  ds3g_siolib_state.serial_info.tx_wm_ptr->q_ptr = &ds3g_to_sio_q;

} /* ds3g_siolib_setup_watermarks() */

/*===========================================================================
FUNCTION DS3G_SIOLIB_MEM_EVENT_CTRL

DESCRIPTION
  This function will call the appropriate memory event actions for the Data
  Services application.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void ds3g_siolib_mem_event_ctrl
(
  dsm_mempool_id_enum_type pool_id,                      /* memory pool ID */
  dsm_mem_level_enum_type event,               /* type of memory event */
  dsm_mem_op_enum_type op
)
{
  if(pool_id == DSM_DS_SMALL_ITEM_POOL)
  {
      switch (event)
      {
        case DSM_MEM_LEVEL_SIO_MANY:
          /*-----------------------------------------------------------------
            Do buffer based flow control. If it seems that we are recovering
            from a lack of items, enable inbound flow again.
           ----------------------------------------------------------------*/
          MSG_LOW(" SMALL ITEM MANY ITEMS MARK Reached",0,0,0);
          ds3g_siolib_set_inbound_flow(DS_FLOW_DSM_SMALL_Q_MASK,
                                       DS_FLOW_ENABLE);
          break;

        case DSM_MEM_LEVEL_SIO_FEW:
          /*-----------------------------------------------------------------
            Deassert flow control on the Um and the Rm interfaces
            ---------------------------------------------------------------*/
          MSG_LOW(" SMALL ITEM FEW ITEMS MARK Reached",0,0,0);
          ds3g_siolib_set_inbound_flow(DS_FLOW_DSM_SMALL_Q_MASK,
                                       DS_FLOW_DISABLE);
          break;

        case DSM_MEM_LEVEL_DNE:
          MSG_LOW(" SMALL ITEM DO NOT EXCEED Reached",0,0,0);
          break;

        default:
          MSG_LOW("Do nothing",0,0,0);
          break;
      }
  }
  else if(pool_id == DSM_DS_LARGE_ITEM_POOL)
  {
      switch (event)
      {
        case DSM_MEM_LEVEL_SIO_MANY:
          /*-----------------------------------------------------------------
            Do buffer based flow control. If it seems that we are recovering
            from a lack of items, enable inbound flow again.
           ----------------------------------------------------------------*/
          MSG_LOW(" LARGE ITEM MANY ITEMS MARK Reached",0,0,0);
          ds3g_siolib_set_inbound_flow(DS_FLOW_DSM_LARGE_Q_MASK,
                                       DS_FLOW_ENABLE);
          break;

        case DSM_MEM_LEVEL_SIO_FEW:
          /*-----------------------------------------------------------------
            Deassert flow control on the Um and the Rm interfaces
            ---------------------------------------------------------------*/
          MSG_LOW(" LARGE ITEM FEW ITEMS MARK Reached",0,0,0);
          ds3g_siolib_set_inbound_flow(DS_FLOW_DSM_LARGE_Q_MASK,
                                       DS_FLOW_DISABLE);
          break;

        case DSM_MEM_LEVEL_DNE:
          MSG_LOW(" LARGE ITEM DO NOT EXCEED Reached",0,0,0);
          break;

        default:
          MSG_LOW("Do nothing",0,0,0);
          break;
      }
  }
  else 
  {
    MSG_LOW("Do nothing", 0, 0, 0);
  }
} /* ds3g_siolib_mem_event_ctrl */

/*===========================================================================
FUNCTION DS3G_SIOLIB_REGISTER_MEM_LEVEL_CALLBACKS

DESCRIPTION
  This function will register additional memory event callbacks for the
  Data Services application.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
LOCAL void ds3g_siolib_register_mem_event_callbacks( void )
{
  /*-------------------------------------------------------------------------
    Register all DSM_DS_SMALL_ITEM_POOL related events.
    -----------------------------------------------------------------------*/
  (void) dsm_reg_mem_event_cb ( DSM_DS_SMALL_ITEM_POOL,
                                DSM_MEM_LEVEL_SIO_MANY,
                                DSM_MEM_OP_FREE,
                                ds3g_siolib_mem_event_ctrl );

  (void) dsm_reg_mem_event_cb ( DSM_DS_SMALL_ITEM_POOL,
                                DSM_MEM_LEVEL_SIO_FEW,
                                DSM_MEM_OP_NEW,
                                ds3g_siolib_mem_event_ctrl );

  (void) dsm_reg_mem_event_cb ( DSM_DS_SMALL_ITEM_POOL,
                                DSM_MEM_LEVEL_DNE,
                                DSM_MEM_OP_NEW,
                                ds3g_siolib_mem_event_ctrl );

  /*-------------------------------------------------------------------------
    Register all DSM_DS_LARGE_ITEM_POOL related events.
    -----------------------------------------------------------------------*/
  (void) dsm_reg_mem_event_cb ( DSM_DS_LARGE_ITEM_POOL,
                                DSM_MEM_LEVEL_SIO_MANY,
                                DSM_MEM_OP_FREE,
                                ds3g_siolib_mem_event_ctrl );

  (void) dsm_reg_mem_event_cb ( DSM_DS_LARGE_ITEM_POOL,
                                DSM_MEM_LEVEL_SIO_FEW,
                                DSM_MEM_OP_NEW,
                                ds3g_siolib_mem_event_ctrl );

  (void) dsm_reg_mem_event_cb ( DSM_DS_LARGE_ITEM_POOL,
                                DSM_MEM_LEVEL_DNE,
                                DSM_MEM_OP_NEW,
                                ds3g_siolib_mem_event_ctrl );

} /* ds3g_siolib_register_mem_event_callbacks() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_REGISTER_CALLBACK_FUNC

DESCRIPTION
  This function registers a mode specific call back function table.

  Caller should check the return value of this function to see if the
  registration succeeded.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if a func_tbl is not registered with SIOLIB already and
  the function pointer passed in is not NULL, otherwise returns FALSE;

SIDE EFFECTS
  None
===========================================================================*/

boolean  ds3g_siolib_register_callback_func
(
  ds3g_siolib_func_tbl_type  *func_tbl_ptr        /* Handler function table */
)
{
  boolean result = FALSE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
  Check to see if the func. ptr. thats being passed in is not null and that
  a func. callback table is not registered with SIOLIB already.
  -------------------------------------------------------------------------*/
  if ((func_tbl_ptr != NULL) && (ds3g_siolib_state.func_tbl == NULL))
  {
    ds3g_siolib_state.func_tbl = func_tbl_ptr;
    /*-----------------------------------------------------------------------
    This is to let ATCOP know that SIO port is now busy
    -----------------------------------------------------------------------*/
    dsat_sio_is_busy();
    result = TRUE;
  }
  else if (func_tbl_ptr == NULL)
  {
    ERR_FATAL ("Callback func. ptr = NULL in SIOLIB", 0, 0, 0);
  }
  else if (ds3g_siolib_state.func_tbl != NULL)
  {
    ERR ("Call back func_tbl already registered with SIOLIB", 0, 0, 0);
  }
  return result;
} /* ds3g_siolib_register_callback_func() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_DEREGISTER_CALLBACK_FUNC

DESCRIPTION
  This function deregisters a mode specific call back function table and
  resets to NULL.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if deregister happened successfully.
  Returns FALSE if func ptr passed in does not match siolib table value.

SIDE EFFECTS
  None
===========================================================================*/

boolean  ds3g_siolib_deregister_callback_func
(
  ds3g_siolib_func_tbl_type  *func_tbl_ptr
)
{
  boolean result = TRUE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
  Check to see if the func. ptr. that was registered before is the one
  trying to deregister.
  -------------------------------------------------------------------------*/
  if (ds3g_siolib_state.func_tbl != func_tbl_ptr)
  {
    result = FALSE;
    ERR ("Callback func. ptr doesn't match SIOLIB func_tbl value", 0, 0, 0);
  }
  else
  {
    ds3g_siolib_state.func_tbl = NULL;
    /*-----------------------------------------------------------------------
    This is to let ATCOP know that SIO port is now free for use
    -----------------------------------------------------------------------*/
    dsat_sio_is_free();
  }
  return result;
} /* ds3g_siolib_register_callback_func() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_SET_CALL_COMING_UP

DESCRIPTION
  This function sets the call coming up boolean variable which determines
  if a data call is in the process of coming up.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void  ds3g_siolib_set_call_coming_up
(
    boolean busy
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
    ds3g_siolib_state.call_coming_up = busy;
} /* ds3g_siolib_set_call_coming_up() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_DTR_CHANGED_CB

DESCRIPTION
  Callback function registered with SIO, used to inform DS of any changes in
  the state of the DTR signal.

  This function simply sets the DS_DTR_CHANGED_SIG.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_siolib_dtr_changed_cb( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  (void)rex_set_sigs( &ds_tcb, DS_DTR_CHANGED_SIG );

} /* ds3g_siolib_dtr_changed_cb() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_PACKET_DETECTED_CB

DESCRIPTION
  Callback function registered with SIO, used to notify DS when a PPP packet
  is received (in Autodetect Mode).

  This function simply sets the DS_SIO_RX_PKT_SIG.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3g_siolib_packet_detected_cb( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  (void)rex_set_sigs( &ds_tcb, DS_SIO_RX_PKT_SIG );

} /* ds3g_siolib_packet_detected_cb() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_ESCAPE_DETECTED_CB

DESCRIPTION
  Callback function registered with SIO, used to notify data services when
  the escape code (+++) is detected (in Rawdata Mode).

  This function simply sets the DS_SIO_ESCAPE_SIG.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3g_siolib_escape_detected_cb( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  (void)rex_set_sigs( &ds_tcb, DS_SIO_ESCAPE_SIG );

} /* ds3g_siolib_escape_detected_cb() */


/*===========================================================================

                      EXTERNAL FUNCTION DEFINITIONS

===========================================================================*/

/*===========================================================================

FUNCTION DS3G_SIOLIB_INIT

DESCRIPTION
  This function performs power-up initialization of SIOLIB.  This includes
  initialization of some state variables and watermark pointers.

  This function must be called once at data services task startup.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

rex_sigs_type ds3g_siolib_init ( void )
{
  rex_sigs_type  signal_mask;                    /* Signal mask to wait on */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Initialize SIOLIB state information.
  -------------------------------------------------------------------------*/
  ds3g_siolib_state.serial_info.serial_state = DS3G_SIOLIB_NULL_SERIAL_STATE;
  ds3g_siolib_state.at_state = DS3G_SIOLIB_NULL_AT_STATE;
  ds3g_siolib_state.call_coming_up = FALSE;
  ds3g_siolib_state.func_tbl = NULL;

  /*-------------------------------------------------------------------------
    Setup the Autodetect watermark items.
  -------------------------------------------------------------------------*/
  ds3g_siolib_state.serial_info.rx_wm_ptr = &ds3g_from_sio_wm_item;
  ds3g_siolib_state.serial_info.tx_wm_ptr = &ds3g_to_sio_wm_item;
  ds3g_siolib_setup_watermarks();

  /*-------------------------------------------------------------------------
    Register open and close routines with RDM. These are called by RDM to
    tell SIOLIB to open or close a serial device, respectively.
  -------------------------------------------------------------------------*/
  rdm_register_open_func( RDM_DATA_SRVC, ds3gi_rdm_open_cb );
  rdm_register_close_func( RDM_DATA_SRVC, ds3gi_rdm_close_cb );

  /*-------------------------------------------------------------------------
    Create the signal mask containing the SIOLIB sub-task signals.
  -------------------------------------------------------------------------*/
  signal_mask = DS_1ST_SIO_RX_SIG | DS_SIO_RX_PKT_SIG | DS_SIO_ESCAPE_SIG |
                DS_DTR_CHANGED_SIG;

  /*-------------------------------------------------------------------------
    Register Wink DCD and RI timer expiration callbacks.
  -------------------------------------------------------------------------*/
  ds3gi_timer_register ( &ds3g_siolib_dcd_wink_timer,
                         ds3g_siolib_timer_cb,
                         DS3G_TIMER_WINK_DCD );

  ds3gi_timer_register ( &ds3g_siolib_ring_timer,
                         ds3g_siolib_timer_cb,
                         DS3G_TIMER_RING_TE );

  /*-------------------------------------------------------------------------
  Register additional event functions specific for Data Services
  application.
  -------------------------------------------------------------------------*/
  ds3g_siolib_register_mem_event_callbacks();

  ds3g_siolib_state.ring_counter = 0;
  
  return( signal_mask );
}


/*===========================================================================

FUNCTION DS3G_SIOLIB_OPEN

DESCRIPTION
  This function opens the specified serial port. Received data bytes will be
  stored in the RX watermark specified by the caller. Data bytes pending
  transmission will be stored in the TX watermark specified by the caller.

  The port is opened in Autodetect mode, and the baud rate is set according
  to the +IPR setting. Callback functions for DTR state changes and packet
  detection are registered with SIO. RS-232 control signals are initialized
  as follows:
    CD  - Initialized per the &C setting
    CTS - Asserted
    RI  - De-asserted
    DSR - Asserted

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_open
(
  sio_port_id_type      port_id,       /* Port id of the port to be opened */
  dsm_watermark_type   *rx_wm_ptr,       /* Watermark for rx data from SIO */
  dsm_watermark_type   *tx_wm_ptr          /* Watermark for tx data to SIO */
)
{
  sio_open_type           open_param;
  sio_ioctl_param_type    ioctl_param;
  ds3g_siolib_status_e_type  ret_val = DS3G_SIOLIB_OK;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  ds3g_siolib_port_id = port_id;

  /*-------------------------------------------------------------------------
    Set the port id and rx/tx watermarks to the values provided by the caller
    of this function.
  -------------------------------------------------------------------------*/
  open_param.port_id = port_id;
  open_param.rx_queue = rx_wm_ptr;
  open_param.tx_queue = tx_wm_ptr;

  /*-------------------------------------------------------------------------
    Set the remaining parameters to the appropriate values. A serial port is
    always opened in Autodetect Mode. The baud rate is set according to the
    +IPR setting.
  -------------------------------------------------------------------------*/
  open_param.stream_mode = SIO_DS_AUTODETECT_MODE;
  open_param.rx_func_ptr = NULL;
  open_param.rx_bitrate = (sio_bitrate_type)dsat_ipr_val;
  open_param.tx_bitrate = (sio_bitrate_type)dsat_ipr_val;
  open_param.tail_char_used = FALSE;
  open_param.rx_flow = SIO_FCTL_BEST;
  open_param.tx_flow = SIO_FCTL_BEST;

  /*-------------------------------------------------------------------------
    Open the serial port, and store the assigned stream id. If the open was
    unsuccessful, log an error.
  -------------------------------------------------------------------------*/
  if( (ds3g_siolib_stream_id = sio_open( &open_param )) == SIO_NO_STREAM_ID )
  {
    ERR( "Data Services couldn't open SIO stream", 0, 0, 0 );
    ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  }
  else
  {
     ds3g_siolib_state.serial_info.serial_state =
                                         DS3G_SIOLIB_AUTODETECT_SERIAL_STATE;
    /*-----------------------------------------------------------------------
      Register callback functions for serial port events of interest, namely:
      changes in DTR state and packet detection.
    -----------------------------------------------------------------------*/
    ioctl_param.enable_dte_ready_event = ds3g_siolib_dtr_changed_cb;
    sio_ioctl( ds3g_siolib_stream_id,
               SIO_IOCTL_ENABLE_DTR_EVENT,
               &ioctl_param );

    /*-----------------------------------------------------------------------
      Initialize the states of MT->TE serial port control signals.
    -----------------------------------------------------------------------*/
    (void)ds3g_siolib_set_cd_state( DS3G_SIOLIB_EVENT_NONE );
    (void)ds3g_siolib_set_inbound_flow ( DS_FLOW_ALL_MASK,
                                         DS_FLOW_ENABLE );
    (void)ds3g_siolib_ri_deassert();
    (void)ds3g_siolib_dsr_assert();

  }

  return( ret_val );

} /* ds3g_siolib_open() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_CLOSE

DESCRIPTION
  This function closes the currently open serial port.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3g_siolib_close( void )
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( ds3g_siolib_stream_id != SIO_NO_STREAM_ID )
  {
    sio_close( ds3g_siolib_stream_id, NULL );
    ds3g_siolib_stream_id = SIO_NO_STREAM_ID;

    /* Do we need to disable the DTR event and/or reset RS-232 control
       signals here? */
  }

} /* ds3g_siolib_close() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_IOCTL

DESCRIPTION
  This function allows control of open stream.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3g_siolib_ioctl
(
  sio_ioctl_cmd_type cmd,
  sio_ioctl_param_type *param
)
{
  
  if( ds3g_siolib_stream_id != SIO_NO_STREAM_ID )
  {
    sio_ioctl ( ds3g_siolib_stream_id, cmd, param );
  }
  else
  {
    MSG_HIGH ( "siolib_stream_id = SIO_NO_STREAM_ID", 0, 0, 0 );
  }
}

/*===========================================================================

FUNCTION DS3G_SIOLIB_IS_BAUD_VALID

DESCRIPTION
  This routine checks if the specified baud rate is valid under the
  current feature set.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the baud rate is valid under the current feature set.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean ds3g_siolib_is_baud_valid
(
  sio_bitrate_type   check_baud              /* SIO baud rate */
)
{
  return(sio_is_baud_valid ( check_baud ));
}

/*===========================================================================

FUNCTION DS3G_SIOLIB_CHANGE_MODE
DESCRIPTION
  This function changes the mode of the currently open serial port to the
  specified mode. After the mode change, received data bytes will be stored
  in the SIO RX watermark , and the rx_func will be called each time data is
  enqueued to the watermark.

  Depending on the new serial mode, callback functions are registered for
  serial events of interest, namely:
    Autodetect Mode - Packet detection callback
    Rawdata Mode    - Escape signal detection callback
    Packet Mode     - None

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_change_mode
(
    sio_stream_mode_type  stream_mode,                    /* New stream mode */
    dsm_watermark_type   *rx_wm_ptr,       /* Watermark for rx data from SIO */
    dsm_watermark_type   *tx_wm_ptr,         /* Watermark for tx data to SIO */
    sio_rx_func_ptr_type  rx_func_ptr   /* Called each time data bytes rx'ed */
)
{
  sio_open_type             open_param;
  sio_ioctl_param_type      ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_OK;
  dsm_item_type*            item_ptr;                /* item to be removed */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( ds3g_siolib_stream_id != SIO_NO_STREAM_ID )
  {
    /*-----------------------------------------------------------------------
      Set the stream mode, rx/tx watermarks and rx function to the values
      provided by the caller of this function.
    -----------------------------------------------------------------------*/
    open_param.stream_mode = stream_mode;

    /*-----------------------------------------------------------------------
    In the case of autodetect mode, SIOLIB maintains the watermarks, so
    the values passed in will be null, hence set them to predefined 
    watermarks.
    -----------------------------------------------------------------------*/
    if ( stream_mode == SIO_DS_AUTODETECT_MODE ) 
    {
      if ( ( rx_wm_ptr == NULL ) && ( tx_wm_ptr == NULL ) )
      {
        open_param.rx_queue = &ds3g_from_sio_wm_item;
        open_param.tx_queue = &ds3g_to_sio_wm_item;
        open_param.rx_func_ptr = NULL;
        ds3g_siolib_setup_watermarks();
        ds3g_siolib_state.at_state = DS3G_SIOLIB_COMMAND_AT_STATE;
      }
      else
      {
        ERR_FATAL ( "WM pointers not null in autodetect mode ", 0, 0, 0 );
      }
    }
    else
    {
      /*-----------------------------------------------------------------------
      Empty autodetect watermarks and queues when switching to any mode
      other than autodetect.
      -----------------------------------------------------------------------*/
      ds3g_siolib_state.at_state = DS3G_SIOLIB_ONLINE_DATA_AT_STATE;
      dsm_empty_queue ( &ds3g_from_sio_wm_item );
      dsm_empty_queue ( &ds3g_to_sio_wm_item );
      if ( (item_ptr = (dsm_item_type *)q_last_get
            ( &ds3g_from_sio_q)) != NULL )
      {
        dsm_free_packet( &item_ptr);
      }
      if ( (item_ptr = (dsm_item_type *)q_last_get
            ( &ds3g_to_sio_q)) != NULL )
      {
        dsm_free_packet( &item_ptr);
      }
      
      open_param.rx_queue = rx_wm_ptr;
      open_param.tx_queue = tx_wm_ptr;
      open_param.rx_func_ptr = rx_func_ptr;
    }

    /*-----------------------------------------------------------------------
      Set the port id to the currently assigned value.
    -----------------------------------------------------------------------*/
    open_param.port_id = ds3g_siolib_port_id;

    /*-----------------------------------------------------------------------
      Set the remaining parameters to the appropriate values.
    -----------------------------------------------------------------------*/
    open_param.rx_bitrate = SIO_BITRATE_BEST;
    open_param.tx_bitrate = SIO_BITRATE_BEST;
    open_param.tail_char_used = FALSE;
    open_param.rx_flow = SIO_FCTL_BEST;
    open_param.tx_flow = SIO_FCTL_BEST;

    /*-----------------------------------------------------------------------
      Change the serial mode to the specified mode.
    -----------------------------------------------------------------------*/
    ioctl_param.open_ptr = &open_param;
    sio_ioctl( ds3g_siolib_stream_id, SIO_IOCTL_CHANGE_MODE, &ioctl_param );

    /*-----------------------------------------------------------------------
      Now do any additional configuration based on the mode.
    -----------------------------------------------------------------------*/
    switch( stream_mode )
    {
      case SIO_DS_AUTODETECT_MODE:
        MSG_HIGH("DS SIO = AUTODETECT",0,0,0);
        dsatprep_set_preprocessor_mode( DSATI_AT_CMD_MODE );
        break;

      case SIO_DS_RAWDATA_MODE:
        /*-------------------------------------------------------------------
          Register callback function for Escape sequence (+++) detection.
        -------------------------------------------------------------------*/
        MSG_HIGH("DS SIO = RAWDATA",0,0,0);
        ioctl_param.record_escape_func_ptr = ds3g_siolib_escape_detected_cb;
        sio_ioctl( ds3g_siolib_stream_id, SIO_IOCTL_RECORD_ESC_FUNC_PTR,
                   &ioctl_param );
        break;

      case SIO_DS_PKT_MODE:
          MSG_HIGH("DS SIO = PACKET",0,0,0);
          break;

      default:
          ERR( "Serial mode set to invalid mode: %d", stream_mode, 0, 0 );
          break;
    }

    ds3g_siolib_state.serial_info.serial_state =
                                      ds3gi_convert_serial_mode(stream_mode);

  } /* if stream_id != NO_STREAM_ID */
  else
  {
    ERR("Changing mode w/bad stream id %d",
        ds3g_siolib_stream_id,0,0);
    ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  }

  return( ret_val );

} /* ds3g_siolib_change_mode() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_CD_ASSERT

DESCRIPTION
  Asserts the Carrier Detect (CD) signal.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_cd_assert( void )
{
  sio_ioctl_param_type      ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_OK;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( ds3g_siolib_stream_id != SIO_NO_STREAM_ID )
  {
    sio_ioctl( ds3g_siolib_stream_id, SIO_IOCTL_CD_ASSERT, &ioctl_param );
  }
  else
  {
    ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  }

  return( ret_val );

} /* ds3g_siolib_cd_assert() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_CD_DEASSERT

DESCRIPTION
  De-asserts the Carrier Detect (CD) signal.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_cd_deassert( void )
{
  sio_ioctl_param_type      ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_OK;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( ds3g_siolib_stream_id != SIO_NO_STREAM_ID )
  {
    sio_ioctl( ds3g_siolib_stream_id, SIO_IOCTL_CD_DEASSERT, &ioctl_param );
  }
  else
  {
    ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  }

  return( ret_val );

} /* ds3g_siolib_cd_deassert() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_RI_ASSERT

DESCRIPTION
  Asserts the Ring Indicator (RI) signal.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_ri_assert( void )
{
  sio_ioctl_param_type      ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_OK;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( ds3g_siolib_stream_id != SIO_NO_STREAM_ID )
  {
    sio_ioctl( ds3g_siolib_stream_id, SIO_IOCTL_RI_ASSERT, &ioctl_param );
    ds3g_siolib_state.serial_info.ri_asserted = TRUE;
  }
  else
  {
    ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  }

  return( ret_val );

} /* ds3g_siolib_ri_assert() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_RI_DEASSERT

DESCRIPTION
  De-asserts the Ring Indicator (RI) signal.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_ri_deassert( void )
{
  sio_ioctl_param_type      ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_OK;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( ds3g_siolib_stream_id != SIO_NO_STREAM_ID )
  {
    sio_ioctl( ds3g_siolib_stream_id, SIO_IOCTL_RI_DEASSERT, &ioctl_param );
    ds3g_siolib_state.serial_info.ri_asserted = FALSE;
  }
  else
  {
    ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  }

  return( ret_val );

} /* ds3g_siolib_ri_deassert() */



/*===========================================================================

FUNCTION DS3G_SIOLIB_DSR_ASSERT

DESCRIPTION
  Asserts the Data Set Ready (DSR) signal.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_dsr_assert( void )
{
  sio_ioctl_param_type      ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_OK;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( ds3g_siolib_stream_id != SIO_NO_STREAM_ID )
  {
    sio_ioctl( ds3g_siolib_stream_id, SIO_IOCTL_DSR_ASSERT, &ioctl_param );
  }
  else
  {
    ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  }

  return( ret_val );

} /* ds3g_siolib_dsr_assert() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_DSR_DEASSERT

DESCRIPTION
  De-asserts the Data Set Ready (DSR) signal.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_dsr_deassert( void )
{
  sio_ioctl_param_type      ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_OK;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( ds3g_siolib_stream_id != SIO_NO_STREAM_ID )
  {
    sio_ioctl( ds3g_siolib_stream_id, SIO_IOCTL_DSR_DEASSERT, &ioctl_param );
  }
  else
  {
    ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  }

  return( ret_val );

} /* ds3g_siolib_dsr_assert() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_IS_DTR_ASSERTED

DESCRIPTION
  Checks the state of the Data Terminal Ready (DTR) signal, and returns a
  value (by means of a passed-by-reference parameter) indicating whether or
  not DTR is asserted.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_is_dtr_asserted
(
  boolean  *dtr_status     /* Set to TRUE if DTR asserted, or FALSE if not */
)
{
  sio_ioctl_param_type      ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_OK;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( ds3g_siolib_stream_id != SIO_NO_STREAM_ID )
  {
    ioctl_param.dte_ready_asserted = dtr_status;

    sio_ioctl( ds3g_siolib_stream_id, SIO_IOCTL_DTE_READY_ASSERTED,
           &ioctl_param );
  }
  else
  {
    ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  }

  return( ret_val );

} /* ds3g_siolib_is_dtr_asserted() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_IS_RTS_ASSERTED

DESCRIPTION
  Checks the state of the Request To Send (RTS) signal, and returns a
  value (by means of a passed-by-reference parameter) indicating whether or
  not RTS is asserted.

DEPENDENCIES
  If there is no port currently open, the rts_status value has no meaning.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_is_rts_asserted
(
  boolean  *rts_status     /* Set to TRUE if RTS asserted, or FALSE if not */
)
{
  sio_ioctl_param_type      ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_OK;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( ds3g_siolib_stream_id != SIO_NO_STREAM_ID )
  {
    ioctl_param.rts_asserted = rts_status;

    sio_ioctl( ds3g_siolib_stream_id, SIO_IOCTL_GET_CURRENT_RTS,
           &ioctl_param );
  }
  else
  {
    ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  }

  return( ret_val );

} /* ds3g_siolib_is_rts_asserted() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_SET_FLOW_CTL_METHOD

DESCRIPTION
  Configures the flow control method for the currently open serial port.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_set_flow_ctl_method
(
  sio_flow_ctl_type  tx_flow,                    /* TX flow control method */
  sio_flow_ctl_type  rx_flow                     /* RX flow control method */
)
{
  sio_ioctl_param_type      ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_OK;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( ds3g_siolib_stream_id != SIO_NO_STREAM_ID )
  {
    ioctl_param.flow_ctl.tx_flow = tx_flow;
    ioctl_param.flow_ctl.rx_flow = rx_flow;

    sio_ioctl( ds3g_siolib_stream_id, SIO_IOCTL_SET_FLOW_CTL, &ioctl_param );
  }
  else
  {
    ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  }

  return( ret_val );

} /* ds3g_siolib_set_flow_ctl_method() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_SETUP_TX

DESCRIPTION
  This function queues data for transmission over the serial interface.
  Additionally, Autodetect Mode is re-armed, if specified by the caller.
  Typically, the re-arm feature is used only by ATCoP.

DEPENDENCIES
  No action is taken if there is no port currently open, or if the current
  serial mode is not Autodetect Mode.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_setup_tx
(
  dsm_item_type  *tx_ptr,         /* DSM item containing data to transmit */
  boolean         rearm_autodetect       /* TRUE = Re-arm Autodetect Mode */
)
{
  sio_ioctl_param_type      ioctl_param;
  dsm_item_type             *tmp_ptr;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_OK;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Uncomment out the folowing assert when underlying drivers start 
    supporting packet chains
  -------------------------------------------------------------------------*/
//  ASSERT ( tx_ptr->pkt_ptr == NULL );
  if( ds3g_siolib_stream_id != SIO_NO_STREAM_ID )
  {
    if( rearm_autodetect )
    {
      if( ds3g_siolib_state.serial_info.serial_state == DS3G_SIOLIB_AUTODETECT_SERIAL_STATE )
      {
        sio_ioctl( ds3g_siolib_stream_id, SIO_IOCTL_ENABLE_AUTODETECT,
                   &ioctl_param );
      }
      else
      {
        MSG_HIGH( "Autodt rearmed in non-Autodt serial mode", 0, 0, 0 );
      }
    }

    while (tx_ptr != NULL)
    {
      tmp_ptr = tx_ptr->pkt_ptr;
      tx_ptr->pkt_ptr = NULL;
      sio_transmit( ds3g_siolib_stream_id, tx_ptr );
      tx_ptr = tmp_ptr;
    }
  }
  else
  {
    (void)dsm_free_packet ( &tx_ptr );
    ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  }

  return( ret_val );

} /* ds3g_siolib_setup_tx() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_CHANGE_BAUD_RATE

DESCRIPTION
  Sets the baud rate of the currently open serial port to the specified
  value.  Change may be scheduled or immediate based on caller.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_change_baud_rate
(
  sio_bitrate_type  baud_rate,             /* New baud rate                */
  boolean           chg_now                /* Flag for immediate change    */
)
{
  sio_ioctl_param_type      ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_OK;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( ds3g_siolib_stream_id != SIO_NO_STREAM_ID )
  {
    ioctl_param.bitrate = baud_rate;
    sio_ioctl( ds3g_siolib_stream_id,
               (chg_now) ? SIO_IOCTL_CHANGE_BAUD_NOW
                         : SIO_IOCTL_CHANGE_BAUD_RATE,
               &ioctl_param );
  }
  else
  {
    ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  }

  return( ret_val );

} /* ds3g_siolib_change_baud_rate() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_SET_CD_STATE

DESCRIPTION
  This function sets the state of the Carrier Detect (CD) signal, based on
  the following:
  1) &C setting
     &C0 - CD is always ON
     &C1 - CD is ON when the remote carrier is ON
     &C2 - CD is always ON except for 1sec 'wink' when a data call is
           disconnected
  2) Remote carrier state
  3) End-of-call winking (only if &C=2)
  4) SMS message entry
     CD is ON during SMS message entry (overrides &C setting)

  This function should be called when a serial port is opened, and whenever
  any event occurs that impacts the CD state.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_set_cd_state
(
  ds3g_siolib_dcd_event_e_type  dcd_event      /* Event that occurred, if any */
)
{
  static boolean            remote_carrier_state = OFF;
  static boolean            dcd_winking = OFF;
  static boolean            sms_entry = OFF;
  boolean                   dcd_state = OFF;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_OK;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    First, update the appropriate internal state variable based on the type
    of event that occurred.
  -------------------------------------------------------------------------*/
  switch( dcd_event )
  {
    case DS3G_SIOLIB_REMOTE_CARRIER_ON:
      remote_carrier_state = ON;
      break;

    case DS3G_SIOLIB_REMOTE_CARRIER_OFF:
      remote_carrier_state = OFF;
      if( dsat_amp_c_val == 2 )
      {
        ds3gi_start_timer( DS3G_TIMER_WINK_DCD,
                           DS3G_SIOLIB_DCD_WINK_INTERVAL );
        dcd_winking = ON;
      }
      break;

    case DS3G_SIOLIB_CD_WINKING_END:
      dcd_winking = OFF;
      break;

    case DS3G_SIOLIB_SMS_MSG_START:
      sms_entry = ON;
      break;

    case DS3G_SIOLIB_SMS_MSG_END:
      sms_entry = OFF;
      break;

    case DS3G_SIOLIB_EVENT_NONE:
    default:
      /* No action needed */
      break;

  } /* switch( dcd_event ) */

  /*-------------------------------------------------------------------------
    Now set the CD state appropriately, based on the &C setting and related
    state variables.
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    &C0 => CD is always ON.
  -------------------------------------------------------------------------*/
  if( dsat_amp_c_val == 0 )
  {
    dcd_state = ON;
  }
  /*-------------------------------------------------------------------------
    &C1 => CD is ON only if the remote carrier is ON.
  -------------------------------------------------------------------------*/
  else if( dsat_amp_c_val == 1 )
  {
    if( remote_carrier_state == ON )
    {
      dcd_state = ON;
    }
    else
    {
      dcd_state = OFF;
    }
  }
  /*-------------------------------------------------------------------------
    &C2 => CD is always ON, except during the end-of-call winking interval.
  -------------------------------------------------------------------------*/
  else if( dsat_amp_c_val == 2 )
  {
    if( dcd_winking == ON )
    {
      dcd_state = OFF;
    }
    else
    {
      dcd_state = ON;
    }
  }

  /*-------------------------------------------------------------------------
    If an SMS message is being entered, CD should be ON. This will override
    the &C setting above.
  -------------------------------------------------------------------------*/
  if( sms_entry == ON )
  {
    dcd_state = ON;
  }

  /*-------------------------------------------------------------------------
    Set the state of the Carrier Detect (CD) signal.
  -------------------------------------------------------------------------*/
  ret_val = dcd_state ? ds3g_siolib_cd_assert() : ds3g_siolib_cd_deassert();

  return( ret_val );

} /* ds3g_siolib_set_cd_state() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_REARM_AUTODETECT

DESCRIPTION
  This function re-arms SIO Autodetect Mode. Typically, the re-arm
  feature is used only by ATCoP.

DEPENDENCIES
  No action is taken if there is no port currently open, or if the current
  serial mode is not Autodetect Mode.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_rearm_autodetect( void )
{
  sio_ioctl_param_type      ioctl_param;
  ds3g_siolib_status_e_type ret_val = DS3G_SIOLIB_OK;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( ds3g_siolib_stream_id != SIO_NO_STREAM_ID )
  {
    if( ds3g_siolib_state.serial_info.serial_state ==
        DS3G_SIOLIB_AUTODETECT_SERIAL_STATE )
    {
      sio_ioctl( ds3g_siolib_stream_id, SIO_IOCTL_ENABLE_AUTODETECT,
                 &ioctl_param );
    }
    else
    {
      MSG_HIGH( "Autodt rearmed in non-Autodt serial mode", 0, 0, 0 );
    }
  }
  else
  {
    ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  }

  return( ret_val );

} /* ds3g_siolib_rearm_autodetect() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_FLUSH_TX

DESCRIPTION
  This function is used to schedule the SIO TX queue to be flushed when
  changed to and from online-command and online-data modes.

  The flush_func_ptr_cb parameter is used to pass a callback function to
  SIO that is called when the flush operation has completed.  The callback
  should be used to notify the task which requested the flush operation
  that the TX queue is now empty.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_flush_tx
(
  void (*flush_func_ptr_cb)(void)
)
{
  ds3g_siolib_status_e_type               ret_val = DS3G_SIOLIB_OK;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( ds3g_siolib_stream_id != SIO_NO_STREAM_ID )
  {
    sio_flush_tx( ds3g_siolib_stream_id, flush_func_ptr_cb );
  }
  else
  {
    ret_val = DS3G_SIOLIB_NO_PORT_OPEN;
  }

  return( ret_val );

} /* ds3g_siolib_flush_tx() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_HANDLE_1ST_SIO_RX_SIG

DESCRIPTION
  This function processes the DS_1ST_SIO_RX_SIG, which indicates that data
  bytes (i.e. AT commands) have been received from SIO, in Command state or
  Online Command state. An ATCoP function is called to process the received
  data bytes.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3g_siolib_handle_1st_sio_rx_sig( void )
{
    dsat_mode_enum_type  at_mode;                              /* ATCoP mode */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*-------------------------------------------------------------------------
      Set the ATCoP mode based on the current AT command processing state.
    -------------------------------------------------------------------------*/
  if( ds3g_siolib_state.at_state == DS3G_SIOLIB_COMMAND_AT_STATE )
  {
    at_mode = DSAT_CMD;
  }
  else if( ds3g_siolib_state.at_state == DS3G_SIOLIB_ONLINE_CMD_AT_STATE )
  {
    at_mode = DSAT_ONLINE_CMD;
  }
  else
  {
    /*-----------------------------------------------------------------------
      We are either in Null state or Online Data state. The 1ST_SIO_RX signal
      should never be set in these states, so log an error and return without
      any further processing.
    -----------------------------------------------------------------------*/
    ERR( "1ST_SIO_RX_SIG rx'ed in invalid state", 0, 0, 0 );
    return;
  }

  /*-------------------------------------------------------------------------
    Invoke an ATCoP function to process the rx'ed data bytes (AT commands).
  -------------------------------------------------------------------------*/
  dsat_process_sio_command(at_mode, ds3g_siolib_state.serial_info.rx_wm_ptr);

} /* ds3g_siolib_handle_1st_sio_rx_sig() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_PERFORM_ONLINE_CMD_SWITCH

DESCRIPTION
  This function is used to complete the transition of an SIO/ATCOP mode
  change from online data to online command mode.  It is responsible for
  switching the SIO watermarks to those specified by the mode specific
  handler when a data call is established.

  This function is called directly from ds3g_siolib_process_enter_online_cmd_mode()
  when a DS3G_SIOLIB_ENTER_ONLINE_CMD_NO_TX_FLUSH command is received and no SIO TX
  flushing is required.  Since there is no flushing required in this case,
  the watermarks can be switched immediately.

  It is also called when a DS3G_SIOLIB_COMPLETE_ONLINE_CMD_SWITCH_CMD is received.
  This command is sent to SIOLIB in response to receiving a
  DS3G_SIOLIB_ENTER_ONLINE_CMD_TX_FLUSH command which requires the SIO TX queue to
  be flushed.  In this situation, the watermarks can NOT be switched until
  the TX queue has been emptied otherwise data may be lost.  When the queue
  is emptied, SIO will invoke a registered callback function that will send
  the DS3G_SIOLIB_COMPLETE_ONLINE_CMD_SWITCH_CMD to SIOLIB indicating that it is
  now okay to switch the watermarks for online-command processing.

DEPENDENCIES
  Refer to the following companion functions for additional details:

    ds3g_enter_online_cmd_mode()
    ds3g_siolib_process_enter_online_cmd_mode()
    ds3g_siolib_signal_complete_online_cmd_switch_cb()

RETURN VALUE
  None

SIDE EFFECTS
  None.

=========================================================================*/
static void ds3g_siolib_perform_online_cmd_switch(void)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ds3g_siolib_change_mode( SIO_DS_AUTODETECT_MODE, 
                           NULL,
                           NULL,
                           NULL );

  ds3g_siolib_state.serial_info.serial_state =
  DS3G_SIOLIB_AUTODETECT_SERIAL_STATE;

  ds3g_siolib_state.at_state = DS3G_SIOLIB_ONLINE_CMD_AT_STATE;
  
  MSG_HIGH( "Completed ONLINE_CMD mode switch", 0,0,0 );
} /* ds3g_siolib_perform_online_cmd_switch() */



/*===========================================================================

FUNCTION DS3G_SIOLIB_HANDLE_SIO_ESCAPE_SIG

DESCRIPTION
  This function is to handle AT escape processing (+++ detection) when
  SIOLIB receives a DS3G_SIOLIB_ESCAPE_SIG signal.  It calls the callback
  function that was registered previously by one of the SIOLIB clients
  like the packet controller, async. controller and so on.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void ds3g_siolib_handle_sio_escape_sig ( void )
{
  ds3g_siolib_func_tbl_type *hdlr_tbl_ptr;
/*-----------------------------------------------------------------------
 Point to the appropriate handler function table based on the current
 mode and call type.
 -----------------------------------------------------------------------*/
  hdlr_tbl_ptr = ds3g_siolib_state.func_tbl;

  /*-----------------------------------------------------------------------
   Ensure that a handler function table is registered, with a non-null
   escape_sequence_handler.
  -----------------------------------------------------------------------*/
  if( hdlr_tbl_ptr != NULL )
  {
    if( hdlr_tbl_ptr->at_escape_sequence_handler != NULL )
    {
      if (hdlr_tbl_ptr->at_escape_sequence_handler() == DSAT_ONLINE_CMD)
      {
        MSG_HIGH( "detected '+++' - switching to online-command mode",0,0,0);

        ds3g_siolib_perform_online_cmd_switch();

        dsat_send_result_code (DSAT_OK);

  #ifdef FEATURE_ETSI_SMS
#error code not present
  #endif /* FEATURE_ETSI_SMS */
      }
      else
      {
        MSG_HIGH( "Mode handler rejected '+++' escape sequence", 0,0,0 );
      }
    } /* if esc_seq_hdlr != NULL */
    else
    {
      MSG_HIGH( "at_escape_sequence_handler() not registered", 0, 0, 0 );
      dsat_send_result_code ( DSAT_ERROR );
    }
  } /* if hdlr_tbl_ptr != NULL */
  else
  {
    MSG_HIGH( "hdlr_tbl_ptr = NULL in siolib table", 0, 0, 0 );
  }
} /* ds3gi_process_sio_escape() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_SIGNAL_COMPLETE_ONLINE_CMD_SWITCH_CB

DESCRIPTION
  This is a callback function that is registered with SIO by SIOLIB when
  the SIO TX fifo is scheduled to be flushed during a mode change to online
  command mode.  It is invoked by SIO and is used to notify SIOLIB when
  flushing of the TX fifo has completed.

  The function sends a DS_COMPLETE_ONLINE_CMD_SWITCH_CMD command to SIOLIB.
  This command is used to by SIOLIB to indicate when it is ok
  to complete the transition from online data to online command mode.

DEPENDENCIES
  Used only during a SIO/ATCOP mode change from online data to online
  command mode.

  Refer to the following companion functions for additional details:

    ds3g_enter_online_cmd_mode()
    ds3g_siolib_process_enter_online_cmd_mode()
    ds3g_siolib_perform_online_cmd_switch()

RETURN VALUE
  None

SIDE EFFECTS
  SIOLIB will be notified that SIO TX flushing has completed.

===========================================================================*/

static void ds3g_siolib_signal_complete_online_cmd_switch_cb(void)
{
  ds_cmd_type *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Allocate the command buffer and queue the command to DS */
  if ( (cmd_ptr = ds_get_cmd_buf()) != NULL )
  {
    cmd_ptr->hdr.cmd_id = DS_COMPLETE_ONLINE_CMD_SWITCH_CMD;

    /* Notify the DS Task */
    ds_put_cmd(cmd_ptr);
  }
  else
  {
    ERR_FATAL("ds_get_cmd_buf() failed", 0,0,0);
  }
} /* ds3g_siolib_signal_complete_online_cmd_switch_cb() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_SIGNAL_COMPLETE_ONLINE_DATA_SWITCH_CB

DESCRIPTION
  This is a callback function that is registered with SIO by SIOLIB when
  the SIO TX fifo is scheduled to be flushed during a mode change to online
  data mode.  It is invoked by SIO and is used to notify SIOLIB when
  flushing of the TX fifo has completed.

  The function sends a DS_COMPLETE_ONLINE_DATA_SWITCH_CMD command to 3G
  SIOLIB.  This command is used to by SIOLIB to indicate when it is ok
  to complete the transition from online command to online data mode.

DEPENDENCIES
  Used only during a SIO/ATCOP mode change from online command to online
  data mode.

  Refer to the following companion functions for additional details:

    ds3g_enter_online_data_mode()
    ds3g_siolib_process_enter_online_data_mode()

RETURN VALUE
  None

SIDE EFFECTS
  SIOLIB will be notified that SIO TX flushing has completed.

===========================================================================*/
static void ds3g_siolib_signal_complete_online_data_switch_cb(void)
{
  ds_cmd_type *cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* Allocate the command buffer and queue the command to DS */
  if ( (cmd_ptr = ds_get_cmd_buf()) != NULL )
  {
    cmd_ptr->hdr.cmd_id = DS_COMPLETE_ONLINE_DATA_SWITCH_CMD;

    /* Notify the DS Task */
    ds_put_cmd(cmd_ptr);
  }
  else
  {
    ERR_FATAL("ds_get_cmd_buf() failed", 0,0,0);
  }
} /* ds3g_siolib_signal_complete_online_data_switch_cb() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_PROCESS_ENTER_ONLINE_DATA_MODE

DESCRIPTION
  This function is used to process either a DS_ENTER_ONLINE_DATA_TX_FLUSH_CMD
  or DS_ENTER_ONLINE_DATA_NO_TX_FLUSH_CMD received from an external task
  request to switch the SIO/ATCOP processing mode to online data mode.

  The flush_tx parameter is used to indicate whether the SIO TX fifo should
  be flushed prior to switching to online data mode as follows:

    If flush_tx == TRUE, the SIO TX fifo will be flushed.
    If flush_tx == FALSE, the SIO TX fifo will not be flushed, any data
      currently in the queue will be lost.

  If TX flushing is required, the ds3g_siolib_signal_complete_online_data_switch_cb()
  function will be registered with SIO to be invoked when TX flushing has
  completed.  The callback function is used to notify SIOLIB that it can
  now complete the mode change to online data mode.

  If no TX flushing is required, the mode change will occur immediately.

  The mode specific handler is invoked when the actual switch to online
  data mode has completed.  The handler should be used to notify the
  requesting task that online data mode processing is now active.

DEPENDENCIES
  A data call must currently be active.  The request must contain a valid
  call identifier.  Must currently be processing in online command mode.

  Refer to the following companion functions for additional details:

    ds3g_enter_online_data_mode()
    ds3g_siolib_signal_complete_online_data_switch_cb()

RETURN VALUE
  None

SIDE EFFECTS
  If the TX buffer is drained, SIO will invoke the specified callback
  function to signal SIOLIB when the fifo has been drained.

===========================================================================*/

void  ds3g_siolib_process_enter_online_data_mode
(
  boolean       flush_tx
)
{
  ds3g_siolib_func_tbl_type  *hdlr_tbl_ptr;    /* Ptr to mode-sp. hdlr table */
  dsm_watermark_type         *from_sio_wm_ptr;
  dsm_watermark_type         *to_sio_wm_ptr;
  sio_rx_func_ptr_type        sio_rx_func_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH( "ONLINE_DATA mode switch, flush_tx=%d", flush_tx,0,0 );

  if (ds3g_siolib_state.at_state == DS3G_SIOLIB_ONLINE_CMD_AT_STATE)
  {
    /*-----------------------------------------------------------------------
      Check to see if the TX buffer needs to be flushed.  If so we schedule
      the flush with SIO.  When the buffer is emptied, SIO will call the
      registered callback function which will complete the transition to
      online command mode.
    -----------------------------------------------------------------------*/
    if (flush_tx)
    {
      MSG_HIGH( "Scheduling TX buffer flush", 0,0,0 );

      ds3g_siolib_state.serial_info.serial_state =
        DS3G_SIOLIB_WAIT_TX_FLUSH_SERIAL_STATE;

      ds3g_siolib_flush_tx(ds3g_siolib_signal_complete_online_data_switch_cb);
    }
    else
    {
      /*-----------------------------------------------------------------------
        Point to the appropriate handler function table based on the current
        mode and call type.
      -----------------------------------------------------------------------*/
      hdlr_tbl_ptr = ds3g_siolib_state.func_tbl;

      /*-----------------------------------------------------------------------
        Ensure that a handler function table is registered, with a non-null
        call_ended_handler.
      -----------------------------------------------------------------------*/
      if ( hdlr_tbl_ptr != NULL )
      {
        if ( hdlr_tbl_ptr->at_return_to_online_data_handler != NULL )
        {
          if ( hdlr_tbl_ptr->at_return_to_online_data_complete_handler != NULL )
          {
            /*-----------------------------------------------------------
              Query the watermarks and SIO RX callback function from
              protocol stack. These parameters are needed for switching
              SIO between online modes.
            -----------------------------------------------------------*/
            hdlr_tbl_ptr->at_return_to_online_data_handler(
                            &from_sio_wm_ptr,
                            &to_sio_wm_ptr,
                            &sio_rx_func_ptr);

            /*-----------------------------------------------------------
              Make the switch to online-data mode.
            -----------------------------------------------------------*/
            ds3g_siolib_change_mode( SIO_DS_RAWDATA_MODE, 
                                     from_sio_wm_ptr,
                                     to_sio_wm_ptr,
                                     sio_rx_func_ptr );
            
            ds3g_siolib_state.serial_info.serial_state =
                DS3G_SIOLIB_RAWDATA_SERIAL_STATE;

            ds3g_siolib_state.at_state = DS3G_SIOLIB_ONLINE_DATA_AT_STATE;

            MSG_HIGH( "Completed ONLINE_DATA mode switch", 0,0,0 );

            /*-----------------------------------------------------------
              Finish the switch to online data mode by performing
              protocol stack specific operations. The mode-specific
              handler should resume data transfer over the air.
            -----------------------------------------------------------*/
            hdlr_tbl_ptr->at_return_to_online_data_complete_handler();

          } /* if at_return_to_online_data_complete_handler != NULL */
          else
          {
            ERR( "Complete mode handler not registered", 0,0,0 );
          }
        } /* if at_return_to_online_data_handler != NULL */
        else
        {
          ERR( "Mode handler not registered", 0,0,0 );
        }
      } /* if hdlr_tbl_ptr != NULL */
    } /* if (flush_tx)*/
  } /* if ( (ds3g_siolib_state.at_state... */
  else
  {
    if ( ds3g_siolib_state.call_coming_up == FALSE )
    {
      ERR( "Switch to ONLINE_CMD mode failed",
           ds3g_siolib_state.call_coming_up, 0, 0);
    }
    else
    {
      ERR( "Switch to ONLINE_CMD mode failed,invalid at_state=%d",
           ds3g_siolib_state.at_state, 0, 0);
    }
  }
} /* ds3gi_process_enter_online_data_mode() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_PROCESS_ENTER_ONLINE_CMD_MODE

DESCRIPTION
  This function is used to process either a DS_ENTER_ONLINE_CMD_TX_FLUSH_CMD
  or DS_ENTER_ONLINE_CMD_NO_TX_FLUSH_CMD received from an external task
  request to switch the SIO/ATCOP processing mode to online command mode.

  The flush_tx parameter is used to indicate whether the SIO TX fifo should
  be flushed prior to switching to online command mode as follows:

    If flush_tx == TRUE, the SIO TX fifo will be flushed.
    If flush_tx == FALSE, the SIO TX fifo will not be flushed, any data
      currently in the queue will be lost.

  If TX flushing is required, the ds3g_siolib_signal_complete_online_cmd_switch_cb()
  function will be registered with SIO to be invoked when TX flushing has
  completed.  The callback function is used to notify SIOLIB that it can
  now complete the mode change to online comand mode.

  If no TX flushing is required, the mode change will occur immediately.

  The mode specific handler is invoked when the actual switch to online
  command mode has completed.  The handler should be used to notify the
  requesting task that online command mode processing is now active.

DEPENDENCIES
  A data call must currently be active.  The request must contain a valid
  call identifier.  Must currently be processing in online data mode.

  Refer to the following companion functions for additional details:

    ds3g_enter_online_cmd_mode()
    ds3g_siolib_signal_complete_online_cmd_switch_cb()
    ds3g_siolib_perform_online_cmd_switch()

RETURN VALUE
  None

SIDE EFFECTS
  If the TX buffer is drained, SIO will invoke the specified callback
  function to signal SIOLIB when the fifo has been drained.

===========================================================================*/

static void  ds3g_siolib_process_enter_online_cmd_mode
(
  boolean       flush_tx
)
{
  ds3g_siolib_func_tbl_type  *hdlr_tbl_ptr;    /* Ptr to mode-sp. hdlr table */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW( "Processing ONLINE_CMD mode switch, flush_tx=%d", flush_tx,0,0 );

  if ( ds3g_siolib_state.at_state == DS3G_SIOLIB_ONLINE_DATA_AT_STATE )
  {
    /*-----------------------------------------------------------------------
      Check to see if the TX buffer needs to be flushed.  If so we schedule
      the flush with SIO.  When the buffer is emptied, SIO will call the
      registered callback function which will complete the transition to
      online command mode.
    -----------------------------------------------------------------------*/
    if (flush_tx)
    {
      MSG_HIGH( "Scheduling TX buffer flush",
                0,0,0 );

      ds3g_siolib_state.serial_info.serial_state =
          DS3G_SIOLIB_WAIT_TX_FLUSH_SERIAL_STATE;

      ds3g_siolib_flush_tx(ds3g_siolib_signal_complete_online_cmd_switch_cb);
    }
    else
    {
      /*-----------------------------------------------------------------------
      Point to the appropriate handler function table based on the current
      mode and call type.
      -----------------------------------------------------------------------*/
      hdlr_tbl_ptr = ds3g_siolib_state.func_tbl;

      /*-----------------------------------------------------------------------
      Ensure that a handler function table is registered, with a non-null
      call_ended_handler.
      -----------------------------------------------------------------------*/
      if ( hdlr_tbl_ptr != NULL )
      {
        if ( hdlr_tbl_ptr->return_to_online_cmd_mode_complete_handler != NULL )
        {
          /*-----------------------------------------------------------------
          Make the switch to online-command mode
          -----------------------------------------------------------------*/
          ds3g_siolib_perform_online_cmd_switch();

          /*-----------------------------------------------------------------
           Invoke the mode-specific handler function.
          -----------------------------------------------------------------*/
          hdlr_tbl_ptr->return_to_online_cmd_mode_complete_handler();
        }
        else
        {
          ERR( "online_cmd_mode_complete mode handler not registered", 0,0,0 );
        }
      } /* if hdlr_tbl_ptr != NULL */
    } /* else (flush_tx) */
  } /* if (ds3g_siolib_state.at_state... */
  else
  {
    if ( ds3g_siolib_state.call_coming_up == FALSE )
    {
      ERR( "Switch to ONLINE_CMD mode failed",
        ds3g_siolib_state.call_coming_up, 0, 0);
    }
    else
    {
      ERR( "Switch to ONLINE_CMD mode failed,invalid at_state=%d",
        ds3g_siolib_state.at_state, 0, 0);
    }
  }
} /* ds3g_siolib_process_enter_online_cmd_mode() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_HANDLE_DTR_CHANGED_SIG

DESCRIPTION
  This function calls the callback function that was previously registered
  by one of the SIOLIB clients like the packet controller, async.
  controller and so on.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void ds3g_siolib_handle_dtr_changed_sig(void)
{
  boolean dtr_status = FALSE;
  ds3g_siolib_status_e_type  ret_val = DS3G_SIOLIB_OK;
  ds3g_siolib_func_tbl_type *hdlr_tbl_ptr = ds3g_siolib_state.func_tbl;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ret_val = ds3g_siolib_is_dtr_asserted ( &dtr_status );

  if ( hdlr_tbl_ptr == NULL ) 
  {
    if ( ds3g_siolib_state.call_coming_up == TRUE )
    {
      ERR( "ds3g_siolib_func_tbl() not registered", 0, 0, 0 );
    }
    return;
  } /* if hdlr_tbl_ptr == NULL */

  if (ret_val == DS3G_SIOLIB_OK)
  {
    if (dtr_status == FALSE) 
    {
      if (dsat_amp_d_val == 0)
      {
        if ( hdlr_tbl_ptr->dtr_changed_sig_handler_ampd0 != NULL )
        {
          hdlr_tbl_ptr->dtr_changed_sig_handler_ampd0 ( dtr_status );
        }
        else
        {
          if ( ds3g_siolib_state.call_coming_up == TRUE )
          {
            MSG_LOW("AT&D=0 callback not registered", 0, 0, 0);
          }
          return;
        }
      }
      else if ((dsat_amp_d_val == 1) && 
               (ds3g_siolib_state.at_state == DS3G_SIOLIB_ONLINE_DATA_AT_STATE))
      {
        if( hdlr_tbl_ptr->at_escape_sequence_handler != NULL )
        {
          if (hdlr_tbl_ptr->at_escape_sequence_handler() == DSAT_ONLINE_CMD)
          {
            /*---------------------------------------------------------------
            &D1 => DTR de-asserted means enter online command mode
            ---------------------------------------------------------------*/
            MSG_HIGH( "DTR de-asserted AT&D=1, enter online-command mode", 0, 0, 0 );
            ds3g_siolib_perform_online_cmd_switch();

            dsat_send_result_code(DSAT_OK);

#ifdef FEATURE_ETSI_SMS
#error code not present
#endif /* FEATURE_ETSI_SMS */
          } /* if esc_seq_hdlr returns DSAT_ONLINE_CMD */ 
        } /* if esc_seq_hdlr != NULL */
        else
        {
          if ( ds3g_siolib_state.call_coming_up == TRUE )
          {
            MSG_LOW ( "escape_seq_handler() not registered", 0, 0, 0 );
          }
          return;
        } /* esc_seq_hdlr == NULL */
      } /* if &D=1 */
      else if (dsat_amp_d_val == 2)
      {
        if ( hdlr_tbl_ptr->dtr_changed_sig_handler != NULL )
        {
          hdlr_tbl_ptr->dtr_changed_sig_handler ( dtr_status );
        } /* if dtr_changed_sig_hdlr != NULL */
        else
        {
          if ( ds3g_siolib_state.call_coming_up == TRUE )
          {
            MSG_LOW ( "dtr_changed_sig_handler() not registered", 0, 0, 0 );
          }
          return;
        } /* dtr_changed_sig_hdlr == NULL */
      } /* amp_d_val = 2 */
      else
      {
        ERR("Invalid AT&D value %d", dsat_amp_d_val, 0, 0); 
      }
    } /* dtr_status == FALSE */
  }
  else
  {
    ERR ( "NO PORT OPEN", 0, 0, 0 );
  }
} /* ds3g_siolib_handle_dtr_changed_sig() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_PROCESS_SIGNALS

DESCRIPTION
  This function processes SIOLIB signals. The appropriate processing is
  performed based on the signal(s) that are set in the signal mask.

  This function should be called by the top-level task dispatcher whenever
  any of the SIOLIB signals are set.

DEPENDENCIES
  SIOLIB should have been initialized prior to calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3g_siolib_process_signals
(
  rex_sigs_type    set_sigs                 /* Mask containing set signals */
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    Perform the appropriate processing based on the signal(s) that are set.
  -------------------------------------------------------------------------*/

  if( (set_sigs & DS_1ST_SIO_RX_SIG) != 0 )
  {
    rex_clr_sigs( &ds_tcb, DS_1ST_SIO_RX_SIG );
    ds3g_siolib_handle_1st_sio_rx_sig();
  }

  if( (set_sigs & DS_SIO_RX_PKT_SIG) != 0 )
  {
    MSG_LOW( "Rx'ed SIO_RX_PKT_SIG", 0, 0, 0 );
    rex_clr_sigs( &ds_tcb, DS_SIO_RX_PKT_SIG );
  }

  if( (set_sigs & DS_SIO_ESCAPE_SIG) != 0 )
  {
    MSG_LOW( "Rx'ed SIO_ESCAPE_SIG", 0, 0, 0 );
    rex_clr_sigs( &ds_tcb, DS_SIO_ESCAPE_SIG );
    ds3g_siolib_handle_sio_escape_sig();
  }

  if( (set_sigs & DS_DTR_CHANGED_SIG) != 0 )
  {
    MSG_LOW( "Rx'ed DTR_CHANGED_SIG", 0, 0, 0 );
    rex_clr_sigs( &ds_tcb, DS_DTR_CHANGED_SIG );
    ds3g_siolib_handle_dtr_changed_sig();
  }

} /* ds3g_process_signals() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_PROCESS_CMDS

DESCRIPTION
  This function processes SIOLIB commands. A command processing function is
  called based on the type of command received.

  This function should be called by the top-level task dispatcher whenever
  any SIOLIB command is received.

DEPENDENCIES
  SIOLIB should have been initialized prior to calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3g_siolib_process_cmds
(
  ds_cmd_type  *cmd_ptr
)
{

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    Invoke the appropriate function to process the command, based on the
    command id.
  -------------------------------------------------------------------------*/
  switch( cmd_ptr->hdr.cmd_id )
  {
    case DS_RDM_OPEN_CMD:
      ds3gi_handle_rdm_open_cmd( cmd_ptr );
      break;

    case DS_RDM_CLOSE_CMD:
      ds3gi_handle_rdm_close_cmd();
      break;

    case DS_ENTER_ONLINE_CMD_TX_FLUSH_CMD:
      /* Don't allow multiple TX flush requests */
      if ( ds3g_siolib_state.serial_info.serial_state != DS3G_SIOLIB_WAIT_TX_FLUSH_SERIAL_STATE )
      {
        ds3g_siolib_process_enter_online_cmd_mode( TRUE );
      }
      else
      {
        MSG_HIGH("Switch to ONLINE_CMD mode rejected, TX flush already in progress",
          0,0,0);
      }
      break;

    case DS_ENTER_ONLINE_CMD_NO_TX_FLUSH_CMD:/* Enter online-data w/no TX flush */
    case DS_COMPLETE_ONLINE_CMD_SWITCH_CMD:  /* Rx'd after a TX flush completes */
      ds3g_siolib_process_enter_online_cmd_mode( FALSE );
      break;

    case DS_ENTER_ONLINE_DATA_TX_FLUSH_CMD:
      /* Don't allow multiple TX flush requests */
      if ( ds3g_siolib_state.serial_info.serial_state != DS3G_SIOLIB_WAIT_TX_FLUSH_SERIAL_STATE )
      {
        ds3g_siolib_process_enter_online_data_mode( TRUE );
      }
      else
      {
        MSG_HIGH("Switch to ONLINE_DATA mode rejected, TX flush already in progress",
          0,0,0);
      }
      break;

    case DS_ENTER_ONLINE_DATA_NO_TX_FLUSH_CMD:/* Enter online-cmd w/no TX flush */
    case DS_COMPLETE_ONLINE_DATA_SWITCH_CMD:  /* Rx'd after a TX flush completes */
      ds3g_siolib_process_enter_online_data_mode( FALSE );
      break;

    default:
      ERR( "SIOLIB rx'ed unrecognized cmd: %d", cmd_ptr->hdr.cmd_id, 0, 0 );
      break;

  } /* switch( cmd_id ) */

} /* ds3g_process_cmds() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_CLEANUP_AUTODETECT_RX_WATERMARK

DESCRIPTION
  This function cleans up the watermark used for RX data from SIO.
  Any data stored in the watermark is discarded.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void  ds3g_siolib_cleanup_autodetect_rx_watermark( void )
{
  dsm_item_type  *item_ptr;                         /* Pointer to dsm item */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  /*-------------------------------------------------------------------------
    De-queue dsm items and free the items (i.e. discard the data), until the
    watermark is empty.
  -------------------------------------------------------------------------*/
  while( (item_ptr = dsm_dequeue( ds3g_siolib_state.serial_info.rx_wm_ptr )) !=
     NULL )
  {
    dsm_free_packet( &item_ptr );
  }

} /* ds3g_siolib_cleanup_autodetect_rx_watermark() */
/*===========================================================================

FUNCTION DS3G_SIOLIB_ENTER_ONLINE_DATA_MODE

DESCRIPTION
  This function is used to initiate an asynchronous SIO/ATCOP processing mode
  change to online data mode.via an external task request.  The flush_tx
  parameter is used to indicate whether the SIO TX fifo should be flushed
  prior to switching to online data mode as follows:

    If flush_tx == TRUE, the SIO TX fifo will be flushed.
    If flush_tx == FALSE, the SIO TX fifo will not be flushed, any data
      currently in the queue will be lost.

  The function sends either a DS_ENTER_ONLINE_DATA_TX_FLUSH_CMD or
  DS_ENTER_ONLINE_DATA_NO_TX_FLUSH_CMD depending on the value of flush_tx to
  SIOLIB to initiate the mode change.  If no TX flushing is required, the
  mode change will occur immediately, otherwise, SIOLIB will schedule the
  SIO TX fifo to be flushed.  When the flush completes, the switch to online
  data mode will be made.

DEPENDENCIES
  Must be able to allocate a DS Task command buffer.

  Refer to the following companion functions for additional details:

    ds3g_siolib_process_enter_online_data_mode()
    ds3g_siolib_signal_complete_online_data_switch_cb()

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void ds3g_siolib_enter_online_data_mode
(
  boolean flush_tx
)
{
  ds_cmd_type * cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH("Requesting mode change to ONLINE_DATA",0,0,0);

  /* Allocate the command buffer and queue the command to the DS Task */
  if ( (cmd_ptr = ds_get_cmd_buf()) != NULL )
  {
    /* Need to determine the appropriate command based on whether or
    ** not the TX buffer should be flushed.
    */
    if (flush_tx)
    {
      cmd_ptr->hdr.cmd_id = DS_ENTER_ONLINE_DATA_TX_FLUSH_CMD;
    }
    else
    {
      cmd_ptr->hdr.cmd_id = DS_ENTER_ONLINE_DATA_NO_TX_FLUSH_CMD;
    }

    /* Send the command on its way */
    ds_put_cmd(cmd_ptr);
  }
  else
  {
    ERR_FATAL("ds_get_cmd_buf() failure", 0,0,0);
  }
} /* ds3g_enter_online_data_mode() */

/*===========================================================================

FUNCTION DS3G_SIOLIB_ENTER_ONLINE_CMD_MODE

DESCRIPTION
  This function is used to initiate an asynchronous SIO/ATCOP processing mode
  change to online command mode.via an external task request.  The flush_tx
  parameter is used to indicate whether the SIO TX fifo should be flushed
  prior to switching to online command mode as follows:

    If flush_tx == TRUE, the SIO TX fifo will be flushed.
    If flush_tx == FALSE, the SIO TX fifo will not be flushed, any data
      currently in the queue will be lost.

  The function sends either a DS_ENTER_ONLINE_CMD_TX_FLUSH_CMD or
  DS_ENTER_ONLINE_CMD_NO_TX_FLUSH_CMD depending on the value of flush_tx to
  SIOLIB to initiate the mode change.  If no TX flushing is required, the
  mode change will occur immediately, otherwise, SIOLIB will schedule the
  SIO TX fifo to be flushed.  When the flush completes, the switch to online
  command mode will be made.

DEPENDENCIES
  Must be able to allocate a DS Task command buffer.

  Refer to the following companion functions for additional details:

    ds3g_siolib_process_enter_online_cmd_mode()
    ds3g_siolib_signal_complete_online_cmd_switch_cb()
    ds3g_siolib_perform_online_cmd_switch()

RETURN VALUE
  None

SIDE EFFECTS
  None.

===========================================================================*/

void ds3g_siolib_enter_online_cmd_mode
(
  boolean flush_tx
)
{
  ds_cmd_type * cmd_ptr;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH("Requesting mode change to ONLINE_CMD",0,0,0);

  /* Allocate the command buffer and queue the command to the DS Task */
  if ( (cmd_ptr = ds_get_cmd_buf()) != NULL )
  {
    /* Need to determine the appropriate command based on whether or
    ** not the TX buffer should be flushed.
    */
    if (flush_tx)
    {
      cmd_ptr->hdr.cmd_id = DS_ENTER_ONLINE_CMD_TX_FLUSH_CMD;
    }
    else
    {
      cmd_ptr->hdr.cmd_id = DS_ENTER_ONLINE_CMD_NO_TX_FLUSH_CMD;
    }

    MSG_LOW("sending command %d to DSTask", cmd_ptr->hdr.cmd_id,0,0);

    /* Send the command on its way */
    ds_put_cmd(cmd_ptr);
  }
  else
  {
    ERR_FATAL("ds_get_cmd_buf() failure", 0,0,0);
  }
} /* ds3g_enter_online_cmd_mode() */


/*===========================================================================

FUNCTION DS3GI_CONVERT_SERIAL_MODE

DESCRIPTION
  This function converts the specified SIO serial mode into the corresponding
  3G Dsmgr serial state.

DEPENDENCIES
  None

RETURN VALUE
  Returns the 3G Dsmgr serial state corresponding to the specified SIO serial
  mode.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_serial_state_e_type  ds3gi_convert_serial_mode
(
  sio_stream_mode_type  sio_serial_mode                 /* SIO serial mode */
)
{
  ds3g_siolib_serial_state_e_type  serial_state;        /* 3G Dsmgr serial state */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( sio_serial_mode == SIO_DS_AUTODETECT_MODE )
  {
    serial_state = DS3G_SIOLIB_AUTODETECT_SERIAL_STATE;
  }
  else if( sio_serial_mode == SIO_DS_RAWDATA_MODE )
  {
    serial_state = DS3G_SIOLIB_RAWDATA_SERIAL_STATE;
  }
  else if( sio_serial_mode == SIO_DS_PKT_MODE )
  {
    serial_state = DS3G_SIOLIB_PACKET_SERIAL_STATE;
  }
  else
  {
    ERR( "Invalid serial mode, using Autodetect", 0, 0, 0 );
    serial_state = DS3G_SIOLIB_AUTODETECT_SERIAL_STATE;
  }

  return( serial_state );

} /* ds3gi_convert_serial_mode() */

/*===========================================================================
FUNCTION DS3G_SIOLIB_SET_INBOUND_FLOW

DESCRIPTION
  This function controls the enabling or diabling of inbound flow ctl.
  This function was required because inbound flow ctl. can be changed
  from several places for different reasons, and we need an arbitrator
  function to handle those changes.

  Basically, the function enables inbound flow, when no caller wants it
  disabled. If even one caller wants the flow disabled, the inbound flow
  is disabled.


DEPENDENCIES
  This is the only function that should be used to toggle the inbound flow.
  This is important because the function keeps track of the state of inbound
  flow, and if the state of inbound flow is changed


RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void ds3g_siolib_set_inbound_flow
(
  uint32 in_mask,          /* input mask of calling function                 */
  ds3g_flow_e_type flow  /* specifies if flow is to be enabled or disabled */
)
{
  sio_ioctl_param_type ioctl_param;
  static uint32 static_mask = 0xFFFFFFFF;  /* Keeps track of state of flow ctl */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    If DS is not assigned a serial port, then doing flow control is
    meaningless.
  -------------------------------------------------------------------------*/
  if ( ds3g_siolib_state.serial_info.serial_state ==
       DS3G_SIOLIB_NULL_SERIAL_STATE )
  {
    return;
  }

  /*-------------------------------------------------------------------------
    If we are asked to enable flow control then OR the input mask to static
    mask to get the new value of the static mask.
    If asked to disable flow control, then AND the static mask to the
    complement of the input mask to get the new static mask.
  -------------------------------------------------------------------------*/
  INTLOCK();

  DS_FLOW_CTRL_SET_MASK (flow, static_mask, in_mask);

  /*-------------------------------------------------------------------------
    If static mask says that all functions have enabled flow control, do the
    actual function call to enable flow control. Even if one function call
    has disabled flow, CTS will stay disabled.
  -------------------------------------------------------------------------*/
  if (static_mask == DS_FLOW_IS_ENABLED)
  {
    sio_ioctl( ds3g_siolib_stream_id, SIO_IOCTL_INBOUND_FLOW_ENABLE,
                 &ioctl_param );
  }
  else
  {
    sio_ioctl( ds3g_siolib_stream_id, SIO_IOCTL_INBOUND_FLOW_DISABLE,
               &ioctl_param );
  }
  INTFREE();

  //MSG_LOW("Call Msk: 0x%x FlowMsk: 0x%x",
  //         in_mask, static_mask, 0);


} /* ds_set_inbound_flow() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_ALERT_USER

DESCRIPTION
  This function rings the serial port to alert the user/TE of an incoming
  data or voice call. The RI signal is toggled, and the RING result code is
  sent to the TE. Auto-answer is also performed, if requested by the caller
  (and if enabled by the S0 s-register).

  This function should be called to start alerting, and subsequently, each
  time the ring timer expires. This function may be invoked either by the
  mode specific handler (for data calls) or ATCoP (for voice calls).

  Note that the auto-answer facility is intended to be used for data calls
  only.

DEPENDENCIES
  This function does nothing if there is no serial port currently open.

RETURN VALUE
  None

SIDE EFFECTS
  Updates 3G Dsmgr state variables (ring_counter).

===========================================================================*/
void  ds3g_siolib_alert_user
(
  dsat_result_enum_type  ring_type,      /* Ring result code to send to TE */
  boolean                auto_answer        /* TRUE => Perform auto-answer */
)
{
  boolean         call_answered = FALSE;
  dword           duration;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/


  if( ds3g_siolib_state.serial_info.serial_state != DS3G_SIOLIB_NULL_SERIAL_STATE )
  {
    ds3g_siolib_state.ring_type = ring_type;
    if( ds3g_siolib_state.serial_info.ri_asserted )
    {
      /*---------------------------------------------------------------------
        RI is asserted, so de-assert it for DS3GI_RI_DEASSERT_INTERVAL sec.
      ---------------------------------------------------------------------*/
      ds3g_siolib_ri_deassert();
      duration = DS3G_SIOLIB_RI_DEASSERT_INTERVAL;

      /*---------------------------------------------------------------------
        Send the RING result code to the TE. The ring type is based on the
        type of call: voice, fax etc.
      ---------------------------------------------------------------------*/
      dsat_send_result_code( ring_type );

      /*---------------------------------------------------------------------
        If auto-answering is to be performed, then do so based on the value
        of S0. Note: It is assumed that auto-answer is only performed for
        data calls (and not for voice calls answered by the TE).
      ---------------------------------------------------------------------*/
      if( auto_answer )
      {
        if( dsat_s0_val != 0 )
        {
          if ( ++ds3g_siolib_state.ring_counter == dsat_s0_val ) 
          {
            if (ds3g_siolib_state.func_tbl->auto_answer_handler != NULL)
            {
              ds3g_siolib_state.func_tbl->auto_answer_handler();
              call_answered = TRUE;
            }
            else
            {
              ERR ( "auto answer handler not registered", 0, 0, 0 );
            }
          }
        } /* if S0 is non-zero */
      } /* if auto_answer */
    } /* if ri_asserted */
    else
    {
      /*---------------------------------------------------------------------
        RI is de-asserted, so assert it for DS3GI_RI_ASSERT_INTERVAL sec.
      ---------------------------------------------------------------------*/
      ds3g_siolib_ri_assert();
      duration = DS3G_SIOLIB_RI_ASSERT_INTERVAL;
    }

    if( call_answered )
    {
      /*---------------------------------------------------------------------
        Call was answered, so de-assert RI and reset the ring counter for the
        next call.
      ---------------------------------------------------------------------*/
      ds3g_siolib_ri_deassert();
      ds3g_siolib_state.ring_counter = 0;
    }
    else
    {
      /*---------------------------------------------------------------------
        Call was not answered, re-start the ring timer.
      ---------------------------------------------------------------------*/
      ds3gi_start_timer( DS3G_TIMER_RING_TE, duration );
    }
  }
} /* ds3g_alert_user() */


/*===========================================================================

FUNCTION DS3G_SIOLIB_STOP_ALERTING

DESCRIPTION
  This function stops alerting the user i.e. stops ringing the serial port.
  The RI signal is de-asserted and the ring timer is stopped.

  This function should be called when the user answers the call or if the
  call is released in the RINGING call state.

DEPENDENCIES
  This function does nothing if there is no serial port currently open.

RETURN VALUE
  None

SIDE EFFECTS
  Updates 3G Dsmgr state variables (resets the ring_counter to zero).

===========================================================================*/

void  ds3g_siolib_stop_alerting( void )
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( ds3g_siolib_state.serial_info.serial_state != DS3G_SIOLIB_NULL_SERIAL_STATE )
  {
    ds3g_siolib_ri_deassert();
    ds3g_siolib_state.ring_counter = 0;

    ds3gi_stop_timer( DS3G_TIMER_RING_TE );
  }
} /* ds3g_stop_alerting() */

#endif /* FEATURE_DATA */
