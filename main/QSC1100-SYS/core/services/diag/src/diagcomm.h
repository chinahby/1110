#ifndef DIAGCOMM_H 
#define DIAGCOMM_H
/*==========================================================================

              Diagnostic Communications Layer Header

General Description
  API declarations for the diagnostic protocol transport layer.
  
Copyright (c) 2000-2008 by QUALCOMM, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                           Edit History
                           
 $Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/diagcomm.h#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/16/07    pc     Added support for diag_dtr_enable and diag_dtr_changed_cb.
12/15/06    as     Fixed compiler warnings.
05/15/05    as     Added support for FEATURE_DATA_SERIALIZER
02/09/05   abu     Added support for Diag over IrDA under FEATURE_IRDA flag
12/14/04    sl     Added support for DIAG to hold multiple SIO ports
04/10/03    wx     Move qcdmg and to_data_mode functions from RDM to here.
10/24/01   jal     Added ability to flush transmit channel
04/06/01   lad     Introduces types for callback function pointers.
                   Moved ASYNC char definitions from diagi.h.
02/23/01   lad     Created file.

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "target.h"
#include "sio.h"
#ifdef FEATURE_DATA_SERIALIZER
#include "rdevmap.h"
#endif

/* ------------------------------------------------------------------------
** Types
** ------------------------------------------------------------------------ */
typedef void (*diagcomm_notify_fnc_ptr_type)( void );

typedef void (*diagcomm_tx_start_fnc_ptr_type)( void );

typedef void (*diagcomm_tx_stop_fnc_ptr_type)( void );

typedef void (*diagcomm_flush_cb_fnc_ptr_type)( void );

typedef void (*diagcomm_close_cb_fnc_ptr_type)( void );

/* Definitions for Half-Duplex Async-HDLC mode.
 * NOTE: Under most circumstances, the communications layer doesn't need to 
 * know about HDLC.  Diag handles this.  However, in the case of routing mode,
 * The communications layer needs to be able to determine the end of a packet
 * in order to forward packets.
 */
#define DIAG_ASYNC_CONTROL_CHAR 0x7E /* Flag character value.  Corresponds 
                                        to the ~ character                  */
#define DIAG_ASYNC_ESC_CHAR     0x7D /* Escape sequence 1st character value */
#define DIAG_ASYNC_ESC_MASK     0x20 /* Escape sequence complement value    */
#define DIAG_MIN_PKT_SIZE       3    /* Command code and CRC                */
#define DIAG_ESC_EXPAND_SIZE    2    /* Size of an expanded escape code     */
#define DIAG_ASYNC_TAIL_SIZE    3    /* CRC plus tail character             */
#define DIAG_FOOTER_SIZE        5    /* 2 byte CRC plus the possibility of 
                                        expanding per AHDLC protocol plus  
                                        the  ASYNC_FLAG                     */
#define DIAG_ESC_EXPAND_SHIFT 1      /* The number of bits to shift for
                                        expanded size due to escaping.      */

#ifdef FEATURE_DIAG_MULTI_PORTS
typedef enum
{
 PORT_NONE = -1,   /* No Port is assigned to DIAG */
 PORT_UART1,       /* Index to the multi_sio_stream_id[] for SIO_PORT_UART_MAIN */
 PORT_USB_DIAG,    /* Index to the multi_sio_stream_id[] for SIO_PORT_USB_DIAG */
 PORT_BT_SPP,      /* Index to the multi_sio_stream_id[] for SIO_PORT_BT_SPP */
#ifdef FEATURE_IRDA
#error code not present
#endif
 PORT_MAX          /* Maximum Stream ID Array index */
}diagcomm_sio_port_e_type;

/* Define port_open_mask bits to check the ports that have to be opened */
#define DIAGCOMM_PORT_UART1_BM        0x0001 /* Bit mask for SIO_PORT_UART_MAIN */
#define DIAGCOMM_PORT_USB_DIAG_BM     0x0002 /* Bit mask for SIO_PORT_USB_DIAG */
#define DIAGCOMM_PORT_BT_SPP_BM       0x0004 /* Bit mask for SIO_PORT_BT_SPP */
#define DIAGCOMM_PORT_IRCOMM_BM       0x0008 /* Bit mask for SIO_PORT_IRCOMM */
#endif

/* ------------------------------------------------------------------------
** Function Prototypes
** ------------------------------------------------------------------------ */

#ifdef __cplusplus
    extern "C" {
#endif

/*===========================================================================

FUNCTION DIAGCOMM_INIT

DESCRIPTION
  This function initializes the diagnostic transport layer.

===========================================================================*/
void diagcomm_init ( void );

/*===========================================================================

FUNCTION DIAGCOMM_REGISTER_INBOUND

DESCRIPTION
  This given callback will be called when inbound data is available from 
  the comm layer.

===========================================================================*/
void diagcomm_register_inbound( 
   diagcomm_notify_fnc_ptr_type inbound_pkt_cb
);

/*===========================================================================

FUNCTION DIAGCOMM_REGISTER_OUTBOUND_FLOW_CTRL

DESCRIPTION
  This function registers callbacks for software flow control for 
  the outbound link.

===========================================================================*/
void diagcomm_register_outbound_flow_ctrl ( 
   diagcomm_tx_start_fnc_ptr_type tx_start_cb,
   diagcomm_tx_stop_fnc_ptr_type  tx_stop_cb
);

/*===========================================================================

FUNCTION DIAGCOMM_OPEN

DESCRIPTION
  Opens the communications port.

===========================================================================*/
boolean diagcomm_open (void);

/*===========================================================================

FUNCTION DIAGCOMM_FLUSH_RX

DESCRIPTION
  Flushes the communications port's receive channel.

===========================================================================*/
void diagcomm_flush_rx (
  diagcomm_flush_cb_fnc_ptr_type flush_fp
);

/*===========================================================================

FUNCTION DIAGCOMM_FLUSH_TX

DESCRIPTION
  Flushes the communications port's transmit channel.

===========================================================================*/
void diagcomm_flush_tx (
  diagcomm_flush_cb_fnc_ptr_type flush_fp
);
/*===========================================================================

FUNCTION diag_dtr_enable

DESCRIPTION
  Invoked when Diag initializes. This registers a callback with SIO that is
  intended to report when there are DTR changes.

===========================================================================*/
void diag_dtr_enable(void);

/*===========================================================================

FUNCTION diag_dtr_changed_cb

DESCRIPTION
  Invoked when DTR changes. This is intended to flip a variable that causes
  Diag to vote/not vote for sleep based on connectivity status.

===========================================================================*/
void diag_dtr_changed_cb( void );

/*===========================================================================

FUNCTION DIAGCOMM_CLOSE

DESCRIPTION
  Closes the communications port.

===========================================================================*/
void diagcomm_close ( 
  diagcomm_close_cb_fnc_ptr_type close_func_ptr
);

/*===========================================================================

FUNCTION DIAGCOMM_STATUS

DESCRIPTION
  This function returns TRUE if the communications layer has an open port.

===========================================================================*/
boolean diagcomm_status (void);

/*===========================================================================

FUNCTION DIAGCOMM_OUTBOUND_ALLOC

DESCRIPTION
  This function requests a buffer from the diag communications layer.
  
===========================================================================*/
void * diagcomm_outbound_alloc ( 
  unsigned int req_length, /* # of bytes requested      */
  unsigned int *count      /* Number of bytes allocated */
);

/*===========================================================================

FUNCTION DIAGCOMM_SEND

DESCRIPTION
  This function sends the given given stream.  No framing is done by this 
  function.

===========================================================================*/
void diagcomm_send ( 
  void *ptr, 
  unsigned int length 
);

/*===========================================================================

FUNCTION DIAGCOMM_INBOUND

DESCRIPTION
  This function returns a pointer to some inbound data.  If no inbound 
  data is available, NULL is returned.

  This data stream is not packet delimited.  The stream may contain more 
  than one packet, or a parital packet.

===========================================================================*/
void * diagcomm_inbound ( 
  unsigned int *stream_len /* number of bytes in the stream (if non-NULL)  */
);

/*===========================================================================

FUNCTION DIAGCOMM_INBOUND_COMPLETE

DESCRIPTION
  This function tells the diag communications layer that the caller is done
  with the buffer returned by diagcomm_send() may now be released.

===========================================================================*/
void diagcomm_inbound_complete ( 
  void *ptr
);

/*===========================================================================
  
FUNCTION DIAGCOMM_SIO_QCDMG

DESCRIPTION
  Execute AT$QCDMG command. Diag will close its open port and grab DS's
  port.

  It returns TRUE if RDM accepts the port mapping; FALSE if RDM can't 
  change the port mapping.

===========================================================================*/
boolean diagcomm_sio_qcdmg 
(
#ifdef FEATURE_DATA_SERIALIZER
  rdm_device_enum_type port_id
#else
  void
#endif
); 


#ifdef FEATURE_DIAG_MULTI_PORTS

/*===========================================================================

FUNCTION DIAGCOMM_RESET_SIO

DESCRIPTION
  This function resets the diagcomm_sio_stream_id.

===========================================================================*/

void diagcomm_reset_sio (void);


/*===========================================================================

FUNCTION DIAGCOMM_GET_MULTI_SIO_STREAM_ID

DESCRIPTION
  This function returns the stream_id for the passed in port parameter.

===========================================================================*/

sio_stream_id_type diagcomm_get_multi_sio_stream_id (
    diagcomm_sio_port_e_type diagcomm_port
);


/*===========================================================================

FUNCTION DIAGCOMM_SET_SIO_STREAM_ID

DESCRIPTION
  This function sets the diagcomm_sio_stream_id to the stream_id of the
  port parameter passed in.

===========================================================================*/

void diagcomm_set_sio_stream_id (
    diagcomm_sio_port_e_type diagcomm_port
);

#endif /* FEATURE_DIAG_MULTI_PORTS */

#if !defined (DIAG_REQ_FWD)
/*===========================================================================

FUNCTION DIAGCOMM_GET_RX_WMQ_CNT

DESCRIPTION
  This function returns the rx watermark queue count.

===========================================================================*/
uint32 diagcomm_get_rx_wmq_cnt(void);
#endif

stream_id_type diagcomm_stream_id(void);

#ifdef __cplusplus
    }
#endif

#endif /* DIAGCOMM_H  */

