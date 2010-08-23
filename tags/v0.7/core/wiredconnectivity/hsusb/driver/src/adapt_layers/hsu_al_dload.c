/*==============================================================================

High Speed USB DLOAD Adaptation Layer

GENERAL DESCRIPTION
Provides High-Speed USB support to DLOAD.

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

#ifndef BUILD_HOSTDL

#include "hsu_al_dload.h"
#include "hsu_al_dload_armprg_common.h"
#include "jtypes.h"
#include "hsu_common.h"
#include "port.h"
#include "uw_args.h"
#include "uw_device_cdc_obex.h"
#include "uw_device_cdc.h"
#include "jslave_init.h"
#include "msg.h"
#include "hsu_mdm_apis.h"
#include "boothw.h"
#include "hsu_bus_mode_notifier.h"
#include "hsu_al_oemsbl_stack_sel.h"

/*******************************************************************************
************************* Stubs needed to solve link problems *****************/
/* Needed by the DBG_* macros */
void msg_send_var (const msg_const_type * const_blk, uint32 num_args, ...)
{
  HSU_USE_PARAM(const_blk);
  HSU_USE_PARAM(num_args);
}

/******************************************************************************/

static cdc_handle_t hsu_al_dload_cdc_handle;

/* Index for putting next byte in hsu_al_dload_transmit_byte() */
static uint32 hsu_al_dload_index_in_tx_buf;

/* Index for getting next byte from the buffer in hsu_al_dload_receive_byte() */
static uint32 hsu_al_dload_index_in_rx_buf;

/* Index (0..1) of rx buffer used for cdc_read */
static uint8 hsu_al_dload_rx_buf_index;

/* Amount of bytes read in last cdc_read(). */
static uint32 hsu_al_dload_rx_num_bytes_read;

static boolean hsu_al_dload_write_is_complete;

/* indicates invalid rx_buf index */
#define HSU_AL_DLOAD_RX_BUF_INDEX_INVALID 0xff

/* store canceled rx_buf index upon USB suspend */
static uint8 hsu_al_dload_canceled_rx_buf_index = HSU_AL_DLOAD_RX_BUF_INDEX_INVALID;

#define HSU_AL_DLOAD_OBEX_SUBCLASS         0xFF
#define HSU_AL_DLOAD_OBEX_PROTOCOL         0xFF

/*===========================================================================

FUNCTION hsu_al_dload_reset_data

DESCRIPTION
This function resets the buffer-related indices.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_al_dload_reset_data()
{
  hsu_al_dload_index_in_rx_buf = 0;
  hsu_al_dload_index_in_tx_buf = 0;
  hsu_al_dload_rx_buf_index = 0;
  hsu_al_dload_rx_num_bytes_read = 0;
}

/*******************************************************************************
* CDC callbacks
*******************************************************************************/
/*===========================================================================

FUNCTION hsu_al_dload_cdc_read_complete_cb

DESCRIPTION
CDC read completion callback.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_al_dload_cdc_read_complete_cb
(
  cdc_iostatus_t status, 
  void *buffer,
  juint32_t size, 
  jbool_t transfer_complete, 
  cdc_appctx_t ctx
)
{
  jresult_t jresult;

  HSU_USE_PARAM(buffer);
  HSU_USE_PARAM(transfer_complete);
  HSU_USE_PARAM(ctx);

  if (status != CDC_TRANSFER_OK)
  {
    if (status == CDC_TRANSFER_CANCELLED)
    {
      /* store the index of the canceled rx_buf. This same rx_buf will be
         given to HW upon USB resume */
      hsu_al_dload_canceled_rx_buf_index = hsu_al_dload_rx_buf_index;
      return;
    }

    HSU_ASSERT("Read completion status is not OK or CANCELLED" == NULL);
    return;
  }

  hsu_al_dload_rx_num_bytes_read = size;

  /* Toggle rx buffer (double buffering is used) */
  hsu_al_dload_rx_buf_index = (hsu_al_dload_rx_buf_index + 1) % 2;

  /* Setup a new transfer */
  jresult = cdc_read(
    hsu_al_dload_cdc_handle, 
    hsu_al_dload_armprg_rx_buffer[hsu_al_dload_rx_buf_index], 
    HSU_AL_DLOAD_ARMPRG_RX_BUFFER_SIZE
    );
  
  HSU_ASSERT(jresult==0);
}

/*===========================================================================

FUNCTION hsu_al_dload_cdc_write_complete_cb

DESCRIPTION
CDC write completion callback.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_al_dload_cdc_write_complete_cb
(
  cdc_iostatus_t status, 
  void *buffer,
  cdc_appctx_t ctx
)
{
  HSU_USE_PARAM(buffer);
  HSU_USE_PARAM(status);
  HSU_USE_PARAM(ctx);

  hsu_al_dload_write_is_complete = TRUE;
}

/*===========================================================================

FUNCTION hsu_al_dload_cdc_enabled_cb

DESCRIPTION
CDC enabled callback - called when the function driver
is enabled (after enumeration is finished).

DEPENDENCIES
None.

RETURN VALUE
0 for success, error number for failure..

SIDE EFFECTS
None.
===========================================================================*/
static jresult_t hsu_al_dload_cdc_enabled_cb(cdc_appctx_t ctx)
{
  jresult_t jresult;

  HSU_USE_PARAM(ctx);

  hsu_al_dload_reset_data();
  jresult = cdc_read(
    hsu_al_dload_cdc_handle, 
    hsu_al_dload_armprg_rx_buffer[hsu_al_dload_rx_buf_index], 
    HSU_AL_DLOAD_ARMPRG_RX_BUFFER_SIZE
    );
  
  HSU_ASSERT(jresult==0);
  
  return 0;
}

/*===========================================================================

FUNCTION hsu_al_dload_cdc_disabled_cb

DESCRIPTION
CDC disabled callback - called when the function driver
is disabled (e.g., when the stack is disabled).

DEPENDENCIES
None.

RETURN VALUE
0 for success, error number for failure.

SIDE EFFECTS
None.
===========================================================================*/
static jresult_t hsu_al_dload_cdc_disabled_cb(cdc_appctx_t ctx)
{
  HSU_USE_PARAM(ctx);

  return 0;
}

/******************************************************************************/

/*===========================================================================

FUNCTION hsu_al_dload_obex_init

DESCRIPTION
OBEX initialization function - called by hsu_al_oemsbl_stack_sel, which is 
called by the function driver on initialization, sets application context 
and callbacks.

DEPENDENCIES
None.

RETURN VALUE
0 for success, error number for failure.

SIDE EFFECTS
None.
===========================================================================*/
jresult_t hsu_al_dload_obex_init
(
  cdc_handle_t handle, 
  obex_callbacks_t* cbs, 
  obex_init_info_t* info, 
  cdc_appctx_t* ctx
)
{
  /* Only one instance of the function driver, so ctx is not important */
  *ctx = (cdc_appctx_t)(1);

  hsu_al_dload_cdc_handle = handle;

  cbs->basic.read_complete_h = hsu_al_dload_cdc_read_complete_cb;
  cbs->basic.write_complete_h = hsu_al_dload_cdc_write_complete_cb;
  cbs->basic.enabled_h = hsu_al_dload_cdc_enabled_cb;
  cbs->basic.disabled_h = hsu_al_dload_cdc_disabled_cb;
  cbs->basic.uninit_h = NULL;

  /* These will never be used in OBEX: */
  cbs->basic.encap_command_h = NULL;
  cbs->basic.encap_response_complete_h = NULL;

  /* Init info struct */
  info->basic.flags = (juint32_t)(CDC_INIT_BUFFER_DMABLE | 
                                  CDC_INIT_SINGLE_INTERFACE |
                                  CDC_INIT_VENDOR_SPECIFIC_CLASS |
                                  CDC_INIT_NO_CLASS_DESC |
                                  CDC_INIT_NO_ALT_SETTINGS);

  info->basic.cci_subclass = HSU_AL_DLOAD_OBEX_SUBCLASS;
  info->basic.cci_protocol = HSU_AL_DLOAD_OBEX_PROTOCOL;

  return 0;
}

/*===========================================================================

FUNCTION hsu_al_dload_drain

DESCRIPTION
This function waits for the completion of any pending IN transfers.

DEPENDENCIES

RETURN VALUE
None.

SIDE EFFECTS
None.

===========================================================================*/
void hsu_al_dload_drain(void)
{
  while (!hsu_al_dload_write_is_complete)
  {
    hsu_al_dload_armprg_handle_interrupts();
  }
}

/*===========================================================================

FUNCTION hsu_al_dload_init

DESCRIPTION
This function initializes HS-USB support for DLOAD.

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/
void hsu_al_dload_init(void)
{
  hsu_al_dload_init_ex(TRUE);
}

/*===========================================================================

FUNCTION HSU_AL_DLOAD_BUS_MODE_CB

DESCRIPTION
This function is registered as callback for USB suspend/resume events

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/
static void hsu_al_dload_bus_mode_cb(hsu_bus_mode_enum new_mode, void* app_data)
{
  jresult_t jresult;

  HSU_USE_PARAM(app_data);

  if ((new_mode == HSU_BUS_MODE_RESUMED) && 
      (hsu_al_dload_canceled_rx_buf_index != HSU_AL_DLOAD_RX_BUF_INDEX_INVALID))
  {
    /* upon USB resume, setup a new read operation instead of the one that 
       was canceled in USB suspend */
    jresult = cdc_read(
      hsu_al_dload_cdc_handle, 
      hsu_al_dload_armprg_rx_buffer[hsu_al_dload_canceled_rx_buf_index], 
      HSU_AL_DLOAD_ARMPRG_RX_BUFFER_SIZE);
  
    HSU_ASSERT(jresult==0);

    hsu_al_dload_canceled_rx_buf_index = HSU_AL_DLOAD_RX_BUF_INDEX_INVALID;
  }
}

/*===========================================================================

FUNCTION hsu_al_dload_init_ex

DESCRIPTION
This function initializes HS-USB support for DLOAD.
It receives a parameter that specifies whether to enumerate
using the same product ID used when running in AMSS (non-download mode).

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/
void hsu_al_dload_init_ex(boolean use_amss_product_id)
{
  jresult_t     jresult;
  hsu_bus_mode_cb_info_struct bus_mode_cb_info;

  /* We don't know yet whether DLOAD will work over USB or not. */
  hsu_al_dload_armprg_common_unmark_dload_used_hsu();

  /* May need to kick the dog here since initialization may take a while */
  BOOTHW_KICK_WATCHDOG();

  hsu_api_init_phy();

  bus_mode_cb_info.bus_mode_cb = hsu_al_dload_bus_mode_cb;
  bus_mode_cb_info.app_data = NULL;
  hsu_register_bus_mode_notif_cb(bus_mode_cb_info);

  /* Set the stack mode to DLOAD, such that OBEX FD will call DLOAD
     implementation of obex_init */
  hsu_al_oemsbl_stack_sel_set_mode(HSU_AL_OEMSBL_STACK_MODE_DLOAD);

  jresult = hsu_al_dload_armprg_start_stack_ex(use_amss_product_id);
  HSU_ASSERT(jresult == 0);
}

/*===========================================================================

FUNCTION hsu_al_dload_receive_byte

DESCRIPTION
This function receives incoming data from the USB bulk OUT EP and
returns one character at a time to the calling function. Though it receives
a bigger packet at once, it always returns one character to the calling function.
This approach is chosen to have a consistency between the UART and USB modules.

DEPENDENCIES

RETURN VALUE
Returns the next character from the receive buffer, or USB_NO_CHAR if there's
no character to return.

SIDE EFFECTS

===========================================================================*/
int hsu_al_dload_receive_byte(void)
{
  uint8 n;

  if (hsu_al_dload_index_in_rx_buf == hsu_al_dload_rx_num_bytes_read)
  {
    /* This means that all the bytes received from the host
    ** were already given to the dload algorithm via calls to
    ** hsu_al_dload_receive_byte.
    */
    hsu_al_dload_index_in_rx_buf = 0;

    /* Time to get new data from the EP (if it is there already) */
    hsu_al_dload_rx_num_bytes_read = 0;

    /* If enumeration is not done yet, this will cause
    ** setup messages to get processed.
    */
    hsu_al_dload_armprg_handle_interrupts();

    /* If the data was there, hsu_al_dload_rx_num_bytes_read should have
    ** been updated (by the read callback).
    */
    if (hsu_al_dload_rx_num_bytes_read == 0)
    {
      /* No data yet */
      return USB_NO_CHAR;
    }
  }

  /* The next byte should be read from the buffer that is not used for cdc_read */
  if (hsu_al_dload_rx_buf_index == 0)
  {
    n = 1;
  }
  else
  {
    n = 0;
  }

  return (int) (hsu_al_dload_armprg_rx_buffer[n][hsu_al_dload_index_in_rx_buf++]);
}

/*===========================================================================

FUNCTION hsu_al_dload_transmit_byte

DESCRIPTION
This function adds a byte to the transmit buffer. If the byte signifies
the end of a packet, the buffer is transmitted to the host.

DEPENDENCIES

RETURN VALUE
None.

SIDE EFFECTS

===========================================================================*/
void hsu_al_dload_transmit_byte(byte chr)
{
  jresult_t jresult;

  HSU_ASSERT(hsu_al_dload_index_in_tx_buf < HSU_AL_DLOAD_ARMPRG_TX_BUFFER_SIZE);
  if (hsu_al_dload_index_in_tx_buf >= HSU_AL_DLOAD_ARMPRG_TX_BUFFER_SIZE)
  {
    /* Prevent buffer overflow.
    ** This only happens if DLOAD tries to transmit a packet larger
    ** than the buffer.
    */
    return;
  }

  /* Copy the byte to the transmit buffer. */
  hsu_al_dload_armprg_tx_buffer[hsu_al_dload_index_in_tx_buf++] = chr;

  /* Check for terminating character */
  if ((chr == 0x7E) && (hsu_al_dload_index_in_tx_buf != 1))
  {
    BOOTHW_KICK_WATCHDOG ();

    /* Send the buffer */
    hsu_al_dload_write_is_complete = FALSE;
    jresult = cdc_write(hsu_al_dload_cdc_handle, hsu_al_dload_armprg_tx_buffer, hsu_al_dload_index_in_tx_buf);
    HSU_ASSERT(jresult==0);

    /* No need to wait for write to be complete. 
    ** The next time the DLOAD algorithm exhausts its receive buffer and checks
    ** the interrupt register, the write completion will be detected.
    ** Thus, we can reset the index to the transmit buffer, since 
    ** hsu_al_dload_transmit_byte() will not get called again until we're done
    ** sending the response and then get a new command from the host.
    ** (Unless the host can retry sending a command after not getting a status)
    */
    hsu_al_dload_index_in_tx_buf = 0;
  }
}

/*===========================================================================

FUNCTION hsu_al_dload_receive_pkt

DESCRIPTION
This function receives incoming data from the USB bulk OUT EP and
returns the contents of the receive buffer to the calling function.
It is up to the calling function to parse the data
A pointer to the packet is stored in the argument passed
by the caller and the length in bytes is returned.

DEPENDENCIES

RETURN VALUE
Returns a pointer to the received packet in the argument passed
by the calling function and returns the length of the packet in bytes.
Returns NULL and 0 if there is no data in the receive buffer.

SIDE EFFECTS

===========================================================================*/
uint32 hsu_al_dload_receive_pkt(byte **buf)
{
  if (hsu_al_dload_index_in_rx_buf == hsu_al_dload_rx_num_bytes_read)
  {
    /* This means that all the bytes received from the host
    ** were already given to the dload algorithm via calls to
    ** hsu_al_dload_receive_byte or hsu_al_dload_receive_pkt.
    */
    hsu_al_dload_index_in_rx_buf = 0;

    /* Time to get new data from the EP (if it is there already) */
    hsu_al_dload_rx_num_bytes_read = 0;

    /* If enumeration is not done yet, this will cause
    ** setup messages to get processed.
    */
    hsu_al_dload_armprg_handle_interrupts();

    /* If the data was there, hsu_al_dload_rx_num_bytes_read should have
    ** been updated (by the read callback).
    */
    if (hsu_al_dload_rx_num_bytes_read == 0)
    {
      /* No data yet */
      *buf = NULL;
      return (0);
    }
  
    /* We got data. This means that read callback was called, and it toggled
    ** hsu_al_dload_rx_buf_index and called cdc_read.
    */
    *buf = hsu_al_dload_armprg_rx_buffer[(hsu_al_dload_rx_buf_index + 1) % 2];
  }

  /* tell the driver we read all bytes in the receive buffer */
  hsu_al_dload_index_in_rx_buf = hsu_al_dload_rx_num_bytes_read;
  /* return the number of bytes read */
  return(hsu_al_dload_rx_num_bytes_read);
} /* end of hsu_al_dload_receive_pkt() */

/*===========================================================================

FUNCTION hsu_al_dload_transmit_pkt

DESCRIPTION
This function buffers the pkt into the USB tx buffer, and initiates transmitting
it over the USB. This is not a generic routine and it only
transmits packets that are smaller or equal to the USB tx buffer.
Other packets are discarded.

DEPENDENCIES
The USB device must be enumerated. Packet fits in the tx buffer

RETURN VALUE
None.

SIDE EFFECTS

===========================================================================*/
void hsu_al_dload_transmit_pkt
(
  byte *pkt, // pointer to pkt to be transmitted
  uint32 len // number of bytes to tx
)
{
  jresult_t jresult;

  /* if packet bigger than tx buffer size, return */
  if (len > HSU_AL_DLOAD_ARMPRG_TX_BUFFER_SIZE) 
  {
    HSU_ASSERT("hsu_al_dload_transmit_pkt: Packet is larger than the tx buffer!" == NULL);
    return;
  }
  /* Copy the pkt to the transmit buffer. */
  memcpy( &hsu_al_dload_armprg_tx_buffer[hsu_al_dload_index_in_tx_buf],pkt, len); 
  hsu_al_dload_index_in_tx_buf+= len;

  /* Send the buffer */
  hsu_al_dload_write_is_complete = FALSE;
  jresult = cdc_write(hsu_al_dload_cdc_handle, hsu_al_dload_armprg_tx_buffer, hsu_al_dload_index_in_tx_buf);
  HSU_ASSERT(jresult==0);

  /* No need to wait for write to be complete. 
  ** The next time the DLOAD algorithm exhausts its receive buffer and checks
  ** the interrupt register, the write completion will be detected.
  ** Thus, we can reset the index to the transmit buffer, since 
  ** hsu_al_dload_transmit_byte() / hsu_al_dload_transmit_pkt() will not get 
  ** called again until we're done sending the response and then get a new 
  ** command from the host.
  ** (Unless the host can retry sending a command after not getting a status)
  */
  hsu_al_dload_index_in_tx_buf = 0;
} /* end of hsu_al_dload_transmit_pkt() */

/*===========================================================================

FUNCTION hsu_al_dload_shutdown

DESCRIPTION
This function shuts down the HS-USB stack.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_dload_shutdown(void)
{
  jslave_disable(0);
  jstop_stack();
}

/*===========================================================================

FUNCTION hsu_al_dload_mark_dload_used_hsu

DESCRIPTION
This function sets a marker in IRAM so that
ARMPRG knows DLOAD worked over HS-USB.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_dload_mark_dload_used_hsu(void)
{
  hsu_al_dload_armprg_common_mark_dload_used_hsu();
}

#else /* BUILD_HOSTDL */
#include "hsu_donothing.h"
#endif /* BUILD_HOSTDL */
