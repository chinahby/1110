#ifndef _HSU_AL_ECM_H_
#define _HSU_AL_ECM_H_

/*==============================================================================

High Speed USB Ethernet Networking control Model (CDC/ECM) Adaptation Layer
H E A D E R   F I L E

GENERAL DESCRIPTION
Defines externalized data and functions for the ECM adaptation layer, 
towards SIO

EXTERNALIZED FUNCTIONS
hsu_al_ecm_config
hsu_al_ecm_control_open
hsu_al_ecm_data_open
hsu_al_ecm_control_transmit
hsu_al_ecm_data_transmit
hsu_al_ecm_ioctl
hsu_al_ecm_data_flush_tx
hsu_al_ecm_control_close
hsu_al_ecm_data_close
hsu_al_ecm_enable_dl_tlp
hsu_al_ecm_enable_ul_tlp
hsu_al_ecm_handle_set_configuration

INITALIZATION AND SEQUENCING REQUIREMENTS
The stack must be initialized first (which causes ecm_init to be called), 
before all other functions in this header file can be used.

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_al_ecm.h#7 $
$DateTime: 2008/10/19 00:54:54 $ $Author: merez $

when     who  what, where, why
-------- ---  ---------------------------------------------------------
05/17/06  ke  Initial version

==============================================================================*/
#include "sio.h"

/*==============================================================================
Forward Declarations
==============================================================================*/

/*==============================================================================
Constants
==============================================================================*/
#define TLP_HEADER_SIZE 2

/*==============================================================================
Macros
==============================================================================*/

/*==============================================================================
Typedefs
==============================================================================*/

/*==============================================================================
PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*==============================================================================

FUNCTION      HSU_AL_ECM_CONFIG

DESCRIPTION   
Called by the hsu_config_selector when the device stack is initialized for the 
first time after power up. 
The function requests from the hsu_config_selector the maximal amount of 
memory it may require, and initializes the buffers passed to the HS-USB HW 
according to the allocated address. 

DEPENDENCIES  
This function should be called before any other function is called, 
specifically before ecm_init is called.

RETURN VALUE  
TRUE for success, FALSE if the memory allocation by the hsu_config_selector 
failed. 

SIDE EFFECTS  
None. 

==============================================================================*/
boolean hsu_al_ecm_config(void);

/*==============================================================================

FUNCTION      HSU_AL_ECM_CONTROL_OPEN

DESCRIPTION   
Called by the application to allocate and configure the control plane for the 
network device.

DEPENDENCIES  
ecm_init() must have been called for this instance.

RETURN VALUE  
SIO_DONE_S: SIO stream as specified in open_ptr has been successfully opened.
SIO_BADP_S:  Bad Parameter
SIO_UNAVAIL_S:  All available streams have been allocated.
SIO_BIRATE_CHANGE_FAILED_S: Attempt to change bit rate failed 

SIDE EFFECTS  
None.

==============================================================================*/
sio_status_type hsu_al_ecm_control_open
(
 sio_open_type * open_ptr
 );

/*==============================================================================

FUNCTION      HSU_AL_ECM_DATA_OPEN

DESCRIPTION   
Called by the application to allocate and configure the data plane for the 
network function.

DEPENDENCIES  
ecm_init() and hsu_al_ecm_control_open() must have been called for this 
instance.

RETURN VALUE  
SIO_DONE_S: SIO stream as specified in open_ptr has been successfully opened.
SIO_BADP_S:  Bad Parameter
SIO_UNAVAIL_S:  All available streams have been allocated.
SIO_BIRATE_CHANGE_FAILED_S: Attempt to change bit rate failed 

SIDE EFFECTS  
This causes 2 buffers to be given to the USB stack, to be used when OUT 
transfers arrive. 

==============================================================================*/
sio_status_type hsu_al_ecm_data_open
(
 sio_open_type * open_ptr
 );

/*==============================================================================

FUNCTION          HSU_AL_ECM_CONTROL_TRANSMIT

DESCRIPTION
Called by the application, when it wishes to send a message on the control
channel. The message is given as a chain of DSM items. 

The function gathers the DSM chain into a preallocated buffer, and gives it to
the cdc_send_encap_response() function of the USB stack. 

The stack is responsible to generate a "response available" notification, and 
then send the message after the host sends an IN token on the control channel
following the notification. 

Only one control message can be sent per instance at a time. 
Meaning, from the moment cdc_send_encap_response was called, and until the 
hsu_al_ecm_encap_response_complete_cb was called, additional calls to 
cdc_send_encap_response() will fail!
therefore, additional calls to hsu_al_ecm_control_transmit cause the DSM chain
to be queued to the control TX WM Q.

DEPENDENCIES  
ecm_init() and hsu_al_ecm_control_open() must have been called for this 
instance.

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
void hsu_al_ecm_control_transmit
( 
 sio_stream_id_type  stream_id, 
 dsm_item_type *     tx_ptr
 );

/*==============================================================================

FUNCTION                  HSU_AL_ECM_DATA_TRANSMIT 

DESCRIPTION
Called by the application, when it wishes to send data on the bulk end point.
The data (Ethernet frame) is given as a chain of DSM items. 

The function gathers the DSM chain into a preallocated buffer using the DMOV, 
and then gives it to the cdc_write() function of the USB stack. 

DEPENDENCIES
ecm_init(), hsu_al_ecm_control_open() and hsu_al_ecm_data_open() must have 
been called for this instance.

RETURN VALUE
None

SIDE EFFECTS
None.

==============================================================================*/
void hsu_al_ecm_data_transmit
( 
 sio_stream_id_type  stream_id,
 sio_port_id_type    port_id,
 dsm_item_type *     tx_ptr 
 );

/*==============================================================================

FUNCTION      HSU_AL_ECM_IOCTL

DESCRIPTION   
This function is used to control various features of the USB function.  
The behavior of this function depends on the 'cmd' issued.

DEPENDENCIES  
The instance must be in the OPNE state, meaning the ecm_init(), 
hsu_al_ecm_control_open() and hsu_al_ecm_open_data() must have been called for
this instance.
When the command is SIO_IOCTL_ENABLE_DTR_EVENT - 'param' cannot be NULL.

RETURN VALUE  
None.

SIDE EFFECTS  
Depends on the command issued.
==============================================================================*/
void hsu_al_ecm_ioctl
(
 sio_stream_id_type      stream_id,
 sio_port_id_type        port_id,
 sio_ioctl_cmd_type      cmd,
 sio_ioctl_param_type *  param
 );

/*==============================================================================

FUNCTION                HSU_AL_ECM_DATA_FLUSH_TX 

DESCRIPTION
TODO ECM

DEPENDENCIES
TODO ECM

RETURN VALUE
None.

SIDE EFFECTS
TODO ECM

==============================================================================*/
void hsu_al_ecm_data_flush_tx
( 
 sio_stream_id_type  stream_id,   
 sio_port_id_type    port_id,
 void                (*flush_func_ptr)(void)  
 /* Function to call when transmission is complete. */
 );

/*==============================================================================

FUNCTION                    HSU_AL_ECM_CONTROL_CLOSE

DESCRIPTION
TODO ECM

DEPENDENCIES
TODO ECM

RETURN VALUE
None

SIDE EFFECTS
TODO ECM

==============================================================================*/
void hsu_al_ecm_control_close
( 
 sio_stream_id_type  stream_id,
 void                (*close_func_ptr)(void)  
 /* Function to call when transmission is complete.   */
 );

/*==============================================================================

FUNCTION          HSU_AL_ECM_DATA_CLOSE

DESCRIPTION
TODO ECM

DEPENDENCIES
TODO ECM

RETURN VALUE
None.

SIDE EFFECTS
TODO ECM

==============================================================================*/
void hsu_al_ecm_data_close
( 
 sio_stream_id_type stream_id,
 sio_port_id_type   port_id,
 void              (*close_func_ptr)(void)    
 /* Function to call when transmission is complete. */
 );

/*==============================================================================

FUNCTION    HSU_AL_ECM_ENABLE_DL_TLP

DESCRIPTION   
This function is called when (and if) a setup packet is received from the host 
that request to enable DL TLP (this setup packet is received as a result of 
sending an MDLM descriptor by the device)

NOTE:
TLP is enabled for all ecm instances! So when we add support for multiple ecm
instances we need to make sure that the folowing doesn't occure:
- The host enables TLP for ecm instance 1 and leaves TLP disabled for ecm
  instance 2.
- Since TLP will be enabled for all ecm instances, the device will work in
  TLP on both ecm instances but the host will work in TLP mode only with the 
  first ecm instance -> the second ecm instance will be unfunctional!

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
#ifdef FEATURE_HS_USB_TLP
void hsu_al_ecm_enable_dl_tlp(void);

/*==============================================================================

FUNCTION    HSU_AL_ECM_ENABLE_UL_TLP  

DESCRIPTION   
This function is called when (and if) a setup packet is received from the host 
that request to enable UL TLP (this setup packet is received as a result of 
sending an MDLM descriptor by the device)

NOTE:
TLP is enabled for all ecm instances! So when we add support for multiple ecm
instances we need to make sure that the folowing doesn't occure:
- The host enables TLP for ecm instance 1 and leaves TLP disabled for ecm
  instance 2.
- Since TLP will be enabled for all ecm instances, the device will work in
  TLP on both ecm instances but the host will work in TLP mode only with the 
  first ecm instance -> the second ecm instance will be unfunctional!

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS  
None.

==============================================================================*/
void hsu_al_ecm_enable_ul_tlp(void);
#endif /*FEATURE_HS_USB_TLP*/

#ifdef T_REXNT
#error code not present
#endif

/*==============================================================================

FUNCTION          HSU_AL_ECM_HANDLE_SET_CONFIGURATION

DESCRIPTION
This function is called when the enabled callback is received.
This function sets the FD handle according to the current configuration FD.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_al_ecm_handle_set_configuration(void);

#endif /* _HSU_AL_ECM_H_ */

