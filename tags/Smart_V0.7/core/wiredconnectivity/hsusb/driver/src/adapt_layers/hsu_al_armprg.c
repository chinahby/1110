/*==============================================================================

High Speed USB ARMPRG Adaptation Layer

GENERAL DESCRIPTION
Provides High-Speed USB support to ARMPRG.

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

#ifdef BUILD_HOSTDL

#include "comdef.h"
#include "msg.h"
#include BOOTHW_MSM_H
#include "msm.h"
#include "ap_armprg.h"
#include "hsu_al_dload_armprg_common.h"
#include "jtypes.h"
#include "hsu_common.h"
#include "port.h"
#include "uw_args.h"
#include "jerrno.h"
#include "uw_device_cdc_obex.h"
#include "uw_device_cdc.h"
#include "hsu_boot_dload.h"
#include "hsu_bus_mode_notifier.h"

#include "mobile.h"

/* ARMPRG function that needs to be called when bytes are received */
extern void packet_handle_incoming (int);

#define HSU_AL_ARMPRG_OBEX_SUBCLASS         0xFF
#define HSU_AL_ARMPRG_OBEX_PROTOCOL         0xFF

/*******************************************************************************
** STUBS
*******************************************************************************/

/* Needed by ERR_FATAL, which is called by os_panic */ 
void err_fatal_core_dump 
(
 unsigned int line,      /* From __LINE__ */
 const char   *file_name, /* From __FILE__ */
 const char   *format   /* format string */
 )						  
{}

/* Needed by ERR_FATAL, which is called by os_panic */ 
void rex_jettison_core ()
{}

/* Needed by the DBG_* macros */
void msg_send_var (const msg_const_type * const_blk, uint32 num_args, ...){}

/******************************************************************************/

/******************************************************************************/ 
/*  USB function dispatch table
*******************************************************************************/ 

/* Forward declarations */
boolean hsu_al_armprg_init (void);
boolean hsu_al_armprg_active (void);
void hsu_al_armprg_drain (void);
void hsu_al_armprg_check (void);
void hsu_al_armprg_transmit_byte (byte);

/* The dispatch table */
DISP usb_dispatch_table= {
  hsu_al_armprg_init,
  hsu_al_armprg_active,
  hsu_al_armprg_drain,
  hsu_al_armprg_check,
  hsu_al_armprg_transmit_byte,
};
/******************************************************************************/

static cdc_handle_t hsu_al_armprg_cdc_handle;

/* Index for putting next byte in hsu_al_armprg_transmit_byte() */
static uint32 hsu_al_armprg_index_in_tx_buf;

/* Index (0..1) of rx buffer used for cdc_read */
static uint8 hsu_al_armprg_rx_buf_index;

static boolean hsu_al_armprg_write_is_complete;

/* indicates invalid rx_buf index */
#define HSU_AL_ARMPRG_RX_BUF_INDEX_INVALID 0xff

/* store canceled rx_buf index upon USB suspend */
static uint8 hsu_al_armprg_canceled_rx_buf_index = HSU_AL_ARMPRG_RX_BUF_INDEX_INVALID;

/*===========================================================================

FUNCTION hsu_al_armprg_reset_data

DESCRIPTION
This function resets the buffer-related indices.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_al_armprg_reset_data()
{
  hsu_al_armprg_index_in_tx_buf = 0;
  hsu_al_armprg_rx_buf_index = 0;
}

/*******************************************************************************
* CDC callbacks
*******************************************************************************/
/*===========================================================================

FUNCTION hsu_al_armprg_cdc_read_complete_cb

DESCRIPTION
CDC read completion callback.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_al_armprg_cdc_read_complete_cb
(
  cdc_iostatus_t status, 
  void *buffer,
  juint32_t size, 
  jbool_t transfer_complete, 
  cdc_appctx_t ctx
)
{
  uint8     buff_index;
  jresult_t jresult;
  uint32    i;

  if (status != CDC_TRANSFER_OK)
  {
    if (status == CDC_TRANSFER_CANCELLED)
    {
      /* store the index of the canceled rx_buf. This same rx_buf will be
         given to HW upon USB resume */
      hsu_al_armprg_canceled_rx_buf_index = hsu_al_armprg_rx_buf_index;
      return;
    }

    HSU_ASSERT("Read completion status is not OK or CANCELLED" == NULL);
    return;
  }

  /* Toggle rx buffer (double buffering is used) */
  buff_index = hsu_al_armprg_rx_buf_index;
  hsu_al_armprg_rx_buf_index = (hsu_al_armprg_rx_buf_index + 1) % 2;

  /* Setup a new transfer */
  jresult = cdc_read(
    hsu_al_armprg_cdc_handle, 
    hsu_al_dload_armprg_rx_buffer[hsu_al_armprg_rx_buf_index], 
    HSU_AL_DLOAD_ARMPRG_RX_BUFFER_SIZE
    );
  HSU_ASSERT(jresult==0);

  if (size>0)
  {
    /* Pass the received bytes to the packet module.*/
    for(i = 0; i < size; i++)
    {
      /* The data is available in the rx_buffer. So, we pass the next byte
      * based on the rx_index to the packet process function.
      */
      packet_handle_incoming((int)(hsu_al_dload_armprg_rx_buffer[buff_index][i]));
    }
  }
}

/*===========================================================================

FUNCTION hsu_al_armprg_cdc_write_complete_cb

DESCRIPTION
CDC write completion callback.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_al_armprg_cdc_write_complete_cb
(
  cdc_iostatus_t status, 
  void *buffer,
  cdc_appctx_t ctx
)
{
  hsu_al_armprg_write_is_complete = TRUE;
}

/*===========================================================================

FUNCTION hsu_al_armprg_cdc_enabled_cb

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
static jresult_t hsu_al_armprg_cdc_enabled_cb(cdc_appctx_t ctx)
{
  jresult_t jresult;

  hsu_al_armprg_reset_data();
  jresult = cdc_read(
    hsu_al_armprg_cdc_handle, 
    hsu_al_dload_armprg_rx_buffer[hsu_al_armprg_rx_buf_index], 
    HSU_AL_DLOAD_ARMPRG_RX_BUFFER_SIZE
    );
  HSU_ASSERT(jresult==0);

  return 0;
}

/*===========================================================================

FUNCTION hsu_al_armprg_cdc_disabled_cb

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
static jresult_t hsu_al_armprg_cdc_disabled_cb(cdc_appctx_t ctx)
{
  return 0;
}

/******************************************************************************/

/*===========================================================================

FUNCTION obex_init

DESCRIPTION
OBEX initialization function - called by the function driver
on initialization, sets application context and callbacks.

DEPENDENCIES
None.

RETURN VALUE
0 for success, error number for failure.

SIDE EFFECTS
None.
===========================================================================*/
jresult_t obex_init
(
  cdc_handle_t handle, 
  obex_callbacks_t* cbs, 
  obex_init_info_t* info, 
  cdc_appctx_t* ctx
)
{
  /* Only one instance of the function driver, so ctx is not important */
  *ctx = (cdc_appctx_t)(1);

  hsu_al_armprg_cdc_handle = handle;

  cbs->basic.read_complete_h = hsu_al_armprg_cdc_read_complete_cb;
  cbs->basic.write_complete_h = hsu_al_armprg_cdc_write_complete_cb;
  cbs->basic.enabled_h = hsu_al_armprg_cdc_enabled_cb;
  cbs->basic.disabled_h = hsu_al_armprg_cdc_disabled_cb;
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

  info->basic.cci_subclass = HSU_AL_ARMPRG_OBEX_SUBCLASS;
  info->basic.cci_protocol = HSU_AL_ARMPRG_OBEX_PROTOCOL;

  return 0;
}

/*===========================================================================

FUNCTION hsu_al_armprg_active

DESCRIPTION
This function supposedly checks for USB activity.
In reality it is not needed, and always returns TRUE.

DEPENDENCIES
None

RETURN VALUE
TRUE

SIDE EFFECTS
None

===========================================================================*/
boolean hsu_al_armprg_active(void)
{
  BOOTHW_KICK_WATCHDOG ();

  return TRUE;
}

/*===========================================================================

FUNCTION hsu_al_armprg_check

DESCRIPTION
This function initiates manual checking of interrupts, and calls 
the HS-USB stack's ISR if needed.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
If data arrives on the BULK OUT EP,
it is transferred to the ARMPRG packet module (this happens in the
read completion callback).

===========================================================================*/
void hsu_al_armprg_check(void)
{
  hsu_al_dload_armprg_handle_interrupts();
  /* If we're enumerating, this advances the process.
  ** If we got data from the host on the bulk out EP,
  ** this causes the read callback to be called.
  */
}

/*===========================================================================

FUNCTION hsu_al_armprg_drain

DESCRIPTION
This function waits for any pending IN transfers to end.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_armprg_drain(void)
{
  while (!hsu_al_armprg_write_is_complete)
  {
    hsu_al_armprg_check();
  }
}

/*===========================================================================

FUNCTION HSU_AL_ARMPRG_BUS_MODE_CB

DESCRIPTION
This function is registered as callback for USB suspend/resume events

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/
static void hsu_al_armprg_bus_mode_cb(hsu_bus_mode_enum new_mode, void* app_data)
{
  jresult_t jresult;

  HSU_USE_PARAM(app_data);
  
  if ((new_mode == HSU_BUS_MODE_RESUMED) && 
      (hsu_al_armprg_canceled_rx_buf_index != HSU_AL_ARMPRG_RX_BUF_INDEX_INVALID))
  {
    /* upon USB resume, setup a new read operation instead of the one that 
       was canceled in USB suspend */
    jresult = cdc_read(
      hsu_al_armprg_cdc_handle, 
      hsu_al_dload_armprg_rx_buffer[hsu_al_armprg_canceled_rx_buf_index], 
      HSU_AL_DLOAD_ARMPRG_RX_BUFFER_SIZE);
  
    HSU_ASSERT(jresult==0);

    hsu_al_armprg_canceled_rx_buf_index = HSU_AL_ARMPRG_RX_BUF_INDEX_INVALID;
  }
}

/*===========================================================================

FUNCTION hsu_al_armprg_init

DESCRIPTION
This function initializes HS-USB support for ARMPRG.

DEPENDENCIES
None

RETURN VALUE
TRUE for success, FALSE if the download did not start on HS-USB or if
initialization fails.

SIDE EFFECTS
None

===========================================================================*/
boolean hsu_al_armprg_init(void)
{
  jresult_t     jresult;
  hsu_bus_mode_cb_info_struct bus_mode_cb_info;

  /* Check if we have a version number in the IRAM data or not.
  ** The version number can be invalid if we have a new version on the ARMPRG
  ** but the AMSS ran an old version so it didn't update the version_num in 
  ** transition to DLOAD.
  ** If the version number is not valid - it means that we have an old IRAM data
  ** so we need to update it with the new variables
  */
  hsu_boot_dload_check_and_update_version_num();
  if (!hsu_al_dload_armprg_common_dload_used_hsu())
  {
    /* DLOAD ran over UART, so there's no point in ARMPRG running over
    ** USB.
    */
    return FALSE;
  }

  bus_mode_cb_info.bus_mode_cb = hsu_al_armprg_bus_mode_cb;
  bus_mode_cb_info.app_data = NULL;
  hsu_register_bus_mode_notif_cb(bus_mode_cb_info);

  jresult = hsu_al_dload_armprg_start_stack();
  if(jresult != 0)
  {
    /* No DIAG, so can't use MSG_ . */
    HSU_ASSERT(jresult == 0);
    return FALSE;
  }

  return TRUE;
}

/*===========================================================================

FUNCTION hsu_al_armprg_transmit_byte

DESCRIPTION
This function adds a byte to the transmit buffer. If the byte signifies
the end of a packet, the buffer is transmitted to the host.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None

===========================================================================*/
void hsu_al_armprg_transmit_byte(byte chr)
{
  jresult_t jresult;

  HSU_ASSERT(hsu_al_armprg_index_in_tx_buf < HSU_AL_DLOAD_ARMPRG_TX_BUFFER_SIZE);
  if (hsu_al_armprg_index_in_tx_buf >= HSU_AL_DLOAD_ARMPRG_TX_BUFFER_SIZE)
  {
    /* Prevent buffer overflow.
    ** This only happens if DLOAD tries to transmit a packet larger
    ** than the buffer.
    */
    return;
  }

  /* Copy the byte to the transmit buffer. */
  hsu_al_dload_armprg_tx_buffer[hsu_al_armprg_index_in_tx_buf++] = chr;

  /* Check for terminating character */
  if ((chr == 0x7E) && (hsu_al_armprg_index_in_tx_buf != 1))
  {
    BOOTHW_KICK_WATCHDOG ();

    /* Send the buffer */
    hsu_al_armprg_write_is_complete = FALSE;
    jresult = cdc_write(hsu_al_armprg_cdc_handle, hsu_al_dload_armprg_tx_buffer, hsu_al_armprg_index_in_tx_buf);
    HSU_ASSERT(jresult==0);

    /* Wait for transfer to complete */
    while (!hsu_al_armprg_write_is_complete)
    {
      hsu_al_armprg_check();
    }

    hsu_al_armprg_index_in_tx_buf = 0;
  }
}

#else /* BUILD_HOSTDL */
#include "hsu_donothing.h"
#endif /* BUILD_HOSTDL */
