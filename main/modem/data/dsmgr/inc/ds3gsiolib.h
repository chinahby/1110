#ifndef DS3GSIOLIB_H
#define DS3GSIOLIB_H
/*===========================================================================

               3 G   D A T A   S E R V I C E S   M A N A G E R

                S I O   I N T E R F A C E   F U N C T I O N S

                           H E A D E R    F I L E

DESCRIPTION
  This file defines services provided by SIOLIB to other Data Services
  software units, to perform serial port operations on the port assigned to
  Data Services. This includes setting and/or checking the state of various
  serial port control signals, configuring and performing flow control,
  configuring the serial baud rate and setting up data transmission.

  Note that Data Services software units other than SIOLIB should NOT use
  the SIO API directly, but should use the functions provided here instead.
  Serial port operations not defined here, such as opening or closing a port,
  are performed only by SIOLIB.

  Copyright (c) 2001 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3gsiolib.h_v   1.4   19 Dec 2002 17:36:26   ajithp  $
  $Header: //source/qcom/qct/modem/data/common/dsmgr/main/lite/inc/ds3gsiolib.h#1 $ $DateTime: 2008/04/11 12:46:53 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/04/06   squ     Fixed a compile warning
11/01/06   an      Featurize ATCOP, RMSM, Async Fax
12/19/02   atp     Removed deprecated functions inbound_flow_enable/disable()
05/08/02   rsl     Added siolib functionality.  
02/19/02   rlp     Code review changes.
10/04/01   smp     Added ds3g_siolib_is_rts_asserted() function.
06/07/01   smp     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA
#include "dsat.h"

#ifndef FEATURE_DATA_STRIP_ATCOP
#include "sio.h"
#include "dsm.h"
#include "ds_flow_control.h"



/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  Serial port status returned by the functions exported in this file.
---------------------------------------------------------------------------*/  
typedef enum
{
  DS3G_SIOLIB_OK,                            /* Serial port is currently open */
  DS3G_SIOLIB_NO_PORT_OPEN                   /* No serial port currently open */
} ds3g_siolib_status_e_type;

/*---------------------------------------------------------------------------
  Events that impact the Carrier Detect (CD) state.
---------------------------------------------------------------------------*/
typedef enum
{
  DS3G_SIOLIB_REMOTE_CARRIER_ON,                      /* Remote carrier is ON */
  DS3G_SIOLIB_REMOTE_CARRIER_OFF,                    /* Remote carrier is OFF */
  DS3G_SIOLIB_CD_WINKING_END,                   /* End of CD winking interval */
  DS3G_SIOLIB_SMS_MSG_START,                    /* Start of SMS message entry */
  DS3G_SIOLIB_SMS_MSG_END,                        /* End of SMS message entry */
  DS3G_SIOLIB_EVENT_NONE                                          /* No event */
} ds3g_siolib_dcd_event_e_type;

/*---------------------------------------------------------------------------
  Serial Port States. Note that any non-null state implies that a
  serial port is assigned to Data Services.
---------------------------------------------------------------------------*/ 
typedef enum
{
  DS3G_SIOLIB_NULL_SERIAL_STATE,      /* No serial port assigned to data services */
  DS3G_SIOLIB_AUTODETECT_SERIAL_STATE,          /* Serial port in Autodetect Mode */
  DS3G_SIOLIB_RAWDATA_SERIAL_STATE,                /* Serial port in Rawdata Mode */
  DS3G_SIOLIB_PACKET_SERIAL_STATE,                  /* Serial port in Packet Mode */
  DS3G_SIOLIB_WAIT_TX_FLUSH_SERIAL_STATE      /* Wait for TX buffer to be flushed */
} ds3g_siolib_serial_state_e_type;


/*---------------------------------------------------------------------------
  SIOLIB AT Command Processing States
---------------------------------------------------------------------------*/ 
typedef enum
{
  DS3G_SIOLIB_NULL_AT_STATE,     /* ATCoP not active i.e. no serial port assigned */
  DS3G_SIOLIB_COMMAND_AT_STATE,    /* No data call active, listen for AT commands */
  DS3G_SIOLIB_ONLINE_DATA_AT_STATE,/* Data call active, rx'ed bytes are user data */
  DS3G_SIOLIB_ONLINE_CMD_AT_STATE     /* Data call active, listen for AT commands */
} ds3g_siolib_at_state_e_type;

/*---------------------------------------------------------------------------
  Type for SIOLIB serial port related information
---------------------------------------------------------------------------*/
typedef struct
{
  ds3g_siolib_serial_state_e_type    serial_state;
  dsm_watermark_type          *rx_wm_ptr;
  dsm_watermark_type          *tx_wm_ptr;
  boolean                      ri_asserted;   /* Ring Indicator (RI) state */
} ds3g_siolib_serial_info_type;

/*---------------------------------------------------------------------------
  Variable to store SIO RX mode-specific handler watermark
---------------------------------------------------------------------------*/ 
extern dsm_watermark_type *ds3g_siolib_mode_hdlr_from_sio_wm_ptr;

/*---------------------------------------------------------------------------
  Variable to store SIO TX mode-specific handler watermark
---------------------------------------------------------------------------*/ 
extern dsm_watermark_type *ds3g_siolib_mode_hdlr_to_sio_wm_ptr;

/*---------------------------------------------------------------------------
  Variable to store SIO RX mode-specific handler function pointer
---------------------------------------------------------------------------*/ 
extern sio_rx_func_ptr_type ds3g_siolib_mode_hdlr_sio_rx_func_ptr;

/*---------------------------------------------------------------------------
  Each mode-specific handler maintains such a table, and registers this
  table with SIOLIB.
---------------------------------------------------------------------------*/
typedef struct
{
/*-------------------------------------------------------------------------
    This handler is invoked when the AT escape sequence is detected, in
    online data mode. The return value indicates whether online command mode
    should be entered (in this case, data transfer over the air should be
    suspended), or if we should stay in online data mode.
  -------------------------------------------------------------------------*/
  dsat_mode_enum_type (*at_escape_sequence_handler)( void );

  /*-------------------------------------------------------------------------
    This handler is invoked when the DTR signal gets asserted or de-asserted.
  -------------------------------------------------------------------------*/
  void (*dtr_changed_sig_handler)( boolean dtr_status );

  /*-------------------------------------------------------------------------
    This handler is invoked when the DTR signal gets asserted or de-asserted
	while AT&D = 0.
  -------------------------------------------------------------------------*/
  void (*dtr_changed_sig_handler_ampd0)( boolean dtr_status );

  void (*at_return_to_online_data_handler)(
                         dsm_watermark_type     **from_sio_wm_ptr,
                         dsm_watermark_type     **to_sio_wm_ptr,
                         sio_rx_func_ptr_type    *sio_rx_func_ptr);

  /*-------------------------------------------------------------------------
    This handler is invoked when the ATO command (return to online data mode)
    is rx'ed in online command mode. The mode-specific handler should resume
    data transfer over the air.
  -------------------------------------------------------------------------*/
  void (*at_return_to_online_data_complete_handler)( void );

  /*-------------------------------------------------------------------------
    This handler is invoked when the DS_ENTER_ONLINE_CMD_* mode transition
    completes (i.e. SIO has been placed in online-command mode).  The mode
    specific handler can be used to notify the initiating task that the
    mode transition has completed.  The handler is only called when exiting
    online-data mode and returning to online-command mode.
  -------------------------------------------------------------------------*/
  void (*return_to_online_cmd_mode_complete_handler)( void );

  /*-------------------------------------------------------------------------
    This handler will be called when auto-answering is to be performed based
    on the value of ATS0.
  -------------------------------------------------------------------------*/
  void (*auto_answer_handler)( void );

} ds3g_siolib_func_tbl_type;

/*---------------------------------------------------------------------------
  SIOLIB state type.
---------------------------------------------------------------------------*/ 

typedef struct
{
  ds3g_siolib_serial_info_type  serial_info;   /* Serial port related info */
  ds3g_siolib_at_state_e_type   at_state;      /* AT command processing state */
  ds3g_siolib_func_tbl_type     *func_tbl;
  boolean                       call_coming_up;
  dsat_result_enum_type         ring_type;     /* Ring result code to send */
  uint8                         ring_counter;  /* Counts the number of rings */
} ds3g_siolib_state_info_type;

/*---------------------------------------------------------------------------
  SIOLIB state variable.
---------------------------------------------------------------------------*/ 
extern ds3g_siolib_state_info_type ds3g_siolib_state;

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

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

rex_sigs_type ds3g_siolib_init ( void );

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
  Returns DS3G_SIOLIB_OK if the open was successful, or DS3G_SIOLIB_NO_PORT_OPEN
  if the open was unsuccessful.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_status_e_type  ds3g_siolib_open
(
  sio_port_id_type      port_id,       /* Port id of the port to be opened */
  dsm_watermark_type   *rx_wm_ptr,       /* Watermark for rx data from SIO */
  dsm_watermark_type   *tx_wm_ptr          /* Watermark for tx data to SIO */
);


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

extern void  ds3g_siolib_close( void );

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

extern void  ds3g_siolib_ioctl
( 
  sio_ioctl_cmd_type cmd,
  sio_ioctl_param_type *param 
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_IS_BAUD_VALID                             EXTERNALIZED FUNCTION

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

extern boolean ds3g_siolib_is_baud_valid
(
  sio_bitrate_type   check_baud              /* SIO baud rate */
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_CHANGE_MODE

DESCRIPTION
  This function changes the mode of the currently open serial port to the
  specified mode. After the mode change, received data bytes will be stored
  in the RX watermark specified by the caller, and the rx_func will be called
  each time data is enqueued to the watermark. One of these (rx_wm_ptr or
  rx_func_ptr) may be NULL. Also, data bytes pending transmission will be
  stored in the TX watermark specified by the caller.

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

extern ds3g_siolib_status_e_type  ds3g_siolib_change_mode
(
  sio_stream_mode_type  stream_mode,                    /* New stream mode */
  dsm_watermark_type   *rx_wm_ptr,       /* Watermark for rx data from SIO */
  dsm_watermark_type   *tx_wm_ptr,         /* Watermark for tx data to SIO */
  sio_rx_func_ptr_type  rx_func_ptr   /* Called each time data bytes rx'ed */
);


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

extern ds3g_siolib_status_e_type  ds3g_siolib_cd_assert( void );


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

extern ds3g_siolib_status_e_type  ds3g_siolib_cd_deassert( void );


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

extern ds3g_siolib_status_e_type  ds3g_siolib_ri_assert( void );


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

extern ds3g_siolib_status_e_type  ds3g_siolib_ri_deassert( void );

/*===========================================================================
FUNCTION DS3G_SIOLIB_SET_CALL_COMING_UP

DESCRIPTION


DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/

extern void  ds3g_siolib_set_call_coming_up
(
    boolean call_coming_up
);

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
  if the passed in function pointer is not NULL, otherwise returns FALSE;

SIDE EFFECTS
  If a mode-specific handler function table was previously registered, 
  it will be over-written.
===========================================================================*/

extern boolean  ds3g_siolib_register_callback_func
(
  ds3g_siolib_func_tbl_type  *func_tbl_ptr        /* Handler function table */
);

/*===========================================================================
FUNCTION DS3G_SIOLIB_DEREGISTER_CALLBACK_FUNC

DESCRIPTION
  This function deregisters a mode-specific handler function table.

DEPENDENCIES
  None

RETURN VALUE
  Returns TRUE if deregister happened successfully. 
  Returns FALSE if func ptr passed in does not match siolib table value.

SIDE EFFECTS
  Resets the callback function table pointer to null. 
===========================================================================*/

extern boolean ds3g_siolib_deregister_callback_func 
( 
  ds3g_siolib_func_tbl_type *func_tbl_ptr
);



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

extern ds3g_siolib_status_e_type  ds3g_siolib_dsr_assert( void );


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

extern ds3g_siolib_status_e_type  ds3g_siolib_dsr_deassert( void );


/*===========================================================================

FUNCTION DS3G_SIOLIB_IS_DTR_ASSERTED

DESCRIPTION
  Checks the state of the Data Terminal Ready (DTR) signal, and returns a
  value (by means of a passed-by-reference parameter) indicating whether or
  not DTR is asserted.

DEPENDENCIES
  If there is no port currently open, the dtr_status value has no meaning.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_status_e_type  ds3g_siolib_is_dtr_asserted
(
  boolean  *dtr_status     /* Set to TRUE if DTR asserted, or FALSE if not */
);


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

extern ds3g_siolib_status_e_type  ds3g_siolib_is_rts_asserted
(
  boolean  *rts_status     /* Set to TRUE if RTS asserted, or FALSE if not */
);


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

extern ds3g_siolib_status_e_type  ds3g_siolib_set_flow_ctl_method
(
  sio_flow_ctl_type  tx_flow,                    /* TX flow control method */
  sio_flow_ctl_type  rx_flow                     /* RX flow control method */
);


/*===========================================================================

FUNCTION DS3G_SIOLIB_SETUP_TX

DESCRIPTION
  This function queues data for transmission over the serial interface.
  Additionally, Autodetect Mode is re-armed, if specified by the caller.
  Typically, the re-arm feature is used only by ATCoP.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_status_e_type  ds3g_siolib_setup_tx
(
  dsm_item_type  *tx_ptr,         /* DSM item containing data to transmit */
  boolean         rearm_autodetect       /* TRUE = Re-arm Autodetect Mode */
);


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

extern ds3g_siolib_status_e_type ds3g_siolib_change_baud_rate
(
  sio_bitrate_type  baud_rate,             /* New baud rate                */
  boolean           chg_now                /* Flag for immediate change    */  
);


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
  2) Remote carrier state (only if &C1)
  3) End-of-call winking (only if &C2)
  4) SMS message entry
       CD is ON during SMS message entry (overrides the &C setting)

  This function should be called when a serial port is opened, and whenever
  any event occurs that impacts the CD state. Use the NO_EVENT event when
  a serial port is opened, or when the &C parameter is changed.

DEPENDENCIES
  If there is no port currently open, the state of the event passed in (if
  any) will be remembered, but the CD state will not be changed.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_status_e_type  ds3g_siolib_set_cd_state
(
  ds3g_siolib_dcd_event_e_type  dcd_event              /* Event that occurred */
);


/*===========================================================================

FUNCTION DS3G_SIOLIB_REARM_AUTODETECT

DESCRIPTION
  This function re-arms SIO Autodetect Mode. Typically, the re-arm
  feature is used only by ATCoP.

DEPENDENCIES
  No action is taken if there is no port currently open.

RETURN VALUE
  Returns DS3G_SIOLIB_OK if a serial port is open, or DS3G_SIOLIB_NO_PORT_OPEN
  otherwise.

SIDE EFFECTS
  None

===========================================================================*/

ds3g_siolib_status_e_type  ds3g_siolib_rearm_autodetect( void );


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

extern ds3g_siolib_status_e_type  ds3g_siolib_flush_tx
( 
  void (*flush_func_ptr_cb)(void)
);

//rsl
/*===========================================================================

FUNCTION DS3G_SIOLIB_RDM_INIT

DESCRIPTION
  This function performs registration with RDM to use port mapping 
  services.  It registers open and close serial device functions with
  RDM.  It also initializes SIOLIB state information and sets up
  autodetect watermark items, both of which are required by the open
  and close serial device functions.

  This function must be called once at data services task startup.

DEPENDENCIES
  Must be called before TMC function to start task.

RETURN VALUE
  None

SIDE EFFECTS
  Initializes state variables required by RDM serial device functions.

===========================================================================*/

void ds3g_siolib_rdm_init( void );


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

extern void  ds3g_siolib_process_signals
(
  rex_sigs_type  set_sigs         /* Mask containing signals that were set */
);

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

extern void  ds3g_siolib_process_cmds
(
  ds_cmd_type  *cmd_ptr                         /* Command to be processed */
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_CLEANUP_AUTODETECT_RX_WATERMARK

DESCRIPTION
  This function cleans up the Autodetect watermark used for RX data from SIO.
  Any data stored in the watermark is discarded.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void  ds3g_siolib_cleanup_autodetect_rx_watermark( void );

/*===========================================================================

FUNCTION DS3GI_CONVERT_SERIAL_MODE

DESCRIPTION
  This function converts the specified SIO serial mode into the corresponding
  SIOLIB serial state.

DEPENDENCIES
  None

RETURN VALUE
  Returns the SIOLIB serial state corresponding to the specified SIO serial
  mode.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3g_siolib_serial_state_e_type  ds3gi_convert_serial_mode
(
  sio_stream_mode_type  sio_serial_mode                 /* SIO serial mode */
);

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
  uint32 in_mask,           /* input mask of calling function */
  ds3g_flow_e_type flow  /* specifies if flow is to be enabled or disabled */
);

/*===========================================================================

FUNCTION DS3G_SIOLIB_ALERT_USER

DESCRIPTION
  This function rings the serial port to alert the user/TE of an incoming
  data or voice call. The RI signal is toggled, and the RING result code is
  sent to the TE. Auto-answer is also performed, if requested by the caller
  (and if enabled by the S0 s-register).

  This function should be called to start alerting, and subsequently, each
  time the ring timer expires. This function may be invoked either by 3G
  Dsmgr (for data calls) or ATCoP (for voice calls).

  Note that the auto-answer facility is intended to be used for data calls
  only.

DEPENDENCIES
  This function does nothing if there is no serial port currently open.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

extern void  ds3g_siolib_alert_user
(
  dsat_result_enum_type  ring_type,     /* Ring result code to send to TE */
  boolean                auto_answer       /* TRUE => Perform auto-answer */
);


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

extern void  ds3g_siolib_stop_alerting( void );
#else
#include "dsatprofile.h"
#endif /* FEATURE_DATA_STRIP_ATCOP */
#endif //FEATURE_DATA

#endif /* DS3GSIOLIB_H */
