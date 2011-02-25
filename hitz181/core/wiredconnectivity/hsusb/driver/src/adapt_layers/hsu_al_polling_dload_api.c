/*==============================================================================

HSU_POLLING_DLOAD_API.C


GENERAL DESCRIPTION
This file define functions for HSU AL DLOAD for AMSS mode.

EXTERNALIZED FUNCTIONS
hsu_al_dload_init
hsu_al_dload_receive_byte
hsu_al_dload_transmit_byte
hsu_al_dload_drain
hsu_al_dload_shutdown
hsu_boot_dload_armprg_transition


INITIALIZATION AND SEQUENCING REQUIREMENTS
hsu_polling_init should be called to use other APIs.

Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/


/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header:

when      who     what, where, why
--------  ---     --------------------------------------------------------------
11/05/08  tk     Initial version

==============================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "hsu_polling.h"
#ifdef FEATURE_PMEM
#include "pmem.h"
#endif /* FEATURE_PMEM */
#include "port.h"
#include "jslave_init.h"
#include "hsu_al_dload.h"
#include "hsu_boot_dload.h"
#include "hsu_modem_proc_services.h"
#include "miaddress.h"
#include "hsu_conf_sel_types.h"
#include <string.h>
#include "queue.h"

size_t strlcpy(char *dst, const char *src, size_t siz);

/*==============================================================================
                    DEFINITIONS AND DECLARATIONS 
==============================================================================*/
#define HSU_AL_POLLING_RX_BUF_SIZE (16*1024)
#define HSU_AL_POLLING_TX_BUF_SIZE (16*1024)
#define NUM_OF_BUF_BLOCK 2
#define HSU_AL_POLLING_RX_MBUF_SIZE \
          (NUM_OF_BUF_BLOCK * HSU_AL_POLLING_RX_BUF_SIZE)
#define HSU_AL_POLLING_CORE_ID 0
#define HSU_AL_POLLING_IN_EP 1
#define HSU_AL_POLLING_OUT_EP 1
#define HSU_AL_POLLING_TERM_CH 0x7E

#define MI_IMEM_ARMPRG_DLOAD_SHARE_ADDR MI_NAND_IMEM_ARMPRG_DLOAD_SHARE_ADDR

/* buffer type */
typedef struct {
  q_link_type link;
  byte *buf; 
  uint32 max_size;
  uint32 write_idx; 
  uint32 read_idx; 
  uint32 count;
} hsu_al_polling_buf_type;

/* static variables */
typedef struct {
  hsu_polling_handle_type *phandle;
  byte *rx_mem;
  byte *tx_mem;
  hsu_al_polling_buf_type *curr_rx_new_buf;
  hsu_al_polling_buf_type *curr_rx_complete_buf;
  hsu_al_polling_buf_type tx_pmem_buf;
  q_type rx_buf_q; /* queue for rxed buffer */       
  q_type free_pmem_buf_q;/* queue for free buffer */
  hsu_al_polling_buf_type rx_buf[NUM_OF_BUF_BLOCK];
} hsu_al_polling_type;


/*==============================================================================
  External Variables

==============================================================================*/
#ifndef FEATURE_PMEM
#pragma arm section zidata = "PMEM_DATA_ZI"
  byte hsu_al_polling_rx_buf[HSU_AL_POLLING_RX_MBUF_SIZE];
  byte hsu_al_polling_tx_buf[HSU_AL_POLLING_TX_BUF_SIZE];
#pragma arm section zidata
#endif /* ! FEATURE_PMEM */


/*==============================================================================
  Local Variables
==============================================================================*/

hsu_al_polling_type hsu_al_polling;


/*===========================================================================

FUNCTION hsu_al_polling_buf_init

DESCRIPTION
 This function initializes the buf, qp.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/

static void hsu_al_polling_buf_init
(
  hsu_al_polling_buf_type *qp,
  byte *q_buff,
  uint32 max_q_size
)
{
  qp->buf = q_buff;
  qp->write_idx = 0;
  qp->read_idx = 0;
  qp->max_size = max_q_size;
  qp->count = 0;
}

/*===========================================================================

FUNCTION hsu_al_polling_dload_rx_cb

DESCRIPTION
This function is the callback function for hsu_polling_read. 

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/

static void hsu_al_polling_dload_rx_cb(hsu_polling_handle_type *phandle)
{
	hsu_al_polling_type *apph = &hsu_al_polling;
  hsu_al_polling_buf_type *curr_rx_new_buf;
  
  if(phandle->rx_state == HSU_POLLING_COMPLETED)
  {
    curr_rx_new_buf = apph->curr_rx_new_buf;
    
    /* put rx pmem buffer to rx buffer queue */
    if(curr_rx_new_buf)
    {
      curr_rx_new_buf->write_idx = phandle->rx_size;
      curr_rx_new_buf->count = phandle->rx_size;
      q_put(&apph->rx_buf_q, &curr_rx_new_buf->link);
    }
    
    /* get a free buffer */
     curr_rx_new_buf = 
    	(hsu_al_polling_buf_type *)q_get(&apph->free_pmem_buf_q);

    phandle->rx_state = HSU_POLLING_IDLE;
    
  	/* start read with pmem buffer */
  	if(curr_rx_new_buf)
  	{
  	  /* start reading */
  	  hsu_polling_read(apph->phandle, 
        curr_rx_new_buf->buf, curr_rx_new_buf->max_size);
   	}
    
  	apph->curr_rx_new_buf = curr_rx_new_buf;
  }
}


 
/*===========================================================================

FUNCTION hsu_al_dload_init

DESCRIPTION
This function is called to start HS USB AMSS polling mode for DLOAD APIs. 

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS

===========================================================================*/

void hsu_al_dload_init(void)
{
  int i;
  /* get polling handle */
  hsu_al_polling_type *apph = &hsu_al_polling;
  byte *rx_mem;
  
  apph->phandle = hsu_polling_init(HSU_AL_POLLING_CORE_ID,
                                HSU_AL_POLLING_IN_EP,
                                HSU_AL_POLLING_OUT_EP,
                                hsu_al_polling_dload_rx_cb,
                                NULL,
                                TRUE
                                );
  
  /* allocate the buffers for TX and RX opeartion */
  #ifdef FEATURE_PMEM
  apph->rx_mem = (byte*)pmem_malloc(HSU_AL_POLLING_RX_MBUF_SIZE,PMEM_USB_ID); 
  apph->tx_mem = (byte*)pmem_malloc(HSU_AL_POLLING_TX_BUF_SIZE,PMEM_USB_ID); 
  #else
  apph->rx_mem = hsu_al_polling_rx_buf; 
  apph->tx_mem = hsu_al_polling_tx_buf; 
  #endif

  /* static buffer pointer init */
  apph->curr_rx_complete_buf = NULL;
  apph->curr_rx_new_buf = NULL;
  
  /* queue header init */
  q_init(&apph->free_pmem_buf_q);
  q_init(&apph->rx_buf_q);

  /* tx queue init */
  hsu_al_polling_buf_init(
    &apph->tx_pmem_buf, apph->tx_mem, HSU_AL_POLLING_TX_BUF_SIZE);
  
  /* rx free pmem buffer queue init */
  rx_mem = apph->rx_mem;
  
  for(i=0;i<NUM_OF_BUF_BLOCK;i++)
  {
    hsu_al_polling_buf_init(&apph->rx_buf[i],rx_mem,HSU_AL_POLLING_RX_BUF_SIZE);
    q_put(&apph->free_pmem_buf_q,
        q_link (&apph->rx_buf[i], (q_link_type*)&(apph->rx_buf[i].link)));
    
    rx_mem += HSU_AL_POLLING_RX_BUF_SIZE;
  }

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
  hsu_al_polling_buf_type *rx_new_buf, *rx_complete_buf;
  hsu_al_polling_type *apph = &hsu_al_polling;

  rx_complete_buf = apph->curr_rx_complete_buf;

  /* if rx complete buffer is NULL, get a buffer from rx buffer queue */
  if( rx_complete_buf == NULL)
  {
      rx_complete_buf = (hsu_al_polling_buf_type *)q_get(&apph->rx_buf_q);
      apph->curr_rx_complete_buf = rx_complete_buf;
  }

  /* if rx complete buffer is not NULL, return a byte to caller */
  if( rx_complete_buf )
  {
    
    /* take out a byte from rx buf */  
    if( rx_complete_buf->count )
    {
      rx_complete_buf->count--;
      return (int)rx_complete_buf->buf[rx_complete_buf->read_idx++];
    }
    else
    {
      /* when rx_complete_buf is empty, clear the buf and free the buffer to 
         free queue for next use. */
      rx_complete_buf->read_idx = 0;
      rx_complete_buf->write_idx = 0;
      
      q_put(&apph->free_pmem_buf_q, &rx_complete_buf->link);
      apph->curr_rx_complete_buf = NULL;
    }
  }
  
  /* allocate a pmem buffer when current rx pmem buffer is NULL */
  if(apph->curr_rx_new_buf == NULL)
  {
    /* get a free buffer */
     rx_new_buf = 
        (hsu_al_polling_buf_type *)q_get(&apph->free_pmem_buf_q);

    /* start read with pmem buffer */
    if(rx_new_buf)
    {

      hsu_polling_read(apph->phandle, 
                                 rx_new_buf->buf, 
                                 rx_new_buf->max_size);

      apph->curr_rx_new_buf = rx_new_buf;


    }
  }

  /* handle pending interrupt */
  hsu_polling_handle_interrupt();
  
  /* return error once PC reachs here */
  return USB_NO_CHAR;
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

void hsu_al_dload_transmit_byte(byte ch)
{
  hsu_al_polling_type *apph = &hsu_al_polling;
  hsu_al_polling_buf_type *tx_pmem_buf;

  tx_pmem_buf = &apph->tx_pmem_buf;
  /* put the byte into tx buf */
  tx_pmem_buf->buf[tx_pmem_buf->write_idx++] = ch;
  tx_pmem_buf->count++;

  /* if the byte is HSU_AL_POLLING_TERM_CH, transmit the stacked data block */
  if(ch == HSU_AL_POLLING_TERM_CH && (tx_pmem_buf->count > 1))
  {
    /* transmit the data block. Data will be sent immediately. */
    hsu_polling_write(apph->phandle, tx_pmem_buf->buf, tx_pmem_buf->count);

    /* clear the buf */
    tx_pmem_buf->read_idx = 0;
    tx_pmem_buf->write_idx = 0;
    tx_pmem_buf->count = 0;
  }
}


/*===========================================================================

FUNCTION hsu_al_dload_drain

DESCRIPTION
This function adds a byte to the transmit buffer. If the byte signifies
the end of a packet, the buffer is transmitted to the host.

DEPENDENCIES

RETURN VALUE
None.

SIDE EFFECTS

===========================================================================*/

void hsu_al_dload_drain(void)
{
  hsu_polling_handle_interrupt();
}

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

FUNCTION hsu_boot_dload_armprg_transition

DESCRIPTION
  Stub
  
DEPENDENCIES

RETURN VALUE
None.

SIDE EFFECTS

===========================================================================*/

void hsu_boot_dload_armprg_transition(void)
{
  hsu_dload_info_type*dload_info = 
                       (hsu_dload_info_type*)MI_IMEM_ARMPRG_DLOAD_SHARE_ADDR;
  
  dload_info->dload_used_hsu = TRUE;
  dload_info->hsu_version_num = HSU_BOOT_DLOAD_VERSION_NUM;

  /* Update the product id according to the ARM9 static variable */
  dload_info->hsu_product_id = 
    hsu_modem_proc_services_get_product_id();

  dload_info->is_null_serial_number = 
    hsu_modem_proc_services_is_serial_number_null();

  /* save the Serial Number, to be used in DLOAD mode */
  if (hsu_modem_proc_services_is_serial_number_null() == FALSE)
  {
    memset(dload_info->serial_number, 0, HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN);
    (void)strlcpy(dload_info->serial_number, 
                  hsu_modem_proc_services_get_serial_number(), 
                  HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN);
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
  juint32_t rx_byte;
  hsu_al_polling_buf_type *rx_new_buf;
  hsu_al_polling_type *apph = &hsu_al_polling;

  /* handle pending interrupt */
  hsu_polling_handle_interrupt();

  /* allocate a rx pmem buffer from rx pmem queue */
  if(apph->curr_rx_new_buf == NULL)
  {
     apph->curr_rx_new_buf = q_get(&apph->free_pmem_buf_q);
  }

  /* start read */
  rx_new_buf = apph->curr_rx_new_buf;
  if(rx_new_buf)
  {

    rx_byte = hsu_polling_read(apph->phandle, 
                               rx_new_buf->buf, 
                               rx_new_buf->max_size);

    /* return the data pointer and size if rx byte is not 0 */
    if(rx_byte)
    {
      apph->curr_rx_new_buf = NULL;
      *buf = rx_new_buf->buf;

      /* free the pmem buffer to free queue */
      q_put(&apph->free_pmem_buf_q, &rx_new_buf->link);
      return rx_byte;
    }
  }

  *buf = NULL;
  return 0;
}

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
  hsu_al_polling_type *apph = &hsu_al_polling;
  hsu_al_polling_buf_type *tx_pmem_buf = &apph->tx_pmem_buf;

  /* copy the data to tx pmem buffer */
  memcpy(tx_pmem_buf->buf, pkt, len);

  /* send data */
  hsu_polling_write(apph->phandle, tx_pmem_buf->buf, len);
} /* end of hsu_al_dload_transmit_pkt() */

