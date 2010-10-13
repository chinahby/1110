/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostic Communications via SIO

General Description
  The routines in this file handle the communications layer for the
  diagnostic subsystem when communicating via Socket layers.

Copyright (c) 2001-2008 by QUALCOMM, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                               Edit History

 $PVCSPath:  L:/src/asw/MSM5200/common/vcs/diagcomm_sio.c_v   1.10   08 May 2002 14:37:42   ldefauw  $
 $Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/diagcomm_sio.c#8 $ $DateTime: 2008/10/09 13:38:56 $ $Author: pratapc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/10/08   pc      Added support for diag_dtr_enable and diag_dtr_changed_cb.
10/31/05   as      Fixed lint errors.
09/09/05   sl      Added support for multiple tx/rx queues for the case of
                   FEATURE_DIAG_MULTI_PORTS
05/15/05   as      Added support for FEATURE_DATA_SERIALIZER
03/28/05   sl      Added an array of watermark queues for the multi port feature
03/24/05   as      Added check in UART/USB notify functions to check if the
                   connected accessory detected by AAM is UART and USB
03/05/05   sl      Corrected BT Featurization for FEATURE_DIAG_MULTI_PORTS
02/20/05   sl      Added support for DIAG over BT via RDM under
                   FEATURE_DIAG_MULTI_PORTS
02/12/05   sl      Fixed a bug that caused the close/open port bit masks
                   not being cleared
02/09/05   abu     Added support for Diag over IrDA under FEATURE_IRDA flag  
01/25/04   sl      Added support for callback notifications from Accessory
                   Attachment Manager (AAM) for USB and UART cable attach and
                   detach under FEATURE_ACCESSORY_ATTACHMENT_MANAGER.
12/14/04   sl      Added support for DIAG to hold multiple SIO ports
                   controlled by FEATURE_DIAG_MULTI_PORTS
04/20/04   pn      Added support for DIAG over Bluetooth.
10/28/03   as      Changes to fix errors when compiling with RVCT2.0
09/29/03   dna     Get T_QUASAR to compile and work again for MSM6500
08/14/03   vsk     DSM 2.0 changes 
04/10/03    wx     Move qcdmg and to_data_mode functions from RDM to here.
05/08/02   lad     Moved dsm_simple_enqueue_isr() call above diagcomm_sio_cb
                   to enqueue data prior to potential context switch in cb.
02/27/02   dwp     Additional changes to use new DSM interface functions for
                   registering/handling memory events.
02/19/02   lad     Made changes to use new DSM interface functions for
                   registering/handling memory events.
01/21/02   as      Changed nv_sio_baud_type to nv_sio_baudrate_type
11/06/01   abp     Added diagcomm_flush_tx() to flush SIO transmit channel
10/30/01   abp     Added diagcomm_sio_close_to_open_cb().
                   In diagcomm_sio_close_done_cb() callback function ptr is
                   checked for NULL.
                   In diagcomm_open() added mechanism to close connection
                   before opening a new connection
10/09/01   ttl     Added second parameter to diagcomm_sio_tx_start_cb() and
                   diagcomm_sio_tx_stop_cb() due the DSM memory pool registration
                   changes.
09/17/01   jal     Fixed diagcomm_close() to notify RDM of completion
                   properly
05/07/01   lad     Cleaned up file.
04/17/01   dwp     Initialize diagcomm_sio_open_params.port_id to NULL. RDM
                   will instruct DIAG as to what port to open.
04/06/01   lad     Introduced types for callback function pointers.
                   Cleaned up FEATURE_RUNTIME_DEVMAP support.
                   Updated watermark settings (this may be changed to save RAM).
02/23/01   lad     Created file.

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "target.h"

#include "assert.h"

#include "diag.h"
#include "diagbuf.h"
#include "diagcmd.h"
#include "diagcomm.h"
#include "diagi.h"
#include "diagpkt.h"
#include "msg.h"

#include "dsm.h"

#if defined (FEATURE_DIAG_NV)
#include "diagnv.h"
#include "nv.h"
#endif

#if defined (FEATURE_RUNTIME_DEVMAP)
#include "rdevmap.h"
#endif

#if defined (FEATURE_SERIAL_DEVICE_MAPPER) && !defined (FEATURE_RUNTIME_DEVMAP)
#include "sdevmap.h"
#endif

#include "sio.h"
#include "task.h"

#if (defined(FEATURE_BT) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
#include "bt.h"
#include "btsd.h"
#endif

#if (defined(FEATURE_IRDA) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
#error code not present
#endif

#if defined (FEATURE_DIAG_MULTI_PORTS) && defined (FEATURE_ACCESSORY_ATTACHMENT_MANAGER)
#error code not present
#endif

/*lint -save -e751 */
/* Supress local typedef 'diag_config_comm_enum_type' not referenced */


/* -------------------------------------------------------------------------
** Definitions and Declarataions
** ------------------------------------------------------------------------- */

/*  Debugging support for this file             */
#ifdef DEBUG_DIAG
#error code not present
#else
#define DEBUG( x )
#endif /* DEBUG_RDM */

static boolean diag_channel_connected = TRUE;

#ifdef FEATURE_RUNTIME_DEVMAP

/* $QDCMG can let DIAG grab DS's port and QXDM's cmd to_data_mode can change
   them back. The port change is called port dancing. Here is the different 
   state of port dancing
*/
typedef enum {
  DIAGCOMM_DANCING_IDLE,        /* idle state                           */
  DIAGCOMM_DANCING_IN_DANCE,    /* in dancing, $QCDMG has succeeded     */
  DIAGCOMM_DANCING_QCDMG_OPENING,
                                /* DIAG has asked RDM to open DS's port 
                                   for $QCDMG                           */
  DIAGCOMM_DANCING_TDM_OPENING_DS,
                                /* DIAG has asked RDM to open DS's 
                                   port for to_data_mode                */
  DIAGCOMM_DANCING_TDM_OPENING_DIAG
                                /* DIAG has asked RDM to open its 
                                   old port for to_data_mode            */
} diagcomm_dancing_state_e_type;

/* Type of dancing state with useful infos                              */
typedef struct {
  diagcomm_dancing_state_e_type  state; /* dancing state                */
  rdm_device_enum_type  ds_old_dev;     /* DS's old port before dance   */
  rdm_device_enum_type  diag_old_dev;   /* DIAG's old port before dance */
} diagcomm_dancing_state_type;

/* contains rdm_assign_port_cb parameter        */
typedef struct {
  /* if the numbers are valid now               */
  boolean                       valid;

  /* status of rdm_assign_port                  */
  rdm_assign_status_type        status;

  /* the service requesting the assignment       */
  rdm_service_enum_type         srv_requester;

  /* the desired port for the srv_requester     */
  rdm_device_enum_type          dev_desired;
} diagcomm_dancing_assign_port_cb_param_type;

#endif /* FEATURE_RUNTIME_DEVMAP */

/* SIO stream ID */
static sio_stream_id_type diagcomm_sio_stream_id = SIO_NO_STREAM_ID;

/* If FEATURE_DIAG_MULT_PORTS is defined, DIAG needs to opem multiple ports.
   Instead of a single diagcomm_sio_stream_id, DIAG will now have
   multiple stream_ids, each corresponding to a different port for
   USB, BT, RS232 and IrDA device. 
   
   If FEATURE_ACCESSORY_ATTACHMENT_MANAGER is defined, DIAG can register
   for notifications on USB and UART cable attach and detach. On detecting
   that a USB or UART cable has been detached, DIAG can then reset the
   active stream_id.
*/

#ifdef FEATURE_DIAG_MULTI_PORTS
diagcomm_sio_port_e_type diagcomm_active_port;
static sio_stream_id_type diagcomm_multi_sio_stream_id[PORT_MAX];
word diagcomm_port_open_mask = 0;
word diagcomm_port_close_mask = 0;
#ifdef FEATURE_ACCESSORY_ATTACHMENT_MANAGER
#error code not present
#endif
void diagcomm_usb_diag_sio_inbound_pkt_notify (dsm_item_type **);
void diagcomm_uart1_sio_inbound_pkt_notify (dsm_item_type **);
#if (defined(FEATURE_BT) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
void diagcomm_bt_spp_sio_inbound_pkt_notify (dsm_item_type **);
#endif
#if (defined(FEATURE_IRDA) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
#error code not present
#endif
#endif /* FEATURE_DIAG_MULTI_PORTS */

/* Queues around which the watermark queues are built */
#ifndef FEATURE_DIAG_MULTI_PORTS
static q_type diagcomm_sio_rx_q;
static q_type diagcomm_sio_tx_q;
#else
static q_type diagcomm_sio_rx_q[PORT_MAX];
static q_type diagcomm_sio_tx_q[PORT_MAX];
#endif

/* Watermark queues used for communications with SIO */
#ifndef FEATURE_DIAG_MULTI_PORTS
static dsm_watermark_type diagcomm_sio_rx_wmq;
static dsm_watermark_type diagcomm_sio_tx_wmq;
#else
static dsm_watermark_type diagcomm_sio_rx_wmq[PORT_MAX];
static dsm_watermark_type diagcomm_sio_tx_wmq[PORT_MAX];
#endif

static sio_open_type diagcomm_sio_open_params;

/* To avoid extra copies between layers in the embedded system, we pass
   pointers from SIO and wait for the caller to release them. */
static dsm_item_type *diagcomm_sio_inbound_item_ptr = NULL;
static dsm_item_type *diagcomm_sio_outbound_item_ptr = NULL;


/* Configuration information for the diagnostic communications layer.
*/
static struct
{
  void (*inbound_cb) (void);

  /* !!! Note: if using a system with separate threads
     ** for RX and TX, these should be NULL.
   */
  void (*tx_start_cb) (void);
  void (*tx_stop_cb) (void);
}
diagcomm_params;


/* Connection state. */
static boolean diagcomm_connected = FALSE;

#if (defined(FEATURE_BT) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
static bt_spp_status_type bt_spp_status;
#endif

#ifdef FEATURE_RUNTIME_DEVMAP
/* port danceing state                          */
static diagcomm_dancing_state_type diagcomm_dancing_state = {
  DIAGCOMM_DANCING_IDLE,        /* state        */
  RDM_NULL_DEV,                 /* ds_old_dev   */
  RDM_NULL_DEV                  /* diag_old_dev */
};

static diagcomm_dancing_assign_port_cb_param_type 
diagcomm_dancing_assign_port_cb_params = {
  FALSE,        /* not valid                */
};

#endif /* FEATURE_RUNTIME_DEVMAP */

/*-------------------------------------------------------------------------
                   Local function prototype
 ------------------------------------------------------------------------- */
#ifdef FEATURE_RUNTIME_DEVMAP 

static void diagcomm_dancing_to_data_mode( void );

static void diagcomm_assign_port_cb
( 
  /* status of rdm_assign_port                  */
  rdm_assign_status_type        status,

  /* the service requesting the assignment       */
  rdm_service_enum_type         srv_requester,

  /* the desired port for the srv_requester     */
  rdm_device_enum_type          dev_desired
);

static void diagcomm_reset_dancing_state( void );

static void diagcomm_process_rdm_cb( void *unused );
#endif /* FEATURE_RUNTIME_DEVMAP */


/*lint -save -e754 */
/* -------------------------------------------------------------------------
** Internal routines for this file (externalized routines found later).
** ------------------------------------------------------------------------- */
/*===========================================================================

FUNCTION DIAGCOMM_SIO_TX_START_CB
FUNCTION DIAGCOMM_SIO_TX_STOP_CB

DESCRIPTION
  Flow control function callbacks for sending data to SIO.

  diag will stop/start streaming as a result of these functions.

===========================================================================*/
void
diagcomm_sio_tx_start_cb (dsm_mempool_id_enum_type lamont,
              dsm_mem_level_enum_type dummy,
              dsm_mem_op_enum_type aunt_esther)
{
  (void) lamont;      /* suppress compiler warning */
  (void) dummy;       /* suppress compiler warning */
  (void) aunt_esther; /* suppress compiler warning */

  if (diagcomm_params.tx_start_cb != NULL)
  {
    (*diagcomm_params.tx_start_cb) ();
  }
}

void
diagcomm_sio_tx_stop_cb (dsm_mempool_id_enum_type lamont,
             dsm_mem_level_enum_type dummy,
             dsm_mem_op_enum_type aunt_esther)
{
  (void) lamont;      /* suppress compiler warning */
  (void) dummy;       /* suppress compiler warning */
  (void) aunt_esther; /* suppress compiler warning */

  if (diagcomm_params.tx_stop_cb != NULL)
  {
    (*diagcomm_params.tx_stop_cb) ();
  }
}

/*===========================================================================

FUNCTION DIAGCOMM_SIO_INBOUND_PKT_NOTIFY

DESCRIPTION
  This is the callback SIO calls when data is received.

  This function enqueues the data onto the diag's watermark.  Further, if the
  last char in the input dsm_item_type is equal to DIAG_ASYNC_CONTROL_CHAR,
  the inbound callback is called.

===========================================================================*/
void
diagcomm_sio_inbound_pkt_notify (dsm_item_type ** item_ptr)
{
  dsm_mempool_id_enum_type item_pool_id;

  item_pool_id = (*item_ptr)->pool_id;

  if (DSM_DONT_EXCEED_ITEMS (item_pool_id) ||
#ifndef FEATURE_DIAG_MULTI_PORTS
      (q_cnt (diagcomm_sio_rx_wmq.q_ptr) >=
#else
      (q_cnt (diagcomm_sio_rx_wmq[diagcomm_active_port].q_ptr) >=
#endif /* FEATURE_DIAG_MULTI_PORTS */
       ( DSM_DIAG_ITEM_CNT - 
        DSM_POOL_MEM_EVENT_LEVEL(DSM_DIAG_ITEM_POOL,DSM_MEM_LEVEL_MANY) - 1 )
      ) )
  {
    dsm_free_packet (item_ptr);
  }
  else
  {
#ifndef FEATURE_DIAG_MULTI_PORTS
    dsm_simple_enqueue_isr (&diagcomm_sio_rx_wmq, item_ptr);
#else
    dsm_simple_enqueue_isr (&diagcomm_sio_rx_wmq[diagcomm_active_port], item_ptr);
#endif

    /* Call the inbound callback to tell the client
       that we have inbound data available. */
    if (diagcomm_params.inbound_cb)
    {
      diagcomm_params.inbound_cb ();
    }
  }
}               /* diagcomm_sio_inbound_notify */

/*===========================================================================

FUNCTION DIAGCOMM_SIO_OPEN_CB

DESCRIPTION
  This callback is called when RDM wants diag to open a port.

===========================================================================*/
static void
diagcomm_sio_open_cb 
(sio_port_id_type port_id
 #ifdef FEATURE_DIAG_MULTI_PORTS
 , void *data
 #endif
)
{
  #ifdef T_QUASAR
#error code not present
  #else
  diagcomm_sio_open_params.port_id = port_id;
  #endif
  
#ifdef FEATURE_DIAG_MULTI_PORTS
  switch(port_id) {
  case SIO_PORT_UART_MAIN:
      diagcomm_port_open_mask |= DIAGCOMM_PORT_UART1_BM;
      break;
#if defined(FEATURE_USB) && \
    (defined(FEATURE_USB_DIAG) || defined(FEATURE_USB_DIAG_NMEA) )
  case SIO_PORT_USB_DIAG:
      diagcomm_port_open_mask |= DIAGCOMM_PORT_USB_DIAG_BM;
      break;
#endif
#if (defined(FEATURE_BT) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
  case SIO_PORT_BT_SPP:
      diagcomm_port_open_mask |= DIAGCOMM_PORT_BT_SPP_BM;
      break;
#endif
#if (defined(FEATURE_IRDA) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
#error code not present
#endif
  default:
      break;
   }
#endif

  (void) rex_set_sigs (&diag_tcb, DIAG_COMM_OPEN_SIG);
}

/*===========================================================================

FUNCTION DIAGCOMM_SIO_CLOSE_CB
DESCRIPTION
  This callback is called when RDM wants diag to close its port.

===========================================================================*/
static void
diagcomm_sio_close_cb (
#ifdef FEATURE_DIAG_MULTI_PORTS
  sio_port_id_type port_id, 
  void *data
#endif
)
{
#ifdef FEATURE_DIAG_MULTI_PORTS
  switch(port_id) 
  {
    case SIO_PORT_UART_MAIN:
      diagcomm_port_close_mask |= DIAGCOMM_PORT_UART1_BM;
      break;
#if defined(FEATURE_USB) && \
    (defined(FEATURE_USB_DIAG) || defined(FEATURE_USB_DIAG_NMEA) )
    case SIO_PORT_USB_DIAG:
      diagcomm_port_close_mask |= DIAGCOMM_PORT_USB_DIAG_BM;
      break;
#endif
#if (defined(FEATURE_BT) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
    case SIO_PORT_BT_SPP:
      diagcomm_port_close_mask |= DIAGCOMM_PORT_BT_SPP_BM;
      break;
#endif
#if (defined(FEATURE_IRDA) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
#error code not present
#endif
    default:
      break;
  }
#endif

  (void) rex_set_sigs (&diag_tcb, DIAG_COMM_CLOSE_SIG);

}

/*===========================================================================

FUNCTION DIAGCOMM_SIO_CLOSE_DONE_CB
DESCRIPTION
  This callback is called when sio_close is completed.

===========================================================================*/
static void
diagcomm_sio_close_done_cb (void)
{
  diag_release ();
}

/* -------------------------------------------------------------------------
** Externalized routines (declared in diagcomm.h).
** ------------------------------------------------------------------------- */
/*===========================================================================

FUNCTION DIAGCOMM_REGISTER_INBOUND

DESCRIPTION
  This function configures the diagnostic subsystem's communcations layer.

===========================================================================*/
void
diagcomm_register_inbound (void (*inbound_pkt_cb) (void))
{
  diagcomm_params.inbound_cb = inbound_pkt_cb;
}               /* diagcomm_register_inbound */

/*===========================================================================

FUNCTION DIAGCOMM_REGISTER_OUTBOUND_FLOW_CONTROL

DESCRIPTION
  This function registers callbacks for software flow control for
  the outbound link.

===========================================================================*/
void
diagcomm_register_outbound_flow_ctrl (void (*tx_start_cb) (void),
                      void (*tx_stop_cb) (void))
{
  diagcomm_params.tx_start_cb = tx_start_cb;
  diagcomm_params.tx_stop_cb = tx_stop_cb;
}               /* diagcomm_register_outbound_flow_ctrl */


#if (defined(FEATURE_BT) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
/*===========================================================================

FUNCTION DIAGCOMM_UPDATE_SIO_CONN_STATUS

DESCRIPTION
  Callback function handling SPP status change for BT.  Called by BT driver.

===========================================================================*/
void diagcomm_update_sio_conn_status(
  bt_spp_status_type* spp_status_ptr
)
{
  if ( (spp_status_ptr->spp_state == BT_SPP_ST_OPENING) &&
#ifdef FEATURE_DIAG_MULTI_PORTS
       (diagcomm_multi_sio_stream_id[PORT_BT_SPP] == SIO_NO_STREAM_ID)
#else
       (diagcomm_sio_stream_id == SIO_NO_STREAM_ID) 
#endif
       )
  {
    /* 
      If BT task has higher priority than DIAG task, this function would
      be called before sio_open() returns a valid stream ID.  We need to
      save the stream ID here.
    */
  #ifdef FEATURE_DIAG_MULTI_PORTS
    diagcomm_multi_sio_stream_id[PORT_BT_SPP] = spp_status_ptr->stream_id;
  #else
    diagcomm_sio_stream_id = spp_status_ptr->stream_id;
  #endif
    
    bt_spp_status = *spp_status_ptr;
  }
  else if ( spp_status_ptr->stream_id == 
#ifdef FEATURE_DIAG_MULTI_PORTS
            diagcomm_multi_sio_stream_id[PORT_BT_SPP]
#else
            diagcomm_sio_stream_id 
#endif
            )
  {
    bt_spp_status = *spp_status_ptr;
    switch ( spp_status_ptr->spp_state )
    {
      case BT_SPP_ST_CLOSED:
      case BT_SPP_ST_OPEN:
      case BT_SPP_ST_OPEN_ERROR:
        diag_release();
        break;
      default:
        break;
    }
  }
}
#endif /* (defined(FEATURE_BT) && defined(FEATURE_DIAG_SUPPORT_WIRELESS)) */

/*===========================================================================

FUNCTION DIAGCOMM_OPEN

DESCRIPTION
  Opens the communcications stream.  Returns boolean indicating success.

===========================================================================*/
boolean
diagcomm_open (void)
{
#if defined(FEATURE_DIAG_NV) && defined(FEATURE_DIAG_DEFAULT_BITRATE_NV)
  nv_sio_baudrate_type nv_default_baud = NV_SIO_BAUDRATE_115200;
  nv_stat_enum_type nv_status;
#endif

#if (defined(FEATURE_IRDA) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
#error code not present
#endif

  static boolean init_done = FALSE;
  boolean connected = diagcomm_connected;

  if (init_done == FALSE)
  {
    init_done = TRUE;
  }

  #ifndef FEATURE_DIAG_MULTI_PORTS
  if (connected)
  {
    /* Flush the diagnostic subsystem's output buffering system. */
    //diagbuf_flush ();

#if defined (FEATURE_RUNTIME_DEVMAP)
    /* As requested, we will now close. */
    sio_close (diagcomm_sio_stream_id, diagcomm_sio_close_done_cb);
    diag_block ();

    diagcomm_sio_stream_id = SIO_NO_STREAM_ID;
#endif /* FEATURE_RUNTIME_DEVMAP */
    /* success or not, consider close operation successful */
    connected = diagcomm_connected = FALSE;

  }
  #endif /* FEATURE_DIAG_MULTI_PORTS */

#if defined(FEATURE_DIAG_NV) && defined(FEATURE_DIAG_DEFAULT_BITRATE_NV)
  nv_status = diag_nv_read (NV_DIAG_DEFAULT_BAUDRATE_I,
                (nv_item_type *) & nv_default_baud);

  switch (nv_status)
  {
  case NV_DONE_S:
    diagcomm_sio_open_params.rx_bitrate = (sio_bitrate_type) nv_default_baud;
    diagcomm_sio_open_params.tx_bitrate = (sio_bitrate_type) nv_default_baud;
    break;

  case NV_NOTACTIVE_S:
    nv_default_baud = NV_SIO_BAUDRATE_115200;
    (void) diag_nv_write (NV_DIAG_DEFAULT_BAUDRATE_I, &nv_default_baud);
    break;

  default:
    MSG_MED ("Failed to read sio bitrate", 0, 0, 0);
  }
#endif /* FEATURE_DIAG_NV && FEATURE_DIAG_DEFAULT_BITRATE_NV */


#if !defined (FEATURE_RUNTIME_DEVMAP)
  diagcomm_sio_open_params.port_id =  sdm_get_port_id (SDM_APP_DIAG);
#endif

  if (
#ifndef FEATURE_DIAG_MULTI_PORTS
      !connected &&
#endif
       diagcomm_sio_open_params.port_id != SIO_PORT_NULL)
  {
#if (defined(FEATURE_BT) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
    /*
       When using BT_SPP port, Bluetooth specific information is needed
       in order to open a server SIO stream that waits for connection 
       requests from other Bluetooth devices (in this case, any laptops 
       equipped with Bluetooth PC card)
     */
    bt_spp_open_type bso;
    char bt_svc_name[] = "BT DIAG";

#ifndef FEATURE_DIAG_MULTI_PORTS
    if ( diagcomm_sio_open_params.port_id == SIO_PORT_BT_SPP )
#else
    if ( diagcomm_port_open_mask & DIAGCOMM_PORT_BT_SPP_BM )
#endif /* FEATURE_DIAG_MULTI_PORTS */
    {
      /* Fill in Bluetooth specific information */
/*lint -save -e691 -e789 */
      diagcomm_sio_open_params.bt_open_ptr = &bso;
/*lint -restore */
      bso.client_app           = FALSE; /* always server */
      bso.bd_addr_ptr          = NULL;  /* not used for server */
      bso.status_change_fptr   = diagcomm_update_sio_conn_status;
      bso.config_change_fptr   = NULL;
      bso.modem_status_fptr    = NULL;
      bso.line_error_fptr      = NULL;
      bso.max_frame_size       = BT_SPP_MFS_AUTOMATIC;
      bso.rc_server_channel    = BT_SPP_SCN_UNSPECIFIED;
      bso.service_uuid         = BT_SD_SERVICE_CLASS_SERIAL_PORT;
      bso.service_version      = 0;
      bso.service_name_str_ptr = bt_svc_name;

      bt_spp_status.spp_state = BT_SPP_ST_CLOSED;
    }
#endif

#if (defined(FEATURE_IRDA) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
#error code not present
#endif

#ifdef FEATURE_DIAG_MULTI_PORTS
    
    if (diagcomm_port_open_mask & DIAGCOMM_PORT_UART1_BM) 
    {
      diagcomm_port_open_mask &= ~(DIAGCOMM_PORT_UART1_BM);
      if(diagcomm_multi_sio_stream_id[PORT_UART1] == SIO_NO_STREAM_ID)
      {
        diagcomm_sio_open_params.port_id = SIO_PORT_UART_MAIN;
        diagcomm_sio_open_params.rx_func_ptr = diagcomm_uart1_sio_inbound_pkt_notify;
        diagcomm_sio_open_params.rx_queue = &diagcomm_sio_rx_wmq[PORT_UART1];
        diagcomm_sio_open_params.tx_queue = &diagcomm_sio_tx_wmq[PORT_UART1];
        diagcomm_multi_sio_stream_id[PORT_UART1] = sio_open (&diagcomm_sio_open_params);
        if(diagcomm_multi_sio_stream_id[PORT_UART1] != SIO_NO_STREAM_ID) {
          diagcomm_connected = connected = TRUE;
          rdm_notify (RDM_DIAG_SRVC, RDM_DONE_S);
        }
        else {
          rdm_notify (RDM_DIAG_SRVC, RDM_NOT_ALLOWED_S);
        }
      }
    }
#if defined(FEATURE_USB) && \
    (defined(FEATURE_USB_DIAG) || defined(FEATURE_USB_DIAG_NMEA) )
    if (diagcomm_port_open_mask & DIAGCOMM_PORT_USB_DIAG_BM)
    {
      diagcomm_port_open_mask &= ~(DIAGCOMM_PORT_USB_DIAG_BM);    
      if (diagcomm_multi_sio_stream_id[PORT_USB_DIAG] == SIO_NO_STREAM_ID)
      {
        diagcomm_sio_open_params.port_id = SIO_PORT_USB_DIAG;
        diagcomm_sio_open_params.rx_func_ptr = diagcomm_usb_diag_sio_inbound_pkt_notify;
        diagcomm_sio_open_params.rx_queue = &diagcomm_sio_rx_wmq[PORT_USB_DIAG];
        diagcomm_sio_open_params.tx_queue = &diagcomm_sio_tx_wmq[PORT_USB_DIAG];
        diagcomm_multi_sio_stream_id[PORT_USB_DIAG] = sio_open (&diagcomm_sio_open_params);
        if(diagcomm_multi_sio_stream_id[PORT_USB_DIAG] != SIO_NO_STREAM_ID) {
          diagcomm_connected = connected = TRUE;
          rdm_notify (RDM_DIAG_SRVC, RDM_DONE_S);
        }
        else {
          rdm_notify (RDM_DIAG_SRVC, RDM_NOT_ALLOWED_S);
        }
      }
    }
#endif
#if (defined(FEATURE_BT) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
    if (diagcomm_port_open_mask & DIAGCOMM_PORT_BT_SPP_BM) 
    {
      diagcomm_port_open_mask &= ~(DIAGCOMM_PORT_BT_SPP_BM);
      if (diagcomm_multi_sio_stream_id[PORT_BT_SPP] == SIO_NO_STREAM_ID)
      {
        diagcomm_sio_open_params.port_id = SIO_PORT_BT_SPP;
        diagcomm_sio_open_params.rx_func_ptr = diagcomm_bt_spp_sio_inbound_pkt_notify;
        diagcomm_sio_open_params.rx_queue = &diagcomm_sio_rx_wmq[PORT_BT_SPP];
        diagcomm_sio_open_params.tx_queue = &diagcomm_sio_tx_wmq[PORT_BT_SPP];
        diagcomm_multi_sio_stream_id[PORT_BT_SPP] = sio_open (&diagcomm_sio_open_params);
        if (diagcomm_multi_sio_stream_id[PORT_BT_SPP] != SIO_NO_STREAM_ID) 
        {
          if ( (bt_spp_status.spp_state == BT_SPP_ST_CLOSED) ||
             (bt_spp_status.spp_state == BT_SPP_ST_OPENING) ) 
          {
            diag_block ();
          }
          if ( bt_spp_status.spp_state == BT_SPP_ST_OPEN )
          {
            diagcomm_connected = connected = TRUE;
            rdm_notify (RDM_DIAG_SRVC, RDM_DONE_S);
          }
          else // must be BT_SPP_ST_OPEN_ERROR
          {
            sio_close (diagcomm_multi_sio_stream_id[PORT_BT_SPP], diagcomm_sio_close_done_cb);
            diagcomm_multi_sio_stream_id[PORT_BT_SPP] = SIO_NO_STREAM_ID;
            rdm_notify (RDM_DIAG_SRVC, RDM_NOT_ALLOWED_S);
          }
        }
      }
    }
#endif
#if (defined(FEATURE_IRDA) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
#error code not present
#endif
    
#else /* FEATURE_DIAG_MULTI_PORTS */
    diagcomm_sio_stream_id = sio_open (&diagcomm_sio_open_params);
    
    if (diagcomm_sio_stream_id != SIO_NO_STREAM_ID)
    {
#if (defined(FEATURE_BT) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
      /*
         The SIO stream opened for BT_SPP is not considered opened
         until it reaches BT_SPP_ST_OPEN state;  if Bluetooth driver
         fails in setting up the BT_SPP port for Bluetooth connection,
         it sets the state to BT_SPP_ST_OPEN_ERROR, in which case the
         SIO stream should be closed;
       */
      if ( diagcomm_sio_open_params.port_id == SIO_PORT_BT_SPP )
      {
        if ( (bt_spp_status.spp_state == BT_SPP_ST_CLOSED) ||
             (bt_spp_status.spp_state == BT_SPP_ST_OPENING) )
        {
          diag_block ();
        }
        if ( bt_spp_status.spp_state == BT_SPP_ST_OPEN )
        {
          diagcomm_connected = connected = TRUE;
        }
        else // must be BT_SPP_ST_OPEN_ERROR
        {
          sio_close (diagcomm_sio_stream_id, diagcomm_sio_close_done_cb);
          diagcomm_sio_stream_id = SIO_NO_STREAM_ID;
        }
      }
      else
#endif
        diagcomm_connected = connected = TRUE;
    }
    else
    {
      MSG_LOW ("Couldn't get stream.", 0, 0, 0);
    }
#endif /* FEATURE_DIAG_MULTI_PORTS */
  }

#if defined(FEATURE_RUNTIME_DEVMAP) && !defined(FEATURE_DIAG_MULTI_PORTS)
  if (connected)
  {
    rdm_notify (RDM_DIAG_SRVC, RDM_DONE_S);
  }
  else
  {
    rdm_notify (RDM_DIAG_SRVC, RDM_NOT_ALLOWED_S);
  }
#endif

  diag_dtr_enable();     /* Registering the callback for DTR changes */
  diag_dtr_changed_cb(); /* To ensure that the state is correct. */
    
  return connected;
}               /* diagcomm_open */


/*===========================================================================

FUNCTION diag_dtr_changed_cb

DESCRIPTION
  Invoked when DTR changes. This is intended to flip a variable that causes
  Diag to vote/not vote for sleep based on connectivity status.

===========================================================================*/
void diag_dtr_changed_cb( void )
{
  sio_ioctl_param_type ioctl_param;
  boolean dte_ready_asserted;

  ioctl_param.dte_ready_asserted = &dte_ready_asserted;

  sio_ioctl( diagcomm_sio_stream_id,
             SIO_IOCTL_DTE_READY_ASSERTED,
             &ioctl_param );
  
  if (!dte_ready_asserted)
  {
    msg_set_all_rt_masks (0);
    diag_channel_connected = FALSE;
    diag_sleep_vote (DIAG_SLEEP_DIAG, TRUE);
  }
  else
  {
    diag_channel_connected = TRUE;
    diag_sleep_vote (DIAG_SLEEP_DIAG, FALSE);
  }
} 

/*===========================================================================

FUNCTION diag_dtr_enable

DESCRIPTION
  Invoked when Diag initializes. This registers a callback with SIO that is
  intended to report when there are DTR changes.

===========================================================================*/
void diag_dtr_enable
(
  void
)
{
  sio_ioctl_param_type ioctl_param;

  /* register DTR callback */
  ioctl_param.enable_dte_ready_event = diag_dtr_changed_cb;
  sio_ioctl( diagcomm_sio_stream_id,
             SIO_IOCTL_ENABLE_DTR_EVENT,
             &ioctl_param );
}


/*===========================================================================

FUNCTION DIAGCOMM_CLOSE

DESCRIPTION
  Closes the current connection, if any.

===========================================================================*/
void
diagcomm_close (void (*close_func_ptr) (void))
{
  /* Got a command to close the connection. */
  if (diagcomm_connected)
  {
    /* Flush the diagnostic subsystem's output buffering system. */
    diagbuf_flush ();

#if defined (FEATURE_RUNTIME_DEVMAP)
#ifndef FEATURE_DIAG_MULTI_PORTS
#if (defined(FEATURE_BT) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
    /* Bring down Bluetooth connection before closing BT_SPP port. */
    if ( (diagcomm_sio_open_params.port_id == SIO_PORT_BT_SPP) &&
         (bt_spp_status.spp_state == BT_SPP_ST_CONNECTED) )
    {
      sio_ioctl( diagcomm_sio_stream_id, SIO_IOCTL_BT_DISCONNECT, NULL );
      if ( bt_spp_status.spp_state != BT_SPP_ST_OPEN )
      {
        diag_block (); /* wait until server port becomes open (=disconnected) */
      }
    }
#endif
    /* As requested, we will now close. */
    sio_close (diagcomm_sio_stream_id, diagcomm_sio_close_done_cb);
    diag_block ();

    rdm_notify (RDM_DIAG_SRVC, RDM_DONE_S);

    if (close_func_ptr != NULL)
    {
      (close_func_ptr) ();
    }

    diagcomm_sio_stream_id = SIO_NO_STREAM_ID;
    /* success or not, consider close operation successful
    */
    diagcomm_connected = FALSE;

#else
    if (diagcomm_port_close_mask & DIAGCOMM_PORT_UART1_BM) 
    {
      diagcomm_port_close_mask &= ~(DIAGCOMM_PORT_UART1_BM);
      if (diagcomm_multi_sio_stream_id[PORT_UART1] != SIO_NO_STREAM_ID)
      {
        sio_close(diagcomm_multi_sio_stream_id[PORT_UART1], diagcomm_sio_close_done_cb);
        diag_block ();
        diagcomm_multi_sio_stream_id[PORT_UART1] = SIO_NO_STREAM_ID;
        rdm_notify (RDM_DIAG_SRVC, RDM_DONE_S);
        if (close_func_ptr != NULL)
        {
          (close_func_ptr) ();
        }
        if(diagcomm_sio_stream_id == diagcomm_multi_sio_stream_id[PORT_UART1])
            diagcomm_sio_stream_id = SIO_NO_STREAM_ID;
      }
    }
    if (diagcomm_port_close_mask & DIAGCOMM_PORT_USB_DIAG_BM)
    {
      diagcomm_port_close_mask &= ~(DIAGCOMM_PORT_USB_DIAG_BM);   
      if (diagcomm_multi_sio_stream_id[PORT_USB_DIAG] != SIO_NO_STREAM_ID)
      {
        sio_close(diagcomm_multi_sio_stream_id[PORT_USB_DIAG], diagcomm_sio_close_done_cb);
        diag_block ();
        diagcomm_multi_sio_stream_id[PORT_USB_DIAG] = SIO_NO_STREAM_ID;
        rdm_notify (RDM_DIAG_SRVC, RDM_DONE_S);
        if (close_func_ptr != NULL)
        {
          (close_func_ptr) ();
        }
        if(diagcomm_sio_stream_id == diagcomm_multi_sio_stream_id[PORT_UART1])
          diagcomm_sio_stream_id = SIO_NO_STREAM_ID;
       }
    }

#ifdef FEATURE_DIAG_SUPPORT_WIRELESS
    if (diagcomm_port_close_mask & DIAGCOMM_PORT_BT_SPP_BM) 
    {
      diagcomm_port_close_mask &= ~(DIAGCOMM_PORT_BT_SPP_BM);     
      if (diagcomm_multi_sio_stream_id[PORT_BT_SPP] != SIO_NO_STREAM_ID)
      {
        if (bt_spp_status.spp_state == BT_SPP_ST_CONNECTED)
        {
          sio_ioctl( diagcomm_multi_sio_stream_id[PORT_BT_SPP], SIO_IOCTL_BT_DISCONNECT, NULL );
          if ( bt_spp_status.spp_state != BT_SPP_ST_OPEN )
          {
            diag_block (); /* wait until server port becomes open (=disconnected) */
          }
        }
        sio_close(diagcomm_multi_sio_stream_id[PORT_BT_SPP], diagcomm_sio_close_done_cb);
        diag_block ();
        diagcomm_multi_sio_stream_id[PORT_BT_SPP] = SIO_NO_STREAM_ID;
        rdm_notify (RDM_DIAG_SRVC, RDM_DONE_S);
        if (close_func_ptr != NULL)
        {
          (close_func_ptr) ();
        }
        if(diagcomm_sio_stream_id == diagcomm_multi_sio_stream_id[PORT_BT_SPP])
            diagcomm_sio_stream_id = SIO_NO_STREAM_ID;
      }
    }
#endif

#if (defined(FEATURE_IRDA) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
#error code not present
#endif
    
#endif
#endif
  }
}               /* diagcomm_close */

/*===========================================================================

 FUNCTION DIAGCOMM_FLUSH_RX

 DESCRIPTION
   Flushes inbound queue.  This is used when a security timeout occurs,
   though the capability is generic.

===========================================================================*/
void
diagcomm_flush_rx (void (*flush_cb) (void))
{
#ifndef FEATURE_DIAG_MULTI_PORTS
  dsm_empty_queue (&diagcomm_sio_rx_wmq);
#else
  dsm_empty_queue (&diagcomm_sio_rx_wmq[diagcomm_active_port]);
#endif

  if (flush_cb)
  {
    flush_cb ();
  }
}

/*===========================================================================

 FUNCTION DIAGCOMM_FLUSH_TX_CB

 DESCRIPTION
   Called by SIO to ack the successful completion of a TX flush.

===========================================================================*/
static void
diagcomm_flush_tx_cb (void)
{
  diag_release ();
}

/*===========================================================================

 FUNCTION DIAGCOMM_FLUSH_TX

 DESCRIPTION
   Flushes the transmit channel on current connection.

===========================================================================*/
void
diagcomm_flush_tx (void (*flush_cb) (void))
{
  if (diagcomm_connected
      #ifdef FEATURE_DIAG_MULTI_PORTS
      && diagcomm_sio_stream_id != SIO_NO_STREAM_ID
      #endif
      )
  {
    /* Tell SIO to flush the TX FIFO */
    sio_flush_tx (diagcomm_sio_stream_id, diagcomm_flush_tx_cb);
    diag_block ();
  }

  if (flush_cb)
  {
    flush_cb ();
  }
}

/*===========================================================================

FUNCTION DIAGCOMM_STATUS

DESCRIPTION
  This function returns TRUE if the communications layer has an open port.

===========================================================================*/
boolean
diagcomm_status (void)
{
  if(diag_channel_connected == TRUE)
    return diagcomm_connected;
  else
    return FALSE;
}

/*===========================================================================

FUNCTION DIAGCOMM_OUTBOUND_ALLOC

DESCRIPTION
  This function requests a buffer from the diag communications layer.

===========================================================================*/
void *
diagcomm_outbound_alloc (unsigned int req_length, unsigned int *count)
{
  void *ptr = NULL;
  ASSERT (count);

  (void) req_length;      /* suppress compiler warning */

  /* The diag comm layer only allows one buffer to be allocated at a time. */
  if (diagcomm_sio_outbound_item_ptr == NULL)
  {
    diagcomm_sio_outbound_item_ptr = dsm_new_buffer (DSM_DIAG_ITEM_POOL);

    if (diagcomm_sio_outbound_item_ptr != NULL)
    {
      *count = diagcomm_sio_outbound_item_ptr->size;

      ptr = diagcomm_sio_outbound_item_ptr->data_ptr;

    }
    else
    {
      MSG_LOW ("Ran out of DSM items", 0, 0, 0);
      *count = 0;
    }
  }

  return ptr;
}               /* diagcomm_outbound_alloc */

/*===========================================================================

FUNCTION DIAGCOMM_SEND
  Sends the given data stream if the connection is open.

DESCRIPTION

===========================================================================*/
#ifdef DEBUG_TX_EFS
#error code not present
#endif /* DEBUG_TX_EFS */

void
diagcomm_send (void *ptr, unsigned int length)
{
#ifdef DEBUG_TX_EFS
#error code not present
#endif

  ASSERT (ptr);

#ifdef DEBUG_TX_EFS
#error code not present
#endif

  if (diagcomm_sio_outbound_item_ptr == NULL ||
      diagcomm_sio_outbound_item_ptr->data_ptr != ptr ||
      diagcomm_sio_outbound_item_ptr->size < length)
  {
    /* Corrupt state.  Free memory */
    MSG_LOW ("Attempt to send invalid ptr", 0, 0, 0);
  }
  else
  {
    if (length > 0 
        #ifdef FEATURE_DIAG_MULTI_PORTS
        && diagcomm_sio_stream_id != SIO_NO_STREAM_ID
        #endif
        )
    {
#ifdef DEBUG_TX_EFS
#error code not present
#endif

      diagcomm_sio_outbound_item_ptr->used = (uint16)length;
      sio_transmit (diagcomm_sio_stream_id, diagcomm_sio_outbound_item_ptr);
    }
    else
    {
      /* If empty, just release it */
      (void) dsm_free_buffer (diagcomm_sio_outbound_item_ptr);
    }

    diagcomm_sio_outbound_item_ptr = NULL;
  }
}               /* diagcomm_send */

/*===========================================================================

FUNCTION DIAGCOMM_INBOUND

DESCRIPTION
  This function returns a pointer to some inbound data.  If no inbound
  data is available, NULL is returned.

===========================================================================*/
void *
diagcomm_inbound (unsigned int *stream_len)
{
  void *return_ptr = NULL;

  ASSERT (stream_len);

  if (diagcomm_sio_inbound_item_ptr != NULL)
  {
    MSG_LOW ("Attempt to get more than one inbound buffer.", 0, 0, 0);

    *stream_len = 0;
  }
  else
  {
#ifndef FEATURE_DIAG_MULTI_PORTS
    diagcomm_sio_inbound_item_ptr = dsm_dequeue (&diagcomm_sio_rx_wmq);
#else
    diagcomm_sio_inbound_item_ptr = dsm_dequeue (&diagcomm_sio_rx_wmq[diagcomm_active_port]);
#endif

    if (diagcomm_sio_inbound_item_ptr != NULL)
    {
      ASSERT (diagcomm_sio_inbound_item_ptr->data_ptr != NULL);

      *stream_len = diagcomm_sio_inbound_item_ptr->used;

      return_ptr = diagcomm_sio_inbound_item_ptr->data_ptr;
    }
  }

  return return_ptr;
}               /* diagcomm_inbound */

/*===========================================================================

FUNCTION DIAGCOMM_INBOUND_COMPLETE

DESCRIPTION
  This function tells the diag communications layer that the caller is done
  with the buffer returned by diagcomm_send() may now be released.

===========================================================================*/
void                                                      
diagcomm_inbound_complete (void *ptr)
{
  ASSERT (ptr != NULL);

  if (diagcomm_sio_inbound_item_ptr != NULL &&
      ptr == diagcomm_sio_inbound_item_ptr->data_ptr)
  {
    (void) dsm_free_buffer (diagcomm_sio_inbound_item_ptr);
    diagcomm_sio_inbound_item_ptr = NULL;
  }
}               /* diagcomm_inbound_complete */

/* -------------------------------------------------------------------------
** Legacy sio coupled routines
** ------------------------------------------------------------------------- */
/*===========================================================================

FUNCTION DIAG_RECORD_DM_BITRATE                         EXTERNALIZED FUNCTION

DESCRIPTION
  This function is used by caller to set baudrate for diag which will take
  effect at the next occurence of DIAG_SIO_OPEN_SIG (calls sio_open).

===========================================================================*/
void
diag_record_dm_baudrate (sio_bitrate_type bitrate   /* Bitrate to use at next SIO open */
  )
{
  diagcomm_sio_open_params.rx_bitrate = bitrate;
  diagcomm_sio_open_params.tx_bitrate = bitrate;
}               /* diag_record_dm_baudrate */

#if defined(FEATURE_DIAG_UI_BAUD)
/*===========================================================================

FUNCTION DIAG_CHANGE_DEFAULT_SIO_BITRATE                EXTERNALIZED FUNCTION

DESCRIPTION
  This function changed the default diag bitrate as well as change the
  current operating bitrate.

===========================================================================*/
void
diag_change_default_sio_bitrate (sio_bitrate_type bitrate   /* Bitrate to use at next SIO open */
  )
{
#ifdef FEATURE_DIAG_DEFAULT_BITRATE_NV
  /* Change NV item that stores the default diag bitrate. */
  static nv_cmd_type nvcmd;
  static nv_sio_baudrate_type nv_item;

  /* Set bitrate */
  nv_item = (nv_sio_baudrate_type) bitrate;

  nvcmd.cmd = NV_WRITE_F;
  nvcmd.data_ptr = (nv_item_type *) & nv_item;
  nvcmd.item = NV_DIAG_DEFAULT_BAUDRATE_I;

  nvcmd.done_q_ptr = NULL;
  nvcmd.sigs = 0;       /* don't need an ACK from NV on this. */
  nvcmd.tcb_ptr = rex_self ();

  nv_cmd (&nvcmd);
#endif

  /* Set diag's bitrate for next open cycle */
  diag_record_dm_baudrate (bitrate);

  /* If diag has a port open, change the bitrate */
  if (diagcomm_status ())
  {
#ifndef FEATURE_DIAG_MULTI_PORTS
    sio_ioctl (diagcomm_sio_stream_id,
           SIO_IOCTL_CHANGE_BAUD_NOW, (sio_ioctl_param_type *) & bitrate);
#else
    if(diagcomm_multi_sio_stream_id[PORT_UART1] != SIO_NO_STREAM_ID)
    sio_ioctl (diagcomm_multi_sio_stream_id[PORT_UART1],
           SIO_IOCTL_CHANGE_BAUD_NOW, (sio_ioctl_param_type *) & bitrate);

    if(diagcomm_multi_sio_stream_id[PORT_USB_DIAG] != SIO_NO_STREAM_ID)
    sio_ioctl (diagcomm_multi_sio_stream_id[PORT_USB_DIAG],
           SIO_IOCTL_CHANGE_BAUD_NOW, (sio_ioctl_param_type *) & bitrate);

#ifdef FEATURE_DIAG_SUPPORT_WIRELESS
    if(diagcomm_multi_sio_stream_id[PORT_BT_SPP] != SIO_NO_STREAM_ID)
    sio_ioctl (diagcomm_multi_sio_stream_id[PORT_BT_SPP],
           SIO_IOCTL_CHANGE_BAUD_NOW, (sio_ioctl_param_type *) & bitrate);
#endif

#if (defined(FEATURE_IRDA) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
#error code not present
#endif

#endif /* FEATURE_DIAG_MULTI_PORTS */
  }

}               /* diag_change_default_sio_bitrate */
#endif /* FEATURE_DIAG_UI_BAUD */


/*===========================================================================

FUNCTION DIAGPKT_TO_DATA_MODE

DESCRIPTION
  This procedure processes to_data_mode command.

============================================================================*/
PACKED void *
diagpkt_to_data_mode (PACKED void *req_pkt, uint16 pkt_len)
{
  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */

#if defined (FEATURE_RUNTIME_DEVMAP)
  diagcomm_dancing_to_data_mode();
#else
  MSG_HIGH ("Switch port to Data Service not supported.", 0, 0, 0);
#endif
  return NULL;
}               /* diagpkt_to_data_mode */


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
)
{
#ifdef FEATURE_RUNTIME_DEVMAP

  /* function return                            */
  boolean               result;

  /* data's current device                      */
  rdm_device_enum_type  ds_dev;

  /* diag's current device                      */
  rdm_device_enum_type  diag_dev;

/*------------------------------------------------------------------*/
  DEBUG(MSG_MED("diagcomm_sio_qcdmg",0,0,0););

  if( (diagcomm_dancing_state.state != DIAGCOMM_DANCING_IDLE) &&
      (diagcomm_dancing_state.state != DIAGCOMM_DANCING_IN_DANCE) )
  {
    /* called in wrong state, we are doing $qcdmg or to_data_mode       */
    return FALSE;
  }

#ifndef FEATURE_DATA_SERIALIZER
  /* get DS's current port                      */
  ds_dev                        = rdm_get_device(RDM_DATA_SRVC);
#else
  ds_dev = port_id;
#endif
  
#ifndef FEATURE_DIAG_MULTI_PORTS
  /* get DIAG's current port                    */
  diag_dev                      = rdm_get_device(RDM_DIAG_SRVC);
#endif

  /* since the $QCDMG is from DS port, data port is open        */
  ASSERT(ds_dev != RDM_NULL_DEV);

  /* test if diag can use DS's port             */
  if(!rdm_is_srv_dev_compat(RDM_DIAG_SRVC, ds_dev))
  {
    return FALSE;
  }

  /* passed condition checking                  */

  /* save the port setting's of DIAG and DS     */
  diagcomm_dancing_state.state           = DIAGCOMM_DANCING_QCDMG_OPENING;
  diagcomm_dancing_state.ds_old_dev      = ds_dev;
  diagcomm_dancing_state.diag_old_dev    = diag_dev;
  
  /* open the ds_old_dev for DIAG                               */
  result = rdm_assign_port(RDM_DIAG_SRVC, diagcomm_dancing_state.ds_old_dev,
                           diagcomm_assign_port_cb);

  return result;
#else
  return FALSE;

#endif /* FEATURE_RUNTIME_DEVMAP */
} /* diagcomm_sio_qcdmg */

#ifdef FEATURE_RUNTIME_DEVMAP


/*===========================================================================

FUNCTION DIAGCOMM_DANCING_TO_DATA_MODE

DESCRIPTION
  Execute DM to_data_mode command. Diag will return its port to DS and open
  DIAG's old port before dance.

===========================================================================*/
static void diagcomm_dancing_to_data_mode( void )
{
  /* current DS port                                    */
  rdm_device_enum_type  cur_ds_dev;

  /* current DIAG port                                  */
  rdm_device_enum_type  cur_diag_dev;
  
/*------------------------------------------------------------------*/

  if(diagcomm_dancing_state.state != DIAGCOMM_DANCING_IN_DANCE)
  {
    /* the DIAG and DS ports was not in dance state     */
    return;
  }

#ifndef FEATURE_DATA_SERIALIZER  
  cur_ds_dev    = rdm_get_device(RDM_DATA_SRVC);
#else
  cur_ds_dev    = RDM_NULL_DEV;
#endif

#ifndef FEATURE_DIAG_MULTI_PORTS
  cur_diag_dev  = rdm_get_device(RDM_DIAG_SRVC);
#else
  cur_diag_dev = diagcomm_dancing_state.ds_old_dev;
#endif

  if(cur_diag_dev != diagcomm_dancing_state.ds_old_dev)
  {
    /* DIAG has opened a different port, the dance has broken   */
    diagcomm_reset_dancing_state();
    return;
  }

  /* pass condition checking                                    */
  if(cur_ds_dev != RDM_NULL_DEV)
  {
    /* the DS acquired new port after last $QCDMG command, 
       No need to return the port back to DS,
       We will return diag to its old port                      */
    diagcomm_dancing_state.state   = DIAGCOMM_DANCING_TDM_OPENING_DIAG;
    if(!rdm_assign_port(RDM_DIAG_SRVC, diagcomm_dancing_state.diag_old_dev, 
                        diagcomm_assign_port_cb))
    {
      /* rdm reject the opening request                   */
      ERR("fail to return DIAG port",0,0,0);
      diagcomm_reset_dancing_state();
    }
  }
  else
  {
    /* DS don't not have a port, return the DS's old port to DS */
    diagcomm_dancing_state.state   = DIAGCOMM_DANCING_TDM_OPENING_DS;
    if(!rdm_assign_port(RDM_DATA_SRVC, cur_diag_dev, diagcomm_assign_port_cb))
    {
      /* rdm reject the opening request                   */
      ERR("TDM: fail to return DS port",0,0,0);
      diagcomm_reset_dancing_state();
    }
  }
} /* diagcomm_dancing_to_data_mode */


/*===========================================================================

FUNCTION DIAGCOMM_ASSIGN_PORT_CB

DESCRIPTION
  The call back function for rdm_assign_port for $QCDMG and to_data_mode
  commands.

===========================================================================*/
static void diagcomm_assign_port_cb
( 
  /* status of rdm_assign_port                  */
  rdm_assign_status_type        status,

  /* the service requesting the assignment       */
  rdm_service_enum_type         srv_requester,

  /* the desired port for the srv_requester     */
  rdm_device_enum_type          dev_desired
)
{
  DEBUG(MSG_MED("diagcomm_assign_port_cb, state: %d", 
                diagcomm_dancing_state.state,0,0););
  DEBUG(MSG_MED("status: %d, srv: %d, dev: %d", 
                status, srv_requester, dev_desired););

  /* make sure the parameters from last time has been processed   */
  ASSERT(!diagcomm_dancing_assign_port_cb_params.valid);
  
  diagcomm_dancing_assign_port_cb_params.valid           = TRUE;
  diagcomm_dancing_assign_port_cb_params.status          = status;
  diagcomm_dancing_assign_port_cb_params.srv_requester   = srv_requester;
  diagcomm_dancing_assign_port_cb_params.dev_desired     = dev_desired;

  // This function will be called in DIAG task context.
  (void) diag_idle_processing_register (diagcomm_process_rdm_cb, NULL);
} /* diagcomm_assign_port_cb */

#endif /* FEATURE_RUNTIME_DEVMAP */


/*===========================================================================

FUNCTION DIAGCOMM_PROCESS_RDM_CB

DESCRIPTION
  The function process call back status of rdm_assign_port for $QCDMG 
  and to_data_mode commands.

===========================================================================*/
static void diagcomm_process_rdm_cb (void *unused)
{
#ifdef FEATURE_RUNTIME_DEVMAP

  /* diagcomm_dancing_assign_port_cb parameters                 */
  diagcomm_dancing_assign_port_cb_param_type rdm_cb_params;

  (void) unused;      /* suppress compiler warning */

  ASSERT(diagcomm_dancing_assign_port_cb_params.valid);
    
  /* get the parameters of rdm_cb to process                    */
  rdm_cb_params = diagcomm_dancing_assign_port_cb_params;

  /* diagcomm_dancing_assign_port_cb_params can be reused       */
  diagcomm_dancing_assign_port_cb_params.valid = FALSE;

  switch (diagcomm_dancing_state.state)
  {
    case DIAGCOMM_DANCING_IDLE:
    case DIAGCOMM_DANCING_IN_DANCE:
      /* we should not be called in this state                  */
      ERR("diagcomm_assign_port_cb called in IDLE",0,0,0);
      break;

    case DIAGCOMM_DANCING_QCDMG_OPENING:
      /* DIAG has asked RDM to open DS's port for $QCDMG        */
      ASSERT(rdm_cb_params.srv_requester == RDM_DIAG_SRVC);
      ASSERT(rdm_cb_params.dev_desired == diagcomm_dancing_state.ds_old_dev);

      /* DIAG has asked RDM to open DS's port for $QCDMG        */
      if(rdm_cb_params.status == RDM_DONE_S)
      {
        /* succeeds                     */
        diagcomm_dancing_state.state = DIAGCOMM_DANCING_IN_DANCE;
      }
      else
      {
        /* can't close open DS's port   */
        ERR("QCDMG: fail to open DS port",0,0,0);
        diagcomm_reset_dancing_state();
      }
      break;

    case DIAGCOMM_DANCING_TDM_OPENING_DS:
      /* DIAG has asked RDM to open DS's port for to_data_mode  */
      ASSERT(rdm_cb_params.srv_requester == RDM_DATA_SRVC);
      ASSERT(rdm_cb_params.dev_desired == diagcomm_dancing_state.ds_old_dev);

      if(rdm_cb_params.status == RDM_DONE_S)
      {
#ifndef FEATURE_DIAG_MULTI_PORTS
        /* continue to get the old diag port back               */
        if(rdm_get_service(diagcomm_dancing_state.diag_old_dev) != 
           RDM_NULL_SRVC)
        {
          /* Diag's old port has been take by other service. Stop here  */
          diagcomm_reset_dancing_state();
        }
        else
        {
          diagcomm_dancing_state.state = DIAGCOMM_DANCING_TDM_OPENING_DIAG;

          if(!rdm_assign_port(RDM_DIAG_SRVC,
                              diagcomm_dancing_state.diag_old_dev, 
                              diagcomm_assign_port_cb))
          {
            /* can't open diag's old port                               */
            ERR("fail to open dev: %d", diagcomm_dancing_state.diag_old_dev,
                0,0);
            diagcomm_reset_dancing_state();
          }
        }
#else
        diagcomm_reset_dancing_state();
#endif
      }
      else
      {
        /* rdm fail to open DS's old port                               */
        ERR("fail to open dev: %d", diagcomm_dancing_state.ds_old_dev,0,0);
        diagcomm_reset_dancing_state();
      }
      break;

    case DIAGCOMM_DANCING_TDM_OPENING_DIAG:
      /* DIAG has asked RDM to open its old port for to_data_mode       */
      ASSERT(rdm_cb_params.srv_requester == RDM_DIAG_SRVC);
      ASSERT(rdm_cb_params.dev_desired == diagcomm_dancing_state.diag_old_dev);

      if(rdm_cb_params.status != RDM_DONE_S)
      {
        /* failed                                               */
        ERR("fail to open %d", diagcomm_dancing_state.diag_old_dev,0,0);
      }
      diagcomm_reset_dancing_state();
      break;

    default:
      /* any other state is wrong                               */
      ERR("wrong state: %d", diagcomm_dancing_state.state,0,0);
      break;
  }

  (void) diag_idle_processing_unregister (diagcomm_process_rdm_cb, NULL);

#endif /* FEATURE_RUNTIME_DEVMAP */
} /* diagcomm_process_rdm_cb */

#ifdef FEATURE_RUNTIME_DEVMAP

/*===========================================================================

FUNCTION DIAGCOMM_RESET_DANCING_STATE

DESCRIPTION
  Reset dancing_state.

===========================================================================*/
static void diagcomm_reset_dancing_state( void )
{
  diagcomm_dancing_state.state           = DIAGCOMM_DANCING_IDLE;
  diagcomm_dancing_state.ds_old_dev      = RDM_NULL_DEV;
  diagcomm_dancing_state.diag_old_dev    = RDM_NULL_DEV;

} /* diagcomm_reset_dancing_state */

#endif /* FEATURE_RUNTIME_DEVMAP */


/*===========================================================================

PACKET   DIAG_CONFIG_COMM_F

PURPOSE  Sent by DM to request a baud rate changefor DIAG serial
         communications.

RESPONSE DMSS returns diag_config_comm_rsp_type packet that contains
         possible configurations or an acknowledgement to a change request.
         Upon sending

===========================================================================*/
typedef enum
{
  DIAG_RS232_CONFIG_INFO = 0,   /* Request for baud rate capabilities */
  DIAG_RS232_CONFIG,        /* Set baud rate                      */
  DIAG_USB_CONFIG_INFO,     /* UNIMPLEMENTED!!!                   */
  DIAG_USB_CONFIG       /* UNIMPLEMENTED!!!                   */
}
diag_config_comm_enum_type;

typedef PACKED struct
{
  byte cmd_code;        /* Command code                         */
  byte sub_cmd;         /* Use diag_config_comm_enum_type       */
  dword baudrate;       /* Baud rate to be set - use only in a  */
  /* config command, not an info request  */
}
diag_config_comm_req_type;

typedef PACKED struct
{
  byte cmd_code;        /* Command code                   */
  byte sub_cmd;         /* Use diag_config_comm_enum_type */

  PACKED union
  {
    boolean success;        /* TRUE if baud rate change ACK   */
    byte num_options;       /* Number of options following    */
  }
  cmd_specific;

  /* The following only exist if responding to an info request.    */

  dword baudrate[4];        /* Baud rates available           */
  /* Only used if num_options is nonzero */
}
diag_config_comm_rsp_type;

/*===========================================================================

FUNCTION DIAGCOMM_CONFIG_COMM

DESCRIPTION
  This procedure processes a handset emulation request for bitmap
  screen data request.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

============================================================================*/
PACKED void *
diagcomm_config_comm (PACKED void *req_pkt, word pkt_len)
{
  diag_config_comm_req_type *req = (diag_config_comm_req_type *) req_pkt;
  diag_config_comm_rsp_type *rsp;
  dword rsp_len = FPOS (diag_config_comm_rsp_type, cmd_specific);
  sio_bitrate_type new_bitrate;

  /* First, we must find out what command we are servicing */
  switch (req->sub_cmd)
  {
  case DIAG_RS232_CONFIG_INFO:  /* Send available baudrates */
    /* Check for valid packet length */
    if (pkt_len != FSIZ (diag_config_comm_req_type, cmd_code) +
    FSIZ (diag_config_comm_req_type, sub_cmd))
    {
      return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len);
    }

    rsp_len = sizeof (diag_config_comm_rsp_type);
    break;

  case DIAG_RS232_CONFIG:   /* Change baudrate */
    rsp_len += FSIZ (diag_config_comm_rsp_type, cmd_specific.success);
    break;

  default:
    /* Do nothing */
    break;
  }

  rsp = (diag_config_comm_rsp_type *) diagpkt_alloc (DIAG_CONFIG_COMM_F, rsp_len);

  if(rsp == NULL)
    return(rsp);

  rsp->sub_cmd = req->sub_cmd;

  switch (req->sub_cmd)
  {
  case DIAG_RS232_CONFIG_INFO:  /* Send available baudrates */

    /* Hardcode for now.  More dynamic later if necessary. */
    rsp->cmd_specific.num_options = 4;

    rsp->baudrate[0] = 19200;
    rsp->baudrate[1] = 38400;
    rsp->baudrate[2] = 57600;
    rsp->baudrate[3] = 115200;

    break;

  case DIAG_RS232_CONFIG:   /* Change baudrate */

    rsp->cmd_specific.success = TRUE;

    switch (req->baudrate)
    {
    case 19200:
      new_bitrate = SIO_BITRATE_19200;
      break;
    case 38400:
      new_bitrate = SIO_BITRATE_38400;
      break;
    case 57600:
      new_bitrate = SIO_BITRATE_57600;
      break;
    case 115200:
      new_bitrate = SIO_BITRATE_115200;
      break;
    default:
      rsp->cmd_specific.success = FALSE;
      break;
    }
    break;

  default:
    /* Do nothing */
    break;
  }

  if (rsp->cmd_specific.success == TRUE)
  {
    /* Prior to flush, we must commit the packet. */
    diagpkt_commit (rsp);

    rsp = NULL;

    /* If streaming, we need to drain the buffer to ensure
       the ACK is sent.  We will also stop streaming. */
    diagbuf_flush ();

#ifdef FEATURE_DIAG_MULTI_PORTS
  if(diagcomm_sio_stream_id != SIO_NO_STREAM_ID)
#endif
    sio_ioctl (diagcomm_sio_stream_id,
           SIO_IOCTL_CHANGE_BAUD_RATE,
           (sio_ioctl_param_type *) & new_bitrate);
  }

  return (rsp);
}               /* diagcomm_config_comm */


#ifdef FEATURE_DATA
/*==========================================================================

PACKET   DIAG_SERIAL_CHG_F

PURPOSE  Request sent from the DM to the DMSS to change the serial port's
         mode back to Data Services

============================================================================*/
DIAGPKT_RSP_DEFINE (DIAG_SERIAL_CHG_F)
     byte mode_changed;     /* Boolean whether mode changed */

DIAGPKT_RSP_END
/*===========================================================================

FUNCTION DIAGCOMM_MODE_CHG_PKT

DESCRIPTION
  This procedure switches Diag's serial port back to Data Services mode

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

============================================================================*/
PACKED void *
diagcomm_mode_chg_pkt (PACKED void *req_pkt, uint16 pkt_len)
{
  DIAG_SERIAL_CHG_F_rsp_type *rsp;
  const uint32 rsp_len = sizeof (DIAG_SERIAL_CHG_F_rsp_type);

  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */

  rsp = (DIAG_SERIAL_CHG_F_rsp_type *) diagpkt_alloc (DIAG_SERIAL_CHG_F, rsp_len);

  if(rsp == NULL)
    return(rsp);

  rsp->mode_changed = TRUE;

  diagpkt_commit (rsp);
  rsp = NULL;

  /* Flush DIAG's TX buffer, make sure the packet gets sent before
     we close down */
  diagbuf_flush ();

#if defined (FEATURE_RUNTIME_DEVMAP)
  diagcomm_dancing_to_data_mode();
#else
  /* At checkin time, it was understood that FEATURE_RUNTIME_DEVMAP
     would never be left undefined.  If you do not wish to use the
     runtime devmapper, you will have to bring over the code from
     diag.c/diag_task() in the MSM_DIAG VU.  You're looking for the
     section wrapped with #if !defined( FEATURE_RUNTIME_DEVMAP ),
     triggered by the variable diag_close_flag being nonzero. */

#endif /* FEATURE_RUNTIME_DEVMAP */

  return (rsp);
}
#endif /* FEATURE_DATA */
 

static const diagpkt_user_table_entry_type diagcomm_tbl[] = {
#ifdef FEATURE_DATA
  {DIAG_SERIAL_CHG_F, DIAG_SERIAL_CHG_F, diagcomm_mode_chg_pkt},
#endif
  {DIAG_CONFIG_COMM_F, DIAG_CONFIG_COMM_F, diagcomm_config_comm}
};

/*===========================================================================

FUNCTION DIAGCOMM_INIT

DESCRIPTION
  This function initializes the diagnostic subsystem's communcations layer.

===========================================================================*/
void
diagcomm_init (void)
{
#ifdef FEATURE_DIAG_MULTI_PORTS
  int port_count;
#ifdef FEATURE_ACCESSORY_ATTACHMENT_MANAGER
#error code not present
#endif
#endif
    
  diag_channel_connected = TRUE;

  /* Register packets with the diag dispatcher */
  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAGPKT_NO_SUBSYS_ID, diagcomm_tbl);

  /* Init callbacks, etc */
  diagcomm_params.inbound_cb = NULL;
  diagcomm_params.tx_start_cb = NULL;
  diagcomm_params.tx_stop_cb = NULL;

  /* Make sure we are closed */
  #ifdef FEATURE_DIAG_MULTI_PORTS
  if(diagcomm_connected) {
  diagcomm_port_close_mask |= DIAGCOMM_PORT_UART1_BM 
      | DIAGCOMM_PORT_USB_DIAG_BM 
      #ifdef FEATURE_DIAG_SUPPORT_WIRELESS
      | DIAGCOMM_PORT_BT_SPP_BM
      #endif
      ;
  diagcomm_close (NULL);
  }
  #else
  diagcomm_close (NULL);
  #endif /* FEATURE_DIAG_MULTI_PORTS */


#ifndef FEATURE_DIAG_MULTI_PORTS
  /* Initialize watermark structures */
  memset (&diagcomm_sio_rx_wmq, 0, sizeof (diagcomm_sio_rx_wmq));
  memset (&diagcomm_sio_tx_wmq, 0, sizeof (diagcomm_sio_tx_wmq));
#else
  for(port_count = PORT_NONE+1; port_count < PORT_MAX; port_count++) 
  {
    memset (&diagcomm_sio_rx_wmq[port_count], 0, sizeof (diagcomm_sio_rx_wmq));
    memset (&diagcomm_sio_tx_wmq[port_count], 0, sizeof (diagcomm_sio_tx_wmq));
  }
#endif /* FEATURE_DIAG_MULTI_PORTS */

#ifndef FEATURE_DIAG_MULTI_PORTS
  /* Set up Queue for receiving data from SIO. */
  (void) q_init (&diagcomm_sio_rx_q);

  diagcomm_sio_rx_wmq.q_ptr = &diagcomm_sio_rx_q;

  /* Set up watermarks for WaterMark Queue.  Also, set-up
     functions which will be called when the watermarks are
     reached. */
  /* Watermarks are not to be used, but the queue is; use inert values. */
  diagcomm_sio_rx_wmq.lo_watermark = DSM_DIAG_ITEM_CNT * DSM_DIAG_ITEM_SIZ;
  diagcomm_sio_rx_wmq.hi_watermark = DSM_DIAG_ITEM_CNT * DSM_DIAG_ITEM_SIZ;
  diagcomm_sio_rx_wmq.dont_exceed_cnt = DSM_DIAG_ITEM_CNT * DSM_DIAG_ITEM_SIZ;

  /* WaterMark Queue starts out empty. */
  diagcomm_sio_rx_wmq.current_cnt = 0;

#else
  for(port_count = PORT_NONE+1; port_count < PORT_MAX; port_count++) 
  {
    /* Set up Queue for receiving data from SIO. */
    (void) q_init (&diagcomm_sio_rx_q[port_count]);

    diagcomm_sio_rx_wmq[port_count].q_ptr = &diagcomm_sio_rx_q[port_count];
    /* Set up watermarks for WaterMark Queue.  Also, set-up
       functions which will be called when the watermarks are
       reached. */
    /* Watermarks are not to be used, but the queue is; use inert values. */
    diagcomm_sio_rx_wmq[port_count].lo_watermark = DSM_DIAG_ITEM_CNT * DSM_DIAG_ITEM_SIZ;
    diagcomm_sio_rx_wmq[port_count].hi_watermark = DSM_DIAG_ITEM_CNT * DSM_DIAG_ITEM_SIZ;
    diagcomm_sio_rx_wmq[port_count].dont_exceed_cnt = DSM_DIAG_ITEM_CNT * DSM_DIAG_ITEM_SIZ;

    /* WaterMark Queue starts out empty. */
    diagcomm_sio_rx_wmq[port_count].current_cnt = 0;
  }
#endif /* FEATURE_DIAG_MULTI_PORTS */


#ifndef FEATURE_DIAG_MULTI_PORTS

  /* Set up Queue for sending data via SIO. */
  (void) q_init (&diagcomm_sio_tx_q);

  diagcomm_sio_tx_wmq.q_ptr = &diagcomm_sio_tx_q;

  /* Set up watermarks for WaterMark Queue.  Also, set-up functions
     which will be called when the watermarks are reached. */

  /* Watermarks are not to be used, but the queue is; use inert values. */
  diagcomm_sio_tx_wmq.lo_watermark = DSM_DIAG_ITEM_CNT * DSM_DIAG_ITEM_SIZ;
  diagcomm_sio_tx_wmq.hi_watermark = DSM_DIAG_ITEM_CNT * DSM_DIAG_ITEM_SIZ;
  diagcomm_sio_tx_wmq.dont_exceed_cnt = DSM_DIAG_ITEM_CNT * DSM_DIAG_ITEM_SIZ;

  /* WaterMark Queue starts out empty. */
  diagcomm_sio_tx_wmq.current_cnt = 0;

#else
  for(port_count = PORT_NONE+1; port_count < PORT_MAX; port_count++) 
  {
    /* Set up Queue for sending data via SIO. */
    (void) q_init (&diagcomm_sio_tx_q[port_count]);

    diagcomm_sio_tx_wmq[port_count].q_ptr = &diagcomm_sio_tx_q[port_count];

    /* Set up watermarks for WaterMark Queue.  Also, set-up functions
     which will be called when the watermarks are reached. */

    /* Watermarks are not to be used, but the queue is; use inert values. */
    diagcomm_sio_tx_wmq[port_count].lo_watermark = DSM_DIAG_ITEM_CNT * DSM_DIAG_ITEM_SIZ;
    diagcomm_sio_tx_wmq[port_count].hi_watermark = DSM_DIAG_ITEM_CNT * DSM_DIAG_ITEM_SIZ;
    diagcomm_sio_tx_wmq[port_count].dont_exceed_cnt = DSM_DIAG_ITEM_CNT * DSM_DIAG_ITEM_SIZ;

    /* WaterMark Queue starts out empty. */
    diagcomm_sio_tx_wmq[port_count].current_cnt = 0;
  }
#endif /* FEATURE_DIAG_MULTI_PORTS */


  /* Register the DSM events we are interested in.
     These events are used for TX flow control. */

  dsm_reg_mem_event_cb (DSM_DIAG_ITEM_POOL, DSM_MEM_LEVEL_MANY,
            DSM_MEM_OP_FREE, diagcomm_sio_tx_start_cb);

  dsm_reg_mem_event_cb (DSM_DIAG_ITEM_POOL, DSM_MEM_LEVEL_FEW,
            DSM_MEM_OP_NEW, diagcomm_sio_tx_stop_cb);

#if defined (FEATURE_RUNTIME_DEVMAP)
#ifndef FEATURE_DIAG_MULTI_PORTS
  /* Register callbacks with RDM */
  rdm_register_open_func (RDM_DIAG_SRVC, diagcomm_sio_open_cb);
  rdm_register_close_func (RDM_DIAG_SRVC, diagcomm_sio_close_cb);
#else
  for(port_count = PORT_NONE+1; port_count < PORT_MAX; port_count++)
      diagcomm_multi_sio_stream_id[port_count] = SIO_NO_STREAM_ID;
  rdm_register_func_multi_dev (RDM_DIAG_SRVC, diagcomm_sio_open_cb, diagcomm_sio_close_cb, NULL);

#ifdef FEATURE_ACCESSORY_ATTACHMENT_MANAGER
#error code not present
#endif
#endif /* FEATURE_DIAG_MULTI_PORTS */
#endif /* FEATURE_RUNTIME_DEVMAP */

  /* Now prepare parameters for opening a port */
  diagcomm_sio_open_params.stream_mode = SIO_DM_MODE;

#ifndef FEATURE_DIAG_MULTI_PORTS
  diagcomm_sio_open_params.rx_queue = &diagcomm_sio_rx_wmq;
  diagcomm_sio_open_params.tx_queue = &diagcomm_sio_tx_wmq;
#endif

#if defined (FEATURE_RUNTIME_DEVMAP)
  diagcomm_sio_open_params.port_id = SIO_PORT_NULL;
#else
  diagcomm_sio_open_cb (SIO_PORT_NULL);
#endif

  /* Normally, this layer doesn't know about the HDLC encoding of the diag
     protocol.  However, SIO is configured to look for tail character. */
  diagcomm_sio_open_params.tail_char_used = TRUE;
  diagcomm_sio_open_params.tail_char = DIAG_ASYNC_CONTROL_CHAR;

#ifndef FEATURE_DIAG_MULTI_PORTS
  diagcomm_sio_open_params.rx_func_ptr = diagcomm_sio_inbound_pkt_notify;
#endif

  /* Use whatever the flow control method is currently. */
  diagcomm_sio_open_params.rx_flow = SIO_FCTL_OFF;
  diagcomm_sio_open_params.tx_flow = SIO_FCTL_OFF;

  /* Default DM bit rate is 38.4Kbps unless specified in NV.
     Now, it uses 115.2 (the max) as the default. */
  diagcomm_sio_open_params.rx_bitrate = SIO_BITRATE_115200;
  diagcomm_sio_open_params.tx_bitrate = SIO_BITRATE_115200;
}               /* diagcomm_init */


#ifdef FEATURE_DIAG_MULTI_PORTS

/*===========================================================================

FUNCTION DIAGCOMM_USB_DIAG_SIO_INBOUND_PKT_NOTIFY

DESCRIPTION
  This function is called when data is received on the USB_DIAG port.
  If no other port is active, the data is passed to the actual inbound_pkt_
  notify function; or else the data is dropped.
  port parameter passed in.

===========================================================================*/

void
diagcomm_usb_diag_sio_inbound_pkt_notify (dsm_item_type ** item_ptr)
{
  if(diagcomm_sio_stream_id == SIO_NO_STREAM_ID 
#ifdef FEATURE_ACCESSORY_ATTACHMENT_MANAGER
#error code not present
#endif
     )
  {
      diagcomm_sio_stream_id = diagcomm_multi_sio_stream_id[PORT_USB_DIAG];
      diagcomm_active_port = PORT_USB_DIAG;
  }

  if (diagcomm_sio_stream_id == diagcomm_multi_sio_stream_id[PORT_USB_DIAG])
      diagcomm_sio_inbound_pkt_notify(item_ptr);
  else
      dsm_free_packet (item_ptr);
}

/*===========================================================================

FUNCTION DIAGCOMM_UART1_SIO_INBOUND_PKT_NOTIFY

DESCRIPTION
  This function is called when data is received on the UART_MAIN port.
  If no other port is active, the data is passed to the actual inbound_pkt_
  notify function; or else the data is dropped.
  port parameter passed in.

===========================================================================*/

void
diagcomm_uart1_sio_inbound_pkt_notify (dsm_item_type ** item_ptr)
{
  if(diagcomm_sio_stream_id == SIO_NO_STREAM_ID 
#ifdef FEATURE_ACCESSORY_ATTACHMENT_MANAGER
#error code not present
#endif
     )
  {
      diagcomm_sio_stream_id = diagcomm_multi_sio_stream_id[PORT_UART1];
      diagcomm_active_port = PORT_UART1;
  }

  if (diagcomm_sio_stream_id == diagcomm_multi_sio_stream_id[PORT_UART1])
      diagcomm_sio_inbound_pkt_notify(item_ptr);
  else
      dsm_free_packet (item_ptr);
}

#if (defined(FEATURE_BT) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
/*===========================================================================

FUNCTION DIAGCOMM_BT_SPP_SIO_INBOUND_PKT_NOTIFY

DESCRIPTION
  This function is called when data is received on the BT_SPP port.
  If no other port is active, the data is passed to the actual inbound_pkt_
  notify function; or else the data is dropped.
  port parameter passed in.

===========================================================================*/

void
diagcomm_bt_spp_sio_inbound_pkt_notify (dsm_item_type ** item_ptr)
{
  if(diagcomm_sio_stream_id == SIO_NO_STREAM_ID)
  {
      diagcomm_sio_stream_id = diagcomm_multi_sio_stream_id[PORT_BT_SPP];
      diagcomm_active_port = PORT_BT_SPP;
  }

  if (diagcomm_sio_stream_id == diagcomm_multi_sio_stream_id[PORT_BT_SPP])
      diagcomm_sio_inbound_pkt_notify(item_ptr);
  else
      dsm_free_packet (item_ptr);
}
#endif

#if (defined(FEATURE_IRDA) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
#error code not present
#endif

/*===========================================================================

 FUNCTION DIAGCOMM_RESET_SIO

 DESCRIPTION
   Diag communication has been broken. We should reset the sio_stream_id
   to be NULL
===========================================================================*/

void
diagcomm_reset_sio (void)
{
  diagcomm_sio_stream_id = SIO_NO_STREAM_ID;
}

/*===========================================================================

FUNCTION DIAGCOMM_GET_MULTI_SIO_STREAM_ID

DESCRIPTION
  This function returns the stream_id for the passed in port parameter.

===========================================================================*/

sio_stream_id_type diagcomm_get_multi_sio_stream_id (diagcomm_sio_port_e_type diagcomm_port)
{
  return diagcomm_multi_sio_stream_id[diagcomm_port];
}

/*===========================================================================

FUNCTION DIAGCOMM_SET_SIO_STREAM_ID

DESCRIPTION
  This function sets the diagcomm_sio_stream_id to the stream_id of the
  port parameter passed in.

===========================================================================*/

void diagcomm_set_sio_stream_id (diagcomm_sio_port_e_type diagcomm_port)
{
  diagcomm_sio_stream_id = diagcomm_multi_sio_stream_id[diagcomm_port];
  diagcomm_active_port = diagcomm_port;
}


#ifdef FEATURE_ACCESSORY_ATTACHMENT_MANAGER
#error code not present
#endif /* FEATURE_ACCESSORY_ATTACHMENT_MANAGER */
#endif /* FEATURE_DIAG_MULTI_PORTS */
/*lint -restore */  
