#ifndef SIORS232_MDD_H
#define SIORS232_MDD_H
/*===========================================================================

           S I O   D E V I C E   L A Y E R   M D D   H E A D E R

DESCRIPTION
   This is the header file for the common code used by the SIO UART driver. 

Copyright (c) 2008 QUALCOMM Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/uart/02.00.XX/siors232_mdd.h#2 $ $DateTime: 2009/02/11 17:20:36 $ $Author: cmihalik $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/21/07   rh      Add IOCTL for RX line state detection
09/24/07   rh      Add new IOCTL function to control TX BREAK event
08/10/07   rh      Init function now initialize all UART ports
07/30/07   rh      Featurize sleep support
02/23/07   rh      Created file.
===========================================================================*/

/*===========================================================================

                        FEATURE FLAGS RESOLUTION 

===========================================================================*/

/*===========================================================================
 
                  Include Files

===========================================================================*/

#include "sio.h"                             /* SIO Server header file     */
#include "clk.h"                             /* Clock services header file */
#include "customer.h"                        /* Customer feature files     */
#include "tramp.h"                 /* Trampoline typedefs and prototypes   */
#include "siors_lib.h"                 
#include "uart_compat.h"                 


/* Required to featurize this to deal with targets that has no power down  */
#ifdef FEATURE_UART_POWERDOWN
#include "sleep.h"
#else
#define sleep_okts_handle     uint32
#endif

/*===========================================================================

                        ENUMERATION TYPE

===========================================================================*/

/* There is a bit of issue letting this file coexist with siors232.h */

#ifndef SIORS232_H

/*---------------------------------------------------------------------------
  State variable to determine if a land to mobile FAX image is in progress
---------------------------------------------------------------------------*/
typedef enum 
{
  SIO_NOT_IN_FAX_IMAGE,
  SIO_IN_FAX_IMAGE,
  SIO_IN_FAX_IMAGE_DLE_FOUND
} sio_fax_image_partition_type;

/*---------------------------------------------------------------------------
 SIO Escape sequence state.  Used in the detection of '+++' escape sequence.
---------------------------------------------------------------------------*/

typedef enum
{
  SIO_ESC_SEQ_NULL,                /* no Escape sequencing in process      */
  SIO_ESC_SEQ_1ST_GUARDTIME,       /* 1st Guardtime occurred               */
  SIO_ESC_SEQ_1ST_CHAR,            /* Received 1st delay and 1st char      */
  SIO_ESC_SEQ_2ND_CHAR,            /* Received 2nd delay and 2nd char      */
  SIO_ESC_SEQ_3RD_CHAR,            /* Received 3rd delay and 3rd char      */
  SIO_ESC_SEQ_DETECTED,            /* Escape Sequence detected             */
  SIO_ESC_SEQ_MAX                  /* For Bounds checking only             */
} sio_esc_seq_enum_type;

/*---------------------------------------------------------------------------
 AT autodetect sequence state.  Used in the detection of AT commands.
---------------------------------------------------------------------------*/

typedef enum
{
  SIO_AD_INIT,                     /* Init state.  No chars received.           */
  SIO_AD_GOT_A,                    /* Got the 'A' part of the AT command.       */
  SIO_AD_GOT_AT,                   /* Got the 'AT' part of the AT command.      */
  SIO_AD_GOT_ASYNC_FLAG,           /* Got an SIO_SYNC_FLAG char after prepause. */  
  SIO_AD_DISCARD                   /* Got an invalid char.  We are discarding.  */

} sio_ad_state_type;

#endif /* SIORS232_H */

/*===========================================================================

                        DATA STRUCTURES 

===========================================================================*/

/*---------------------------------------------------------------------------
 Structure definition for MSM UART variables
---------------------------------------------------------------------------*/
typedef struct                /* This structure contain UART variables         */
{
                              /* Pointer to queue onto which incoming packets 
                               * should be placed.                             */
    dsm_watermark_type      *sio_rx_watermark_queue_ptr;
                              /* Function pointer which should be called with 
                               * incoming packet in lieu of enqueuing onto the 
                               * above queue.                                  */
    sio_rx_func_ptr_type    sio_rx_func_ptr;
                              /* Pointer to queue from which SIO will dequeue 
                               * packets to TX.                                */
    dsm_watermark_type      *sio_tx_watermark_queue_ptr;
                               /* The Current RX and TX bit rates              */
    sio_bitrate_type        sio_current_rx_bitrate;
    sio_bitrate_type        sio_current_tx_bitrate;
                               /* Identifier of stream which is currently using 
                                * SIO services.                                */
    sio_stream_id_type      sio_current_stream_id;
                               /* Mode of current stream.                      */
    sio_stream_mode_type    sio_current_stream_mode;
    sio_stream_type         sio_type_of_stream;
                               /* Indicates whether or not RX/TX stream has been 
                                * opened.  Notice that when USB support is 
                                * necessary, this will become a bit-map or 
                                * counter to reflect the multiple streams that 
                                * can use SIO simultaneously.                  */
    boolean                 sio_stream_open_rx;
    boolean                 sio_stream_open_tx;
                               /* Stream ID at open time                       */
    sio_stream_id_type      open_stream_id;   
                               /* Pointer to packet which contains the item which 
                                * is being transmitted.                        */
    dsm_item_type           *sio_tx_current_wm_item_ptr;
                               /* Pointer to buffer portion of 
                                * sio_tx_current_wm_item_ptr.  This is the actual 
                                * data that needs to be TX'ed.                 */
    byte                    *sio_current_tx_ptr;
                               /* This is the number of bytes which need to be 
                                * transmitted from current packet.             */
    int32                   sio_bytes_to_tx;
                               /* Pointer to function which will be called when 
                                * the last bit of packet waiting transmission 
                                * has been transmitted.                        */
    sio_vv_func_ptr_type    sio_stream_close_func_ptr;
                               /* Clock Call back item used in determining the 
                                * time at which all of the bytes that need to be 
                                * TX'ed have completely left SIO hardware.     */
    clk_cb_type             sio_stream_close_cb_item;
                               /* This boolean indicates whether or not SIO 
                                * needs to look for a tail character.          */
    boolean                 sio_tail_char_used;
                               /* This is the actual tail character for which SIO 
                                * needs to examine incoming stream.            */
    byte                    sio_tail_char;
                               /* Indicates whether or not SIO is in the middle 
                                * of switching streams (closing one stream and 
                                * opening another).                            */
    boolean                 sio_switch_streams;
                               /* Indicates whether or not the TX interrupt was 
                                * generated in order to check TX FIFO empty 
                                * condition.  TX FIFO Watermark is set to 0 in 
                                * order to check this condition.               */
    boolean                 sio_chk_empty_tx_fifo_condition;
    
    /* --------- Control and shadow Variables ---------------------------------*/
    boolean                 sio_uart_packet_mode;
                               /* RXLEV for data flow, at the given bit-rate.  */
    word                    sio_uart_flow_rx_rfr_lev;
                               /* TXLEV for data flow.                         */
    word                    sio_uart_flow_txlev;
                               /* Number of bytes that can be written to transmit 
                                * FIFO, if TXLEV interrupt happens.            */
    uint16                  sio_num_avail_tx_fifo_bytes;
                               /* Indicates to the sio_schdl_stream_chng if it 
                                * should assume that the stream is being closed, 
                                * or tx is merely being flushed.  
                                * (TRUE = closing, FALSE = flushing)           */
    boolean                 sio_closing_stream;
                               /* If to change the baudrate.   Changed once the 
                                * tx watermark and tx fifo empty               */
    boolean                 sio_changing_baudrate;
                               /* Baudrate to switch to when the user calls the 
                                * ioctl used to change baudrate                */
    sio_bitrate_type        sio_new_bitrate;
    /* --------- Variables used in '+++' escape code detection. ---------------*/
                               /* State associated with escape code detection. */
    sio_esc_seq_enum_type   sio_esc_seq_state;
                               /* Clock call back structure which is used in 
                                * escape code detection.                       */
    clk_cb_type             sio_esc_seq_cb_item;
                               /* Pointer to function which will be called when 
                                * escape code is detected.                     */
    sio_vv_func_ptr_type    sio_escape_code_func_ptr;
                               /* Pointer to function which will be called when 
                                * packet is detected.                          */
    /* --------- Variables used in packet detection. --------------------------*/
                               /* Pointer to function which will be called when 
                                * packet is detected.                          */
    sio_vv_func_ptr_type    sio_packet_func_ptr;
                               /* Boolean indicating, if TRUE, that timer for 
                                * byte following PPP flag is scheduled but has 
                                * not happened yet. FALSE, timer has expired.  */
    boolean                 sio_packet_detection_active;
    /* --------- Call back function pointers ----------------------------------*/
                               /* Pointer to task context block which is used in 
                                * signalling task when DTR changes state.  DTR/ 
                                * pin allows dedication of GPIO_INT pin to the 
                                * purpose.                                     */
    void                    (*siors_dtr_func_ptr)(void);
    /* --------- Power down related variable ----------------------------------*/
                               /* Count the number of times the no cable check
                                * is performed.  If the value is zero,
                                * power down the port                          */
    byte                    check_no_cable_cnt;
                               /* Store the current sleep handle used          */
    sleep_okts_handle       sleep_handle;
                               /* True if the UART can be wake from sleep with
                                * signal on RX line                            */
    boolean                 can_wake_on_rx_event;
                               /* Store the callback registered when RX
                                * activity check is required                   */
    sio_vv_func_ptr_type    rx_activity_cb_func_ptr;
} sio_mdd_variable_type;


typedef struct              /* Contains variables related to autodetect         */
{
                            /* Current state of AD processing.  If no chars
                             * have been received, state is INIT.               */
    sio_ad_state_type   state;
                            /* Semaphores for Autodetect mode valid Packet 
                             * detection.                                       */
    boolean             packetpause_occurred;
                            /* Count of rx characters while in discard state.   */
    word                rx_cnt;
                            /* Clock callbacks struct used for the serial RX 
                             * line pause checking                              */
    clk_cb_type         pausecheck_cb_item;
    clk_cb_type         packetpause_cb_item;
                            /* Stream IDs for Autodetect and 'in a data call'   */
    sio_stream_id_type  autodetect_stream_id;
    sio_stream_id_type  ds_incall_stream_id;
    uart_device_id_type autodetect_device_id;
    uart_device_id_type ds_incall_device_id;

                            /* Mode indicator                                   */
    boolean             is_in_autodetect_mode;
    boolean             is_in_ds_call_mode;
} sio_mdd_autodetect_var_type;

/*---------------------------------------------------------------------------
 Variables used in flow control.  Most of these are used for Software
 flow control, but some are used for hardware flow control as well.

 There are two types of software flow control, failsafe and non-failsafe.
 In non-failsafe XON/XOFF flow-control, an XOFF character is not sent unless
 an XON character has already been sent.  Starting communication at the very
 beginning is taken to be tantamount to sending an XON character, given a
 dormant port.

 In failsafe flow-control, both XON and XOFF characters may be re-transmitted
 multiple times, if the other side does not seem to have gotten or does not
 seem to have heeded the control character.

 In the transmission part of XON/XOFF flow control, the UART's transmission
 capability is disabled, when an XOFF is received and re-enabled when
 an XON is received.  Transmission of XON/XOFF characters is allowed,
 however.
---------------------------------------------------------------------------*/
typedef struct              /* This structure contain Flow control variables   */
{
                            /* Serial RX and TX flow control variables set to 
                             * the default method (HW)                         */
    sio_flow_ctl_type     sio_tx_flow_control_method;
    sio_flow_ctl_type     sio_rx_flow_control_method;

    boolean               sio_rx_software_flow_control;
                            /* Record of whether or not TX flow control is 
                             * software flow control.                          */
    boolean               sio_tx_software_flow_control;
                            /* Fail-safe software flow control indicator.  
                             * Default is off.                                 */
    boolean               sio_sw_fc_fail_safe;
                            /* Fail-safe software flow control tx char counter */
    int32                 sio_sw_fc_fail_safe_ctr;
                            /* Indicates whether a posted software flow control 
                             * character has been TX'd                         */
    boolean               sio_sw_fc_ctlchr_tx_pending;
                            /* Indicates whether or not pending software flow 
                             * control character TX is fail-safe transmission. */
    boolean               sio_sw_fc_pending_tx_fail_safe;
                            /* Indicates whether serial outbound flow is enabled*/
    boolean               sio_outbound_flow_disabled;
                            /* In-bound flow control state                     */
    boolean               sio_inbound_flow_enabled;
                            /* This variable indicates whether or not incoming 
                             * flow control characters should be stripped from 
                             * stream sent to caller (task or ISR).            */
    boolean               sio_tx_sw_fc_strip;
    word                  sio_uart_flow_rxlev;
} sio_mdd_flowctrl_var_type;


typedef struct              /* The port_type structure identify a PORT     */
{
    uart_device_id_type     device_id; /* device id (minor number)         */
    sio_mdd_variable_type   gv;        /* General variables                */
    sio_mdd_flowctrl_var_type fv;      /* Flow control variables           */
    sio_mdd_autodetect_var_type adv;   /* Autodetect variables             */
    sio_pdd_api_type        fp;        /* APIs available from PDD layer    */
    boolean                 can_autodetect; /* Can this port autodetect?   */
    boolean                 can_use_data;   /* Can this port use data mode?*/
    boolean                 is_clock_enabled; /* Is the clock ON?          */
    void *pPortHead;           /* Points to the driver structure head */
} sio_mdd_port_type;


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/*===========================================================================
 FUNCTION/PROCEDURE prototypes.
===========================================================================*/              

/*===========================================================================

FUNCTION SIO_RS232_OPEN_STREAM

DESCRIPTION
  This function allocates stream as indicated in open_ptr.
    
DEPENDENCIES
  None
  
RETURN VALUE
  SIO_DONE_S: TX was successfully allocated.
  SIO_UNAVAIL_S: TX could not be allocated.  
  
SIDE EFFECTS
  None
  
===========================================================================*/

extern sio_status_type siors_mdd_api_open_stream
(
  sio_open_type     *open_ptr
);


/*===========================================================================

FUNCTION SIO_RS232_CLOSE_STREAM

DESCRIPTION
  This function deallocates given stream.
    
DEPENDENCIES
  None
  
RETURN VALUE
  None
   
SIDE EFFECTS
  None
  
===========================================================================*/

extern void siors_mdd_api_close_stream
(
  sio_stream_id_type stream_id,             /* Stream ID                   */
  void             (*close_func_ptr)(void)  /* Function to call when 
                                               pending transmission is 
                                               complete                    */
);


/*===========================================================================

FUNCTION SIO_RS232_FLUSH_TX

DESCRIPTION
  This function flushes the TX.

DEPENDENCIES
 User should not queue more data until after the registered callback
 is executed.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void siors_mdd_api_flush_tx
(
  sio_stream_id_type stream_id,             /* Stream ID                   */
  sio_port_id_type   port_id,               /* Port ID                     */
  void             (*flush_func_ptr)(void)  /* Function to call when 
                                               pending transmission is 
                                               complete                    */
);


/*===========================================================================

FUNCTION SIO_RS232_DEV_INIT

DESCRIPTION
  Initialize the MSM UART serial interface for operation and initialize 
  data structures.

DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
extern void siors_mdd_api_dev_init (void);


/*===========================================================================

FUNCTION SIO_RS232_HW_TRANSMIT                   

DESCRIPTION
  This function transmits over MSMs RS-232 UART.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void siors_mdd_api_dev_transmit
(
  sio_stream_id_type stream_id,           /* Stream ID                     */
  sio_port_id_type   port_id,             /* SIO Port ID                   */
  dsm_item_type     *tx_ptr               /* Packet for transmission       */
);


/*===========================================================================

FUNCTION SIO_RS232_IOCTL                                EXTERNALIZED FUNCTION

DESCRIPTION
  Allows control of open stream, given that the device is an RS232 serial
  device.
    
DEPENDENCIES
  None
                                                  
RETURN VALUE
  None
    
SIDE EFFECTS

===========================================================================*/
void siors_mdd_api_ioctl
(
  sio_stream_id_type    stream_id,             /* Stream ID                */
  sio_port_id_type      port_id,               /* SIO Port ID              */
  sio_ioctl_cmd_type    cmd,                   /* Command                  */
  sio_ioctl_param_type *param                  /* Parameter                */
);


/*===========================================================================

FUNCTION SIO_RS232_DISABLE_DEVICE                       EXTERNALIZED FUNCTION

DESCRIPTION
  This routine disables given device, including turning off clock to save
  power.  This is different from UART powerdown feature which attempts to
  save power with minimal loss of data.  Instead, this function is meant 
  for customers who may not want to use both UARTs.
  
  Effects of this function may be reversed by calling sio_rs232_dev_init.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void siors_mdd_api_disable_device
(
  sio_port_id_type   port_id              /* SIO Port ID                   */  
);


#endif /* SIORS232_MDD_H */

