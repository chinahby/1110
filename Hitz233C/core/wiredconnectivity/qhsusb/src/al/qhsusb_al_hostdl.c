/*=======================================================================*//**
  @file         qhsusb_al_hostdl.c

  @brief        ADAPTATION LAYER FOR ARMPRG

  @details      
                Armprog require dispatch table, defined as global structuse
                instance, to be filled.

               EXTERNALIZED FUNCTIONS (through dispatch table)
                  usb_drain
                    To drain the USB transmit buffer.
               
                  usb_init
                     Initializes the USB for transmit and receive.
                     
                  usb_active
                     Determines if activity on any endpoint
                     
                  do_usb_check
                     Determines if any data is available and calls function to process it
               
                  usb_transmit_byte
                     Transmits a single byte to the USB
               
               INITIALIZATION AND SEQUENCING REQUIREMENTS
                 All necessary initialization for normal CPU operation must have
                 been performed before entering this module.  usb_init() should
                 be called before any other functions.

 
                Copyright (c) 2008 QUALCOMM Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*//*========================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //source/qcom/qct/core/wiredconnectivity/qhsusb/main/latest/src/al/qhsusb_al_hostdl.c#14 $$DateTime: 2009/04/02 00:57:38 $$Author: vkondrat $

  when        who       what, where, why
  ----------  ---      -------------------------------------------------------
  2008-08-14  vkondrat  redesign - use simple 1 buffer mode, Rx callback
  2008-08-10  vkondrat pass full download flow
  2008-07-17  vkondrat Initial version with empty stubs

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "comdef.h"
#include "ap_armprg.h"
#include "ap_msm.h"    /**< for KICK_WATCHDOG */
#include "qhsusb_fd_hdlc.h"
#include "qhsusb_dci_api.h"
#include "qhsusb_dcd.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define CONNECTION_TIMEOUT (500000) /**< usec */

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static struct qhsusb_dcd_dsc_device* dev = NULL;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

static boolean usb_init(void);
static boolean usb_active(void);
static void usb_drain(void);
static void do_usb_check(void);
static void usb_transmit_byte(byte chr);

static uint8 qhsusb_str_product[28] = {                       // iProduct
   sizeof(qhsusb_str_product),USB_DESCTYPE_STRING,
   'Q',0,
   'H',0,
   'S',0,
   'U',0,
   'S',0,
   'B',0,
   '_',0,
   'A',0,
   'R',0,
   'M',0,
   'P',0,
   'R',0,
   'G',0,
};

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/* 
 *  USB function dispatch table
 */ 
DISP usb_dispatch_table = {
   usb_init,
   usb_active,
   usb_drain,
   do_usb_check,
   usb_transmit_byte,
};

#if defined(FEATURE_QHSUSB_ARMPROG_USE_BYTE_HANDLER)
extern void packet_handle_incoming(int);
/* if we have no external packet handling routine, provide one */
static inline void packet_handle_incoming_buf(byte* buf, int len)
{
   while ( len-- >0 ) {
      packet_handle_incoming(*buf++);
   }
}
#else /* use packet handler */
extern void packet_handle_incoming_buf(byte* buf, int len);
#endif


/**
 * This function checks for the assertion of any enabled
 * USB function controller interrupt.
 * 
 * @note The watchdog is reset.
 */
static void do_usb_check(void)
{
   qhsusb_dcd_poll(dev);
}


/**
 * This function buffers a byte into USB tx buffer.
 * If the end of message (0x7E) is received from the
 * packet module, then this function will send the data.
 * 
 * @note May block
 * @note The watchdog may be reset.
 * @param chr    Byte to transmit
 */
static void usb_transmit_byte(byte chr)
{
   qhsusb_fd_hdlc_tx_byte(chr);
}

/**
 * This function waits for the IN transfer to complete.
 * This allows the caller to be sure that all
 * characters are transmitted.
 * 
 * @note The watchdog may be reset.
 */
static void usb_drain(void)
{
   /* Wait for transfer to complete */
   while ( qhsusb_fd_hdlc_is_tx_active() != 0 ) {
      qhsusb_dcd_poll(dev);
   }
}

/**
 * 
 * @return False : The USB is not active
 *         True : USB is active
 */
static boolean usb_active(void)
{
   /* If we called usb_init() and returned TRUE, which is the only way we
    * call this function, we are guaranteed to have data, so just return
    * TRUE.
    */
   return TRUE;
}

static void ap_rx_complete(struct qhsusb_urb* urb)
{
   if ( QHSUSB_URB_STATUS_COMPLETE_OK == urb->transfer_status ) {
      packet_handle_incoming_buf(urb->transfer_buffer_ptr,(int)urb->actual_length);
      /* mark it done */
      qhsusb_fd_hdlc_rx_completed(urb);
   }
}

/**
 * This function initializes the MSM USB device controller
 * with no interrupts enabled but both receiver and 
 * transmitter running.
 * 
 * @return True if init succeeded
 */
static boolean usb_init(void)
{
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
#ifdef FEATURE_QHSUSB_USE_SCRATCH_ON_TIMERS
/**
 * query USB mode
 */
   {
      uint32 core_id = 0;
      uint32 r0, r1;
      qhsusb_dci_read_scratch( core_id, &r0, &r1 );
      qhsusb_hdlc_use_cdcacm( (int)(r0) );
   }
#endif
#endif

   dev = qhsusb_fd_hdlc_init(ap_rx_complete, qhsusb_str_product);
   if ( NULL != dev ) {
      uint32 t_end;
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
      if ( 0 != qhsusb_hdlc_get_use_cdcacm() ) {
         /** 
          *  we got here without reconnect. enumeration already
          *  completed. Simulate connection and SET_CONFIG
          */
         qhsusb_dcd_port_status_changed(dev); /* attachment, speed... */
         dev->address = 1; /* don't care, just not 0 */
         qhsusb_dcd_set_config(dev,1);
      }
#endif
      /**
       * Wait until speed is detected or CONNECTION_TIMEOUT 
       *  
       * Hardware timer get reset on stack initialization; its range 
       * is 24bit, it corresponds to about 16 seconds. All timeouts 
       * are way shorter, thus no wrap around possible.
       */
      t_end = qhsusb_dci_get_time_usec(dev->core_id,QHSUSB_TIMER_0_FREE_RUN) + 
         CONNECTION_TIMEOUT;
      while ( (long)( t_end - qhsusb_dci_get_time_usec(dev->core_id,QHSUSB_TIMER_0_FREE_RUN) ) > 0 ) {
         KICK_WATCHDOG();
         qhsusb_dcd_poll(dev);
         if ( QHSUSB_DCD_SPEED_UNKNOWN != dev->speed ) {
            return TRUE;
         }
      }
   }
   return FALSE;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

