/*=======================================================================*//**
 * @file        qhsusb_al_dload.c
 * @author:     amirs
 * @date        14-July-2008
 *
 * @brief       QHSUSB (Qualcomm High-Speed USB) AL (Adaptation-Layer) implementation.
 *
 * @details     This file contains the implementation of the API to be used by the boot downloader.
 *              A software download application such QPST can communicate with the MSM using the USB.
 *              The host application can send software download commands over the OUT Bulk endpoint,
 *              and receive status information from the MSM over the IN Bulk endpoint.
 *
 * @note        The DLOAD was originally using UART ,
 *              therefore the TX and RX API are using bytes.
 *
 *              Copyright (c) 2008 QUALCOMM Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
*//*========================================================================*/


// ===========================================================================
// 
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
// 
// $Header: //source/qcom/qct/core/wiredconnectivity/qhsusb/main/latest/src/al/qhsusb_al_dload.c#16 $ $DateTime: 2009/04/16 08:55:21 $ $Author: vkondrat $
// 
// when       who     what, where, why
// --------   ---     ----------------------------------------------------------
// 07/14/08   amirs   First Draft
// 08/08/08   amirs   First working DLOAD process
// 08/11/08   amirs   Add files and functions headers according to coding standard.
// 08/14/08   vkondrat redesign - Rx callback
//
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "comdef.h"                 // common defines - basic types as byte word uint32 etc
//#include "boothw.h"                 // BOOTHW_KICK_WATCHDOG()
//#include "pm.h"                     // PMIC API

#include "qhsusb_al_dload_api.h"    // qhsusb_al_dload_drain() etc
#if defined(BUILD_BOOT_CHAIN)
#include "boot_util.h"
static inline void* memcpy(void* dst, const void* src,unsigned int len) {
   qmemcpy(dst,src,len);
   return dst;
}
#else
#include <memory.h>
#endif

// ==== Private QHSUSB H files ===
#include "qhsusb_fd_hdlc.h"        // qhsusb_fd_hdlc_poll() etc

#include "qhsusb_dcd.h"            // dev->core_id

#include "qhsusb_dci_api.h"        // disconnect/connect


//----------------------------------------------------------------------------
// Preprocessor Definitions and Constants
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------
// None

//----------------------------------------------------------------------------
// Global Data Definitions
//----------------------------------------------------------------------------
static uint8 qhsusb_str_product[26] = {                       // iProduct
   sizeof(qhsusb_str_product),USB_DESCTYPE_STRING,
   'Q',0,
   'H',0,
   'S',0,
   'U',0,
   'S',0,
   'B',0,
   '_',0,
   'D',0,
   'L',0,
   'O',0,
   'A',0,
   'D',0,
};

//----------------------------------------------------------------------------
// Static Variable Definitions
//----------------------------------------------------------------------------
// None


//----------------------------------------------------------------------------
// Static Function Declarations and Definitions
//----------------------------------------------------------------------------

static void spoof_reconnect(void);
static void delay_usec(uint32 usec);

static struct qhsusb_urb* dload_rx_urb = NULL;
static struct qhsusb_urb* dload_rx_urb_next = NULL;
static uint32 dload_rx_byte_index = 0;
static struct qhsusb_dcd_dsc_device* dev = NULL;

static void dload_rx_complete(struct qhsusb_urb* urb)
{
   if ( QHSUSB_URB_STATUS_COMPLETE_OK == urb->transfer_status ) {
      if ( dload_rx_urb == NULL ) {
         dload_rx_urb = urb;
         dload_rx_byte_index = 0;
      } else {
         dload_rx_urb_next = urb;
      }
   }
}

#if 0
static void dload_set_max_current( int milliamps )
{
   /* tell to charger */
   if ( milliamps > 0 ) {
      chg_usb_i_is_available(milliamps);
   } else {
      chg_usb_i_is_not_available();
   }
}
#endif
//----------------------------------------------------------------------------
// Externalized Function Definitions
//----------------------------------------------------------------------------


// ===========================================================================
/**
 * @function    qhsusb_al_dload_drain
 * 
 * @brief       wait until tx complete.
 *
 * @details
 *          This function waits for the last character in the USB's transmit
 *          FIFO to be transmitted.  This allows the caller to be sure that all
 *          characters are transmitted.
 * 
 * @param   None
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qhsusb_al_dload_drain(void)
{
    // Wait for transfer to complete
    // TODO - Any Timeout using FRAME-INDEX ?
    while (qhsusb_fd_hdlc_is_tx_active()) 
    {
        qhsusb_dcd_poll(dev);
    }
}

// ===========================================================================
/**
 * @function    qhsusb_al_dload_init
 * 
 * @brief   Initialize the USB stack
 *
 * @details
 *         This function initializes the USB device controller with no
 *         interrupts enabled but both receiver and transmitter running.
 *        
 * @param   None
 * 
 * @return  None.
 * 
 */
// ===========================================================================

#if 0 /* fast enum test */
void hsu_al_fast_enum_init(void);
void hsu_al_fast_enum_shutdown(void);
boolean hsu_al_fast_enum_poll(void);
#endif

#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
void qhsusb_al_dload_init(int use_cdcacm)
#else /* defined(FEATURE_QHSUSB_HDLC_CDCACM) */
void qhsusb_al_dload_init(void)
#endif /* defined(FEATURE_QHSUSB_HDLC_CDCACM) */
{
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
   qhsusb_hdlc_use_cdcacm(use_cdcacm);
#endif
   #if 0 /* fast enum test */
   {
      hsu_al_fast_enum_init();
      while (!hsu_al_fast_enum_poll()) {
         BOOTHW_KICK_WATCHDOG();
      }
      hsu_al_fast_enum_shutdown();
   }
   #endif
   dev = qhsusb_fd_hdlc_init(dload_rx_complete,qhsusb_str_product);
   if ( NULL == dev ) 
    {
        //! @todo Assert on Error
    }
}

// ===========================================================================
/**
 * @function    qhsusb_al_dload_receive_byte
 * 
 * @brief   return a byte received from the host.
 *
 * @details
 *         This function receives an incoming data from the respective USB out fifos and
 *         returns one character at a time to the calling function. Though it receives
 *         a bigger packet at once, it always retuns one character to the calling function.
 *         This approach is choosen to have a consitancy between the UART and USB modules.
 * 
 * @param   None
 * 
 * @return Value/Status
 *         Returns the next character from the receive buffer.
 *         If there is nothing in the receive buffer then it return USB_NO_CHAR (-1).
 * 
 */
// ===========================================================================
int qhsusb_al_dload_receive_byte(void)
{
   int value = USB_NO_CHAR;
    
    qhsusb_dcd_poll(dev);
    if ( dload_rx_urb != NULL ) { 
      if ( dload_rx_byte_index < dload_rx_urb->actual_length ) {
        value = ((uint8*)dload_rx_urb->transfer_buffer_ptr)[dload_rx_byte_index++];
      }
      if ( dload_rx_byte_index >= dload_rx_urb->actual_length ) {
         qhsusb_fd_hdlc_rx_completed(dload_rx_urb);
         /* go to next, if any */
         dload_rx_urb = dload_rx_urb_next;
         dload_rx_byte_index = 0;
         dload_rx_urb_next = NULL;
      }
    }

    return value;
}

// ===========================================================================
/**
 * @function    qhsusb_al_dload_transmit_byte
 * 
 * @brief       Transmit a byte to the host.
 *
 * @details
 *          This function transmits a single byte through the USB.
 *          The byte is actually added to a buffer until the tx buffer is full,
 *          or an "end of message" (0x7E) is detected.
 * 
 *          This function also poll the USB stack for any event,
 *          like previouse rx/tx completed.
 * 
 * @param   chr
 *          one byte character value.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qhsusb_al_dload_transmit_byte (byte  chr)
{
    qhsusb_dcd_poll(dev);
    qhsusb_fd_hdlc_tx_byte(chr);
}

// ===========================================================================
/**
 * @function    qhsusb_al_dload_shutdown
 * 
 * @brief       This function will cancel any pending transfer.
 *
 * @note        This function is called before the DLOAD jump to ARMPROG.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 * @see     go_cmd
 * 
 */
// ===========================================================================
void qhsusb_al_dload_shutdown(void)
{
    qhsusb_fd_hdlc_shutdown();
}


int32 qhsusb_al_dload_receive_pkt(byte* receive_ptr, uint32* len)
{
   qhsusb_dcd_poll(dev);
   if ( ( dload_rx_urb != NULL ) && 
        ( dload_rx_byte_index < dload_rx_urb->actual_length ) ) {
      uint32 have_bytes = dload_rx_urb->actual_length - dload_rx_byte_index;
      if ( have_bytes > *len ) {
         have_bytes = *len;
      }
      memcpy(receive_ptr,&((uint8*)dload_rx_urb->transfer_buffer_ptr)[dload_rx_byte_index],have_bytes);
      dload_rx_byte_index += have_bytes;
      *len = have_bytes;
      if ( dload_rx_byte_index >= dload_rx_urb->actual_length ) {
         qhsusb_fd_hdlc_rx_completed(dload_rx_urb);
         /* go to next, if any */
         dload_rx_urb = dload_rx_urb_next;
         dload_rx_byte_index = 0;
         dload_rx_urb_next = NULL;
      }
      return QHSUSB_PKT_DRIVER_ALL_OK;
   }
   return QHSUSB_PKT_DRIVER_NO_CHAR;
}

// ===========================================================================
/**
 * @function    qhsusb_al_dload_transmit_pkt
 * 
 * @brief       This function transmit a buffer to the host.
 *
 * @note        Not implemented yet.
 * 
 * @param   pkt
 *          pointer to pkt to be transmitted
 * 
 * @param   len
 *          number of bytes to tx
 * 
 * @return  none
 * 
 * @see     qhsusb_al_dload_receive_pkt
 * 
 */
// ===========================================================================
void qhsusb_al_dload_transmit_pkt(byte *pkt, uint32 len)
{
    // @todo - stub for Linking only

    // Have lint not complain about unused parameters
    //lint -esym(715,pkt)
    //lint -esym(818,pkt)
    //lint -esym(715,len)

}
// ===========================================================================

// ===========================================================================
/**
 * @function    qhsusb_boot_dload_armprg_transition
 * 
 * @brief       This function handle the transition from DLOAD to ARMPROG from the USB point of view.
 *
 * @param   None.
 * 
 * @return  None
 * 
 * @see     go_cmd
 * 
 */
// ===========================================================================
void qhsusb_boot_dload_armprg_transition(void)
{
    qhsusb_al_dload_drain();
    qhsusb_al_dload_shutdown();
#if defined(FEATURE_QHSUSB_HDLC_CDCACM)
#ifdef FEATURE_QHSUSB_USE_SCRATCH_ON_TIMERS
/* store selected mode of operation */
    {
       uint32 core_id = dev->core_id;
       qhsusb_dci_write_scratch(core_id, (uint32)(qhsusb_hdlc_get_use_cdcacm()), 0);
    }
#endif /* FEATURE_QHSUSB_USE_SCRATCH_ON_TIMERS */
   if ( 0 != qhsusb_hdlc_get_use_cdcacm() ) {
      /**
       * If we run CDC/ACM mode, armprg transition should be performed 
       * without reconnect (Microsoft serial driver issue). 
       */
      return;
   }
#endif /* defined(FEATURE_QHSUSB_HDLC_CDCACM) */

    // hsu_al_dload_armprg_common_mark_dload_used_hsu();
    // hsu_boot_dload_copy_shared_segment_to_imem();

    spoof_reconnect();

}
// ===========================================================================

// ===========================================================================
/**
 * @function    delay_usec
 * 
 * @brief       do delay in microseconds
 *
 * @param   microseconds
 * 
 * @return  None
 * 
 * @see     fsbl_hw_clk_busy_wait
 * 
 */
// ===========================================================================
static void delay_usec(uint32 microseconds)
{
#if 0 // use busy wait
    const uint32 step = 1000;
    uint32 counter;

    for (counter = 0; counter < microseconds; counter += step)
    {
        clk_busy_wait(step); 
        //BOOTHW_KICK_WATCHDOG();
    }   
#else // use DCI timers
    uint32 core_id = dev->core_id;
    uint32 time_usec = 0;

    qhsusb_dci_stop_timer(core_id,QHSUSB_TIMER_1_ONE_SHOT); // stop before start

    qhsusb_dci_start_timer(core_id,QHSUSB_TIMER_1_ONE_SHOT,FALSE);

    do
    {
        time_usec = qhsusb_dci_get_time_usec(core_id,QHSUSB_TIMER_1_ONE_SHOT); 
    } while(time_usec < microseconds);

    qhsusb_dci_stop_timer(core_id,QHSUSB_TIMER_1_ONE_SHOT); // stop before start
#endif
}
// ===========================================================================

// ===========================================================================
/**
 * @function    spoof_reconnect
 * 
 * @brief       This function perform disconnect and connect to the USB Bus.
 *
 * @details     This function is using the D+ pull-up to signal to the host disconnect and connect.
 *              This force the host to start again the enumeration process when ARMPROG will be running.
 *
 * @param   None
 * 
 * @return  None
 * 
 * @see     pm_otg_config_interface
 * 
 */
// ===========================================================================
static void spoof_reconnect(void)
{
  qhsusb_dci_disconnect(dev->core_id);

  /* Wait for host to detect the disconnect */
  delay_usec(3000);

  qhsusb_dci_connect(dev->core_id);
}
// ===========================================================================



