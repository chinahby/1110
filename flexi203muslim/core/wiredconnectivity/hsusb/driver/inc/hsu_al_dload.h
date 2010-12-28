#ifndef HSU_AL_DLOAD_H
#define HSU_AL_DLOAD_H

/*==============================================================================

High Speed USB DLOAD Adaptation Layer

GENERAL DESCRIPTION
Defines externalized functions for the mass storage adaptation layer.

EXTERNALIZED FUNCTIONS

hsu_al_dload_drain
hsu_al_dload_init
hsu_al_dload_receive_byte
hsu_al_dload_transmit_byte
hsu_al_dload_receive_pkt
hsu_al_dload_transmit_pkt
hsu_al_dload_shutdown
hsu_al_dload_obex_init

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

#include "comdef.h"
#include "uw_device_cdc_obex.h"
#include "uw_device_cdc.h"
#include "jtypes.h"

/* Error return code from hsu_al_dload_receive_byte */
/* This int value must not be any valid unsigned char value. */
#define  USB_NO_CHAR    (-1)

typedef enum
{
  INTERFACE_UNKNOWN,
  INTERFACE_UART1,
  INTERFACE_USB
} 
hsu_al_dload_interface_type;

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

void hsu_al_dload_drain(void);

/*===========================================================================

FUNCTION hsu_al_dload_init

DESCRIPTION
This function initializes HS-USB support for DLOAD.

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/

void hsu_al_dload_init(void);

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

void hsu_al_dload_init_ex(boolean use_amss_product_id);

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

int hsu_al_dload_receive_byte(void);

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

void hsu_al_dload_transmit_byte(byte chr);

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
uint32 hsu_al_dload_receive_pkt(byte **buf);

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
);

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
void hsu_al_dload_shutdown(void);

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
void hsu_al_dload_mark_dload_used_hsu(void);

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
);

#endif
