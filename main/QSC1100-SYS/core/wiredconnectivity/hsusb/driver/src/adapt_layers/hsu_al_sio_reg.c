/*==============================================================================

High Speed USB SIO functions registration 

GENERAL DESCRIPTION
This module is responsible for registering HSUSB driver into SIO.

EXTERNALIZED FUNCTIONS
hsu_sio_reg_hsusb_driver

INITALIZATION AND SEQUENCING REQUIREMENTS
None.

Copyright (c) 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/adapt_layers/hsu_al_sio_reg.c#2 $
$DateTime: 2009/02/15 07:48:17 $ $Author: dsegal $

when      who     what, where, why
--------  ---     ------------------------------------------------------------
02/04/09  sm      inital varsion

==============================================================================*/

/*==============================================================================

INCLUDE FILES FOR MODULE

==============================================================================*/
#include "hsu_common.h" 

#ifdef FEATURE_HSUSB_WRAPPER_FREE

#include "hsu_al_sio_reg.h"
#include "hsu_al_ser.h"
#include "hsu_al_ecm.h"
#include "hsu_host_al_eem.h"

/*==============================================================================

DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/
/*==============================================================================
Typedefs
==============================================================================*/
/*==============================================================================
Variables
==============================================================================*/
/*==============================================================================
Forward Declarations
==============================================================================*/

static void hsu_sio_reg_ecm_control_close_wrapper
( 
  sio_stream_id_type stream_id, 
  sio_port_id_type   port_id,
  void               (*close_func_ptr)(void)
);

static void hsu_sio_reg_ecm_control_transmit_wrapper
( 
  sio_stream_id_type stream_id, 
  sio_port_id_type   port_id,
  dsm_item_type*     tx_ptr
);

static void al_sio_set_func_cb (sio_driver_interface* driver_iface, 
  void* data_open, void* data_close, void* ioctl, void* data_transmit,
  void* flush_tx, void* disable_device, void* control_open, void* control_close,
  void* control_ioctl, void* control_transmit);


static uint32 al_sio_reg_device_iface(sio_driver_interface* driver_iface, 
  uint32 sio_major_num, uint32 sio_minor_num, 
  sio_register_device_driver_cb_type register_driver_fn);
/*==============================================================================

FUNCTION DECLARATIONS FOR MODULE

==============================================================================*/
#ifdef FEATURE_HS_USB_ECM
/*==============================================================================

FUNCTION          HSU_SIO_REG_ECM_CONTROL_CLOSE_WRAPPER

DESCRIPTION   
  This function is a wrapper function for hsu_al_ecm_control_close().

DEPENDENCIES  
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
  None.
==============================================================================*/
static void hsu_sio_reg_ecm_control_close_wrapper
( 
  sio_stream_id_type stream_id, 
  sio_port_id_type port_id,
  void             (*close_func_ptr)(void)
)
{ 
  HSU_USE_PARAM(port_id);
  hsu_al_ecm_control_close(stream_id, close_func_ptr);
}
/*==============================================================================

FUNCTION          HSU_SIO_REG_ECM_CONTROL_TRANSMIT_WRAPPER

DESCRIPTION   
  This function is a wrapper function for hsu_al_ecm_control_transmit().

DEPENDENCIES  
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
  None.
==============================================================================*/
static void hsu_sio_reg_ecm_control_transmit_wrapper
( 
  sio_stream_id_type stream_id, 
  sio_port_id_type port_id,
  dsm_item_type *tx_ptr
)
{ 
  HSU_USE_PARAM(port_id);
  hsu_al_ecm_control_transmit(stream_id, tx_ptr);
}
#endif /* FEATURE_HS_USB_ECM */


/*==============================================================================

FUNCTION          HSU_AL_SIO_REG_HSUSB_DRIVER

DESCRIPTION   
  This function responsible for mapping HSUSB driver functions to SIO.

DEPENDENCIES  
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
  None.
==============================================================================*/

void hsu_al_sio_reg_hsusb_driver(sio_register_device_driver_cb_type register_driver_fn)
{
  sio_driver_interface driver_iface;
  uint32 ret;

/* SER */
#ifndef FEATURE_NO_USB_HW_ON_PROCESSOR
  al_sio_set_func_cb(&driver_iface ,(void*)hsu_al_ser_open, (void*)hsu_al_ser_close, (void*)hsu_al_ser_ioctl,
    (void*)hsu_al_ser_transmit, (void*)hsu_al_ser_flush_tx, NULL, NULL, NULL, NULL, NULL);
#else 
  al_sio_set_func_cb(&driver_iface ,NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL);
#endif

 /* USB_MDM */
#ifdef FEATURE_USB_CDC_ACM
  ret = al_sio_reg_device_iface(&driver_iface, SIO_MAJOR_HSUSB, SIO_MINOR_HSUSB_MDM,
    register_driver_fn);
  HSU_ASSERT(ret);
#endif /* FEATURE_USB_CDC_ACM */

/* USB_SER1/DIAG */
#if defined (SIO_USES_FSHS_USB) && (defined (FEATURE_USB_DIAG) || defined(FEATURE_USB_DIAG_NMEA))
  ret = al_sio_reg_device_iface(&driver_iface, SIO_MAJOR_HSUSB, SIO_MINOR_HSUSB_SER1,
    register_driver_fn);
  HSU_ASSERT(ret);
#endif /* SIO_USES_FSHS_USB && (FEATURE_USB_DIAG || FEATURE_USB_DIAG_NMEA) */

 /* USB_SER2 */
#if defined (FEATURE_USB_DIAG_NMEA) 
  ret = al_sio_reg_device_iface(&driver_iface, SIO_MAJOR_HSUSB, SIO_MINOR_HSUSB_SER2,
    register_driver_fn);
  HSU_ASSERT(ret);
#endif /* FEATURE_USB_DIAG_NMEA */

 /* USB_SER3 */
#if defined(SIO_HAS_USB_SER3) 
  ret = al_sio_reg_device_iface(&driver_iface, SIO_MAJOR_HSUSB, SIO_MINOR_HSUSB_SER3,
    register_driver_fn);
  HSU_ASSERT(ret);
#endif
    
 /* RM_NET */
#ifdef FEATURE_HS_USB_ECM
#if defined (SIO_USES_FSHS_USB) && !defined (FEATURE_NO_USB_HW_ON_PROCESSOR)  
  al_sio_set_func_cb(&driver_iface, (void*)hsu_al_ecm_data_open, (void*)hsu_al_ecm_data_close, 
   (void*)hsu_al_ecm_ioctl, (void*)hsu_al_ecm_data_transmit, (void*)hsu_al_ecm_data_flush_tx, NULL, (void*)hsu_al_ecm_control_open,
   (void*)hsu_sio_reg_ecm_control_close_wrapper, NULL, (void*)hsu_sio_reg_ecm_control_transmit_wrapper);
#else
 al_sio_set_func_cb(&driver_iface ,NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL);
#endif /* SIO_USES_FSHS_USB && !FEATURE_NO_USB_HW_ON_PROCESSOR && FEATURE_HS_USB_ECM */
    
  ret = al_sio_reg_device_iface(&driver_iface, SIO_MAJOR_HSUSB_ECM, SIO_MINOR_HSUSB_ECM_NET_WWAN,
    register_driver_fn);
  HSU_ASSERT(ret);
#endif /* FEATURE_HS_USB_ECM */

/* EEM */
#if defined (FEATURE_HS_USB_UICC)
#ifndef FEATURE_NO_USB_HW_ON_PROCESSOR
  al_sio_set_func_cb (&driver_iface, (void*)hsu_host_al_eem_data_open, (void*)hsu_host_al_eem_data_close,
  (void*)hsu_host_al_eem_ioctl, (void*)hsu_host_al_eem_data_transmit,
  (void*)hsu_host_al_eem_data_flush_tx, NULL, NULL, NULL, NULL, NULL);
#else
  al_sio_set_func_cb(&driver_iface ,NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL);
#endif /* FEATURE_NO_USB_HW_ON_PROCESSOR */
  ret = al_sio_reg_device_iface (&driver_iface, SIO_MAJOR_HSUSB_EEM, SIO_MINOR_HSUSB_EEM,
    register_driver_fn);
  HSU_ASSERT(ret);
#endif /* FEATURE_HS_USB_UICC */
}
/*==============================================================================

FUNCTION          AL_SIO_REG_DEVICE_INFACE

DESCRIPTION   
  This function responsible for registering additional details for each 
  class driver (e.g majoy and minor numbers).

DEPENDENCIES  
  None.

RETURN VALUE  
  0 if fails.

SIDE EFFECTS  
  None.
==============================================================================*/
static uint32 al_sio_reg_device_iface(sio_driver_interface* driver_iface,
  uint32 sio_major_num, uint32 sio_minor_num, 
  sio_register_device_driver_cb_type register_driver_fn)
{
  sio_device_interafce device_iface;
  boolean enb_fwd_port;
  boolean is_driver_available;

#ifndef FEATURE_NO_USB_HW_ON_PROCESSOR
  enb_fwd_port = FALSE;
  is_driver_available = TRUE;
#else  
  enb_fwd_port = TRUE;
  is_driver_available = FALSE;
#endif /* FEATURE_NO_USB_HW_ON_PROCESSOR */

  device_iface.driver_interface = driver_iface;
  device_iface.forward_port     = enb_fwd_port;
  device_iface.driver_available = is_driver_available;
  device_iface.device_id        = DEVICE_ID(sio_major_num, sio_minor_num); 
  return register_driver_fn(&device_iface);
}
/*==============================================================================

FUNCTION          AL_SIO_SET_FUNC_CB

DESCRIPTION   
  This function responsible for mapping HSUSB driver functions to SIO.

DEPENDENCIES  
  None.

RETURN VALUE  
  None.

SIDE EFFECTS  
  None.
==============================================================================*/
static void al_sio_set_func_cb (sio_driver_interface* driver_iface, 
  void* data_open, void* data_close,void* ioctl, void* data_transmit,
  void* flush_tx, void* disable_device, void* control_open, void* control_close,
  void* control_ioctl, void* control_transmit
)
{
  (void *)(driver_iface->HW_open)             = data_open;
  (void *)(driver_iface->HW_close)            = data_close; 
  (void *)(driver_iface->HW_ioctl)            = ioctl; 
  (void *)(driver_iface->HW_transmit)         = data_transmit;
  (void *)(driver_iface->HW_flush_tx)         = flush_tx;
  (void *)(driver_iface->HW_disable_device)   = disable_device;
  (void *)(driver_iface->HW_control_open)     = control_open;
  (void *)(driver_iface->HW_control_close)    = control_close;
  (void *)(driver_iface->HW_control_ioctl)    = control_ioctl;
  (void *)(driver_iface->HW_control_transmit) = control_transmit;
}
#endif /* FEATURE_HSUSB_WRAPPER_FREE */
