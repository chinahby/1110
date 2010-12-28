/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             S I O    W R A P P E R   F U N C T I O N S
                            
       SIO wrapper functions for serving all devices that uses SIO 

GENERAL DESCRIPTION
  This module contains the wrapper function used to access SIO server
  layer.  This layer is only use temporary.  Each port that wish to access
  the SIO layer should register their own function with SIO server, insteaded
  going though the wrapper functions.
  
NOTE DURING DEVELOPEMNT
    Each served device must have thses functions wrapped:
        HW_open
        HW_close
        HW_ioctl
        HW_flush_tx
        HW_disable_device
        HW_control_transmit
        HW_control_close
        HW_control_open



        open
        close
        ioctl
        flush_tx
        disable_device
        control_transmit
        control_close
        control_open

    Magic number:
        Value used to globally identify a device.  32 bit number divided into
        16 bit Major and 16 bit minor number    
  
EXTERNALIZED FUNCTIONS

    sio_wrapper_init()
      Used to install all the function with the SIO layer.  

SERVED FUNCTIONS

  
Copyright (c) 2007-2008 QUALCOMM Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary


*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/services/sio/02.00.XX/sio_wrapper.c#1 $ $DateTime: 2008/10/19 17:40:04 $ $Author: vaghasia $

mm/dd/yy   who     what, where, why
--------   ---     ----------------------------------------------------------
09/12/08   amv     Added SMD ports for MODEM, APP and QDSP processor
05/19/08   rh      Add the include for aps_serial_vsp.h
05/05/08   rh      Fix a typo on disable_device for UART2
03/24/08   bfc     Added some new SMD DATA ports.
03/26/08   rh      Add entry for HSUSB ctrl_open for port NET_WWAN
02/29/08   rh      Wrapped calls to hsu_al_ecm_xx APIs with FEATURE_HS_USB_ECM
01/31/08   rh      Add sio_open and control_open to registerable function
09/26/07   rh      Adding MDD/PDD support for UART2 and UART3
07/30/07   rh      Remove entry for FSUSB on port SIO_PORT_USB_SER3
07/12/07   rh      Add new port - SIO_PORT_USB_SER3
06/13/07   rh      Add the use of MDD layer for UART1
01/11/07   rh      Created module.

===========================================================================*/

/*===========================================================================

                            INCLUDE FILES

===========================================================================*/
#include "target.h"                /* Target specific definitions          */
#include "comdef.h"                /* Definitions for byte, word, etc.     */
#include "sio.h"                   /* Serial I/O Services.                 */
#include "assert.h"
#include "sio_wrapper.h"
#include "sio_priv.h"

#if !defined(SIO_NO_UART) && !defined(SIORS_USES_MDDPDD_DRIVER)
#include "siors232.h"
#endif

/*===========================================================================

            Components that are wrapped by wrapper function 

===========================================================================*/
#ifdef FEATURE_USB
#include "siousb.h"                /* USB device driver layer.             */
#endif  /* FEATURE_USB */

#ifdef FEATURE_BT
#include "bt.h"
#include "bti.h"  
#include "btsio.h"                 /* BT Device Driver Layer.              */
#endif  /* FEATURE_BT */

#ifdef FEATURE_SMD
#include "smd_sio.h"

#ifdef FEATURE_SMD_PORT_MGR
  #include "smd_port_mgr.h"
#endif

#include "oncrpc.h"
#include "oncrpc_proxy.h"
#include "remote_apis.h"
#endif  /* FEATURE_SMD */

#if defined (FEATURE_SMD) && defined (FEATURE_SMD_BRIDGE)

#include "smd_bridge.h"

#ifdef IMAGE_APPS_PROC
  #define SMD_BRIDGE_CONNECT_FUNC     smd_bridge_connect_modem
  #define SMD_BRIDGE_DISCONNECT_FUNC  smd_bridge_disconnect_modem
#else /* !IMAGE_APPS_PROC */
  #define SMD_BRIDGE_CONNECT_FUNC     smd_bridge_connect_apps
  #define SMD_BRIDGE_DISCONNECT_FUNC  smd_bridge_disconnect_apps
#endif /* IMAGE_APPS_PROC */

#endif /* FEATURE_SMD && FEATURE_SMD_BRIDGE */

#ifdef SIO_USES_HS_USB
#include "hsu_al_ser.h"
#include "hsu_al_ecm.h"
#endif

#ifdef FEATURE_MMGPS_CSD
#error code not present
#endif  

#ifdef FEATURE_APS                                                                                                                 
#include "aps_serial_vsp.h"
#endif 

#ifdef SIORS_USES_MDDPDD_DRIVER
#include "siors_lib.h"
#include "siors232_mdd.h"
#endif

/* Pulling some variable from sio.c file, should be removed later */
extern sio_port_id_type     sio_stream_port[ SIO_MAX_STREAM];
extern sio_port_id_type     sio_control_stream_port[ SIO_MAX_STREAM];

/*===========================================================================

FUNCTION sio_wrapper_bt_xxx                         INTERNAL FUNCTION

DESCRIPTION
  Blue tooth driver wrapper

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
#ifdef FEATURE_BT
void sio_wrapper_bt_ioctl
(
    sio_stream_id_type stream_id,
    sio_port_id_type      port_id,
    sio_ioctl_cmd_type    cmd,
    sio_ioctl_param_type *param
)
{
    bt_sio_cmd_ioctl(stream_id, cmd, param);
}

void sio_wrapper_bt_tx_flush
(
  sio_stream_id_type stream_id,           /* Stream ID                     */
  sio_port_id_type  port_id,
  void (*flush_func_ptr) (void)
)
{
    sio_ioctl_param_type ioctl_param;
    ioctl_param.record_flush_func_ptr = flush_func_ptr;
    bt_sio_cmd_ioctl( stream_id, SIO_IOCTL_FLUSH_TX, &ioctl_param );
}

void sio_wrapper_bt_transmit
(
  sio_stream_id_type stream_id,           /* Stream ID                     */
  sio_port_id_type   port_id,
  dsm_item_type*     dsm_ptr
)
{
    bt_sio_cmd_transmit( stream_id, dsm_ptr );
}
#endif

/*===========================================================================

FUNCTION sio_wrapper_usb_xxx                         INTERNAL FUNCTION

DESCRIPTION
  USB driver wrapper

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
#ifdef FEATURE_USB
#if defined (FEATURE_USB_CDC_ACM) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
void sio_wrapper_usb_close
( 
    sio_stream_id_type stream_id,             
    void             (*close_func_ptr)(void)
)
{ 
   sio_usb_close_stream( stream_id, SIO_PORT_NULL, close_func_ptr );
}
#endif
#endif /* FEATURE_USB */


/*===========================================================================

FUNCTION sio_wrapper_hs_usb_xxx                         INTERNAL FUNCTION

DESCRIPTION
  HS USB driver wrapper

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
#ifdef SIO_USES_HS_USB
void sio_wrapper_hs_usb_close
( 
    sio_stream_id_type stream_id,             
    void             (*close_func_ptr)(void)
)
{
    sio_port_id_type   port_id;               /* Port which corresponds to
                                                 stream_id                   */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    port_id = sio_stream_port[ stream_id];
    hsu_al_ser_close( stream_id, port_id, close_func_ptr );
}


#ifdef FEATURE_HS_USB_ECM
void sio_wrapper_hs_usb_ecm_close
( 
    sio_stream_id_type stream_id,             
    void             (*close_func_ptr)(void)
)
{
    sio_port_id_type   port_id;               /* Port which corresponds to
                                                 stream_id                   */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    port_id = sio_stream_port[ stream_id];
    hsu_al_ecm_data_close( stream_id, port_id, close_func_ptr );
}
#endif /* FEATURE_HS_USB_ECM */

#endif /* SIO_USES_HS_USB */


/*===========================================================================

FUNCTION sio_wrapper_aps_xxx                         INTERNAL FUNCTION

DESCRIPTION
  APS driver wrapper

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
#ifdef FEATURE_APS
void sio_wrapper_aps_close
( 
    sio_stream_id_type stream_id,             
    void             (*close_func_ptr)(void)
)
{
    aps_serial_vsp_close_stream(stream_id, SIO_PORT_NULL, close_func_ptr);
}
#endif /* FEATURE_APS */


#ifdef FEATURE_SMEM_DS
#error code not present
#endif

#if defined(FEATURE_SMD) && defined(FEATURE_SMD_PORT_MGR)
/*===========================================================================

FUNCTION sio_smd_port_mgr_free_port_helper_f

DESCRIPTION
  This is the ONCRPC helper function that will be called from 
  a helper task context to call the smd_port_mgr_free_port()
  function which is an RPC call from the applications processor.  It will
  also call the user close callback if present.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  smd_port_mgr_free_port will be called
===========================================================================*/
void 
sio_smd_port_mgr_free_port_helper_f
(
  /* ONCRPC Proxy command message */
  oncrpc_proxy_cmd_client_call_type *msg
)
{
  sio_port_id_type port_id = (sio_port_id_type) (int32) msg->data[0];

  /* Sanity check */
  ASSERT ((port_id >= SIO_PORT_SMD_BRIDGE_FIRST) && 
          (port_id <= SIO_PORT_SMD_BRIDGE_LAST));

  /* Let the port manager know that we are done with
  ** the port
  */
  smd_port_mgr_free_port (port_id);

} /* sio_smd_bridge_port_close_cb */


/*===========================================================================

FUNCTION sio_smd_port_close_cb 

DESCRIPTION
  This function is the close callback function that is registered with SMD
  for bridge ports.  It de-registers with the SMD port manager, and calls
  the client callback if one is present. 
      
DEPENDENCIES
  port must be bridge port on dual procssor build
      
RETURN VALUE
  None
    
SIDE EFFECTS
  De-registers with SMD port manager
  
===========================================================================*/
void sio_smd_bridge_port_close_cb
( 
  void  *cb_data
)
{
  /* ONCRPC Proxy command structure */
  oncrpc_proxy_cmd_client_call_type *smd_port_mgr_free_port_ptr;

  /* Can't call smd_port_mgr_free_port here directly because
  ** it is an RPC call and this call occurs from SMD task context.
  ** Jump through the hoops necessary to have
  ** it called from another task context
  */
  /* Get a message pointer */
  smd_port_mgr_free_port_ptr = remote_apis_client_call_get();

  /* Check that the message pointer is valid */
  if( smd_port_mgr_free_port_ptr != NULL )
  {
    /* Fill up the pointer data */
    smd_port_mgr_free_port_ptr->client_call =
      (void(*)( struct oncrpc_proxy_cmd_client_call_type *ptr ))
        sio_smd_port_mgr_free_port_helper_f;

    /* Set message data to be the callback data */
    smd_port_mgr_free_port_ptr->data[0] = (int32) cb_data;

    /* Queue up the message */
    oncprc_proxy_client_call_send( smd_port_mgr_free_port_ptr );
    /* Explicitly set to NULL */
    smd_port_mgr_free_port_ptr = NULL;
  }
} /* sio_smd_bridge_port_close_cb */

#endif /* if defined(FEATURE_SMD) && defined(FEATURE_SMD_PORT_MGR) */


/*===========================================================================

FUNCTION sio_wrapper_smd_bridge_xxx                         INTERNAL FUNCTION

DESCRIPTION
  SMD bridge driver wrapper

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
#ifdef FEATURE_SMD_BRIDGE
sio_status_type sio_wrapper_smd_bridge_open
( 
    sio_open_type *open_ptr
)
{
    sio_port_id_type   port_id;                   /* Port which corresponds to
                                                     stream_id               */
    sio_status_type    return_val = SIO_DONE_S;   /* Return Value            */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    port_id = open_ptr->port_id;

    ASSERT (SIO_PORT_IS_BRIDGE(port_id));
    return_val = smd_sio_open(open_ptr);

    /* If the smd open is successfulregister port with SMD port mgr.  
    ** Otherwise, disconnect the bridge 
    */ 
    if (return_val == SIO_DONE_S)
    {
      /* Register with SMD port manager that we are using port */
      smd_port_mgr_reg_port (port_id);
    }
    else
    {
      SMD_BRIDGE_DISCONNECT_FUNC (port_id,NULL,NULL,NULL);   
    }
    return return_val;
}

#endif /* FEATURE_SMD_BRIDGE */

#ifdef FEATURE_SMD_BRIDGE
void sio_wrapper_smd_bridge_close
( 
    sio_stream_id_type stream_id,             
    void             (*close_func_ptr)(void)
)
{
    sio_port_id_type   port_id;               /* Port which corresponds to
                                                 stream_id                   */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    port_id = sio_stream_port[ stream_id];

    /* Close the shared memory port */
    smd_sio_close_ext (port_id, 
                         sio_smd_bridge_port_close_cb, 
                         (void *)(int32) port_id);

    /* Close down the bridge */
    SMD_BRIDGE_DISCONNECT_FUNC (port_id,
                               close_func_ptr,
                               NULL,
                               NULL);   
}

#endif /* FEATURE_SMD_BRIDGE */

#ifdef FEATURE_SMD
void sio_wrapper_smd_sio_close
( 
    sio_stream_id_type stream_id,             
    void             (*close_func_ptr)(void)
)
{
    sio_port_id_type   port_id;               /* Port which corresponds to
                                                 stream_id                   */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    port_id = sio_stream_port[ stream_id];

    /* Close the shared memory port */
    smd_sio_close (port_id, close_func_ptr); 
                     
}

void sio_wrapper_smd_control_close
( 
    sio_stream_id_type stream_id,             
    void             (*close_func_ptr)(void)
)
{
    sio_port_id_type   port_id;               /* Port which corresponds to
                                                 stream_id                   */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    port_id = sio_control_stream_port[ stream_id];

    /* Close the shared memory port */
    smd_sio_control_close (port_id, close_func_ptr); 
                     
}

void sio_wrapper_smd_control_transmit
( 
    sio_stream_id_type stream_id,             
    dsm_item_type *tx_ptr
)
{
    sio_port_id_type   port_id;               /* Port which corresponds to
                                                 stream_id                   */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    port_id = sio_control_stream_port[ stream_id];

    /* Close the shared memory port */
    smd_sio_control_transmit (port_id, tx_ptr); 
                     
}

void sio_wrapper_smd_dev_tx
( 
  sio_stream_id_type stream_id,           /* Stream ID                     */
  sio_port_id_type  port_id,
  dsm_item_type     *tx_ptr               /* Packet for transmission       */
)
{
   smd_sio_dev_transmit(port_id, tx_ptr);
}

void sio_wrapper_smd_flush_tx
(
    sio_stream_id_type stream_id,           /* Stream ID                     */
    sio_port_id_type  port_id,
    void (*flush_func_ptr) (void)
)
{
    smd_sio_flush_tx(port_id, flush_func_ptr);
}

void sio_wrapper_smd_ioctl
(
    sio_stream_id_type stream_id,
    sio_port_id_type      port_id,
    sio_ioctl_cmd_type    cmd,
    sio_ioctl_param_type *param
)
{
    smd_sio_ioctl(port_id, cmd, param);
}

void sio_wrapper_smd_disable
(
    sio_port_id_type      port_id
)
{
    smd_sio_disable_device();
}
#endif /* FEATURE_SMD */


/*===========================================================================

FUNCTION sio_wrapper_mmgps_csd_xxx                         INTERNAL FUNCTION

DESCRIPTION
  GPS driver wrapper

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
#ifdef FEATURE_MMGPS_CSD
#error code not present
#endif  


/*===========================================================================

FUNCTION sio_wrapper_null_xxx                         EXTERNALIZED FUNCTION

DESCRIPTION
  These function are a set of null function that is runned when the registered
  driver do not register for a particular API

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
sio_status_type sio_wrapper_null_open (sio_open_type *open_ptr)
{
    return SIO_DONE_S;
}

void sio_wrapper_null_close (sio_stream_id_type stream_id,
                        void (*close_func_ptr)(void))
{
}

void sio_wrapper_null_ioctl (sio_stream_id_type stream_id,
                      sio_port_id_type      port_id,
                      sio_ioctl_cmd_type    cmd,
                      sio_ioctl_param_type *param)
{
}


void sio_wrapper_null_transmit (sio_stream_id_type stream_id,
                         sio_port_id_type   port_id,
                         dsm_item_type *tx_ptr)
{
}


void sio_wrapper_null_flush_tx (sio_stream_id_type stream_id,
                         sio_port_id_type   port_id,
                         void (*flush_func_ptr)(void))
{
}

void sio_wrapper_null_disable_device (sio_port_id_type port_id)
{
}

void sio_wrapper_null_control_transmit (sio_stream_id_type stream_id,
                                 dsm_item_type     *tx_ptr)
{
    MSG_ERROR("Stream %d does not support control streams", stream_id, 0, 0);
}

void sio_wrapper_null_control_close (sio_stream_id_type stream_id,
                              void (*close_func_ptr)(void))
{
    MSG_ERROR("Stream %d does not support control streams", stream_id, 0, 0);
}

sio_status_type sio_wrapper_null_control_open (sio_open_type *open_ptr)
{
    return SIO_DONE_S;
}

/*===========================================================================

FUNCTION sio_wrapper_no_control_open                  INTERNAL FUNCTION

DESCRIPTION
  These function are a set of null function that is runned when the registered
  driver do not register for a particular API

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
static sio_status_type sio_wrapper_no_control_open (sio_open_type *open_ptr)
{
    MSG_ERROR("Port_id %d  does not support control streams", open_ptr->port_id, 0, 0);
    return SIO_UNAVAIL_S;
}

/*===========================================================================

FUNCTION sio_wrapper_init                             EXTERNALIZED FUNCTION

DESCRIPTION
  This function serves as starting point for trigger the initialization 
  routine for each driver that uses the sio service

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void sio_wrapper_init(void)
{
    sio_device_info_type Info;
    uint32 ret;
    /* == Do any internal initialization required == */


    /* == Do any external initialization required == */
#ifndef SIORS_USES_MDDPDD_DRIVER  /* If MDD/PDD driver is not in use */
  
#ifdef FEATURE_FIRST_UART
  sio_rs232_dev_init( SIO_PORT_UART_MAIN);
#endif 

#ifdef FEATURE_SECOND_UART
  sio_rs232_dev_init( SIO_PORT_UART_AUX);
#endif

#ifdef FEATURE_THIRD_UART
  sio_rs232_dev_init( SIO_PORT_UART_THIRD);
#endif

#ifndef SIO_NO_UART
  /* SIO_PORT_MAX marks the finish of init*/
  sio_rs232_dev_init( SIO_PORT_MAX );   
#endif

#else /* If MDD/PDD driver is in use */

#ifndef SIO_NO_UART
  siors_mdd_api_dev_init();
#endif
  
#endif /* SIORS_USES_MDDPDD_DRIVER */

#if !defined (FEATURE_NO_USB_HW_ON_PROCESSOR) && !defined(FEATURE_NO_SIO_USB_DEV_INIT)
#if defined (FEATURE_USB_CDC_ACM) || \
    defined (FEATURE_USB_DIAG)    || \
    defined (FEATURE_USB_DIAG_NMEA) || \
    defined (FEATURE_USB_PERIPHERAL_MASS_STORAGE)
  // Currently done in MC.
  sio_usb_dev_init();
#endif /* FTE_USB_CDC_ACM or FT_USB_DIAG or FT_USB && FT_MMC */
#endif /* !defined (FEATURE_NO_USB_HW_ON_PROCESSOR) */

#ifdef FEATURE_SMEM_DS
#error code not present
#endif /* FEATURE_SMEM_DS */

#ifdef FEATURE_SMD
  smd_sio_dev_init();

  #ifdef FEATURE_SMD_BRIDGE
  smd_bridge_init();
    
  #if defined(FEATURE_SMD_BRIDGE) && defined(IMAGE_MODEM_PROC)
  smd_port_mgr_init();
  #endif /* IMAGE_MODEM_PROC */
  
  #endif /* FEATURE_SMD_BRIDGE */
#endif /* FEATURE_SMD */

#ifdef FEATURE_QMIP
#error code not present
#endif

#ifdef FEATURE_APS
  aps_serial_vsp_dev_init();
#endif

    /* == Start to register all the driver that uses SIO == */
    /* As a temporary push to get the change rolling, the sequence where
     * the driver is registered must be the same as the definition
     * of sio_port_id_type */

    /* UART1 */
#ifdef FEATURE_FIRST_UART
#ifdef SIORS_USES_MDDPDD_DRIVER
    Info.HW_open = siors_mdd_api_open_stream;
    Info.HW_close = siors_mdd_api_close_stream;
    Info.HW_ioctl = siors_mdd_api_ioctl;
    Info.HW_flush_tx = siors_mdd_api_flush_tx;
    Info.HW_transmit = siors_mdd_api_dev_transmit;
    Info.HW_disable_device = siors_mdd_api_disable_device;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(1, 1);
#else
    Info.HW_open = sio_rs232_open_stream;
    Info.HW_close = sio_rs232_close_stream;
    Info.HW_ioctl = sio_rs232_ioctl;
    Info.HW_flush_tx = sio_rs232_flush_tx;
    Info.HW_transmit = sio_rs232_dev_transmit;
    Info.HW_disable_device = sio_rs232_disable_device;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(1, 1);
#endif
#else /* FEATURE_FIRST_UART */
    Info.HW_open = NULL;
    Info.HW_close = NULL;
    Info.HW_ioctl = NULL;
    Info.HW_flush_tx = NULL;
    Info.HW_transmit = NULL;
    Info.HW_disable_device = NULL;
    Info.forward_port = TRUE;
    Info.device_id = DEVICE_ID(0, 0);
#endif  
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = sio_wrapper_no_control_open;

    ret = sio_register_driver(&Info);
    ASSERT(ret);

    /* UART2 */
#ifdef FEATURE_SECOND_UART
#ifdef SIORS_USES_MDDPDD_DRIVER
    Info.HW_open = siors_mdd_api_open_stream;
    Info.HW_close = siors_mdd_api_close_stream;
    Info.HW_ioctl = siors_mdd_api_ioctl;
    Info.HW_flush_tx = siors_mdd_api_flush_tx;
    Info.HW_transmit = siors_mdd_api_dev_transmit;
    Info.HW_disable_device = siors_mdd_api_disable_device;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(1, 2);
#else
    Info.HW_open = sio_rs232_open_stream;
    Info.HW_close = sio_rs232_close_stream;
    Info.HW_ioctl = sio_rs232_ioctl;
    Info.HW_flush_tx = sio_rs232_flush_tx;
    Info.HW_transmit = sio_rs232_dev_transmit;
    Info.HW_disable_device = sio_rs232_disable_device;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(1, 2);
#endif
#else  /* FEATURE_SECOND_UART */
    Info.HW_open = NULL;
    Info.HW_close = NULL;
    Info.HW_ioctl = NULL;
    Info.HW_flush_tx = NULL;
    Info.HW_transmit = NULL;
    Info.HW_disable_device = NULL;
    Info.forward_port = TRUE;
    Info.device_id = DEVICE_ID(0, 0);
#endif  
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = sio_wrapper_no_control_open;

    ret = sio_register_driver(&Info);
    ASSERT(ret);

    /* UART3 */
#ifdef FEATURE_THIRD_UART
#ifdef SIORS_USES_MDDPDD_DRIVER
    Info.HW_open = siors_mdd_api_open_stream;
    Info.HW_close = siors_mdd_api_close_stream;
    Info.HW_ioctl = siors_mdd_api_ioctl;
    Info.HW_flush_tx = siors_mdd_api_flush_tx;
    Info.HW_transmit = siors_mdd_api_dev_transmit;
    Info.HW_disable_device = siors_mdd_api_disable_device;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(1, 3);
#else
    Info.HW_open = sio_rs232_open_stream;
    Info.HW_close = sio_rs232_close_stream;
    Info.HW_ioctl = sio_rs232_ioctl;
    Info.HW_flush_tx = sio_rs232_flush_tx;
    Info.HW_transmit = sio_rs232_dev_transmit;
    Info.HW_disable_device = sio_rs232_disable_device;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(1, 3);
#endif
#else /* FEATURE_THIRD_UART */
    Info.HW_open = NULL;
    Info.HW_close = NULL;
    Info.HW_ioctl = NULL;
    Info.HW_flush_tx = NULL;
    Info.HW_transmit = NULL;
    Info.HW_disable_device = NULL;
    Info.forward_port = TRUE;
    Info.device_id = DEVICE_ID(0, 0);
#endif  
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = sio_wrapper_no_control_open;

    ret = sio_register_driver(&Info);
    ASSERT(ret);

    /* USB_MDM */
#if defined (FEATURE_USB_CDC_ACM) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
#ifdef SIO_USES_HS_USB
    Info.HW_open = hsu_al_ser_open;
    Info.HW_close = sio_wrapper_hs_usb_close;
    Info.HW_ioctl = hsu_al_ser_ioctl;
    Info.HW_flush_tx = hsu_al_ser_flush_tx;
    Info.HW_transmit = hsu_al_ser_transmit;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = NULL;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(2, 1);
#else
    Info.HW_open = sio_usb_open_stream;
    Info.HW_close = sio_wrapper_usb_close;
    Info.HW_ioctl = sio_usb_ioctl;
    Info.HW_flush_tx = sio_usb_flush_tx;
    Info.HW_transmit = sio_usb_dev_transmit;
    Info.HW_disable_device = sio_usb_disable_device;
    Info.HW_control_transmit = sio_usb_control_transmit;
    Info.HW_control_close = sio_usb_control_close_stream;
    Info.HW_control_open = sio_usb_control_open_stream;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(2, 1);
#endif
#else   /* Driver not available */
    Info.HW_open = NULL;
    Info.HW_close = NULL;
    Info.HW_ioctl = NULL;
    Info.HW_flush_tx = NULL;
    Info.HW_transmit = NULL;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = NULL;
    Info.forward_port = TRUE;
    Info.device_id = DEVICE_ID(0, 0);
#endif
    ret = sio_register_driver(&Info);
    ASSERT(ret);

    /* USB_SER1 / DIAG */
#if defined(SIO_USES_FSHS_USB) && \
   (defined( FEATURE_USB_DIAG) || defined(FEATURE_USB_DIAG_NMEA)) && \
   !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
#ifdef SIO_USES_HS_USB
    Info.HW_open = hsu_al_ser_open;
    Info.HW_close = sio_wrapper_hs_usb_close;
    Info.HW_ioctl = hsu_al_ser_ioctl;
    Info.HW_flush_tx = hsu_al_ser_flush_tx;
    Info.HW_transmit = hsu_al_ser_transmit;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = NULL;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(2, 2);
#else
    Info.HW_open = sio_usb_open_stream;
    Info.HW_close = sio_wrapper_usb_close;
    Info.HW_ioctl = sio_usb_ioctl;
    Info.HW_flush_tx = sio_usb_flush_tx;
    Info.HW_transmit = sio_usb_dev_transmit;
    Info.HW_disable_device = sio_usb_disable_device;
    Info.HW_control_transmit = sio_usb_control_transmit;
    Info.HW_control_close = sio_usb_control_close_stream;
    Info.HW_control_open = sio_usb_control_open_stream;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(2, 2);
#endif
#else   /* Driver not available */
    Info.HW_open = NULL;
    Info.HW_close = NULL;
    Info.HW_ioctl = NULL;
    Info.HW_flush_tx = NULL;
    Info.HW_transmit = NULL;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = NULL;
    Info.forward_port = TRUE;
    Info.device_id = DEVICE_ID(0, 0);
#endif
    ret = sio_register_driver(&Info);
    ASSERT(ret);

    /* USB_MMC */
#if defined(FEATURE_USB_PERIPHERAL_MASS_STORAGE) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
    Info.HW_open = sio_usb_open_stream;
    Info.HW_close = sio_wrapper_usb_close;
    Info.HW_ioctl = sio_usb_ioctl;
    Info.HW_flush_tx = sio_usb_flush_tx;
    Info.HW_transmit = sio_usb_dev_transmit;
    Info.HW_disable_device = sio_usb_disable_device;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = sio_wrapper_no_control_open;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(2, 3);
#else   /* Driver not available */
    Info.HW_open = NULL;
    Info.HW_close = NULL;
    Info.HW_ioctl = NULL;
    Info.HW_flush_tx = NULL;
    Info.HW_transmit = NULL;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = NULL;
    Info.forward_port = TRUE;
    Info.device_id = DEVICE_ID(0, 0);
#endif
    ret = sio_register_driver(&Info);
    ASSERT(ret);

    /* BT */
#ifdef FEATURE_BT
    Info.HW_open = bt_sio_cmd_open;
    Info.HW_close = bt_sio_cmd_close;
    Info.HW_ioctl = sio_wrapper_bt_ioctl;
    Info.HW_flush_tx = sio_wrapper_bt_tx_flush;
    Info.HW_transmit = sio_wrapper_bt_transmit;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = sio_wrapper_no_control_open;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(3, 1);
#else   /* Driver not available */
    Info.HW_open = NULL;
    Info.HW_close = NULL;
    Info.HW_ioctl = NULL;
    Info.HW_flush_tx = NULL;
    Info.HW_transmit = NULL;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = NULL;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(0, 0);
#endif
    ret = sio_register_driver(&Info);         /* BT SPP */
    ASSERT(ret);

#ifdef FEATURE_BT
    Info.device_id = DEVICE_ID(3, 2);
#endif
    ret = sio_register_driver(&Info);         /* BT NA  */
    ASSERT(ret);

    /* USB_QMIP */
    /* Driver not available */
    Info.HW_open = NULL;
    Info.HW_close = NULL;
    Info.HW_ioctl = NULL;
    Info.HW_flush_tx = NULL;
    Info.HW_transmit = NULL;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = NULL;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(0, 0);
    ret = sio_register_driver(&Info);

    /* SMEM_DS */
#ifdef FEATURE_SMEM_DS
#error code not present
#else   /* Driver not available */
    Info.HW_open = NULL;
    Info.HW_close = NULL;
    Info.HW_ioctl = NULL;
    Info.HW_flush_tx = NULL;
    Info.HW_transmit = NULL;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = NULL;
    Info.device_id = DEVICE_ID(0, 0);
#endif

    /* Not forward all SMD ports */
    Info.forward_port = FALSE;

    ret = sio_register_driver(&Info);
    ASSERT(ret);

#ifdef FEATURE_SMD
#define SMD_ID_MACRO(major, minor)     Info.device_id = DEVICE_ID(major,minor)
#else
#define SMD_ID_MACRO(major, minor)     
#endif
    
    /* SMD */
#ifdef FEATURE_SMD
    Info.HW_open = smd_sio_open;
    Info.HW_close = sio_wrapper_smd_sio_close;
    Info.HW_ioctl = sio_wrapper_smd_ioctl;
    Info.HW_flush_tx = sio_wrapper_smd_flush_tx;
    Info.HW_transmit = sio_wrapper_smd_dev_tx;
    Info.HW_disable_device = sio_wrapper_smd_disable;
    Info.HW_control_transmit = sio_wrapper_smd_control_transmit;
    Info.HW_control_close = sio_wrapper_smd_control_close;
    Info.HW_control_open = smd_sio_control_open;
    Info.device_id = DEVICE_ID(6, 1);
#else   /* Driver not available */
    Info.HW_open = NULL;
    Info.HW_close = NULL;
    Info.HW_ioctl = NULL;
    Info.HW_flush_tx = NULL;
    Info.HW_transmit = NULL;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = NULL;
    Info.device_id = DEVICE_ID(0, 0);
#endif
   
    /* Not forward all SMD ports */
    Info.forward_port = FALSE;

    ret = sio_register_driver(&Info); /* SMD_DS */
    SMD_ID_MACRO(6, 2);
    ret |= sio_register_driver(&Info); /* SMD_DIAG */
    SMD_ID_MACRO(6, 3);
    ret |= sio_register_driver(&Info); /* SMD_DIAG_APPS */
    SMD_ID_MACRO(6, 4);
    ret |= sio_register_driver(&Info); /* SMD_DIAG_MODEM */
    SMD_ID_MACRO(6, 5);
    ret |= sio_register_driver(&Info); /* SMD_DIAG_QDSP */
    SMD_ID_MACRO(6, 6);
    ret |= sio_register_driver(&Info); /* SMD_RPC_CALL */
    SMD_ID_MACRO(6, 7);
    ret |= sio_register_driver(&Info); /* SMD_RPC_REPLY */
    SMD_ID_MACRO(6, 8);
    ret |= sio_register_driver(&Info); /* SMD_BT */
    SMD_ID_MACRO(6, 9);
    ret |= sio_register_driver(&Info); /* SMD_CONTROL */
    SMD_ID_MACRO(6, 10);
    ret |= sio_register_driver(&Info); /* SMD_MEMCPY */
    SMD_ID_MACRO(6, 11);
    ret |= sio_register_driver(&Info); /* SMD_DATA1 */
    SMD_ID_MACRO(6, 12);
    ret |= sio_register_driver(&Info); /* SMD_DATA2 */
    SMD_ID_MACRO(6, 13);
    ret |= sio_register_driver(&Info); /* SMD_DATA3 */
    SMD_ID_MACRO(6, 14);
    ret |= sio_register_driver(&Info); /* SMD_DATA4 */
    SMD_ID_MACRO(6, 15);
    ret |= sio_register_driver(&Info); /* SMD_DATA5 */
    SMD_ID_MACRO(6, 16);
    ret |= sio_register_driver(&Info); /* SMD_DATA6 */
    SMD_ID_MACRO(6, 17);
    ret |= sio_register_driver(&Info); /* SMD_DATA7 */
    SMD_ID_MACRO(6, 18);
    ret |= sio_register_driver(&Info); /* SMD_DATA8 */
    SMD_ID_MACRO(6, 19);
    ret |= sio_register_driver(&Info); /* SMD_DATA9 */
    SMD_ID_MACRO(6, 20);
    ret |= sio_register_driver(&Info); /* SMD_DATA10 */
    SMD_ID_MACRO(6, 21);
    ret |= sio_register_driver(&Info); /* SMD_DATA11 */
    SMD_ID_MACRO(6, 22);
    ret |= sio_register_driver(&Info); /* SMD_DATA12 */
    SMD_ID_MACRO(6, 23);
    ret |= sio_register_driver(&Info); /* SMD_DATA13 */
    SMD_ID_MACRO(6, 24);
    ret |= sio_register_driver(&Info); /* SMD_DATA14 */
    SMD_ID_MACRO(6, 25);
    ret |= sio_register_driver(&Info); /* SMD_DATA15 */
    SMD_ID_MACRO(6, 26);
    ret |= sio_register_driver(&Info); /* SMD_DATA16 */
    SMD_ID_MACRO(6, 27);
    ret |= sio_register_driver(&Info); /* SMD_DATA17 */
    SMD_ID_MACRO(6, 28);
    ret |= sio_register_driver(&Info); /* SMD_DATA18 */
    SMD_ID_MACRO(6, 29);
    ret |= sio_register_driver(&Info); /* SMD_DATA19 */
    SMD_ID_MACRO(6, 30);
    ret |= sio_register_driver(&Info); /* SMD_DATA20 */
    SMD_ID_MACRO(6, 31);
    ret |= sio_register_driver(&Info); /* SMD_DATA21 */
    SMD_ID_MACRO(6, 32);
    ret |= sio_register_driver(&Info); /* SMD_DATA22 */
    SMD_ID_MACRO(6, 33);
    ret |= sio_register_driver(&Info); /* SMD_DATA23 */
    SMD_ID_MACRO(6, 34);
    ret |= sio_register_driver(&Info); /* SMD_DATA24 */
    SMD_ID_MACRO(6, 35);
    ret |= sio_register_driver(&Info); /* SMD_DATA25 */
    SMD_ID_MACRO(6, 36);
    ret |= sio_register_driver(&Info); /* SMD_DATA26 */
    SMD_ID_MACRO(6, 37);
    ret |= sio_register_driver(&Info); /* SMD_DATA27 */
    SMD_ID_MACRO(6, 38);
    ret |= sio_register_driver(&Info); /* SMD_DATA28 */
    SMD_ID_MACRO(6, 39);
    ret |= sio_register_driver(&Info); /* SMD_DATA29 */
    SMD_ID_MACRO(6, 40);
    ret |= sio_register_driver(&Info); /* SMD_DATA30 */
    SMD_ID_MACRO(6, 41);
    ret |= sio_register_driver(&Info); /* SMD_DATA31 */
    SMD_ID_MACRO(6, 42);
    ret |= sio_register_driver(&Info); /* SMD_DATA32 */
    SMD_ID_MACRO(6, 43);
    ret |= sio_register_driver(&Info); /* SMD_DATA33 */
    SMD_ID_MACRO(6, 44);
    ret |= sio_register_driver(&Info); /* SMD_DATA34 */
    SMD_ID_MACRO(6, 45);
    ret |= sio_register_driver(&Info); /* SMD_DATA35 */
    SMD_ID_MACRO(6, 46);
    ret |= sio_register_driver(&Info); /* SMD_DATA36 */
    SMD_ID_MACRO(6, 47);
    ret |= sio_register_driver(&Info); /* SMD_DATA37 */
    SMD_ID_MACRO(6, 48);
    ret |= sio_register_driver(&Info); /* SMD_DATA38 */
    SMD_ID_MACRO(6, 49);
    ret |= sio_register_driver(&Info); /* SMD_DATA39 */
    SMD_ID_MACRO(6, 50);
    ret |= sio_register_driver(&Info); /* SMD_DATA40 */
    SMD_ID_MACRO(6, 51);
    ret |= sio_register_driver(&Info); /* SMD_GPS_NMEA */
	ASSERT(ret);
#undef SMD_ID_MACRO
#ifdef FEATURE_SMD_BRIDGE
#define SMD_ID_MACRO(major, minor)     Info.device_id = DEVICE_ID(major,minor)
#else
#define SMD_ID_MACRO(major, minor)     
#endif
    
    /* SMD BRIDGE */
#ifdef FEATURE_SMD_BRIDGE
    Info.HW_open = sio_wrapper_smd_bridge_open;
    Info.HW_close = sio_wrapper_smd_bridge_close;
    Info.HW_ioctl = sio_wrapper_smd_ioctl;
    Info.HW_flush_tx = sio_wrapper_smd_flush_tx;
    Info.HW_transmit = sio_wrapper_smd_dev_tx;
    Info.HW_disable_device = sio_wrapper_smd_disable;
    Info.HW_control_transmit = sio_wrapper_smd_control_transmit;
    Info.HW_control_close = sio_wrapper_smd_control_close;
    Info.HW_control_open = smd_sio_control_open;
    Info.device_id = DEVICE_ID(7, 1);
#else   /* Driver not available */
    Info.HW_open = NULL;
    Info.HW_close = NULL;
    Info.HW_ioctl = NULL;
    Info.HW_flush_tx = NULL;
    Info.HW_transmit = NULL;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = NULL;
    Info.device_id = DEVICE_ID(0, 0);
#endif
    /* Not forward all SMD ports */
    Info.forward_port = FALSE;

    ret = sio_register_driver(&Info); /* SMD_BRIDGE1 */
    SMD_ID_MACRO(7, 2);
    ret |= sio_register_driver(&Info); /* SMD_BRIDGE2 */
    SMD_ID_MACRO(7, 3);
    ret |= sio_register_driver(&Info); /* SMD_BRIDGE3 */
    SMD_ID_MACRO(7, 4);
    ret |= sio_register_driver(&Info); /* SMD_BRIDGE4 */
    SMD_ID_MACRO(7, 5);
    ret |= sio_register_driver(&Info); /* SMD_BRIDGE5 */
    ASSERT(ret);
#undef SMD_ID_MACRO

    /* CSD */
#ifdef FEATURE_MMGPS_CSD
#error code not present
#else   /* Driver not available */
    Info.HW_open = NULL;
    Info.HW_close = NULL;
    Info.HW_ioctl = NULL;
    Info.HW_flush_tx = NULL;
    Info.HW_transmit = NULL;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = NULL;
    Info.forward_port = TRUE;
    Info.device_id = DEVICE_ID(0, 0);
#endif
    ret = sio_register_driver(&Info);
    ASSERT(ret);

    /* USB_SER2 */
#if defined (FEATURE_USB_DIAG_NMEA) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
#ifdef SIO_USES_HS_USB
    Info.HW_open = hsu_al_ser_open;
    Info.HW_close = sio_wrapper_hs_usb_close;
    Info.HW_ioctl = hsu_al_ser_ioctl;
    Info.HW_flush_tx = hsu_al_ser_flush_tx;
    Info.HW_transmit = hsu_al_ser_transmit;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = NULL;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(2, 4);
#else
    Info.HW_open = sio_usb_open_stream;
    Info.HW_close = sio_wrapper_usb_close;
    Info.HW_ioctl = sio_usb_ioctl;
    Info.HW_flush_tx = sio_usb_flush_tx;
    Info.HW_transmit = sio_usb_dev_transmit;
    Info.HW_disable_device = sio_usb_disable_device;
    Info.HW_control_transmit = sio_usb_control_transmit;
    Info.HW_control_close = sio_usb_control_close_stream;
    Info.HW_control_open = sio_usb_control_open_stream;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(2, 4);
#endif
#else   /* Driver not available */
    Info.HW_open = NULL;
    Info.HW_close = NULL;
    Info.HW_ioctl = NULL;
    Info.HW_flush_tx = NULL;
    Info.HW_transmit = NULL;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = NULL;
    Info.forward_port = TRUE;
    Info.device_id = DEVICE_ID(0, 0);
#endif
    ret = sio_register_driver(&Info);
    ASSERT(ret);

    /* USB_RRDM */
#if defined (FEATURE_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)
    Info.HW_open = sio_usb_open_stream;
    Info.HW_close = sio_wrapper_usb_close;
    Info.HW_ioctl = NULL;
    Info.HW_flush_tx = sio_usb_flush_tx;
    Info.HW_transmit = sio_usb_dev_transmit;
    Info.HW_disable_device = sio_usb_disable_device;
    Info.HW_control_transmit = sio_usb_control_transmit;
    Info.HW_control_close = sio_usb_control_close_stream;
    Info.HW_control_open = sio_usb_control_open_stream;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(2, 5);
#else   /* Driver not available */
    Info.HW_open = NULL;
    Info.HW_close = NULL;
    Info.HW_ioctl = NULL;
    Info.HW_flush_tx = NULL;
    Info.HW_transmit = NULL;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = NULL;
    Info.forward_port = TRUE;
    Info.device_id = DEVICE_ID(0, 0);
#endif
    ret = sio_register_driver(&Info);     /* RRDM */
    ASSERT(ret);

    /* NET_WWAN */
#if defined (SIO_USES_FSHS_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR) && \
    defined (FEATURE_HS_USB_ECM)
#ifdef SIO_USES_HS_USB
    Info.HW_open = hsu_al_ecm_data_open;
    Info.HW_close = sio_wrapper_hs_usb_ecm_close;
    Info.HW_ioctl = hsu_al_ecm_ioctl;
    Info.HW_flush_tx = hsu_al_ecm_data_flush_tx;
    Info.HW_transmit = hsu_al_ecm_data_transmit;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = hsu_al_ecm_control_transmit;
    Info.HW_control_close = hsu_al_ecm_control_close;
    Info.HW_control_open = hsu_al_ecm_control_open;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(2, 6);

#else
    Info.HW_open = sio_usb_open_stream;
    Info.HW_close = sio_wrapper_usb_close;
    Info.HW_ioctl = sio_usb_ioctl;
    Info.HW_flush_tx = sio_usb_flush_tx;
    Info.HW_transmit = sio_usb_dev_transmit;
    Info.HW_disable_device = sio_usb_disable_device;
    Info.HW_control_transmit = sio_usb_control_transmit;
    Info.HW_control_close = sio_usb_control_close_stream;
    Info.HW_control_open = sio_usb_control_open_stream;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(2, 6);
#endif  /* HS USB */
#else   /* Driver not available */
    Info.HW_open = NULL;
    Info.HW_close = NULL;
    Info.HW_ioctl = NULL;
    Info.HW_flush_tx = NULL;
    Info.HW_transmit = NULL;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = NULL;
    Info.forward_port = TRUE;
    Info.device_id = DEVICE_ID(0, 0);
#endif
    ret = sio_register_driver(&Info); 
    ASSERT(ret);

    /* RMNET_2 */
#if defined (SIO_USES_FSHS_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR) && defined(FEATURE_HS_USB_ECM)
#ifdef SIO_USES_HS_USB
    Info.HW_open = hsu_al_ecm_data_open;
    Info.HW_close = sio_wrapper_hs_usb_ecm_close;
    Info.HW_ioctl = hsu_al_ecm_ioctl;
    Info.HW_flush_tx = hsu_al_ecm_data_flush_tx;
    Info.HW_transmit = hsu_al_ecm_data_transmit;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = hsu_al_ecm_control_transmit;
    Info.HW_control_close = hsu_al_ecm_control_close;
    Info.HW_control_open = NULL;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(2, 7);

#else
    Info.HW_open = sio_usb_open_stream;
    Info.HW_close = sio_wrapper_usb_close;
    Info.HW_ioctl = sio_usb_ioctl;
    Info.HW_flush_tx = sio_usb_flush_tx;
    Info.HW_transmit = sio_usb_dev_transmit;
    Info.HW_disable_device = sio_usb_disable_device;
    Info.HW_control_transmit = sio_usb_control_transmit;
    Info.HW_control_close = sio_usb_control_close_stream;
    Info.HW_control_open = sio_usb_control_open_stream;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(2, 7);
#endif  /* HS USB */
#else   /* Driver not available */
    Info.HW_open = NULL;
    Info.HW_close = NULL;
    Info.HW_ioctl = NULL;
    Info.HW_flush_tx = NULL;
    Info.HW_transmit = NULL;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = NULL;
    Info.forward_port = TRUE;
    Info.device_id = DEVICE_ID(0, 0);
#endif
    ret = sio_register_driver(&Info); 
    ASSERT(ret);

    /* RMNET_3 */
#if defined (SIO_USES_FSHS_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR) && defined(FEATURE_HS_USB_ECM)
#ifdef SIO_USES_HS_USB
    Info.HW_open = hsu_al_ecm_data_open;
    Info.HW_close = sio_wrapper_hs_usb_ecm_close;
    Info.HW_ioctl = hsu_al_ecm_ioctl;
    Info.HW_flush_tx = hsu_al_ecm_data_flush_tx;
    Info.HW_transmit = hsu_al_ecm_data_transmit;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = hsu_al_ecm_control_transmit;
    Info.HW_control_close = hsu_al_ecm_control_close;
    Info.HW_control_open = NULL;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(2, 8);

#else
    Info.HW_open = sio_usb_open_stream;
    Info.HW_close = sio_wrapper_usb_close;
    Info.HW_ioctl = sio_usb_ioctl;
    Info.HW_flush_tx = sio_usb_flush_tx;
    Info.HW_transmit = sio_usb_dev_transmit;
    Info.HW_disable_device = sio_usb_disable_device;
    Info.HW_control_transmit = sio_usb_control_transmit;
    Info.HW_control_close = sio_usb_control_close_stream;
    Info.HW_control_open = sio_usb_control_open_stream;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(2, 8);
#endif  /* HS USB */
#else   /* Driver not available */
    Info.HW_open = NULL;
    Info.HW_close = NULL;
    Info.HW_ioctl = NULL;
    Info.HW_flush_tx = NULL;
    Info.HW_transmit = NULL;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = NULL;
    Info.forward_port = TRUE;
    Info.device_id = DEVICE_ID(0, 0);
#endif
    ret = sio_register_driver(&Info); 
    ASSERT(ret);

    /* APS */
#ifdef FEATURE_APS
    Info.HW_open = aps_serial_vsp_open_stream;
    Info.HW_close = sio_wrapper_aps_close;
    Info.HW_ioctl = aps_serial_vsp_ioctl;
    Info.HW_flush_tx = aps_serial_vsp_flush_tx;
    Info.HW_transmit = aps_serial_vsp_dev_transmit;
    Info.HW_disable_device = aps_serial_vsp_disable_device;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = sio_wrapper_no_control_open;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(10, 1);
#else   /* Driver not available */
    Info.HW_open = NULL;
    Info.HW_close = NULL;
    Info.HW_ioctl = NULL;
    Info.HW_flush_tx = NULL;
    Info.HW_transmit = NULL;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = NULL;
    Info.forward_port = TRUE;
    Info.device_id = DEVICE_ID(0, 0);
#endif
    ret = sio_register_driver(&Info);
    ASSERT(ret);
    
    /* USB_SER3 */
    /* This driver is only available if HS-USB is in use */
#if defined(SIO_HAS_USB_SER3) && defined(SIO_USES_HS_USB)
    Info.HW_open = hsu_al_ser_open;
    Info.HW_close = sio_wrapper_hs_usb_close;
    Info.HW_ioctl = hsu_al_ser_ioctl;
    Info.HW_flush_tx = hsu_al_ser_flush_tx;
    Info.HW_transmit = hsu_al_ser_transmit;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = NULL;
    Info.forward_port = FALSE;
    Info.device_id = DEVICE_ID(2, 7);
#else   /* Driver not available */
    Info.HW_open = NULL;
    Info.HW_close = NULL;
    Info.HW_ioctl = NULL;
    Info.HW_flush_tx = NULL;
    Info.HW_transmit = NULL;
    Info.HW_disable_device = NULL;
    Info.HW_control_transmit = NULL;
    Info.HW_control_close = NULL;
    Info.HW_control_open = NULL;
    Info.forward_port = TRUE;
    Info.device_id = DEVICE_ID(0, 0);
#endif
    ret = sio_register_driver(&Info);
    ASSERT(ret);
}


